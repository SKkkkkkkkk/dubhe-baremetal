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

#ifndef __PM_DEFINE_H
#define __PM_DEFINE_H

#include <stdio.h> 

// #define CONFIG_PM_DEBUG
#define CONFIG_PM_DEBUG_LEVEL 5
#define ROM_SYSLOG      printf

#define ROM_DUMP_MASK   (1 << 0)
#define ROM_DBG_MASK    (1 << 1)
#define ROM_INF_MASK    (1 << 2)
#define ROM_WRN_MASK    (1 << 3)
#define ROM_ERR_MASK    (1 << 4)
#define ROM_ANY_MASK    (1 << 5)
#define ROM_ABORT_MASK  (1 << 6)
#define ROM_TOTAL_MASKS (ROM_DUMP_MASK | ROM_DBG_MASK | ROM_INF_MASK | \
                          ROM_WRN_MASK | ROM_ERR_MASK | ROM_ANY_MASK | \
                          ROM_ABORT_MASK)
#define ROM_DEBUG_SHIFT  (6)

#define PM_LOGD_ON (CONFIG_PM_DEBUG_LEVEL > 5)
#define PM_LOGN_ON (CONFIG_PM_DEBUG_LEVEL > 4)
#define PM_LOGW_ON (CONFIG_PM_DEBUG_LEVEL > 3)
#define PM_LOGE_ON (CONFIG_PM_DEBUG_LEVEL > 2)
#define PM_LOGA_ON (CONFIG_PM_DEBUG_LEVEL > 1)
#define PM_ABORT_ON (CONFIG_PM_DEBUG_LEVEL > 0)

#define PM_DEBUG(flags, fmt, arg...)        \
	do {                                    \
		if (flags)                          \
			printf("PMA: "fmt, ##arg);      \
	} while (0)

#define ROM_LOG(level, mask, expand, fmt, arg...)       \
	do {                                            \
		if (level & mask)                       \
		ROM_SYSLOG(expand fmt, ##arg);  \
	} while (0)


#define PM_DUMP(format, args...) ROM_LOG(pm_debug_mask, ROM_DUMP_MASK, "", format, ##args)
#define PM_LOGD(format, args...) PM_DEBUG(PM_LOGD_ON, format, ##args)
#define PM_LOGN(format, args...) PM_DEBUG(PM_LOGN_ON, format, ##args)
#define PM_LOGW(format, args...) PM_DEBUG(PM_LOGW_ON, format, ##args)
#define PM_LOGE(format, args...) PM_DEBUG(PM_LOGE_ON, format, ##args)
#define PM_LOGA(format, args...) PM_DEBUG(PM_LOGA_ON, format, ##args)
#define PM_ABORT(exp) (if (exp && PM_ABORT_ON) while (1);)

#define PM_BUG_ON(d, v)                                                \
	do {                                                               \
		if (v) {                                                       \
			printf("PMA: BUG at %s:%d dev:%s(%p)!\n", __func__,        \
			       __LINE__, (d && ((struct soc_device *)d)->name) ?   \
			       ((struct soc_device *)d)->name : "NULL", d);        \
			while (1)                                                  \
				;                                                      \
		}                                                              \
	} while (0)
#define PM_WARN_ON(d, v)                                               \
	do {                                                               \
		if (v) {                                                       \
			printf("PMA: WARN at %s:%d dev:%s(%p)!\n", __func__,       \
			       __LINE__, (d && ((struct soc_device *)d)->name) ?   \
			       ((struct soc_device *)d)->name : "NULL", d);        \
		}                                                              \
	} while (0)

#endif /* __PM_DEFINE_H */
