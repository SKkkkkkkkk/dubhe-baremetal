/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m3.h"
#include "core_cm3.h"

#include "_pm_define.h"
#include "pm.h"
#include "pm_i.h"
#include "port.h"
#include "regs_type.h"
#include "sys/list.h"

#ifdef CONFIG_PM

extern void cpu_tz_suspend(void);

struct pmic_callback_ops axp2101_pmic_ops;

#ifdef CONFIG_PM_DEBUG
static struct arm_CMX_core_regs vault_arm_registers;
#endif

#define NVIC_SCR            (0xe000e000 + 0xd10)
#define NVIC_SYSTICK_CTRL   (0xe000e000 + 0x010)
#define NVIC_ICSR           (0xe000e000 + 0xd04)
#define NVIC_PEND_ACTIVE1   (0xe000e000 + 0x200)
#define NVIC_PEND_ACTIVE2   (0xe000e000 + 0x204)
#define NVIC_PEND_CLEAR1    (0xe000e000 + 0x280)
#define NVIC_PEND_CLEAR2    (0xe000e000 + 0x284)

#define PM_TIMEOFDAY_SAVE() // timeofday_save()

#define OS_WAIT_FOREVER     0xffffffffU /* Wait forever timeout value */

__ramdata unsigned int g_value;
__ramdata unsigned int g_msp;
__ramdata unsigned int g_psp;
__ramdata unsigned int g_primask;
__ramdata unsigned int g_faultmask;
__ramdata unsigned int g_basepri;
__ramdata unsigned int g_control;
__ramdata unsigned int g_systick;

static uint32_t _pm_tmo = OS_WAIT_FOREVER;

static int __suspend_begin(enum suspend_state_t state)
{
    PM_LOGD("--> %s line %d\n", __func__, __LINE__);
    /* set SEVONPEND flag */
    SCB->SCR = 0x10;

    return 0;
}

/* hibernation whole system, save user data to flash before call this func.
 * BE SUURE: all hardware has been closed and it's prcm config setted to default
 * value.
 */
#if 1
static void pm_hibernation(void)
{
    __record_dbg_status(PM_HIBERNATION | 0);

    /* step 1 & 2 has been done when wlan sys poweroff */
    /* step3: writel(0x0f, GPRCM_SYS1_WAKEUP_CTRL) to tell PMU that turn on
     * SW1, SW2, SRSW1, LDO before release application system reset signal.
     */

    /* 配置PMU相关寄存器 */
    /* 配置PMIC进入sleep模式 */
    __record_dbg_status(PM_HIBERNATION | 5);

    /* step4: writel(0x0f, GPRCM_SYS1_SLEEP_CTRL) to tell PMU that turn off SW1,
     * SW3 SRSW1, LDO after pull down application system reset signal.
     */
    __record_dbg_status(PM_HIBERNATION | 7);

    /* step5: switch to HOSC, close SYS1_CLK. */
    __record_dbg_status(PM_HIBERNATION | 9);

    /* step6: set nvic deepsleep flag, and enter wfe. */
    SCB->SCR = 0x14;
    /* 配置启动方式为cold_reset */
    /* 保存一个时间 */
    PM_TIMEOFDAY_SAVE();

    __disable_fault_irq();
    __disable_irq();

    /* 检查是否有唤醒中断 */
    if (check_wakeup_irqs()) {
        PM_REBOOT();
    }

    wfe();
    if (check_wakeup_irqs()) {
        PM_REBOOT();
    }

    extern void cpu_tz_hibernation(void);
    cpu_tz_hibernation();

    wfe();
    /* some irq generated when second wfe */
    PM_REBOOT();

    __record_dbg_status(PM_HIBERNATION | 0x0ff);
}
#endif

