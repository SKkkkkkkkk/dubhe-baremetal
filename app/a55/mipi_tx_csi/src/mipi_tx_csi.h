#ifndef __IMGOUT_CSI_REGS_H__
#define __IMGOUT_CSI_REGS_H__

////////////////////////////////////////////////////////////////
// baisc reg info                                             //
////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "systimer.h"

#define BYPASS_ISP                                      1
#define ONE_SENSOR                                      1
#define TWO_SENSOR                                      1
#define SPE_SENSOR                                      0
#define SENSOR_TO_DDR                                   1
#define SKIP_FRAME_NUM                                  100

// #define DEBUG                                           1
#define DRV_VERSION 									"1.0.0"
#define DRV_NAME    									"imgout_csi"
#define IMGOUT_CSI_NAME    								"imgout_csi: "
#define IMGOUT_CSI_BASE                                 (seehi_imgout_csi_base_va)
#define IMGOUT_CSI_BUFFER_1M                            (1024*1024)
#define IMGOUT_CSI_BUFFER_SIZE    						(4*IMGOUT_CSI_BUFFER_1M)

enum bufer_select {
	SRC0_BUFFER = 0,
	SRC1_BUFFER = 1,
	DST0_BUFFER = 2,
	DST1_BUFFER = 3,
	BCK0_BUFFER = 4,
	BCK1_BUFFER = 5,
	BCK2_BUFFER = 6,
	BCK3_BUFFER = 7,
};

enum work_state {
	DISABLE_WORKQUEUE = 1,
	ENABLE_WORKQUEUE = 2,
};

enum event_state {
	POLLIN_STATE = 1,
	POLLOUT_STATE = 2,
};

enum {
	REGS_BASE_PHY = 0,
	REGS_BASE_IDMA = 1,
	REGS_BASE_CTRL = 2,
	ERG_BASE_END = 3,
};

struct sensor_reg_value {
	uint16_t reg;
	uint16_t val;
};

struct imgout_csi_sensor_id {
	char name[128];
	const struct sensor_reg_value *data;
	uint32_t size;
	uint32_t type;
};

struct imgout_csi_addr {
	void *base_va;	//
	uint32_t *base_phys;
	size_t base_offset;	//  real image offset
	size_t base_length;	// real image size
};

struct imgout_csi_res {
	uint32_t row_num;
	uint32_t col_num;
	uint32_t dual_img;
	uint32_t src0_row_offset;
	uint32_t src1_row_offset;
	uint32_t dst0_row_offset;
	uint32_t dst1_row_offset;
};

struct mg_ir {
	uint32_t ir_shift;
	uint32_t ir_value;
};

struct cfg_map_mg {
	struct mg_ir ir[9];
	uint32_t mg_k1;
	uint32_t mg_k2;
	uint32_t mg_k3;
	uint32_t mg_p1;
	uint32_t mg_p2;
	uint32_t mg_fx;
	uint32_t mg_fy;
	uint32_t mg_u0;
	uint32_t mg_v0;
};

struct imgout_csi {
	uint8_t id;
	uint8_t revision;
	uint8_t irq_line;
	uint8_t irq_imgout_csi;
	uint8_t channel;
	uint8_t idi0_status;
	uint8_t idi1_status;
	uint32_t idi0_count;
	uint32_t idi1_count;
	uint32_t img_width;
	uint32_t img_height;
	uint32_t stride;
	uint32_t pixel_width;
	uint8_t img_format;
	uint8_t lane_num;
	uint8_t disable;
	uint32_t loop_mode;

	void *reg_va[ERG_BASE_END];	// reg ioremap
	uint32_t cfg_row_num;
	uint32_t cfg_col_num;
	uint32_t cfg_dual_img;

	size_t buffer_length;	// kmalloc buffer size
	uint32_t buffer_line;
	struct imgout_csi_addr cfg_src0;
	struct imgout_csi_addr cfg_src1;
	struct imgout_csi_addr cfg_dst0;
	struct imgout_csi_addr cfg_dst1;
	struct imgout_csi_addr cfg_bck0;
	struct imgout_csi_addr cfg_bck1;
	struct imgout_csi_addr cfg_bck2;
	struct imgout_csi_addr cfg_bck3;

	struct cfg_map_mg cfg_mg0;
	struct cfg_map_mg cfg_mg1;

	enum event_state estate;
	enum work_state wstate;
};


void imgout_set_dphy(uint32_t dphy_clk);
int imgout_csi_start_work_loop(struct imgout_csi *imgout_csi);
int mipi_test_main(void);
#endif // __IMGOUT_CSI_REGS_H__

// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
