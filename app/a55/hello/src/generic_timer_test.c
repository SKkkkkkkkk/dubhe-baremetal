#include <stdio.h>
#include <stdbool.h>
#include "gic.h"
#include "irq_ctrl.h"
#include "arch_features.h"


DEFINE_SYSREG_RW_FUNCS(cntvct_el0)

DEFINE_SYSREG_RW_FUNCS(cntv_cval_el0)
DEFINE_SYSREG_RW_FUNCS(cntv_tval_el0)
DEFINE_SYSREG_RW_FUNCS(cntv_ctl_el0)

DEFINE_SYSREG_RW_FUNCS(cnthv_cval_el2)
DEFINE_SYSREG_RW_FUNCS(cnthv_tval_el2)
DEFINE_SYSREG_RW_FUNCS(cnthv_ctl_el2)

#define SYSTEM_COUNTER_FRQ (20000000/10)

void System_Counter_test(void)
{

}


/*******EL3 physical timer*********/

/* Poll & CompareValue views */
void EL3_physical_timer_delay1(void)
{
	printf("\tEL3_physical_timer_delay1 Test:\n\r");
	uint64_t i = 0;
	uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntps_ctl_el1(3);
	while(1)
	{
		if(i==5)
		{
			write_cntps_ctl_el1(0);
			break;
		}
		printf("\t%lu\n\r", i++);
		cntpct_el0 = read_cntpct_el0();
		write_cntps_cval_el1(delta + cntpct_el0);
		while( !(read_cntps_ctl_el1()&4) );
	}
	printf("\tEL3_physical_timer_delay1 Test Pass.\n\r");
}

/* Poll & TimerValue views */
void EL3_physical_timer_delay2(void)
{
	printf("\tEL3_physical_timer_delay2 Test:\n\r");
	uint64_t i = 0;
	// uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntps_ctl_el1(3);
	while(1)
	{
		if(i==5)
		{
			write_cntps_ctl_el1(0);
			break;
		}
		printf("\t%lu\n\r", i++);
		// cntpct_el0 = read_cntpct_el0();
		// write_cntps_cval_el1(delta + cntpct_el0);
		write_cntps_tval_el1(delta);
		while( !(read_cntps_ctl_el1()&4) );
	}
	printf("\tEL3_physical_timer_delay2 Test Pass.\n\r");
}

static volatile bool el3_physical_timer_flag = false;
void el3_physical_timer_irq_handler(void)
{
	el3_physical_timer_flag =true;
	write_cntps_ctl_el1(0); // clear irq.
}

/* Interrput & CompareValue views */
void EL3_physical_timer_delay3(void)
{
	printf("\tEL3_physical_timer_delay3 Test:\n\r");
	uint64_t i = 0;
	uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntps_ctl_el1(0);
	IRQ_SetHandler(Secure_physical_timer_IRQn, el3_physical_timer_irq_handler); //设置中断处理函数
	IRQ_SetPriority(Secure_physical_timer_IRQn, 0 << 3); //设置优先级
	IRQ_Enable(Secure_physical_timer_IRQn); //使能该中断
	while(1)
	{
		if(i==5)
			break;
		printf("\t%lu\n\r", i++);
		cntpct_el0 = read_cntpct_el0();
		write_cntps_cval_el1(delta + cntpct_el0);
		write_cntps_ctl_el1(1);
		while(!el3_physical_timer_flag);
		el3_physical_timer_flag = false;
	}
	printf("\tEL3_physical_timer_delay3 Test Pass.\n\r");
}

