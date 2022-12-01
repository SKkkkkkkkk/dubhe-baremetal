#ifndef QEMU
#include <stdio.h>
#include "arch_features.h"
#include "FreeRTOS_CLI.h"
#include "system_counter.h"
#include "fip_update.h"



int main()
{
	initSystemCounter(0, 0);
	printf("FWU_DRAM: "BUILD_TIMESTAMP"\n\r");
	printf("git hash: "GIT_HASH"\n\r");

	if(1) // CLI update
	{
		star_tinyCLI();
		assert(0);
	}
	else // use gpio
	{
		// TODO
	}

	return 0;
}

#else
#include "main_qemu.c"
#endif