# CKS: Certified Kubernetes Security

Following this Udemy course: https://talend.udemy.com/course/certified-kubernetes-security-specialist/learn/lecture/23174064#overview

# Tips

## How to see into VMWare guests from Windowns 10 ?

- https://slmeng.medium.com/how-to-ssh-into-vmware-player-virtual-box-guest-linux-os-remotely-host-os-is-window-10-40cb348c996f

- This is how I ssh to CKS-Mater: ('cks-master' is an alias to Localhost)
  ```bash
  ssh -p 2244 amemni@cks-master
  ```

- This is how I ssh to CKS-Worker: ('cks-worker' is an alias to Localhost)
  ```bash
  ssh -p 2245 amemni@cks-worker
  ```

# Resume

## Section 4: Foundations - K8s secure architecture

- All You Need to Know About Certificates in Kubernetes: https://www.youtube.com/watch?v=gXz4cq3PKdg
- Kubernetes Components: https://kubernetes.io/docs/concepts/overview/components

- PKI certificates and requirements: https://kubernetes.io/docs/setup/best-practices/certificates

## Section 5: Foundations - Containers under the hood

- Container tools introduction:
  - Docker: container runtime + tool for managing containers and images
  - Containerd: container runtime
  - Crictl: CLI for CRI-compatible container runtimes
  - Podman: tool for manging containers and images

- What have namespaces done for you lately: https://www.youtube.com/watch?v=MHv6cWjvQjM&ab_channel=Docker

## Section 6: Cluster setup - Network policies

- Documentation: https://kubernetes.io/docs/concepts/services-networking/network-policies/

## Section 7: Cluster setup - GUI elements

- K8s dashboard arguments: https://github.com/kubernetes/dashboard/blob/master/docs/common/dashboard-arguments.md

- K8s dashboard access control: https://github.com/kubernetes/dashboard/tree/master/docs/user/access-control

## Section 7: Cluster setup - Secure ingress

- Useful commands:
```bash
  179  k expose pod pod1 --port 80 --name service1
  191  openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
  192  k create secret tls secure-ingress --cert=cert.pem --key=key.pem
  204  curl https://secure-ingress.com:31079/service1 -kv --resolve secure-ingress.com:31079:192.168.13.138
```

## Section 8: Cluster setup - Node metadata protection

- Accessing node metadata in GCP:
```bash
curl "http://metadata.google.internal/computeMetadata/v1/instance/disks/" -H "Metadata-Flavor: Google"
```

## Section 10: Cluster setup - CIS benchmarks

- CIS: Center for Internet Security

- CIS Kubernetes benchmark v1.6.0: [CIS Kubernetes Benchmark V1.23](./docs/CIS Kubernetes Benchmark V1.23 -PDF.pdf)

- Running Kubebench:
```bash
docker run --pid=host -v /etc:/etc:ro -v /var:/var:ro -t aquasec/kube-bench:latest master --version 1.23
docker run --pid=host -v /etc:/etc:ro -v /var:/var:ro -t aquasec/kube-bench:latest node --version 1.23
```

- GCP CIS benchmarks: https://cloud.google.com/kubernetes-engine/docs/concepts/cis-benchmarks

- Consistent security controls through CIS benchmarks: https://www.youtube.com/watch?v=53-v3stlnCo&ab_channel=Bristech
- Docker bench: https://github.com/docker/docker-bench-security

## Section 11: Cluster setup - Verify platform binaries

- Download platform binaries from: https://github.com/kubernetes/kubernetes/blob/master/CHANGELOG/CHANGELOG-1.24.md#server-binaries

- Trick: accessing the root filesystem of a container:
```bash
root@cks-master:~# ps -aux |grep kube-apiserver
root       2464  4.4  7.5 1042176 301012 ?      Ssl  10:29   0:35 kube-apiserver --advertise-address=192.168.13.138 --allow-privileged=true --authorization-mode=Node,RBAC
...
root      14016  0.0  0.0  14432  1152 pts/0    S+   10:42   0:00 grep --color=auto kube-apiserver
root@cks-master:~# ls /proc/2464/root/
bin   dev  go-runner  lib   root  sbin  tmp  var
boot  etc  home       proc  run   sys   usr
root@cks-master:~# find /proc/2464/root/ |grep kube-api
/proc/2464/root/usr/local/bin/kube-apiserver
root@cks-master:~# sha512sum /proc/2464/root/usr/local/bin/kube-apiserver
21c301f6af6df23555bbb699863c770ae94bfd7342c975e87282c287a21302371711327e7f0ec30e8ac43ad86f3e23b8e67e30d63c6095fbad87793f96aad0a5  /proc/2464/root/usr/local/bin/kube-apiserver
root@cks-master:~#
```

## Section 12: Cluster hardening - RBAC

- List namespaced and non-namespaced API resources:
```bash
kubectl api-resources --namespaced=true
kubectl api-resources --namespaced=false
```
- Create Roles and Rolebindings:
```bash
root@cks-master:~# k create clusterrole deploy-deleter --verb delete --resource deployments
clusterrole.rbac.authorization.k8s.io/deploy-deleter created
root@cks-master:~# k create clusterrolebinding deploy-deleter --clusterrole deploy-deleter --user jane
clusterrolebinding.rbac.authorization.k8s.io/deploy-deleter created
root@cks-master:~# k -n red create rolebinding deploy-deleter --clus
terrole deploy-deleter --user jim
rolebinding.rbac.authorization.k8s.io/deploy-deleter created
root@cks-master:~#
```

- Confirm permissions:
```bash
root@cks-master:~# k  auth can-i --as jim delete deployments
no
root@cks-master:~# k  auth can-i --as jane delete deployments
yes
root@cks-master:~# k  auth can-i --as jane delete deployments -A
yes
...
controlplane $ k auth can-i get deployments --as system:serviceaccount:ns1:pipeline -n ns1
yes
controlplane $
```

- No K8s user resources. A user is someone with a certificate and a key. Cloud provider IDP services have to issue certificates, store and managed certificates and keys, then use these to communicate with K8s APIs:
  - User client certificate has to be signed by the cluster's CA
  - Username should be under common name /CN=jane

- Approve a CSR for user authentication:
```bash
root@cks-master:~# k certificate approve jane
certificatesigningrequest.certificates.k8s.io/jane approved
root@cks-master:~# k describe csr jane
...
Status:              Approved,Issued
...
root@cks-master:~#
```

- Configure and use a new user context:
```bash
root@cks-master:~# k config set-credentials jane --client-key jane.key --client-certificate jane.crt --embed-certs
User "jane" set.
root@cks-master:~# k config set-context jane --user=jane --cluster=k
uberntes
Context "jane" created.
root@cks-master:~#
...
root@cks-master:~# k config use-context jane
Switched to context "jane".
root@cks-master:~#
```

- Using RBAC authorization: https://kubernetes.io/docs/reference/access-authn-authz/rbac/

## Section 13: Cluster hardening - Exercise caution in using service accounts

