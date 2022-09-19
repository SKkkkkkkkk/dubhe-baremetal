#ifndef QEMU
#include <stdio.h>
#include "dw_apb_gpio.h"
#include "systimer.h"

int main()
{
	systimer_init();
	printf("hello world.\n\r");

	pin_set_iomux(GROUP_GPIO0, 16, 3);
	pin_set_iomux(GROUP_GPIO0, 17, 3);
	pin_set_iomux(GROUP_GPIO0, 18, 3);
	pin_set_iomux(GROUP_GPIO0, 19, 3);
	pin_set_iomux(GROUP_GPIO0, 20, 3);
	pin_set_iomux(GROUP_GPIO0, 21, 3);

	gpio_init_config_t gpio_init_config = {
	    .group = GROUP_GPIO0,
	    .pin = 16,
	    .gpio_control_mode = Software_Mode,
	    .gpio_mode = GPIO_Output_Mode
	};
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 17;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 18;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 19;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 20;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 21;
	gpio_init(&gpio_init_config);


	while(1)
	{
		gpio_write_pin(GROUP_GPIO0, 16, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO0, 17, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO0, 18, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO0, 19, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO0, 20, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO0, 21, GPIO_PIN_SET);
		// systimer_delay(500, IN_US);
		gpio_write_pin(GROUP_GPIO0, 16, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO0, 17, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO0, 18, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO0, 19, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO0, 20, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO0, 21, GPIO_PIN_RESET);
		// systimer_delay(500, IN_US);
	}
	return 0;
}

#else
#include "main_qemu.c"
#endif