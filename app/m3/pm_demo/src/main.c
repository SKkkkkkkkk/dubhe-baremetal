#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "axp2101.h"
#include "dw_apb_gpio.h"
#include "m3.h"
#include "mem_and_clock.h"
#include "pm.h"
#include "systimer.h"
#include "pinmux.h"
#include "pmu.h"
#include "i2c_wo.h"
#include "main.h"

#define TEST_SLEEP           0
#define TEST_STANDBY         1
#define TEST_HIBERNATION     0
#define STACK_SIZE           1024
#define Interrupt0_IRQn      0
#define Interrupt1_IRQn      1
#define Interrupt79_IRQn     79
#define GPIO0                GPIO0_BASE
#define STOP                 1

#define KEY_EINT_PIN         16
#define KEY_EINT_PIN_GROUP   (KEY_EINT_PIN / 32)
#define KEY_EINT_PIN_NUM     (KEY_EINT_PIN % 32)

#define ROUND_UP(x, align)   (((int) (x) + (align - 1)) & ~(align - 1))
#define ROUND_DOWN(x, align) ((int) (x) & ~(align - 1))

extern const VECTOR_TABLE_Type __VECTOR_TABLE[ 240 ];
__ramvector VECTOR_TABLE_Type  __VECTOR_TABLE_IN_SRAM[ 240 ];

__ramdata int          count = 0;
__ramdata StaticTask_t xTaskBuffer;
__ramdata StackType_t  xStack[ STACK_SIZE ];
__ramdata StackType_t  IdleTaskStack[ configMINIMAL_STACK_SIZE ];
__ramdata StaticTask_t IdleTaskTCB;
__ramdata StackType_t  TimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];
__ramdata StaticTask_t TimerTaskTCB;

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t  **ppxIdleTaskStackBuffer,
                                   uint32_t      *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer   = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t  **ppxTimerTaskStackBuffer,
                                    uint32_t      *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer   = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer = TimerTaskStack;
    *pulTimerTaskStackSize   = configTIMER_TASK_STACK_DEPTH;
}

__ramfunc void irq_handler0(void)
{
    count++;
    // printf("hello world in irq 0. %d\n", count);
    *(uint32_t *) (__EXTERNAL_RAM_BASE + 4) = count;
    return;
}

void irq_handler1(void)
{
    count++;
    MAIN_LOGD("hello world in irq 1. %d\n", count);
    return;
}

void irq_handler_gpio(void)
{
    uint32_t intstatus = REG32(GPIO0 + 0x40);

    count++;

    if (intstatus & (1 << KEY_EINT_PIN_NUM)) {
        gpio_clear_interrput(KEY_EINT_PIN_GROUP, KEY_EINT_PIN_NUM);
        MAIN_LOGD("gpio irq 79. %d intstatus 0x%lx\n", count, intstatus);
    } else {
        printf("gpio irq 79 intstatus 0x%lx\n", intstatus);
    }

    return;
}

void irq_handler_rtc(void)
{
    uint32_t intstatus = REG32(RTC_BASE + 0x200);

    REG32(RTC_BASE + 0x200) = 0;
    printf("rtc irq status 0x%lx\r\n", intstatus);
    count++;

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
    MAIN_LOGD("seehi--> %s.\n", __func__);
    return 0;
}

static int pm_resume(struct soc_device *dev, enum suspend_state_t state)
{
    MAIN_LOGD("seehi--> %s.\n", __func__);
    return 0;
}

static const struct soc_device_driver pm_drv = {
    .name    = "pm_drv",
    .suspend = pm_suspend,
    .resume  = pm_resume,
};

static struct soc_device pm_dev = {
    .name          = "pm_dev",
    .driver        = &pm_drv,
    .platform_data = NULL,
};

#define PM_DEV (&pm_dev)

static int pm_suspend_noirq(struct soc_device *dev, enum suspend_state_t state)
{
    MAIN_LOGD("seehi--> %s.\n", __func__);
    return 0;
}

static int pm_resume_noirq(struct soc_device *dev, enum suspend_state_t state)
{
    MAIN_LOGD("seehi--> %s.\n", __func__);
    return 0;
}

static const struct soc_device_driver pm_noirq_drv = {
    .name          = "pm_noirq_drv",
    .suspend_noirq = pm_suspend_noirq,
    .resume_noirq  = pm_resume_noirq,
};

static struct soc_device pm_noirq_dev = {
    .name          = "pm_noirq_dev",
    .driver        = &pm_noirq_drv,
    .platform_data = NULL,
};

#define PM_NOIRQ_DEV (&pm_noirq_dev)

