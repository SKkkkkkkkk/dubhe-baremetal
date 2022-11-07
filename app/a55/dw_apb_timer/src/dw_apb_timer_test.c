#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "arch_features.h"
#include "dw_apb_timers.h"
#include "gic.h"
#include "irq_ctrl.h"
#include "systimer.h"


// #define SIMPALE_TEST
#define TIMER_FREQ (20000000/100)


static  volatile bool flag = false;
static volatile uint32_t timerx2_t1_i = 0U;
static volatile uint32_t timerx2_t2_i = 0U;
static volatile uint32_t timerx6_t1_i = 0;
static volatile uint32_t timerx6_t2_i = 0;
static volatile uint32_t timerx6_t3_i = 0;
static volatile uint32_t timerx6_t4_i = 0;
static volatile uint32_t timerx6_t5_i = 0;
static volatile uint32_t timerx6_t6_i = 0;
static volatile uint32_t timerx2_t1_i_copy = 0U;
static volatile uint32_t timerx2_t2_i_copy = 0U;
static volatile uint32_t timerx6_t1_i_copy = 0;
static volatile uint32_t timerx6_t2_i_copy = 0;
static volatile uint32_t timerx6_t3_i_copy = 0;
static volatile uint32_t timerx6_t4_i_copy = 0;
static volatile uint32_t timerx6_t5_i_copy = 0;
static volatile uint32_t timerx6_t6_i_copy = 0;
void dw_apb_timer_test(bool sample)
{
	// GIC_Enable();
#ifdef SIMPALE_TEST
	printf("dw_apb_timer_test 1.\n\r");
	uint32_t tick = 0;
	systimer_init();
	while(1)
	{
		printf("%d\n", (int)(tick++));
		systimer_delay(1,IN_S);
	};
#else
	printf("dw_apb_timer_test 2.\n\r");
	timer_init_config_t timer_init_config = {
		.int_mask = 0, .loadcount = TIMER_FREQ, .timer_id = Timerx2_T1, .timer_mode = Mode_User_Defined
	};
	timer_init(&timer_init_config);

	timer_init_config.timer_id = Timerx2_T2; //ok  //ok
	timer_init_config.loadcount = 2 * TIMER_FREQ;
	timer_init(&timer_init_config);

	timer_init_config.timer_id = Timerx6_T1; //ok  //ok
	timer_init_config.loadcount = (3) * TIMER_FREQ;
	timer_init(&timer_init_config);

	timer_init_config.timer_id = Timerx6_T2; //err  //err
	timer_init_config.loadcount = (4) * TIMER_FREQ;
	timer_init(&timer_init_config);

	timer_init_config.timer_id = Timerx6_T3; //ok  //err
	timer_init_config.loadcount = (5) * TIMER_FREQ;
	timer_init(&timer_init_config);

	timer_init_config.timer_id = Timerx6_T4; //err //err
	timer_init_config.loadcount = (6) * TIMER_FREQ;
	timer_init(&timer_init_config);

	timer_init_config.timer_id = Timerx6_T5; //err //err
	timer_init_config.loadcount = (7) * TIMER_FREQ;
	timer_init(&timer_init_config);

	timer_init_config.timer_id = Timerx6_T6; //ok //err
	timer_init_config.loadcount = (8) * TIMER_FREQ;
	timer_init(&timer_init_config);

	void timerx2_t1_irqhandler(void);
	GIC_SetTarget(Timerx2_T1_IRQn, 1 << 0);
	IRQ_SetHandler(Timerx2_T1_IRQn, timerx2_t1_irqhandler);
	IRQ_SetPriority(Timerx2_T1_IRQn, 0 << 3);
	IRQ_Enable(Timerx2_T1_IRQn);

	void timerx2_t2_irqhandler(void);
	GIC_SetTarget(Timerx2_T2_IRQn, 1 << 0);
	IRQ_SetHandler(Timerx2_T2_IRQn, timerx2_t2_irqhandler);
	IRQ_SetPriority(Timerx2_T2_IRQn, 0 << 3);
	IRQ_Enable(Timerx2_T2_IRQn);

	void timerx6_t1_irqhandler(void);
	GIC_SetTarget(Timerx6_T1_IRQn, 1 << 0);
	IRQ_SetHandler(Timerx6_T1_IRQn, timerx6_t1_irqhandler);
	IRQ_SetPriority(Timerx6_T1_IRQn, 0 << 3);
	IRQ_Enable(Timerx6_T1_IRQn);

	void timerx6_t2_irqhandler(void);
	GIC_SetTarget(Timerx6_T2_IRQn, 1 << 0);
	IRQ_SetHandler(Timerx6_T2_IRQn, timerx6_t2_irqhandler);
	IRQ_SetPriority(Timerx6_T2_IRQn, 0 << 3);
	IRQ_Enable(Timerx6_T2_IRQn);

	void timerx6_t3_irqhandler(void);
	GIC_SetTarget(Timerx6_T3_IRQn, 1 << 0);
	IRQ_SetHandler(Timerx6_T3_IRQn, timerx6_t3_irqhandler);
	IRQ_SetPriority(Timerx6_T3_IRQn, 0 << 3);
	IRQ_Enable(Timerx6_T3_IRQn);

	void timerx6_t4_irqhandler(void);
	GIC_SetTarget(Timerx6_T4_IRQn, 1 << 0);
	IRQ_SetHandler(Timerx6_T4_IRQn, timerx6_t4_irqhandler);
	IRQ_SetPriority(Timerx6_T4_IRQn, 0 << 3);
	IRQ_Enable(Timerx6_T4_IRQn);

	void timerx6_t5_irqhandler(void);
	GIC_SetTarget(Timerx6_T5_IRQn, 1 << 0);
	IRQ_SetHandler(Timerx6_T5_IRQn, timerx6_t5_irqhandler);
	IRQ_SetPriority(Timerx6_T5_IRQn, 0 << 3);
	IRQ_Enable(Timerx6_T5_IRQn);

	void timerx6_t6_irqhandler(void);
	GIC_SetTarget(Timerx6_T6_IRQn, 1 << 0);
	IRQ_SetHandler(Timerx6_T6_IRQn, timerx6_t6_irqhandler);
	IRQ_SetPriority(Timerx6_T6_IRQn, 0 << 3);
	IRQ_Enable(Timerx6_T6_IRQn);

	timer_enable(Timerx6_T6);
	timer_enable(Timerx6_T5);
	timer_enable(Timerx6_T4);
	timer_enable(Timerx6_T3);
	timer_enable(Timerx6_T2);
	timer_enable(Timerx6_T1);
	timer_enable(Timerx2_T2);
	timer_enable(Timerx2_T1);

	while(1)
	{
		if(flag)
		{
			printf("%u %u %u, %u, %u, %u, %u, %u\n", timerx2_t1_i_copy, timerx2_t2_i_copy, timerx6_t1_i_copy, timerx6_t2_i_copy, \
						timerx6_t3_i_copy, timerx6_t4_i_copy, timerx6_t5_i_copy, timerx6_t6_i_copy);
			flag = false;
		}
	}
#endif
}

