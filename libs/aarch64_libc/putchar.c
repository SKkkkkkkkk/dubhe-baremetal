/*
 * Copyright (c) 2013-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef QEMU
	#include "pl011.h"
#else
	#include "dw_apb_uart.h"
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
		if(seehi_uart_config_baudrate(115200, 20000000, SEEHI_UART1)!=0)
			while(1);
		putchar_init_flag = true;
	}
	if(c == '\n')
		if(uart_sendchar(SEEHI_UART1, '\r') < 0)
			return EOF;
	return uart_sendchar(SEEHI_UART1, c);
#endif
}

__attribute__((weak)) int getchar()
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
	int ret;
	char c;
	while((ret = uart_getchar(&c)) != UART_OK);
	return (int)(uint8_t)c;
#else
	if(!putchar_init_flag)
	{
		if(seehi_uart_config_baudrate(115200, 20000000, SEEHI_UART1)!=0)
			while(1);
		putchar_init_flag = true;
	}
	return uart_getchar(SEEHI_UART1);
#endif
}
