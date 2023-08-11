#if defined (A55) || (RI5CY)

#include "chip_mem_layout.h"

#define __GIC_PRESENT		 1U
#ifdef A55
	#ifdef QEMU
		#define GIC_DISTRIBUTOR_BASE (0x08000000UL)
		#define GIC_INTERFACE_BASE   (0x08010000UL)
	#else
		#define GIC400_BASE			 (GIC_BASE)
		#define GIC_DISTRIBUTOR_BASE (GIC400_BASE + 0x1000UL)
		#define GIC_INTERFACE_BASE   (GIC400_BASE + 0x2000UL)
	#endif
#else // RI5CY
	#define GIC_DISTRIBUTOR_BASE (0x030C1000UL)
	#define GIC_INTERFACE_BASE   (0x030C2000UL)
#endif


#define OFFSET_SPIs	32
#ifdef A55
	typedef enum IRQn
	{
	/******  SGI Interrupts Numbers                 ****************************************/
		SGI0_IRQn            =  0,        /*!< Software Generated Interrupt 0 */
		SGI1_IRQn            =  1,        /*!< Software Generated Interrupt 1 */
		SGI2_IRQn            =  2,        /*!< Software Generated Interrupt 2 */
		SGI3_IRQn            =  3,        /*!< Software Generated Interrupt 3 */
		SGI4_IRQn            =  4,        /*!< Software Generated Interrupt 4 */
		SGI5_IRQn            =  5,        /*!< Software Generated Interrupt 5 */
		SGI6_IRQn            =  6,        /*!< Software Generated Interrupt 6 */
		SGI7_IRQn            =  7,        /*!< Software Generated Interrupt 7 */
		SGI8_IRQn            =  8,        /*!< Software Generated Interrupt 8 */
		SGI9_IRQn            =  9,        /*!< Software Generated Interrupt 9 */
		SGI10_IRQn           = 10,        /*!< Software Generated Interrupt 10 */
		SGI11_IRQn           = 11,        /*!< Software Generated Interrupt 11 */
		SGI12_IRQn           = 12,        /*!< Software Generated Interrupt 12 */
		SGI13_IRQn           = 13,        /*!< Software Generated Interrupt 13 */
		SGI14_IRQn           = 14,        /*!< Software Generated Interrupt 14 */
		SGI15_IRQn           = 15,        /*!< Software Generated Interrupt 15 */

	/******  Cortex-A7 Processor Exceptions Numbers ****************************************/
		Virtual_maintenance_IRQn  				= 25,
		Hypervisor_timer_IRQn 					= 26,
		Virtual_timer_IRQn		 				= 27,
		Legacy_FIQ_signal_IRQn 					= 28,
		Secure_physical_timer_IRQn  			= 29,
		Non_secure_physical_timer_IRQn			= 30,
		Legacy_IRQ_signal_IRQn					= 31,

		/******  Platform Exceptions Numbers ***************************************************/
		Timerx6_T1_IRQn		= (0+OFFSET_SPIs),
		Timerx6_T2_IRQn		= (1+OFFSET_SPIs),
		Timerx6_T3_IRQn		= (2+OFFSET_SPIs),
		Timerx6_T4_IRQn		= (3+OFFSET_SPIs),
		Timerx6_T5_IRQn		= (4+OFFSET_SPIs),
		Timerx6_T6_IRQn		= (5+OFFSET_SPIs),
		Timerx2_T1_IRQn		= (6+OFFSET_SPIs),
		Timerx2_T2_IRQn		= (7+OFFSET_SPIs),
		WDT0_IRQn			= (8+OFFSET_SPIs),
		Mailbox_IRQn		= (9+OFFSET_SPIs),
		RTC_IRQn			= (10+OFFSET_SPIs),
		WDT1_IRQn			= (11+OFFSET_SPIs),
		PMU_IRQn			= (12+OFFSET_SPIs),
		NPU0_IRQn			= (13+OFFSET_SPIs),
	#if defined(HW_VERSION_MPW) // Removed in FM
		NPU1_IRQn			= (14+OFFSET_SPIs),
	#endif
		DMAC0_IRQn			= (15+OFFSET_SPIs),
		ISP_IRQn			= (16+OFFSET_SPIs),
		ISP_MI_IRQn			= (17+OFFSET_SPIs),
		ISP_FE_IRQn			= (18+OFFSET_SPIs),
		DWP_DWE_IRQn		= (19+OFFSET_SPIs),
		DWP_VSE_IRQn		= (20+OFFSET_SPIs),
		DWP_FE_IRQn			= (21+OFFSET_SPIs),
		Uart4_IRQn			= (22+OFFSET_SPIs),
	#if defined(HW_VERSION_MPW) // Substituted for GMAC in FM
		GPU_GPU_IRQn		= (23+OFFSET_SPIs),
		GPU_MMU_IRQn		= (24+OFFSET_SPIs),
		GPU_Job_IRQn		= (25+OFFSET_SPIs),
		GPU_Evet_IRQn		= (26+OFFSET_SPIs),
	#else
		GMAC_SBD_IRQn		= (23+OFFSET_SPIs),
		GMAC_SBD_PERCH_IRQn	= (24+OFFSET_SPIs),	
		GMAC_SBD_SFTY_IRQn  = (25+OFFSET_SPIs),
	#endif
	#if defined(HW_VERSION_MPW) // Substituted for CVE in FM
		ROT_Raw_IRQn		= (27+OFFSET_SPIs),
		ROT_YUV_IRQn		= (28+OFFSET_SPIs),
	#else
		CVE_ROTATE_IRQn		= (27+OFFSET_SPIs),
		CVE_SCALER_IRQn		= (28+OFFSET_SPIs)
	#endif
		IDI_Bridge_IRQn		= (29+OFFSET_SPIs),
		IDMA_IRQn			= (30+OFFSET_SPIs),
		DC_DE0_IRQn			= (31+OFFSET_SPIs),
		DC_SE0_IRQn			= (32+OFFSET_SPIs),
		PWM4_IRQn			= (33+OFFSET_SPIs),
		CSI2_Ctrl0_IRQn		= (34+OFFSET_SPIs),
		CSI2_Ctrl1_IRQn		= (35+OFFSET_SPIs),
		CSI2_Ctrl2_IRQn		= (36+OFFSET_SPIs),
		CSI2_Ctrl3_IRQn		= (37+OFFSET_SPIs),
		DSI_TX_IRQn			= (38+OFFSET_SPIs),
		CSI2_TX_IRQn		= (39+OFFSET_SPIs),
		Uart5_IRQn			= (40+OFFSET_SPIs),
		Uart6_IRQn			= (41+OFFSET_SPIs),
		Uart7_IRQn			= (42+OFFSET_SPIs),
		VPU_IRQn			= (43+OFFSET_SPIs),
		USB_IRQn			= (44+OFFSET_SPIs),
		GMAC_IRQn			= (45+OFFSET_SPIs),
		EMMC0_IRQn			= (46+OFFSET_SPIs),
		EMMC1_IRQn			= (47+OFFSET_SPIs),
		SDIO2_IRQn			= (48+OFFSET_SPIs),
		DMAC1_IRQn			= (49+OFFSET_SPIs),
		SPACC_IRQn			= (50+OFFSET_SPIs),
		PKA_IRQn			= (51+OFFSET_SPIs),
		EMMC0_Wakeup_IRQn	= (52+OFFSET_SPIs),
		PWM5_IRQn			= (53+OFFSET_SPIs),
		PWM6_IRQn			= (54+OFFSET_SPIs),
		PWM7_IRQn			= (55+OFFSET_SPIs),
		BootSPI_IRQn		= (56+OFFSET_SPIs),
		SPI0_IRQn			= (57+OFFSET_SPIs),
		SPI1_IRQn			= (58+OFFSET_SPIs),
		SPI2_IRQn			= (59+OFFSET_SPIs),
		SPIS_IRQn			= (60+OFFSET_SPIs),
		I2C4_IRQn			= (61+OFFSET_SPIs),
		I2C5_IRQn			= (62+OFFSET_SPIs),
		I2C6_IRQn			= (63+OFFSET_SPIs),
		I2C0_IRQn			= (64+OFFSET_SPIs),
		I2C1_IRQn			= (65+OFFSET_SPIs),
		I2C2_IRQn			= (66+OFFSET_SPIs),
		I2C3_IRQn			= (67+OFFSET_SPIs),
		PDM4_IRQn			= (68+OFFSET_SPIs),
		PDM5_IRQn			= (69+OFFSET_SPIs),
		VAD_IRQn			= (70+OFFSET_SPIs),
		PDM3_IRQn			= (71+OFFSET_SPIs),
		UART0_IRQn			= (72+OFFSET_SPIs),
		UART1_IRQn			= (73+OFFSET_SPIs),
		UART2_IRQn			= (74+OFFSET_SPIs),
		UART3_IRQn			= (75+OFFSET_SPIs),
		PDM2_IRQn			= (76+OFFSET_SPIs),
		I2S0_IRQn			= (77+OFFSET_SPIs),
		ADC_IRQn			= (78+OFFSET_SPIs),
		GPIO0_IRQn			= (79+OFFSET_SPIs),
		GPIO1_IRQn			= (80+OFFSET_SPIs),
		GPIO2_IRQn			= (81+OFFSET_SPIs),
		I2S3_IRQn			= (82+OFFSET_SPIs),
		DDR0_IRQn			= (83+OFFSET_SPIs),
		DDR1_IRQn			= (84+OFFSET_SPIs),
		PMON0_IRQn			= (85+OFFSET_SPIs),
		PMON1_IRQn			= (86+OFFSET_SPIs),
		I2S1_IRQn			= (87+OFFSET_SPIs),
		I2S2_IRQn			= (88+OFFSET_SPIs),
		PDM0_IRQn			= (89+OFFSET_SPIs),
		PDM1_IRQn			= (90+OFFSET_SPIs),
		PWM0_IRQn			= (91+OFFSET_SPIs),
		PWM1_IRQn			= (92+OFFSET_SPIs),
		PWM2_IRQn			= (93+OFFSET_SPIs),
		PWM3_IRQn			= (94+OFFSET_SPIs),
		EFUSE_INTR_IRQN		= (95+OFFSET_SPIs),
		CPU_NCLUSTERPMUIRQ_IRQn	= (96+OFFSET_SPIs),	
		CPU_NPMUIRQ0_IRQn	= (97+OFFSET_SPIs),
		CPU_NPMUIRQ1_IRQn	= (98+OFFSET_SPIs),
		CPU_NPMUIRQ2_IRQn	= (99+OFFSET_SPIs),
		CPU_NPMUIRQ3_IRQn	= (100+OFFSET_SPIs),
		CLCI_Error_IRQn		= (101+OFFSET_SPIs),
		CLCI_Mission_IRQn	= (102+OFFSET_SPIs),

	} IRQn_Type;
#else
	typedef enum IRQn
	{
		Timer0_IRQn     = 0+OFFSET_SPIs,
		Timer1_IRQn		= 1+OFFSET_SPIs,
		BootSPI_IRQn	= 3+OFFSET_SPIs,
		I2C_IRQn		= 5+OFFSET_SPIs,
		Uart_IRQn		= 7+OFFSET_SPIs,
		VAD_IRQn		= 9+OFFSET_SPIs,
		I2S_IRQn		= 11+OFFSET_SPIs,
		GPIO_IRQn		= 13+OFFSET_SPIs,	
	} IRQn_Type;
#endif


#endif