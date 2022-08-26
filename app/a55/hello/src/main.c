#ifndef QEMU
#include "dw_apb_uart.h"
#include <stdio.h>

int putchar(int c)
{
	if(c == '\n')
		if(uart_sendchar(SEEHI_UART1, '\r') < 0)
			return -1;
	return uart_sendchar(SEEHI_UART1, c);
}

int main()
{
	seehi_uart_config_baudrate(SEEHI_UART_BAUDRATE_115200, 20000000, SEEHI_UART1);
	printf("hello world.\n\r");
	return 0;
}

#else
#include "main_qemu.c"
#endif