- Get service account token from inside pod and use it to talk to the API server:
```bash
root@cks-master:~# k exec -it po/accessor -- bash
root@accessor:/# mount |grep sec
tmpfs on /run/secrets/kubernetes.io/serviceaccount type tmpfs (ro,relatime,size=3892284k)
root@accessor:/# cat /run/secrets/kubernetes.io/serviceaccount/
..2022_04_03_12_28_31.226667515/ namespace
..data/                          token
ca.crt
root@accessor:/# cat /run/secrets/kubernetes.io/serviceaccount/token
eyJhbGciOiJSUzI1NiIsImtpZCI6Im44WlNDUVRUamVvaVg2bDR2b2ZwUW11VzlmUDNTYjFUYTZWV1E2TXdzSUkifQ.eyJhdWQiOlsiaHR0cHM6Ly9rdWJlcm5ldGVzLmRlZmF1bHQuc3ZjLmNsdXN0ZXIubG9jYWwiXSwiZXhwIjoxNjgwNTI0OTExLCJpYXQiOjE2NDg5ODg5MTEsImlzcyI6Imh0dHBzOi8va3ViZXJuZXRlcy5kZWZhdWx0LnN2Yy5jbHVzdGVyLmxvY2FsIiwia3ViZXJuZXRlcy5pbyI6eyJuYW1lc3BhY2UiOiJkZWZhdWx0IiwicG9kIjp7Im5hbWUiOiJhY2Nlc3NvciIsInVpZCI6ImRiNTlkZTYzLTcxYjUtNDM0Ny1iNjBhLTAzNWEzOTY3ZWJjNCJ9LCJzZXJ2aWNlYWNjb3VudCI6eyJuYW1lIjoiYWNjZXNzb3IiLCJ1aWQiOiIzODA5ZWI2OS0xYzllLTQ0NTItODkxNy1lYjExNjdjNmQ0NjIifSwid2FybmFmdGVyIjoxNjQ4OTkyNTE4fSwibmJmIjoxNjQ4OTg4OTExLCJzdWIiOiJzeXN0ZW06c2VydmljZWFjY291bnQ6ZGVmYXVsdDphY2Nlc3NvciJ9.kOpW_Gt5ng9-3p4y5o-CivaBU1LuZ8CDoF7TOvRVaHHV77-bO2catkS7QnZ8wytkFDDrkNyqV-CfwInrijH4U0XF7QVQ7sXaplG8ynD1K5iWdF7lCDsot1j_hEtav2AbawsVx6jHKfv8ddyJrH5bFPJ46-GQhio88TE49a2_EyZNqRpAEjroot@accessor:/# curl https://kubernetes -k -H "Authorization: Bearer eyJhbGciOiJSUzI1NiIsImtpZCI6Im44WlNDUVRUamVvaVg2bDR2b2ZwUW11VzlmUDNTYjFUYTZWV1E2TXdzSUkifQ.eyJhdWQiOlsiaHR0cHM6Ly9rdWJlcm5ldGVzLmRlZmF1bHQuc3ZjLmNsdXN0ZXIubG9jYWwiXSwiZXhwIjoxNjgwNTI0OTExLCJpYXQiOjE2NDg5ODg5MTEsImlzcyI6Imh0dHBzOi8va3ViZXJuZXRlcy5kZWZhdWx0LnN2Yy5jbHVzdGVyLmxvY2FsIiwia3ViZXJuZXRlcy5pbyI6eyJuYW1lc3BhY2UiOiJkZWZhdWx0IiwicG9kIjp7Im5hbWUiOiJhY2Nlc3NvciIsInVpZCI6ImRiNTlkZTYzLTcxYjUtNDM0Ny1iNjBhLTAzNWEzOTY3ZWJjNCJ9LCJzZXJ2aWNlYWNjb3VudCI6eyJuYW1lIjoiYWNjZXNzb3IiLCJ1aWQiOiIzODA5ZWI2OS0xYzllLTQ0NTItODkxNy1lYjExNjdjNmQ0NjIifSwid2FybmFmdGVyIjoxNjQ4OTkyNTE4fSwibmJmIjoxNjQ4OTg4OTExLCJzdWIiOiJzeXN0ZW06c2VydmljZWFjY291bnQ6ZGVmYXVsdDphY2Nlc3NvciJ9.kOpW_Gt5ng9-3p4y5o-CivaBU1LuZ8CDoF7TOvRVaHHV77-bO2catkS7QnZ8wytkFDDrkNyqV-CfwInrijH4U0XF7QVQ7sXaplG8ynD1K5iWdF7lCDsot1j_hEtav2AbawsVx6jHKfv8ddyJrH5bFPJ46-GQhio88TE49a2_EyZNqRpAEjdDKaZnpVqQxqhI7zvh0t3Frl5uGCCXH5dHZGCSQQY1RBT5a6xy8tPrpJcQVVFiXcPANfjYqsP_ycRuXYugYXNCzewJG89XlynWOjdtcMs7r-yMh7CNgCkLEVRrv4h88kWeHJzRMgQY8TaBVhI6V1OYGEF5ZG9Up1U5uQ"
{
  "kind": "Status",
  "apiVersion": "v1",
  "metadata": {},
  "status": "Failure",
  "message": "forbidden: User \"system:serviceaccount:default:accessor\" cannot get path \"/\"",
  "reason": "Forbidden",
  "details": {},
  "code": 403
root@accessor:/#
```

- Disable auto-mount default service accounts in pods:

```bash
...
spec:
  automountServiceAccountToken: false
  serviceAccount: accessor
...
```

- Managing Service Accounts: https://kubernetes.io/docs/reference/access-authn-authz/service-accounts-admin

- Configure Service Accounts for Pods: https://kubernetes.io/docs/tasks/configure-pod-container/configure-service-account

## Section 14: Cluster hardening - Restrict API access

- Restrictions:

  1. Don't allow anonymous access
  2. Close insecure port
  3. Don't expose ApiServer to the outside
  4. Restrict access from Nodes to API (NodeRestriction)
  5. Prevent unauthorized access (RBAC)
  6. Prevent pods from accessing API
  7. Apiserver port behind firewall / allowed IP ranges (cloud provider)

- Use `--anonymous-auth=false` to disable anonymous access to the API server

- Use `--insecure-port=8080` to enable insecure access (bypass authentication + authorization, keeps admission controllers)

- Perform manual API requests:

```bash
root@cks-master:~# k config view --raw
... # get CA, Cert and Key of K8s-admin
root@cks-master:~# curl https://192.168.13.138:6443 --cacert ca --cert crt --key key -s |head
{
  "paths": [
    "/.well-known/openid-configuration",
    "/api",
    "/api/v1",
    "/apis",
    "/apis/",
    "/apis/admissionregistration.k8s.io",
    "/apis/admissionregistration.k8s.io/v1",
    "/apis/apiextensions.k8s.io",
root@cks-master:~#
```

- Switch the `kubernetes` service to type NodePort in order to expose the API server:

```bash
root@cks-master:~# k config view --raw > conf
root@cks-master:~# vi /etc/hosts
... # Create a host entry to 'cks-master' or 'kubernetes' to not get SSL errors
root@cks-master:~# vi conf
... # Make sure to change the 'server:' to "cks-master" or "kubernetes"
root@cks-master:~# k --kubeconfig conf get ns
```

- Use `--enable-admission-plugins=NodeRestriction` to enable the NodeRestriction admission plugin

```bash
root@cks-master:/etc/kubernetes/pki# export KUBECONFIG=/etc/kubernetes/kubelet.conf
root@cks-master:/etc/kubernetes/pki# k get ns
Error from server (Forbidden): namespaces is forbidden: User "system:node:cks-master" cannot list resource "namespaces" in API group "" at the cluster scope
root@cks-master:/etc/kubernetes/pki# k label node cks-master cks-test=yes
node/cks-master labeled
root@cks-master:/etc/kubernetes/pki# k label node cks-worker cks-test=yes
Error from server (Forbidden): nodes "cks-worker" is forbidden: node "cks-master" is not allowed to modify node "cks-worker"
root@cks-master:/etc/kubernetes/pki# k label node cks-master node-restriction.kubernetes.io/test=yes
Error from server (Forbidden): nodes "cks-master" is forbidden: is not allowed to modify labels: node-restriction.kubernetes.io/test
root@cks-master:/etc/kubernetes/pki#
```

- Controlling access to the Kubernetes API: https://kubernetes.io/docs/concepts/security/controlling-access/

## Section 15: Cluster hardening - Upgrade kubernetes

- Upgrading kuberetes is done by:

  - Upgrading master components: apiserver, controller-manager, scheduler
  - Upgrading worker components (to the same minor version as apiserver): kubelet, kube-proxy
  - Upgrading kubectl (can be 1 version away)
  - Rule of thumb: components should be same version as apiserver or 1 below

- Upgrading a node can be done with `kubectl drain`: safely evict pods + mark as SchedulingDisabled (`kubectl cordon`)

- Make sure an application can survive an upgrade with:

  - Pod gracePeriod / Terminating state
  - Pod lifecycle events
  - PodDisruptionBudget

- Upgrading master node:
```bash
  256  k drain cks-master --ignore-daemonsets
  261  apt update
  265  apt-cache show kubeadm |grep 1.23
  268  apt-mark hold kubectl kubelet
  269  apt-mark unhold kubeadm
  273  apt install kubeadm=1.23.5-00
  275  kubeadm upgrade plan
  276  kubeadm upgrade apply v1.23.5
  277  apt-mark unhold kubectl kubelet
  278  apt install kubectl=1.23.5-00
  279  apt install kubelet=1.23.5-00
  280 service kubelet restart
  281  k uncordon cks-master
root@cks-master:~#
```

- Upgrading worker node: same except for `kubeadm upgrade node`

- Upgrading Kubeadm clusters: https://kubernetes.io/docs/tasks/administer-cluster/kubeadm/kubeadm-upgrade/

- Kubernetes version skew policy: https://kubernetes.io/releases/version-skew-policy/

## Section 16: Microservices vulnerabilities - Manage Kubernetes secrets

- Secrets should be managed outside of the deployment workflow of applications (CICD, source control)

- Create secret as Mount fs or as Env var:

```bash
apiVersion: v1
kind: Pod
metadata:
  labels:
    run: pod
  name: pod
spec:
  containers:
  - image: nginx
    name: pod
    resources: {}
    env:
    - name: PASSWORD
      valueFrom:
        secretKeyRef:
          name: secret2
          key: pass
    volumeMounts:
    - name: secret1
      mountPath: "/etc/secret1"
      readOnly: true
  volumes:
  - name: secret1
    secret:
      secretName: secret1
```

- `crictl inspect CONTAINER_ID` allows you to see secrets as env variables, also get the pid. With that, you can see the root fs of the container:

```bash
root@cks-worker:~# crictl inspect f0656fefe81f0 |grep PASSWORD
          "key": "PASSWORD",
          "PASSWORD=12345678",
root@cks-worker:~# crictl inspect f0656fefe81f0 |grep pid
    "pid": 3801,
root@cks-worker:~#
root@cks-worker:~# ls /proc/3801/root/etc/secret1/user
/proc/3801/root/etc/secret1/user
root@cks-worker:~#
```

