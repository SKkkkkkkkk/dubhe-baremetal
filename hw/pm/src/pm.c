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
#include <string.h>
#include <stdlib.h>

#include "m3.h"
#include "core_cm3.h"

#include "sys/list.h"
#include "pm.h"
#include "_pm_define.h"
#include "pm_i.h"
#include "port.h"

#ifdef CONFIG_PM

#ifdef CONFIG_PM_DEBUG
static struct arm_CMX_core_regs vault_arm_registers;
#endif

#define PM_TIMEOFDAY_SAVE() //timeofday_save()

#define OS_WAIT_FOREVER         0xffffffffU /* Wait forever timeout value */

static uint32_t _pm_tmo = OS_WAIT_FOREVER;

static int __suspend_begin(enum suspend_state_t state)
{
	/* set SEVONPEND flag */
	SCB->SCR = 0x10;

	return 0;
}

/* hibernation whole system, save user data to flash before call this func.
 * BE SUURE: all hardware has been closed and it's prcm config setted to default value.
 */
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

static void __suspend_enter(enum suspend_state_t state)
{
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
		PM_LOGD("PM_MODE_HIBERNATION \n"); /* debug info. */
		__record_dbg_status(PM_SUSPEND_ENTER | 7);
		/* 配置休眠类型 */
		/* 清楚挂起唤醒事件 */
		/* clear */
		/* set hold */
		pm_hibernation(); /* never return */
	} else if (state < PM_MODE_STANDBY) {
		PM_LOGD("PM_MODE_SLEEP \n"); /* debug info. */
		__record_dbg_status(PM_SUSPEND_ENTER | 8);
		/* TODO: set system bus to low freq */
		/* 调用汇编代码实现 */
		__cpu_sleep(state);
		/* 配置标识表明唤醒之前的状态 */
		/* TODO: restore system bus to normal freq */
	} else {
		PM_LOGD("PM_MODE_STANDBY \n"); /* debug info. */
		/* 配置PMU相关寄存器 */
		/* 配置唤醒原 */
		/* 配置始终相关 */
		/* 跳转到sram中，配置ddr相关 */

		__record_dbg_status(PM_SUSPEND_ENTER | 9);
		/* 调用汇编代码实现 */
		__cpu_suspend(state);

		/* 配置标识表明唤醒之前的状态 */
	}

	__record_dbg_status(PM_SUSPEND_ENTER | 0xa);
	/* 关闭唤醒原 */

	__record_dbg_status(PM_SUSPEND_ENTER | 0xb);
}

static void __suspend_end(enum suspend_state_t state)
{
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
				        vault_arm_registers.reg12[j + i * 4]);
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

#else
#define suspend_test_start()
#define suspend_test_finish(x...)
#define suspend_test(l)         0
void pm_set_test_level(enum suspend_test_level_t level) { ; }
#define pm_dump_regs(flag)
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
	suspend_ops.begin = __suspend_begin;
	suspend_ops.prepare = platform_prepare;
	suspend_ops.prepare_late = platform_prepare_late;
	suspend_ops.enter = __suspend_enter;
	suspend_ops.wake = platform_wake;
	suspend_ops.finish = platform_finish;
	suspend_ops.end = __suspend_end;

	/* 配置PMU相关的寄存器 */

	/* 保存挂起回复的预定时间 */

	return 0;
}

#ifdef CONFIG_ARCH_APP_CORE
/**
 * @brief Set a magin to synchronize with net.
 */
void pm_set_sync_magic(void)
{
	PM_SetCPUBootArg(PM_SYNC_MAGIC); /* set flag to notify net to run */
}
#endif

/**
 * @brief Set system to a lowpower mode.
 * @param state:
 *        @arg state->The lowpower mode will enter.
 * @retval  0 if success or other if failed.
 */
static int _pm_enter_mode(enum suspend_state_t state)
{
	int err, record;
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

	if (state_use < PM_MODE_SLEEP)
		return 0;

	pm_select_mode(state_use);
	PM_LOGA(PM_SYS" enter mode: %s\n", pm_states[state_use]);
	record = __get_last_record_step();
	if (record != PM_RESUME_COMPLETE)
		PM_LOGN("last suspend record:%x\n", record);
#ifdef CONFIG_PM_DEBUG
	parse_dpm_list(PM_OP_NORMAL);  /* debug info. */
	parse_dpm_list(PM_OP_NOIRQ);
#endif

	err = suspend_devices_and_enter(state_use);
	/* 添加循环多次判断是否进入成功 */

	pm_set_sync_magic();

	return err;
}

static uint32_t _pm_state = PM_MODE_ON;

enum suspend_state_t pm_get_mode(void)
{
	return _pm_state;
}

int pm_enter_mode_timeout(enum suspend_state_t state, uint32_t tmo)
{
	int ret;

	if (state < PM_MODE_SLEEP)
		return 0;

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
