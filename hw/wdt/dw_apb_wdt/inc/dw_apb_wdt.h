#ifndef __DW_APB_WDT_H__
#define __DW_APB_WDT_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

typedef enum { 
    WDT0_ID = 0,
    #ifdef A55
    WDT1_ID = 1
    #endif
} wdt_id_t;

/**
 * @brief 看门狗配置并启动.
 * @param id 选择wdt, WDT0_ID or WDT1_ID.
 * @param rpl Reset pulse length. 祥见wdt_databook, Control Register.
 * @param rmod Response mode. 祥见wdt_databook, Control Register.
 * @param timeout  Timeout period for initialization. 祥见wdt_databook, Timeout Range Register.
 * @return void
 */
void wdt_setup(wdt_id_t id, uint8_t rpl, uint8_t rmod, uint8_t timeout);

/**
 * @brief 复位看门狗.
 * @param id 选择wdt, WDT0_ID or WDT1_ID.
 * @return void
 */
void wdt_reset(wdt_id_t id);

/**
 * @brief 喂狗.
 * @param id 选择wdt, WDT0_ID or WDT1_ID.
 * @return void
 */
void wdt_feed(wdt_id_t id);

/**
 * @brief 清楚看门狗中断.
 * @param id 选择wdt, WDT0_ID or WDT1_ID.
 * @return void
 */
void wdt_clear_irq(wdt_id_t id);


#ifdef __cplusplus
    }
#endif

#endif