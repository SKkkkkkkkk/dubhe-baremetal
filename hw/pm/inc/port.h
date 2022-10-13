/*
 */

#ifndef __PM_PORT_H
#define __PM_PORT_H

#include "cmsis_gcc.h"
#include "interrupt.h"

#define arch_suspend_disable_irqs __disable_irq
#define arch_suspend_enable_irqs  __enable_irq

#define nop()                     __NOP()
#define isb()                     __ISB()
#define dsb()                     __DSB()
#define dmb()                     __DMB()
#define wfi()                     __WFI()
#define wfe()                     __WFE()

#define PM_SYS                    "appos"
#define PM_SetCPUBootFlag(f)      // HAL_PRCM_SetCPUABootFlag(f)

/* Internal reg, should not used outside.
 * used record state during hibernation and poweroff,
 * used reocrd resume arg when sleep and standby.
 */
#define PM_SetCPUBootArg(a)       // HAL_PRCM_SetCPUABootArg(a)
#define PM_SystemDeinit()         // SystemDeInit(SYSTEM_DEINIT_FLAG_RESET_CLK)
#define pm_udelay(us)             // HAL_UDelay(us)
#define PM_REBOOT() \
    while (1) {     \
    } // HAL_WDG_ResetCpu(WDG_RESET_CPU_PORESET)
#define PM_IRQ_SAVE               arch_irq_save
#define PM_IRQ_RESTORE            arch_irq_restore
#define PM_IRQ_GET_FLAGS          arch_irq_get_flags

#define __set_last_record_step(s)   // HAL_PRCM_SetCPUAPrivateData(0, s)
#define __get_last_record_step()  0 // HAL_PRCM_GetCPUAPrivateData(0)  //TODO

extern struct platform_suspend_ops suspend_ops;

static __always_inline void __record_dbg_status(int val)
{
    // __set_last_record_step(val);
    dsb();
    isb();
}

extern unsigned int nvic_int_mask[];

extern int  platform_prepare(enum suspend_state_t state);
extern int  platform_prepare_late(enum suspend_state_t state);
extern void platform_wake(enum suspend_state_t state);
extern void platform_finish(enum suspend_state_t state);
extern void cpu_suspend_cb(void);

void     pm_wakelocks_init(void);
void     pm_wakelocks_deinit(void);
uint32_t pm_wakelocks_wait(uint32_t timeout);
uint32_t pm_wakelocks_is_active(void);
void     pm_wakelocks_touch(void);
uint32_t pm_wakelocks_is_touched(void);

#endif /* __PM_PORT_H */
