#include "chip_mem_layout.h"
#include <regs_type.h>
#include "dw_apb_wdt_regs.h"

#define WDT0 ((DW_APB_WDT_TypeDef *)WDT0_BASE)
#ifdef A55
	#define WDT1 ((DW_APB_WDT_TypeDef *)WDT1_BASE)
#endif

/*
0--ffff
1--1_FFFF
2--3_FFFF
...
f--7FFF_FFFF
*/

void wdt0_start(uint8_t time)
{
	if (WDT0->CR & WDT_EN_Msk)
		return;
	WDT0->TORR = time;
	WDT0->CRR = 0x76UL;

	WDT0->CR &= ~RMOD_MsK;
	WDT0->CR |= WDT_EN_Msk;
}


/**
 * @brief 喂狗
 */
void wdt0_feed(void)
{
	WDT0->CRR = 0x76UL;
}

#ifdef A55
void wdt1_start(uint8_t time)
{
	if (WDT1->CR & WDT_EN_Msk)
		return;
	WDT1->TORR = time;
	WDT1->CRR = 0x76UL;

	WDT1->CR &= ~RMOD_MsK;
	WDT1->CR |= WDT_EN_Msk;
}


/**
 * @brief 喂狗
 */
void wdt1_feed(void)
{
	WDT1->CRR = 0x76UL;
}
#endif