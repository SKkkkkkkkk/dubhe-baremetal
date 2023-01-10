#include <stdio.h>
#include <assert.h>
#include "m3.h"
#include "mailbox.h"

#define mailbox_irq_handler Interrupt9_Handler
#define ILLEGAL_M3_BL2_BASE 0xFFFFFFFFU

static volatile uintptr_t m3_bl2_base = ILLEGAL_M3_BL2_BASE;
static volatile bool is_m3_bl2_ready = false;

static inline bool is_m3_bl2_addr_legal(uintptr_t addr)
{
	if( ((addr>=0x00100000)&&(addr<0x00200000)) /* AXI SRAM */ || \
		((addr>=0x20000000)&&(addr<0x20100000)) /* SYSRAM */ || \
		((addr>=0x60000000)&&(addr<0xC0000000)) /* DDR */ )
		return	true;
	return false;
}

static inline void m3_bl1_to_m3_bl2_irq_handler(void)
{
	assert(!is_m3_bl2_ready);
	assert(m3_bl2_base==ILLEGAL_M3_BL2_BASE);

	if(b_get_cmd(0) == 0xa5a5a5a5) // key match
	{
		uintptr_t _m3_bl2_base = b_get_data(0);
		if(is_m3_bl2_addr_legal(_m3_bl2_base))
		{
			m3_bl2_base = _m3_bl2_base;
			is_m3_bl2_ready = true;
		}
		else
		{
			printf("M3_BL1: M3_BL2_BASE is illegal!\n\r");
		}
	}
	else
	{
		// do nothing.
	}
}

void mailbox_irq_handler(void)
{
	uint32_t status;
	status = MAILBOX_A2B->a2b_status;
	if(status & 1) // ch0
	{
		MAILBOX_A2B->a2b_status = 0x1; // clear irq
		m3_bl1_to_m3_bl2_irq_handler();
	}
	else
	{
		printf("M3_BL1: Recevied a spurious mailbox sign.\n\r");
	}
}

static inline void b2a_init()
{
	MAILBOX_B2A->b2a_status = 0x0f; // Clear the interrupt by writing 1 to this bit.

	NVIC_SetPriority(Mailbox_IRQn, 0);
	NVIC_SetVector(Mailbox_IRQn, (uint32_t)mailbox_irq_handler);
	NVIC_EnableIRQ(Mailbox_IRQn);
}

static inline void setpc(uintptr_t pc)
{
	assert((pc&1) == 1); // thumb
	__DMB();
	__DSB();
	__ISB();
	__asm__ volatile ("bx %0"::"r"(pc):"memory");
}

int main()
{
	printf("M3_BL1: "BUILD_TIMESTAMP"\n\r");
	printf("M3_BL1: Waiting for M3_BL2 to be ready...\n\r");
	b2a_init();

	while(!is_m3_bl2_ready) 
		__asm__ volatile("wfi");

	uintptr_t pc = *(uintptr_t*)(m3_bl2_base + 4);
	assert(is_m3_bl2_addr_legal(pc));
	printf("M3_BL1: Recevie mailbox sign, ready to go.\n\r");
	printf("M3_BL1: M3_BL2 entrypoint: 0x%x.\n\r", pc-1);
	setpc(pc);
	return 0;
}