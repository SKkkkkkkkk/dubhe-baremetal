#ifndef QEMU
#include <stdio.h>
#include "arch_features.h"
#include "dw_apb_ssi.h"
#include "systimer.h"
#include "nor_flash.h"
#include <dw_apb_gpio.h>


#define FLASH_SECTOR_SIZE 4096
#define TEST_FLASH_ADDR   0
uint8_t r_buf[FLASH_SECTOR_SIZE] = {0};
uint8_t w_buf[FLASH_SECTOR_SIZE] = {0};
int main()
{
	printf("hello world.\n\r");
	systimer_init();

	// flash_init(BOOTSPI_ID, 100, 3, UNKNOWN_FLASH);
	// uint8_t flash_id[3];
	// // while(1)
	// // {
	// 	flash_read_id(BOOTSPI_ID, flash_id, 3);
	// 	printf("flash id: 0x%x 0x%x 0x%x\n\r", flash_id[0], flash_id[1], flash_id[2]);
	// 	systimer_delay(500, IN_MS);
	// }

	// for(uint64_t i=0;i<sizeof(w_buf);i++)
	// {
	// 	w_buf[i] = i%256;
	// }

	// // 1.read
	// flash_read(BOOTSPI_ID, TEST_FLASH_ADDR, r_buf, sizeof(r_buf));
	// // while(1);

	// // 2.erase
	// flash_sector_erase(BOOTSPI_ID, TEST_FLASH_ADDR);
	// flash_read(BOOTSPI_ID, TEST_FLASH_ADDR, r_buf, sizeof(r_buf));
	// for(uint64_t i = 0; i<FLASH_SECTOR_SIZE; i++)
	// {
	// 	if(r_buf[i] != 0xff)
	// 	{
	// 		printf("flash sector erase error!\n\r");
	// 		while(1);
	// 	}
	// }
	// printf("flash sector erase finished!\n\r");

	// // 3.write
	// flash_write(BOOTSPI_ID, TEST_FLASH_ADDR, w_buf, sizeof(w_buf));

	// // 4.read back
	// flash_read(BOOTSPI_ID, TEST_FLASH_ADDR, r_buf, sizeof(r_buf));
	// if(memcmp(w_buf, r_buf , sizeof(r_buf)) == 0)
	// {
	// 	printf("flash write/read compared through.\n\r");
	// }
	// else
	// {
	// 	printf("flash write/read did not compare through.\n\r");
	// }

	// while(1);

	void nor_flash_test(spi_id_t spi_id, flash_model_t flash_model);
	nor_flash_test(SPI0_ID, GD25LQ64);

	void flash_fastest_read_test(spi_id_t spi_id, flash_model_t flash_model);
	flash_fastest_read_test(SPI0_ID, GD25LQ64);
	return 0;
}

#else
#include "main_qemu.c"
#endif