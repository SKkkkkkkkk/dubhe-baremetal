#include "pl001.h"
#include "gic.h"
#include "irq_ctrl.h"
#include "xlat_tables_v2.h"
#include "wakeup_core.h"
#include <stdio.h>

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-label"
	#pragma GCC diagnostic ignored "-Wmisleading-indentation"
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif



#define MAP_ROM MAP_REGION_FLAT( \
0x00000000, \
0x04000000, \
MT_MEMORY|MT_RW|MT_EXECUTE|MT_SECURE )

#define MAP_RAM MAP_REGION_FLAT( \
0x0e000000, \
0x01000000, \
MT_MEMORY|MT_RW|MT_EXECUTE|MT_SECURE)


#define MAP_TEXT MAP_REGION_FLAT( \
0x00000000, \
0x04000000, \
MT_MEMORY|MT_RW|MT_EXECUTE|MT_SECURE )
#define MAP_RO_DATA
#define MAP_RW_DATA


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


u_register_t get_sctlr_el3(void)		
{							
	u_register_t v;					
	__asm__ volatile ("mrs %0, sctlr_el3": "=r" (v));
	return v;						
}


int putchar(int c)
{
	uart_putchar(c);
	return 0;
}

#define __DDR_BASE 0x0e000000
#define __DDR_SIZE 0x01000000

extern char __TEXT_START__[];
extern char __TEXT_END__[];
extern char __RODATA_START__[];
extern char __RODATA_END__[];

#define MAP_DDR MAP_REGION_FLAT( \
__DDR_BASE, \
__DDR_SIZE, \
MT_RW_DATA|MT_SECURE )

#define MAP_CODE MAP_REGION_FLAT( \
(unsigned long)&__TEXT_START__, \
((unsigned long)&__TEXT_END__ - (unsigned long)&__TEXT_START__), \
MT_CODE|MT_SECURE )

#define MAP_DEVICE MAP_REGION_FLAT( \
0x04000000,			\
0x0a001000,			\
MT_DEVICE|MT_RW|MT_SECURE)


void test_irq_handler(void)
{
	printf("hello world in irq.\n\r");
	return;
}

int main()
{
	GIC_Enable();
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
	Non_secure_EL2_virtual_timer_delay3();
	Non_secure_EL2_virtual_timer_delay4();
	printf("Generic Timer Test Pass.\n\r");

	return 0;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif