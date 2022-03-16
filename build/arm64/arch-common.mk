# Architecture specific configuration for arm64

BASE_ARCH        := arm
ALL_ENVIRONMENTS := mmu64le

# On arm64 all environments need to have MMU enabled
MMU_ENVIRONMENTS := $(ALL_ENVIRONMENTS)

$(foreach env,$(ALL_ENVIRONMENTS),$(eval $(env)_guest := arm64))
$(foreach env,$(ALL_ENVIRONMENTS),$(eval $(env)_arch := arm64))

defcfg-arm64 := $(ROOT)/config/default-arm.cfg.in

COMMON_CFLAGS += -march=armv8-a

# Prevent the compiler from using FP/ASIMD registers
COMMON_CFLAGS += -mgeneral-regs-only

# Include arm common makefile
include $(ROOT)/build/arm-common/arch-common.mk
