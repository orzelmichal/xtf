# Architecture specific configuration for arm64

BASE_ARCH        := arm
ALL_ENVIRONMENTS := 64le

$(foreach env,$(ALL_ENVIRONMENTS),$(eval $(env)_guest := arm64))
$(foreach env,$(ALL_ENVIRONMENTS),$(eval $(env)_arch := arm64))

defcfg-arm64 := $(ROOT)/config/default-arm.cfg.in

COMMON_CFLAGS += -march=armv8-a

# Helpers to avoid using __aarch64__ or __arm__. These cannot be defined in
# a header arch/config.h as it is only included for per-env files.
COMMON_CFLAGS += -DCONFIG_ARM_64
COMMON_AFLAGS += -DCONFIG_ARM_64

# Include arm common makefile
include $(ROOT)/build/arm-common/arch-common.mk