void timerx2_t1_irqhandler(void)
{
	(void)TIMERX2->Timer1EOI;
	++timerx2_t1_i;
}

void timerx2_t2_irqhandler(void)
{
	(void)TIMERX2->Timer2EOI;
	++timerx2_t2_i;
}

void timerx6_t1_irqhandler(void)
{
	(void)TIMERX6->Timer1EOI;
	++timerx6_t1_i;
}

void timerx6_t2_irqhandler(void)
{
	(void)TIMERX6->Timer2EOI;
	(++timerx6_t2_i);
}

void timerx6_t3_irqhandler(void)
{
	(void)TIMERX6->Timer3EOI;
	(++timerx6_t3_i);
}

void timerx6_t4_irqhandler(void)
{
	(void)TIMERX6->Timer4EOI;
	(++timerx6_t4_i);
}

void timerx6_t5_irqhandler(void)
{
	(void)TIMERX6->Timer5EOI;
	(++timerx6_t5_i);
}

void timerx6_t6_irqhandler(void)
{
	(void)TIMERX6->Timer6EOI;
	(++timerx6_t6_i);
	timerx2_t1_i_copy = timerx2_t1_i;
	timerx2_t2_i_copy = timerx2_t2_i;
	timerx6_t1_i_copy = timerx6_t1_i;
	timerx6_t2_i_copy = timerx6_t2_i;
	timerx6_t3_i_copy = timerx6_t3_i;
	timerx6_t4_i_copy = timerx6_t4_i;
	timerx6_t5_i_copy = timerx6_t5_i;
	timerx6_t6_i_copy = timerx6_t6_i;
	flag = true;
}