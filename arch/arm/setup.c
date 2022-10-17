/**
 * @file arch/arm/setup.c
 *
 * Early bringup code for arm.
 */
#include <xtf.h>
#include <xtf/lib.h>
#include <xtf/hypercall.h>
#include <arch/system.h>
#include <arch/gic.h>
#include <arch/time.h>

/* Structure to store boot arguments. */
struct init_data
{
    uint64_t phys_offset;
    void *fdt;
} boot_data;

const char environment_description[] = ENVIRONMENT_DESCRIPTION;

/* Supported feature flags obtained by XENVER_get_features. */
struct feature_flags xtf_features;

/* GIC controller. */
struct gic_controller *gic;

shared_info_t __page_aligned_bss shared_info;

static void setup_pv_console(void)
{
    xencons_interface_t *cons_ring;
    evtchn_port_t cons_evtchn;
    uint64_t raw_ev = 0, raw_pfn = 0, phys;

    if ( hvm_get_param(HVM_PARAM_CONSOLE_EVTCHN, &raw_ev) != 0 ||
         hvm_get_param(HVM_PARAM_CONSOLE_PFN, &raw_pfn) != 0 )
        return;

    /* 0 - uninitialized value. Skip PV console setup. */
    if ( !raw_pfn )
        return;

    cons_evtchn = raw_ev;
    phys = pfn_to_phys(raw_pfn);
    cons_ring = (xencons_interface_t *)set_fixmap(FIXMAP_PV_CONSOLE, phys,
                                                  DESC_PAGE_TABLE_MEM);

    init_pv_console(cons_ring, cons_evtchn);
}

static void map_shared_info(void)
{
    int ret;
    struct xen_add_to_physmap xatp =
    {
        .domid = DOMID_SELF,
        .space = XENMAPSPACE_shared_info,
        .idx = 0,
        .gfn = virt_to_pfn(&shared_info),
    };

    ret = hypercall_memory_op(XENMEM_add_to_physmap, &xatp);

    if ( ret )
        panic("Failed to map shared_info. ret=%d\n", ret);
}

static void get_feature_flags(void)
{
    xen_feature_info_t fi;
    int ret;

    fi.submap_idx = 0;
    ret = hypercall_xen_version(XENVER_get_features, &fi);

    if ( ret )
        panic("Failed to obtain Xen features. ret=%d\n", ret);

    if ( fi.submap & (1 << XENFEAT_dom0) )
        xtf_features.isinitdomain = true;

    if ( fi.submap & (1 << XENFEAT_direct_mapped) )
        xtf_features.isdirectmap = true;

    printk("Feature flags:\n");
    printk(" isinitdomain: %s\n", xtf_features.isinitdomain ? "true" : "false");
    printk(" isdirectmap: %s\n", xtf_features.isdirectmap ? "true" : "false");
}

static void setup_console(void)
{
    /* Use Xen console to print messages */
    register_console_callback(hypercall_console_write);
}

void arch_setup(void)
{
    setup_mm(boot_data.phys_offset);

    setup_console();
    get_feature_flags();

    /* Use PV console when running as a guest */
    if ( !xtf_features.isinitdomain )
        setup_pv_console();

    map_shared_info();

    gic_register();

    /* Initialize GIC and enable interrupts. */
    if ( gic_available() )
    {
        gic_init();
        local_irq_enable();
    }

    /* Initialize timer interface. */
    time_init();
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