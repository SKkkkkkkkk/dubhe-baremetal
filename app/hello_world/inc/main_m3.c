#include "m3.h"

volatile unsigned int * const UART0DR = (unsigned int *)0x4000C000;

void print_uart0(const char *s) {
	while(*s != '\0') { /* Loop until end of string */
	*UART0DR = (unsigned int)(*s); /* Transmit char */
	s++; /* Next char */
	}
}

int main()
{
	SystemInit();
	print_uart0("hello world.\n");
	print_uart0("???.\n");
	return 0;
}