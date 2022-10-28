#ifndef _UART_OLD_H_
#define _UART_OLD_H_

#include <stdint.h>
#include "dw_apb_uart_regs.h"


#ifdef A55
	#include <ca55_chip_define.h>
	#define TIMERX6 ((APE1210_Timerx6_TypeDef *)TIMERX6_BASE)
#else
	#include <cm3_chip_define.h>
#endif


#ifdef A55
	#include <ca55_chip_define.h>
	#define TIMERX6 ((APE1210_Timerx6_TypeDef *)TIMERX6_BASE)
#else
	#include <cm3_chip_define.h>
#endif

#define UART0 ((APE1210_UART_TypeDef *)(uintptr_t)UART0_BASE)
#define UART1 ((APE1210_UART_TypeDef *)(uintptr_t)UART1_BASE)
#define UART2 ((APE1210_UART_TypeDef *)(uintptr_t)UART2_BASE)
#define UART3 ((APE1210_UART_TypeDef *)(uintptr_t)UART3_BASE)

/*-----------------------------------------------------------------------
 * parameter definition clk
 *-----------------------------------------------------------------------*/
#define  SEEHI_UART_SCLK_200M           200000000
#define  SEEHI_UART_SCLK_100M           100000000
#define  SEEHI_UART_SCLK_96M            96000000
#define  SEEHI_UART_SCLK_66M            66000000
#define  SEEHI_UART_SCLK_62_4M          62400000
#define  SEEHI_UART_SCLK_31_2M          31200000
#define  SEEHI_UART_SCLK_24M            24000000
#define  SEEHI_UART_SCLK_20M            20000000
#define  SEEHI_UART_SCLK_4M             4000000
#define  SEEHI_UART_SCLK_2M             2000000

/*-----------------------------------------------------------------------
 * definition of UART Baudrate
 *-----------------------------------------------------------------------*/
#define SEEHI_UART_BAUDRATE_24M         24000000
#define SEEHI_UART_BAUDRATE_6M          6000000
#define SEEHI_UART_BAUDRATE_4M          4000000
#define SEEHI_UART_BAUDRATE_1500000     1500000
#define SEEHI_UART_BAUDRATE_500000      500000
#define SEEHI_UART_BAUDRATE_115200      115200
#define SEEHI_UART_BAUDRATE_57600       57600
#define SEEHI_UART_BAUDRATE_19200       19200
#define SEEHI_UART_BAUDRATE_9600        9600

typedef enum {
    SEEHI_UART0 = 0,
    SEEHI_UART1 = 1,
    SEEHI_UART2 = 2,
    SEEHI_UART3 = 3,
    SEEHI_UART_BUTT
} seehi_uart_no;

//-----------------------------------------------------------------------
//	\fn     void seehi_uart_config_baudrate(unsigned long baudrate, unsigned long uart_clk, seehi_uart_no uart_no)
//	\brief  config the Uart baudrate
//
//  \param  baudrate : the baudrate \n
//              e.g 9600 19200 57600 115200
//
//  \param  uart_clk : the Uart clock \n
//              e.g 24000000 : 24Mhz
//              e.g 50000000 : 50Mhz
//
//
//	\param  uart_no : the uart number selection \n
//		          0 - UART0\n
//		          1 - UART1\n
//		          2 - UART2\n
//		          3 - UART3
//
//	\return void
//-----------------------------------------------------------------------
extern int seehi_uart_config_baudrate(unsigned long baudrate, unsigned long uart_clk, seehi_uart_no uart_no);

//-----------------------------------------------------------------------
//	\fn     void seehi_uart_int_enable_old(int uart_no)
//	\brief  interrupt enable
//	\param  uart_no :the uart number selection\n
//		            0 - UART0\n
//		            1 - UART1\n
//		            2 - UART2\n
//		            3 - UART3
//
//	\return  void
//-----------------------------------------------------------------------
extern void seehi_uart_int_enable_old(seehi_uart_no uart_no);

//-----------------------------------------------------------------------
//	\fn     void seehi_uart_int_disable_old(int uart_no)
//	\brief  interrupt disable
//	\param  uart_no :the uart number selection\n
//		            0 - UART0\n
//		            1 - UART1\n
//		            2 - UART2\n
//		            3 - UART3
//
//	\return  void
//-----------------------------------------------------------------------
extern void seehi_uart_int_disable_old(seehi_uart_no uart_no);

// int putchar(int s);

// int getchar();
extern int uart_getchar(seehi_uart_no uart_no);

extern int uart_getc(seehi_uart_no uart_no);

extern int uart_sendchar(seehi_uart_no uart_no, const int c);

extern int tstc();

#endif
