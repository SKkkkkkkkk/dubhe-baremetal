#include "pl001.h"
#include "irq_ctrl.h"
#include "xlat_tables_v2.h"
#include "wakeup_core.h"
#include <stdio.h>

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

int main()
{
	printf("hello world\n\r");
	const mmap_region_t mmap_region_list[] = {
		MAP_DDR,
		MAP_CODE,
		MAP_DEVICE0,
		MAP_DEVICE1,
		{0}
	};
	mmap_add(mmap_region_list);
	init_xlat_tables();
	enable_mmu_el3(0);
	printf("hello world\n\r");
	while(1);
	return 0;
}