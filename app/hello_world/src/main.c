#if defined A55
#include <stdio.h>
#include "wakeup_core.h"
#include "pl001.h"
void core1_c_entry(void)
{
	printf("hello world,in core.%u\n", get_core_id());
	void core2_c_entry(void);
	wakeup_core(2, core2_c_entry);
	while(1);
}

void core2_c_entry(void)
{
	printf("hello world,in core.%u\n", get_core_id());
	void core3_c_entry(void);
	wakeup_core(3, core3_c_entry);
	while(1);
}

void core3_c_entry(void)
{
	printf("hello world,in core.%u\n", get_core_id());
	while(1);
}

int main()
{
	printf("hello world,in core.%u\n", get_core_id());
	wakeup_core(1, core1_c_entry);
	// wakeup_core(2, core2_c_entry);
	// wakeup_core(3, core3_c_entry);
	while(1);
}
#elif defined RI5CY

int main()
{
	while(1);
}

void IRQInterrupt()
{
	
}

#elif defined CV32E40P

int main()
{
	while(1);
}

#elif defined(M3)
	volatile unsigned int * const UART0DR = (unsigned int *)0x4000C000;
 
	void print_uart0(const char *s) {
	while(*s != '\0') { /* Loop until end of string */
	*UART0DR = (unsigned int)(*s); /* Transmit char */
	s++; /* Next char */
	}
	}

	int main()
	{
		print_uart0("hello world.\n");
		return 0;
	}
#else

#error "unknowed device!"

#endif