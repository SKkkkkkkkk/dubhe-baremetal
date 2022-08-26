#ifndef QEMU
#include "dw_apb_uart.h"
#include "seehi_print.h"
#include "gic.h"
#include "irq_ctrl.h"
#include "dw_apb_timers.h"
#include "systimer.h"
#include "xlat_tables_v2.h"
#include <stdio.h>

uint32_t arch_timer_get_cntfrq_el0(void)
{
	uint32_t cntfrq_el0;
	asm volatile("mrs %0, cntfrq_el0": "=r"(cntfrq_el0));
  	return cntfrq_el0;
}

void arch_timer_set_cntfrq_el0(uint32_t cntfrq_el0)
{
	asm volatile("msr cntfrq_el0, %0": "=r"(cntfrq_el0));
	return;
}


uint64_t arch_timer_get_cntpct_el0(void)
{
	uint64_t cntpct;
	asm volatile("mrs %0, cntpct_el0": "=r"(cntpct));
	return cntpct;
}

void arch_timer_test()
{



}

void test_irq_handler(void)
{
	static int i = 0;
	printf("%d in irq\n",i++);
	(void)TIMERX6->Timer2EOI;
}

#define round_up(value, boundary)		\
	((((value) - 1) | round_boundary(value, boundary)) + 1)

#define __ROM_BASE 0x00000000
#define __ROM_SIZE 0x00010000 //64KB

#define __RAM_BASE 0x00A00000
#define __RAM_SIZE 0x00020000 //128KB

#define __DDR_BASE 0x80000000
#define __DDR_SIZE 0x01000000

extern char __TEXT_START__[];
extern char __TEXT_END__[];
extern char __RODATA_START__[];
extern char __RODATA_END__[];


#define MAP_ROM MAP_REGION_FLAT( \
 __ROM_BASE, \
 __ROM_SIZE, \
 MT_CODE|MT_SECURE )

#define MAP_RAM MAP_REGION_FLAT( \
 __RAM_BASE, \
 __RAM_SIZE, \
 MT_RW_DATA|MT_SECURE )


#define MAP_DDR MAP_REGION_FLAT( \
__DDR_BASE, \
__DDR_SIZE, \
MT_RW_DATA|MT_SECURE )

#define MAP_CODE MAP_REGION_FLAT( \
(unsigned long)&__TEXT_START__, \
((unsigned long)&__TEXT_END__ - (unsigned long)&__TEXT_START__), \
MT_CODE|MT_SECURE )


#define MAP_DEVICE MAP_REGION_FLAT( \
0x04000000,			\
0x0a001000,			\
MT_DEVICE|MT_RW|MT_SECURE)


int putchar(int c)
{
	if(c == '\n')
		if(uart_sendchar(SEEHI_UART1, '\r') < 0)
			return -1;
	return uart_sendchar(SEEHI_UART1, c);
}

// int main()
// {
// 	seehi_uart_config_baudrate(SEEHI_UART_BAUDRATE_115200, 20000000, SEEHI_UART1);
// 	printf("hi\n\r");
// 	*(uint32_t*)(uintptr_t)(0x2E020000) = 0x1;
// 	dsb();
// 	// const mmap_region_t mmap_region_list[] = {
// 	// 	MAP_DDR,
// 	// 	MAP_CODE,
// 	// 	MAP_DEVICE,
// 	// 	{0}
// 	// };
// 	// mmap_add(mmap_region_list);
// 	// init_xlat_tables();
// 	// enable_mmu_el3(0);

// 	GIC_Enable();
// 	systimer_init();
// 	timer_init_config_t timer_init_config = {
// 		.int_mask = false, .loadcount = 20000000, .timer_id = Timerx6_T2, .timer_mode = Mode_User_Defined
// 	};
// 	timer_init(&timer_init_config);
// 	IRQ_SetHandler(Timerx6_2_IRQn, test_irq_handler); //设置中断处理函数
// 	IRQ_SetPriority(Timerx6_2_IRQn, 0 << 3); //设置优先级
// 	IRQ_Enable(Timerx6_2_IRQn); //使能该中断
// 	timer_enable(timer_init_config.timer_id);

