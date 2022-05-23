/**
 * @file arch/arm/include/arch/arm32/io.h
 */
#ifndef XTF_ARM32_IO_H
#define XTF_ARM32_IO_H

#include <arch/barrier.h>

/*
 * Generic IO read/write enforcing memory ordering.
 */
static inline uint32_t io_readl(const volatile void *addr)
{
    uint32_t val;
    asm volatile("ldr %1, %0\n": "+Qo" (*(volatile uint32_t*)addr), "=r" (val));
    rmb();
    return val;
}

static inline uint8_t io_readb(const volatile void *addr)
{
    uint8_t val;
    asm volatile("ldrb %1, %0\n": "+Qo" (*(volatile uint8_t*)addr), "=r" (val));
    rmb();
    return val;
}

static inline void io_writel(uint32_t val, volatile void *addr)
{
    wmb();
    asm volatile("str %1, %0\n": "+Qo" (*(volatile uint32_t*)addr) : "r" (val));
}

static inline void io_writeb(uint8_t val, volatile void *addr)
{
    wmb();
    asm volatile("strb %1, %0\n": "+Qo" (*(volatile uint8_t*)addr) : "r" (val));
}

#endif /* XTF_ARM32_IO_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
