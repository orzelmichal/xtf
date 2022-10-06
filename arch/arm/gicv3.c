/**
 * @file arch/arm/gicv3.c
 *
 * ARM Generic Interrupt Controller v3.
 */

#include <xtf/lib.h>
#include <xtf.h>
#include <arch/system.h>
#include <arch/io.h>
#include <arch/mm.h>
#include <arch/gic.h>
#include <arch/time.h>

struct gic {
    char *dist;
    char *rdist;
    char *rdist_sgi;
};

static struct gic gicv3;

static void gicv3_wait_rwp(void *base)
{
    while (io_readl(base + GICD_CTLR) & GICD_CTLR_RWP);
}

static void gicv3_enable_int(unsigned int intid)
{
    uint32_t mask = 1U << (intid % 32);

    if (intid < GIC_SPI_BASE)
        io_writel(mask, gicv3.rdist_sgi + GICD_ISENABLER + (intid / 32) * 4);
    else
        io_writel(mask, gicv3.dist + GICD_ISENABLER + (intid / 32) * 4);
}

static void gicv3_disable_int(unsigned int intid)
{
    uint32_t mask = 1U << (intid % 32);

    if (intid < GIC_SPI_BASE)
    {
        io_writel(mask, gicv3.rdist_sgi + GICD_ICENABLER + (intid / 32) * 4);
        gicv3_wait_rwp(gicv3.rdist_sgi);
    }
    else
    {
        io_writel(mask, gicv3.dist + GICD_ICENABLER + (intid / 32) * 4);
        gicv3_wait_rwp(gicv3.dist);
    }
}

static void gicv3_set_int_priority(unsigned int intid, unsigned int priority)
{
    if (intid < GIC_SPI_BASE)
        io_writeb(priority, gicv3.rdist_sgi + GICR_IPRIORITYR0 + intid);
    else
        io_writeb(priority, gicv3.dist + GICD_IPRIORITYR + intid);
}

static void gicv3_set_int_type(unsigned int intid, unsigned int type)
{
    void *base;
    uint32_t cfg, mask;

    /* SGI's are always edge-triggered. */
    if (intid < GIC_PPI_BASE)
        return;

    if (intid >= GIC_SPI_BASE)
        base = gicv3.dist + GICD_ICFGR + (intid / 16) * 4;
    else
        base = gicv3.rdist_sgi + GICR_ICFGR1 + (intid / 16) * 4;

    cfg = io_readl(base);

    mask = 2u << (2 * (intid % 16));
    if (type == IRQ_TYPE_LEVEL)
        cfg &= ~mask;
    else if (type == IRQ_TYPE_EDGE)
        cfg |= mask;

    io_writel(cfg, base);
}

static void gicv3_init_cpu(void)
{
    unsigned int i;

    /* Disable all SGIs, PPIs. */
    io_writel(GIC_32BIT_MASK, gicv3.rdist_sgi + GICR_ICENABLER0);

    gicv3_wait_rwp(gicv3.rdist);

    /* Configure SGIs/PPIs as non-secure Group-1. */
    io_writel(GIC_32BIT_MASK, gicv3.rdist_sgi + GICR_IGROUPR0);

    /* Set priority on PPI and SGI interrupts. */
    for (i = GIC_SGI_BASE; i < GIC_SPI_BASE; i += 4)
        io_writel(GIC_DEF_PRIORITY_X4,
                  gicv3.rdist_sgi + GICR_IPRIORITYR0 + (i / 4) * 4);

    /* Configure PPIs as level triggered. */
    io_writel(IRQ_TYPE_LEVEL, gicv3.rdist_sgi + GICD_ICFGR1);

    /* Make sure accesses to ICC_* at EL1 are not trapped to EL1. */
    write_sysreg(ICC_SRE_DIB | ICC_SRE_DFB | ICC_SRE_SRE, ICC_SRE_EL1);

    /* Set priority mask. */
    write_sysreg(GIC_IDLE_PRIORITY, ICC_PMR_EL1);

    /* Enable Group1 interrupts. */
    write_sysreg(1, ICC_IGRPEN1_EL1);

    isb();
}

static void gicv3_enable_rdist(void)
{
    uint32_t val;
    val = io_readl(gicv3.rdist + GICR_WAKER);

    if (!(val & GICR_WAKER_CA))
        return;

    val &= ~GICR_WAKER_PS;
    while (io_readl(gicv3.rdist + GICR_WAKER) & GICR_WAKER_CA);
}

