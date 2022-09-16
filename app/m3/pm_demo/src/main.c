#include "m3.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "pm.h"

#define TEST_SLEEP          1
#define TEST_STANDBY        1
#define TEST_HIBERNATION    1

void hardware_init_hook(void)
{
	/* 时钟相关 */
	pm_init();
	/* 判断启动源 */
	/* 中断相关 */
	/* RTC相关 */
}

static int pm_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	printf("%s.\n\r", __func__);
	return 0;
}

static int pm_resume(struct soc_device *dev, enum suspend_state_t state)
{
	printf("%s.\n\r", __func__);
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
	printf("%s.\n\r", __func__);
	return 0;
}

static int pm_resume_noirq(struct soc_device *dev, enum suspend_state_t state)
{
	printf("%s.\n\r", __func__);
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


int main()
{
	pm_register_ops(PM_DEV);
	pm_register_ops(PM_NOIRQ_DEV);

	void task1(void* arg);
	void task2(void* arg);
	if (xTaskCreate(task1, "task1", 1024, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	if (xTaskCreate(task2, "task2", 1024, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	vTaskStartScheduler();
	return 0;
}
