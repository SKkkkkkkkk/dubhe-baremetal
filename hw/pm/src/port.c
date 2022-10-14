/*
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "sys/list.h"
#include "pm.h"
#include "pm_i.h"
#include "port.h"
#include "_pm_define.h"

#ifdef CONFIG_PM

int platform_prepare(enum suspend_state_t state)
{
    PM_LOGD("--> %s line %d\n", __func__, __LINE__);

    return 0;
}

int platform_prepare_late(enum suspend_state_t state)
{
    PM_LOGD("--> %s line %d\n", __func__, __LINE__);

    return 0;
}

void platform_wake(enum suspend_state_t state)
{
    PM_LOGD("--> %s line %d\n", __func__, __LINE__);
}

void platform_finish(enum suspend_state_t state)
{
    PM_LOGD("--> %s line %d\n", __func__, __LINE__);
}

void cpu_suspend_cb(void) { PM_LOGD("--> %s line %d\n", __func__, __LINE__); }

void cpu_tz_hibernation(void)
{
    PM_LOGD("--> %s line %d\n", __func__, __LINE__);
}

__ramfunc void cpu_tz_suspend(void) {}

#endif