__ramfunc static inline void _mdelay(unsigned long msec)
{
#define udelay(x)                \
    {                            \
        unsigned xx = x;         \
        while (xx--) {           \
            asm volatile("nop"); \
        }                        \
    }
    while (msec--)
        udelay(500);
}

__ramfunc void clear_ddr(void)
{
    extern int __bss_end__[];
    int        i    = 0;
    int        size = (uint32_t) __bss_end__ - __EXTERNAL_RAM_BASE;

    printf("ddr __bss_end__ 0x%lx size 0x%x up 0x%x\n", (uint32_t) __bss_end__,
           size, ROUND_UP(size, 128));

    for (i = 0; i < ROUND_UP(size, 128); i += 4) {
        *(uint32_t *) (__EXTERNAL_RAM_BASE + i) = 0;
    }

    i = 0;
    while (1) {
        _mdelay(500);
        *(uint32_t *) (__EXTERNAL_RAM_BASE) = i++;
        __DSB();
        __ISB();
        NVIC->ISPR[ (((uint32_t) Interrupt0_IRQn) >> 5UL) ] =
            (uint32_t) (1UL << (((uint32_t) Interrupt0_IRQn) & 0x1FUL));
    }
}

#if 0
    struct pmic_cfg cfg;

    strcpy(cfg.name, "axp2101");
    cfg.reg_addr   = 0x34;
    cfg.i2c_bus    = 2;
    cfg.check_addr = 0x00;
    cfg.check_len  = 1;

    // err = axp2101_i2c_init(&cfg);

    if (!err) {
        axp20x_i2c_write(AXP2101_INTEN1, 0);
        axp20x_i2c_write(AXP2101_INTEN2, 0);
        axp20x_i2c_write(AXP2101_INTEN3, 0);
        axp2101_pmic_ops.pmic_to_sleep  = axp2101_powerkey_suspend;
        axp2101_pmic_ops.pmic_to_resume = axp2101_powerkey_resume;
    }
#endif

int pmic_to_sleep_delay(int ms)
{
    uint32_t tmp;
    tmp = REG32(SYS_BASE + 0x8dc);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8dc) = tmp;

    tmp = REG32(SYS_BASE + 0x8e0);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8e0) = tmp;
    i2c_wo_init(0x34);

    i2c_wo_delay(ms); // 1s
    // i2c_wo_fifo(0x10);  //poweroff
    // i2c_wo_fifo(0x01 | STOP<<8);
    i2c_wo_fifo(AXP2101_INTEN1);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(AXP2101_INTEN2);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(AXP2101_INTEN3);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x26);
    i2c_wo_fifo(0x09 | STOP << 8);
    i2c_wo_fifo(0x80);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x90);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x91);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x26);
    i2c_wo_fifo(0x19 | STOP << 8);
    i2c_wo_fifo(0x41);
    i2c_wo_fifo(0x0c | STOP << 8);
    i2c_wo_start();

    return 0;
}

int pmic_clear_irq_wo(int ms)
{
    uint32_t tmp;
    tmp = REG32(SYS_BASE + 0x8dc);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8dc) = tmp;

    tmp = REG32(SYS_BASE + 0x8e0);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8e0) = tmp;
    i2c_wo_init(0x34);

    i2c_wo_delay(ms); // 1s
    // i2c_wo_fifo(0x10);  //poweroff
    // i2c_wo_fifo(0x01 | STOP<<8);
    i2c_wo_fifo(AXP2101_INTEN1);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(AXP2101_INTEN2);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(AXP2101_INTEN3);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_start();

    return 0;
}

void set_gpio_wakeup(void)
{
    pinmux(KEY_EINT_PIN, 7); // gpio0_10

    gpio_init_config_t gpio_init_config = {
        .group             = KEY_EINT_PIN_GROUP,
        .pin               = KEY_EINT_PIN,
        .gpio_control_mode = Software_Mode,
        .gpio_mode         = GPIO_Falling_Int_Mode,
        // .gpio_mode = GPIO_Input_Mode,
    };
    gpio_init(&gpio_init_config);
}

