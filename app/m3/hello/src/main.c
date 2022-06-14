#include "m3.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern const VECTOR_TABLE_Type __VECTOR_TABLE[240];
VECTOR_TABLE_Type __VECTOR_TABLE_IN_SRAM[240] __aligned(4);

void irq_handler(void)
{
	printf("hello world in irq.\n");
	return;
}

int main()
{
	memcpy(__VECTOR_TABLE_IN_SRAM, __VECTOR_TABLE, sizeof(__VECTOR_TABLE_IN_SRAM));
	__DSB();
	SCB->VTOR = (uint32_t) &(__VECTOR_TABLE_IN_SRAM[0]);
	__DSB();
	__ISB();
	NVIC_SetVector(Interrupt0_IRQn, (uintptr_t)irq_handler);
	NVIC_EnableIRQ(Interrupt0_IRQn);
	NVIC_SetPendingIRQ(Interrupt0_IRQn);
	printf("hello world.\n");
	return 0;
}