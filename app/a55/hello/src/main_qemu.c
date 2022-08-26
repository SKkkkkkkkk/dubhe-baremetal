#include "pl001.h"
#include <stdio.h>

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-label"
	#pragma GCC diagnostic ignored "-Wmisleading-indentation"
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

int putchar(int c)
{
	uart_putchar(c);
	return 0;
}

int main()
{
	printf("hello world.\n\r");
	return 0;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif