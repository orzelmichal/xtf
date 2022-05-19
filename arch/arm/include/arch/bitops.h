/**
 * @file arch/arm/include/arch/bitops.h
 *
 * Low level bit operations.
 */
#ifndef XTF_ARM_BITOPS_H
#define XTF_ARM_BITOPS_H

#include <xtf/lib.h>
#include <arch/system.h>

#define BITS_PER_WORD 32
#define BIT_WORD(bit) ((bit) / BITS_PER_WORD)
#define BIT_MASK(bit) (1UL << ((bit) % BITS_PER_WORD))

/* Non-atomic operations. */
static inline bool test_bit(unsigned int bit, const void *addr)
{
    return (((const unsigned int *)addr)[BIT_WORD(bit)]) & BIT_MASK(bit);
}

static inline bool _test_and_set_bit(unsigned int bit, volatile void *addr)
{
    volatile unsigned int *word =
        ((volatile unsigned int *)addr) + BIT_WORD(bit);
    unsigned int mask = BIT_MASK(bit);
    bool ret = (*word & mask);

    *word |= mask;

    return ret;
}

static inline bool _test_and_change_bit(unsigned int bit, volatile void *addr)
{
    volatile unsigned int *word =
        ((volatile unsigned int *)addr) + BIT_WORD(bit);
    unsigned int mask = BIT_MASK(bit);
    bool ret = (*word & mask);

    *word ^= mask;

    return ret;
}

static inline bool _test_and_clear_bit(unsigned int bit, volatile void *addr)
{
    volatile unsigned int *word =
        ((volatile unsigned int *)addr) + BIT_WORD(bit);
    unsigned int mask = BIT_MASK(bit);
    bool ret = (*word & mask);

    *word &= ~mask;

    return ret;
}

/* Atomic operations. */
static inline bool test_and_set_bit(unsigned int bit, volatile void *addr)
{
    unsigned long flags;
    bool ret;

    local_irq_save(flags);
    ret = _test_and_set_bit(bit, addr);
    local_irq_restore(flags);

    return ret;
}

static inline bool test_and_change_bit(unsigned int bit, volatile void *addr)
{
    unsigned long flags;
    bool ret;

    local_irq_save(flags);
    ret = _test_and_change_bit(bit, addr);
    local_irq_restore(flags);

    return ret;
}

static inline bool test_and_clear_bit(unsigned int bit, volatile void *addr)
{
    unsigned long flags;
    bool ret;

    local_irq_save(flags);
    ret = _test_and_clear_bit(bit, addr);
    local_irq_restore(flags);

    return ret;
}

#endif /* XTF_ARM_BITOPS_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
