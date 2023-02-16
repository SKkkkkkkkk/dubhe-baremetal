#include "m3.h"
#include <stdio.h>
#include "systimer.h"


void ddr_test(uintptr_t start_addr, uintptr_t end_addr, uint8_t bytes);
int main()
{
	printf("M2v5: "BUILD_TIMESTAMP"\n\r");
	printf("git hash: "GIT_HASH"\n\r");
	printf("M3_BL2: hello world.\n\r");

	// ddr_test(DDR_BASE+(1024*1024), 0xBFFFFFFF, 1);
	// ddr_test(DDR_BASE+(1024*1024), 0xBFFFFFFF, 2);
	// ddr_test(DDR_BASE+(1024*1024), 0xBFFFFFFF, 4);

	systimer_init();
	uint32_t i = 0;

	systimer_id_t timer_id = systimer_acquire_timer();
	ddr_test(DDR_BASE+(1024*1024), DDR_BASE+(2*1024*1024), 1);
	uint64_t delta = systimer_get_elapsed_time(timer_id, IN_MS);
	// ddr_test(DDR_BASE+(1024*1024), DDR_BASE+(2*1024*1024), 2);
	// ddr_test(DDR_BASE+(1024*1024), DDR_BASE+(2*1024*1024), 4);
	printf("DDR_TEST PASS: %lu.\n\r", (uint32_t)delta);


	while(1)
	{
		printf("%lu\n\r", i++);
		systimer_delay(1, IN_S);
	}
	return 0;
}


void ddr_test(uintptr_t start_addr, uintptr_t end_addr, uint8_t bytes)
{
	switch (bytes)
	{
	case 1:
		while(start_addr+1 <= end_addr)
		{
			uint8_t w_value = start_addr%0x100;
			*(uint8_t*)start_addr = w_value;
			uint8_t r_value = *(uint8_t*)start_addr;
			if(r_value != w_value)
			{
				printf("DDR_TEST ERROR: write 0x%x to 0x%x, read back 0x%0x.\n\r", w_value, start_addr, r_value);
				while(1) {asm volatile("");}
			}
			start_addr += 1;
		}
		break;

	case 2:
		while(start_addr+2 <= end_addr)
		{
			uint16_t w_value = start_addr%0x10000;
			*(uint16_t*)start_addr = w_value;
			uint16_t r_value = *(uint16_t*)start_addr;
			if(r_value != w_value)
			{
				printf("DDR_TEST ERROR: write 0x%x to 0x%x, read back 0x%0x.\n\r", w_value, start_addr, r_value);
				while(1) {asm volatile("");}
			}
			start_addr += 2;
		}
		break;

	case 4:
		while(start_addr+4 <= end_addr)
		{
			uint32_t w_value = start_addr%0x10000;
			*(uint32_t*)start_addr = w_value;
			uint32_t r_value = *(uint32_t*)start_addr;
			if(r_value != w_value)
			{
				printf("DDR_TEST ERROR: write 0x%lx to 0x%x, read back 0x%0lx.\n\r", w_value, start_addr, r_value);
				while(1) {asm volatile("");}
			}
			start_addr += 4;
		}
		break;
	
	default:
		printf("DDR_TEST ERROR: ???\n\r");
		break;
	}

}