- We can also see secrets un-encrypted in ETCD like this:

```bash
root@cks-master:~# cat /etc/kubernetes/manifests/kube-apiserver.yaml |less
root@cks-master:~# ETCDCTL_API=3 etcdctl --cert /etc/kubernetes/pki/apiserver-etcd-client.crt --key /etc/kubernetes/pki/apiserver-etcd-client.key --cacert /etc/kubernetes/pki/etcd/ca.crt get /registry/secrets/default/secret2
/registry/secrets/default/secret2
k8s


v1Secret�
�
secret2default"*$a13f1ef8-60df-4e12-813f-11bc41e272362����z�a
kubectl-createUpdatev����FieldsV1:-
+{"f:data":{".":{},"f:pass":{}},"f:type":{}}B
pass12345678Opaque"
root@cks-master:~#
```

- Use `--encryption-provider-config` to pass an EncryptionConfiguration to the API server in order to encrypt ETCD at rest.

```
apiVersion: apiserver.config.k8s.io/v1
kind: EncryptionConfiguration
resources:
  - resources:
      - secrets
    providers:
      - aescbc:
          keys:
            - name: key1
              secret: <BASE 64 ENCODED SECRET>
      - identity: {}
```

- After that, run this command to ensure all secrets are encrypted (or decrypted):

```
kubectl get secrets --all-namespaces -o json | kubectl replace -f -
```

- Use `--encryption-provider-config=/etc/kubernetes/etcd/ec.yaml` to pass an EncryptionConfiguration to the API server.

- K8s secret risks: https://kubernetes.io/docs/concepts/configuration/secret/#risks

- Base64 is not encryption, a better story for K8s secrets: https://www.youtube.com/watch?v=f4Ru6CPG1z4

- Build secure apps faster without secrets: https://www.cncf.io/webinars/kubernetes-secrets-management-build-secure-apps-faster-without-secrets

## Section 17: Microservices vulnerabilities - Container runtime sandboxes

- Sandbox ? Additional security layer to reduce attack surface, comes between the User space and the Kernel space.

-> More resources needed
-> Might be better for small containers
-> Not good for syscall heavy workloads
-> No direct access to hardware

- Container calls Linux kernel:

```bash
root@cks-master:~# k exec pod -it -- bash
root@pod:/# uname -r
5.4.0-109-generic
root@pod:/# exit
exit
root@cks-master:~# uname -r
5.4.0-109-generic
root@cks-master:~#
```

- Read about the `Dirty cow` Linux kernel exploit.

- OCI: Open Container Initiative

- K8s runtimes and CRIs:

![cri_tuntimes](./pics/cri_tuntimes.png)

- Container runtime sandboxes:
  - Kata containers: additional isolation with a lightweight VM and individual kernels (Hypervisor-based: QEMU)
  - gVisor: user-space Kernel for containers (simulates kernel syscalls with limited functionnality)

- Create a RuntimeClass:

```bash
root@cks-master:~# cat rc.yaml
apiVersion: node.k8s.io/v1
kind: RuntimeClass
metadata:
  name: gvisor
handler: runsc
root@cks-master:~#
```

- Use `RuntimeClassName: gvisor` in your pod manifest to specify the runtime class.

- OCI, CRI, ??: Making Sense of the Container Runtime Landscape in K8s: https://www.youtube.com/watch?v=RyXL1zOa8Bw

- Sandboxing your containers with gVisor: https://www.youtube.com/watch?v=kxUZ4lVFuVo

- Kata containers, an introduction and overview: https://www.openstack.org/videos/summits/berlin-2018/kata-containers-an-introduction-and-overview

## Section 18: Microservices vulnerabilities - OS level security domains

- Security contexts: define UserID, GroupID, run privleged or unprivileged, linux capabilities

- Set container user and group:

```bash
  securityContext:
    runAsUser: 1000
    runAsGroup: 3000
```

- Force container to run as non-root:

```bash
    securityContext:
      runAsNonRoot: true
```

- Privileged containers:
  - Use `--privileged` or:

```bash
    securityContext:
      privileged: true
```

- Privilege escalation:
  - A process can gain more privileges than its parent process

```bash
    securityContext:
      allowPrivilegeEscalation: false
```

- PodSecurityPolicies:
  - Enable with `--enable-admission-plugins=PodSecurityPolicy`

```bash
apiVersion: policy/v1beta1
kind: PodSecurityPolicy
metadata:
  name: default
spec:
  privileged: false
  allowPrivilegeEscalation: false
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
```

  - Don't forget to allow the default SA to access the PSP !!
    (Once the AdmissionController is enabled, a pod has to have a PSP to run)

```bash
  367  k create role psp-access --verb=use --resource=podsecuritypolicies
  371  k create rolebinding psp-access --role=psp-access --serviceaccount=default:default
```

- Pod Security Policies: https://kubernetes.io/docs/concepts/security/pod-security-policy/

## Section 19: Microservices vulnerabilities - Mtls

- mTLS between 2 pods: every pod will have the ability to encrypt/decrypt traffic sent to other pods

- ServiceMesh/Proxy (with an ExternalManager like Istio): using a proxy sidecar container that takes care of TLS and certificates

- Example (abstract):

```bash
...
spec:
  containers:
...
  - name: proxy
    image: ubuntu
    command:
    - sh
    - -c
    - 'apt-get update && apt-get install iptables -y && iptables -L && sleep 1d'
    securityContext:
      capabilities:
        add: ["NET_ADMIN"]
...
```

- Demystifying Istio sidecar injection model: https://istio.io/latest/blog/2019/data-plane-setup/

## Section 20: Open Policy Agent (OPA)

- OPA: not k8s specific, Rego language, YAML/JSON, uses AdmissionController, knows no pods or deps

- The OPA Gatekeeper uses `ConstraintTemplate` and `Constrain` CRDs.

- Admission webhook types:
  - Validating admission webhook (denies or approves) <- OPA
  - Mutating admission webhook (mutates, e.g: adds labels)

- Example: enforce namespace labels:

```bash
apiVersion: templates.gatekeeper.sh/v1beta1
kind: ConstraintTemplate
metadata:
  name: k8srequiredlabels
spec:
  crd:
    spec:
      names:
        kind: K8sRequiredLabels
      validation:
        # Schema for the `parameters` field
        openAPIV3Schema:
          properties:
            labels:
              type: array
              items: string
  targets:
    - target: admission.k8s.gatekeeper.sh
      rego: |
        package k8srequiredlabels
        violation[{"msg": msg, "details": {"missing_labels": missing}}] {
          provided := {label | input.review.object.metadata.labels[label]}
          required := {label | label := input.parameters.labels[_]}
          missing := required - provided
          count(missing) > 0
          msg := sprintf("you must provide labels: %v", [missing])
        }
```

```bash
apiVersion: constraints.gatekeeper.sh/v1beta1
kind: K8sRequiredLabels
metadata:
  name: ns-must-have-cks
spec:
  match:
    kinds:
      - apiGroups: [""]
        kinds: ["Namespace"]
  parameters:
    labels: ["cks"]
```

- Example: enforce deployment replica count:

```bash
apiVersion: templates.gatekeeper.sh/v1beta1
kind: ConstraintTemplate
metadata:
  name: k8sminreplicacount
spec:
  crd:
    spec:
      names:
        kind: K8sMinReplicaCount
      validation:
        # Schema for the `parameters` field
        openAPIV3Schema:
          properties:
            min:
              type: integer
  targets:
    - target: admission.k8s.gatekeeper.sh
      rego: |
        package k8sminreplicacount
        violation[{"msg": msg, "details": {"missing_replicas": missing}}] {
          provided := input.review.object.spec.replicas
          required := input.parameters.min
          missing := required - provided
          missing > 0
          msg := sprintf("you must provide %v more replicas", [missing])
        }
```

- The Rego playground: https://play.openpolicyagent.org/

- Policing Your Kubernetes Clusters with Open Policy Agent (OPA): https://www.youtube.com/watch?v=RDWndems-sk

## Section 21: Supply chain security - Image footprint

- `RUN`, `COPY` and `ADD` create layers. Other instructs create temporary intermediate images and do not increase build size.

- Example:
```bash
FROM ubuntu
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y golang-go
COPY app.go .
RUN CGO_ENABLED=0 go build app.go

FROM alpine:3.12.1
RUN chmod a-w /etc
RUN addgroup -S appgroup && adduser -S appuser -G appgroup -h /home/appuser
RUN rm -rf /bin/*
COPY --from=0 /app /home/appuser/app
USER appuser
CMD ["/home/appuser/app"]
```
^^ The resulting image will always contain only the last stage.
^^ User specific package versions.
^^ Don't run as root.
^^ Make filesystem read-only.
^^ Remove shell access.

- Best practices for writing Dockerfiles: https://docs.docker.com/develop/develop-images/dockerfile_best-practices/

## Section 22: Supply chain security - Static analysis

- Static analysis: looks at src code and txt files, no binary, checks against/enforces rules

- https://kubesec.io/

- Example on how to use `kubesec`:

