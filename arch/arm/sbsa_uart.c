/**
 * @file arch/arm/sbsa_uart.c
 *
 * ARM SBSA UART driver to be used with Xen virtual PL011 UART.
 *
 * Only TX support in polling mode.
 */

#include <xtf.h>
#include <arch/sbsa_uart.h>
#include <arch/io.h>

static char *sbsa_uart_base;

static void sbsa_uart_putc(char c)
{
    uint32_t busy;

    do {
        busy = io_readl(sbsa_uart_base + SBSA_UART_FR);
    } while ( busy & SBSA_UART_FR_BUSY );

    io_writel((uint32_t)(unsigned char)c, sbsa_uart_base + SBSA_UART_DR);
}

static void sbsa_uart_puts(const char *s, size_t len)
{
    for ( ; len > 0; len--, s++ )
    {
        char c = *s;
        sbsa_uart_putc(c);
    }
}

static void sbsa_uart_mapping(void)
{
    /*
     * When running as dom0less direct-mapped domU, the UART address is set
     * to the address of the Xen dtuart, thus it needs to be provided using
     * CONFIG_SBSA_UART_ADDRESS=<address>.
     */
    if ( xtf_features.isdirectmap )
    {
        sbsa_uart_base = (char *)CONFIG_SBSA_UART_ADDRESS;

        /* Make sure NULL address is not passed further. */
        if ( !sbsa_uart_base )
            panic("SBSA UART address is NULL.\n");
    }
    else
        sbsa_uart_base = (char *)XEN_GUEST_PL011;

    printk("SBSA UART @ %p\n", sbsa_uart_base);

    sbsa_uart_base = (char *)set_fixmap(FIXMAP_UART, (paddr_t)sbsa_uart_base,
                                        DESC_PAGE_TABLE_DEV);
}

void sbsa_uart_console_write(const char *buf, size_t len)
{
    sbsa_uart_puts(buf, len);
}

void sbsa_uart_init(void)
{
    ASSERT(!xtf_features.isinitdomain);
    sbsa_uart_mapping();
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
