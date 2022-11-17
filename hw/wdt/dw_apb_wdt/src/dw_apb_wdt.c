#include <stdio.h>
#include <assert.h>
#include "chip_mem_layout.h"
#include "regs_type.h"
#include "dw_apb_wdt_regs.h"
#include "dw_apb_wdt.h"

#define WDT0 ((DW_APB_WDT_TypeDef *)WDT0_BASE)
#ifdef A55
	#define WDT1 ((DW_APB_WDT_TypeDef *)WDT1_BASE)
#endif

static inline DW_APB_WDT_TypeDef* get_wdt_by_id(wdt_id_t id)
{
	DW_APB_WDT_TypeDef* wdt;
	switch (id)
	{
	case WDT0_ID:
		wdt = WDT0;
		break;
	#ifdef A55
	case WDT1_ID:
		wdt = WDT1;
		break;
	#endif
	default:
		wdt = NULL;
		// break;
	}
	return wdt;
}

/* extern */
void wdt_setup(wdt_id_t id, uint8_t rpl, uint8_t rmod, uint8_t timeout)
{
	DW_APB_WDT_TypeDef* wdt = get_wdt_by_id(id);
	assert(wdt);
	if (wdt->CR & WDT_EN_Msk)
		return;

	/*
		0--ffff
		1--1_FFFF
		2--3_FFFF
		...
		f--7FFF_FFFF
	*/
	wdt->TORR = timeout;
	wdt->CRR = 0x76UL;

	uint32_t tmp = 0;
	tmp |= rpl << RPL_Pos;
	tmp |= rmod << RMOD_Pos;
	tmp |= WDT_EN_Msk;
	wdt->CR = tmp;
}

void wdt_reset(wdt_id_t id)
{
	DW_APB_WDT_TypeDef* wdt = get_wdt_by_id(id);
	assert(wdt);
	wdt->CR = 0;
}

void wdt_feed(wdt_id_t id)
{
	DW_APB_WDT_TypeDef* wdt = get_wdt_by_id(id);
	assert(wdt);
	wdt->CRR = 0x76UL;
}

void wdt_clear_irq(wdt_id_t id)
{
	DW_APB_WDT_TypeDef* wdt = get_wdt_by_id(id);
	assert(wdt);
	(void)wdt->EOI;
}