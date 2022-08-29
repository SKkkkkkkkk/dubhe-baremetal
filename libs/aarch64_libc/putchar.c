/*
 * Copyright (c) 2013-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdbool.h>
#include "uart_16550.h"

static bool putchar_init_flag = false;

__attribute__((weak)) int putchar(int c)
{
	if(!putchar_init_flag)
	{
		console_16550_core_init(UART1_BASE, 20000000, 115200);
		putchar_init_flag = true;
	}
	if(c == '\n')
		if(console_16550_core_putc('\r', UART1_BASE) < 0)
			return EOF;
	return console_16550_core_putc(c, UART1_BASE);
}