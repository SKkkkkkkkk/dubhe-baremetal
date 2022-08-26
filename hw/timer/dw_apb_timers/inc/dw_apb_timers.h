#ifndef __APE1210_TIMER_H__
#define __APE1210_TIMER_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
    extern "C" {
#endif

#include "dw_apb_timers_regs.h"
#define TIMERX2_BASE (0x21070000UL) /*!< (Timerx2   ) Base Address */
#define TIMERX6_BASE (0x24040000UL) /*!< (Timerx6   ) Base Address */
#define TIMERX2 ((APE1210_Timerx2_TypeDef *)TIMERX2_BASE)
#define TIMERX6 ((APE1210_Timerx6_TypeDef *)TIMERX6_BASE)


typedef enum _timer_id {
	Timerx2_T1 = 1,
	Timerx2_T2 = 2,
	Timerx6_T1 = 3,
	Timerx6_T2 = 4,
	Timerx6_T3 = 5,
	Timerx6_T4 = 6,
	Timerx6_T5 = 7,
	Timerx6_T6 = 8,
} timer_id_t;

typedef enum _timer_mode {
	Mode_Free_Running = 0,
	Mode_User_Defined = 1,
} timer_mode_t;

typedef struct _timer_init_config {
	bool int_mask;
	timer_id_t timer_id;
	timer_mode_t timer_mode;
	uint32_t loadcount;
} timer_init_config_t;

/**
 * @brief Enable Timer
 * @param timer_id 从enum _timer_id中选择一个Timer
 * @return true - the timer is enabled, false - 没有此timer
 */
bool timer_enable(timer_id_t timer_id);

/**
 * @brief Disable Timer
 * @param timer_id 从enum _timer_id中选择一个Timer
 * @return true - the timer is disabled, false - 没有此timer
 */
bool timer_disable(timer_id_t timer_id);

/**
 * @brief 设置Timer的Mode,Mode_Free_Running or Mode_User_Defined
 * @param timer_id 从enum _timer_id中选择一个Timer
 * @param timer_mode Mode_Free_Running or Mode_User_Defined
 * @return true - the timer is disabled, false - 没有此timer
 */
bool timer_set_mode(timer_id_t timer_id, timer_mode_t timer_mode);

/**
 * @brief mask or unmask timer interrupt
 * @param timer_id 从enum _timer_id中选择一个Timer
 * @param mask true - mask int, false - unmask int
 * @return true - the timer is disabled, false - 没有此timer
 */
bool timer_set_int_mask(timer_id_t timer_id, bool mask);

/**
 * @brief 设置Timer的装载值
 * @param timer_id 从enum _timer_id中选择一个Timer
 * @param loadcount 定时器装载值
 * @return true - the timer is disabled, false - 没有此timer
 */
bool timer_set_loadcount(timer_id_t timer_id, uint32_t loadcount);

/**
 * @brief Init Timer,初始化完成,但是没有Enable
 * @param timer_init_config_t 定时器初始化结构体变量的地址
 * @return true - the timer is disabled, false - 没有此timer
 */
bool timer_init(timer_init_config_t const *const timer_init_config);

#ifdef __cplusplus
    }
#endif

#endif