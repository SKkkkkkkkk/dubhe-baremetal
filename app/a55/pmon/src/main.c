#include <stdio.h>
#include "pmon0.h"
#include "gic.h"
#include "irq_ctrl.h"


uint32_t w_bytes = 0;
uint32_t r_bytes = 0;
uint32_t wreq_num = 0;
uint32_t rreq_num = 0;
uint64_t wlat_total = 0;
uint64_t rlat_total = 0;
uint32_t wlat_max = 0;
uint32_t rlat_max = 0;


void pmon0_irqhandler(void)
{
/* 	printf("interrupt ....\n\r"); */

	IRQ_Disable(PMON0_IRQn);

	w_bytes = pmon0_get_sts_wbytes();
	r_bytes = pmon0_get_sts_rbytes();

	wreq_num = pmon0_get_sts_wreq_num();
	rreq_num = pmon0_get_sts_rreq_num();

	wlat_total = pmon0_get_sts_wlat_total();
	rlat_total = pmon0_get_sts_rlat_total();

	wlat_max = pmon0_get_sts_wlat_max();
	rlat_max = pmon0_get_sts_rlat_max();

	pmon0_set_clr_intr(0);
	IRQ_Enable(PMON0_IRQn);
}


int main(void)
{
	printf("test %s ...\n", __FILE__);

	
	GIC_DistInit();
  	GIC_CPUInterfaceInit(); //per CPU

	void pmon0_irqhandler(void);
	GIC_SetTarget(PMON0_IRQn, 1 << 0);
	IRQ_SetHandler(PMON0_IRQn, pmon0_irqhandler);
	IRQ_SetPriority(PMON0_IRQn, 0 << 3);
	IRQ_Enable(PMON0_IRQn);
	


	pmon0_start(24000000 , 	// period  24M
			   0,			// chn
			   0,			// axi_id
			   0,			// use_id
			   1			// intr_en
	);

	while (1)
	{
		printf("w_bytes=%d\n\rr_bytes=%d\n\r", w_bytes,r_bytes);
		printf("wreq_num=%d\n\rrreq_num=%d\n\r", wreq_num,rreq_num);
		printf("wlat_total=%ld\n\rrlat_total=%ld\n\r", wlat_total,rlat_total);
		printf("wlat_max=%d\n\rrlat_maxs=%d\n\r", wlat_max,rlat_max);
	}	
	return 0;
}