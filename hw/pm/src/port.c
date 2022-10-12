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
