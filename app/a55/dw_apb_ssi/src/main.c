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
	nor_flash_test(BOOTSPI_ID, W25Q64JW);

	void flash_fastest_read_test(spi_id_t spi_id, flash_model_t flash_model);
	flash_fastest_read_test(BOOTSPI_ID, W25Q64JW);

	// void spi0_gpio_test();
	// spi0_gpio_test();
	return 0;
}


void spi0_gpio_test()
{
	printf("spi0_gpio_test.\n\r");

	pin_set_iomux(GROUP_GPIO1, 21, 3);
	pin_set_iomux(GROUP_GPIO1, 22, 3);
	pin_set_iomux(GROUP_GPIO1, 23, 3);
	pin_set_iomux(GROUP_GPIO1, 24, 3);
	pin_set_iomux(GROUP_GPIO1, 25, 3);
	pin_set_iomux(GROUP_GPIO1, 26, 3);

	gpio_init_config_t gpio_init_config = {
		.group = GROUP_GPIO1,
		.pin = 21,
		.gpio_control_mode = Software_Mode,
		.gpio_mode = GPIO_Output_Mode
	};
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 22;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 23;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 24;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 25;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 26;
	gpio_init(&gpio_init_config);


	while(1)
	{
		gpio_write_pin(GROUP_GPIO1, 21, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO1, 22, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO1, 23, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO1, 24, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO1, 25, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO1, 26, GPIO_PIN_SET);
		systimer_delay(500, IN_US);
		gpio_write_pin(GROUP_GPIO1, 21, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO1, 22, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO1, 23, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO1, 24, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO1, 25, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO1, 26, GPIO_PIN_RESET);
		systimer_delay(500, IN_US);
	}
	return;
}

#else
#include "main_qemu.c"
#endif