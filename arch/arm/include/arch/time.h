/**
 * @file arch/arm/include/arch/time.h
 */
#ifndef XTF_ARM_TIME_H
#define XTF_ARM_TIME_H

#include <xtf/lib.h>
#include <arch/system.h>

#define CNTX_CTL_ENABLE (1 << 0)
#define CNTX_CTL_MASK   (1 << 1)
#define CNTX_CTL_STATUS (1 << 2)

#define VTIMER_IRQ       27
#define SEC_TO_NS(s)    (1000000000UL * (s))

extern uint32_t timer_frequency;

static inline void timer_set_virtual_cval(uint64_t cval)
{
    write_sysreg(cval, CNTV_CVAL_EL0);
}

static inline void timer_set_virtual_tval(uint32_t tval)
{
    write_sysreg(tval, CNTV_TVAL_EL0);
}

static inline void timer_set_virtual_ctrl(uint32_t val)
{
    write_sysreg(val, CNTV_CTL_EL0);
}

static inline uint64_t timer_get_virtual_count(void)
{
    return read_sysreg(CNTVCT_EL0);
}

static inline uint32_t timer_get_freq(void)
{
    return read_sysreg(CNTFRQ_EL0);
}

uint64_t ticks_to_ns(uint64_t ticks);
uint64_t ns_to_ticks(uint64_t ns);
void vtimer_interrupt(unsigned int id);

#ifdef CONFIG_ARM_64
void time_init(void);
#else
/* Currently there is no timer support on arm32. */
static inline void time_init(void) {}
#endif

#endif /* XTF_ARM_TIME_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
