#ifdef A55
#include <stdint.h>

#define CORE_NUMS 4
volatile uint64_t secondary_cores_entry[(CORE_NUMS-1)*2];
// 0 1 | 2 3 | 4 5
uint64_t get_core_id()
{
	uint64_t core_id;
	asm volatile("mrs %0, mpidr_el1":"=r"(core_id));
	return core_id&0xff;
}

void wakeup_core(uint8_t core_id, void* func)
{
	if(core_id==get_core_id())	return;
	secondary_cores_entry[(core_id-1)*2+0] = (uint64_t)func;
	secondary_cores_entry[(core_id-1)*2+1] = 0x123456788654321;
	asm volatile("dsb 0xf":::"memory");
	asm volatile("sev");
	return;
}
#endif