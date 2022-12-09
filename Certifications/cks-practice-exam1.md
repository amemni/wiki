# CKS practice exam 1

## Questions

https://talend.udemy.com/course/certified-kubernetes-security-specialist-certification/learn/lecture/24141000#overview

## Solutions

### Question 1

1. Create /etc/kubernetes/confcontrol/config.yml:

```bash
apiVersion: apiserver.config.k8s.io/v1
kind: AdmissionConfiguration
plugins:
- name: ImagePolicyWebhook
  configuration:
    imagePolicy:
      kubeConfigFile: /etc/kubernetes/confcontrol/kubeconfig.yml
      allowTTL: 50
      denyTTL: 50
      retryBackoff: 500
      defaultAllow: false
```

2. Create /etc/kubernetes/confcontrol/kubeconfig.yml:

```bash
apiVersion: v1
kind: Config

clusters:
- name: kplabs-imagepolicy-service
  cluster:
    certificate-authority: /etc/kubernetes/pki/ca.crt
    server: https://webhook.kplabs.internal

users:
- name: kplabs-api-server
  user:
    client-certificate: /etc/kubernetes/pki/apiserver.crt
    client-key: /etc/kubernetes/pki/apiserver.key
```

3. Add '--admission-control-config-file=/etc/kubernetes/confcontrol/config.yml' and make sure to add the 'hostPath' volume and mount it under the '/etc/kubernetes/confcontrol/'.

4. When running a pod, you should see this:

```bash
  Warning  FailedCreate  37s                replicaset-controller  Error creating: pods "pod-deploy-f6f74df79-vxxvs" is forbidden: Post "https://webhook.kplabs.internal/?timeout=30s": dial tcp: lookup webhook.kplabs.internal on 8.8.8.8:53: no such host
```

### Question 2

1. Download and load the Apparmor profile:

```bash
root@cks-master:/# cat /etc/apparmor.d/apparmor-profile | apparmor_parser -q
apparmor_parser: Unable to add "k8s-apparmor-example-deny-write".  Profile already exists
root@cks-master:/#
```

2. Create the busybox deployment with the right annotation:

```bash
root@cks-master:/# cat busybox-delpoy.yml
apiVersion: apps/v1
kind: Deployment
metadata:
  labels:
    app: pod-deploy
  name: pod-deploy
spec:
  replicas: 2
  selector:
    matchLabels:
      app: pod-deploy
  strategy: {}
  template:
    metadata:
      annotations:
        container.apparmor.security.beta.kubernetes.io/busybox-container: localhost/k8s-apparmor-example-deny-write
      labels:
        app: pod-deploy
    spec:
      containers:
      - image: busybox
        name: busybox-container
        command: ["sleep"]
        args: ["36000"]
        resources: {}
status: {}
root@cks-master:/#
```

3. Create the deployment, you should see that pods are blocked:

```bash
root@cks-master:/# k get po pod-deploy-f6f74df79-677js
NAME                         READY   STATUS    RESTARTS   AGE
pod-deploy-f6f74df79-677js   0/1     Blocked   0          22h
root@cks-master:/#
```

### Question 3

1. The AuditPolicy file should be like:

```bash
root@cks-master:/practice_exam1# cat /etc/kubernetes/audit-policy.yaml
apiVersion: audit.k8s.io/v1 # This is required.
kind: Policy
rules:
  - level: RequestResponse
    resources:
    - group: ""
      resources: ["namespaces"]
  - level: Request
    resources:
    - group: ""
      resources: ["pods"]
  - level: None
    resources:
    - group: ""
      resources: ["configmaps"]
  - level: Metadata
root@cks-master:/practice_exam1#
```

2. Add these flags in the '/etc/kubernetes/manifests/kube-apiserver.yaml' make sure to add the 'hostPath' volume and mount it under the '/var/log/audit.log'.:

```bash
    - --audit-policy-file=/etc/kubernetes/audit-policy.yaml
    - --audit-log-path=/var/log/audit.log
```

### Question 4

1. Download the 'secrets/yaml' and apply it, then get the content and echo it to '/tmp/passwd':

