# My home Lab

## Into

Here I'll be documenting steps I'm following to build my home Lab and experiment with different tools and technology.

## K3d

//TODO

## Strongswan IPSec with my RPi

Following: <https://forums.raspberrypi.com/viewtopic.php?t=101673>

//TBC

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

//TBC
