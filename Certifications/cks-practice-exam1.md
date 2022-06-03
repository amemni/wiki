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

4. Run the pod:

```bash
k run -it