#ifndef QEMU
#include <stdio.h>
#include "imgout_sys_regs.h"
#include "mipi_tx_csi.h"
#if defined M3
#include "_cm3_chip_define.h"
#else
#include "_ca55_chip_define.h"
#endif

void seehi_pmu_init(void)
{
	REG32(0x2e045000) = 8;
	REG32(0x2e046000) = 8;
	REG32(0x2e047000) = 8;
	REG32(0x2e048000) = 8;
	REG32(0x2e04b000) = 8;
	REG32(0x2e04d000) = 8;
	REG32(0x2e04e000) = 8;
}

int main(void) {
	// printf("M2v5: "BUILD_TIMESTAMP"\n\r");
	// printf("git hash: "GIT_HASH"\n\r");
	seehi_pmu_init();
	printf("A55: hello world.\n\r");
	// systimer_init();
	mipi_test_main();
	return 0;
}
#else
#include "main_qemu.c"
#endif