```bash
root@cks-master:~# root@cks-master:~# docker run -i kubesec/kubesec:v2 scan /dev/stdin < pod.yaml
[
  {
    "object": "Pod/nginx.default",
    "valid": true,
    "fileName": "STDIN",
    "message": "Passed with a score of 1 points",
    "score": 1,
    "scoring": {
      "passed": [
        {
          "id": "RunAsNonRoot",
          "selector": "containers[] .securityContext .runAsNonRoot == true",
          "reason": "Force the running image to run as a non-root user to ensure least privilege",
          "points": 1
        }
      ],
      "advise": [
        {
          "id": "ApparmorAny",
          "selector": ".metadata .annotations .\"container.apparmor.security.beta.kubernetes.io/nginx\"",
          "reason": "Well defined AppArmor policies may provide greater protection from unknown threats. WARNING: NOT PRODUCTION READY",
          "points": 3
        },
...
```

- Conftest - OPA: unit-test framework for K8s configs

- Example of a Rego policy for K8s:

```bash
root@cks-master:~/cks-course-environment/course-content/supply-chain-security/static-analysis/conftest/kubernetes/policy# cat deployment.rego
# from https://www.conftest.dev
package main

deny[msg] {
  input.kind = "Deployment"
  not input.spec.template.spec.securityContext.runAsNonRoot = true
  msg = "Containers must not run as root"
}

deny[msg] {
  input.kind = "Deployment"
  not input.spec.selector.matchLabels.app
  msg = "Containers must provide app label for pod selectors"
}
root@cks-master:~/cks-course-environment/course-content/supply-chain-security/static-analysis/conftest/kubernetes/policy#
```

  , and performing a test:

```bash
root@cks-master:~/cks-course-environment/course-content/supply-chain-security/static-analysis/conftest/kubernetes# docker run --rm -v $(pwd):/project openpolicyagent/conftest test deploy.yaml
Unable to find image 'openpolicyagent/conftest:latest' locally
latest: Pulling from openpolicyagent/conftest
df9b9388f04a: Already exists
41922b78f199: Pull complete
c1b0167a8ead: Pull complete
6d4b43f93fd0: Pull complete
2d990839474c: Pull complete
Digest: sha256:b00e4d6d26969da5da8cb39708322f7fdc6e28262b938cdefeedb40ae2e37811
Status: Downloaded newer image for openpolicyagent/conftest:latest
FAIL - deploy.yaml - main - Containers must not run as root

2 tests, 1 passed, 0 warnings, 1 failure, 0 exceptions
root@cks-master:~/cks-course-environment/course-content/supply-chain-security/static-analysis/conftest/kubernetes#
```

- Example of a Rego policy for Docker:

```bash
root@cks-master:~/cks-course-environment/course-content/supply-chain-security/static-analysis/conftest/docker/policy# cat base.rego
# from https://www.conftest.dev
package main

denylist = [
  "ubuntu"
]

deny[msg] {
  input[i].Cmd == "from"
  val := input[i].Value
  contains(val[i], denylist[_])

  msg = sprintf("unallowed image found %s", [val])
}
root@cks-master:~/cks-course-environment/course-content/supply-chain-security/static-analysis/conftest/docker/policy# cat commands.rego
# from https://www.conftest.dev

package commands

denylist = [
  "apk",
  "apt",
  "pip",
  "curl",
  "wget",
]

deny[msg] {
  input[i].Cmd == "run"
  val := input[i].Value
  contains(val[_], denylist[_])

  msg = sprintf("unallowed commands found %s", [val])
}
root@cks-master:~/cks-course-environment/course-content/supply-chain-security/static-analysis/conftest/docker/policy#
```

- Rego - Policy language: https://www.openpolicyagent.org/docs/latest/policy-language/#:~:text=Use%20Rego%20for%20defining%20policy,how%20queries%20should%20be%20executed.

## Section 23: Supply chain security - Image vulnerability scanning

- Vulnerabilities databases:
  - https://cve.mitre.org/
  - https://nvd.nist.gov/

- Example tools for image scanning:
  - `clair`: static analysic of containers, API, configureable set of sources
  - `trivy`: for containers and artifacts, 1 cmd run all

- Example of using Trivy:

```bash
root@cks-master:~# docker run ghcr.io/aquasecurity/trivy:latest image nginx:latest |grep CRITICAL
...

Total: 136 (UNKNOWN: 0, LOW: 92, MEDIUM: 20, HIGH: 21, CRITICAL: 3)
| curl             | CVE-2021-22945   | CRITICAL | 7.74.0-1.3+deb11u1      |                   | curl: use-after-free and                |
| libcurl4         | CVE-2021-22945   | CRITICAL | 7.74.0-1.3+deb11u1      |                   | curl: use-after-free and                |
| libfreetype6     | CVE-2022-27404   | CRITICAL | 2.10.4+dfsg-1           |                   | FreeType: Buffer Overflow               |
```

## Section 24: Supply chain security - Secure supply chain

- Steps to use a Docker private registry in Kubernetes:

```bash
# Create secret
$ kubectl create secret docker-registry my-private-registry --docker-server=my-private-registry-server --docker-username=username --docker-password=password --docker-email=email
# Add imagePullSecret
$ kubectl patch serviceaccount default -p '{"imagePullSecrets": [{"name": "my-private-regsitry"}]}'
```

- Using an image digest for Kube-apiserver:

```bash
root@cks-master:~# grep image /etc/kubernetes/manifests/kube-apiserver.yaml
    image: k8s.gcr.io/kube-apiserver@sha256:ddf5bf7196eb534271f9e5d403f4da19838d5610bb5ca191001bde5f32b5492e
    imagePullPolicy: IfNotPresent
root@cks-master:~#
```

- Rego to allow only images from trusted registries:

```bash
root@cks-master:~# cat template.yaml
apiVersion: templates.gatekeeper.sh/v1beta1
kind: ConstraintTemplate
metadata:
  name: k8strustedimages
spec:
  crd:
    spec:
      names:
        kind: K8sTrustedImages
  targets:
    - target: admission.k8s.gatekeeper.sh
      rego: |
        package k8strustedimages
        violation[{"msg": msg}] {
          image := input.review.object.spec.containers[_].image
          not startswith(image, "docker.io/")
          not startswith(image, "k8s.gcr.io/")
          msg := "not trusted image!"
        }
root@cks-master:~# cat constraint.yaml
apiVersion: constraints.gatekeeper.sh/v1beta1
kind: K8sTrustedImages
metadata:
  name: pod-trusted-images
spec:
  match:
    kinds:
      - apiGroups: [""]
        kinds: ["Pod"]
root@cks-master:~#
```

- Steps to enable the `ImagePolicyWebhook`:
  1) Add "--enable-admission-plugins=***,ImagePolicyWebook" in the /etc/kubernetes/manifests/kube-apiserver.yaml

  2) Add the admission configuration:

  ```bash
  root@cks-master:/etc/kubernetes/admission# cat admission_config.yaml
  apiVersion: apiserver.config.k8s.io/v1
  kind: AdmissionConfiguration
  plugins:
    - name: ImagePolicyWebhook
      configuration:
        imagePolicy:
          kubeConfigFile: /etc/kubernetes/admission/kubeconf
          allowTTL: 50
          denyTTL: 50
          retryBackoff: 500
          defaultAllow: false
  root@cks-master:/etc/kubernetes/admission# cat kubeconf
  apiVersion: v1
  kind: Config

  # clusters refers to the remote service.
  clusters:
  - cluster:
      certificate-authority: /etc/kubernetes/admission/external-cert.pem  # CA for verifying the remote service.
      server: https://external-service:1234/check-image                   # URL of remote service to query. Must use 'https'.
    name: image-checker

  contexts:
  - context:
      cluster: image-checker
      user: api-server
    name: image-checker
  current-context: image-checker
  preferences: {}

  # users refers to the API server's webhook configuration.
  users:
  - name: api-server
    user:
      client-certificate: /etc/kubernetes/admission/apiserver-client-cert.pem     # cert for the webhook admission controller to use
      client-key:  /etc/kubernetes/admission/apiserver-client-key.pem             # key matching the cert
  root@cks-master:/etc/kubernetes/admission# ls
  admission_config.yaml  apiserver-client-cert.pem  apiserver-client-key.pem  external-cert.pem  external-key.pem  kubeconf
  root@cks-master:/etc/kubernetes/admission#
  ```

  3) Add "--admission-control-config-file=/etc/kubernetes/admission/admission_config.yaml" in /etc/kubernetes/manifests/kube-apiserver.yaml

  *ATTN:* Don't forget to add volumes and volume mounts:

  ```bash
  root@cks-master:/etc/kubernetes/admission# grep k8s-admission /etc/kubernetes/manifests/kube-apiserver.yaml -A3
        name: k8s-admission
        readOnly: true
      - mountPath: /etc/pki
        name: etc-pki
  --
      name: k8s-admission
    - hostPath:
        path: /etc/pki
        type: DirectoryOrCreate
  root@cks-master:/etc/kubernetes/admission#
  ```

  4) Test (You can also set "defaultAllow: true" in the admission_config.yaml):

  ```bash
  root@cks-master:/etc/kubernetes/admission# k run test --image=nginx
  Error from server (Forbidden): pods "test" is forbidden: Post "https://external-service:1234/check-image?timeout=30s": dial tcp: lookup external-service on 8.8.8.8:53: no such host
  root@cks-master:/etc/kubernetes/admission#
  ```

