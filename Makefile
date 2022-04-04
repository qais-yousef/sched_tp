obj-m += sched_tp.o

EXTRA_CFLAGS = -I$(src)

VMLINUX_DEPS_UCLAMP_H = vmlinux_deps_uclamp.h
VMLINUX_DEPS_H = vmlinux_deps.h
VMLINUX_H = vmlinux.h

VMLINUX_DEPS_UCLAMP_TXT = vmlinux_deps_uclamp.txt
VMLINUX_DEPS_TXT = vmlinux_deps.txt
VMLINUX_TXT = vmlinux.txt

KERNEL_SRC ?= /usr/lib/modules/$(shell uname -r)/build

ifeq ($(wildcard $(KERNEL_SRC)/vmlinux), )
	VMLINUX ?= /sys/kernel/btf/vmlinux
else
	VMLINUX ?= $(KERNEL_SRC)/vmlinux
endif

all: $(VMLINUX_H)
	make -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	make -C $(KERNEL_SRC) M=$(PWD) clean
	rm -f $(VMLINUX_H) $(VMLINUX_DEPS_H) $(VMLINUX_DEPS_UCLAMP_H)

$(VMLINUX_DEPS_UCLAMP_H): $(VMLINUX_DEPS_UCLAMP_TXT) $(VMLINUX)
	@rm -f $@
	pahole -C file://vmlinux_deps_uclamp.txt $(VMLINUX) >> $@

$(VMLINUX_DEPS_H): $(VMLINUX_DEPS_TXT) $(VMLINUX)
	@rm -f $@
ifeq ($(shell pahole --version), v1.15)
	@echo "pahole version v1.15: applying workaround..."
	@echo "typedef int (*cpu_stop_fn_t)(void *arg);" > $@;
endif
	pahole -C file://vmlinux_deps.txt $(VMLINUX) >> $@

$(VMLINUX_H): $(VMLINUX_DEPS_UCLAMP_H) $(VMLINUX_DEPS_H) $(VMLINUX_TXT) $(VMLINUX)
	pahole -C file://vmlinux.txt $(VMLINUX) > $@