#if 1
__ramfunc static void cpu_sleep(void)
{

    /* disable systick */
    g_value = REG32(NVIC_SYSTICK_CTRL);
    // printf("NVIC_SYSTICK_CTRL = 0x%x\n", g_value);
    g_systick = g_value;
    asm volatile("isb 0xF" ::: "memory");
    REG32(NVIC_SYSTICK_CTRL) = g_value & ~0x03;

    /* clear systick */
    asm volatile("isb 0xF" ::: "memory");
    g_value = REG32(NVIC_ICSR);
    // printf("NVIC_ICSR = 0x%x\n", g_value);
    asm volatile("isb 0xF" ::: "memory");
    REG32(NVIC_ICSR) = (g_value & 0x4000000) >> 1;
    asm volatile("isb 0xF" ::: "memory");

    /* switch to 24M/div */
    // LDR R1, =GPRCM_SYSCLK1_CTRLS          //TODO
    // LDR R0, [R1]
    // BIC R0, R0, #0x30000
    // ORR R0, R0, #0x10000
    // STR R0, [R1]
    // DSB
    // ISB

    asm volatile("wfi" ::: "memory");
    asm volatile("wfi" ::: "memory");
    asm volatile("wfi" ::: "memory");

    /* switch cpu clk to pll */
    // LDR R1, =GPRCM_SYSCLK1_CTRLS       //TODO
    // LDR R0, [R1]
    // BIC R0, R0, #0x30000
    // ORR R0, R0, #0x20000
    // STR R0, [R1]
    // DSB
    // ISB

    asm volatile("isb 0xF" ::: "memory");
    /* enable systick */
    REG32(NVIC_SYSTICK_CTRL) = g_systick;
    asm volatile("isb 0xF" ::: "memory");
}
#endif

