#ifndef QEMU
#include <stdio.h>
#include <regs_type.h>
#include <arch_helpers.h>

int main()
{
	REG32(0x2E020000 + CNTCR_OFF) = CNTCR_FCREQ(0U) | CNTCR_EN;
	void CLIRegisterCommand(void);
	void vCommandConsoleTask(void *pvParameters);
	CLIRegisterCommand();
	vCommandConsoleTask(NULL);
	return 0;
}

#else
#include "main_qemu.c"
#endif