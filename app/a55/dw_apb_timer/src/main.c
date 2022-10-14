// #ifndef QEMU
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <wakeup_core.h>
#include <assert.h>
#include <systimer.h>

uint64_t* secondary_cores_entry1 = (void*)0x0000000000200410;

static void wakeup_core1(uint8_t core_id, void* func)
{
	assert((core_id<=3) && (core_id!=0));
	if(core_id==get_core_id())	return;
	secondary_cores_entry1[(core_id-1)*2+0] = (uint64_t)func;
	secondary_cores_entry1[(core_id-1)*2+1] = 0x123456788654321;
	asm volatile("dsb 0xf":::"memory");
	asm volatile("sev");
	return;
}

void corex_c_entry(void)
{
	GIC_CPUInterfaceInit(); //per CPU
	printf("hello world in core.%d\n\r", get_core_id());
	while(1);
}


int main()
{
	printf("hello world in core.%d\n\r", get_core_id());
	systimer_init();
	GIC_DistInit();
  	GIC_CPUInterfaceInit(); //per CPU
	extern void entrypoint();
	wakeup_core1(1, entrypoint);
	wakeup_core1(2, entrypoint);
	wakeup_core1(3, entrypoint);

	wakeup_core(1, corex_c_entry);
	wakeup_core(2, corex_c_entry);
	wakeup_core(3, corex_c_entry);
	systimer_delay(1, IN_S);
	void dw_apb_timer_test(bool sample);
	dw_apb_timer_test(false);
	while(1);
	return 0;
}

// #else
// #include "main_qemu.c"
// #endif