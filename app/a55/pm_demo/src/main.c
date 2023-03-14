#include "chip_mem_layout.h"
#include <stdio.h>
#include "mailbox.h"
#include "irq_ctrl.h"
#include "gic.h"
#include <stdlib.h>

enum {
    TEST_SLEEP = 1,
    TEST_STANDBY,
    TEST_HIBERNATION,
};

#define M3_SWITCH_TO_LINUX      0x10100000
#define A55_SWITCH_TO_RTOS      0x10200000
#define A55_TO_M3_SUSPEND       0x10300000
#define A55_TO_M3_REBOOT        0x10400000
#define A55_TO_M3_POWEROFF      0x10500000

#define A55_TO_M3_ACK           0x20100000
#define A55_TO_M3_RESPOND       0x20200000
#define M3_TO_A55_ACK           0x20300000
#define M3_TO_A55_RESPOND       0x20400000

#define DEFAULT_MAILBOX_CH      0x0
#define DEFAULT_MAILBOX_LOCKCH  0x0
#define DEFAULT_MAILBOX_TIMEOUT 0x10000
#define DEFAULT_MAILBOX_MASK    0xffff0000

// static void a2b_init()
// {
// MAILBOX_A2B->a2b_status =
// 0x0f; // Clear the interrupt by writing 1 to this bit.
// }

static inline void seehi_mailbox_lock(uint32_t ch)
{
    while (MAILBOX_LOCK->atomic_lock[ ch ] == 0)
        ;
}

static inline void seehi_mailbox_unlock(uint32_t ch)
{
    MAILBOX_LOCK->atomic_lock[ ch ] = 1;
}

static inline void a55_to_m3_suspend(void)
{
    uint32_t timeout        = DEFAULT_MAILBOX_TIMEOUT;
    uint32_t m3_to_a55_data = 0;
    uint32_t m3_to_a55_cmd  = 0;
    seehi_mailbox_lock(DEFAULT_MAILBOX_LOCKCH);
    a2b_send(DEFAULT_MAILBOX_CH, A55_TO_M3_SUSPEND,
             (A55_TO_M3_ACK | TEST_SLEEP));
    printf("DEFAULT_MAILBOX_CH, 0x%x A55_TO_M3_SUSPEND 0x%x data 0x%x\n",
           DEFAULT_MAILBOX_CH, A55_TO_M3_SUSPEND, (A55_TO_M3_ACK | TEST_SLEEP));
    seehi_mailbox_unlock(DEFAULT_MAILBOX_LOCKCH);

    while (1) {
        if ((MAILBOX_B2A->b2a_status & (0x1 << DEFAULT_MAILBOX_CH)) ==
            (0x1 << DEFAULT_MAILBOX_CH)) {
            m3_to_a55_data = a_get_data(DEFAULT_MAILBOX_CH);
            m3_to_a55_cmd  = a_get_cmd(DEFAULT_MAILBOX_CH);
        }

        if (m3_to_a55_cmd == A55_TO_M3_SUSPEND &&
            (m3_to_a55_data & DEFAULT_MAILBOX_MASK) == A55_TO_M3_RESPOND) {
            break;
        }

        if (timeout-- == 0) {
            printf(
                "%s line %d timerout m3_to_a55_data 0x%x, m3_to_a55_cmd 0x%x\n",
                __func__, __LINE__, a_get_data(DEFAULT_MAILBOX_CH),
                a_get_cmd(DEFAULT_MAILBOX_CH));
            break;
        }
    }

    MAILBOX_B2A->b2a_status = (0x1 << DEFAULT_MAILBOX_CH);
    printf(
        "MAILBOX_B2A->b2a_status 0x%x m3_to_a55_data 0x%x m3_to_a55_cmd 0x%x\n",
        MAILBOX_B2A->b2a_status, a_get_data(DEFAULT_MAILBOX_CH),
        a_get_cmd(DEFAULT_MAILBOX_CH));
}

static inline void a55_to_m3_reboot(void) {}

static inline void a55_to_m3_poweroff(void) {}

static inline void udelay(uint64_t x)
{
    for (uint32_t i = x; i != 0; i--)
        asm volatile("");
}

static volatile uint32_t mailbox_i = 0U;
void                     mailbox_irqhandler(void)
{
    uint32_t sta;
    sta = MAILBOX_B2A->b2a_status;
    for (int i = 0; i < 4; i++) {
        if (((sta >> i) & 1) == 1) {
            printf("A55 receive: cmd: %u  data: %u by ch%u\n\r", a_get_cmd(i),
                   a_get_data(i), i);
            switch (i) {
            case 0:
                MAILBOX_B2A->b2a_status = 0x01;
                break;
            case 1:
                MAILBOX_B2A->b2a_status = 0x02;
                break;
            case 2:
                MAILBOX_B2A->b2a_status = 0x04;
                break;
            case 3:
                MAILBOX_B2A->b2a_status = 0x08;
                break;
            default:
                break;
            }
        }
    }
}

static void a2b_init()
{
    MAILBOX_A2B->a2b_status =
        0xf; // Clear the interrupt by writing 1 to this bit.
             // GIC_SetTarget(Mailbox_IRQn, 1 << 0);
             // IRQ_SetHandler(Mailbox_IRQn, mailbox_irqhandler);
             // IRQ_SetPriority(Mailbox_IRQn, 0 << 3);
             // IRQ_Enable(Mailbox_IRQn);
}

uint32_t GIC_GetIRQStatus1(IRQn_Type IRQn) { return GIC_GetIRQStatus(IRQn); }

int main()
{
    IRQ_Initialize();
    a2b_init();

    GIC_GetIRQStatus1(Mailbox_IRQn);

    // for(uint32_t i = 0; i < 4; i++) {
    // a2b_send(i, i, i);
    // printf("A55 send cmd: %u, data: %u by ch%u\n\r", i, i, i);
    // }

    a55_to_m3_suspend();

    while (1) {
        asm volatile("nop");
    };
}
