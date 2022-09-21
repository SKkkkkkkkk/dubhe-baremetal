#include "m3.h"
#include <stdio.h>
#include <assert.h>

static inline void rom2xxx(uint32_t pc)
{
	assert(pc&1);
	__DMB();
	__DSB();
	__ISB();
	asm volatile ("bx r0":::"memory");
}

int main()
{
	// printf("rom2ddr\n\r");
	// init_ddr();
	rom2xxx(0x60000430+1);
	return 0;
}