#if 1
__ramfunc static void cpu_suspend(void)
{
    cpu_tz_suspend();

    asm volatile("mrs %0, MSP" : "=r"(g_value));
    // printf("g_msp = 0x%x\n", g_value);
    g_msp = g_value;
    asm volatile("isb 0xF" ::: "memory");

    asm volatile("mrs %0, PSP" : "=r"(g_value));
    // printf("g_psp = 0x%x\n", g_value);
    g_psp = g_value;
    asm volatile("isb 0xF" ::: "memory");

    asm volatile("mrs %0, PRIMASK" : "=r"(g_value));
    // printf("PRIMASK = 0x%x\n", g_value);
    g_primask = g_value;
    asm volatile("isb 0xF" ::: "memory");

    asm volatile("mrs %0, FAULTMASK" : "=r"(g_value));
    // printf("FAULTMASK = 0x%x\n", g_value);
    g_faultmask = g_value;
    asm volatile("isb 0xF" ::: "memory");

    asm volatile("mrs %0, BASEPRI" : "=r"(g_value));
    // printf("BASEPRI = 0x%x\n", g_value);
    g_basepri = g_value;
    asm volatile("isb 0xF" ::: "memory");

    asm volatile("mrs %0, CONTROL" : "=r"(g_value));
    // printf("CONTROL = 0x%x\n", g_value);
    g_control = g_value;
    asm volatile("isb 0xF" ::: "memory");

    /* set deepsleep mode */
    REG32(NVIC_SCR) = 0x14;
    asm volatile("isb 0xF" ::: "memory");

    /* disable systick */
    g_value = REG32(NVIC_SYSTICK_CTRL);
    // printf("NVIC_SYSTICK_CTRL = 0x%x\n", g_value);
    g_systick = g_value;
    asm volatile("isb 0xF" ::: "memory");
    REG32(NVIC_SYSTICK_CTRL) = g_value & ~0x03;

    /* clear systick */
    asm volatile("isb 0xF" ::: "memory");
    g_value = REG32(NVIC_ICSR);
    // printf("NVIC_ICSR = 0x%x\n", g_value);
    asm volatile("isb 0xF" ::: "memory");
    REG32(NVIC_ICSR) = (g_value & 0x4000000) >> 1;

    /* set bootflag */
    // LDR R0, =0x429b0001                //TODO
    // LDR R1, =GPRCM_CPUA_BOOT_FLAG
    // ISB
    // STR R0, [R1]

    __asm volatile(
        ".equ NVIC_SCR,                          (0xe000e000 + 0xd10)\n"
        ".equ NVIC_SYSTICK_CTRL,                 (0xe000e000 + 0x010)\n"
        ".equ NVIC_ICSR,                         (0xe000e000 + 0xd04)\n"
        ".equ NVIC_PEND_ACTIVE1,                 (0xe000e000 + 0x200)\n"
        ".equ NVIC_PEND_ACTIVE2,                 (0xe000e000 + 0x204)\n"

        /* ".fnstart\n" */
        /* ".cantunwind\n" */

        /* set resume address in thumb state */
        //  LDR R1, =g_resume_entry     //TODO
        //  LDR R0, [R1]
        //  CMP R0, #0
        //  BNE entry
        //  LDR R0, =__resume
        // entry:
        //  ORR.W R0, R0, #1
        //  LDR R1, =GPRCM_CPUA_BOOT_ADDR
        //  ISB
        //  STR R0, [R1]

        /* go to cpu tz suspend */
        //	"BL cpu_tz_suspend\n"
        "NOP\n"

        // /* switch to HFCLK */
        // LDR R1, =GPRCM_SYSCLK1_CTRLS    //TODO
        // ISB
        // LDR R0, [R1]
        // BIC R0, R0, #0x30000
        // /*ORR R0, R0, #0x10000 *//*switch to LFCLK*/
        // STR R0, [R1]
        "DSB\n"
        "ISB\n"
        "NOP\n"
        "NOP\n"
        "NOP\n"

        /* the WFE instruction will cause two kinds of CPU actions:
         * 1. EVNET_REGISTER = 1, WFE will clear the EVENT_REGISTER and the
         *	 CPU executes the next instruction.
         * 2. EVENT_REGISTER = 0, WFE will make the CPU go to SLEEP state.
         */
        /* first time executing WFE instruction, there are some different
         * situations as follows:
         * 1. if there are interrupts pending and be cleared already,
         *	 the WFE will only clear the CPU EVENT_REGISTER.
         * 2. if there are new interrupts pending after ar400_deepsleep_lock
         *	 operation, the WFE will only clear the CPU EVENT_REGISTER.
         * 3. if the SEV/NMI/DEBUG events coming before now, WFE will only
         *	 clear the CPU EVENT_REGISTER.
         * 4. if there are no SEV/NMI/DEBUG events before and no interrupts
         *	 pending too, WFE wil make the CPU go to the SLEEP state.
         */
        // WFE
        //"WFI\n"

        /* read the NVIC SET_PENDING_REGISTER to check whether there are
         *  any new pending interrupts after ar400_deepsleep_lock operation
         *  which make the first WFE executing failed.
         * 1. If ther are some new pending interrupts, jump to the RESUME_ENTRY
         *	 and abandon the next WFE execution.
         * 2. If there is no new pending interrupts, we execute WFE instruction
         *	 twice to ensure the CPU goes to SLEEP state successfully.
         */
        "LDR R0, =NVIC_PEND_ACTIVE1\n"
        "LDR R1, [R0]\n"
        "LDR R0, =NVIC_PEND_ACTIVE2\n"
        "LDR R2, [R0]\n"
        "ORR R1, R2\n"
        "CMP R1, #0\n"
        // "BNE __resume\n"
        "ISB\n"
        "NOP\n"
        "NOP\n"
        "NOP\n"

        // WFE
        "WFI\n"
        "WFI\n"
        "WFI\n"

        "NOP\n"

        "__resume:\n" // TODO
        //  /* switch cpu clk to pll */
        //  LDR R1, =GPRCM_SYSCLK1_CTRLS
        //  ISB
        //  LDR R0, [R1]
        //  BIC R0, R0, #0x30000
        //  ORR R0, R0, #0x20000
        //  STR R0, [R1]
        //  DSB
        //  ISB

        /* needn't remove address */

        "ISB\n"
        "NOP\n"

        /* ".fnend\n" */

        :
        :
        : "memory");

    /* remove bootflag */
    // LDR R0, =0x429b0000           //TODO
    // LDR R1, =GPRCM_CPUA_BOOT_FLAG
    // ISB
    // STR R0, [R1]

    asm volatile("isb 0xF" ::: "memory");
    /* enable systick */
    REG32(NVIC_SYSTICK_CTRL) = g_systick;
    asm volatile("isb 0xF" ::: "memory");

    /* set normal mode */
    REG32(NVIC_SCR) = 0x0;
    asm volatile("isb 0xF" ::: "memory");

    g_value = g_msp;
    asm volatile("msr MSP, %0" : : "r"(g_value) : "memory");
    asm volatile("isb 0xF" ::: "memory");
    // printf("g_msp = 0x%x\n", g_value);

    g_value = g_psp;
    asm volatile("msr PSP, %0" : : "r"(g_value) : "memory");
    asm volatile("isb 0xF" ::: "memory");
    // printf("g_psp = 0x%x\n", g_value);

    g_value = g_primask;
    asm volatile("msr PRIMASK, %0" : : "r"(g_value) : "memory");
    asm volatile("isb 0xF" ::: "memory");
    // printf("PRIMASK = 0x%x\n", g_value);

    g_value = g_faultmask;
    asm volatile("msr FAULTMASK, %0" : : "r"(g_value) : "memory");
    asm volatile("isb 0xF" ::: "memory");
    // printf("FAULTMASK = 0x%x\n", g_value);

    g_value = g_basepri;
    asm volatile("msr BASEPRI, %0" : : "r"(g_value) : "memory");
    asm volatile("isb 0xF" ::: "memory");
    // printf("BASEPRI = 0x%x\n", g_value);

    g_value = g_control;
    asm volatile("msr CONTROL, %0" : : "r"(g_value) : "memory");
    asm volatile("isb 0xF" ::: "memory");
    // printf("CONTROL = 0x%x\n", g_value);
}
#endif

