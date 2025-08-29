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

# Check if CONFIG_UCLAMP_TASK is enabled in kernel config
UCLAMP_ENABLED := $(shell grep -q "CONFIG_UCLAMP_TASK=y" $(KERNEL_SRC)/.config && echo "yes" || echo "no")

all: $(VMLINUX_H)
	make -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	make -C $(KERNEL_SRC) M=$(PWD) clean
	rm -f $(VMLINUX_H) $(VMLINUX_DEPS_H) $(VMLINUX_DEPS_UCLAMP_H)

$(VMLINUX_DEPS_UCLAMP_H): $(VMLINUX_DEPS_UCLAMP_TXT) $(VMLINUX)
ifeq ($(UCLAMP_ENABLED),yes)
	@rm -f $@
	pahole -C file://$(VMLINUX_DEPS_UCLAMP_TXT) $(VMLINUX) >> $@
else
	@echo "CONFIG_UCLAMP_TASK not enabled, skipping uclamp deps generation"
	@touch $@
endif

$(VMLINUX_DEPS_H): $(VMLINUX_DEPS_TXT) $(VMLINUX)
	@rm -f $@
ifeq ($(shell pahole --version), v1.15)
	@echo "pahole version v1.15: applying workaround..."
	@echo "typedef int (*cpu_stop_fn_t)(void *arg);" > $@;
endif
	pahole -C file://$(VMLINUX_DEPS_TXT) $(VMLINUX) >> $@

$(VMLINUX_H): $(VMLINUX_DEPS_UCLAMP_H) $(VMLINUX_DEPS_H) $(VMLINUX_TXT) $(VMLINUX)
	pahole -C file://$(VMLINUX_TXT) $(VMLINUX) > $@