```bash
root@cks-master:/practice_exam1# k apply -f secrets.yaml
namespace/kplabs-secret created
secret/demo-secret created
root@cks-master:/practice_exam1# k get secret -n kplabs-secret demo-secret -oyaml
apiVersion: v1
data:
  admin: cGFzc3dvcmQ=
kind: Secret
metadata:
  annotations:
    kubectl.kubernetes.io/last-applied-configuration: |
      {"apiVersion":"v1","data":{"admin":"cGFzc3dvcmQ="},"kind":"Secret","metadata":{"annotations":{},"name":"demo-secret","namespace":"kplabs-secret"}}
  creationTimestamp: "2022-06-03T15:49:18Z"
  name: demo-secret
  namespace: kplabs-secret
  resourceVersion: "157947"
  uid: 8b7fdfab-02f9-4aa4-a877-744680941d64
type: Opaque
root@cks-master:/practice_exam1# echo cGFzc3dvcmQ= |base64 --decode
root@cks-master:/practice_exam1#
```

2. Create the new secret:

```bash
root@cks-master:/practice_exam1# k create secret generic mount-secret --from-literal=username=dbadmin --from-literal=password=dbpasswd123
secret/mount-secret created
root@cks-master:/practice_exam1#
```

3. Create a pod and mount the 'mount-secret' secret like this:

```bash
root@cks-master:/practice_exam1# cat secret-pod.yaml
apiVersion: v1
kind: Pod
metadata:
  creationTimestamp: null
  labels:
    run: secret-pod
  name: secret-pod
spec:
  containers:
  - image: ubuntu
    name: secret-pod
    resources: {}
    volumeMounts:
    - name: mount-secret
      mountPath: "/etc/mount-secret"
      readOnly: true
  dnsPolicy: ClusterFirst
  restartPolicy: Always
  volumes:
  - name: mount-secret
    secret:
      secretName: mount-secret
      optional: false
status: {}
root@cks-master:/practice_exam1# k apply -f secret-pod.yaml
pod/secret-pod configured
root@cks-master:/practice_exam1#
```

### Question 5

1. Create the SA, ClusterRole and ClusterRoleBinding resources:

```bash
k create serviceaccount new-sa -n default
k create clusterrole new-sa --verb=list --resource=secrets
k create clusterrolebinding new-sa --serviceaccount=default:new-sa --clusterrole=new-sa
```

2. Create a pod and mount the SA:

```bash
root@cks-master:/practice_exam1# cat nginx-pod.yaml
apiVersion: v1
kind: Pod
metadata:
  creationTimestamp: null
  labels:
    run: nginx-pod
  name: nginx-pod
spec:
  serviceAccountName: new-sa
  containers:
  - image: nginx
    name: nginx-pod
    resources: {}
  dnsPolicy: ClusterFirst
  restartPolicy: Always
status: {}
root@cks-master:/practice_exam1# k apply -f nginx-pod.yaml
pod/nginx-pod configured
root@cks-master:/practice_exam1#
```

3. You can get secrets like this:

```bash
root@nginx-pod:/# export TOKEN=`cat /var/run/secrets/kubernetes.io/serviceaccount/token`
root@nginx-pod:/# curl https://10.96.0.1:443/api/v1/secrets -k --cacert /var/run/secrets/kubernetes.io/serviceaccount/ca.crt --header "Authorization: Bearer ${TOKEN}" -X GET
...
```


### Question 6

1. Pod-3 can be deleted because of this:

```bash
    securityContext:
      privileged: true
```

2. Pod-1 can be kept because of this:

```bash
    securityContext:
      readOnlyRootFilesystem: true
```

3. Pod-2 can be deleted.

### Question 7

1. Do like this for the Nginx image:

