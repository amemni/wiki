# My home Lab

## Into

Here I'll be documenting steps I'm following to build my home Lab and experiment with different tools and technology.

## K3d

I'm using K3d (a a wrapper for K3s, from Rancher) to deploy a minimal K8s cluster on my laptop and connecting my RPi as a worker node to it.

Installation can be done using the installation script: <https://k3d.io/k3/#releases>

Then, you can provision a new cluster like this:

```sh
amemni@Amemnis-KALI:~$ k3d cluster create amemni --config k3d.yaml
amemni@Amemnis-KALI:~$
```

Follow this link if you want rootless Podman instead of docker: <https://k3d.io/v5.8.3/usage/advanced/podman/#using-podman>
And, beware of this issue: <https://github.com/k3d-io/k3d/issues/1052>
Basically, checkout the `--kubelet-arg` added in order to run Kubelet in userspace.

To be continued ..

## Cilium

Following: <https://trainingportal.linuxfoundation.org/learn/course/introduction-to-cilium-lfs146>

Instructions to install Cilium CLI: <https://docs.cilium.io/en/stable/gettingstarted/k8s-install-default/#install-the-cilium-cli>
Instructions to install Hubble CLI: <https://docs.cilium.io/en/stable/observability/hubble/setup/#hubble-setup>

Let's starting installing Cilium on my K3d cluster using the CLI method (in contrast to Helm):

```sh
amemni@Amemnis-KALI:~$ cilium install
🔮 Auto-detected Kubernetes kind: K3s
ℹ️  Using Cilium version 1.18.5
🔮 Auto-detected cluster name: k3d-amemni-cluster
amemni@Amemnis-KALI:~$
```

After a few minutes, let's use the following command to confirm and watch the status:

```sh
amemni@Amemnis-KALI:~$ cilium status --wait
    /¯¯\
 /¯¯\__/¯¯\    Cilium:             OK
 \__/¯¯\__/    Operator:           OK
 /¯¯\__/¯¯\    Envoy DaemonSet:    OK
 \__/¯¯\__/    Hubble Relay:       OK
    \__/       ClusterMesh:        disabled
...
amemni@Amemnis-KALI:~$
```

:bulb: Since we're using K3d, beware of this open issue: <https://github.com/cilium/cilium/issues/38222>
For the time being, you have to change the tunnel protocol from "vxlan" to "geneve"

Finally, let's enable Hubble:

```sh
amemni@Amemnis-KALI:~$ cilium hubble enable --ui
# already enabled
amemni@Amemnis-KALI:~$
```

You can deploy the connectivity test suite to confirm network and policy enforcement:

```
amemni@Amemnis-KALI:~$ cilium connectivity test --request-timeout 30s --connect-timeout 10s
ℹ️  Monitor aggregation detected, will skip some flow validation steps
✨ [k3d-amemni-cluster] Creating namespace cilium-test-1 for connectivity check...
✨ [k3d-amemni-cluster] Deploying echo-same-node service...
✨ [k3d-amemni-cluster] Deploying DNS test server configmap...
✨ [k3d-amemni-cluster] Deploying same-node deployment...
✨ [k3d-amemni-cluster] Deploying client deployment...
✨ [k3d-amemni-cluster] Deploying client2 deployment...
✨ [k3d-amemni-cluster] Deploying client3 deployment...
✨ [k3d-amemni-cluster] Deploying echo-other-node service...
✨ [k3d-amemni-cluster] Deploying other-node deployment...
✨ [host-netns] Deploying k3d-amemni-cluster daemonset...
✨ [host-netns-non-cilium] Deploying k3d-amemni-cluster daemonset...
ℹ️  Skipping tests that require a node Without Cilium
✨ [k3d-amemni-cluster] Deploying ccnp deployment...
⌛ [k3d-amemni-cluster] Waiting for deployment cilium-test-1/client to become ready...
...
amemni@Amemnis-KALI:~$
```

Questions:

- What are eBPF maps ?

## Strongswan IPSec with my RPi

Following: <https://forums.raspberrypi.com/viewtopic.php?t=101673>

On my RPi and my host, I first needed to install following packages:

```sh
sudo apt install strongswan
sudo apt install strongswan-starter
sudo apt install strongswan-charon
sudo apt install strongswan-pki
```

For a host-to-host case, the IPsec transport mode should be sufficient. However, we'll use the IPsec tunnel mode for learning purposes.

Here's a configuration example: <https://docs.strongswan.org/docs/latest/config/quickstart.html#_host_to_host_case>

To be continued ..

## cGroups v2 on my RPi

Following: <https://downey.io/blog/exploring-cgroups-raspberry-pi/>

On my RPi, the memory cgroup seems missing:

```sh
amemni@Amemnis-RASP:~ $ cat /proc/cgroups 
#subsys_name    hierarchy       num_cgroups     enabled
cpuset  0       66      1
cpu     0       66      1
cpuacct 0       66      1
blkio   0       66      1
devices 0       66      1
freezer 0       66      1
net_cls 0       66      1
perf_event      0       66      1
net_prio        0       66      1
pids    0       66      1
amemni@Amemnis-RASP:~ $ 
```

Seems the reason for that is that the memory controller adds a noticeable overhead. To enable it, follow the documentation: [Enable Memory Cgroups](https://docs.k0sproject.io/v1.33.1+k0s.0/raspberry-pi5/#enable-memory-cgroups)

Now let's do a test, I will create a new cGroup and see what's inside:

```sh
amemni@Amemnis-RASP:~ $ sudo mkdir -p /sys/fs/cgroup/stress-test
amemni@Amemnis-RASP:~ $ ls /sys/fs/cgroup/stress-test
cgroup.controllers  cgroup.max.depth        cgroup.subtree_control  cpu.max         cpu.weight       memory.events.local  memory.min        memory.reclaim       memory.swap.high      memory.zswap.max        pids.events.local
cgroup.events       cgroup.max.descendants  cgroup.threads          cpu.max.burst   cpu.weight.nice  memory.high          memory.numa_stat  memory.stat          memory.swap.max       memory.zswap.writeback  pids.max
cgroup.freeze       cgroup.procs            cgroup.type             cpu.stat        memory.current   memory.low           memory.oom.group  memory.swap.current  memory.swap.peak      pids.current            pids.peak
cgroup.kill         cgroup.stat             cpu.idle                cpu.stat.local  memory.events    memory.max           memory.peak       memory.swap.events   memory.zswap.current  pids.events
amemni@Amemnis-RASP:~ $
```

I will assign the cGroup to my running shell:

```sh
amemni@Amemnis-RASP:~ $ sudo sh -c "echo $$ > /sys/fs/cgroup/stress-test/cgroup.procs"
amemni@Amemnis-RASP:~ $ cat /sys/fs/cgroup/stress-test/cgroup.procs
1763
1869
amemni@Amemnis-RASP:~ $
```

At this stage, I could not assign assign a limit of 100 MB to the cgroup:

```sh
amemni@Amemnis-RASP:~ $ sudo sh -c "echo 268435456 > /sys/fs/cgroup/stress-test/memory.limit_in_bytes"
sh: 1: cannot create /sys/fs/cgroup/stress-test/memory.limit_in_bytes: Permission denied
amemni@Amemnis-RASP:~ $ 
```

I need to figure out why the memory controller is not enabled yet. There seems to be an issue: <https://forums.raspberrypi.com/viewtopic.php?t=389843>

To be continued ..
