/*
 * Copyright (c) 2013-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef QEMU
	#include "pl001.h"
#else
	#include "uart_16550.h"
#endif
static bool putchar_init_flag = false;

__attribute__((weak)) int putchar(int c)
{
#ifdef QEMU
	if(!putchar_init_flag)
	{
		// uart_config _uart_config = {
		// 	.data_bits = 8,
		// 	.stop_bits = 1,
		// 	.parity = false,
		// 	.baudrate = 115200
		// };
		// if(uart_configure(&_uart_config) != UART_OK)
		// 	while(1);
		putchar_init_flag = true;
	}
	if(c == '\n')
		uart_putchar('\r');
	uart_putchar(c);
	return c;
#else
	if(!putchar_init_flag)
	{
		console_16550_core_init(UART1_BASE, 20000000, 115200);
		putchar_init_flag = true;
	}
	if(c == '\n')
		if(console_16550_core_putc('\r', UART1_BASE) < 0)
			return EOF;
	return console_16550_core_putc(c, UART1_BASE);
#endif
}