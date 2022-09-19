#include "dw_apb_gpio.h"
#include "dw_apb_gpio_regs.h"

#ifdef A55
	#define PIN_IOMUX_BASE 0x2e000800

	#define GPIO0_BASE (0x210A0000UL)
	#define GPIO1_BASE (0x27000000UL)
	#define GPIO2_BASE (0x27010000UL)
	#define GPIO3_BASE (0x27020000UL)
#else
	#define PIN_IOMUX_BASE 0x4e000800

	#define GPIO0_BASE (0x410A0000UL)
	#define GPIO1_BASE (0x47000000UL)
	#define GPIO2_BASE (0x47010000UL)
	#define GPIO3_BASE (0x47020000UL)
#endif



#define GPIO0 ((DW_APB_GPIO_TypeDef *)GPIO0_BASE)
#define GPIO1 ((DW_APB_GPIO_TypeDef *)GPIO1_BASE)
#define GPIO2 ((DW_APB_GPIO_TypeDef *)GPIO2_BASE)
#define GPIO3 ((DW_APB_GPIO_TypeDef *)GPIO3_BASE)

#if 1
	#define GPIO_LOCK_MUTEX(x)
	#define GPIO_UNLOCK_MUTEX(x)
	static uint32_t mutex_for_iomux;
	static uint32_t mutex_for_gpio_init[4];
	static uint32_t mutex_for_gpio_write;
	void gpio_mutexs_init(void)
	{
		(void)mutex_for_iomux;
		(void)mutex_for_gpio_init;
		(void)mutex_for_gpio_write;
		return;
	}
#else
	#include "FreeRTOS.h"
	#include "semphr.h"
	static SemaphoreHandle_t mutex_for_iomux;
	static SemaphoreHandle_t mutex_for_gpio_init[3];
	static SemaphoreHandle_t mutex_for_gpio_write;
 	void gpio_mutexs_init(void)
	{
		static bool init = false;
		if(init==true) return;
		mutex_for_iomux = xSemaphoreCreateMutex();
		mutex_for_gpio_init[0] = xSemaphoreCreateMutex();
		mutex_for_gpio_init[1] = xSemaphoreCreateMutex();
		mutex_for_gpio_init[2] = xSemaphoreCreateMutex();
		mutex_for_gpio_write = xSemaphoreCreateMutex();
		configASSERT(mutex_for_iomux!=NULL);
		configASSERT(mutex_for_gpio_init[0]!=NULL);
		configASSERT(mutex_for_gpio_init[1]!=NULL);
		configASSERT(mutex_for_gpio_init[2]!=NULL);
		configASSERT(mutex_for_gpio_write!=NULL);
		init = true;
		return;
	}

	#define GPIO_LOCK_MUTEX(x) 	xSemaphoreTake( x, portMAX_DELAY )
	#define GPIO_UNLOCK_MUTEX(x) xSemaphoreGive( x )
#endif

void pin_set_iomux(gpio_group_t group, uint8_t pin, uint8_t iomux)
{
	uint8_t num = group * 32 + pin;

	uint32_t base = (num / 2) * 4 + PIN_IOMUX_BASE;

	GPIO_LOCK_MUTEX(mutex_for_iomux);
	uint32_t tmp = REG32(base);
	if (num % 2 == 0) {
		tmp &= ~(3 << 1);
		tmp |= iomux << 1;
		tmp |= 1 << 0;
	} else {
		tmp &= ~(3 << 17);
		tmp |= iomux << 17;
		tmp |= 1 << 16;
	}
	REG32(base) = tmp;
	GPIO_UNLOCK_MUTEX(mutex_for_iomux);
}