## Section 25: Runtime security - Behavioral analytics at host and container level

- `strace`: intercepts and logs syscalls made and signals received by a proc

- Example:

```bash
root@cks-master:~# strace -cw ls
accessor.yaml  cassandra-deny.yaml     conf               csr.yaml           gatekeeper.yaml      jane.csr    kubernetes-server-linux-amd64.tar.gz  pod.yaml         template_3.yaml
app.go         cassandra.yaml          constraint_2.yaml  default-deny.yaml  ingress.yaml         jane.key    nginx-ingress-controller.yaml         psp.yaml         template.yaml
app.yaml       cert.pem                constraint_3.yaml  deploy.yaml        install_master.sh    key         np-cloud-metadata-allow.yaml          rc.yaml
backend.yaml   cks-course-environment  constraint.yaml    Dockerfile         install_master.sh.1  key.pem     np-cloud-metadata-deny.yaml           snap
ca             compare                 crt                frontend.yaml      jane.crt             kubernetes  ns.yaml                               template_2.yaml
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 24.65    0.035027        2060        17           mmap
 17.30    0.024580        4916         5           write
 17.01    0.024179        2418        10           fstat
  7.31    0.010391         866        12           mprotect
  5.39    0.007667         697        11           close
  3.93    0.005580         620         9           openat
  3.85    0.005474        1825         3           brk
  3.23    0.004586         573         8         8 access
  3.07    0.004360        2180         2           rt_sigaction
  2.64    0.003753         536         7           read
  2.33    0.003306        3306         1           execve
  2.06    0.002928        1464         2         2 statfs
  1.43    0.002026        2026         1           set_tid_address
  1.42    0.002022        2022         1           set_robust_list
  1.26    0.001785         893         2           ioctl
  1.12    0.001586         793         2           getdents
  0.84    0.001188        1188         1           rt_sigprocmask
  0.50    0.000705         705         1           munmap
  0.43    0.000608         608         1           prlimit64
  0.26    0.000364         364         1           arch_prctl
------ ----------- ----------- --------- --------- ----------------
100.00    0.142115                    97        10 total
root@cks-master:~#
```

- Example: list syscalls of etcd:

```bash
root@cks-master:~# ps -aux |grep etcd | awk '{print $2}'
2549
9750
23389
root@cks-master:~# strace -p 2549 -f | head
strace: Process 2549 attached with 10 threads
[pid  2663] futex(0x1ab2818, FUTEX_WAIT_PRIVATE, 0, NULL <unfinished ...>
[pid  2650] futex(0xc00005ad50, FUTEX_WAIT_PRIVATE, 0, NULL <unfinished ...>
[pid  6824] futex(0xc000303550, FUTEX_WAIT_PRIVATE, 0, NULL <unfinished ...>
[pid  2659] futex(0xc0002bc950, FUTEX_WAIT_PRIVATE, 0, NULL <unfinished ...>
[pid  2642] futex(0x1ab28c0, FUTEX_WAIT_PRIVATE, 0, NULL <unfinished ...>
[pid  2686] futex(0xc000f4e150, FUTEX_WAIT_PRIVATE, 0, NULL <unfinished ...>
[pid  2664] futex(0xc000302d50, FUTEX_WAIT_PRIVATE, 0, NULL <unfinished ...>
...
```

- Example: list open files of etcd:

```bash
root@cks-master:~# ls /proc/2549/fd -lrt
total 0
lrwx------ 1 root root 64 May 14 06:35 9 -> 'socket:[53995]'
lrwx------ 1 root root 64 May 14 06:35 82 -> 'socket:[148085]'
lrwx------ 1 root root 64 May 14 06:35 81 -> 'socket:[148669]'
lrwx------ 1 root root 64 May 14 06:35 80 -> 'socket:[148666]'
lrwx------ 1 root root 64 May 14 06:35 8 -> 'socket:[53994]'
lrwx------ 1 root root 64 May 14 06:35 79 -> 'socket:[148081]'
lrwx------ 1 root root 64 May 14 06:35 78 -> 'socket:[148079]'
lrwx------ 1 root root 64 May 14 06:35 77 -> 'socket:[148663]'
lrwx------ 1 root root 64 May 14 06:35 76 -> 'socket:[148661]'
lrwx------ 1 root root 64 May 14 06:35 75 -> 'socket:[148074]'
lrwx------ 1 root root 64 May 14 06:35 74 -> 'socket:[148659]'
lrwx------ 1 root root 64 May 14 06:35 73 -> 'socket:[148070]'
lrwx------ 1 root root 64 May 14 06:35 72 -> 'socket:[148657]'
lrwx------ 1 root root 64 May 14 06:35 71 -> 'socket:[148656]'
lrwx------ 1 root root 64 May 14 06:35 70 -> 'socket:[148654]'
lrwx------ 1 root root 64 May 14 06:35 7 -> 'socket:[53989]'
lrwx------ 1 root root 64 May 14 06:35 69 -> 'socket:[148652]'
lrwx------ 1 root root 64 May 14 06:35 68 -> 'socket:[148650]'
lrwx------ 1 root root 64 May 14 06:35 67 -> 'socket:[148648]'
lrwx------ 1 root root 64 May 14 06:35 66 -> 'socket:[148646]'
lrwx------ 1 root root 64 May 14 06:35 65 -> 'socket:[148059]'
lrwx------ 1 root root 64 May 14 06:35 64 -> 'socket:[148057]'
lrwx------ 1 root root 64 May 14 06:35 63 -> 'socket:[148054]'
lrwx------ 1 root root 64 May 14 06:35 62 -> 'socket:[148642]'
lrwx------ 1 root root 64 May 14 06:35 61 -> 'socket:[148640]'
lrwx------ 1 root root 64 May 14 06:35 60 -> 'socket:[148051]'
l-wx------ 1 root root 64 May 14 06:35 6 -> 'pipe:[53973]'
lrwx------ 1 root root 64 May 14 06:35 59 -> 'socket:[148049]'
lrwx------ 1 root root 64 May 14 06:35 58 -> 'socket:[148047]'
lrwx------ 1 root root 64 May 14 06:35 57 -> 'socket:[148045]'
lrwx------ 1 root root 64 May 14 06:35 56 -> 'socket:[148043]'
lrwx------ 1 root root 64 May 14 06:35 55 -> 'socket:[148629]'
lrwx------ 1 root root 64 May 14 06:35 54 -> 'socket:[148041]'
lrwx------ 1 root root 64 May 14 06:35 53 -> 'socket:[148039]'
lrwx------ 1 root root 64 May 14 06:35 52 -> 'socket:[148036]'
lrwx------ 1 root root 64 May 14 06:35 51 -> 'socket:[148034]'
lrwx------ 1 root root 64 May 14 06:35 50 -> 'socket:[148032]'
lr-x------ 1 root root 64 May 14 06:35 5 -> 'pipe:[53973]'
lrwx------ 1 root root 64 May 14 06:35 49 -> 'socket:[148030]'
lrwx------ 1 root root 64 May 14 06:35 48 -> 'socket:[148620]'
lrwx------ 1 root root 64 May 14 06:35 47 -> 'socket:[148617]'
lrwx------ 1 root root 64 May 14 06:35 46 -> 'socket:[148613]'
lrwx------ 1 root root 64 May 14 06:35 45 -> 'socket:[148611]'
lrwx------ 1 root root 64 May 14 06:35 44 -> 'socket:[148609]'
lrwx------ 1 root root 64 May 14 06:35 43 -> 'socket:[148025]'
lrwx------ 1 root root 64 May 14 06:35 42 -> 'socket:[148606]'
lrwx------ 1 root root 64 May 14 06:35 41 -> 'socket:[148604]'
lrwx------ 1 root root 64 May 14 06:35 40 -> 'socket:[148602]'
lrwx------ 1 root root 64 May 14 06:35 4 -> 'anon_inode:[eventpoll]'
lrwx------ 1 root root 64 May 14 06:35 39 -> 'socket:[148597]'
lrwx------ 1 root root 64 May 14 06:35 38 -> 'socket:[148589]'
lrwx------ 1 root root 64 May 14 06:35 37 -> 'socket:[148586]'
lrwx------ 1 root root 64 May 14 06:35 36 -> 'socket:[148015]'
lrwx------ 1 root root 64 May 14 06:35 35 -> 'socket:[148583]'
lrwx------ 1 root root 64 May 14 06:35 34 -> 'socket:[148013]'
lrwx------ 1 root root 64 May 14 06:35 33 -> 'socket:[148010]'
lrwx------ 1 root root 64 May 14 06:35 32 -> 'socket:[148008]'
lrwx------ 1 root root 64 May 14 06:35 31 -> 'socket:[148579]'
lrwx------ 1 root root 64 May 14 06:35 30 -> 'socket:[148577]'
lrwx------ 1 root root 64 May 14 06:35 3 -> 'socket:[53972]'
lrwx------ 1 root root 64 May 14 06:35 29 -> 'socket:[148003]'
lrwx------ 1 root root 64 May 14 06:35 28 -> 'socket:[148000]'
lrwx------ 1 root root 64 May 14 06:35 27 -> 'socket:[147998]'
lrwx------ 1 root root 64 May 14 06:35 26 -> 'socket:[147996]'
lrwx------ 1 root root 64 May 14 06:35 25 -> 'socket:[147994]'
lrwx------ 1 root root 64 May 14 06:35 24 -> 'socket:[147991]'
lrwx------ 1 root root 64 May 14 06:35 23 -> 'socket:[147989]'
lrwx------ 1 root root 64 May 14 06:35 22 -> 'socket:[147987]'
lrwx------ 1 root root 64 May 14 06:35 21 -> 'socket:[148569]'
lrwx------ 1 root root 64 May 14 06:35 20 -> 'socket:[54028]'
l-wx------ 1 root root 64 May 14 06:35 2 -> 'pipe:[52482]'
lrwx------ 1 root root 64 May 14 06:35 19 -> 'socket:[52882]'
lrwx------ 1 root root 64 May 14 06:35 18 -> 'socket:[54027]'
lrwx------ 1 root root 64 May 14 06:35 17 -> 'socket:[52881]'
lrwx------ 1 root root 64 May 14 06:35 16 -> 'socket:[147982]'
lrwx------ 1 root root 64 May 14 06:35 15 -> 'socket:[147980]'
lrwx------ 1 root root 64 May 14 06:35 14 -> 'socket:[52875]'
l-wx------ 1 root root 64 May 14 06:35 13 -> /var/lib/etcd/member/wal/0.tmp
lr-x------ 1 root root 64 May 14 06:35 12 -> /var/lib/etcd/member/wal
lrwx------ 1 root root 64 May 14 06:35 11 -> /var/lib/etcd/member/wal/0000000000000001-0000000000015b2c.wal
lrwx------ 1 root root 64 May 14 06:35 10 -> /var/lib/etcd/member/snap/db
l-wx------ 1 root root 64 May 14 06:35 1 -> 'pipe:[52481]'
lrwx------ 1 root root 64 May 14 06:35 0 -> /dev/null
root@cks-master:~# tail /proc/2549/fd/13
^C
root@cks-master:~# tail -f /proc/2549/fd/10
Q/registry/events/gatekeeper-system/gatekeeper-controller-manager.16eabe47d313e46c *k8s


v1Event

.gatekeeper-controller-manager.16eabe47d313e46cgatekeeper-system"*$c6516c6a-db59-4dd5-ab6c-eba3e044ef122z
kube-controller-managerUpdatevFieldsV1:
{"f:count":{},"f:firstTimestamp":{},"f:involvedObject":{},"f:lastTimestamp":{},"f:message":{},"f:reason":{},"f:source":{"f:component":{}},"f:type":{}}Bv

Deploymentgatekeeper-systemgatekeeper-controller-manager"$e8df29fe-7dfc-49de-856f-1d1f6435d788*apps/v1233077:ScalingReplicaSet"CScaled up replica set gatekeeper-controller-manager-8456c55df4 to 3*
deployment-controller@JNormalRbrz"0÷{
^C
root@cks-master:~#
```

