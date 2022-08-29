#ifndef UART_16550_H
#define UART_16550_H

#include <stdint.h>

#define UART0_BASE (0x21090000UL) /*!< (UART0     ) Base Address */
#define UART1_BASE (0x270A0000UL) /*!< (UART1     ) Base Address */
#define UART2_BASE (0x270B0000UL) /*!< (UART2     ) Base Address */
#define UART3_BASE (0x270C0000UL) /*!< (UART3     ) Base Address */

int console_16550_core_init(uintptr_t base_addr, unsigned int uart_clk, unsigned int baud_rate);

int console_16550_core_putc(int c, uintptr_t base_addr);

int console_16550_core_getc(uintptr_t base_addr);

void console_16550_core_flush(uintptr_t base_addr);

#endif /* UART_16550_H */
