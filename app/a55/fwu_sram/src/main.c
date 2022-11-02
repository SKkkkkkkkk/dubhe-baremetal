#ifndef QEMU
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <arch_features.h>

static inline void setpc(uint64_t pc)
{
	assert((pc&3)==0);
	dmbsy();
	dsbsy();
	isb();
	asm volatile ("br %0"::"r"(pc):"memory");
}



int main()
{
	printf("git hash: "GIT_HASH"\n\r");
	printf("FWU SRAM:\n\r");
	printf("\tddr init.\n\r");
	printf("\tgo to FWU DDR.\n\r");
	setpc(0x40300000UL);
	return 0;
}

#else
#include "main_qemu.c"
#endif