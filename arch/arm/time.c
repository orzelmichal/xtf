/**
 * @file arch/arm/time.c
 */

#include <arch/time.h>
#include <arch/gic.h>

/* Timer frequency in HZ. */
uint32_t timer_frequency;

static uint64_t muldiv64(uint64_t a, uint32_t b, uint32_t c)
{
    union {
        uint64_t ll;
        struct {
            uint32_t low, high;
        } l;
    } u, res;
    uint64_t rl, rh;

    u.ll = a;
    rl = (uint64_t)u.l.low * (uint64_t)b;
    rh = (uint64_t)u.l.high * (uint64_t)b;
    rh += (rl >> 32);
    res.l.high = rh / c;
    res.l.low = (((rh % c) << 32) + (rl & 0xffffffff)) / c;

    return res.ll;
}

uint64_t ticks_to_ns(uint64_t ticks)
{
    return muldiv64(ticks, SEC_TO_NS(1), timer_frequency);
}

uint64_t ns_to_ticks(uint64_t ns)
{
    return muldiv64(ns, timer_frequency, SEC_TO_NS(1));
}

/* Mark this as weak to allow tests to modify it to suite their needs. */
__weak void vtimer_interrupt(unsigned int id)
{
    printk("IRQ: Virtual timer\n");
    timer_set_virtual_ctrl(~CNTX_CTL_ENABLE);
}

void time_init(void)
{
    timer_frequency = timer_get_freq();

    printk("Timer frequency: %u[Hz]\n", timer_frequency);

    if (gic_available())
    {
        /* Enable virtual timer interrupt */
        gic_set_int_priority(VTIMER_IRQ, 0);
        gic_enable_int(VTIMER_IRQ);
    }
}

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
