#include "m3.h"
#include <stdio.h>
#include <stdbool.h>

int main()
{
	printf("hello world.\n\r");
	SCB->SHCSR |= 7<<16;
	// printf("SCB->SHCSR: 0x%lx\n\r", SCB->SHCSR);
	__DSB();
	void dw_apb_timer_test(bool sample);
	dw_apb_timer_test(0);
	return 0;
}