# Kops

## Cannot evict pod as it would violate the pod's disruption budget

It happens during a rolling update, but you're in the middle of a production deployment, and it's too late to go there and adjust the `PodDisruptionBudget` of your application, so how can you force evict your pods ? Just run something like this:

```sh
#!/usr/bin/env bash
​
NODE=$(kubectl get nodes --field-selector spec.unschedulable==true -o custom-columns=NAME:.metadata.name --no-headers=true)
kubectl get pods --all-namespaces --field-selector metadata.namespace!=kube-system,spec.nodeName==$NODE --no-headers=true | awk '{print $1,$2}' | while read i; do kubectl delete pod -n $i; done
```

Later, the approriate thing to do is to go back and fix the `PodDisruptionBudget` of your application so that it allows you to actually do a rolling update and/or make updates to your application. Of course, seeting `minAvailable` to 100% or 99% is not realistic and you should leave space for evicting, at least, one pod by one.

What is a `PodDisruptionBudget` ? Read: <https://kubernetes.io/docs/tasks/run-application/configure-pdb/#specifying-a-poddisruptionbudget>
