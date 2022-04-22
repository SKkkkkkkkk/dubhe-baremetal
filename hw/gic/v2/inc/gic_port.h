#if defined (A55) || (RI5CY)

#define __GIC_PRESENT		 1U
#ifdef A55
	#ifdef QEMU
		#define GIC_DISTRIBUTOR_BASE (0x08000000UL)
		#define GIC_INTERFACE_BASE   (0x08010000UL)
		#define __GIC_PRIO_BITS 	 5U
		#define IRQ_GIC_LINE_COUNT   128U
	#else
		#define GIC_DISTRIBUTOR_BASE (0x04011000UL)
		#define GIC_INTERFACE_BASE   (0x04012000UL)
		#define __GIC_PRIO_BITS 	 5U
		#define IRQ_GIC_LINE_COUNT   128U
	#endif
#else // RI5CY
	#define GIC_DISTRIBUTOR_BASE (0x030C1000UL)
	#define GIC_INTERFACE_BASE   (0x030C2000UL)
	#define __GIC_PRIO_BITS 	 5U
	#define IRQ_GIC_LINE_COUNT   128U
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
		SecurePhyTimer_IRQn  = 29,        /*!< Physical Timer Interrupt                      */

	/******  Platform Exceptions Numbers ***************************************************/
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