#ifndef __DW_APB_GPIO_REGS_H__
#define __DW_APB_GPIO_REGS_H__

#include "regs_type.h"
typedef struct _DW_APB_GPIO {
	__IOM uint32_t DR;
	__IOM uint32_t DDR;
	__IOM uint32_t CTL;
	RESERVED(0 [9], uint32_t)
	__IOM uint32_t INTEN;
	__IOM uint32_t INTMASK;
	__IOM uint32_t INTTYPE_LEVEL;
	__IOM uint32_t INT_POLARITY;
	__IOM uint32_t INTSTATUS;
	__IOM uint32_t RAW_INTSTATUS;
	__IOM uint32_t DEBOUNCE;
	__IOM uint32_t PORTA_EOI;
	__IOM uint32_t EXT_PORTA;
	RESERVED(1 [3], uint32_t)
	__IOM uint32_t LS_SYNC;
	__IOM uint32_t ID_CODE;
	__IOM uint32_t INT_BOTHEDGE;
	__IOM uint32_t VER_ID_CODE;
	__IOM uint32_t CONFIG_REG2;
	__IOM uint32_t CONFIG_REG1;
} DW_APB_GPIO_TypeDef;

#endif