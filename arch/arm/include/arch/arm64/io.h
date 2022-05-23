/**
 * @file arch/arm/include/arch/arm64/io.h
 */
#ifndef XTF_ARM64_IO_H
#define XTF_ARM64_IO_H

#include <arch/barrier.h>

/*
 * Generic IO read/write enforcing memory ordering.
 */
static inline uint64_t io_readq(const volatile void *addr)
{
    uint64_t val;
    asm volatile("ldr %0, [%1]\n" : "=r" (val) : "r" (addr));
    rmb();
    return val;
}

static inline uint32_t io_readl(const volatile void *addr)
{
    uint32_t val;
    asm volatile("ldr %w0, [%1]\n" : "=r" (val) : "r" (addr));
    rmb();
    return val;
}

static inline uint8_t io_readb(const volatile void *addr)
{
    uint8_t val;
    asm volatile("ldrb %w0, [%1]\n" : "=r" (val) : "r" (addr));
    rmb();
    return val;
}

static inline void io_writeq(uint64_t val, volatile void *addr)
{
    wmb();
    asm volatile("str %0, [%1]\n" : : "r" (val), "r" (addr));
}

static inline void io_writel(uint32_t val, volatile void *addr)
{
    wmb();
    asm volatile("str %w0, [%1]\n" : : "r" (val), "r" (addr));
}

static inline void io_writeb(uint8_t val, volatile void *addr)
{
    wmb();
    asm volatile("strb %w0, [%1]\n" : : "r" (val), "r" (addr));
}

#endif /* XTF_ARM64_IO_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
