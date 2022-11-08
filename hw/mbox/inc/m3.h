/**************************************************************************//**
 * @file     ARMCM3.h
 * @brief    CMSIS Core Peripheral Access Layer Header File for
 *           ARMCM3 Device
 * @version  V5.3.1
 * @date     09. July 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARMCM3_H
#define ARMCM3_H

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */

#ifdef M3
typedef enum IRQn
{
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn           = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn                = -13,     /*  3 HardFault Interrupt */
  MemoryManagement_IRQn         = -12,     /*  4 Memory Management Interrupt */
  BusFault_IRQn                 = -11,     /*  5 Bus Fault Interrupt */
  UsageFault_IRQn               = -10,     /*  6 Usage Fault Interrupt */
  SVCall_IRQn                   =  -5,     /* 11 SV Call Interrupt */
  DebugMonitor_IRQn             =  -4,     /* 12 Debug Monitor Interrupt */
  PendSV_IRQn                   =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn                  =  -1,     /* 15 System Tick Interrupt */

/* -------------------  Processor Interrupt Numbers  ------------------------------ */
  INT_TIMER0_INTR0              =  6,
  INT_TIMER0_INTR1              =  7,
  INT_WDT0_INTR                 =  8,
  INT_MAILBOX_BB_INTR           =  9,
  INT_RTC_INTR                  = 10,
  INT_PMU_INTR                  = 12,
  INT_NPU0_INTR                 = 13,
  INT_NPU1_INTR                 = 14,
  INT_ISP_INTR                  = 16,
  INT_ISP_MI_INTR               = 17,
  INT_ISP_FE_INTR               = 18,
  INT_DWP_DWE_INT               = 19,
  INT_DWP_VSE_INT               = 20,
  INT_DWP_FE_INT                = 21,
  INT_UART4_INTR                = 22,
  INT_GPU_GPU_INTR              = 23,
  INT_GPU_MMU_INTR              = 24,
  INT_GPU_JOB_INTR              = 25,
  INT_GPU_EVENT_INTR            = 26,
  INT_ROT_RAW_INTR              = 27,
  INT_ROT_YUV_INTR              = 28,
  INT_IDI_BRIDGE_INTR           = 29,
  INT_IDI_INTR                  = 30,
  INT_DC_DE0_INTR               = 31,
  INT_DC_SE0_INTR               = 32,
  INT_PWM4_INTR                 = 33,
  INT_CSI2_CTRL0_INTR           = 34,
  INT_CSI2_CTRL1_INTR           = 35,
  INT_CSI2_CTRL2_INTR           = 36,
  INT_CSI2_CTRL3_INTR           = 37,
  INT_DSI_TX_INTR               = 38,
  INT_CSI2_TX_INTR              = 39,
  INT_UART5_INTR                = 40,
  INT_UART6_INTR                = 41,
  INT_UART7_INTR                = 42,
  INT_VPU_INTR                  = 43,
  INT_USB_INTR                  = 44,
  INT_GMAC_INTR                 = 45,
  INT_EMMC0_INTR                = 46,
  INT_EMMC1_INTR                = 47,
  INT_SDIO2_INTR                = 48,
  INT_DMAC0_INTR                = 49,
  INT_SPACC_INTR                = 50,
  INT_PKA_INTR                  = 51,
  INT_EMMC0_WAKEUP_INTR         = 52,
  INT_PWM5_INTR                 = 53,
  INT_PWM6_INTR                 = 54,
  INT_PWM7_INTR                 = 55,
  INT_BOOTSPI_INTR              = 56,
  INT_SPI0_INTR                 = 57,
  INT_SPI1_INTR                 = 58,
  INT_SPI2_INTR                 = 59,
  INT_SPI3_INTR                 = 60,
  INT_I2C4_INTR                 = 61,
  INT_I2C5_INTR                 = 62,
  INT_I2C6_INTR                 = 63,
  INT_I2C0_INTR                 = 64,
  INT_I2C1_INTR                 = 65,
  INT_I2C2_INTR                 = 66,
  INT_I2C3_INTR                 = 67,
  INT_PDM4_INTR                 = 68,
  INT_PDM5_INTR                 = 69,
  INT_VAD_INTR                  = 70,
  INT_PDM3_INTR                 = 71,
  INT_UART0_INTR                = 72,
  INT_UART1_INTR                = 73,
  INT_UART2_INTR                = 74,
  INT_UART3_INTR                = 75,
  INT_PDM2_INTR                 = 76,
  INT_I2S0_INTR                 = 77,
  INT_ADC_INTR                  = 78,
  INT_GPIO0_INTR                = 79,
  INT_GPIO1_INTR                = 80,
  INT_GPIO2_INTR                = 81,
  INT_I2S3_INTR                 = 82,
  INT_DDRCTRL0_INTR             = 83,
  INT_DDRCTRL1_INTR             = 84,
  INT_PMON0_INTR                = 85,
  INT_PMON1_INT                 = 86,
  INT_I2S1_INTR                 = 87,
  INT_I2S2_INTR                 = 88,
  INT_PDM0_INTR                 = 89,
  INT_PDM1_INTR                 = 90,
  INT_PWM0_INTR                 = 91,
  INT_PWM1_INTR                 = 92,
  INT_PWM2_INTR                 = 93,
  INT_PWM3_INTR                 = 94,
  INT_EFUSE_INTR                = 95

  /* Interrupts 10 .. 224 are left out */
} IRQn_Type;
#endif


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


/* --------  Configuration of Core Peripherals  ----------------------------------- */
#define __CM3_REV                 0x0201U   /* Core revision r2p1 */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */

#include "core_cm3.h"                       /* Processor and core peripherals */
#include "system_m3.h"                  /* System Header */


/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


#ifdef __cplusplus
}
#endif

#endif  /* ARMCM3_H */
