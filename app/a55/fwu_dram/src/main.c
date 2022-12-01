#ifndef QEMU
#include <stdio.h>
#include "arch_features.h"
#include "FreeRTOS_CLI.h"
#include "system_counter.h"

int main()
{
	initSystemCounter(0, 0);
	printf("FWU_DRAM: "BUILD_TIMESTAMP"\n\r");
	printf("git hash: "GIT_HASH"\n\r");
	void CLIRegisterCommand(void);
	void vCommandConsoleTask(void *pvParameters);
	CLIRegisterCommand();
	vCommandConsoleTask(NULL);
	return 0;
}

#else
#include "main_qemu.c"
#endif