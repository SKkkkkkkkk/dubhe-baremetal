#ifndef __MAIN_H__
#define __MAIN_H__


#define CONFIG_MAIN_DEBUG_LEVEL 6
#define ROM_SYSLOG            printf

#define MAIN_LOGD_ON      (CONFIG_MAIN_DEBUG_LEVEL > 5)
#define MAIN_LOGN_ON      (CONFIG_MAIN_DEBUG_LEVEL > 4)
#define MAIN_LOGW_ON      (CONFIG_MAIN_DEBUG_LEVEL > 3)
#define MAIN_LOGE_ON      (CONFIG_MAIN_DEBUG_LEVEL > 2)
#define MAIN_LOGA_ON      (CONFIG_MAIN_DEBUG_LEVEL > 1)
#define MAIN_ABORT_ON     (CONFIG_MAIN_DEBUG_LEVEL > 0)

#define MAIN_DEBUG(flags, fmt, arg...)           \
    do {                                       \
        if (flags) printf("MAIN: " fmt, ##arg); \
    } while (0)

#define ROM_LOG(level, mask, expand, fmt, arg...)        \
    do {                                                 \
        if (level & mask) ROM_SYSLOG(expand fmt, ##arg); \
    } while (0)

#define MAIN_DUMP(format, args...) \
    ROM_LOG(pm_debug_mask, ROM_DUMP_MASK, "", format, ##args)
#define MAIN_LOGD(format, args...) MAIN_DEBUG(MAIN_LOGD_ON, format, ##args)
#define MAIN_LOGN(format, args...) MAIN_DEBUG(MAIN_LOGN_ON, format, ##args)
#define MAIN_LOGW(format, args...) MAIN_DEBUG(MAIN_LOGW_ON, format, ##args)
#define MAIN_LOGE(format, args...) MAIN_DEBUG(MAIN_LOGE_ON, format, ##args)
#define MAIN_LOGA(format, args...) MAIN_DEBUG(MAIN_LOGA_ON, format, ##args)
#define MAIN_ABORT(exp)            (if (exp && MAIN_ABORT_ON) while (1);)

#define MAIN_BUG_ON(d, v)                                                   \
    do {                                                                  \
        if (v) {                                                          \
            printf("MAIN: BUG at %s:%d dev:%s(%p)!\n", __func__, __LINE__, \
                   (d && ((struct soc_device *) d)->name)                 \
                       ? ((struct soc_device *) d)->name                  \
                       : "NULL",                                          \
                   d);                                                    \
            while (1)                                                     \
                ;                                                         \
        }                                                                 \
    } while (0)
#define MAIN_WARN_ON(d, v)                                                   \
    do {                                                                   \
        if (v) {                                                           \
            printf("MAIN: WARN at %s:%d dev:%s(%p)!\n", __func__, __LINE__, \
                   (d && ((struct soc_device *) d)->name)                  \
                       ? ((struct soc_device *) d)->name                   \
                       : "NULL",                                           \
                   d);                                                     \
        }                                                                  \
    } while (0)

#endif //__MAIN_H__
