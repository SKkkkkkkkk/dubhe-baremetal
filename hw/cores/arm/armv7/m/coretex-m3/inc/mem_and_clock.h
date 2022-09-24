#ifndef __MEM_AND_CLOCK_H__
#define __MEM_AND_CLOCK_H__

#define __CODE_BASE 		0x00000000
#define __SRAM_BASE			0x20000000
#define __EXTERNAL_RAM_BASE	0x60000000
#if defined(QEMU)
	#define __CODE_SIZE 0x00040000
	#define __SRAM_SIZE 0x00010000

	#define __STACK_SIZE 0x00000400
	#define __HEAP_SIZE  0x00000C00
#elif  defined(RTL) ||  defined(FPGA) ||  defined(EVB) // RTL, FPGA, EVB
	#define __CODE_SIZE 		0x00010000
	#define __SRAM_SIZE 		0x00020000
	#define __EXTERNAL_RAM_SIZE	0x00100000

	#define __STACK_SIZE 0x00001000
	#define __HEAP_SIZE  0x00000C00
#else
	#error "unkowned board."
#endif

#endif
