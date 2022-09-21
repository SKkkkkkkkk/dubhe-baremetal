#ifndef __SYSTIMER_PORT_H__
#define __SYSTIMER_PORT_H__

#if defined(A55)
    #include "gic.h"
    #include "irq_ctrl.h"
#elif defined(M3)
    #include "m3.h"
#else
    #error "unsupport core."
#endif

#include "dw_apb_timers.h"


#define SYSTIMER_USE_NUMS   5   //最多可同时使用的定时器数目
#define SYSTIMER_CNT_BITS   32  //CNT占的bit数
#define SYSTIMER_UP         0   //向下计数


#define SYSTIMER_CLOCK      20000000
#if defined(A55)
    #define SYSTIMER_CNT        (TIMERX2->Timer1CurrentValue)
    #define SYSTIMER_CLEAR_INT_FLAG() ((void)TIMERX2->Timer1EOI)
    #define systimer_stop()     timer_disable(Timerx2_T1)
#elif defined(M3)
    #define SYSTIMER_CNT        (TIMERX2->Timer2CurrentValue)
    #define SYSTIMER_CLEAR_INT_FLAG() ((void)TIMERX2->Timer2EOI)
    #define systimer_stop()     timer_disable(Timerx2_T2)
    #define SysTimer_IRQHandler Interrupt6_Handler
#else
    #error "unsupport core."
#endif




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
                                                // .timer_id = Timerx6_T1,
                                                .timer_mode = Mode_User_Defined
                                            };
#if defined(A55)
    timer_init_config.timer_id = Timerx2_T1;
    timer_init(&timer_init_config);
    void SysTimer_IRQHandler(void);
    IRQ_SetHandler(Timerx2_T1_IRQn, SysTimer_IRQHandler);
    IRQ_SetPriority(Timerx2_T1_IRQn, 0<<3);
    IRQ_Enable(Timerx2_T1_IRQn);
    timer_enable(Timerx2_T1);
#elif defined(M3)
    timer_init_config.timer_id = Timerx2_T2;
    timer_init(&timer_init_config);
    void SysTimer_IRQHandler(void);
    NVIC_SetPriority(Interrupt6_IRQn, 0);
	NVIC_SetVector(Interrupt6_IRQn, (uint32_t)(uintptr_t)SysTimer_IRQHandler);
	NVIC_EnableIRQ(Interrupt6_IRQn);
    timer_enable(Timerx2_T2);
#else
    #error "unsupport core."
#endif
   
}





#endif
