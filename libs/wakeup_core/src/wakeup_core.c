#ifdef A55
#include <stdint.h>

#define CORE_NUMS 4
volatile uint64_t secondary_cores_entry[CORE_NUMS*2];
// 0 1 | 2 3 | 4 5 | 6 7

// uint32_t get_core_id()
// {
// 	uint32_t core_id;
// 	asm volatile("mrs %0, mpidr_el1":"=r"(core_id));
// #if defined QEMU
// 	return core_id&0xff;
// #else
// 	return (core_id&0xffff)>>8;
// #endif
// }

void wakeup_core(uint8_t core_id, void* func)
{
	extern uint32_t get_core_id();
	if(core_id==get_core_id())	return;
	secondary_cores_entry[core_id*2+0] = (uint64_t)func;
	secondary_cores_entry[core_id*2+1] = 0x123456788654321;
	asm volatile("dsb 0xf":::"memory");
	asm volatile("sev");
	return;
}
#endif