- Example: read secret value from etcd:

```bash
root@cks-master:~# ls -lrt /proc/2549/fd/10
lrwx------ 1 root root 64 May 14 06:35 /proc/2549/fd/10 -> /var/lib/etcd/member/snap/db
root@cks-master:~# cat /proc/2549/fd/10 |strings |grep credit-car -A10 -B10
kube-node-lease"
*$59807e2b-2597-4df4-b82f-61a5ac5739ff2
Node
cks-master"$86b0e3ce-9106-4ea3-966e-b9922008f420*
Go-http-client
Update
coordination.k8s.io/v1"
FieldsV1:
{"f:metadata":{"f:ownerReferences":{".":{},"k:{\"uid\":\"86b0e3ce-9106-4ea3-966e-b9922008f420\"}":{}}},"f:spec":{"f:holderIdentity":{},"f:leaseDurationSeconds":{},"f:renewTime":{}}}B
cks-master
$/registry/secrets/default/credit-car
k8s:enc:aescbc:v1:key1:x
9-qw&
.:@%
JRxZl
4/registry/leases/kube-system/kube-controller-manager
coordination.k8s.io/v1
Lease
kube-controller-manager
kube-system"
*$0cb42b91-2d7d-49f1-a899-82e2c6b3af342
root@cks-master:~#
```

- Example: read secret value from environent values:

```bash
root@cks-worker:~# pstree -p |grep httpd |head -1
           |-containerd-shim(23667)-+-httpd(23768)-+-httpd(23795)-+-{httpd}(23800)
root@cks-worker:~# cd /proc/23768/
root@cks-worker:/proc/23768# ls -lh exe
lrwxrwxrwx 1 root root 0 May 14 07:24 exe -> /usr/local/apache2/bin/httpd
root@cks-worker:/proc/23768# ls fd
0  1  2  3  4  5  6  7
root@cks-worker:/proc/23768# ls fd -lrt
total 0
l-wx------ 1 root root 64 May 14 07:24 2 -> 'pipe:[217161]'
l-wx------ 1 root root 64 May 14 07:24 1 -> 'pipe:[217160]'
lrwx------ 1 root root 64 May 14 07:24 0 -> /dev/null
l-wx------ 1 root root 64 May 14 07:24 6 -> 'pipe:[216993]'
l-wx------ 1 root root 64 May 14 07:24 7 -> 'pipe:[217160]'
lr-x------ 1 root root 64 May 14 07:24 5 -> 'pipe:[216993]'
lrwx------ 1 root root 64 May 14 07:24 4 -> 'socket:[216982]'
lrwx------ 1 root root 64 May 14 07:24 3 -> 'socket:[216981]'
root@cks-worker:/proc/23768# cat environ
HTTPD_VERSION=2.4.53KUBERNETES_SERVICE_PORT=443KUBERNETES_PORT=tcp://10.96.0.1:443HOSTNAME=apacheHOME=/rootHTTPD_PATCHES=KUBERNETES_PORT_443_TCP_ADDR=10.96.0.1PATH=/usr/local/apache2/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/binKUBERNETES_PORT_443_TCP_PORT=443HTTPD_SHA256=d0bbd1121a57b5f2a6ff92d7b96f8050c5a45d3f14db118f64979d525858db63KUBERNETES_PORT_443_TCP_PROTO=tcpSECRET=555556666667777777HTTPD_PREFIX=/usr/local/apache2KUBERNETES_PORT_443_TCP=tcp://10.96.0.1:443KUBERNETES_SERVICE_PORT_HTTPS=443KUBERNETES_SERVICE_root@cks-worker:/proc/23768#
```

- `Falco` features:

  - access: deep kernel tracing based on linux kernel
  - assert: describe security rules against system (+default ones)
  - assert: detect un-wanted behaviour
  - action: automate responses to security violations

- Example: Falco error messages:

```bash
root@cks-worker:~# tail /var/log/syslog -f |grep falco
May 14 09:08:23 cks-worker falco: Starting internal webserver, listening on port 8765
May 14 09:09:17 cks-worker falco: 09:09:17.943291726: Notice A shell was spawned in a container with an attached terminal (user=root user_loginuid=-1 apache (id=42031e67301b) shell=bash parent=runc cmdline=bash terminal=34816 container_id=42031e67301b image=docker.io/library/httpd)
May 14 09:12:14 cks-worker falco: 09:12:14.358754650: Error File below /etc opened for writing (user=root user_loginuid=-1 command=bash parent=<NA> pcmdline=<NA> file=/etc/passwd program=bash gparent=<NA> ggparent=<NA> gggparent=<NA> container_id=42031e67301b image=docker.io/library/httpd)
May 14 09:12:53 cks-worker falco: 09:12:53.250780195: Error Package management process launched in container (user=root user_loginuid=-1 command=apt update container_id=42031e67301b container_name=apache image=docker.io/library/httpd:latest)
May 14 09:18:06 cks-worker falco: 09:18:06.114362937: Error Package management process launched in container (user=root user_loginuid=-1 command=apt-get update container_id=95770fd34593 container_name=apache image=docker.io/library/httpd:latest)
May 14 09:18:09 cks-worker falco: 09:18:09.122330681: Error Package management process launched in container (user=root user_loginuid=-1 command=apt-get update container_id=95770fd34593 container_name=apache image=docker.io/library/httpd:latest)
^C
root@cks-worker:~#
```

- Checkout falco rules in /etc/falco/k8s_audit_rules.yaml and /etc/falco/falco_rules.yaml.

- Rules in /etc/falco/falco_rules.local.yaml override other files.

- What have syscalls done for you lately: https://www.youtube.com/watch?v=8g-NUUmCeGI&ab_channel=Docker

- Intro: Falco - Loris Degioanni, Sysdig: https://www.youtube.com/watch?v=zgRFN3o7nJE&ab_channel=CNCF%5BCloudNativeComputingFoundation%5D

