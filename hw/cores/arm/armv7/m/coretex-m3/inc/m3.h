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
	/******  Platform Exceptions Numbers ***************************************************/
		// Timerx6_T1_IRQn		= 0,
		// Timerx6_T2_IRQn		= 1,
		// Timerx6_T3_IRQn		= 2,
		// Timerx6_T4_IRQn		= 3,
		// Timerx6_T5_IRQn		= 4,
		// Timerx6_T6_IRQn		= 5,
		Timerx2_T1_IRQn		= 6,
		Timerx2_T2_IRQn		= 7,
		WDT_IRQn			    = 8,
		Mailbox_IRQn		  = 9,
		RTC_IRQn			    = 10,

		PMU_IRQn			    = 12,
		NPU0_IRQn			    = 13,
		NPU1_IRQn			    = 14,
		
		ISP_IRQn			    = 16,
		ISP_MI_IRQn			  = 17,
		ISP_FE_IRQn			  = 18,
		DWP_DWE_IRQn		  = 19,
		DWP_VSE_IRQn		  = 20,
		DWP_FE_IRQn			  = 21,
		Uart4_IRQn			  = 22,
		GPU_GPU_IRQn		  = 23,
		GPU_MMU_IRQn		  = 24,
		GPU_Job_IRQn		  = 25,
		GPU_Evet_IRQn		  = 26,
		ROT_Raw_IRQn		  = 27,
		ROT_YUV_IRQn		  = 28,
		IDI_Bridge_IRQn		= 29,
		IDI_IRQn		    	= 30,
		DC_DE0_IRQn			  = 31,
		DC_SE0_IRQn			  = 32,
		PWM4_IRQn			    = 33,
		CSI2_Ctrl0_IRQn		= 34,
		CSI2_Ctrl1_IRQn		= 35,
		CSI2_Ctrl2_IRQn		= 36,
		CSI2_Ctrl3_IRQn		= 37,
		DSI_TX_IRQn			  = 38,
		CSI2_TX_IRQn		  = 39,
		Uart5_IRQn			  = 40,
		Uart6_IRQn			  = 41,
		Uart7_IRQn			  = 42,
		VPU_IRQn			    = 43,
		USB_IRQn			    = 44,
		GMAC_IRQn			    = 45,
		EMMC0_IRQn			  = 46,
		EMMC1_IRQn			  = 47,
		SDIO2_IRQn			  = 48,
		DMAC1_IRQn			  = 49,
		SPACC_IRQn			  = 50,
		PKA_IRQn			    = 51,
		EMMC0_Wakeup_IRQn	= 52,
		PWM5_IRQn			    = 53,
		PWM6_IRQn			    = 54,
		PWM7_IRQn			    = 55,
		BootSPI_IRQn	    = 56,
		SPI0_IRQn			    = 57,
		SPI1_IRQn			    = 58,
		SPI2_IRQn			    = 59,
		SPIS_IRQn			    = 60,
		I2C4_IRQn			    = 61,
		I2C5_IRQn			    = 62,
		I2C6_IRQn			    = 63,
		I2C0_IRQn			    = 64,
		I2C1_IRQn			    = 65,
		I2C2_IRQn			    = 66,
		I2C3_IRQn			    = 67,
		PDM4_IRQn			    = 68,
		PDM5_IRQn			    = 69,
		VAD_IRQn			    = 70,
		PDM3_IRQn			    = 71,
		UART0_IRQn		    = 72,
		UART1_IRQn		    = 73,
		UART2_IRQn		    = 74,
		UART3_IRQn		    = 75,
		PDM2_IRQn			    = 76,
		I2S0_IRQn			    = 77,
		ADC_IRQn			    = 78,
		GPIO0_IRQn		    = 79,
		GPIO1_IRQn		    = 80,
		GPIO2_IRQn		    = 81,
		I2S3_IRQn			    = 82,
		DDR0_IRQn			    = 83,
		DDR1_IRQn			    = 84,
		PMON0_IRQn		    = 85,
		PMON1_IRQn		    = 86,
		I2S1_IRQn			    = 87,
		I2S2_IRQn			    = 88,
		PDM0_IRQn			    = 89,
		PDM1_IRQn			    = 90,
		PWM0_IRQn			    = 91,
		PWM1_IRQn			    = 92,
		PWM2_IRQn			    = 93,
		PWM3_IRQn			    = 94,
		eFUSE_IRQN		    = 95,
		// CPU_NCLUSTERPMUIRQ_IRQn	= 96,	
		// CPU_NPMUIRQ0_IRQn	= 97,
		// CPU_NPMUIRQ1_IRQn	= 98,
		// CPU_NPMUIRQ2_IRQn	= 99,
		// CPU_NPMUIRQ3_IRQn	= 100,
		CLCI_Error_IRQn		= 101,
		CLCI_Mission_IRQn	= 102,
} IRQn_Type;


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
#ifdef QEMU
  #define __NVIC_PRIO_BITS          8U
#else
  #define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#endif
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
