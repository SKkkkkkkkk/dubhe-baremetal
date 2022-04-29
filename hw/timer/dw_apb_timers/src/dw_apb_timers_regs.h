#ifndef __APE1210_TIMER_REGS_H__
#define __APE1210_TIMER_REGS_H__

#include "regs_type.h"

typedef struct {
	__IOM uint32_t Timer1LoadCount;
	__IOM uint32_t Timer1CurrentValue;
	__IOM uint32_t Timer1ControlReg;
	__IOM uint32_t Timer1EOI;
	__IOM uint32_t Timer1IntStatus;

	__IOM uint32_t Timer2LoadCount;
	__IOM uint32_t Timer2CurrentValue;
	__IOM uint32_t Timer2ControlReg;
	__IOM uint32_t Timer2EOI;
	__IOM uint32_t Timer2IntStatus;
	RESERVED(0 [30], uint32_t)

	__IOM uint32_t TimersIntStatus;
	__IOM uint32_t TimersEOI;
	__IOM uint32_t TimersRawIntStatus;

	__IOM uint32_t TIMERS_COMP_VERSION;

	__IOM uint32_t Timer1LoadCount2;
	__IOM uint32_t Timer2LoadCount2;
	RESERVED(1 [6], uint32_t)

	__IOM uint32_t TIMER_1_PROT_LEVEL;
	__IOM uint32_t TIMER_2_PROT_LEVEL;
	RESERVED(2 [6], uint32_t)

} APE1210_Timerx2_TypeDef;

typedef struct {
	__IOM uint32_t Timer1LoadCount;
	__IOM uint32_t Timer1CurrentValue;
	__IOM uint32_t Timer1ControlReg;
	__IOM uint32_t Timer1EOI;
	__IOM uint32_t Timer1IntStatus;

	__IOM uint32_t Timer2LoadCount;
	__IOM uint32_t Timer2CurrentValue;
	__IOM uint32_t Timer2ControlReg;
	__IOM uint32_t Timer2EOI;
	__IOM uint32_t Timer2IntStatus;

	__IOM uint32_t Timer3LoadCount;
	__IOM uint32_t Timer3CurrentValue;
	__IOM uint32_t Timer3ControlReg;
	__IOM uint32_t Timer3EOI;
	__IOM uint32_t Timer3IntStatus;

	__IOM uint32_t Timer4LoadCount;
	__IOM uint32_t Timer4CurrentValue;
	__IOM uint32_t Timer4ControlReg;
	__IOM uint32_t Timer4EOI;
	__IOM uint32_t Timer4IntStatus;

	__IOM uint32_t Timer5LoadCount;
	__IOM uint32_t Timer5CurrentValue;
	__IOM uint32_t Timer5ControlReg;
	__IOM uint32_t Timer5EOI;
	__IOM uint32_t Timer5IntStatus;

	__IOM uint32_t Timer6LoadCount;
	__IOM uint32_t Timer6CurrentValue;
	__IOM uint32_t Timer6ControlReg;
	__IOM uint32_t Timer6EOI;
	__IOM uint32_t Timer6IntStatus;

	RESERVED(0 [10], uint32_t)

	__IOM uint32_t TimersIntStatus;
	__IOM uint32_t TimersEOI;
	__IOM uint32_t TimersRawIntStatus;

	__IOM uint32_t TIMERS_COMP_VERSION;

	__IOM uint32_t Timer1LoadCount2;
	__IOM uint32_t Timer2LoadCount2;
	__IOM uint32_t Timer3LoadCount2;
	__IOM uint32_t Timer4LoadCount2;
	__IOM uint32_t Timer5LoadCount2;
	__IOM uint32_t Timer6LoadCount2;
	RESERVED(1 [2], uint32_t)

	__IOM uint32_t TIMER_1_PROT_LEVEL;
	__IOM uint32_t TIMER_2_PROT_LEVEL;
	__IOM uint32_t TIMER_3_PROT_LEVEL;
	__IOM uint32_t TIMER_4_PROT_LEVEL;
	__IOM uint32_t TIMER_5_PROT_LEVEL;
	__IOM uint32_t TIMER_6_PROT_LEVEL;
	RESERVED(2 [2], uint32_t)

} APE1210_Timerx6_TypeDef;

typedef struct {
	__IM uint32_t CVL;
	__IM uint32_t CVH;
	__IOM uint32_t SVL;
	__IOM uint32_t SVH;
	__IOM uint32_t CTL;
	__IOM uint32_t IRQS;
} APE1210_Timerx6_TypeDef_old;

#define TIMER_ENABLE_Pos 0UL
#define TIMER_ENABLE_Msk (1UL << TIMER_ENABLE_Pos)

#define TIMER_MODE_Pos 1UL
#define TIMER_MODE_Msk (1UL << TIMER_MODE_Pos)

#define TIMER_INTERRUPT_MASK_Pos 2UL
#define TIMER_INTERRUPT_MASK_Msk (1UL << TIMER_INTERRUPT_MASK_Pos)

#endif