/* Interrput & TimerValue views */
void EL3_physical_timer_delay4(void)
{
	printf("\tEL3_physical_timer_delay4 Test:\n\r");
	uint64_t i = 0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntps_ctl_el1(0);
	IRQ_SetHandler(Secure_physical_timer_IRQn, el3_physical_timer_irq_handler); //设置中断处理函数
	IRQ_SetPriority(Secure_physical_timer_IRQn, 0 << 3); //设置优先级
	IRQ_Enable(Secure_physical_timer_IRQn); //使能该中断
	while(1)
	{
		if(i==5)
			break;
		printf("\t%lu\n\r", i++);
		write_cntps_tval_el1(delta);
		write_cntps_ctl_el1(1);
		while(!el3_physical_timer_flag);
		el3_physical_timer_flag = false;
	}
	printf("\tEL3_physical_timer_delay4 Test Pass.\n\r");
}

/*******Non-secure EL2 physical timer*********/

/* Poll & CompareValue views */
void Non_secure_EL2_physical_timer_delay1(void)
{
	printf("\tNon_secure_EL2_physical_timer_delay1 Test:\n\r");
	uint64_t i = 0;
	uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cnthp_ctl_el2(3);
	while(1)
	{
		if(i==5)
		{
			write_cnthp_ctl_el2(0);
			break;
		}
		printf("\t%lu\n\r", i++);
		cntpct_el0 = read_cntpct_el0();
		write_cnthp_cval_el2(delta + cntpct_el0);
		while( !(read_cnthp_ctl_el2()&4) );
	}
	printf("\tNon_secure_EL2_physical_timer_delay1 Test Pass.\n\r");
}

/* Poll & TimerValue views */
void Non_secure_EL2_physical_timer_delay2(void)
{
	printf("\tNon_secure_EL2_physical_timer_delay2 Test:\n\r");
	uint64_t i = 0;
	// uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cnthp_ctl_el2(3);
	while(1)
	{
		if(i==5)
		{
			write_cnthp_ctl_el2(0);
			break;
		}
		printf("\t%lu\n\r", i++);
		// cntpct_el0 = read_cntpct_el0();
		// write_cnthp_cval_el2(delta + cntpct_el0);
		write_cnthp_tval_el2(delta);
		while( !(read_cnthp_ctl_el2()&4) );
	}
	printf("\tNon_secure_EL2_physical_timer_delay2 Test Pass.\n\r");
}

static volatile bool Non_secure_EL2_physical_timer_flag = false;
void Non_secure_EL2_physical_timer_irq_handler(void)
{
	Non_secure_EL2_physical_timer_flag =true;
	write_cnthp_ctl_el2(0); // clear irq.
}

/* Interrput & CompareValue views */
void Non_secure_EL2_physical_timer_delay3(void)
{
	printf("\tNon_secure_EL2_physical_timer_delay3 Test:\n\r");
	uint64_t i = 0;
	uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cnthp_ctl_el2(0);
	IRQ_SetHandler(Hypervisor_timer_IRQn, Non_secure_EL2_physical_timer_irq_handler); //设置中断处理函数
	IRQ_SetPriority(Hypervisor_timer_IRQn, 0 << 3); //设置优先级
	IRQ_Enable(Hypervisor_timer_IRQn); //使能该中断
	while(1)
	{
		if(i==5)
			break;
		printf("\t%lu\n\r", i++);
		cntpct_el0 = read_cntpct_el0();
		write_cnthp_cval_el2(delta + cntpct_el0);
		write_cnthp_ctl_el2(1);
		while(!Non_secure_EL2_physical_timer_flag);
		Non_secure_EL2_physical_timer_flag = false;
	}
	printf("\tNon_secure_EL2_physical_timer_delay3 Test Pass.\n\r");
}

/* Interrput & TimerValue views */
void Non_secure_EL2_physical_timer_delay4(void)
{
	printf("\tNon_secure_EL2_physical_timer_delay4 Test:\n\r");
	uint64_t i = 0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cnthp_ctl_el2(0);
	IRQ_SetHandler(Hypervisor_timer_IRQn, Non_secure_EL2_physical_timer_irq_handler); //设置中断处理函数
	IRQ_SetPriority(Hypervisor_timer_IRQn, 0 << 3); //设置优先级
	IRQ_Enable(Hypervisor_timer_IRQn); //使能该中断
	while(1)
	{
		if(i==5)
			break;
		printf("\t%lu\n\r", i++);
		write_cnthp_tval_el2(delta);
		write_cnthp_ctl_el2(1);
		while(!Non_secure_EL2_physical_timer_flag);
		Non_secure_EL2_physical_timer_flag = false;
	}
	printf("\tNon_secure_EL2_physical_timer_delay4 Test Pass.\n\r");
}


