#include <stdio.h>
#include <assert.h>
#include "m3.h"
#include "mailbox.h"

#define mailbox_irq_handler Interrupt9_Handler

static volatile bool is_m3_bl2_ready = false;
void mailbox_irq_handler(void)
{
	uint32_t status;
	status = MAILBOX_A2B->a2b_status;
	if(status & 1) // ch0
	{
		MAILBOX_A2B->a2b_status = 0x1; // clear irq
		if((b_get_cmd(0) == 0xa5a5a5a5) && (b_get_data(0) == 0x5a5a5a5a)) // compare cmd & data
		{
			assert(!is_m3_bl2_ready);
			is_m3_bl2_ready = true;
		}
	}
	else
	{
		printf("M3_BL1: Recevied a spurious mailbox sign.\n\r");
	}
}

static void b2a_init()
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

	while(!is_m3_bl2_ready) __asm__ volatile("");

	uintptr_t pc = *(uintptr_t*)(0x60000000 + 4);
	printf("M3_BL1: Recevie mailbox sign, ready to go.\n\r");
	printf("M3_BL1: M3_BL2 entrypoint: 0x%x.\n\r", pc-1);
	setpc(pc);
	return 0;
}