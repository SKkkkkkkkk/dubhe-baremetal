#ifndef __MEM_AND_CLOCK_H__
#define __MEM_AND_CLOCK_H__


#if defined(QEMU)
	#define __CODE_BASE 		0x00000000
	#define __SRAM_BASE			0x20000000

	#define __CODE_SIZE 0x00040000
	#define __SRAM_SIZE 0x00010000

	#define __STACK_SIZE 0x00000400
	#define __HEAP_SIZE  0x00001000
#elif  defined(RTL) ||  defined(FPGA) ||  defined(EVB) // RTL, FPGA, EVB
	#define __CODE_BASE 		0x00000000
	#define __AXI_SRAM_BASE		0x00100000
	#define __SRAM_BASE			0x2003E000 //最后8KB
	#define __EXTERNAL_RAM_BASE	0x79600000

	#define __CODE_SIZE 		0x00010000
	#define __AXI_SRAM_SIZE		0x00008000
	#define __SRAM_SIZE 		0x00002000 //最后8KB
	#define __EXTERNAL_RAM_SIZE	0x00400000

	#define __STACK_SIZE 0x00000400
	#define __HEAP_SIZE  0x00001000
#else
	#error "unkowned board."
#endif

#endif
