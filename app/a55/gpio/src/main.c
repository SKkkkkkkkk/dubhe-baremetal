#include <stdio.h>
#include "dw_apb_gpio.h"
#include "systimer.h"
#include <pinmux.h>

int main()
{
	systimer_init();
	printf("gpio test.\n\r");

	pinmux(56, 7);
	pinmux(57, 7);
	pinmux(58, 7);
	pinmux(59, 7);
	pinmux(60, 7);
	pinmux(61, 7);

	gpio_init_config_t gpio_init_config = {
	    .group = GROUP_GPIO1,
	    .pin = 24,
	    .gpio_control_mode = Software_Mode,
	    .gpio_mode = GPIO_Output_Mode
	};
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 25;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 26;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 27;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 28;
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 29;
	gpio_init(&gpio_init_config);


	while(1)
	{
		gpio_write_pin(GROUP_GPIO1, 24, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO1, 25, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO1, 26, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO1, 27, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO1, 28, GPIO_PIN_SET);
		gpio_write_pin(GROUP_GPIO1, 29, GPIO_PIN_SET);
		systimer_delay(500, IN_US);
		gpio_write_pin(GROUP_GPIO1, 24, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO1, 25, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO1, 26, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO1, 27, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO1, 28, GPIO_PIN_RESET);
		gpio_write_pin(GROUP_GPIO1, 29, GPIO_PIN_RESET);
		systimer_delay(500, IN_US);
	}
	return 0;
}