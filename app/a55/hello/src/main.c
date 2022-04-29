#ifdef QEMU
#include "pl001.h"
#include "irq_ctrl.h"
#include "xlat_tables_v2.h"
#include "wakeup_core.h"

#define MAP_ROM MAP_REGION_FLAT( \
0x00000000, \
0x04000000, \
MT_MEMORY|MT_RW|MT_EXECUTE|MT_SECURE )

#define MAP_RAM MAP_REGION_FLAT( \
0x0e000000, \
0x01000000, \
MT_MEMORY|MT_RW|MT_EXECUTE|MT_SECURE)


#define MAP_TEXT MAP_REGION_FLAT( \
0x00000000, \
0x04000000, \
MT_MEMORY|MT_RW|MT_EXECUTE|MT_SECURE )
#define MAP_RO_DATA
#define MAP_RW_DATA


#define DEVICE0_BASE			0x08000000
#define DEVICE0_SIZE			0x01000000
#define DEVICE1_BASE			0x09000000
#define DEVICE1_SIZE			0x00c00000

#define MAP_DEVICE0	MAP_REGION_FLAT(DEVICE0_BASE,			\
					DEVICE0_SIZE,			\
					MT_DEVICE | MT_RW | MT_SECURE)

#define MAP_DEVICE1	MAP_REGION_FLAT(DEVICE1_BASE,			\
					DEVICE1_SIZE,			\
					MT_DEVICE | MT_RW | MT_SECURE)


u_register_t get_sctlr_el3(void)		
{							
	u_register_t v;					
	__asm__ volatile ("mrs %0, sctlr_el3": "=r" (v));
	return v;						
}

volatile int a = 0x1234;
volatile int b = 0xa5a5;
volatile int c __attribute__((__aligned__(4096*2))) = 0xa5a5;
int main()
{
	(void)a;
	(void)b;
	(void)c;
	// const mmap_region_t plat_bl1_region[] = {
	// 	MAP_ROM,
	// 	MAP_RAM,
	// 	MAP_DEVICE0,
	// 	MAP_DEVICE1,
	// 	{0}
	// };
	// (void)plat_bl1_region;
	// mmap_add(plat_bl1_region);
	// init_xlat_tables();
	// (void)get_sctlr_el3();
	// enable_mmu_el3(0);
	uart_write("hello world.\n");
	uart_write("hello world????.\n");
	while(1);
	return 0;
}
#else
#include "dw_apb_uart.h"
#include <stdio.h>
#include "seehi_print.h"
#include "gic.h"
#include "irq_ctrl.h"

void test_irq_handler(void)
{
	uart_sendchar(SEEHI_UART1, '?');
	uart_sendchar(SEEHI_UART1, '?');
	uart_sendchar(SEEHI_UART1, '?');
	uart_sendchar(SEEHI_UART1, '?');
	uart_sendchar(SEEHI_UART1, '\n');
	uart_sendchar(SEEHI_UART1, '\r');
}
int main()
{
	seehi_uart_config_baudrate(SEEHI_UART_BAUDRATE_115200, 20000000, SEEHI_UART1);
	// uart_sendchar(SEEHI_UART1, '?');
	// // char* c = "hello world\n";
	// uart_sendchar(SEEHI_UART1, 'h');
	// uart_sendchar(SEEHI_UART1, 'e');
	// uart_sendchar(SEEHI_UART1, 'l');
	// uart_sendchar(SEEHI_UART1, 'l');
	// uart_sendchar(SEEHI_UART1, 'o');
	// timer_init_config_t timer_init_config = {
	// 	.int_mask = true, .loadcount = TIMER_FREQ, .timer_id = Timerx2_T1, .timer_mode = Mode_User_Defined
	// };
	// timer_init(&timer_init_config);
	// GIC_SetTarget(32, 1 << 0); //设置core0为目标core
	// IRQ_SetHandler(32, test_irq_handler); //设置中断处理函数
	// IRQ_SetPriority(32, 0 << 3); //设置优先级
	// IRQ_Enable(32); //使能该中断
	// IRQ_SetPending(32);
	// uart_sendchar(SEEHI_UART1, 't');
	// uart_sendchar(SEEHI_UART1, 'i');
	// uart_sendchar(SEEHI_UART1, 'c');
	// uart_sendchar(SEEHI_UART1, 'k');
	// uart_sendchar(SEEHI_UART1, '\n');
	// uart_sendchar(SEEHI_UART1, '\r');
	while(1)
	{
		for(volatile uint32_t i = 0xffff;i!=0;i--);
		uart_sendchar(SEEHI_UART1, 't');
		uart_sendchar(SEEHI_UART1, 'i');
		uart_sendchar(SEEHI_UART1, 'c');
		uart_sendchar(SEEHI_UART1, 'k');
		uart_sendchar(SEEHI_UART1, '\n');
		uart_sendchar(SEEHI_UART1, '\r');
	}
	// seehi_printf("\nhello world\n");
	// printf("\nhello world\n");
	// while(*c!='\0')
	// 	uart_sendchar(SEEHI_UART1, *(c++));
	while(1);
	return 0;
}
#endif