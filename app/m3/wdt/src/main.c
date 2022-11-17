#include <stdio.h>
#include "dw_apb_wdt.h"
#include "systimer.h"

int main()
{
	printf("wdt test.\n\r");
	systimer_init();

	NVIC_SetPriority(WDT0_IRQn, 0);
	void wdt_irqhandler(void);
	NVIC_SetVector(WDT0_IRQn, (uint32_t)(uintptr_t)wdt_irqhandler);
	NVIC_EnableIRQ(WDT0_IRQn);

	wdt_setup(WDT0_ID, 0, 1, 0xb);

	uint32_t i = 0;
	while(1)
	{
		printf("%lus\n\r", i++);
		systimer_delay(1, IN_S);
		// wdt_feed(WDT0_ID);
	}
	return 0;
}

void wdt_irqhandler(void)
{
	printf("in irq.\n\r");
	wdt_clear_irq(WDT0_ID);
	wdt_feed(WDT0_ID);
}