```bash
root@cks-master:~/practice_exam1# docker run aquasec/trivy:latest image nginx:1.19.2 |grep -E "HIGH|CRITICAL"
Total: 387 (UNKNOWN: 1, LOW: 147, MEDIUM: 75, HIGH: 124, CRITICAL: 40)
│ curl                │ CVE-2020-8169    │ HIGH     │ 7.64.0-4+deb10u1          │ 7.64.0-4+deb10u2          │ libcurl: partial password leak over DNS on HTTP redirect     │
│ curl                │ CVE-2020-8177    │ HIGH     │ 7.64.0-4+deb10u1          │ 7.64.0-4+deb10u2          │ curl: Incorrect argument check can allow remote servers to   │
│ curl                │ CVE-2020-8286    │ HIGH     │ 7.64.0-4+deb10u1          │ 7.64.0-4+deb10u2          │ curl: Inferior OCSP verification                             │
│ curl                │ CVE-2021-22946   │ HIGH     │ 7.64.0-4+deb10u1          │                           │ curl: Requirement to use TLS not properly enforced for IMAP, │
│ e2fsprogs           │ CVE-2022-1304    │ HIGH     │ 1.44.5-1+deb10u3          │                           │ e2fsprogs: out-of-bounds read/write via crafted filesystem   │
│ gcc-8-base          │ CVE-2018-12886   │ HIGH     │ 8.3.0-6                   │                           │ gcc: spilling of stack protection address in cfgexpand.c and │
│ gcc-8-base          │ CVE-2019-15847   │ HIGH     │ 8.3.0-6                   │                           │ gcc: POWER9 "DARN" RNG intrinsic produces repeated output    │
│ gzip                │ CVE-2022-1271    │ HIGH     │ 1.9-3                     │ 1.9-3+deb10u1             │ gzip: arbitrary-file-write vulnerability                     │
│ libbsd0             │ CVE-2019-20367   │ CRITICAL │ 0.9.1-2                   │ 0.9.1-2+deb10u1           │ nlist.c in libbsd before 0.10.0 has an out-of-bounds read    │
│ libc-bin            │ CVE-2021-35942   │ CRITICAL │ 2.28-10                   │                           │ glibc: Arbitrary read in wordexp()                           │
│ libc-bin            │ CVE-2022-23218   │ CRITICAL │ 2.28-10                   │                           │ glibc: Stack-based buffer overflow in svcunix_create via     │
│ libc-bin            │ CVE-2020-1751    │ HIGH     │ 2.28-10                   │                           │ glibc: array overflow in backtrace functions for powerpc     │
│ libc-bin            │ CVE-2020-1752    │ HIGH     │ 2.28-10                   │                           │ glibc: use-after-free in glob() function when expanding      │
│ libc-bin            │ CVE-2021-3999    │ HIGH     │ 2.28-10                   │                           │ glibc: Off-by-one buffer overflow/underflow in getcwd()      │
│ libc6               │ CVE-2021-33574   │ CRITICAL │                           │                           │ glibc: mq_notify does not handle separately allocated thread │
│ libc6               │ CVE-2021-35942   │ CRITICAL │ 2.28-10                   │                           │ glibc: Arbitrary read in wordexp()                           │
│ libc6               │ CVE-2022-23218   │ CRITICAL │ 2.28-10                   │                           │ glibc: Stack-based buffer overflow in svcunix_create via     │
│ libc6               │ CVE-2020-1751    │ HIGH     │ 2.28-10                   │                           │ glibc: array overflow in backtrace functions for powerpc     │
│ libc6               │ CVE-2020-1752    │ HIGH     │ 2.28-10                   │                           │ glibc: use-after-free in glob() function when expanding      │
│ libc6               │ CVE-2021-3999    │ HIGH     │ 2.28-10                   │                           │ glibc: Off-by-one buffer overflow/underflow in getcwd()      │
│ libcom-err2         │ CVE-2022-1304    │ HIGH     │ 1.44.5-1+deb10u3          │                           │ e2fsprogs: out-of-bounds read/write via crafted filesystem   │
│ libcurl4            │ CVE-2020-8177    │ HIGH     │ 7.64.0-4+deb10u1          │ 7.64.0-4+deb10u2          │ curl: Incorrect argument check can allow remote servers to   │
│ libcurl4            │ CVE-2020-8286    │ HIGH     │ 7.64.0-4+deb10u1          │ 7.64.0-4+deb10u2          │ curl: Inferior OCSP verification                             │
│ libcurl4            │ CVE-2021-22946   │ HIGH     │ 7.64.0-4+deb10u1          │                           │ curl: Requirement to use TLS not properly enforced for IMAP, │
│ libdb5.3            │ CVE-2019-8457    │ CRITICAL │ 5.3.28+dfsg1-0.5          │                           │ sqlite: heap out-of-bound read in function rtreenode()       │
│ libexpat1           │ CVE-2022-25235   │ CRITICAL │ 2.2.6-2+deb10u1           │ 2.2.6-2+deb10u3           │ expat: Malformed 2- and 3-byte UTF-8 sequences can lead to   │
│ libexpat1           │ CVE-2022-25315   │ CRITICAL │ 2.2.6-2+deb10u1           │ 2.2.6-2+deb10u3           │ expat: Integer overflow in storeRawNames()                   │
│ libexpat1           │ CVE-2021-45960   │ HIGH     │ 2.2.6-2+deb10u1           │ 2.2.6-2+deb10u2           │ expat: Large number of prefixed XML attributes on a single   │
│ libexpat1           │ CVE-2021-46143   │ HIGH     │ 2.2.6-2+deb10u1           │ 2.2.6-2+deb10u2           │ expat: Integer overflow in doProlog in xmlparse.c            │
│ libext2fs2          │ CVE-2022-1304    │ HIGH     │ 1.44.5-1+deb10u3          │                           │ e2fsprogs: out-of-bounds read/write via crafted filesystem   │
│ libfreetype6        │ CVE-2022-27404   │ CRITICAL │ 2.9.1-3+deb10u1           │                           │ FreeType: Buffer Overflow                                    │
│                     │ CVE-2022-27405   │ HIGH     │                           │                           │ FreeType: Segementation Fault                                │
│ libgcc1             │ CVE-2018-12886   │ HIGH     │ 8.3.0-6                   │                           │ gcc: spilling of stack protection address in cfgexpand.c and │
│ libgcc1             │ CVE-2019-15847   │ HIGH     │ 8.3.0-6                   │                           │ gcc: POWER9 "DARN" RNG intrinsic produces repeated output    │
│ libgcrypt20         │ CVE-2021-33560   │ HIGH     │ 1.8.4-5                   │                           │ libgcrypt: mishandles ElGamal encryption because it lacks    │
│ libgd3              │ CVE-2017-6363    │ HIGH     │ 2.2.5-5.2                 │                           │ ** DISPUTED ** In the GD Graphics Library (aka LibGD)        │
│ libgmp10            │ CVE-2021-43618   │ HIGH     │ 2:6.1.2+dfsg-4            │ 2:6.1.2+dfsg-4+deb10u1    │ gmp: Integer overflow and resultant buffer overflow via      │
│ libgnutls30         │ CVE-2021-20231   │ CRITICAL │ 3.6.7-4+deb10u5           │ 3.6.7-4+deb10u7           │ gnutls: Use after free in client key_share extension         │
│ libgnutls30         │ CVE-2021-20232   │ CRITICAL │ 3.6.7-4+deb10u5           │ 3.6.7-4+deb10u7           │ gnutls: Use after free in client_send_params in              │
│                     │ CVE-2020-24659   │ HIGH     │                           │                           │ gnutls: Heap buffer overflow in handshake with               │
│ libgssapi-krb5-2    │ CVE-2020-28196   │ HIGH     │ 1.17-3                    │ 1.17-3+deb10u1            │ krb5: unbounded recursion via an ASN.1-encoded Kerberos      │
│ libhogweed4         │ CVE-2021-20305   │ HIGH     │ 3.4.1-1                   │ 3.4.1-1+deb10u1           │ nettle: Out of bounds memory access in signature             │
│ libidn2-0           │ CVE-2019-12290   │ HIGH     │ 2.0.5-1+deb10u1           │                           │ GNU libidn2 before 2.2.0 fails to perform the roundtrip      │
│ libjpeg62-turbo     │ CVE-2020-13790   │ HIGH     │ 1:1.5.2-2                 │ 1:1.5.2-2+deb10u1         │ libjpeg-turbo: heap-based buffer over-read in get_rgb_row()  │
│ libk5crypto3        │ CVE-2020-28196   │ HIGH     │ 1.17-3                    │ 1.17-3+deb10u1            │ krb5: unbounded recursion via an ASN.1-encoded Kerberos      │
│ libkrb5-3           │ CVE-2020-28196   │ HIGH     │                           │ 1.17-3+deb10u1            │ krb5: unbounded recursion via an ASN.1-encoded Kerberos      │
│ libkrb5support0     │ CVE-2020-28196   │ HIGH     │                           │ 1.17-3+deb10u1            │ krb5: unbounded recursion via an ASN.1-encoded Kerberos      │
│ libldap-2.4-2       │ CVE-2022-29155   │ CRITICAL │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u7     │ openldap: OpenLDAP SQL injection                             │
│ libldap-2.4-2       │ CVE-2020-25692   │ HIGH     │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u3     │ openldap: NULL pointer dereference for unauthenticated       │
│ libldap-2.4-2       │ CVE-2020-36223   │ HIGH     │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u5     │ openldap: Out-of-bounds read in Values Return Filter         │
│ libldap-2.4-2       │ CVE-2020-36226   │ HIGH     │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u5     │ openldap: Denial of service via length miscalculation in     │
│ libldap-2.4-2       │ CVE-2020-36229   │ HIGH     │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u5     │ openldap: Type confusion in ad_keystring in ad.c             │
│ libldap-2.4-2       │ CVE-2021-27212   │ HIGH     │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u6     │ openldap: Assertion failure in slapd in the                  │
│ libldap-common      │ CVE-2022-29155   │ CRITICAL │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u7     │ openldap: OpenLDAP SQL injection                             │
│ libldap-common      │ CVE-2020-25692   │ HIGH     │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u3     │ openldap: NULL pointer dereference for unauthenticated       │
│ libldap-common      │ CVE-2020-36223   │ HIGH     │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u5     │ openldap: Out-of-bounds read in Values Return Filter         │
│ libldap-common      │ CVE-2020-36226   │ HIGH     │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u5     │ openldap: Denial of service via length miscalculation in     │
│ libldap-common      │ CVE-2020-36229   │ HIGH     │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u5     │ openldap: Type confusion in ad_keystring in ad.c             │
│ libldap-common      │ CVE-2021-27212   │ HIGH     │ 2.4.47+dfsg-3+deb10u2     │ 2.4.47+dfsg-3+deb10u6     │ openldap: Assertion failure in slapd in the                  │
│ liblz4-1            │ CVE-2021-3520    │ CRITICAL │ 1.8.3-1                   │ 1.8.3-1+deb10u1           │ lz4: memory corruption due to an integer overflow bug caused │
│ liblzma5            │ CVE-2022-1271    │ HIGH     │ 5.2.4-1                   │ 5.2.4-1+deb10u1           │ gzip: arbitrary-file-write vulnerability                     │
│ libncursesw6        │ CVE-2022-29458   │ HIGH     │ 6.1+20181013-2+deb10u2    │                           │ ncurses: segfaulting OOB read                                │
│ libnettle6          │ CVE-2021-20305   │ HIGH     │ 3.4.1-1                   │ 3.4.1-1+deb10u1           │ nettle: Out of bounds memory access in signature             │
│ libnghttp2-14       │ CVE-2020-11080   │ HIGH     │ 1.36.0-2+deb10u1          │                           │ nghttp2: overly large SETTINGS frames can lead to DoS        │
│ libp11-kit0         │ CVE-2020-29361   │ HIGH     │ 0.23.15-2                 │ 0.23.15-2+deb10u1         │ p11-kit: integer overflow when allocating memory for arrays  │
│ libp11-kit0         │ CVE-2020-29363   │ HIGH     │ 0.23.15-2                 │ 0.23.15-2+deb10u1         │ p11-kit: out-of-bounds write in                              │
│ libsasl2-2          │ CVE-2022-24407   │ HIGH     │ 2.1.27+dfsg-1+deb10u1     │ 2.1.27+dfsg-1+deb10u2     │ cyrus-sasl: failure to properly escape SQL input allows an   │
│ libss2              │ CVE-2022-1304    │ HIGH     │ 1.44.5-1+deb10u3          │                           │ e2fsprogs: out-of-bounds read/write via crafted filesystem   │
│ libssh2-1           │ CVE-2019-13115   │ HIGH     │ 1.8.0-2.1                 │                           │ libssh2: integer overflow in                                 │
│ libssl1.1           │ CVE-2021-3711    │ CRITICAL │ 1.1.1d-0+deb10u3          │ 1.1.1d-0+deb10u7          │ openssl: SM2 Decryption Buffer Overflow                      │
│                     │ CVE-2021-23840   │ HIGH     │                           │ 1.1.1d-0+deb10u5          │ openssl: integer overflow in CipherUpdate                    │
│ libssl1.1           │ CVE-2022-0778    │ HIGH     │ 1.1.1d-0+deb10u3          │ 1.1.1d-0+deb10u8          │ openssl: Infinite loop in BN_mod_sqrt() reachable when       │
│ libstdc++6          │ CVE-2018-12886   │ HIGH     │ 8.3.0-6                   │                           │ gcc: spilling of stack protection address in cfgexpand.c and │
│ libstdc++6          │ CVE-2019-15847   │ HIGH     │ 8.3.0-6                   │                           │ gcc: POWER9 "DARN" RNG intrinsic produces repeated output    │
│ libsystemd0         │ CVE-2019-3843    │ HIGH     │ 241-7~deb10u4             │                           │ systemd: services with DynamicUser can create SUID/SGID      │
│ libtiff5            │ CVE-2020-35523   │ HIGH     │ 4.1.0+git191117-2~deb10u1 │ 4.1.0+git191117-2~deb10u2 │ libtiff: Integer overflow in tif_getimage.c                  │
│ libtinfo6           │ CVE-2022-29458   │ HIGH     │ 6.1+20181013-2+deb10u2    │                           │ ncurses: segfaulting OOB read                                │
│ libudev1            │ CVE-2019-3843    │ HIGH     │ 241-7~deb10u4             │                           │ systemd: services with DynamicUser can create SUID/SGID      │
│ libwebp6            │ CVE-2018-25009   │ CRITICAL │ 0.6.1-2                   │ 0.6.1-2+deb10u1           │ libwebp: out-of-bounds read in WebPMuxCreateInternal         │
│ libwebp6            │ CVE-2020-36328   │ CRITICAL │ 0.6.1-2                   │ 0.6.1-2+deb10u1           │ libwebp: heap-based buffer overflow in WebPDecode*Into       │
│ libwebp6            │ CVE-2020-36329   │ CRITICAL │ 0.6.1-2                   │ 0.6.1-2+deb10u1           │ libwebp: use-after-free in EmitFancyRGB() in dec/io_dec.c    │
│ libwebp6            │ CVE-2020-36330   │ CRITICAL │ 0.6.1-2                   │ 0.6.1-2+deb10u1           │ libwebp: out-of-bounds read in ChunkVerifyAndAssign() in     │
│ libwebp6            │ CVE-2020-36332   │ HIGH     │ 0.6.1-2                   │ 0.6.1-2+deb10u1           │ libwebp: excessive memory allocation when reading a file     │
│ libx11-6            │ CVE-2021-31535   │ CRITICAL │ 2:1.6.7-1                 │ 2:1.6.7-1+deb10u2         │ libX11: missing request length checks                        │
│ libx11-6            │ CVE-2020-14363   │ HIGH     │ 2:1.6.7-1                 │ 2:1.6.7-1+deb10u1         │ libX11: integer overflow leads to double free in locale      │
│ libx11-data         │ CVE-2021-31535   │ CRITICAL │                           │ 2:1.6.7-1+deb10u2         │ libX11: missing request length checks                        │
│ libx11-data         │ CVE-2020-14363   │ HIGH     │ 2:1.6.7-1                 │ 2:1.6.7-1+deb10u1         │ libX11: integer overflow leads to double free in locale      │
│ libxml2             │ CVE-2017-16932   │ HIGH     │ 2.9.4+dfsg1-7             │                           │ libxml2: Infinite recursion in parameter entities            │
│ libxml2             │ CVE-2019-19956   │ HIGH     │ 2.9.4+dfsg1-7             │ 2.9.4+dfsg1-7+deb10u1     │ libxml2: memory leak in xmlParseBalancedChunkMemoryRecover   │
│ libxml2             │ CVE-2019-20388   │ HIGH     │ 2.9.4+dfsg1-7             │ 2.9.4+dfsg1-7+deb10u1     │ libxml2: memory leak in xmlSchemaPreRun in xmlschemas.c      │
│ libxml2             │ CVE-2020-7595    │ HIGH     │ 2.9.4+dfsg1-7             │ 2.9.4+dfsg1-7+deb10u1     │ libxml2: infinite loop in xmlStringLenDecodeEntities in some │
│ libxml2             │ CVE-2022-23308   │ HIGH     │ 2.9.4+dfsg1-7             │ 2.9.4+dfsg1-7+deb10u3     │ libxml2: Use-after-free of ID and IDREF attributes           │
│ ncurses-base        │ CVE-2022-29458   │ HIGH     │ 6.1+20181013-2+deb10u2    │                           │ ncurses: segfaulting OOB read                                │
│ ncurses-bin         │ CVE-2022-29458   │ HIGH     │ 6.1+20181013-2+deb10u2    │                           │ ncurses: segfaulting OOB read                                │
│ nginx               │ CVE-2021-3618    │ HIGH     │ 1.19.2-1~buster           │                           │ ALPACA: Application Layer Protocol Confusion - Analyzing and │
│ openssl             │ CVE-2021-3711    │ CRITICAL │ 1.1.1d-0+deb10u3          │ 1.1.1d-0+deb10u7          │ openssl: SM2 Decryption Buffer Overflow                      │
│                     │ CVE-2021-23840   │ HIGH     │                           │ 1.1.1d-0+deb10u5          │ openssl: integer overflow in CipherUpdate                    │
│ openssl             │ CVE-2022-0778    │ HIGH     │ 1.1.1d-0+deb10u3          │ 1.1.1d-0+deb10u8          │ openssl: Infinite loop in BN_mod_sqrt() reachable when       │
│ perl-base           │ CVE-2020-16156   │ HIGH     │ 5.28.1-6+deb10u1          │                           │ perl-CPAN: Bypass of verification of signatures in CHECKSUMS │
│ zlib1g              │ CVE-2018-25032   │ HIGH     │ 1:1.2.11.dfsg-1           │ 1:1.2.11.dfsg-1+deb10u1   │ zlib: A flaw found in zlib when compressing (not             │
root@cks-master:~/practice_exam1#
```

