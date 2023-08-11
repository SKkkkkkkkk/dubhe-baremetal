#include <stdio.h>
#include "nand_flash.h"
#include <string.h>

#define	FEATURE_REG_NAND_BLKLOCK_REG_ADDR 		 0xA0
#define	FEATURE_REG_NAND_CONFIGURATION_REG_ADDR  0xB0
#define	FEATURE_REG_NAND_STATUS_REG_ADDR 		 0xC0
#define	FEATURE_REG_NAND_DIE_SELECT_REC_ADDR	 0xD0

int main()
{
	nand_flash_init(BOOTSPI_ID, 10, 3, UNKNOWN_FLASH);
	nand_flash_reset(BOOTSPI_ID);
	uint8_t flash_id[3];
	// while(1)
	// {
		nand_flash_read_id(BOOTSPI_ID, flash_id, 3);
		printf("flash_id: %x %x %x\n\r", flash_id[0], flash_id[1], flash_id[2]);
	// }

	uint8_t nand_get_feature(spi_id_t spi_id, uint8_t feature_addr);
	void nand_set_feature(spi_id_t spi_id, uint8_t feature_addr, uint8_t feature);

	uint8_t protection = nand_get_feature(BOOTSPI_ID, FEATURE_REG_NAND_BLKLOCK_REG_ADDR);
	uint8_t config = nand_get_feature(BOOTSPI_ID, FEATURE_REG_NAND_CONFIGURATION_REG_ADDR);
	uint8_t status = nand_get_feature(BOOTSPI_ID, FEATURE_REG_NAND_STATUS_REG_ADDR);
	uint8_t die_select = nand_get_feature(BOOTSPI_ID, FEATURE_REG_NAND_DIE_SELECT_REC_ADDR);

	printf("protection: 0x%x\n\r", protection);
	printf("config: 0x%x\n\r", config);
	printf("status: 0x%x\n\r", status);
	printf("die_select: 0x%x\n\r", die_select);

	nand_set_feature(BOOTSPI_ID, FEATURE_REG_NAND_BLKLOCK_REG_ADDR, 0x00); // disable block lock

	printf("protection: 0x%x\n\r", nand_get_feature(BOOTSPI_ID, FEATURE_REG_NAND_BLKLOCK_REG_ADDR));

	uint8_t w_buf[NAND_PAGE_SIZE];
	uint8_t r_buf[NAND_PAGE_SIZE] = {0};
	for(int i=0; i<NAND_PAGE_SIZE; i++)
		w_buf[i] = i%256;

	#define TEST_ADDR 0
	nand_flash_erase(BOOTSPI_ID, TEST_ADDR); // erase 1 block(64 pages)
	nand_flash_page_program(BOOTSPI_ID, TEST_ADDR, w_buf);
	nand_flash_read(BOOTSPI_ID, TEST_ADDR, r_buf, NAND_PAGE_SIZE);

	if(memcmp(w_buf, r_buf, NAND_PAGE_SIZE)==0)
		printf("nand flash rw test pass\n\r");
	else
		printf("nand flash rw test fail\n\r");

	while(1)
	{
		__asm__ volatile("nop");
	};
	return 0;
}