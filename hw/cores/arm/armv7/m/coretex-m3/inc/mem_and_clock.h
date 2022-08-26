#ifndef __MEM_AND_CLOCK_H__
#define __MEM_AND_CLOCK_H__

#if defined(RTL)
	#define __CODE_BASE 0x00000000
	#define __CODE_SIZE 0x00010000

	#define __SRAM_BASE	0x20000000
	#define __SRAM_SIZE 0x00020000

	#define __STACK_SIZE 0x00000400
	#define __HEAP_SIZE  0x00000C00
#elif defined(FPGA)
	#define __CODE_BASE 0x00000000
	#define __CODE_SIZE 0x00010000

	#define __SRAM_BASE	0x20000000
	#define __SRAM_SIZE 0x00020000

	#define __STACK_SIZE 0x00000400
	#define __HEAP_SIZE  0x00000C00
#elif defined(EVB)

#elif defined(QEMU)
	#define __CODE_BASE 0x00000000
	#define __CODE_SIZE 0x00040000

	#define __SRAM_BASE	0x20000000
	#define __SRAM_SIZE 0x00010000

	#define __STACK_SIZE 0x00000400
	#define __HEAP_SIZE  0x00000C00
#else
	#error "unkowned board."
#endif


#endif