obj-m += sched_tp.o

ifneq ($(KERNELRELEASE),)
# Module build inside kernel build system
ccflags-y += -I$(M) -I$(src)
else
# External module build

# Compiler selection - can be overridden: make CC=clang
# Default to clang if available, fallback to gcc
ifeq ($(origin CC),default)
CC := $(shell command -v clang >/dev/null 2>&1 && echo clang || echo gcc)
endif

# Build type: debug, release, or custom
BUILD_TYPE ?= release

# Compiler-specific flags
ifeq ($(CC),clang)
	COMPILER_FLAGS = -Wno-gnu-variable-sized-type-not-at-end \
			 -Wno-address-of-packed-member \
			 -Wno-format-invalid-specifier \
			 -Wno-gnu \
			 -Wno-tautological-compare
	EXTRA_CFLAGS += $(COMPILER_FLAGS)
else ifeq ($(findstring gcc,$(CC)),gcc)
	COMPILER_FLAGS = -Wno-packed-not-aligned \
			 -Wno-stringop-truncation \
			 -Wno-format-overflow \
			 -Wno-restrict
	EXTRA_CFLAGS += $(COMPILER_FLAGS)
endif

# Build type specific flags
ifeq ($(BUILD_TYPE),debug)
	EXTRA_CFLAGS += -DDEBUG -g -O0 -fno-omit-frame-pointer
else ifeq ($(BUILD_TYPE),release)
	EXTRA_CFLAGS += -DNDEBUG -O2
else ifeq ($(BUILD_TYPE),aggressive)
	EXTRA_CFLAGS += -DNDEBUG -O3 -funroll-loops
endif

# Additional configurable flags  
EXTRA_CFLAGS += -I$(PWD)
ccflags-y += -I$(PWD)

# Architecture-specific optimizations
ifneq ($(ARCH_OPTS),)
	EXTRA_CFLAGS += $(ARCH_OPTS)
endif

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

# Default target
all: info $(VMLINUX_H)
	make -C $(KERNEL_SRC) M=$(PWD) modules CC=$(CC)

# Build targets for different configurations
debug:
	$(MAKE) BUILD_TYPE=debug

release:
	$(MAKE) BUILD_TYPE=release

aggressive:
	$(MAKE) BUILD_TYPE=aggressive

# Compiler-specific targets
gcc:
	$(MAKE) CC=gcc

clang:
	$(MAKE) CC=clang

# Combined targets
debug-gcc:
	$(MAKE) BUILD_TYPE=debug CC=gcc

debug-clang:
	$(MAKE) BUILD_TYPE=debug CC=clang

release-gcc:
	$(MAKE) BUILD_TYPE=release CC=gcc

release-clang:
	$(MAKE) BUILD_TYPE=release CC=clang

# Show build configuration
info:
	@echo "=== Build Configuration ==="
	@echo "Compiler: $(CC)"
	@echo "Build Type: $(BUILD_TYPE)"
	@echo "Compiler Flags: $(COMPILER_FLAGS)"
	@echo "Extra CFlags: $(EXTRA_CFLAGS)"
	@echo "Kernel Source: $(KERNEL_SRC)"
	@echo "UClamp Enabled: $(UCLAMP_ENABLED)"
	@echo "=========================="

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build with default settings (auto-detect compiler)"
	@echo "  debug         - Build with debug flags (-g -O0)"
	@echo "  release       - Build with release flags (-O2)"
	@echo "  aggressive    - Build with aggressive optimization (-O3)"
	@echo ""
	@echo "Compiler selection:"
	@echo "  gcc           - Force GCC compilation"
	@echo "  clang         - Force Clang compilation"
	@echo ""
	@echo "Combined targets:"
	@echo "  debug-gcc     - Debug build with GCC"
	@echo "  debug-clang   - Debug build with Clang"
	@echo "  release-gcc   - Release build with GCC"
	@echo "  release-clang - Release build with Clang"
	@echo ""
	@echo "Other targets:"
	@echo "  info          - Show current build configuration"
	@echo "  clean         - Clean build artifacts"
	@echo "  help          - Show this help"
	@echo ""
	@echo "Variables you can override:"
	@echo "  CC=<compiler>           - Compiler to use (gcc/clang)"
	@echo "  BUILD_TYPE=<type>       - Build type (debug/release/aggressive)"
	@echo "  ARCH_OPTS=<flags>       - Architecture-specific flags"
	@echo "  KERNEL_SRC=<path>       - Kernel source directory"
	@echo ""
	@echo "Examples:"
	@echo "  make CC=clang BUILD_TYPE=debug"
	@echo "  make release-clang ARCH_OPTS='-march=native'"

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
	pahole -C file://$(VMLINUX_DEPS_TXT) --skip_missing $(VMLINUX) >> $@

$(VMLINUX_H): $(VMLINUX_DEPS_UCLAMP_H) $(VMLINUX_DEPS_H) $(VMLINUX_TXT) $(VMLINUX)
	pahole -C file://$(VMLINUX_TXT) --skip_missing $(VMLINUX) > $@

# End of external module build
endif

# Declare phony targets
.PHONY: all debug release aggressive gcc clang debug-gcc debug-clang \
	release-gcc release-clang info help clean
