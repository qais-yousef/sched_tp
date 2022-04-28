# sched_tp

Kernel module to create scheduler trace events.

Scheduler provide bare tracepoints without any events associated with them.
This allows the scheduler to keep flexibility without creating ABI as Trace
Events has proved to be ABIs. It provides the hooks, but out of tree module is
required to attach to these hooks and create events. Users can then customize
these events to their heart's content!

To further decouple from the kernel, we maintain our own helper functions and
use pahole to generate required definitions of private kernel structs like
struct rq.

We can generate these headers from BTF (/sys/kernel/btf/vmlinux) or from DWARF
if you're a developer and build your kernel with debuginfo enabled.

## Original development repo

[https://github.com/qais-yousef/tracepoints-helpers/tree/pelt-tps-v3-create-events](https://github.com/qais-yousef/tracepoints-helpers/tree/pelt-tps-v3-create-events)

# Requirements

- pahole v1.15 or above for vmlinux + DWARF. For vmlinux + BTF support you need
1.23 or above.

`sudo apt install dwarves`

## Warning

If you have pahole older than v1.23 and are using BTF, then the module will
compile but you most likely to get weird kernel crashes when loading the
module. There's an alignment problem and we'll end up accessing data from
misaligned offsets.

At the time of writing v1.23 is not released yet and you must compile it from
source to get it.

[https://github.com/acmel/dwarves](https://github.com/acmel/dwarves)

# Usage

## Building natively

### BTF requirements

The kernel you're running must have been compiled with these configs:

- CONFIG_DEBUG_INFO_BTF=y

### DWARF requirements

The kernel you're running must have been compiled with these configs:

- CONFIG_DEBUG_INFO=y
- CONFIG_DEBUG_INFO_REDUCE is not set

### Command

Override `VMLINUX` variable to point to your BTF/DWARF vmlinux.
Override `KERNEL_SRC` variable to point to your exported kernel headers.

Default values work on Ubuntu for vmlinux + BTF. Getting DWARF vmlinux is a bit
of a headache unless you're compiling your own kernel. BTF should be ON by
default on most distros now.

```
sudo apt install linux-headers-$(uname -r)
make
```

## Cross compile

### Using prebuilt kernel tree

Must compile with these configs:

- CONFIG_DEBUG_INFO=y
- CONFIG_DEBUG_INFO_REDUCE is not set

```
make KERNEL_SRC=path/to/prebuilt/kernel/tree ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
```

### Using BTF and CONFIG_IKHEADERS

	WIP: Not really tested.

If the system you're cross compiling for was built with

- CONFIG_DEBUG_INFO_BTF=y
- CONFIG_IKHEADERS

Then you can extract `/sys/kernel/btf/vmlinux` and
`/sys/kernel/kheaders.tar.xz` from the target device and use them to build.

You will need to override VMLINUX variable to point to your extracted BTF.

```
make VMLINUX=path/to/extracted/btf KERNEL_SRC=path/to/extracted/kheaders ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
```
