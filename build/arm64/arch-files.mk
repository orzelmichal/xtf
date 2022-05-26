# Architecture specific files compiled and linked for arm64

# Include arm common files
include $(ROOT)/build/arm-common/arch-files.mk

# Specific files for arm64
obj-perenv += $(ROOT)/arch/arm/arm64/head.o
obj-perenv += $(ROOT)/arch/arm/arm64/cache.o
obj-perenv += $(ROOT)/arch/arm/arm64/hypercall.o
ifeq ($(CONFIG_GICV3), y)
obj-perenv += $(ROOT)/arch/arm/gicv3.o
else ifeq ($(CONFIG_GICV2), y)
obj-perenv += $(ROOT)/arch/arm/gicv2.o
endif