void gpio_init(gpio_init_config_t const *const gpio_init_config)
{
	DW_APB_GPIO_TypeDef *gpio;
	#if 1
		uint32_t mutex_for_gpio_init1;
		(void)mutex_for_gpio_init1;
	#else
		SemaphoreHandle_t mutex_for_gpio_init1;
	#endif
	uint8_t pin = gpio_init_config->pin;
	switch (gpio_init_config->group) {
	case GROUP_GPIO0:
		gpio = GPIO0;
		mutex_for_gpio_init1 = (mutex_for_gpio_init[0]);
		break;

	case GROUP_GPIO1:
		gpio = GPIO1;
		mutex_for_gpio_init1 = (mutex_for_gpio_init[1]);
		break;

	case GROUP_GPIO2:
		gpio = GPIO2;
		mutex_for_gpio_init1 = (mutex_for_gpio_init[2]);
		break;
	
	case GROUP_GPIO3:
		gpio = GPIO3;
		mutex_for_gpio_init1 = (mutex_for_gpio_init[3]);
		break;

	default:
		return;
	}

	GPIO_LOCK_MUTEX(mutex_for_gpio_init1);
	//software or hardware mode
	uint32_t tmp = gpio->CTL;
	if (gpio_init_config->gpio_control_mode == Software_Mode)
		tmp &= ~(1 << pin);
	else
		tmp |= 1 << pin;
	gpio->CTL = tmp;

	//set direction
	uint32_t gpio_mode = gpio_init_config->gpio_mode;
	switch (gpio_mode) {
	case GPIO_Output_Mode:
		gpio->DDR |= 1 << pin;
		break;
	case GPIO_Rising_Int_Mode:
		gpio->INTTYPE_LEVEL |= (1 << pin);
		gpio->INT_POLARITY |= (1 << pin);
		gpio->INT_BOTHEDGE &= ~(1 << pin);
		break;
	case GPIO_Falling_Int_Mode:
		gpio->INTTYPE_LEVEL |= (1 << pin);
		gpio->INT_POLARITY &= ~(1 << pin);
		gpio->INT_BOTHEDGE &= ~(1 << pin);
		break;
	case GPIO_Low_Int_Mode:
		gpio->INTTYPE_LEVEL &= ~(1 << pin);
		gpio->INT_POLARITY &= ~(1 << pin);
		gpio->INT_BOTHEDGE &= ~(1 << pin);
		break;
	case GPIO_High_Int_Mode:
		gpio->INTTYPE_LEVEL &= ~(1 << pin);
		gpio->INT_POLARITY |= (1 << pin);
		gpio->INT_BOTHEDGE &= ~(1 << pin);
		break;
	case GPIO_Edge_Int_Mode:
		gpio->INT_BOTHEDGE |= (1 << pin);
		break;
	case GPIO_Input_Mode:
		gpio->DDR &= ~(1 << pin);
		break;
	default:
		GPIO_UNLOCK_MUTEX(mutex_for_gpio_init1);
		return;
	}

	if ((gpio_mode != GPIO_Input_Mode) && (gpio_mode != GPIO_Output_Mode)) {
		gpio->DDR &= ~(1 << pin);

		gpio->DEBOUNCE |= (1 << pin);
		gpio->INTMASK &= ~(1 << pin);
		gpio->INTEN |= (1 << pin);
	}
	GPIO_UNLOCK_MUTEX(mutex_for_gpio_init1);
	return;
}

gpio_pin_state_t gpio_read_pin(gpio_group_t group, uint8_t pin)
{
	DW_APB_GPIO_TypeDef *gpio;
	switch (group) {
	case GROUP_GPIO0:
		gpio = GPIO0;
		break;

	case GROUP_GPIO1:
		gpio = GPIO1;
		break;

	case GROUP_GPIO2:
		gpio = GPIO2;
		break;

	case GROUP_GPIO3:
		gpio = GPIO3;
		break;

	default:
		return GPIO_PIN_RESET;
	}

	//输出
	return ((gpio->EXT_PORTA & (1 << pin)) == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET;
}

void gpio_write_pin(gpio_group_t group, uint8_t pin, gpio_pin_state_t pin_state)
{
	DW_APB_GPIO_TypeDef *gpio;
	switch (group) {
	case GROUP_GPIO0:
		gpio = GPIO0;
		break;

	case GROUP_GPIO1:
		gpio = GPIO1;
		break;

	case GROUP_GPIO2:
		gpio = GPIO2;
		break;
	
	case GROUP_GPIO3:
		gpio = GPIO3;
		break;

	default:
		return;
	}

	//输出
	GPIO_LOCK_MUTEX(mutex_for_gpio_write);
	uint32_t tmp = gpio->DR;
	if (pin_state == GPIO_PIN_RESET)
		tmp &= ~(1 << pin);
	else
		tmp |= 1 << pin;
	gpio->DR = tmp;
	GPIO_UNLOCK_MUTEX(mutex_for_gpio_write);
}

void gpio_clear_interrput(gpio_group_t group, uint8_t pin)
{
	DW_APB_GPIO_TypeDef *gpio;
	switch (group) {
	case GROUP_GPIO0:
		gpio = GPIO0;
		break;

	case GROUP_GPIO1:
		gpio = GPIO1;
		break;

	case GROUP_GPIO2:
		gpio = GPIO2;
		break;

	case GROUP_GPIO3:
		gpio = GPIO3;
		break;

	default:
		return;
	}

	gpio->PORTA_EOI = (1 << pin);
}