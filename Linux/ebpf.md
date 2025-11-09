# Leaning eBPF

This document collects notes, findings and interesting links I encounter whilst trying to learn eBPF.

## What is eBPF 

To my understanding:

- eBPF is an extension to BPF (the Berkley Paket Filter) introduced on kernel version 3.18 in 2014,
- eBPF allows to write custom code that can be loaded dynamically into the kernel (changing its behavior) and, therefore, to instrument an application using eBPF based tools, you don't need to rebuild the application !!,
- eBPF can be used for many observability and security applications, ranging from performance tracing, high-performance networking, detecting and preventing malicious activity ..etc.

## Code examples

These code examples are taken from the [Leanring eBPF](https://isovalent.com/books/learning-ebpf/) book by Liz Rice.

### Hello world

Code:

```py
#!/usr/bin/python3  
from bcc import BPF
import sys

program = r"""
int hello(void *ctx) {
    bpf_trace_printk("Hello World!");
    return 0;
}
"""

b = BPF(text=program)
syscall = b.get_syscall_fnname("execve")
b.attach_kprobe(event=syscall, fn_name="hello")

try:
    b.trace_print()
except KeyboardInterrupt:
    sys.exit(0)
```

This uses BCC's Python library. In the above example, `hello.py` (the script) is the user space part of the application, whereas `hello()` is the eBPF program that gets run in the kernel.

Here, `get_syscall_fnname()` is a helper function (a feature of eBPF) which is a convenient way to find the name of the function that implements the "excerve()" system call (which can vary from a kernel version to another).

What happens next is the `attach_kprobe` will attach, using a kprobe (kernel probe), the eBPF program to the said kernel function. The eBPF program will then be loaded whenever an executable is launched in the machine and writes the tracing on the screen.

### Hello world for a network interface

Code:

```c
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

int counter = 0;

SEC("xdp")
int hello(struct xdp_md *ctx) {
    bpf_printk("Hello World %d", counter);
    counter++; 
    return XDP_PASS;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
```

In the above example, the user space and the eBPF program are in c code, which is why it's recommended to put eBPF programs into filenames ending with `.bpf.c`.

Good to mention that the macro `SEC()` defines a section called XDP (an Express Data Path type of eBPF program) that will be visible in the compiled object file. The macro `SEC()` will be called again at the end to define a license string, a requirement to be able to use eBPF helper functions.

The eBPF program will simply listen for incoming network packets and do nothing (no inspection) except incrementing a counter and printing a message. The eBPF program returns an `XDP_PASS` at the end, which is a verdict indicating the kernel should not block the paket.

### Loading a program into the kernel

Let's refer to the previous example of a hello world for a network interface. What comes next is:

- The eBPF source code needs to be complied into an eBPF byte code (instructions that the eBPF virtual machine will understand). The Clang compiler from the [LLVM](https://llvm.org/) project can be used for that. Here's an extract from the Makefile that does that:

```make
hello.bpf.o: %.o: %.c
clang \
-target bpf \
-I/usr/include/$(shell uname -m)-linux-gnu \
-g \
-O2 -c $< -o $@
```

- After that, using a tool called `bpftool`, we can load the eBPF program into the kernel like this:

```sh
amemni@Amemnis-KALI:~$ sudo bpftool prog load hello.bpf.o /sys/fs/bpf/hello
amemni@Amemnis-KALI:~$
```

- The eBPF program is then loaded and pinned into the location `/sys/fs/bpf`:

```sh
amemni@Amemnis-KALI:~$ sudo ls /sys/fs/bpf
hello
amemni@Amemnis-KALI:~$ sudo bpftool prog list
...
77: xdp  name hello  tag d35b94b4c0c10efb  gpl
        loaded_at 2025-11-09T21:55:36+0100  uid 0
        xlated 96B  jited 76B  memlock 4096B  map_ids 17,18
        btf_id 270
amemni@Amemnis-KALI:~$  
```

- The eBPF program can be inspected like this:

```sh
amemni@Amemnis-KALI:~$ sudo bpftool prog show id 77 --pretty
{
    "id": 77,
    "type": "xdp",
    "name": "hello",
    "tag": "d35b94b4c0c10efb",
    "gpl_compatible": true,
    "loaded_at": 1762721736,
    "uid": 0,
    "orphaned": false,
    "bytes_xlated": 96,
    "jited": true,
    "bytes_jited": 76,
    "bytes_memlock": 4096,
    "map_ids": [17,18
    ],
    "btf_id": 270
}
amemni@Amemnis-KALI:~$ 
```

- Since we're talking about an XDP program here, let's try to attach the eBPF program to an XDP event on network interface eth0 like this:

```sh
amemni@Amemnis-KALI:~$ sudo bpftool prog show id 77 --pretty
{
    "id": 77,
    "type": "xdp",
    "name": "hello",
    "tag": "d35b94b4c0c10efb",
    "gpl_compatible": true,
    "loaded_at": 1762721736,
    "uid": 0,
    "orphaned": false,
    "bytes_xlated": 96,
    "jited": true,
    "bytes_jited": 76,
    "bytes_memlock": 4096,
    "map_ids": [17,18
    ],
    "btf_id": 270
}
amemni@Amemnis-KALI:~$ 
```

- To confirm, let's see the details of the network interface using `ip link`:

```sh
amemni@Amemnis-KALI:~$ ip link show eth0
2: eth0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 xdpgeneric qdisc fq_codel state DOWN mode DEFAULT group default qlen 1000
    link/ether 98:fa:9b:d7:e7:c9 brd ff:ff:ff:ff:ff:ff
    prog/xdp id 77 
amemni@Amemnis-KALI:~$
```

- If everything went fine, we should see trace events with the command `bpftool prog tracelog`, or simply by looking at `/sys/kernel/debug/tracing/trace_pipe`.

### More examples

To be continued ..

## Playing around with Pixie

## Interesting links

Tutorials and blogs:

- [lizrice / learning-ebpf](https://github.com/lizrice/learning-ebpf/)
- [eunomia-bpf / eBPF developer tutorials](https://github.com/eunomia-bpf/bpf-developer-tutorial)
- [notes on BPF & eBPF](https://jvns.ca/blog/2017/06/28/notes-on-bpf---ebpf/)

Tools and projects:

- [cloudflare / bpftools](https://github.com/cloudflare/bpftools)
- [pixie](https://px.dev/)
- [bpftrace](https://bpftrace.org/)