2. Do like this for the Kube-apiserver image:

```bash
root@cks-master:~/practice_exam1# docker run aquasec/trivy:latest image k8s.gcr.io/kube-apiserver@sha256:ddf5bf7196eb534271f9e5d403f4da19838d5610bb5ca191001bde5f32b5492e |grep -E "HIGH|CRITICAL"
Total: 0 (UNKNOWN: 0, LOW: 0, MEDIUM: 0, HIGH: 0, CRITICAL: 0)
root@cks-master:~/practice_exam1#
```

### Question 8

1. We can add this rule as an example:

```bash
root@cks-master:/etc/falco# tail -5 /etc/falco/falco_rules.local.yaml
- rule: List new and spawned processes
  desc: "List new and spawned processes"
  condition: spawned_process
  output: "[time=%evt.time][process-name=%proc.name][uid=%user.loginuid]"
  priority: NOTICE
root@cks-master:/etc/falco# journalctl -fu falco
-- Logs begin at Sun 2022-03-20 03:48:33 PDT. --
Jun 03 14:29:49 cks-master falco[10894]: Loading rules from file /etc/falco/falco_rules.local.yaml:
Jun 03 14:29:49 cks-master falco[10894]: Fri Jun  3 14:29:49 2022: Loading rules from file /etc/falco/falco_rules.local.yaml:
Jun 03 14:29:49 cks-master falco[10894]: Starting internal webserver, listening on port 8765
Jun 03 14:29:49 cks-master falco[10894]: Fri Jun  3 14:29:49 2022: Starting internal webserver, listening on port 8765
Jun 03 14:29:51 cks-master falco[10894]: 14:29:51.902097762: Notice [time=14:29:51.902097762][process-name=clear][uid=1000]
Jun 03 14:29:54 cks-master falco[10894]: 14:29:54.279511418: Notice [time=14:29:54.279511418][process-name=iptables][uid=-1]
Jun 03 14:29:56 cks-master falco[10894]: 14:29:56.503592969: Notice [time=14:29:56.503592969][process-name=tail][uid=1000]
Jun 03 14:30:02 cks-master falco[10894]: 14:30:02.623169391: Notice [time=14:30:02.623169391][process-name=iptables][uid=-1]
Jun 03 14:30:02 cks-master falco[10894]: 14:30:02.724588825: Notice [time=14:30:02.724588825][process-name=ip6tables][uid=-1]
Jun 03 14:30:04 cks-master falco[10894]: 14:30:04.281498633: Notice [time=14:30:04.281498633][process-name=iptables][uid=-1]
Jun 03 14:30:05 cks-master falco[10894]: 14:30:04.984330178: Notice [time=14:30:04.984330178][process-name=journalctl][uid=1000]
^C
root@cks-master:/etc/falco#
```

