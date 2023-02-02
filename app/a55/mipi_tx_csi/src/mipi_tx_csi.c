#include <stdio.h>
#include <stdint.h>
#include "imgout_sys_regs.h"
#include "mipi_tx_csi.h"
#include "idma_regs.h"

#define YUV8BIT 0x1e
#define RGB888  0x24
#define RAW8    0x2a
#define RAW12   0x2c
#define RAW16   0x2e

struct imgout_csi st_imgout_csi;

void imgout_set_dphy(uint32_t dphy_clk)
{
	IMAGE_SYS_SET_TX_PHY_POWER_READY(0x0);
	if (dphy_clk == 400) {
		IMAGE_SYS_SET_TX_PHY_PLL_PRE_DIV(0);
		IMAGE_SYS_SET_TX_PHY_PLL_FBK_INT(0x85);
		IMAGE_SYS_SET_TX_PHY_EXTD_CYCLE_SEL(0x2);
		IMAGE_SYS_SET_TX_PHY_PLL_FBK_FRA(0x555555);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_PRE_TIME(0xa);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_ZERO_TIME(0x18);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_TRAIL_TIME(0x11);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_PRE_TIME(0x7);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_ZERO_TIME(0x35);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_TRAIL_TIME(0xf);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_CLK_PRE_TIME(0x3);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_CLK_POST_TIME(0x25);
	} else if (dphy_clk == 800) {
		IMAGE_SYS_SET_TX_PHY_PLL_PRE_DIV(0);
		IMAGE_SYS_SET_TX_PHY_PLL_FBK_INT(0x85);
		IMAGE_SYS_SET_TX_PHY_EXTD_CYCLE_SEL(0x1);
		IMAGE_SYS_SET_TX_PHY_PLL_FBK_FRA(0x555555);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_PRE_TIME(0x9);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_ZERO_TIME(0x17);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_TRAIL_TIME(0x10);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_PRE_TIME(0x7);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_ZERO_TIME(0x35);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_TRAIL_TIME(0xf);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_CLK_PRE_TIME(0x1);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_CLK_POST_TIME(0x18);
	} else if (dphy_clk == 1000) {
		IMAGE_SYS_SET_TX_PHY_PLL_PRE_DIV(0);
		IMAGE_SYS_SET_TX_PHY_PLL_FBK_INT(0xa6);
		IMAGE_SYS_SET_TX_PHY_EXTD_CYCLE_SEL(0x1);
		IMAGE_SYS_SET_TX_PHY_PLL_FBK_FRA(0xaaaaaa);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_PRE_TIME(0xb);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_ZERO_TIME(0x1c);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_TRAIL_TIME(0x13);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_PRE_TIME(0x9);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_ZERO_TIME(0x42);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_TRAIL_TIME(0x12);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_CLK_PRE_TIME(0x1);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_CLK_POST_TIME(0x1b);
	} else if (dphy_clk == 1200) {
		IMAGE_SYS_SET_TX_PHY_PLL_PRE_DIV(0);
		IMAGE_SYS_SET_TX_PHY_PLL_FBK_INT(0xc8);
		IMAGE_SYS_SET_TX_PHY_EXTD_CYCLE_SEL(0x1);
		IMAGE_SYS_SET_TX_PHY_PLL_FBK_FRA(0x0);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_PRE_TIME(0xe);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_ZERO_TIME(0x20);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_TRAIL_TIME(0x16);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_PRE_TIME(0xa);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_ZERO_TIME(0x50);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_TRAIL_TIME(0x15);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_CLK_PRE_TIME(0x1);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_CLK_POST_TIME(0x1e);
	} else {
		IMAGE_SYS_SET_TX_PHY_PLL_PRE_DIV(0);
		IMAGE_SYS_SET_TX_PHY_PLL_FBK_INT(0x85);
		IMAGE_SYS_SET_TX_PHY_EXTD_CYCLE_SEL(0x2);
		IMAGE_SYS_SET_TX_PHY_PLL_FBK_FRA(0x555555);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_PRE_TIME(0xa);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_ZERO_TIME(0x18);
		IMAGE_SYS_SET_TX_PHY_DLANE_HS_TRAIL_TIME(0x11);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_PRE_TIME(0x7);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_ZERO_TIME(0x35);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_TRAIL_TIME(0xf);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_CLK_PRE_TIME(0x3);
		IMAGE_SYS_SET_TX_PHY_CLANE_HS_CLK_POST_TIME(0x25);

	}

	IMAGE_SYS_SET_TX_PHY_POWER_READY(0x1);
	IMAGE_SYS_SET_RX_PHY0_POWER_READY(1);
}

