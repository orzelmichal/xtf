/**
 * @file arch/arm/include/arch/gic.h
 */
#ifndef XTF_ARM_GIC_H
#define XTF_ARM_GIC_H

#include <arch/barrier.h>

/*
 * Xen vGIC mappings.
 * We need to rely on them as there is no DTB support in XTF.
 */
#define XEN_GUEST_GICD      0x3001000U
#define XEN_GUEST_GICC      0x3002000U
#define XEN_GUEST_GICR      0x3020000U

/* SGI base is at 64K offset from Redistributor. */
#define GICR_SGI_BASE_OFF   0x10000

#define GIC_SGI_BASE        0
#define GIC_PPI_BASE        16
#define GIC_SPI_BASE        32
#define GIC_DEF_PRIORITY_X4 0xa0a0a0a0U
#define GIC_IDLE_PRIORITY   0xff
#define GIC_32BIT_MASK      0xffffffffU

/* IRQ types. */
#define IRQ_TYPE_LEVEL      0x0
#define IRQ_TYPE_EDGE       0x1

/* GIC Distributor Interface. */
#define GICD_CTLR           (0x000)
#define GICD_TYPER          (0x004)
#define GICD_IIDR           (0x008)
#define GICD_IGROUPR        (0x080)
#define GICD_IGROUPRN       (0x0FC)
#define GICD_ISENABLER      (0x100)
#define GICD_ISENABLERN     (0x17C)
#define GICD_ICENABLER      (0x180)
#define GICD_ICENABLERN     (0x1fC)
#define GICD_ISPENDR        (0x200)
#define GICD_ISPENDRN       (0x27C)
#define GICD_ICPENDR        (0x280)
#define GICD_ICPENDRN       (0x2FC)
#define GICD_ISACTIVER      (0x300)
#define GICD_ISACTIVERN     (0x37C)
#define GICD_ICACTIVER      (0x380)
#define GICD_ICACTIVERN     (0x3FC)
#define GICD_IPRIORITYR     (0x400)
#define GICD_IPRIORITYRN    (0x7F8)
#define GICD_ITARGETSR      (0x800)
#define GICD_ITARGETSR7     (0x81C)
#define GICD_ITARGETSR8     (0x820)
#define GICD_ITARGETSRN     (0xBF8)
#define GICD_ICFGR          (0xC00)
#define GICD_ICFGR1         (0xC04)
#define GICD_ICFGR2         (0xC08)
#define GICD_ICFGRN         (0xCFC)
#define GICD_NSACR          (0xE00)
#define GICD_NSACRN         (0xEFC)
#define GICD_SGIR           (0xF00)
#define GICD_CPENDSGIR      (0xF10)
#define GICD_CPENDSGIRN     (0xF1C)
#define GICD_SPENDSGIR      (0xF20)
#define GICD_SPENDSGIRN     (0xF2C)
#define GICD_ICPIDR2        (0xFE8)

/* GIC Redistributor Interface. */
#define GICR_WAKER          (0x0014)
#define GICR_IGROUPR0       (0x0080)
#define GICR_ISENABLER0     (0x0100)
#define GICR_ICENABLER0     (0x0180)
#define GICR_ISPENDR0       (0x0200)
#define GICR_ICPENDR0       (0x0280)
#define GICR_ISACTIVER0     (0x0300)
#define GICR_ICACTIVER0     (0x0380)
#define GICR_IPRIORITYR0    (0x0400)
#define GICR_IPRIORITYR7    (0x041C)
#define GICR_ICFGR0         (0x0C00)
#define GICR_ICFGR1         (0x0C04)
#define GICR_IGRPMODR0      (0x0D00)
#define GICR_NSACR          (0x0E00)

