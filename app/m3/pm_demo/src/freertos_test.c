#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "pm.h"

#define TEST_SLEEP       0
#define TEST_STANDBY     1
#define TEST_HIBERNATION 0

static SemaphoreHandle_t pmSemaphore;
int32_t                  arg_flag = 1;

void task1(void *arg)
{
    pmSemaphore = xSemaphoreCreateBinary();
    pm_set_test_level(TEST_NONE); // test mode

    while (1) {
        printf("task1\n");
        if (xSemaphoreTake(pmSemaphore, portMAX_DELAY)) {
            printf("\nPM example start!\n");
            printf("Support 3 low power modes: sleep/standby/hibernation\n");

#if TEST_SLEEP
            /*enetr sleep test*/
            printf("Enter sleep mode, setup wakeup source irq&timer&button\n");
            /* 唤醒源配置初始化 */

            pm_enter_mode(PM_MODE_SLEEP);
            printf("Exit sleep mode\n\n");
            /* 唤醒源配置关闭 */
#endif
#if TEST_STANDBY
            /*enetr sleep test*/
            printf(
                "Enter standby mode, setup wakeup source irq&timer&button\n");
            /* 唤醒源配置初始化 */

            pm_enter_mode(PM_MODE_STANDBY);
            printf("Exit standby mode\n\n");
            /* 唤醒源配置关闭 */
#endif
#if TEST_HIBERNATION
            /*enetr sleep test*/
            printf("Enter hibernation mode, setup wakeup source "
                   "irq&timer&button\n");
            /* 唤醒源配置初始化 */

            pm_enter_mode(PM_MODE_HIBERNATION);
            printf("Exit hibernation mode\n\n");
            /* 唤醒源配置关闭 */
#endif
            // NVIC->ISPR[(((uint32_t)Interrupt0_IRQn) >> 5UL)] = (uint32_t)(1UL
            // << (((uint32_t)Interrupt0_IRQn) & 0x1FUL));
        }
        vTaskDelay(100);
    }
}

void single_func(void)
{
    printf("task2\n");
    vTaskDelay(1000);
    /* if(test_count++ < 1) */
    xSemaphoreGive(pmSemaphore);
}

void task2(void *arg)
{
    /* int32_t test_count = 0; */

    while (1) {
        single_func();
    }
}
