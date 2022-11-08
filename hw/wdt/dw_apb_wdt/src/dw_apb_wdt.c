#include "chip_mem_layout.h"
#include <regs_type.h>
#include "dw_apb_wdt_regs.h"

#define WDT ((DW_APB_WDT_TypeDef *)WDT0_BASE)

/*
0--ffff
1--1_FFFF
2--3_FFFF
...
f--7FFF_FFFF
*/

void wdt_start(uint8_t time)
{
	if (WDT->CR & WDT_EN_Msk)
		return;
	WDT->TORR = time;
	WDT->CRR = 0x76UL;

	WDT->CR &= ~RMOD_MsK;
	WDT->CR |= WDT_EN_Msk;
}


/**
 * @brief 喂狗
 */
void wdt_feed(void)
{
	WDT->CRR = 0x76UL;
}