/* GIC CPU Interface. */
#define GICC_CTLR           (0x0000)
#define GICC_PMR            (0x0004)
#define GICC_BPR            (0x0008)
#define GICC_IAR            (0x000C)
#define GICC_EOIR           (0x0010)
#define GICC_RPR            (0x0014)
#define GICC_HPPIR          (0x0018)
#define GICC_APR            (0x00D0)
#define GICC_NSAPR          (0x00E0)
#define GICC_IIDR           (0x00FC)
#define GICC_DIR            (0x1000)

/* System register interface to GICv3. */
#define ICC_IGRPEN1_EL1     S3_0_C12_C12_7
#define ICC_SGI1R           S3_0_C12_C11_5
#define ICC_SRE_EL1         S3_0_C12_C12_5
#define ICC_CTLR_EL1        S3_0_C12_C12_4
#define ICC_PMR_EL1         S3_0_C4_C6_0
#define ICC_RPR_EL1         S3_0_C12_C11_3
#define ICC_IGRPEN0_EL1     S3_0_C12_C12_6
#define ICC_HPPIR0_EL1      S3_0_C12_C8_2
#define ICC_HPPIR1_EL1      S3_0_C12_C12_2
#define ICC_IAR0_EL1        S3_0_C12_C8_0
#define ICC_IAR1_EL1        S3_0_C12_C12_0
#define ICC_EOIR0_EL1       S3_0_C12_C8_1
#define ICC_EOIR1_EL1       S3_0_C12_C12_1
#define ICC_SGI0R_EL1       S3_0_C12_C11_7
#define ICC_BPR1_EL1        S3_0_C12_C12_3
#define ICC_DIR_EL1         S3_0_C12_C11_1
#define ICC_SGI1R_EL1       S3_0_C12_C11_5

/* GICR_WAKER */
#define GICR_WAKER_CA       (1 << 2)
#define GICR_WAKER_PS       (1 << 1)

#define GICD_TYPE_LINES     0x1f
#define GICD_CTLR_RWP       (1UL << 31)
#define GICD_CTLR_ARE       (1U << 4)
#define GICD_CTLR_EN_G1NS   (1U << 1)
#define GICD_CTLR_EN_G0     (1U << 0)

/* Interrupt Controller System Register Enable register. */
#define ICC_SRE_DIB         (1 << 2)
#define ICC_SRE_DFB         (1 << 1)
#define ICC_SRE_SRE         (1 << 0)

struct gic_controller {
    void (*enable_int)(unsigned int);
    void (*disable_int)(unsigned int);
    void (*set_int_priority)(unsigned int,unsigned int);
    void (*set_int_type)(unsigned int,unsigned int);
    void (*eoi)(unsigned int);
    unsigned int (*get_active_irq)(void);
    void (*handler)(void);
    void (*init)(void);
};

extern struct gic_controller *gic;

static inline bool gic_available(void)
{
    return (gic != NULL);
}

static inline void gic_enable_int(unsigned int intid)
{
    gic->enable_int(intid);
}

static inline void gic_disable_int(unsigned int intid)
{
    gic->disable_int(intid);
}

static inline void gic_set_int_priority(unsigned int intid,
                                        unsigned int priority)
{
    gic->set_int_priority(intid, priority);
}

static inline void gic_set_int_type(unsigned int intid, unsigned int type)
{
    gic->set_int_type(intid, type);
}

static inline void gic_eoi(unsigned int intid)
{
    gic->eoi(intid);
}

static inline unsigned int gic_get_active_irq(void)
{
    return gic->get_active_irq();
}

static inline void gic_handler(void)
{
    gic->handler();
}

static inline void gic_init(void)
{
    gic->init();
}

#ifdef CONFIG_GICV3
void gicv3_register(void);
#endif

#ifdef CONFIG_GICV2
void gicv2_register(void);
#endif

static inline void gic_register(void)
{
#ifdef CONFIG_GICV3
    gicv3_register();
#else
#ifdef CONFIG_GICV2
    gicv2_register();
#endif
#endif
};

#endif /* XTF_ARM_GIC_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