## Section 26: Runtime security - Immutability of containers at runtime

- Immutability: a container won't be modified during its lifetime. => We always know the state !!

- Immutability allows:

  - advanced deployment methods
  - easy rollback
  - more reliability
  - better security (on container level)

- Ensuring immutability:

  - removing bash/shell
  - making filesystem read-only
  - run as user and non root
  - else:

    - using a `startupProbe` with command "chmod a-w-R /"
    - enforcing read-only root filesystems with SecurityContexts or PodSecurityPolicies
    - moving logic to init containers (e.g: only give read-write permissions to init containers)

- Example:

```bash
spec:
  containers:
  - image: httpd
    name: immutable
    resources: {}
    securityContext:
      readOnlyRootFilesystem: true
    volumeMounts:
    - mountPath: /usr/local/apache2/logs
      name: cache-volume
  volumes:
  - name: cache-volume
    emptyDir: {}
```

- Similarly with Docker:

```bash
docker run --read-only --tmpfs /run my-container
```

## Section 27: Runtime security - Auditing

- Audit logs: history of API calls

- Audit Policy Stages:

  - RequestReceived
  - ResponseStarted: (response headers sent but not the body)
  - ResponseComplete
  - Panic

- Audit Policy Levels:

  - None
  - Metadata: (request metadata)
  - Request: (request metadata + body)
  - RequestResponse: (request + response metadata + body)

- Example of Audit Policy:

```bash
root@cks-master:/etc/kubernetes/manifests# cat ../audit/policy.yaml
apiVersion: audit.k8s.io/v1
kind: Policy
rules:
- level: Metadata
root@cks-master:/etc/kubernetes/manifests#
```

To configure it, add the following arguments in the kube-apiserver.yaml:

```bash
    - --audit-policy-file=/etc/kubernetes/audit/policy.yaml       # add
    - --audit-log-path=/etc/kubernetes/audit/logs/audit.log       # add
    - --audit-log-maxsize=500                                     # add
    - --audit-log-maxbackup=5                                     # add
```

  *ATTN:* Don't forget to add volumes and volume mounts:

- Audit log example:

```bash
e","apiVersion":"v1"},"responseStatus":{"metadata":{},"code":200},"requestReceivedTimestamp":"2022-05-15T17:50:18.699178Z","stageTimestamp":"2022-05-15T17:50:18.700870Z","annotations":{"authorization.k8s.io/decision":"allow","authorization.k8s.io/reason":""}}
{"kind":"Event","apiVersion":"audit.k8s.io/v1","level":"Metadata","auditID":"6f268b7f-d717-484a-a4c5-6ca08365a57c","stage":"ResponseComplete","requestURI":"/api/v1/namespaces/default/secrets?fieldManager=kubectl-create","verb":"create","user":{"username":"kubernetes-admin","groups":["system:masters","system:authenticated"]},"sourceIPs":["192.168.13.138"],"userAgent":"kubectl/v1.23.5 (linux/amd64) kubernetes/c285e78","objectRef":{"resource":"secrets","namespace":"default","name":"very-secure","apiVersion":"v1"},"responseStatus":{"metadata":{},"code":201},"requestReceivedTimestamp":"2022-05-15T17:50:20.617783Z","stageTimestamp":"2022-05-15T17:50:20.620154Z","annotations":{"authorization.k8s.io/decision":"allow","authorization.k8s.io/reason":""}}
```

- Another example of Audit Policy, logging only Metadata from Secrets, not logging some verbs, omotting some stages:

```bash
apiVersion: audit.k8s.io/v1
kind: Policy
rules:
- level: RequestResponse
- level: None
  verbs: ["get", "list", "watch"]
- level: Metadata
  resources:
  - group: ""
    resources: ["secrets"]
omitStages:
  - "RequestReceived"
```

- Auditing in Kubernetes 101 - Nikhita Raghunath, Loodse: https://www.youtube.com/watch?v=HXtLTxo30SY

## Section 28: System hardening - Kernel hardening tools

- Container isolation:
  - namespaces: restrict what procs can see
  - cgroups: restrict resource usage of procs
  - sandboxing: w/o it containers talk directly w the syscall iface (getpid(), reboot()) => Here we use Seccomp or Apparmor

- Apparmor:
  - profile modes:
    - Unconfined: process can escape
    - Complain: process can escape but it will be logged
    - Enforce: process cannot escape
  - commands:
    - `aa-status`: show all profs
    - `aa-genprof`: create new prof
    - `aa-complain`: put prof in complain mode
    - `aa-enforce`: put prof in enforce mode
    - `aa-logprof`: up prof if app produces more logs

- Example: Apparmor for curl:
  1. Generate new profile in enforce mode:

```bash
root@cks-worker:~# aa-genprof curl
Writing updated profile for /usr/bin/curl.
Setting /usr/bin/curl to complain mode.

Before you begin, you may wish to check if a
profile already exists for the application you
wish to confine. See the following wiki page for
more information:
http://wiki.apparmor.net/index.php/Profiles

Profiling: /usr/bin/curl

Please start the application to be profiled in
another window and exercise its functionality now.

Once completed, select the "Scan" option below in
order to scan the system logs for AppArmor events.

For each AppArmor event, you will be given the
opportunity to choose whether the access should be
allowed or denied.

[(S)can system log for AppArmor events] / (F)inish
Setting /usr/bin/curl to enforce mode.

Reloaded AppArmor profiles in enforce mode.

Please consider contributing your new profile!
See the following wiki page for more information:
http://wiki.apparmor.net/index.php/Profiles

Finished generating profile for /usr/bin/curl.
root@cks-worker:~# curl killer.sh -v
* Rebuilt URL to: killer.sh/
* Could not resolve host: killer.sh
* Closing connection 0
curl: (6) Could not resolve host: killer.sh
root@cks-worker:~#
```

  2. Update profile based on usage logs from /var/log/syslog

```bash
root@cks-worker:/etc/apparmor.d# aa-logprof
Reading log entries from /var/log/syslog.
Updating AppArmor profiles in /etc/apparmor.d.
Enforce-mode changes:

# Last Modified: Sun May 22 02:27:20 2022
#include <tunables/global>

/usr/bin/curl {
  #include <abstractions/base>
  #include <abstractions/nameservice>
  #include <abstractions/openssl>

  /lib/x86_64-linux-gnu/ld-*.so mr,
  /usr/bin/curl mr,

}
...skipping...
Profile:  /usr/bin/curl
Path:     /etc/ssl/openssl.cnf
New Mode: owner r
Severity: 2

 [1 - #include <abstractions/openssl>]
  2 - #include <abstractions/ssl_keys>
  3 - owner /etc/ssl/openssl.cnf r,
(A)llow / [(D)eny] / (I)gnore / (G)lob / Glob with (E)xtension / (N)ew / Audi(t) / (O)wner permissions off / Abo(r)t / (F)inish
Adding #include <abstractions/openssl> to profile.

Profile:  /usr/bin/curl
Path:     /etc/host.conf
New Mode: owner r
Severity: unknown

 [1 - #include <abstractions/nameservice>]
  2 - owner /etc/host.conf r,
(A)llow / [(D)eny] / (I)gnore / (G)lob / Glob with (E)xtension / (N)ew / Audi(t) / (O)wner permissions off / Abo(r)t / (F)inish
Adding #include <abstractions/nameservice> to profile.

= Changed Local Profiles =

The following local profiles were changed. Would you like to save them?

 [1 - /usr/bin/curl]
(S)ave Changes / Save Selec(t)ed Profile / [(V)iew Changes] / View Changes b/w (C)lean profiles / Abo(r)t
Writing updated profile for /usr/bin/curl.
root@cks-worker:/etc/apparmor.d# cat /etc/apparmor.d/usr.bin.curl
# Last Modified: Sun May 22 02:27:20 2022
#include <tunables/global>

/usr/bin/curl {
  #include <abstractions/base>
  #include <abstractions/nameservice>
  #include <abstractions/openssl>

  /lib/x86_64-linux-gnu/ld-*.so mr,
  /usr/bin/curl mr,

}
root@cks-worker:/etc/apparmor.d#
```

- Example: Apparmor for docker w nginx:

