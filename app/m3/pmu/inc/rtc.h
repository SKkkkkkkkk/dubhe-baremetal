#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
    extern "C" {
#endif
#if defined M3
#include "_cm3_chip_define.h"
#else
#include "_ca55_chip_define.h"
#endif
typedef struct {
	__IOM uint32_t RTC_WPR;
	__IOM uint32_t RTC_ISO_EN;
	__IOM uint32_t RTC_ISO_TIME;
	__IOM uint32_t RTCSD;
	__IOM uint32_t INIT_TIME;
	__IM  uint32_t CUR_TIME;
	__IOM uint32_t ALARM_EN;
	__IOM uint32_t ALARM_TIME;
	__IOM uint32_t POWER_CNT_CLR;
	__IOM uint32_t POWER_DETECT_EN;
	__IM  uint32_t POWER_CNT;
	__IOM uint32_t IO_CFG;
	__IM  uint32_t RST_PULL_DOWN;
} RTC_TypeDef;

#define RTC    ((RTC_TypeDef*) RTC_BASE)
typedef struct {
    __IOM uint32_t  PMIC_WPR;
    __IOM uint32_t  SOFT_REG0;
    __IOM uint32_t  SOFT_REG1;
    __IOM uint32_t  SOFT_REG2;
    __IOM uint32_t  SOFT_REG3;
    __IOM uint32_t  SOFT_REG4;
    __IOM uint32_t  SOFT_REG5;
    __IOM uint32_t  SOFT_REG6;
    __IOM uint32_t  SOFT_REG7;
    __IOM uint32_t  IRQ_OUT_MODE;
    __IOM uint32_t  IRQ_OUT_POLAR;
    __IOM uint32_t  IRQ_OUT_TIME;
    __IOM uint32_t  IRQ_IN_MODE;
    __IOM uint32_t  IO_CFG;
 } PMIC_TypeDef;

#define PMIC    ((PMIC_TypeDef*) (RTC_BASE + 0x100))
typedef struct {
    __IOM uint32_t  INT_CLR;
    __IOM uint32_t  INT_MASK;
 } INT_TypeDef;

#define INT    ((INT_TypeDef*) (RTC_BASE + 0x200))


#ifdef __cplusplus
    }
#endif

#endif
