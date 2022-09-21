#include "systimer.h"
typedef struct _systimer
{
    volatile bool     running;
    volatile uint32_t level2_cnt;
    volatile cnt_type level1_cnt;
} systimer_t;


static systimer_t systimer_arry[SYSTIMER_USE_NUMS] ={0};


/*开启一个空闲定时器*/
systimer_id_t systimer_acquire_timer(void)
{
    systimer_id_t i;
    for(i=0;i<SYSTIMER_USE_NUMS;i++)
    {
        uint32_t irq_mask = portSYSTIMER_ENTER_CRITICAL();
        (void)irq_mask;
        if(!systimer_arry[i].running)
        {
            systimer_arry[i].level1_cnt = SYSTIMER_CNT;
            systimer_arry[i].level2_cnt = 0;
            systimer_arry[i].running = true;
            portSYSTIMER_EXIT_CRITICAL(irq_mask);
            return i;
        }
        portSYSTIMER_EXIT_CRITICAL(irq_mask);
    }
    return SYSTIMER_ERR_ID; //没有空闲定时器
}

/*释放一个定时器*/
void systimer_release_timer(systimer_id_t timer_id)
{
    if( timer_id==SYSTIMER_ERR_ID || !systimer_arry[timer_id].running) return;
    uint32_t irq_mask = portSYSTIMER_ENTER_CRITICAL();
    (void)irq_mask;
        //systimer_arry[timer_id].level1_cnt = 0;
        //systimer_arry[timer_id].level2_cnt = 0;
        systimer_arry[timer_id].running = false;
    portSYSTIMER_EXIT_CRITICAL(irq_mask);
    return;
}

/*时间差*/
uint64_t systimer_get_elapsed_time(uint8_t timer_id, time_precision_t time_precision)
{
    if(!systimer_arry[timer_id].running) return 0;
    uint32_t irq_mask = portSYSTIMER_ENTER_CRITICAL();
    (void)irq_mask;
        uint32_t level2_cnt = systimer_arry[timer_id].level2_cnt;
        cnt_type curr_level1_cnt = (cnt_type)SYSTIMER_CNT;
    portSYSTIMER_EXIT_CRITICAL(irq_mask);
    uint32_t start_level1_cnt = systimer_arry[timer_id].level1_cnt;

    uint64_t diffcnt = 0;
    if(level2_cnt == 0)
        #if(SYSTIMER_UP == 1)
            diffcnt = (uint64_t)(cnt_type)(curr_level1_cnt - start_level1_cnt);
        #else
            diffcnt = (uint64_t)(cnt_type)(start_level1_cnt - curr_level1_cnt);
        #endif
    else
    {
        #if(SYSTIMER_UP == 1)
            if(curr_level1_cnt<start_level1_cnt)
                --level2_cnt;
            diffcnt = (uint64_t)(cnt_type)(curr_level1_cnt - start_level1_cnt);
        #else
            if(curr_level1_cnt>start_level1_cnt)
                --level2_cnt;
            diffcnt = (uint64_t)(cnt_type)(start_level1_cnt - curr_level1_cnt);
        #endif
            diffcnt += level2_cnt*SYSTIMER_CNT_MAX;
    }

    if(time_precision == IN_CNT)
        return diffcnt;
    if(time_precision == IN_US)
        return COUNT_TO_USEC(diffcnt, SYSTIMER_CLOCK);
    if(time_precision == IN_MS)
        return COUNT_TO_MSEC(diffcnt, SYSTIMER_CLOCK);
    // if(time_precision == IN_S)
        return COUNT_TO_SEC(diffcnt, SYSTIMER_CLOCK);
}

/*判断是否超时*/
bool systimer_is_timeout(systimer_id_t timer_id, uint64_t cnt, time_precision_t time_precision)
{
    if(systimer_arry[timer_id].running == false)
        return true;
    if(systimer_get_elapsed_time(timer_id, time_precision) >= cnt)
        return true;
    return false;
}

/*延时函数*/
bool systimer_delay(uint64_t time, time_precision_t time_precision)
{
    systimer_id_t timer_id = systimer_acquire_timer();
    if(timer_id == SYSTIMER_ERR_ID)    return false;   //没有空闲timer
    while(!systimer_is_timeout(timer_id, time, time_precision));
    systimer_release_timer(timer_id);
    return true;
}

/*中断服务函数*/
void SysTimer_IRQHandler(void)
{
    SYSTIMER_CLEAR_INT_FLAG();
    
    for(systimer_id_t i = 0;i<SYSTIMER_USE_NUMS;++i)
        if(systimer_arry[i].running)
            ++systimer_arry[i].level2_cnt;
}