```bash
root@cks-worker:/etc/apparmor.d# cat /etc/apparmor.d/docker-nginx
#include <tunables/global>


profile docker-nginx flags=(attach_disconnected,mediate_deleted) {
  #include <abstractions/base>

  network inet tcp,
  network inet udp,
  network inet icmp,

  deny network raw,

  deny network packet,

  file,
  umount,

  deny /bin/** wl,
  deny /boot/** wl,
  deny /dev/** wl,
  deny /etc/** wl,
  deny /home/** wl,
  deny /lib/** wl,
  deny /lib64/** wl,
  deny /media/** wl,
  deny /mnt/** wl,
  deny /opt/** wl,
  deny /proc/** wl,
  deny /root/** wl,
  deny /sbin/** wl,
  deny /srv/** wl,
  deny /tmp/** wl,
  deny /sys/** wl,
  deny /usr/** wl,

  audit /** w,

  /var/run/nginx.pid w,

  /usr/sbin/nginx ix,

  deny /bin/dash mrwklx,
  deny /bin/sh mrwklx,
  deny /usr/bin/top mrwklx,


  capability chown,
  capability dac_override,
  capability setuid,
  capability setgid,
  capability net_bind_service,

  deny @{PROC}/* w,   # deny write for all files directly in /proc (not in a subdir)
  # deny write to files not in /proc/<number>/** or /proc/sys/**
  deny @{PROC}/{[^1-9],[^1-9][^0-9],[^1-9s][^0-9y][^0-9s],[^1-9][^0-9][^0-9][^0-9]*}/** w,
  deny @{PROC}/sys/[^k]** w,  # deny /proc/sys except /proc/sys/k* (effectively /proc/sys/kernel)
  deny @{PROC}/sys/kernel/{?,??,[^s][^h][^m]**} w,  # deny everything except shm* in /proc/sys/kernel/
  deny @{PROC}/sysrq-trigger rwklx,
  deny @{PROC}/mem rwklx,
  deny @{PROC}/kmem rwklx,
  deny @{PROC}/kcore rwklx,

  deny mount,

  deny /sys/[^f]*/** wklx,
  deny /sys/f[^s]*/** wklx,
  deny /sys/fs/[^c]*/** wklx,
  deny /sys/fs/c[^g]*/** wklx,
  deny /sys/fs/cg[^r]*/** wklx,
  deny /sys/firmware/** rwklx,
  deny /sys/kernel/security/** rwklx,
}
root@cks-worker:/etc/apparmor.d# apparmor_parser /etc/apparmor.d/docker-nginx
root@cks-worker:/etc/apparmor.d# docker run --security-opt apparmor=docker-nginx nginx
/docker-entrypoint.sh: 13: cannot create /dev/null: Permission denied
/docker-entrypoint.sh: No files found in /docker-entrypoint.d/, skipping configuration
2022/05/22 09:49:50 [notice] 1#1: using the "epoll" event method
2022/05/22 09:49:50 [notice] 1#1: nginx/1.21.6
2022/05/22 09:49:50 [notice] 1#1: built by gcc 10.2.1 20210110 (Debian 10.2.1-6)
2022/05/22 09:49:50 [notice] 1#1: OS: Linux 5.4.0-110-generic
2022/05/22 09:49:50 [notice] 1#1: getrlimit(RLIMIT_NOFILE): 1048576:1048576
2022/05/22 09:49:50 [notice] 1#1: start worker processes
2022/05/22 09:49:50 [notice] 1#1: start worker process 11
2022/05/22 09:49:50 [notice] 1#1: start worker process 12
^C2022/05/22 09:49:59 [notice] 1#1: signal 2 (SIGINT) received, exiting
2022/05/22 09:49:59 [notice] 11#11: exiting
2022/05/22 09:49:59 [notice] 11#11: exit
2022/05/22 09:49:59 [notice] 12#12: exiting
2022/05/22 09:49:59 [notice] 12#12: exit
2022/05/22 09:49:59 [notice] 1#1: signal 17 (SIGCHLD) received from 11
2022/05/22 09:49:59 [notice] 1#1: worker process 11 exited with code 0
2022/05/22 09:49:59 [notice] 1#1: worker process 12 exited with code 0
2022/05/22 09:49:59 [notice] 1#1: exit
root@cks-worker:/etc/apparmor.d#
```

(see above the nginx container is being permission denied)

- Apparmor w k8s:
  - CR must support apparmor
  - Apparmor must be installed in all nodes
  - Apparmor prof must be available in all nodes
  - Apparmor prof must be specified on container level (w annotations)

- Example: k8s pod w apparmore:

```bash
root@cks-master:~# cat pod.yaml
apiVersion: v1
kind: Pod
metadata:
  creationTimestamp: null
  annotations:
    container.apparmor.security.beta.kubernetes.io/secure: localhost/docker-nginx
  labels:
    run: secure
  name: secure
spec:
  containers:
  - image: nginx
    name: secure
    resources: {}
  dnsPolicy: ClusterFirst
  restartPolicy: Always
status: {}
root@cks-master:~#
```

- Seccomp:
  - Secure Computing mode
  - Security facility in the Linux kernel
  - Restricts execution of syscalls made by processes
  - (default) --------------> exit()       }
              |-------------> sigreturn()  } only these are allowed
              |-------------> read()       }
              |-------------> write()      }
              |-------------> exex()   } SIGKILL
              |-------------> getpid() }
  - `seccomp-bpf`: combined with BPF filters

- Example: Seccomp profile for docker w nginx: https://github.com/killer-sh/cks-course-environment/blob/master/course-content/system-hardening/kernel-hardening-tools/seccomp/profile-docker-nginx.json
  - (try removing 'write' from the file)

```bash
vi default.json
root@cks-worker:~# root@cks-worker:~# docker run --security-opt seccomp=default.json nginx
docker: Error response from daemon: OCI runtime start failed: cannot start an already running container: unknown.
ERRO[0000] error waiting for container: context canceled
root@cks-worker:~#
```

- Example: k8s pod w seccomp:

```bash
root@cks-master:~# cat pod.yaml
apiVersion: v1
kind: Pod
metadata:
  creationTimestamp: null
  labels:
    run: secure
  name: secure
spec:
  securityContext:
    seccompProfile:
      type: Localhost
      localhostProfile: default.json
  containers:
  - image: nginx
    name: secure
    resources: {}
  dnsPolicy: ClusterFirst
  restartPolicy: Always
status: {}
root@cks-master:~#
```

- What Have Syscalls Done for you Lately? : https://www.youtube.com/watch?v=8g-NUUmCeGI&ab_channel=Docker

- Aaron Jones: Introduction To Firejail, AppArmor, and SELinux: https://www.youtube.com/watch?v=JFjXvIwAeVI&ab_channel=BrianCluff

## Section 29: System hardening - Reduce attack surface

- Applications: Keep up to date + Up linux kernel + Rm non needed packages
- Network: Network behind firewall + Check and close open ports
- IAM: run as user, not root + restrict user permissions

- Nodes should only run k8s components => rm un-necessary services
- Nodes should be ephemeral, created from images and can be recycled fast at any time

- Linux distros have many services => (+) more convenient but (-) wider attack surface

- Example: make sure service is disabled:

```bash
root@cks-master:~# systemctl list-units --type=service --state=running |grep snapd
snapd.service               loaded active running Snap Daemon
root@cks-master:~#
```

# Preparations

- TODO:
  - Alias + Autocomplete <- K8s cheat sheet

- Allowed tools:
  - https://kubernetes.io/docs/ and their subdomains
  - https://github.com/kubernetes/ and their subdomains
  - https://kubernetes.io/blog/ and their subdomains
  - Trivy documentation https://aquasecurity.github.io/trivy/
  - Falco documentation https://falco.org/docs/
  - App Armor documentation https://gitlab.com/apparmor/apparmor/-/wikis/Documentation

- Things to remeber:
  - How to run `kube-bench`:

  ```bash
  docker run --pid=host -v /etc:/etc:ro -v /var:/var:ro -t aquasec/kube-bench:latest master --version 1.23
  ```

  - Server binaries: https://github.com/kubernetes/kubernetes/blob/v1.24.1/CHANGELOG/CHANGELOG-1.24.md#server-binaries

  - Use `automountServiceAccountToken: false` to disable svc account mounting
  - Use `--anonymous-auth=false` to disable anonymous auth
  - Use `--enable-admission-plugins=NodeRestriction` to enable NodeRestriction admission plugin
  - Use `--encryption-provider-config` to pass an EncryptionConfiguration for ETCD
  - Use `RuntimeClassName: gvisor` to specify a RuntimeClass and use a Sandbox
  - System ServiceAccounts are named like: `system:serviceaccount:ns1:pipeline`
  - Use `--enable-admission-plugins=PodSecurityPolicy` to enable PSP admission plugin (and allow default SA to use PSPs)

  - How to run `kube-sec`:

  ```bash
  docker run -i kubesec/kubesec:v2 scan /dev/stdin < pod.yaml
  ```

  - How to run `opa/conftest`:

  ```bash
  docker run --rm -v $(pwd):/project openpolicyagent/conftest test deploy.yaml
  ```

  - How to run `trivy`:

  ```bash
  docker run ghcr.io/aquasecurity/trivy:latest image nginx:latest |grep CRITICAL
  ```

  - Use `--enable-admission-plugins=ImagePolicyWebook` to enable Image policy webhook
  - User `--audit-policy-file` and `--audit-log-path|maxsize|maxbackup` to configure an Audit policy

  - AA commands: `aa-status`, `aa-genprof`, `aa-complain`, `aa-enforce` and `aa-logprof`
  - Crictl commands: `crictl ps -id` and `crictl pods -id`
  - Etcdctl commands: `etcdctl ... get /registry/{type}/{namespace}/{name}`

  - Vim: to insert in many lines, use CTRL + V then SHIFT + I
  - Awk: to have many delimiters, user -F '[,.]'

  - User `--group system:serviceaccounts` when creating RoleBindings to allow all NSs.