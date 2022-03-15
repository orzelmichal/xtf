# Architecture specific configuration for arm32

BASE_ARCH        := arm
ALL_ENVIRONMENTS := 32le

$(foreach env,$(ALL_ENVIRONMENTS),$(eval $(env)_guest := arm32))
$(foreach env,$(ALL_ENVIRONMENTS),$(eval $(env)_arch := arm32))

defcfg-arm32 := $(ROOT)/config/default-arm.cfg.in

COMMON_CFLAGS += -march=armv7ve

# Helpers to avoid using __aarch64__ or __arm__. These cannot be defined in
# a header arch/config.h as it is only included for per-env files.
COMMON_CFLAGS += -DCONFIG_ARM_32
COMMON_AFLAGS += -DCONFIG_ARM_32

# Include arm common makefile
include $(ROOT)/build/arm-common/arch-common.mk