### Question 9

1. The PodSecurityPolicy and ClusterRole can be like this:

```bash
root@cks-master:~/practice_exam1# cat psp-restrictive.yaml
apiVersion: policy/v1beta1
kind: PodSecurityPolicy
metadata:
  name: psp-restrictive
spec:
  privileged: false
  seLinux:
    rule: RunAsAny
  supplementalGroups:
    rule: RunAsAny
  runAsUser:
    rule: RunAsAny
  fsGroup:
    rule: RunAsAny
  volumes:
  - '*'
---
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRole
metadata:
  name: psp-restrictive
rules:
- apiGroups: ['policy']
  resources: ['podsecuritypolicies']
  verbs:     ['use']
  resourceNames:
  - psp-restrictive
root@cks-master:~/practice_exam1# k apply -f psp-restrictive.yaml
Warning: policy/v1beta1 PodSecurityPolicy is deprecated in v1.21+, unavailable in v1.25+
podsecuritypolicy.policy/psp-restrictive configured
clusterrole.rbac.authorization.k8s.io/psp-restrictive unchanged
root@cks-master:~/practice_exam1#
```

2. The ClusterRoleBinding can be created like this:

```bash
root@cks-master:~/practice_exam1# k create clusterrolebinding psp-restrictive --clusterrole=psp-restrictive --serviceaccount=test:default
clusterrolebinding.rbac.authorization.k8s.io/psp-restrictive created
root@cks-master:~/practice_exam1#
```

