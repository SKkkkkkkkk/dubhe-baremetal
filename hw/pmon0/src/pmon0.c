#include "pmon0_regs.h"
#include "pmon0.h"

void pmon0_start(uint32_t period, uint8_t chn, uint16_t axi_id, uint8_t use_id, uint8_t intr_en)
{
    /*assert(period >= 1000);*/
    /*assert(chn < 6);*/

    PMON0_SET_SOFT_RST_N(0);

    PMON0_SET_CFG_PERIOD(period);
    PMON0_SET_CFG_CHN_SEL(chn);
    PMON0_SET_CFG_AXI_ID(axi_id);
    PMON0_SET_CFG_USE_ID(use_id != 0);
    PMON0_SET_INTR_EN(intr_en != 0);

    PMON0_SET_SOFT_RST_N(1);
}

void pmon0_stop()
{
    PMON0_SET_SOFT_RST_N(0);
}

void pmon0_get_status(pmon0_status* status)
{
    /*assert(status);*/

    status->wbytes      = PMON0_GET_STS_WBYTES;
    status->rbytes      = PMON0_GET_STS_RBYTES;
    status->wreq_num    = PMON0_GET_STS_WREQ_NUM;
    status->wlat_total  = ((uint64_t)PMON0_GET_STS_WLAT_TOTAL_1 << 32)
                        | PMON0_GET_STS_WLAT_TOTAL_0;
    status->wlat_max    = PMON0_GET_STS_WLAT_MAX;
    status->rreq_num    = PMON0_GET_STS_RREQ_NUM;
    status->rlat_total  = ((uint64_t)PMON0_GET_STS_RLAT_TOTAL_1 << 32)
                        | PMON0_GET_STS_RLAT_TOTAL_0;
    status->rlat_max    = PMON0_GET_STS_RLAT_MAX;
    status->error       = PMON0_GET_STS_ERROR;

    status->wlat_avg    = (status->wreq_num==0)? 0 : (uint16_t)(status->wlat_total/status->wreq_num);
    status->rlat_avg    = (status->rreq_num==0)? 0 : (uint16_t)(status->rlat_total/status->rreq_num);
}
uint32_t pmon0_get_sts_rbytes(void)
{
	return PMON0_GET_STS_RBYTES;

}
uint32_t pmon0_get_sts_wbytes(void)
{
	return PMON0_GET_STS_WBYTES;
}

uint32_t pmon0_get_sts_wreq_num(void)
{
	return PMON0_GET_STS_WREQ_NUM;
}

uint32_t pmon0_get_sts_rreq_num(void)
{
	return PMON0_GET_STS_RREQ_NUM;
}

uint64_t pmon0_get_sts_wlat_total(void)
{
	return ((uint64_t)PMON0_GET_STS_WLAT_TOTAL_1 << 32)
                        | PMON0_GET_STS_WLAT_TOTAL_0;
}

uint64_t pmon0_get_sts_rlat_total(void)
{
	return ((uint64_t)PMON0_GET_STS_RLAT_TOTAL_1 << 32)
                        | PMON0_GET_STS_RLAT_TOTAL_0;
}

uint32_t pmon0_get_sts_wlat_max(void)
{
	return PMON0_GET_STS_WLAT_MAX;
}

uint32_t pmon0_get_sts_rlat_max(void)
{
	return PMON0_GET_STS_RLAT_MAX;
}

void pmon0_set_clr_intr(uint8_t value)
{
	PMON0_SET_CLR_INTR(value);
}

