#ifndef QEMU
#include "dw_apb_uart.h"
#include "seehi_print.h"
#include "gic.h"
#include "irq_ctrl.h"
#include "dw_apb_timers.h"
#include "systimer.h"
#include "xlat_tables_v2.h"

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
	seehi_printf("%d in irq\n",i++);
	(void)TIMERX2->Timer2EOI;
}

#define __ROM_BASE 0x00000000
#define __ROM_SIZE 0x00010000 //64KB

#define __RAM_BASE 0x00A00000
#define __RAM_SIZE 0x00020000 //128KB

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

int main()
{
	const mmap_region_t mmap_region_list[] = {
		MAP_ROM,
		MAP_RAM,
		// MAP_DEVICE0,
		// MAP_DEVICE1,
		{0}
	};
	mmap_add(mmap_region_list);
	init_xlat_tables();
	enable_mmu_el3(0);

	GIC_Enable();
	systimer_init();
	seehi_uart_config_baudrate(SEEHI_UART_BAUDRATE_115200, 20000000, SEEHI_UART1);
	seehi_printf("hello world\n");
	// timer_init_config_t timer_init_config = {
	// 	.int_mask = false, .loadcount = 20000000, .timer_id = Timerx2_T2, .timer_mode = Mode_User_Defined
	// };
	// timer_init(&timer_init_config);
	// IRQ_SetHandler(Timer1_2_IRQn, test_irq_handler); //设置中断处理函数
	// IRQ_SetPriority(Timer1_2_IRQn, 0 << 3); //设置优先级
	// IRQ_Enable(Timer1_2_IRQn); //使能该中断
	// timer_enable(timer_init_config.timer_id);

	seehi_printf("cntfrq: %d\n",arch_timer_get_cntfrq_el0());
	arch_timer_set_cntfrq_el0(1875000);
	seehi_printf("cntfrq: %d\n",arch_timer_get_cntfrq_el0());

	while(1)
	{
		static int i = 0;
		seehi_printf("%ds\n",i++);
		seehi_printf("0x%x\n", (int)arch_timer_get_cntpct_el0());
		systimer_delay(1, IN_S);
	}
	return 0;
}



#else
#include "main_qemu.c"
#endif