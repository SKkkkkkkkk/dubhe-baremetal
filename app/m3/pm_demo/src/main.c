#include "m3.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "pm.h"
#include "mem_and_clock.h"
#include "axp2101.h"
#include "systimer.h"
#include "dw_apb_gpio.h"

#define TEST_SLEEP          0
#define TEST_STANDBY        1
#define TEST_HIBERNATION    0
#define STACK_SIZE			1024
#define ROUND_UP(x, align) (((int) (x) + (align - 1)) & ~(align - 1))
#define ROUND_DOWN(x, align) ((int)(x) & ~(align - 1))

extern const VECTOR_TABLE_Type __VECTOR_TABLE[240];
__ramvector VECTOR_TABLE_Type __VECTOR_TABLE_IN_SRAM[240];

__ramdata int count = 0;
__ramdata StaticTask_t xTaskBuffer;
__ramdata StackType_t xStack[STACK_SIZE];
__ramdata StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
__ramdata StaticTask_t IdleTaskTCB;
__ramdata StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];
__ramdata StaticTask_t TimerTaskTCB;


void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
									StackType_t **ppxIdleTaskStackBuffer,
									uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&IdleTaskTCB;
	*ppxIdleTaskStackBuffer=IdleTaskStack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
									StackType_t **ppxTimerTaskStackBuffer,
									uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer=&TimerTaskTCB;
	*ppxTimerTaskStackBuffer=TimerTaskStack;
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}

__ramfunc void irq_handler0(void)
{
	count++;
	// printf("hello world in irq 0. %d\n", count);
	*(uint32_t *)(__EXTERNAL_RAM_BASE + 4) = count;
	return;
}

void irq_handler1(void)
{
	count++;
	printf("hello world in irq 1. %d\n", count);
	return;
}

void hardware_init_hook(void)
{
	/* 时钟相关 */
	systimer_init();
	pm_init();
	/* 判断启动源 */
	/* 中断相关 */
	/* RTC相关 */
}

static int pm_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	printf("--> %s.\n", __func__);
	return 0;
}

static int pm_resume(struct soc_device *dev, enum suspend_state_t state)
{
	printf("--> %s.\n", __func__);
	return 0;
}

static const struct soc_device_driver pm_drv = {
	.name = "pm_drv",
	.suspend = pm_suspend,
	.resume = pm_resume,
};

static struct soc_device pm_dev = {
	.name = "pm_dev",
	.driver = &pm_drv,
	.platform_data = NULL,
};

#define PM_DEV (&pm_dev)

static int pm_suspend_noirq(struct soc_device *dev, enum suspend_state_t state)
{
	printf("--> %s.\n", __func__);
	return 0;
}

static int pm_resume_noirq(struct soc_device *dev, enum suspend_state_t state)
{
	printf("--> %s.\n", __func__);
	return 0;
}

static const struct soc_device_driver pm_noirq_drv = {
	.name = "pm_noirq_drv",
	.suspend_noirq = pm_suspend_noirq,
	.resume_noirq = pm_resume_noirq,
};

static struct soc_device pm_noirq_dev = {
	.name = "pm_noirq_dev",
	.driver = &pm_noirq_drv,
	.platform_data = NULL,
};

#define PM_NOIRQ_DEV (&pm_noirq_dev)

__ramfunc static inline void _mdelay(unsigned long msec)
{
#define udelay(x)                                                                                                      \
	{                                                                                                              \
		unsigned xx = x;                                                                                       \
		while (xx--) {                                                                                         \
			asm volatile("nop");                                                                           \
		}                                                                                                      \
	}
	while (msec--)
		udelay(500);
}

__ramfunc void clear_ddr(void)
{

	extern int __bss_end__[];
	int i = 0;
	int size = (uint32_t)__bss_end__ - __EXTERNAL_RAM_BASE;

	printf("ddr __bss_end__ 0x%lx size 0x%x up 0x%x\n", (uint32_t)__bss_end__, size, ROUND_UP(size, 128));

	for(i=0; i<ROUND_UP(size, 128); i+=4){
		*(uint32_t *)(__EXTERNAL_RAM_BASE + i) = 0;
	}

	i = 0;
	while(1){
		_mdelay(500);
		*(uint32_t *)(__EXTERNAL_RAM_BASE) = i++;
		__DSB();
		__ISB();
		NVIC->ISPR[(((uint32_t)Interrupt0_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)Interrupt0_IRQn) & 0x1FUL));
	}
}

