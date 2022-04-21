#include <stdio.h>
#include <stdint.h>
#include "wakeup_core.h"
#include "pl001.h"
#include "gic.h"
#include "irq_ctrl.h"
#include "xlat_tables_v2.h"

uint32_t get_daif()
{
	uint32_t daif;
	asm volatile("mrs %0, daif":"=r"(daif));
	return daif;
}

// uint64_t get_allint()
// {
// 	uint64_t allint;
// 	asm volatile("mrs %0, ALLINT":"=r"(allint));
// 	return allint;
// }

uint32_t get_currentel()
{
	uint32_t currentel;
	asm volatile("mrs %0, currentel":"=r"(currentel));
	return currentel;
}

uint64_t get_sctlr_el3()
{
	uint64_t sctlr_el3;
	asm volatile("mrs %0, sctlr_el3":"=r"(sctlr_el3));
	return sctlr_el3;
}

uint64_t get_scr_el3()
{
	uint64_t scr_el3;
	asm volatile("mrs %0, scr_el3":"=r"(scr_el3));
	return scr_el3;
}


void set_sctlr_el3(uint64_t sctlr_el3)
{
	asm volatile("msr sctlr_el3, %0"::"r"(sctlr_el3));
}

void core1_c_entry(void)
{
	GIC_CPUInterfaceInit(); //per CPU
	printf("hello world,in core.%u\n", get_core_id());
	void core2_c_entry(void);
	wakeup_core(2, core2_c_entry);
	while(1);
}

void core2_c_entry(void)
{
	GIC_CPUInterfaceInit(); //per CPU
	printf("hello world,in core.%u\n", get_core_id());
	void core3_c_entry(void);
	wakeup_core(3, core3_c_entry);
	while(1);
}

void core3_c_entry(void)
{
	GIC_CPUInterfaceInit(); //per CPU
	printf("hello world,in core.%u\n", get_core_id());
	while(1);
}

void gic_send_sgi(uint8_t core_id, IRQn_Type irqn)
{
	GICDistributor->SGIR = ((1 << (16 + core_id)) | irqn);
}


void irq32_handler(void)
{
	volatile double d0;
	volatile double pi = 5.55555;
	(void)(pi*pi/1.234);
	asm volatile("fmov %0, d0":"=r"(d0));
	printf("\tin irq d0:%f\n", d0);
}

#define MAP_ROM MAP_REGION_FLAT( \
0x00000000, \
0x04000000, \
MT_MEMORY|MT_RO|MT_SECURE )

#define MAP_RAM MAP_REGION_FLAT( \
0x0e000000, \
0x01000000, \
MT_MEMORY|MT_RW|MT_SECURE )


#define DEVICE0_BASE			0x08000000
#define DEVICE0_SIZE			0x01000000
#define DEVICE1_BASE			0x09000000
#define DEVICE1_SIZE			0x00c00000

#define MAP_DEVICE0	MAP_REGION_FLAT(DEVICE0_BASE,			\
					DEVICE0_SIZE,			\
					MT_DEVICE | MT_RW | MT_SECURE)

#define MAP_DEVICE1	MAP_REGION_FLAT(DEVICE1_BASE,			\
					DEVICE1_SIZE,			\
					MT_DEVICE | MT_RW | MT_SECURE)

int main()
{
	printf("hello world,in core.%u\n", get_core_id());

	const mmap_region_t plat_bl1_region[] = {
		MAP_ROM,
		MAP_RAM,
		MAP_DEVICE0,
		MAP_DEVICE1,
		{0}
	};
	(void)plat_bl1_region;
	mmap_add(plat_bl1_region);
	init_xlat_tables();
	enable_mmu_el3(0);
	IRQ_Initialize();
	IRQ_SetHandler(32, irq32_handler);
	IRQ_SetPriority(32, 0);
	IRQ_Enable((IRQn_ID_t)32);
	IRQ_SetPending(32);

	wakeup_core(1, core1_c_entry);
	while(1);
}


