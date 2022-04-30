#ifndef __SYSTIMER_PORT_H__
#define __SYSTIMER_PORT_H__

#include "gic.h"
#include "irq_ctrl.h"
#include "dw_apb_timers.h"


#define SYSTIMER_USE_NUMS   5   //最多可同时使用的定时器数目
#define SYSTIMER_CNT_BITS   32  //CNT占的bit数
#define SYSTIMER_UP         0   //向下计数
#define SYSTIMER_CNT        (TIMERX2->Timer1CurrentValue)
#define SYSTIMER_CLOCK      20000000
#define SYSTIMER_CLEAR_INT_FLAG() ((void)TIMERX2->Timer1EOI)

// #ifdef NO_SYS
    #define portSYSTIMER_ENTER_CRITICAL()           ((uint32_t)0)
    #define portSYSTIMER_EXIT_CRITICAL(irq_mask)
// #else
//     #include "FreeRTOS.h"
//     #include "task.h"
//     #define portSYSTIMER_ENTER_CRITICAL()           (taskENTER_CRITICAL(),(uint32_t)0)
//     #define portSYSTIMER_EXIT_CRITICAL(irq_mask)    taskEXIT_CRITICAL()
// #endif

// #define portSYSTIMER_ENTER_CRITICAL()
// #define portSYSTIMER_EXIT_CRITICAL()


static inline void systimer_init(void)
{
    timer_init_config_t timer_init_config = {
                                                .int_mask = 0,
                                                .loadcount = 0xFFFFFFFFUL,
                                                .timer_id = Timerx2_T1,
                                                .timer_mode = Mode_User_Defined
                                            };
    timer_init(&timer_init_config);
    void SysTimer_IRQHandler(void);
    IRQ_SetHandler(Timer1_1_IRQn, SysTimer_IRQHandler);
    IRQ_SetPriority(Timer1_1_IRQn, 0<<3);
    IRQ_Enable(Timer1_1_IRQn);
    timer_enable(Timerx2_T1);
}

#define systimer_stop()     timer_disable(Timerx2_T1)



#endif
