#include <stdio.h>
#include "wakeup_core.h"
#include "pl001.h"
#include "gic.h"
#include "irq_ctrl.h"

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
	printf("this is irq.32.\n");
}

int main()
{
	printf("hello world,in core.%u\n", get_core_id());

	IRQ_Initialize();
	IRQ_SetHandler(32, irq32_handler);
	// IRQ_SetPriority(32, 0);
	IRQ_Enable(32);
	IRQ_SetPending(32);

	wakeup_core(1, core1_c_entry);
	// wakeup_core(2, core2_c_entry);
	// wakeup_core(3, core3_c_entry);
	while(1);
}