### Question 10

1. Set `--authorization-mode=Node,RBAC` in the kube-apiserver.yaml to enable Node and RBAC authorization modes.
2. Set `-enable-admission-plugins=..,AlwaysPullImage` in the kube-apiserver.yaml to enable the AlwaysPullImage admission plugin.
3. In the '/var/lib/kubelet/config.yaml', set 'authentiation.anonymous.enabled: false', or add '--anonymous-auth=false' to the KUBELET_CONFIG_ARGS.
4. Set `-client-cert-auth=true` in the etcd.yaml to enable client cert authentication.

### Question 11

1. The NS and NetworkPolicy can be created as such:

```bash
apiVersion: v1
kind: Namespace
metadata:
  labels:
    kubernetes.io/metadata.name: custom-namespace
  name: custom-namespace
---
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: my-network-policy
  namespace: custom-namespace
spec:
  podSelector: {}
  policyTypes:
    - Ingress
  ingress:
    - from:
        - podSelector: {}
      ports:
        - protocol: TCP
          port: 80
```

### Question 12

1. The NetworkPolicy can be created as such:

```bash
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: red-blue
  namespace: default
spec:
  podSelector:
    matchLabels:
      color: blue
  policyTypes:
    - Ingress
  ingress:
    - from:
        - podSelector:
            matchLabels:
              color: red
      ports:
        - protocol: TCP
          port: 80
```