static void __suspend_enter(enum suspend_state_t state)
{

    PM_LOGD("--> %s line %d\n", __func__, __LINE__);
    /* 写到无复位寄存器表明最终的位置 */
    __record_dbg_status(PM_SUSPEND_ENTER | 5);

    __record_dbg_status(PM_SUSPEND_ENTER | 6);

    /* 使能唤醒原io */

    /* 打印时钟信息 */
    PM_LOGD("device info: rst clk \n"); /* debug info. */

#ifdef CONFIG_PM_DEBUG
    /* PM_SetCPUBootArg((uint32_t)&vault_arm_registers); */
#endif

    if (state == PM_MODE_HIBERNATION) {
        PM_LOGN("PM_MODE_HIBERNATION enter\n"); /* debug info. */
        __record_dbg_status(PM_SUSPEND_ENTER | 7);
        /* 配置休眠类型 */
        /* 清楚挂起唤醒事件 */
        /* clear */
        /* set hold */
        pm_hibernation();                       /* never return */
        PM_LOGN("PM_MODE_HIBERNATION  exit\n"); /* debug info. */
    } else if (state < PM_MODE_STANDBY) {
        PM_LOGN("PM_MODE_SLEEP  enter\n"); /* debug info. */
        __record_dbg_status(PM_SUSPEND_ENTER | 8);
        /* TODO: set system bus to low freq */
        /* 调用汇编代码实现 */
        /* __cpu_sleep(state); */
        cpu_sleep();
        /* 配置标识表明唤醒之前的状态 */
        /* TODO: restore system bus to normal freq */
        PM_LOGN("PM_MODE_SLEEP  exit\n"); /* debug info. */
    } else {
        PM_LOGN("PM_MODE_STANDBY enter\n"); /* debug info. */

        if (axp2101_pmic_ops.pmic_to_sleep != NULL)
            axp2101_pmic_ops.pmic_to_sleep();

        /* 配置PMU相关寄存器 */
        /* 配置唤醒原 */
        /* 配置始终相关 */
        /* 跳转到sram中，配置ddr相关 */

        __record_dbg_status(PM_SUSPEND_ENTER | 9);
        /* 调用汇编代码实现 */
        /* __cpu_suspend(state); */
        cpu_suspend();

        if (axp2101_pmic_ops.pmic_to_resume != NULL)
            axp2101_pmic_ops.pmic_to_resume();
        /* 配置标识表明唤醒之前的状态 */
        PM_LOGN("PM_MODE_STANDBY  exit\n"); /* debug info. */
    }

    __record_dbg_status(PM_SUSPEND_ENTER | 0xa);
    /* 关闭唤醒原 */

    __record_dbg_status(PM_SUSPEND_ENTER | 0xb);
}

static void __suspend_end(enum suspend_state_t state)
{
    PM_LOGD("--> %s line %d\n", __func__, __LINE__);
    /* clear SEVONPEND flag */
    SCB->SCR = 0x0;
}

#ifdef CONFIG_PM_DEBUG
void pm_dump_regs(unsigned int flag)
{
    if (flag & 1 << 0) { /* cpu */
        int i, j;

        PM_LOGD("regs:\n");
        PM_LOGD("msp:0x%08x, psp:0x%08x, psr:0x%08x, primask:0x%08x\n",
                vault_arm_registers.msp, vault_arm_registers.psp,
                vault_arm_registers.psr, vault_arm_registers.primask);
        PM_LOGD("faultmask:0x%08x, basepri:0x%08x, control:0x%08x\n",
                vault_arm_registers.faultmask, vault_arm_registers.basepri,
                vault_arm_registers.control);
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 4; j++) {
                PM_LOGD("reg[%d]:0x%08x ", j + i * 4,
                        vault_arm_registers.reg12[ j + i * 4 ]);
            }
            PM_LOGD("\n");
        }
        PM_LOGD("last step:%x\n", __get_last_record_step());
    }
    if (flag & 1 << 1) { /* nvic */
                         /*nvic_print_regs();*/
    }
    if (flag & 1 << 2) { /* ccmu */
                         /*ccm_print_regs();*/
    }
    if (flag & 1 << 3) { /* gpio */
                         /*gpio_print_regs();*/
    }
}

