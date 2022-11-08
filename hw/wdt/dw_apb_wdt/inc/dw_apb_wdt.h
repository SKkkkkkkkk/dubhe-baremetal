#ifndef __DW_APB_WDT_H__
#define __DW_APB_WDT_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

/*
void wdt_start(uint8_t time)的time代表的时间如下：
0--ffff
1--1_FFFF
2--3_FFFF
...
f--7FFF_FFFF
*/

/**
 * @brief 开启系统看门狗
 * @param time 定时时间
 */
void wdt_start(uint8_t time);

/**
 * @brief 喂狗
 */
void wdt_feed(void);

#ifdef __cplusplus
    }
#endif

#endif