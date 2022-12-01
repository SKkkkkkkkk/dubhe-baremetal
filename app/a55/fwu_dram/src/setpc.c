#include <stdint.h>
#include <assert.h>
#include "arch_helpers.h"


void setpc(uintptr_t pc)
{
	assert((pc&3)==0);
	dmbsy();
	dsbsy();
	isb();
	// disable_mmu_icache();
	invalidate_icache_all();
	asm volatile ("br %0"::"r"(pc):"memory");
}
