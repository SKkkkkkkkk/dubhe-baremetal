#include <stdio.h>
#include "chip_define.h"
#include "mailbox.h"
#include "m3.h"
#include "sh100_print.h"

static volatile uint32_t mailbox_i = 0U;
void Interrupt9_Handler(void)
{
	uint32_t sta;
	NVIC_DisableIRQ(INT_MAILBOX_BB_INTR);   // disable an external interrupt
	printf("%s  %d A2B_STATUS= 0x%lx\n\r", __func__, __LINE__, MAILBOX_A2B->a2b_status);
	sta = MAILBOX_A2B->a2b_status;
	for(int i = 0; i < 4; i++) {
		if((sta >> i) == 1) {
			b_recv_cmd(i);
			b_recv_dat(i);
			//MAILBOX_A2B -> a2b_status = 0xf; 
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

			printf("%s line %d  mailbox_i = %lx\n\r", __func__, __LINE__, ++mailbox_i);
		}

		NVIC_EnableIRQ(INT_MAILBOX_BB_INTR);    // enable an external interrupt
	}

	MAILBOX_LOCK ->atomic_lock[0] = 0x1;
}

void b2a_init()
{
	MAILBOX_A2B -> a2b_status = 0x0f;
	MAILBOX_B2A -> b2a_status = 0x0f;
	printf("%s line %d A2B_STATUS = %lx B2A_STATUS = %lx ATOMIC_LOCK01 = %lx\n\r", __func__, __LINE__,
	                                                        				MAILBOX_A2B -> a2b_status,
	                                                        				MAILBOX_B2A -> b2a_status,
																			MAILBOX_LOCK ->atomic_lock[1]);	
	NVIC_SetPriority(INT_MAILBOX_BB_INTR, 0);
	NVIC_SetVector(INT_MAILBOX_BB_INTR, (uint32_t)(uintptr_t)Interrupt9_Handler);
	NVIC_EnableIRQ(INT_MAILBOX_BB_INTR);    // enable an external interrupt
}

int main()
{
	printf("%s line  %d  b2a_main_start   \n\r", __func__, __LINE__);	
	b2a_init();

	for(int i = 0; i < 4; i++) {
		b_send_message(i,0x5555,0x5555);
		udelay(350);
	}

	printf("%s line %d B2A_STATUS = %lx\n\r", __func__, __LINE__, MAILBOX_B2A->b2a_status);

	while(1){
		asm volatile ("nop");
	};
}
