#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "m3.h"
#include "pm.h"
#include "mailbox.h"

enum {
    TEST_SLEEP = 1,
    TEST_STANDBY,
    TEST_HIBERNATION,
};

#define M3_SWITCH_TO_LINUX     0x10100000
#define A55_SWITCH_TO_RTOS     0x10200000
#define A55_TO_M3_SUSPEND      0x10300000
#define A55_TO_M3_REBOOT       0x10400000
#define A55_TO_M3_POWEROFF     0x10500000

#define A55_TO_M3_ACK          0x20100000
#define A55_TO_M3_RESPOND      0x20200000
#define M3_TO_A55_ACK          0x20300000
#define M3_TO_A55_RESPOND      0x20400000

#define DEFAULT_MAILBOX_CH     0x0
#define DEFAULT_MAILBOX_LOCKCH 0x0
#define DEFAULT_MAILBOX_MASK   0xffff0000

static SemaphoreHandle_t pmSemaphore;
static int32_t           pm_mode = TEST_SLEEP;

void single_to_m3_suspend(void) { xSemaphoreGive(pmSemaphore); }

static inline void seehi_mailbox_lock(uint32_t ch)
{
    while (MAILBOX_LOCK->atomic_lock[ ch ] == 0)
        ;
}

static inline void seehi_mailbox_unlock(uint32_t ch)
{
    MAILBOX_LOCK->atomic_lock[ ch ] = 1;
}

static inline void a55_to_m3_irq_handler(void)
{
    uint32_t a55_to_m3_data = b_get_data(DEFAULT_MAILBOX_CH);

    switch (b_get_cmd(DEFAULT_MAILBOX_CH)) {
    case M3_SWITCH_TO_LINUX:
        break;
    case A55_SWITCH_TO_RTOS:
        break;
    case A55_TO_M3_SUSPEND:
        if ((a55_to_m3_data & DEFAULT_MAILBOX_MASK) == A55_TO_M3_ACK) {

            pm_mode = a55_to_m3_data & (DEFAULT_MAILBOX_MASK >> 16);
            single_to_m3_suspend();
			seehi_mailbox_lock(DEFAULT_MAILBOX_LOCKCH);
            b2a_send(DEFAULT_MAILBOX_CH, A55_TO_M3_SUSPEND, A55_TO_M3_RESPOND);
			seehi_mailbox_unlock(DEFAULT_MAILBOX_LOCKCH);
        } else {
            printf("pm modes param is err!!!\n");
        }
        break;
    case A55_TO_M3_REBOOT:
        break;
    case A55_TO_M3_POWEROFF:
        break;
    default:
        break;
    }

    MAILBOX_LOCK->atomic_lock[ DEFAULT_MAILBOX_LOCKCH ] = 1;
}

void mailbox_irq_handler(void)
{
    uint32_t status;
    status = MAILBOX_A2B->a2b_status;
    if (status & (0x1 < DEFAULT_MAILBOX_CH)) {              // ch0
        MAILBOX_A2B->a2b_status = 0x1 < DEFAULT_MAILBOX_CH; // clear irq
        a55_to_m3_irq_handler();
    } else {
    }
}

static inline void b2a_init()
{
    MAILBOX_A2B->a2b_int_en = 0x0f; // enable interrupt
    MAILBOX_B2A->b2a_status =
        0x0f; // Clear the interrupt by writing 1 to this bit.

    NVIC_SetPriority(Mailbox_IRQn, 0);
    NVIC_SetVector(Mailbox_IRQn, (uint32_t) mailbox_irq_handler);
    NVIC_EnableIRQ(Mailbox_IRQn);
}

void task1(void *arg)
{
    pmSemaphore = xSemaphoreCreateBinary();
    pm_set_test_level(TEST_NONE); // none mode

    printf("task1\n");
    while (1) {
        if (xSemaphoreTake(pmSemaphore, portMAX_DELAY)) {
            printf("\nPM example start!\n");
            printf("Support 3 low power modes: sleep/standby/hibernation\n");

            if (pm_mode == TEST_SLEEP) {
                /*enetr sleep test*/
                printf(
                    "Enter sleep mode, setup wakeup source irq&timer&button\n");
                /* 唤醒源配置初始化 */

                pm_enter_mode(PM_MODE_SLEEP);
                printf("Exit sleep mode\n\n");
                /* 唤醒源配置关闭 */
            } else if (pm_mode == TEST_STANDBY) {
                /*enetr sleep test*/
                printf("Enter standby mode, setup wakeup source "
                       "irq&timer&button\n");
                /* 唤醒源配置初始化 */

                pm_enter_mode(PM_MODE_STANDBY);
                printf("Exit standby mode\n\n");
                /* 唤醒源配置关闭 */
            } else if (pm_mode == TEST_HIBERNATION) {
                /*enetr sleep test*/
                printf("Enter hibernation mode, setup wakeup source "
                       "irq&timer&button\n");
                /* 唤醒源配置初始化 */

                pm_enter_mode(PM_MODE_HIBERNATION);
                printf("Exit hibernation mode\n\n");
                /* 唤醒源配置关闭 */
            } else {
                printf("pm_mode err!!!\n");
            }
            // NVIC->ISPR[(((uint32_t)Interrupt0_IRQn) >> 5UL)] = (uint32_t)(1UL
            // << (((uint32_t)Interrupt0_IRQn) & 0x1FUL));
        }
        vTaskDelay(100);
    }
}

void task2(void *arg)
{
    /* int32_t test_count = 0; */

    printf("task2\n");
    while (1) {
        // single_func();
        vTaskDelay(100);
    }
}
