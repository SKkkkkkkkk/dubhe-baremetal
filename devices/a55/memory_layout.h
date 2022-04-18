#ifndef __A55_MEMORY_LAYOUT_H__
#define __A55_MEMORY_LAYOUT_H__

#ifndef QEMU
	#define __ROM_BASE 0x00000000U
	#define __ROM_SIZE 0x00010000U //64KB

	#define __RAM_BASE 0x00A00000U
	#define __RAM_SIZE 0x00020000U //128KB
#else
	#define __ROM_BASE 0x00000000U
	#define __ROM_SIZE 0x04000000U //64MB

	#define __RAM_BASE 0x0e000000U
	#define __RAM_SIZE 0x01000000U //16MB
#endif

#define __STACK_SIZE 0x4000U //16KB
#define __HEAP_SIZE  0x4000U //16KB

#define PAGE_SIZE	4096

#endif