/*******EL1 physical timer*********/

/* Poll & CompareValue views */
void EL1_physical_timer_delay1(void)
{
	printf("\tEL1_physical_timer_delay1 Test:\n\r");
	uint64_t i = 0;
	uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntp_ctl_el0(3);
	while(1)
	{
		if(i==5)
		{
			write_cntp_ctl_el0(0);
			break;
		}
		printf("\t%lu\n\r", i++);
		cntpct_el0 = read_cntpct_el0();
		write_cntp_cval_el0(delta + cntpct_el0);
		while( !(read_cntp_ctl_el0()&4) );
	}
	printf("\tEL1_physical_timer_delay1 Test Pass.\n\r");
}

/* Poll & TimerValue views */
void EL1_physical_timer_delay2(void)
{
	printf("\tEL1_physical_timer_delay2 Test:\n\r");
	uint64_t i = 0;
	// uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntp_ctl_el0(3);
	while(1)
	{
		if(i==5)
		{
			write_cntp_ctl_el0(0);
			break;
		}
		printf("\t%lu\n\r", i++);
		// cntpct_el0 = read_cntpct_el0();
		// write_cntp_cval_el0(delta + cntpct_el0);
		write_cntp_tval_el0(delta);
		while( !(read_cntp_ctl_el0()&4) );
	}
	printf("\tEL1_physical_timer_delay2 Test Pass.\n\r");
}

static volatile bool EL1_physical_timer_flag = false;
void EL1_physical_timer_irq_handler(void)
{
	EL1_physical_timer_flag =true;
	write_cntp_ctl_el0(0); // clear irq.
}

/* Interrput & CompareValue views */
void EL1_physical_timer_delay3(void)
{
	printf("\tEL1_physical_timer_delay3 Test:\n\r");
	uint64_t i = 0;
	uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntp_ctl_el0(0);
	IRQ_SetHandler(Non_secure_physical_timer_IRQn, EL1_physical_timer_irq_handler); //设置中断处理函数
	IRQ_SetPriority(Non_secure_physical_timer_IRQn, 0 << 3); //设置优先级
	IRQ_Enable(Non_secure_physical_timer_IRQn); //使能该中断
	while(1)
	{
		if(i==5)
			break;
		printf("\t%lu\n\r", i++);
		cntpct_el0 = read_cntpct_el0();
		write_cntp_cval_el0(delta + cntpct_el0);
		write_cntp_ctl_el0(1);
		while(!EL1_physical_timer_flag);
		EL1_physical_timer_flag = false;
	}
	printf("\tEL1_physical_timer_delay3 Test Pass.\n\r");
}

/* Interrput & TimerValue views */
void EL1_physical_timer_delay4(void)
{
	printf("\tEL1_physical_timer_delay4 Test:\n\r");
	uint64_t i = 0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntp_ctl_el0(0);
	IRQ_SetHandler(Non_secure_physical_timer_IRQn, EL1_physical_timer_irq_handler); //设置中断处理函数
	IRQ_SetPriority(Non_secure_physical_timer_IRQn, 0 << 3); //设置优先级
	IRQ_Enable(Non_secure_physical_timer_IRQn); //使能该中断
	while(1)
	{
		if(i==5)
			break;
		printf("\t%lu\n\r", i++);
		write_cntp_tval_el0(delta);
		write_cntp_ctl_el0(1);
		while(!EL1_physical_timer_flag);
		EL1_physical_timer_flag = false;
	}
	printf("\tEL1_physical_timer_delay4 Test Pass.\n\r");
}



/*******EL1 virtual timer*******/

