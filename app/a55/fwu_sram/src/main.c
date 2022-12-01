#ifndef QEMU
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "arch_features.h"
#include "FreeRTOS_CLI.h"
#include "system_counter.h"

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
	initSystemCounter(0, 0);
	printf("FWU_SRAM: "BUILD_TIMESTAMP"\n\r");
	printf("git hash: "GIT_HASH"\n\r");
	
	// 1. init ddr
	printf("ddr init.\n\r");

	// 2. load and run fwu_dram
	int load_and_run(bool use_cli);
	load_and_run(true);
	return 0;
}

#else
#include "main_qemu.c"
#endif