int imgout_csi_start_work_loop(struct imgout_csi *imgout_csi)
{
	int ret;
	printf("imgout_csi_start_work_loop start\r\n");
	printf("imgphy base 0x%lx\r\n", IMAGE_SYS_BASE);
	printf("idma base 0x%lx\r\n", IDMA_BASE);
	printf("idma ctl 0x%lx\r\n", MIPI_TX_CSI_BASE);

	ret = readl(IDMA_BASE);
	printf(" ret = 0x%x \r\n", ret);

    IMAGE_SYS_SET_TX_PHY_L0_SWAP(2);	
	printf("IMAGE_SYS_SET_TX_PHY_L0_SWAP ok\r\n");
    IMAGE_SYS_SET_TX_PHY_L1_SWAP(1);
	printf("IMAGE_SYS_SET_TX_PHY_L1_SWAP ok\r\n");
    IMAGE_SYS_SET_TX_PHY_L2_SWAP(4);
    IMAGE_SYS_SET_TX_PHY_L3_SWAP(0);
    IMAGE_SYS_SET_TX_PHY_L4_SWAP(3);

	IMAGE_SYS_SET_TX_PHY_REFCLK_SEL(1); //reg200 12M
	IMAGE_SYS_SET_TX_PHY_POWER_READY(1); //reg200 power ready

	IMAGE_SYS_SET_SF_TX_DPHY_SEL(0);  //reg304 csi
	IMAGE_SYS_SET_ISP_DISABLE(0);
	IMAGE_SYS_SET_SF_DSI_LCD_SEL(0);
	printf("IMAGE_SYS_SET_SF_DSI_LCD_SEL ok\r\n");
	writel(imgout_csi->lane_num - 1,
		  imgout_csi->reg_va[REGS_BASE_CTRL] + 0xe4); //set lanes
	printf("writel ok\r\n");
	writel(0x10, imgout_csi->reg_va[REGS_BASE_CTRL] + 0xf0);//clk division
	printf("writel ok\r\n");
	writel(0x1, imgout_csi->reg_va[REGS_BASE_CTRL] + 0xe8); //clk continuous mode enable
	printf("writel ok\r\n");
	writel(0x1, imgout_csi->reg_va[REGS_BASE_CTRL] + 0x4); //ctrl dereset
	printf("writel ok\r\n");
	writel(0x7, imgout_csi->reg_va[REGS_BASE_CTRL] + 0xe0); //phy dereset
	printf("writel ok\r\n");

	IDMA_SET_CFG_IMG_WIDTH(imgout_csi->img_width - 1);
	IDMA_SET_CFG_IMG_HEIGHT(imgout_csi->img_height - 1);
	IDMA_SET_CFG_IN_OUT_FORMAT(imgout_csi->img_format);
	IDMA_SET_CFG_IMG_STRIDE(imgout_csi->stride);
	IDMA_SET_CFG_ARLEN(0x7);
	IDMA_SET_CFG_OUTSTANDING(0x0);
	IDMA_SET_CFG_FRAME_BLANKING(80);
	IDMA_SET_CFG_LINE_BLANKING(20);
	IDMA_SET_EBD_LINES(0);
	IDMA_SET_IDI_VC_ID(0);
	IDMA_SET_INT0_MASK(3);
	IDMA_SET_SOFT_RST(0);
	IDMA_SET_WORK_ADDR(imgout_csi->cfg_src0.base_phys);
	IDMA_SET_WORK_START(1);
	return 0;
}