static void gicv3_init_dist(void)
{
    unsigned int num_ints, i;

    /* Disable distributor. */
    io_writel(0, gicv3.dist + GICD_CTLR);
    gicv3_wait_rwp(gicv3.dist);

    num_ints = io_readl(gicv3.dist + GICD_TYPER);
    num_ints = 32 * ((num_ints & GICD_TYPE_LINES) + 1);

    /* Disable all SPIs. Configure SPIs as G1 NS. */
    for (i = GIC_SPI_BASE; i < num_ints; i += 32)
    {
        io_writel(GIC_32BIT_MASK, gicv3.dist + GICD_ICENABLER + (i / 32) * 4);
        io_writel(GIC_32BIT_MASK, gicv3.dist + GICD_IGROUPR + (i / 32) * 4);
    }

    gicv3_wait_rwp(gicv3.dist);

    /* Set priority on SPIs. */
    for (i = GIC_SPI_BASE; i < num_ints; i += 4)
        io_writel(GIC_DEF_PRIORITY_X4,
                  gicv3.dist + GICD_IPRIORITYR + (i / 4) * 4);

    /* Configure all SPIs as level triggered, active low. */
    for (i = GIC_SPI_BASE; i < num_ints; i += 16)
        io_writel(0, gicv3.dist + GICD_ICFGR + (i / 16) * 4);

    /* Enable distributor. */
    io_writel(GICD_CTLR_ARE | GICD_CTLR_EN_G1NS | GICD_CTLR_EN_G0,
              gicv3.dist + GICD_CTLR);
}

static void gicv3_eoi(unsigned int intid)
{
    dsb(sy);
    write_sysreg(intid, ICC_EOIR1_EL1);
}

static unsigned int gicv3_get_active_irq(void)
{
    return read_sysreg(ICC_IAR1_EL1);
}

static void gicv3_mapping(void)
{
    if (use_hardware_layout())
    {
        gicv3.dist = (char *)CONFIG_GICV3_DIST_ADDRESS;
        gicv3.rdist = (char *)CONFIG_GICV3_RDIST_ADDRESS;

        /* Make sure NULL addresses are not passed further. */
        if (!gicv3.dist || !gicv3.rdist)
            panic("Address of GICv3 distributor/redistributor is NULL.\n");
    }
    else
    {
        gicv3.dist = (char *)XEN_GUEST_GICD;
        gicv3.rdist = (char *)XEN_GUEST_GICR;
    }

    printk("GICv3: GICD @ %p, GICR @ %p\n", gicv3.dist, gicv3.rdist);

    gicv3.rdist_sgi = gicv3.rdist + GICR_SGI_BASE_OFF;

#ifdef CONFIG_MMU
    gicv3.dist = (char *)set_fixmap(FIXMAP_GICD, (paddr_t)gicv3.dist,
                                    DESC_PAGE_TABLE_DEV);
    gicv3.rdist = (char *)set_fixmap(FIXMAP_GICR, (paddr_t)gicv3.rdist,
                                     DESC_PAGE_TABLE_DEV);
    gicv3.rdist_sgi = (char *)set_fixmap(FIXMAP_GICS, (paddr_t)gicv3.rdist_sgi,
                                         DESC_PAGE_TABLE_DEV);
#endif
}

static void gicv3_handler(void)
{
    unsigned int id;

    /* Read the IAR to get the INTID of the interrupt taken. */
    id = gicv3_get_active_irq();

    switch(id)
    {
    case VTIMER_IRQ:
        vtimer_interrupt(id);
        break;
    default:
        printk("IRQ: Unexpected INTID %u\n", id);
    }

    /* Write EOIR to deactivate interrupt. */
    gicv3_eoi(id);
}

static void gicv3_init(void)
{
    gicv3_mapping();

    gicv3_init_dist();

    gicv3_enable_rdist();

    gicv3_init_cpu();
}

static struct gic_controller gicv3_controller = {
    .enable_int = gicv3_enable_int,
    .disable_int = gicv3_disable_int,
    .set_int_priority = gicv3_set_int_priority,
    .set_int_type = gicv3_set_int_type,
    .eoi = gicv3_eoi,
    .get_active_irq = gicv3_get_active_irq,
    .handler = gicv3_handler,
    .init = gicv3_init,
};

void gicv3_register(void)
{
    gic = &gicv3_controller;
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
