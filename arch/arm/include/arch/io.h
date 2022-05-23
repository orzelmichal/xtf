/**
 * @file arch/arm/include/arch/io.h
 */
#ifndef XTF_ARM_IO_H
#define XTF_ARM_IO_H

#ifdef CONFIG_ARM_64
#include <arch/arm64/io.h>
#else
#include <arch/arm32/io.h>
#endif

#endif /* XTF_ARM_IO_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
