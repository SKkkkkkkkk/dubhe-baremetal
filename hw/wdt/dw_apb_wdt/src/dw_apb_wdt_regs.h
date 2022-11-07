#ifndef __DW_APB_WDT_REGS_H__
#define __DW_APB_WDT_REGS_H__

typedef struct {
	__IOM uint32_t CR;
	__IOM uint32_t TORR;
	__IM uint32_t CCVR;
	__OM uint32_t CRR;
	__IM uint32_t STAT;
	__IM uint32_t EOI;
	RESERVED(0, uint32_t);
	__IOM uint32_t PROT_LEVEL;
	RESERVED(1 [49], uint32_t);
	__IM uint32_t COMP_PARAM_5;
	__IM uint32_t COMP_PARAM_4;
	__IM uint32_t COMP_PARAM_3;
	__IM uint32_t COMP_PARAM_2;
	__IM uint32_t COMP_PARAM_1;
	__IM uint32_t COMP_VERSION;
	__IM uint32_t COMP_TYPE;
} DW_APB_WDT_TypeDef;

#define WDT_EN_Pos 0UL
#define WDT_EN_Msk (1UL << WDT_EN_Pos)

#define RMOD_Pos 1UL
#define RMOD_MsK (1UL << RMOD_Pos)

#endif