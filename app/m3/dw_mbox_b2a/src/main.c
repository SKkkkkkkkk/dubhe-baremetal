#include <stdio.h>
#include "chip_mem_layout.h"
#include "mailbox.h"
#include "m3.h"

static inline void udelay(uint64_t x)
{
	for(uint32_t i = x; i!=0; i--)
		asm volatile("");
}


static volatile uint32_t mailbox_i = 0U;
void Interrupt9_Handler(void)
{
	uint32_t sta;
	sta = MAILBOX_A2B->a2b_status;
	for(int i = 0; i < 4; i++) {
		if(((sta >> i)&1) == 1) {
			printf("CM3 receive: cmd: %lu  data: %lu by ch%u\n\r", b_get_cmd(i), b_get_data(i), i);
			switch (i) {
				case 0:
					MAILBOX_A2B -> a2b_status = 0x1;
					break;
				case 1:
					MAILBOX_A2B -> a2b_status = 0x2;
					break;
				case 2:
					MAILBOX_A2B -> a2b_status = 0x4;
					break;
				case 3:
					MAILBOX_A2B -> a2b_status = 0x8;
					break;
				default:
					break;
			}
		}
	}
}

void b2a_init()
{
	MAILBOX_B2A->b2a_status = 0x0f; // Clear the interrupt by writing 1 to this bit.
	NVIC_SetPriority(Mailbox_IRQn, 0);
	NVIC_SetVector(Mailbox_IRQn, (uint32_t)(uintptr_t)Interrupt9_Handler);
	NVIC_EnableIRQ(Mailbox_IRQn);
}

int main()
{
	b2a_init();

	for(int i = 0; i < 4; i++) {
		b2a_send(i, 3-i, 3-i);
		printf("CM3 send cmd: %u, data: %u by ch%u\n\r", 3-i, 3-i, i);
	}

	while(1){
		asm volatile ("nop");
	};
}