int main()
{
	memcpy(__VECTOR_TABLE_IN_SRAM, __VECTOR_TABLE, sizeof(__VECTOR_TABLE_IN_SRAM));
	__DSB();
	SCB->VTOR = (uint32_t) &(__VECTOR_TABLE_IN_SRAM[0]);
	__DSB();
	__ISB();

	NVIC_SetVector(Interrupt0_IRQn, (uintptr_t)irq_handler0);
	NVIC_EnableIRQ(Interrupt0_IRQn);
	NVIC_SetPendingIRQ(Interrupt0_IRQn);

	NVIC_SetVector(Interrupt1_IRQn, (uintptr_t)irq_handler1);
	NVIC_EnableIRQ(Interrupt1_IRQn);
	NVIC_SetPendingIRQ(Interrupt1_IRQn);

	hardware_init_hook();

	pm_register_ops(PM_DEV);
	pm_register_ops(PM_NOIRQ_DEV);
//
	void task1(void* arg);
	void task2(void* arg);

	// clear_ddr();

	pin_set_iomux(GROUP_GPIO0, 10, 3);
	pin_set_iomux(GROUP_GPIO0, 11, 3);

	gpio_init_config_t gpio_init_config = {
	    .group = GROUP_GPIO0,
	    .pin = 10,
	    .gpio_control_mode = Software_Mode,
	    .gpio_mode = GPIO_Output_Mode
	};
	gpio_init(&gpio_init_config);

	gpio_init_config.pin = 11;
	gpio_init(&gpio_init_config);

	struct pmic_cfg cfg;
	strcpy(cfg.name, "axp2101");
	cfg.reg_addr = 0x34;
	cfg.i2c_bus = 2;
	cfg.check_addr = 0x00;
	cfg.check_len = 1;
	axp2101_i2c_init(&cfg);

	u32 val = 0;
	u32 count = 0;
	axp20x_i2c_write(AXP2101_INTEN1, 0);
	axp20x_i2c_write(AXP2101_INTEN2, 0);
	axp20x_i2c_write(AXP2101_INTEN3, 0);
	axp20x_i2c_read(AXP2101_INTEN1, &val);
	printf("AXP2101_INTEN1 = 0x%x\n", val);
	axp20x_i2c_read(AXP2101_INTEN2, &val);
	printf("AXP2101_INTEN2 = 0x%x\n", val);
	axp20x_i2c_read(AXP2101_INTEN3, &val);
	printf("AXP2101_INTEN3 = 0x%x\n", val);
	axp2101_powerkey_suspend();
	// axp2101_powerkey_resume();

	while(1){
		systimer_delay(1, IN_S);
		axp20x_i2c_read(AXP2101_SLEEP_CFG, &val);
		printf("AXP2101_SLEEP_CFG = 0x%x\n", val);
		// axp20x_i2c_read(AXP2101_INTEN2, &val);
		// printf("AXP2101_INTEN2 = 0x%x\n", val);
		// axp20x_i2c_read(AXP2101_INTSTS2, &val);
		// printf("AXP2101_INTSTS2 = 0x%x\n", val);
		axp20x_i2c_read(AXP2101_DCDC_CFG0, &val);
		printf("AXP2101_DCDC_CFG0 = 0x%x\n", val);
		axp20x_i2c_read(AXP2101_LDO_EN_CFG0, &val);
		printf("AXP2101_LDO_EN_CFG0 = 0x%x\n", val);
		axp20x_i2c_read(AXP2101_LDO_EN_CFG1, &val);
		printf("AXP2101_LDO_EN_CFG1 = 0x%x\n", val);
		axp20x_i2c_read(AXP2101_INTEN1, &val);
		printf("AXP2101_INTEN1 = 0x%x\n", val);
		axp20x_i2c_read(AXP2101_INTEN2, &val);
		printf("AXP2101_INTEN2 = 0x%x\n", val);
		axp20x_i2c_read(AXP2101_INTEN3, &val);
		printf("AXP2101_INTEN3 = 0x%x\n", val);
		axp20x_i2c_read(AXP2101_INTSTS1, &val);
		printf("AXP2101_INTSTS1 = 0x%x\n", val);
		axp20x_i2c_read(AXP2101_INTSTS2, &val);
		printf("AXP2101_INTSTS2 = 0x%x\n", val);
		axp20x_i2c_read(AXP2101_INTSTS3, &val);
		printf("AXP2101_INTSTS3 = 0x%x\n", val);

		axp20x_i2c_read(AXP2101_INTSTS2, &val);
		if((val &= 0x08) != 0){
			count++;
		}

		if(count == 10){
			printf("//////////count = %d/////////////\n", count);
			// axp20x_i2c_write(AXP2101_SLEEP_CFG, 0x02);
			axp20x_i2c_write(AXP2101_INTSTS2, 0x08);
			printf("//////////count = %d/////////////\n", count);
			count = 0;
		}
		// gpio_write_pin(GROUP_GPIO0, 10, GPIO_PIN_SET);
		// gpio_write_pin(GROUP_GPIO0, 11, GPIO_PIN_SET);
		// systimer_delay(500, IN_US);
		// gpio_write_pin(GROUP_GPIO0, 10, GPIO_PIN_RESET);
		// gpio_write_pin(GROUP_GPIO0, 11, GPIO_PIN_RESET);
		// systimer_delay(500, IN_US);
	}
	// if (xTaskCreateStatic(task1, "task1", STACK_SIZE, NULL, 1, xStack, &xTaskBuffer) == NULL)
		// while (1);

	// if (xTaskCreate(task2, "task2", 512, NULL, 1, NULL) != pdPASS)
		// while (1);
	// vTaskStartScheduler();
	return 0;
}
