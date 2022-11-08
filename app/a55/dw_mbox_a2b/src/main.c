#include "chip_define.h"
#include <stdio.h>
#include "mailbox.h"
#include "irq_ctrl.h"
#include "gic.h"
#include <stdlib.h>


static volatile uint32_t mailbox_i = 0U;
void mailbox_irqhandler(void)
{
  	uint32_t sta;
  	IRQ_Disable(INT_MAILBOX_AP_INTR);   
  	printf("%s  %d B2A_STATUS= 0x%x\n\r", __func__, __LINE__, MAILBOX_B2A->b2a_status);
	sta = MAILBOX_B2A->b2a_status;
	// MAILBOX_B2A->b2a_status |= sta;
  	for(int i = 0; i < 4; i++) {
  	  	if((sta >> i) == 1) {
  	    	a_recv_cmd(i);
  	    	a_recv_dat(i);
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

  	    	printf("%s  %d  mailbox_i = %x\n\r", __func__, __LINE__, ++mailbox_i);
  	  	}

  		IRQ_Enable(INT_MAILBOX_AP_INTR);   
  	}

	MAILBOX_LOCK ->atomic_lock[1] = 0x1;
}

static void a2b_init()
{
  	IRQ_Initialize_RTL();                     					// initial interrupt controller
	IRQ_SetHandler(INT_MAILBOX_AP_INTR, mailbox_irqhandler);  	// irq_sethandler(irq, handler)
	IRQ_SetPriority(INT_MAILBOX_AP_INTR, 0);  					// AP_IRQ first
	IRQ_Enable_RTL(INT_MAILBOX_AP_INTR);      					// open irq_enable

  	MAILBOX_A2B ->a2b_status = 0xf;
  	MAILBOX_B2A ->b2a_status = 0xf;
  	printf("%s A2B_STATUS = %d B2A_STATUS = %d ATOMIC_LOCK00 = %d\r\n", __func__,
															MAILBOX_A2B ->a2b_status,
															MAILBOX_B2A ->b2a_status,
															MAILBOX_LOCK ->atomic_lock[0]);
}

int main()
{
  	printf("%s line  %d  a2b_main_start \n\r", __func__, __LINE__);
  	a2b_init();

	for(int i = 0; i < 4; i++) {
		a_send_message(i,0x3333,0x3333);
		udelay(350);
	}

	printf("%s line %d A2B_STATUS = %x\n\r", __func__, __LINE__, MAILBOX_A2B->a2b_status);

	while(1) {
		asm volatile ("nop");
	};
}
