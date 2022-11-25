#include <stdio.h>
#include "dw_apb_wdt.h"
#include "systimer.h"
#include "gic.h"
#include "irq_ctrl.h"
#include "chip_mem_layout.h"
#include "regs_type.h"


int main()
{
	printf("reset reg: 0x%x\n\r", REG32(SYSCTRL_BASE+0x410));
	GIC_DistInit();
	GIC_CPUInterfaceInit(); //per CPU

	printf("wdt test.\n\r");


	systimer_init();

	wdt_reset(WDT0_ID);

	GIC_SetTarget(WDT0_IRQn, 1 << 0);
	void wdt_irqhandler(void);
	IRQ_SetHandler(WDT0_IRQn, wdt_irqhandler);
	IRQ_SetPriority(WDT0_IRQn, 0 << 3);
	IRQ_Enable(WDT0_IRQn);

	wdt_setup(WDT0_ID, 0, 1, 0xa);
	uint32_t i = 0;
	while(1)
	{
		printf("%u\n\r", i++);
		systimer_delay(1, IN_S);
	}
	return 0;
}


void wdt_irqhandler(void)
{
	printf("in irq.\n\r");
	wdt_clear_irq(WDT0_ID);
	// wdt_feed(WDT0_ID);
}