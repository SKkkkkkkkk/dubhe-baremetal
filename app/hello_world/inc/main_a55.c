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

int main()
{
	uint64_t sctlr = get_sctlr_el3();
	sctlr |= (1<<1); //A
	sctlr |= (1<<3); //SA
	printf("0x%x\n", *(uint32_t*)1);
	set_sctlr_el3(sctlr);
	sctlr = get_sctlr_el3();
	printf("0x%x\n", *(uint32_t*)1);
	GIC_Enable();
	IRQ_Initialize();
	IRQ_SetPriority(32, 0);
	IRQ_Enable(32);
	IRQ_SetPending(32);
	for(uint32_t i=0xffff;i!=0;i--);
	gic_send_sgi(0, SGI0_IRQn);
	gic_send_sgi(0, SGI1_IRQn);
	printf("0x%x\n", *(uint32_t*)0);
	printf("hello world,in core.%u\n", get_core_id());
	wakeup_core(1, core1_c_entry);
	// wakeup_core(2, core2_c_entry);
	// wakeup_core(3, core3_c_entry);
	while(1);
}