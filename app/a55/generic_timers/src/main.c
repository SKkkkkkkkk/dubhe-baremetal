#ifndef QEMU
#include "gic.h"
#include "arch_features.h"
#include "system_counter.h"
#include <stdio.h>

struct memory_mapped_timer_module
{
        volatile uint32_t CNTCR;                  // +0x0000 - RW - Counter Control Register
  const volatile uint32_t CNTSR;                  // +0x0004 - RO - Counter Status Register
        volatile uint64_t CNTCV;                  // +0x0008 - RW - Counter Count Value register
        volatile uint32_t CNTSCR;                 // +0x0010 - RW - Counter Scaling Value (ARMv8.4-CNTSC)
  const volatile uint32_t padding0[2];            // +0x0014 - RESERVED
  const volatile uint32_t CNTID;                  // +0x001C - RO - Counter ID register
        volatile uint32_t CNTFID[3];              // +0x0020 - RW - Counter Access Control Register N
};
extern struct memory_mapped_timer_module * const counter_module;
DEFINE_SYSREG_RW_FUNCS(cntvct_el0)
int main()
{
	printf("in main:\n\r");
	GIC_Enable();
	initSystemCounter(0, 0);
	printf("CNTCR: 0x%x\n\r", counter_module->CNTCR);

	printf("Generic Timer Test:\n\r");
	void EL3_physical_timer_delay1(void);
	void EL3_physical_timer_delay2(void);
	void EL3_physical_timer_delay3(void);
	void EL3_physical_timer_delay4(void);
	void Non_secure_EL2_physical_timer_delay1(void);
	void Non_secure_EL2_physical_timer_delay2(void);
	void Non_secure_EL2_physical_timer_delay3(void);
	void Non_secure_EL2_physical_timer_delay4(void);
	void EL1_physical_timer_delay1(void);
	void EL1_physical_timer_delay2(void);
	void EL1_physical_timer_delay3(void);
	void EL1_physical_timer_delay4(void);
	void EL1_virtual_timer_delay1(void);
	void EL1_virtual_timer_delay2(void);
	void EL1_virtual_timer_delay3(void);
	void EL1_virtual_timer_delay4(void);
	void Non_secure_EL2_virtual_timer_delay1(void);
	void Non_secure_EL2_virtual_timer_delay2(void);
	void Non_secure_EL2_virtual_timer_delay3(void);
	void Non_secure_EL2_virtual_timer_delay4(void);
	EL3_physical_timer_delay1();
	EL3_physical_timer_delay2();
	EL3_physical_timer_delay3();
	EL3_physical_timer_delay4();
	Non_secure_EL2_physical_timer_delay1();
	Non_secure_EL2_physical_timer_delay2();
	Non_secure_EL2_physical_timer_delay3();
	Non_secure_EL2_physical_timer_delay4();
	EL1_physical_timer_delay1();
	EL1_physical_timer_delay2();
	EL1_physical_timer_delay3();
	EL1_physical_timer_delay4();
	EL1_virtual_timer_delay1();
	EL1_virtual_timer_delay2();
	EL1_virtual_timer_delay3();
	EL1_virtual_timer_delay4();
	Non_secure_EL2_virtual_timer_delay1();
	Non_secure_EL2_virtual_timer_delay2();
	// Non_secure_EL2_virtual_timer_delay3();
	// Non_secure_EL2_virtual_timer_delay4();
	printf("Generic Timer Test Pass.\n\r");

	void cntvoff_el2_test();
	cntvoff_el2_test();
	return 0;
}


#else
#include "main_qemu.c"
#endif