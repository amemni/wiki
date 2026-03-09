from bcc import BPF
import sys

program = r"""
int hello (void *ctx) {
    bpf_trace_print("Hello from Aymen!!");
    return 0;
}
"""

b = BPF(text=program)
syscall = b.get_syscall_fname("excerve")
b.attach_kprobe(event=syscall, fn_name="hello")

try:
    b.trace_point()
except KeyboardInterrupt:
    sys.exit(0)