### Question 13

1. The NS and NetworkPolicy can be created as such:

```bash
apiVersion: v1
kind: Namespace
metadata:
  labels:
    kubernetes.io/metadata.name: color-namespace
  name: color-namespace
---
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: red-bright
  namespace: color-namespace
spec:
  podSelector: {}
  policyTypes:
    - Ingress
  ingress:
    - from:
        - podSelector:
            matchLabels:
              color: red
        - namespaceSelector:
            matchLabels:
              color: bright
```

### Question 14

1. The NetworkPolicy can be created as such:

```bash
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: test-network-policy
  namespace: test-namespace
spec:
  podSelector: {}
  policyTypes:
    - Egress
  egress:
    - to:
        - podSelector:
            matchLabels:
              color: yellow
      ports:
        - protocol: TCP
          port: 80
```

### Question 15

1. Create the RuntimeClass like this:

```bash
root@cks-master:~/practice_exam1# cat gvisor.yaml
apiVersion: node.k8s.io/v1
kind: RuntimeClass
metadata:
  name: gvisor-class
handler: runsc
root@cks-master:~/practice_exam1# k apply -f gvisor.yaml
runtimeclass.node.k8s.io/gvisor-class created
root@cks-master:~/practice_exam1#
```

2. Create the deploymment and assign the RuntimeClass to it like this:

```bash
oot@cks-master:~/practice_exam1# cat gvisor-deploy.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  labels:
    app: gvisor-deploy
  name: gvisor-deploy
spec:
  replicas: 3
  selector:
    matchLabels:
      app: gvisor-deploy
  strategy: {}
  template:
    metadata:
      labels:
        app: gvisor-deploy
    spec:
      runtimeClassName: gvisor-class
      containers:
      - image: nginx
        name: nginx
        resources: {}
status: {}
root@cks-master:~/practice_exam1# k apply -f gvisor-deploy.yaml
deployment.apps/gvisor-deploy created
root@cks-master:~/practice_exam1#
oot@cks-master:~/practice_exam1# cat gvisor-deploy.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  labels:
    app: gvisor-deploy
  name: gvisor-deploy
spec:
  replicas: 3
  selector:
    matchLabels:
      app: gvisor-deploy
  strategy: {}
  template:
    metadata:
      labels:
        app: gvisor-deploy
    spec:
      runtimeClassName: gvisor-class
      containers:
      - image: nginx
        name: nginx
        resources: {}
status: {}
root@cks-master:~/practice_exam1# k apply -f gvisor-deploy.yaml
deployment.apps/gvisor-deploy created
root@cks-master:~/practice_exam1#
```

### Question 16

1. From the 'secure-pod.yaml', remove 'securityContext.privileged: true'.
2. From the 'secure-Dockerfile.yaml', change to the 'USER root' to 'USER app-user' and change the 'ubuntu:latest' to 'ubuntu:16.04'.
