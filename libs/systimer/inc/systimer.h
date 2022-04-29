#ifndef __SYSTIMER_H__
#define __SYSTIMER_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "systimer_port.h"
#include <limits.h>

#define USEC_TO_COUNT(us, clockFreqInHz) (uint64_t)(((uint64_t)(us) * (clockFreqInHz)) / 1000000U)
#define COUNT_TO_USEC(count, clockFreqInHz) (uint64_t)((uint64_t)(count) * 1000000U / (clockFreqInHz))
#define MSEC_TO_COUNT(ms, clockFreqInHz) (uint64_t)((uint64_t)(ms) * (clockFreqInHz) / 1000U)
#define COUNT_TO_MSEC(count, clockFreqInHz) (uint64_t)((uint64_t)(count) * 1000U / (clockFreqInHz))
#define SEC_TO_COUNT(ms, clockFreqInHz) (uint64_t)((uint64_t)(s) * (clockFreqInHz) / 1U)
#define COUNT_TO_SEC(count, clockFreqInHz) (uint64_t)((uint64_t)(count) * 1U / (clockFreqInHz))


/* systimer个数 */
#if SYSTIMER_USE_NUMS >= UINT32_MAX
    #error SYSTIMER_NUMS must less than UINT32_MAX!
#else // SYSTIMER_USE_NUMS < UINT32_MAX
    #if SYSTIMER_USE_NUMS < UINT8_MAX  // SYSTIMER_USE_NUMS < UINT8_MAX
        typedef uint8_t systimer_id_t;
        #define SYSTIMER_ERR_ID UINT8_MAX
    #else  // UINT8_MAX <= SYSTIMER_USE_NUMS < UINT16_MAX
        #if SYSTIMER_USE_NUMS < UINT16_MAX
            typedef uint16_t systimer_id_t;
            #define SYSTIMER_ERR_ID UINT16_MAX
        #else // UINT16_MAX <= SYSTIMER_USE_NUMS < UINT32_MAX
            typedef uint32_t systimer_id_t;
            #define SYSTIMER_ERR_ID UINT32_MAX
        #endif
    #endif
#endif

/* timer计数器的bit数 */
#ifndef SYSTIMER_CNT_BITS
    #error please define SYSTIMER_CNT_BITS!
#else
    #if SYSTIMER_CNT_BITS == 8
        typedef uint8_t cnt_type;
        #define SYSTIMER_CNT_MAX    0xFFUL	//加不加1,取决于是溢出产生中断，还是匹配产生中断
    #endif

    #if SYSTIMER_CNT_BITS == 16
        typedef uint16_t cnt_type;
        #define SYSTIMER_CNT_MAX    0xFFFFUL	//加不加1,取决于是溢出产生中断，还是匹配产生中断
    #endif

    #if SYSTIMER_CNT_BITS == 32
        typedef uint32_t cnt_type;
        #define SYSTIMER_CNT_MAX    0xFFFFFFFFUL	//加不加1,取决于是溢出产生中断，还是匹配产生中断
    #endif
#endif


/* 向上 or 向下计数 */
#ifndef SYSTIMER_UP
    #error please indicate whether to count up or down!
#else
    #if SYSTIMER_UP == 0
        #define SYSTIMER_CNT_INIT   SYSTIMER_CNT_MAX
    #else
        #define SYSTIMER_CNT_INIT   0U
    #endif
#endif


typedef enum _time_precision
{
    IN_CNT = 0,
    IN_US  = 1,
    IN_MS  = 2,
    IN_S   = 3
} time_precision_t;

/**
 * @brief 开启一个空闲时钟
 * @return 255(max(systimer_id_t)) - 没有空闲时钟,返回其它值代表开启的空闲时钟的id 
 */
systimer_id_t systimer_acquire_timer(void);

/**
 * @brief 关闭一个时钟
 * @param timer_id 待关闭的时钟id
 */
void systimer_release_timer(systimer_id_t timer_id);

/**
 * @brief 获得距离上次开启该时钟后过去的时间
 * @param timer_id 时钟id
 * @param time_precision 时间精度
 * @return 距离上次开启后过去的时间
 */
uint64_t systimer_get_elapsed_time(systimer_id_t timer_id, time_precision_t time_precision);

/**
 * @brief 判断一个时钟是否超时
 * @param timer_id 时钟
 * @param time 设置一个超时时间
 * @param time_precision 设置的超时时间精度
 * @return true - 超时,false - 未超时 
 */
bool systimer_is_timeout(systimer_id_t timer_id, uint64_t time, time_precision_t time_precision);

/**
 * @brief 阻塞延时指定时长后返回
 * @param time 设置一个delay时长
 * @param time_precision 设置时间精度
 * @return true - 延时完成,false - 延时失败，无空闲可用时钟
 */
bool systimer_delay(uint64_t time, time_precision_t time_precision);

#ifdef __cplusplus
    }
#endif

#endif