/* Poll & CompareValue views */
void EL1_virtual_timer_delay1(void)
{
	printf("\tEL1_virtual_timer_delay1 Test:\n\r");
	write_cntvoff_el2(1000000/2);
	isb();
	uint64_t i = 0;
	uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntv_ctl_el0(3);
	while(1)
	{
		if(i==5)
		{
			write_cntv_ctl_el0(0);
			break;
		}
		printf("\t%lu\n\r", i++);
		cntpct_el0 = read_cntvct_el0();
		write_cntv_cval_el0(delta + cntpct_el0);
		while( !(read_cntv_ctl_el0()&4) );
	}
	printf("\tEL1_virtual_timer_delay1 Test Pass.\n\r");
}

/* Poll & TimerValue views */
void EL1_virtual_timer_delay2(void)
{
	printf("\tEL1_virtual_timer_delay2 Test:\n\r");
	uint64_t i = 0;
	// uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntv_ctl_el0(3);
	while(1)
	{
		if(i==5)
		{
			write_cntv_ctl_el0(0);
			break;
		}
		printf("\t%lu\n\r", i++);
		// cntpct_el0 = read_cntvct_el0();
		// write_cntv_cval_el0(delta + cntpct_el0);
		write_cntv_tval_el0(delta);
		while( !(read_cntv_ctl_el0()&4) );
	}
	printf("\tEL1_virtual_timer_delay2 Test Pass.\n\r");
}

static volatile bool EL1_virtual_timer_flag = false;
void EL1_virtual_timer_irq_handler(void)
{
	EL1_virtual_timer_flag =true;
	write_cntv_ctl_el0(0); // clear irq.
}

/* Interrput & CompareValue views */
void EL1_virtual_timer_delay3(void)
{
	printf("\tEL1_virtual_timer_delay3 Test:\n\r");
	uint64_t i = 0;
	uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntv_ctl_el0(0);
	IRQ_SetHandler(Virtual_timer_IRQn, EL1_virtual_timer_irq_handler); //设置中断处理函数
	IRQ_SetPriority(Virtual_timer_IRQn, 0 << 3); //设置优先级
	IRQ_Enable(Virtual_timer_IRQn); //使能该中断
	while(1)
	{
		if(i==5)
			break;
		printf("\t%lu\n\r", i++);
		cntpct_el0 = read_cntvct_el0();
		write_cntv_cval_el0(delta + cntpct_el0);
		write_cntv_ctl_el0(1);
		while(!EL1_virtual_timer_flag);
		EL1_virtual_timer_flag = false;
	}
	printf("\tEL1_virtual_timer_delay3 Test Pass.\n\r");
}

/* Interrput & TimerValue views */
void EL1_virtual_timer_delay4(void)
{
	printf("\tEL1_virtual_timer_delay4 Test:\n\r");
	uint64_t i = 0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cntv_ctl_el0(0);
	IRQ_SetHandler(Virtual_timer_IRQn, EL1_virtual_timer_irq_handler); //设置中断处理函数
	IRQ_SetPriority(Virtual_timer_IRQn, 0 << 3); //设置优先级
	IRQ_Enable(Virtual_timer_IRQn); //使能该中断
	while(1)
	{
		if(i==5)
			break;
		printf("\t%lu\n\r", i++);
		write_cntv_tval_el0(delta);
		write_cntv_ctl_el0(1);
		while(!EL1_virtual_timer_flag);
		EL1_virtual_timer_flag = false;
	}
	printf("\tEL1_virtual_timer_delay4 Test Pass.\n\r");
}


/*******Non-secure EL2 virtual timer*******/

