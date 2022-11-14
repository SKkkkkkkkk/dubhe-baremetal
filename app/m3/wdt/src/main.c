#include <stdio.h>
#include "dw_apb_wdt.h"
#include "systimer.h"

int main()
{
	printf("wdt test.\n\r");


	systimer_init();
	wdt0_start(0xb);

	uint32_t i = 0;
	while(1)
	{
		printf("%lus\n\r", i++);
		systimer_delay(1, IN_S);
		wdt0_feed();
	}
	return 0;
}