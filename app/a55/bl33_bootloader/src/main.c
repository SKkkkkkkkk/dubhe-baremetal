#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nor_flash.h"


#define STORAGE_MEDIA_MEMORYMAP 0
#define STORAGE_MEDIA_NORFLASH	1

#define STORAGE_MEDIA STORAGE_MEDIA_NORFLASH


static inline void jump2xxx(uint64_t pc)
{
	// Ensure all instructions before the jump are completed
	__asm__ volatile("dsb ish");

	// Invalidate instruction cache
	__asm__ volatile("ic iallu");

	// Branch to new address
	__asm__ volatile("br %0" : : "r" (pc));
}

int main()
{
	printf("Build Time: "BUILD_TIMESTAMP"\n\r");
	printf("Git Hash: "GIT_HASH"\n\r");

	uint64_t offset, size, pc;
	pc = 0xb0000000;

#if (STORAGE_MEDIA == STORAGE_MEDIA_MEMORYMAP)
	offset = 0x50000000;
	size = 64*1024;

	(void)memcpy((void *)pc, (void *)offset, size);
#elif (STORAGE_MEDIA == STORAGE_MEDIA_NORFLASH)
	offset = 0;
	size = 64*1024;

	if(!flash_init(BOOTSPI_ID, 10, 3, UNKNOWN_FLASH))
	{
		printf("flash init failed\n\r");
		return -1;
	}	
	flash_read_dma(BOOTSPI_ID, offset, (uint8_t *)pc, size);
#else

#endif

	jump2xxx(pc);
	return 0;
}