void set_power_off_seq(void)
{
    // set_pmu_reg(PMU, PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,
    // ~(1 << PMU_IMR_PMU_WAKEUP_1_MASK_LSB |
    // 1 << PMU_IMR_PPU_LP_IRQ_MASK_LSB));
    // set_pmu_reg(PMU,PMU_PDSEQ_0_LOGICID_ADDR,(PERI0 << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_0_LOGICID_ADDR, (DDR1 << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_1_LOGICID_ADDR, (DDR0 << 8 | OFF));
    set_pmu_reg(PMU, PMU_PDSEQ_2_LOGICID_ADDR, (LP << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_0_LOGICID_ADDR,(PERI1 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_4_LOGICID_ADDR,(CORE3 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_5_LOGICID_ADDR,(CORE2 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_6_LOGICID_ADDR,(CORE1 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_7_LOGICID_ADDR,(CORE0 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_8_LOGICID_ADDR,(AP << 8 | OFF));
    set_pmu_reg(PMU, PMU_PD_CR_NUM_PD_ADDR, (3 << 4 | 1));
}

int pmu_enter_suspend(void)
{
    MAIN_LOGD("%s enter\n", __func__);
    set_pmu_wakeup(1, 0x2); // set wakeup gpio16 target:lp
    set_pmu_wakeup(3, 0x2); // set wakeup rtc target:lp

    set_power_off_seq();

	i2c_wo_appower_enable(0);
    pmic_to_sleep_delay(500);

    timer_disable(Timerx2_T1);

    return 0;
}

int pmu_enter_resume(void)
{
    MAIN_LOGD("%s enter\n", __func__);
    set_pmu_wakeup_clear(1, 0x2);
    set_pmu_wakeup_clear(3, 0x2);

	i2c_wo_appower_enable(0);
    pmic_clear_irq_wo(1);

    timer_enable(Timerx2_T1);

    global_set_power_on_a55(AP, OP4);
    global_set_power_on_a55(CORE0, OP4);

    return 0;
}

int pmu_enter_suspend_core0(void)
{
    MAIN_LOGD("%s enter\n", __func__);
    timer_disable(Timerx2_T1);
    return 0;
}

int pmu_enter_resume_core0(void)
{
    MAIN_LOGD("%s enter\n", __func__);
    timer_enable(Timerx2_T1);

    global_set_power_on_a55(AP, OP4);
    global_set_power_on_a55(CORE0, OP4);
    return 0;
}

int pmu_enter_poweroff(void)
{
    MAIN_LOGD("%s enter\n", __func__);
    set_pmu_wakeup(1, 0x2); // set wakeup gpio16 target:lp
    set_pmu_wakeup(3, 0x2); // set wakeup rtc target:lp

    set_power_off_seq();
    pmic_to_sleep_delay(500);

    return 0;
}

int main()
{
    MAIN_LOGD("test %s ...\n", __FILE__);

    extern char __ram_start, __RAM_DATA_LMA_START__;
    extern char __RAM_DATA_SIZE__;
    int         size = (uint32_t) &__RAM_DATA_SIZE__;
    memcpy(&__ram_start, &__RAM_DATA_LMA_START__, size);

    memcpy(__VECTOR_TABLE_IN_SRAM, __VECTOR_TABLE,
           sizeof(__VECTOR_TABLE_IN_SRAM));
    __DSB();
    SCB->VTOR = (uint32_t) & (__VECTOR_TABLE_IN_SRAM[ 0 ]);
    __DSB();
    __ISB();

    NVIC_SetVector(Interrupt0_IRQn, (uintptr_t) irq_handler0);
    NVIC_EnableIRQ(Interrupt0_IRQn);
    NVIC_SetPendingIRQ(Interrupt0_IRQn);

    NVIC_SetVector(Interrupt1_IRQn, (uintptr_t) irq_handler1);
    NVIC_EnableIRQ(Interrupt1_IRQn);
    NVIC_SetPendingIRQ(Interrupt1_IRQn);

    NVIC_SetVector(GPIO0_IRQn, (uintptr_t) irq_handler_gpio);
    NVIC_EnableIRQ(GPIO0_IRQn);

    NVIC_SetVector(RTC_IRQn, (uintptr_t) irq_handler_rtc);
    NVIC_EnableIRQ(RTC_IRQn);

    hardware_init_hook();

    pm_register_ops(PM_DEV);
    pm_register_ops(PM_NOIRQ_DEV);
    //
    void task1(void *arg);
    void task2(void *arg);

    set_gpio_wakeup();
    // clear_ddr();

    // pm_ops.pm_to_poweroff = pmu_enter_poweroff;
    // pm_ops.pm_to_sleep  = pmu_enter_suspend;
    pm_ops.pm_to_sleep  = pmu_enter_suspend_core0;
    pm_ops.pm_to_resume = pmu_enter_resume_core0;

    if (xTaskCreateStatic(task1, "task1", STACK_SIZE, NULL, 1, xStack,
                          &xTaskBuffer) == NULL)
        while (1)
            ;

    if (xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE, NULL, 1, NULL) !=
        pdPASS)
        while (1)
            ;
    vTaskStartScheduler();

    return 0;
}
