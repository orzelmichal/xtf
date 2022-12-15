/**
 * @file tests/hyp-xen-version/main.c
 * @ref test-hyp-xen-version
 *
 * @page test-hyp-xen-version Hypercall xen_version
 *
 * Functional testing of xen_version hypercall.
 *
 * @see tests/hyp-xen-version/main.c
 */
#include <xtf.h>

const char test_title[] = "Hypercall xen_version testing";

#define INVALID_CMD -1

void test_main(void)
{
    int ret;

    printk("Checking XENVER_version:\n");
    {
        /*
        * Version is returned directly in format: ((major << 16) | minor),
        * so no need to check the return value for an error.
        */
        ret = hypercall_xen_version(XENVER_version, NULL);
        printk(" version: %u.%u\n", ret >> 16, ret & 0xFFFF);
    }

    printk("Checking XENVER_extraversion:\n");
    {
        xen_extraversion_t xen_ev;
        memset(&xen_ev, 0, sizeof(xen_ev));

        ret = hypercall_xen_version(XENVER_extraversion, xen_ev);
        if ( ret < 0 )
            return xtf_error("Error %d\n", ret);

        printk(" extraversion: %s\n", xen_ev);
    }

    printk("Checking XENVER_compile_info:\n");
    {
        xen_compile_info_t xen_ci;
        memset(&xen_ci, 0, sizeof(xen_ci));

        ret = hypercall_xen_version(XENVER_compile_info, &xen_ci);
        if ( ret < 0 )
            return xtf_error("Error %d\n", ret);

        printk(" compiler:       %s\n", xen_ci.compiler);
        printk(" compile_by:     %s\n", xen_ci.compile_by);
        printk(" compile_domain: %s\n", xen_ci.compile_domain);
        printk(" compile_date:   %s\n", xen_ci.compile_date);
    }

    printk("Checking XENVER_changeset:\n");
    {
        xen_changeset_info_t xen_cs;
        memset(&xen_cs, 0, sizeof(xen_cs));

        ret = hypercall_xen_version(XENVER_changeset, &xen_cs);
        if ( ret < 0 )
            return xtf_error("Error %d\n", ret);

        printk(" changeset: %s\n", xen_cs);
    }

    printk("Checking XENVER_get_features:\n");
    {
        for ( unsigned int i = 0; i < XENFEAT_NR_SUBMAPS; i++ )
        {
            xen_feature_info_t xen_fi = { .submap_idx = i };

            ret = hypercall_xen_version(XENVER_get_features, &xen_fi);
            if ( ret < 0 )
                return xtf_error("Error %d for submap[%u]\n", ret, i);

            printk(" submap[%u]: %#x\n", i, xen_fi.submap);
        }
    }

    printk("Checking invalid command:\n");
    {
        /* Invalid cmd should result in returning -ENOSYS. */
        ret = hypercall_xen_version(INVALID_CMD, NULL);
        if ( ret != -ENOSYS )
            return xtf_error("Error %d\n", ret);

        printk(" ok\n");
    }

    xtf_success(NULL);
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
