# Common files compiled and linked for arm

obj-perbits += $(ROOT)/common/console.o
obj-perbits += $(ROOT)/common/lib.o
obj-perbits += $(ROOT)/common/libc/stdio.o
obj-perbits += $(ROOT)/common/libc/string.o
obj-perbits += $(ROOT)/common/libc/vsnprintf.o
obj-perbits += $(ROOT)/common/report.o
obj-perbits += $(ROOT)/common/setup.o
obj-perbits += $(ROOT)/common/xenbus.o
obj-perbits += $(ROOT)/common/weak-defaults.o

obj-perenv += $(ROOT)/arch/arm/setup.o
obj-perenv += $(ROOT)/arch/arm/traps.o
