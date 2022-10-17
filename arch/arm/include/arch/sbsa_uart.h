/**
 * @file arch/arm/include/arch/sbsa_uart.h
 *
 * ARM SBSA UART driver to be used with Xen virtual PL011 UART.
 */
#ifndef XTF_ARM_SBSA_UART_H
#define XTF_ARM_SBSA_UART_H

/*
 * Xen PL011 mappings.
 * We need to rely on them as there is no DTB support in XTF.
 */
#define XEN_GUEST_PL011   0x22000000U

/* UART register offsets */
#define SBSA_UART_DR      (0x00)   /* Data register. */
#define SBSA_UART_FR      (0x18)   /* Flag register. */

/* UARTFR bits */
#define SBSA_UART_FR_BUSY (1 << 3) /* Transmit is not complete. */

#ifdef CONFIG_SBSA_UART
void sbsa_uart_console_write(const char *buf, size_t len);
void sbsa_uart_init(void);
#endif

#endif /* XTF_ARM_SBSA_UART_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