void imgout_csi_int_handler()
{				/*{{{ */
	uint32_t status;
	// uint32_t finish_addr;
	status = readl(IDMA_BASE + 0x100);
	printf("imgout_csi_int_handler 0x%x\n", status);
	if (status & 0x2)
        ;
	if (status & 0x1) {
		// finish_addr = IDMA_GET_FINISH_WORK_ADDR;
		printf("imgout_csi_int_handler 0x%x\n", status);
		IDMA_SET_WORK_ADDR(st_imgout_csi.cfg_src0.base_phys);
		st_imgout_csi.disable = 1;
		if (st_imgout_csi.disable == 0) {
			IDMA_SET_WORK_START(1);
		}
	}
	writel(0, IDMA_BASE + 0x100);
}				/*}}} */

/**
 * @brief 
 * @param imgout_csi : 传入图像设置结构体
 */
void imgout_res(struct imgout_csi *imgout_csi)
{
	switch (imgout_csi->img_format) {
	case YUV8BIT:
		imgout_csi->pixel_width = 16;
		break;
	case RGB888:
		imgout_csi->pixel_width = 24;
		break;
	case RAW8:
		imgout_csi->pixel_width = 8;
		break;
	case RAW12:
		imgout_csi->pixel_width = 12;
		break;
	case RAW16:
		imgout_csi->pixel_width = 16;
		break;
	default:
		imgout_csi->pixel_width = 8;
	}
	imgout_csi->stride =
	    (imgout_csi->img_width * imgout_csi->pixel_width / 8 +
	     127) / 128 * 128;
}

/* 向内存中填充数据
 */
void mipi_creat_img(unsigned char * pImg) {

	int i;
	for(i = 0; i < 896 * 600 * 2; i += 2){
	*((uint8_t *)pImg+ i + 0) = 0x11;
	*((uint8_t *)pImg+ i + 1) = 0x22;
	}
}

void mipi_printf_img(unsigned char * pImg) {

	int i;
	printf(" image buf : \r\n");
	for(i = 0; i < 800; i++){
		printf(" 0x%x", pImg[i]);
		if( ((i % 10) == 0) ) {
			printf("\r\n");
		}
	}
}

int mipi_test_main(void)
{
	printf("init prarm ok\r\n");
    uint32_t *work_addr = (uint32_t *)DDR_BASE + 0x1000000;
	printf("init prarm ok\r\n");
    // stride = (800*pixel_width/8 + 127)/128*128;
    st_imgout_csi.img_width = 800;
	st_imgout_csi.img_height = 600;
	st_imgout_csi.img_format = RAW16; //RAW8 12 16
	st_imgout_csi.reg_va[REGS_BASE_CTRL] = (void *)MIPI_TX_CSI_BASE;
	st_imgout_csi.cfg_src0.base_phys = work_addr;
	
	st_imgout_csi.lane_num = 4; //lane
	printf("init prarm ok\r\n");
	imgout_res(&st_imgout_csi);
	printf("init imgout_res ok\r\n");
	mipi_creat_img((unsigned char *)st_imgout_csi.cfg_src0.base_phys);
	printf("init mipi_creat_img ok\r\n");

	GIC_DistInit();
	GIC_CPUInterfaceInit(); //per CPU

    GIC_SetTarget(IDI_IRQn, 1 << 0); //core0
	printf("init GIC_SetTarget ok\r\n");
    IRQ_SetHandler(IDI_IRQn, imgout_csi_int_handler);
	printf("init IRQ_SetHandler ok\r\n");
	IRQ_SetPriority(IDI_IRQn, 0 << 3);
	printf("init IRQ_SetPriority ok\r\n");
	IRQ_Enable(IDI_IRQn);
	printf("init IRQ_Enable ok\r\n");

	imgout_set_dphy(400);
	imgout_csi_start_work_loop(&st_imgout_csi);
	printf("init imgout_csi_start_work_loop ok\r\n");
	while(1) {
		if ( st_imgout_csi.disable == 1 ) {
			mipi_printf_img((unsigned char *)st_imgout_csi.cfg_src0.base_phys);
			IDMA_SET_WORK_START(1);
			st_imgout_csi.disable = 0;
		}
	}

    return 0;
}