// 	// printf("cntfrq: %d\n",arch_timer_get_cntfrq_el0());
// 	// arch_timer_set_cntfrq_el0(1875000);
// 	// printf("cntfrq: %d\n",arch_timer_get_cntfrq_el0());

// 	while(1)
// 	{
// 		printf("0x%lx\n\r", read_cntpct_el0());
// 		systimer_delay(1, IN_S);
// 	}
// 	return 0;
// }


int main()
{
	seehi_uart_config_baudrate(SEEHI_UART_BAUDRATE_115200, 20000000, SEEHI_UART1);
	printf("in main:\n\r");
	GIC_Enable();
	*(uint32_t*)(uintptr_t)(0x2E020000) = 0x1;
	dsb();

	// timer_init_config_t timer_init_config = {
	// 	.int_mask = false, .loadcount = 20000000, .timer_id = Timerx6_T2, .timer_mode = Mode_User_Defined
	// };
	// timer_init(&timer_init_config);
	// IRQ_SetHandler(Timerx6_2_IRQn, test_irq_handler); //设置中断处理函数
	// IRQ_SetPriority(Timerx6_2_IRQn, 0 << 3); //设置优先级
	// IRQ_Enable(Timerx6_2_IRQn); //使能该中断
	// timer_enable(timer_init_config.timer_id);
	// IRQ_SetPending(Timerx6_2_IRQn);
	// while(1);

	printf("Generic Timer Test:\n\r");
	void EL3_physical_timer_delay1(void);
	void EL3_physical_timer_delay2(void);
	void EL3_physical_timer_delay3(void);
	void EL3_physical_timer_delay4(void);
	void Non_secure_EL2_physical_timer_delay1(void);
	void Non_secure_EL2_physical_timer_delay2(void);
	void Non_secure_EL2_physical_timer_delay3(void);
	void Non_secure_EL2_physical_timer_delay4(void);
	void EL1_physical_timer_delay1(void);
	void EL1_physical_timer_delay2(void);
	void EL1_physical_timer_delay3(void);
	void EL1_physical_timer_delay4(void);
	void EL1_virtual_timer_delay1(void);
	void EL1_virtual_timer_delay2(void);
	void EL1_virtual_timer_delay3(void);
	void EL1_virtual_timer_delay4(void);
	void Non_secure_EL2_virtual_timer_delay1(void);
	void Non_secure_EL2_virtual_timer_delay2(void);
	void Non_secure_EL2_virtual_timer_delay3(void);
	void Non_secure_EL2_virtual_timer_delay4(void);
	EL3_physical_timer_delay1();
	EL3_physical_timer_delay2();
	EL3_physical_timer_delay3();
	EL3_physical_timer_delay4();
	Non_secure_EL2_physical_timer_delay1();
	Non_secure_EL2_physical_timer_delay2();
	Non_secure_EL2_physical_timer_delay3();
	Non_secure_EL2_physical_timer_delay4();
	EL1_physical_timer_delay1();
	EL1_physical_timer_delay2();
	EL1_physical_timer_delay3();
	EL1_physical_timer_delay4();
	EL1_virtual_timer_delay1();
	EL1_virtual_timer_delay2();
	EL1_virtual_timer_delay3();
	EL1_virtual_timer_delay4();
	Non_secure_EL2_virtual_timer_delay1();
	Non_secure_EL2_virtual_timer_delay2();
	// Non_secure_EL2_virtual_timer_delay3();
	// Non_secure_EL2_virtual_timer_delay4();
	printf("Generic Timer Test Pass.\n\r");

	void cntvoff_el2_test();
	cntvoff_el2_test();
	return 0;
}


#else
#include "main_qemu.c"
#endif