/* Poll & CompareValue views */
void Non_secure_EL2_virtual_timer_delay1(void)
{
	printf("\tNon_secure_EL2_virtual_timer_delay1 Test:\n\r");
	// write_cntvoff_el2(SYSTEM_COUNTER_FRQ);
	uint64_t i = 0;
	uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cnthv_ctl_el2(3);
	while(1)
	{
		if(i==5)
		{
			write_cnthv_ctl_el2(0);
			break;
		}
		printf("\t%lu\n\r", i++);
		cntpct_el0 = read_cntvct_el0();
		write_cnthv_cval_el2(delta + cntpct_el0);
		while( !(read_cnthv_ctl_el2()&4) );
	}
	printf("\tNon_secure_EL2_virtual_timer_delay1 Test Pass.\n\r");
}

/* Poll & TimerValue views */
void Non_secure_EL2_virtual_timer_delay2(void)
{
	printf("\tNon_secure_EL2_virtual_timer_delay2 Test:\n\r");
	uint64_t i = 0;
	// uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cnthv_ctl_el2(3);
	while(1)
	{
		if(i==5)
		{
			write_cnthv_ctl_el2(0);
			break;
		}
		printf("\t%lu\n\r", i++);
		// cntpct_el0 = read_cntvct_el0();
		// write_cnthv_cval_el2(delta + cntpct_el0);
		write_cnthv_tval_el2(delta);
		while( !(read_cnthv_ctl_el2()&4) );
	}
	printf("\tNon_secure_EL2_virtual_timer_delay2 Test Pass.\n\r");
}

static volatile bool Non_secure_EL2_virtual_timer_flag = false;
void Non_secure_EL2_virtual_timer_irq_handler(void)
{
	Non_secure_EL2_virtual_timer_flag =true;
	write_cnthv_ctl_el2(0); // clear irq.
}

/* Interrput & CompareValue views */
void Non_secure_EL2_virtual_timer_delay3(void)
{
	printf("\tNon_secure_EL2_virtual_timer_delay3 Test:\n\r");
	uint64_t i = 0;
	uint64_t cntpct_el0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cnthv_ctl_el2(0);
	IRQ_SetHandler(Virtual_timer_IRQn, Non_secure_EL2_virtual_timer_irq_handler); //设置中断处理函数
	IRQ_SetPriority(Virtual_timer_IRQn, 0 << 3); //设置优先级
	IRQ_Enable(Virtual_timer_IRQn); //使能该中断
	while(1)
	{
		if(i==5)
			break;
		printf("\t%lu\n\r", i++);
		cntpct_el0 = read_cntvct_el0();
		write_cnthv_cval_el2(delta + cntpct_el0);
		write_cnthv_ctl_el2(1);
		while(!Non_secure_EL2_virtual_timer_flag);
		Non_secure_EL2_virtual_timer_flag = false;
	}
	printf("\tNon_secure_EL2_virtual_timer_delay3 Test Pass.\n\r");
}

/* Interrput & TimerValue views */
void Non_secure_EL2_virtual_timer_delay4(void)
{
	printf("\tNon_secure_EL2_virtual_timer_delay4 Test:\n\r");
	uint64_t i = 0;
	uint64_t delta = SYSTEM_COUNTER_FRQ;
	write_cnthv_ctl_el2(0);
	IRQ_SetHandler(Virtual_timer_IRQn, Non_secure_EL2_virtual_timer_irq_handler); //设置中断处理函数
	IRQ_SetPriority(Virtual_timer_IRQn, 0 << 3); //设置优先级
	IRQ_Enable(Virtual_timer_IRQn); //使能该中断
	while(1)
	{
		if(i==5)
			break;
		printf("\t%lu\n\r", i++);
		write_cnthv_tval_el2(delta);
		write_cnthv_ctl_el2(1);
		while(!Non_secure_EL2_virtual_timer_flag);
		Non_secure_EL2_virtual_timer_flag = false;
	}
	printf("\tNon_secure_EL2_virtual_timer_delay4 Test Pass.\n\r");
}

/* 测试 cntvoff_el2 是否起作用 */
void cntvoff_el2_test()
{
	write_cntvoff_el2(100000);
	isb();
	volatile uint64_t p = read_cntpct_el0();
	volatile uint64_t v = read_cntvct_el0();
	(void)p;
	(void)v;
	while(1);
}