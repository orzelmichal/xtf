/**
 * @file arch/arm/include/arch/arm64/system.h
 */
#ifndef XTF_ARM64_SYSTEM_H
#define XTF_ARM64_SYSTEM_H

#define local_irq_disable() asm volatile ( "msr daifset, #2\n" ::: "memory" )
#define local_irq_enable()  asm volatile ( "msr daifclr, #2\n" ::: "memory" )

#define local_irq_save(flags)    \
({                               \
    asm volatile (               \
    "mrs %0, daif\n"             \
    "msr daifset, #2\n"          \
    : "=r" (flags) :: "memory"); \
})

#define local_irq_restore(flags) \
({                               \
    asm volatile (               \
    "msr daif, %0\n"             \
    :: "r" (flags) : "memory");  \
})

/* Access to system registers. */
#define write_sysreg(v, name)                                \
({                                                           \
    uint64_t r_ = (v);                                       \
    asm volatile("msr "STRINGIFY(name)", %0" :: "r" (r_));   \
})

#define read_sysreg(name)                                    \
({                                                           \
    uint64_t r_;                                             \
    asm volatile("mrs  %0, "STRINGIFY(name) : "=r" (r_));    \
    r_;                                                      \
})

#endif /* XTF_ARM64_SYSTEM_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
