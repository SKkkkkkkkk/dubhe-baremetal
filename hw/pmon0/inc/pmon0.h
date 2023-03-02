#ifndef __pmon0_H__
#define __pmon0_H__

#include <stdint.h>

typedef struct {
    // 0x0c
    uint32_t sts_wbytes;
    // 0x10
    uint32_t sts_rbytes;
    // 0x14
    uint32_t _reserved0;
    // 0x18
    uint32_t _reserved1;
    // 0x1c
    uint32_t sts_wreq_num;
    // 0x20
    uint32_t sts_wlat_total_0;
    // 0x24
    uint32_t sts_wlat_total_1 : 16;  // 15 :  0
    uint32_t sts_wlat_max     : 16;  // 31 : 16
    // 0x28
    uint32_t sts_rreq_num;
    // 0x2c
    uint32_t sts_rlat_total_0;
    // 0x30
    uint32_t sts_rlat_total_1 : 16;  // 15 :  0
    uint32_t sts_rlat_max     : 16;  // 31 : 16
    // 0x34
    uint32_t sts_error;
} pmon0_result;

typedef struct {
    // 0x50
    uint32_t ddr_refresh_in_process   :  1;  //  0 :  0
    uint32_t ddr_q_almost_full        :  1;  //  1 :  1
    uint32_t _reserved0               :  7;  //  8 :  2
    uint32_t ddr_dqs_osc_in_progress  :  1;  //  9 :  9
    uint32_t ddr_ctrl_dp_idle         :  1;  // 10 : 10
    uint32_t ddr_controller_init_done :  1;  // 11 : 11
    uint32_t ddr_controller_busy      :  1;  // 12 : 12
    uint32_t ddr_cke_status           :  2;  // 14 : 13
    uint32_t ddr_dfi_zq_in_progress   :  1;  // 15 : 15
    uint32_t ddr_perf_mon_lp_cmds     : 12;  // 27 : 16
    uint32_t _reserved1               :  4;  // 31 : 28
} pmon0_ddr_status;

typedef struct {
    // 0x5c
    uint32_t sts_ddr_perf_mon_cnt;
    // 0x60
    uint32_t sts_ddr_port_cmd_full_cnt;
    // 0x64
    uint32_t sts_ddr_port_rsp_full_cnt;
    // 0x68
    uint32_t sts_ddr_port_wr_full_cnt;
    // 0x6c
    uint32_t sts_ddr_port_rd_full_cnt;
} pmon0_ddr_perf_stat;

typedef struct {
    // 0x70
    uint32_t ddr_wqos0  :  4;  //  3 :  0
    uint32_t ddr_rqos0  :  4;  //  7 :  4
    uint32_t ddr_wqos1  :  4;  // 11 :  8
    uint32_t ddr_rqos1  :  4;  // 15 : 12
    uint32_t ddr_wqos2  :  4;  // 19 : 16
    uint32_t ddr_rqos2  :  4;  // 23 : 20
    uint32_t ddr_wqos3  :  4;  // 27 : 24
    uint32_t ddr_rqos3  :  4;  // 31 : 28
    // 0x74
    uint32_t ddr_wqos4  :  4;  //  3 :  0
    uint32_t ddr_rqos4  :  4;  //  7 :  4
    uint32_t ddr_wqos5  :  4;  // 11 :  8
    uint32_t ddr_rqos5  :  4;  // 15 : 12
    uint32_t ddr_wqos6  :  4;  // 19 : 16
    uint32_t ddr_rqos6  :  4;  // 23 : 20
    uint32_t _reserved0 :  8;  // 31 : 24
    // 0x78
    uint32_t ddr_wcobuf;
    // 0x7c
    uint32_t ddr_rapcmd :  7;  //  6 :  0
    uint32_t _reserved1 :  1;  //  7 :  7
    uint32_t ddr_wapcmd :  7;  // 14 :  8
    uint32_t _reserved2 : 17;  // 31 : 15
} pmon0_ddr_cfg;


#define PMON0_RESULT        ((volatile pmon0_result*       )PMON0_RESULT_ADDR       )
#define PMON0_DDR_STATUS    ((volatile pmon0_ddr_status*   )PMON0_DDR_STATUS_ADDR   )
#define PMON0_DDR_PERF_STAT ((volatile pmon0_ddr_perf_stat*)PMON0_DDR_PERF_STAT_ADDR)
#define PMON0_DDR_CFG       ((volatile pmon0_ddr_cfg*      )PMON0_DDR_CFG_ADDR      )

typedef struct {
    uint32_t wbytes;
    uint32_t rbytes;
    uint32_t wreq_num;
    uint64_t wlat_total;
    uint16_t wlat_avg;
    uint16_t wlat_max;
    uint32_t rreq_num;
    uint64_t rlat_total;
    uint16_t rlat_avg;
    uint16_t rlat_max;
    uint8_t  error;
} pmon0_status;

void pmon0_start(uint32_t period, uint8_t chn, uint16_t axi_id, uint8_t use_id, uint8_t intr_en);
void pmon0_stop();
void pmon0_get_status(pmon0_status* status);
uint32_t pmon0_get_sts_rbytes(void);
uint32_t pmon0_get_sts_wbytes(void);
uint32_t pmon0_get_sts_wreq_num(void);
uint32_t pmon0_get_sts_rreq_num(void);
uint64_t pmon0_get_sts_wlat_total(void);
uint64_t pmon0_get_sts_rlat_total(void);
uint32_t pmon0_get_sts_wlat_max(void);
uint32_t pmon0_get_sts_rlat_max(void);
void pmon0_set_clr_intr(uint8_t value);

#endif // __pmon0_H__

