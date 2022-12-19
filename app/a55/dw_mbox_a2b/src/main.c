#include "chip_mem_layout.h"
#include <stdio.h>
#include "mailbox.h"
#include "irq_ctrl.h"
#include "gic.h"
#include <stdlib.h>


static inline void udelay(uint64_t x)
{
	for(uint32_t i = x; i!=0; i--)
		asm volatile("");
}


static volatile uint32_t mailbox_i = 0U;
void mailbox_irqhandler(void)
{
  	uint32_t sta;
	sta = MAILBOX_B2A->b2a_status;
  	for(int i = 0; i < 4; i++) {
  	  	if(((sta >> i)&1) == 1) {
			printf("A55 receive: cmd: %u  data: %u by ch%u\n\r", a_get_cmd(i), a_get_data(i), i);
			switch (i) {
			case 0:
				MAILBOX_B2A->b2a_status = 0x01;
				break;
			case 1:
				MAILBOX_B2A->b2a_status = 0x02;
				break;
			case 2:
				MAILBOX_B2A->b2a_status = 0x04;
				break;
			case 3:
				MAILBOX_B2A->b2a_status = 0x08;
				break;
			default:
				break;
			}
  	  	}
  	}
}

static void a2b_init()
{
	MAILBOX_A2B->a2b_status = 0xf; // Clear the interrupt by writing 1 to this bit.
	GIC_SetTarget(Mailbox_IRQn, 1 << 0);
	IRQ_SetHandler(Mailbox_IRQn, mailbox_irqhandler);
	IRQ_SetPriority(Mailbox_IRQn, 0 << 3);
	IRQ_Enable(Mailbox_IRQn);
}


uint32_t GIC_GetIRQStatus1(IRQn_Type IRQn)
{
	return GIC_GetIRQStatus(IRQn);
}


int main()
{
	IRQ_Initialize();
  	a2b_init();

	GIC_GetIRQStatus1(Mailbox_IRQn);

	for(uint32_t i = 0; i < 4; i++) {
		a2b_send(i, i, i);
		printf("A55 send cmd: %u, data: %u by ch%u\n\r", i, i, i);
	}


	while(1) {
		asm volatile ("nop");
	};
}