#endif

/**
 * @brief Initialize the PM-related part of a device object.
 * @note not use printf for this fun is called very earlier.
 * @retval  0 if success or other if failed.
 */
int pm_init(void)
{

    /* 配置打印级别 */

    /* 获取当前的启动方式 */

    /* 关闭唤醒相关 */

    /* pm相关的回调函数 */
    suspend_ops.begin        = __suspend_begin;
    suspend_ops.prepare      = platform_prepare;
    suspend_ops.prepare_late = platform_prepare_late;
    suspend_ops.enter        = __suspend_enter;
    suspend_ops.wake         = platform_wake;
    suspend_ops.finish       = platform_finish;
    suspend_ops.end          = __suspend_end;

    /* 配置PMU相关的寄存器 */

    /* 保存挂起回复的预定时间 */

    return 0;
}

/**
 * @brief Set a magin to synchronize with net.
 */
void pm_set_sync_magic(void)
{
    PM_SetCPUBootArg(PM_SYNC_MAGIC); /* set flag to notify net to run */
}

/**
 * @brief Set system to a lowpower mode.
 * @param state:
 *        @arg state->The lowpower mode will enter.
 * @retval  0 if success or other if failed.
 */
static int _pm_enter_mode(enum suspend_state_t state)
{
    int                  err, record;
    enum suspend_state_t state_use = state;

    int loop;

    if (!(pm_mode_platform_config & (1 << state))) {
        for (loop = (1 << state_use); loop; loop >>= 1) {
            if (pm_mode_platform_config & loop) {
                break;
            }
            state_use--;
        }
    }

    /* 检测是否有唤醒io，有退出 */

    if (state_use >= PM_MODE_MAX) {
        PM_LOGE("%s:%d err mode:%d!\n", __func__, __LINE__, state_use);
        return -1;
    }

    /* 配置唤醒事件 */

    if (state_use < PM_MODE_SLEEP) return 0;

    pm_select_mode(state_use);
    PM_LOGA(PM_SYS " enter mode: %s\n", pm_states[ state_use ]);
    record = __get_last_record_step();
    if (record != PM_RESUME_COMPLETE)
        PM_LOGN("last suspend record:%x\n", record);
#ifdef CONFIG_PM_DEBUG
    parse_dpm_list(PM_OP_NORMAL); /* debug info. */
    parse_dpm_list(PM_OP_NOIRQ);
#endif

    err = suspend_devices_and_enter(state_use);
    /* 添加循环多次判断是否进入成功 */

    pm_set_sync_magic();

    return err;
}

static uint32_t _pm_state = PM_MODE_ON;

enum suspend_state_t pm_get_mode(void) { return _pm_state; }

int pm_enter_mode_timeout(enum suspend_state_t state, uint32_t tmo)
{
    int ret;

    if (state < PM_MODE_SLEEP) return 0;

    if (state >= PM_MODE_MAX) {
        PM_LOGN("%s exit!\n", __func__);
        return -1;
    }
#if 0
	PM_BUG_ON(NULL, PM_IRQ_GET_FLAGS());
#endif
    if (PM_IRQ_GET_FLAGS()) {
        PM_LOGE("%s exit for irq disabled!\n", __func__);
        return -1;
    }

    if (_pm_state != PM_MODE_ON) {
        PM_LOGN("%s exit!\n", __func__);
        return -1;
    }

    _pm_tmo = tmo;
#ifdef CONFIG_PM_WAKELOCKS
#else
    ret = _pm_enter_mode(state);
#endif
    _pm_state = PM_MODE_ON;

    return ret;
}

int pm_enter_mode(enum suspend_state_t state)
{
    return pm_enter_mode_timeout(state, OS_WAIT_FOREVER);
}

void pm_suspend_abort(void)
{
    _pm_tmo = 0;
#ifdef CONFIG_PM_WAKELOCKS
#endif
}

void pm_start(void)
{
#ifdef CONFIG_PM_WAKELOCKS
#endif
}

void pm_stop(void)
{
#ifdef CONFIG_PM_WAKELOCKS
#endif
}

#endif /* CONFIG_PM */
