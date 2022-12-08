/**
 * @file arch/arm/arm64/gicv2.c
 *
 * ARM Generic Interrupt Controller v2.
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
    char *cpu;
};

static struct gic gicv2;

/*
 * TODO: Some code is generic between gic v3 and v2 and might be possible to
 * put in generic gic implementation (int_enable, handle, etc).
 */

static void gicv2_enable_int(unsigned int intid)
{
    uint32_t mask = 1U << (intid % 32);

    io_writel(mask, gicv2.dist + GICD_ISENABLER + (intid / 32) * 4);
}

static void gicv2_disable_int(unsigned int intid)
{
    uint32_t mask = 1U << (intid % 32);

    io_writel(mask, gicv2.dist + GICD_ICENABLER + (intid / 32) * 4);
}

static void gicv2_set_int_priority(unsigned int intid, unsigned int priority)
{
    io_writeb(priority, gicv2.dist + GICD_IPRIORITYR + intid);
}

static void gicv2_set_int_type(unsigned int intid, unsigned int type)
{
    void *base;
    uint32_t cfg, mask;

    /* SGI's are always edge-triggered */
    if (intid < GIC_PPI_BASE)
        return;

    base = gicv2.dist + GICD_ICFGR + (intid / 16) * 4;

    cfg = io_readl(base);

    mask = 2u << (2 * (intid % 16));
    if (type & IRQ_TYPE_LEVEL)
        cfg &= ~mask;
    else if (type & IRQ_TYPE_EDGE)
        cfg |= mask;

    io_writel(cfg, base);
}

static void gicv2_init_cpu(void)
{
    /* Disable all SGIs, PPIs. */
    io_writel(GIC_32BIT_MASK, gicv2.dist + GICD_ICENABLER);

    /* Set priority mask. */
    io_writel(GIC_IDLE_PRIORITY, gicv2.cpu + GICC_PMR);

    /* Enable Group1 interrupts. */
    io_writel(1, gicv2.cpu + GICC_CTLR);

    isb();
}

static void gicv2_init_dist(void)
{
    unsigned int num_ints, i;

    /* Disable distributor. */
    io_writel(0, gicv2.dist + GICD_CTLR);

    num_ints = io_readl(gicv2.dist + GICD_TYPER);
    num_ints = 32 * ((num_ints & GICD_TYPE_LINES) + 1);

    /* Disable all SPIs. Configure SPIs as G1 NS. */
    for (i = GIC_SPI_BASE; i < num_ints; i += 32)
    {
        io_writel(GIC_32BIT_MASK, gicv2.dist + GICD_ICENABLER + (i / 32) * 4);
        io_writel(GIC_32BIT_MASK, gicv2.dist + GICD_IGROUPR + (i / 32) * 4);
    }

    /* Set priority on SPIs. */
    for (i = GIC_SPI_BASE; i < num_ints; i += 4)
        io_writel(GIC_DEF_PRIORITY_X4,
                  gicv2.dist + GICD_IPRIORITYR + (i / 4) * 4);

    /* Configure all SPIs as level triggered, active low. */
    for (i = GIC_SPI_BASE; i < num_ints; i += 16)
        io_writel(0, gicv2.dist + GICD_ICFGR + (i / 16) * 4);

    /* Enable distributor. */
    io_writel(GICD_CTLR_ARE | GICD_CTLR_EN_G1NS | GICD_CTLR_EN_G0,
              gicv2.dist + GICD_CTLR);
}

static void gicv2_eoi(unsigned int intid)
{
    dsb(sy);
    io_writel(intid, gicv2.cpu + GICC_EOIR);
}

static unsigned int gicv2_get_active_irq(void)
{
    return io_readl(gicv2.cpu + GICC_IAR);
}

static void gicv2_mapping(void)
{
    if (isinitdomain)
    {
        gicv2.dist = (char *)CONFIG_GICV2_DIST_ADDRESS;
        gicv2.cpu = (char *)CONFIG_GICV2_CPU_ADDRESS;

        /* Make sure NULL addresses are not passed further. */
        if (!gicv2.dist || !gicv2.cpu)
            panic("Address of GICv2 distributor/cpu interface is NULL.\n"
                  "When using XTF as dom0, specify proper addresses.\n");
    }
    else
    {
        gicv2.dist = (char *)XEN_GUEST_GICD;
        gicv2.cpu = (char *)XEN_GUEST_GICC;
    }

    printk("GICv2: GICD @ %p, GICC @ %p\n", gicv2.dist, gicv2.cpu);

#ifdef CONFIG_MMU
    gicv2.dist = (char *)set_fixmap(FIXMAP_GICD, (paddr_t)gicv2.dist,
                                    DESC_PAGE_TABLE_DEV);
    gicv2.cpu = (char *)set_fixmap(FIXMAP_GICC, (paddr_t)gicv2.cpu,
                                     DESC_PAGE_TABLE_DEV);
#endif
}

static void gicv2_handler(void)
{
    unsigned int id;

    /* Read the IAR to get the INTID of the interrupt taken. */
    id = gicv2_get_active_irq();

    switch(id)
    {
    case VTIMER_IRQ:
        vtimer_interrupt(id);
        break;
    default:
        printk("IRQ: Unexpected INTID %u\n", id);
    }

    /* Write EOIR to deactivate interrupt. */
    gicv2_eoi(id);
}

static void gicv2_init(void)
{
    gicv2_mapping();

    gicv2_init_dist();

    gicv2_init_cpu();
}

static struct gic_controller gicv2_controller = {
    .enable_int = gicv2_enable_int,
    .disable_int = gicv2_disable_int,
    .set_int_priority = gicv2_set_int_priority,
    .set_int_type = gicv2_set_int_type,
    .eoi = gicv2_eoi,
    .get_active_irq = gicv2_get_active_irq,
    .handler = gicv2_handler,
    .init = gicv2_init,
};

void gicv2_register(void)
{
    gic = &gicv2_controller;
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
