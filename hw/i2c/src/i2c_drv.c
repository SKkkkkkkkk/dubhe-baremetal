#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "i2c_drv.h"
#include "i2c_slave.h"
#include "i2c_reg.h"
#include "systimer.h"
#include "i2c_debug.h"

void i2c_udelay(uint32_t us)
{
// #ifndef NO_SYS
#if 0
	vTaskDelay(pdMS_TO_TICKS(ms));
#else
	systimer_delay(us, IN_US);
#endif
}

static struct i2c_adapter g_i2c_adapter[4] = {0};

i2c_regs *i2c_get_base(int hwadapnr)
{
	switch (hwadapnr) {
	case 0:
		return (i2c_regs *)CONFIG_SYS_I2C0_BASE;
	case 1:
		return (i2c_regs *)CONFIG_SYS_I2C1_BASE;
	case 2:
		return (i2c_regs *)CONFIG_SYS_I2C2_BASE;
	case 3:
		return (i2c_regs *)CONFIG_SYS_I2C3_BASE;
	default:
		i2c_debug_err("Wrong I2C-adapter number %d\n", hwadapnr);
	}

	return NULL;
}

void _i2c_enable(i2c_regs *i2c_base, int enable)
{
	uint32_t ena = enable ? IC_ENABLE_0B : 0;
	int timeout = 200000;
	do {
		writel(ena, &i2c_base->IC_ENABLE);
		if ((readl(&i2c_base->IC_ENABLE_STATUS) & IC_ENABLE_0B) == ena)
			return;

		/*
         * Wait 10 times the signaling period of the highest I2C
         * transfer supported by the driver (for 400KHz this is
         * 25us) as described in the DesignWare I2C databook.
         */
		i2c_udelay(1);
		timeout -= 1;
	} while (timeout > 0);
}

void __i2c_calculate_speed(dw_scl_sda_cfg *scl_sda_cfg, uint32_t speed)
{
#if 0
	uint32_t scl_time_ns = (1 * 1000 * 1000 * 1000) / speed; /* 纳秒 */
	uint32_t ic_clk_time_ns = (1 * 1000 * 1000 * 1000) / IC_CLK; /* 纳秒 */
	uint32_t fall_and_rise_time_ns = 600; /* 上升下降沿占时间0.6毫秒 */
	unsigned ic_xx_spklen = 1;
	uint32_t hcnt = 0, lcnt = 0;

	/* 根据公式 scl_time = ((hcnt + ic_xx_spklen + 7) + lcnt + 1) * ic_clk_time + fall_and_rise_time_ns
     * 暂时将 ((hcnt + ic_xx_spklen + 7) 与 LCNT + 1对等
     * 则 lcnt + 1 = (scl_time / ic_clk_time) / 2 - 1
     *    hcnt = lcnt + 1 - (ic_xx_spklen + 7)
     */

	lcnt = ((scl_time_ns - fall_and_rise_time_ns) / ic_clk_time_ns) / 2 - 1;
	hcnt = lcnt + 1 - (ic_xx_spklen + 7);

	i2c_debug_dbg("hcnt = %d, lcnt = %d!\n",hcnt, lcnt);

	if (speed < I2C_STANDARD_SPEED_MAX) {
		scl_sda_cfg->ss_hcnt = hcnt;
		scl_sda_cfg->ss_lcnt = lcnt;
		scl_sda_cfg->ic_hs_spklen = ic_xx_spklen;
	} else if (speed <= I2C_FAST_SPEED_MAX) {
		if (hcnt < 14) { /* 极限 */
			lcnt = lcnt - (14 - hcnt);
			hcnt = 14;
		}

		lcnt = (lcnt < 16) ? 16 : lcnt;
		scl_sda_cfg->fs_hcnt = hcnt;
		scl_sda_cfg->fs_lcnt = lcnt;
		scl_sda_cfg->ic_fs_spklen = ic_xx_spklen;
	} else { /* fast 的极限 */
		scl_sda_cfg->fs_hcnt = 14;
		scl_sda_cfg->fs_lcnt = 16;
		scl_sda_cfg->ic_fs_spklen = ic_xx_spklen;
	}

	scl_sda_cfg->sda_hold = 0x00010001;
	i2c_debug_dbg("hcnt = %d, lcnt = %d!\n", hcnt, lcnt);

#else
	uint32_t scl_time_ns = (1 * 1000 * 1000 * 1000) / speed; /* 纳秒 */
    uint32_t ic_clk_time_ns =  (1 * 1000 * 1000 * 1000) / IC_CLK; /* 纳秒 */
    uint32_t fall_and_rise_time_ns = 600; /* 上升下降沿占时间0.6微秒 */
    uint32_t ic_xx_spklen = 1;
    uint32_t hcnt = 0, lcnt = 0;
    uint32_t diff_time = 0;
    uint32_t scl_high_time_ns = 0, scl_low_time_ns = 0;

    if (speed < I2C_STANDARD_SPEED_MAX) {
        fall_and_rise_time_ns = I2C_STANDARD_FALL_RISE_TIME;//1300;
        diff_time = 700;
    } else if (speed < I2C_FAST_SPEED_MAX) {
        fall_and_rise_time_ns = I2C_FAST_FALL_RISE_TIME;//600;
        diff_time = 700;
    } else if (speed < I2C_FAST_PLUS_SPEED_MAX) {
        fall_and_rise_time_ns = I2C_FAST_P_FALL_RISE_TIME;//240;
        diff_time = 240;
    } else {
        fall_and_rise_time_ns = 74;
        diff_time = 100;
    }

    /* 根据公式 scl_time = ((hcnt + ic_xx_spklen + 7) + lcnt + 1) * ic_clk_time + fall_and_rise_time_ns
     * 暂时将 ((hcnt + ic_xx_spklen + 7) 与 LCNT + 1对等
     * 则 lcnt + 1 = (scl_time / ic_clk_time) / 2 - 1
     *    hcnt = lcnt + 1 - (ic_xx_spklen + 7)
     */

    /*
     * scl_low_time_ns =  scl_high_time_ns + diff_time
     * scl_low_time_ns + scl_high_time_ns = scl_time_ns - fall_and_rise_time_ns
     * scl_high_time_ns = (scl_time_ns - fall_and_rise_time_ns - diff_time) / 2;
     * hcnt = scl_high_time_ns / ic_clk_time_ns;
     */

    scl_high_time_ns = (scl_time_ns - fall_and_rise_time_ns - diff_time) / 2;
    scl_low_time_ns =  scl_high_time_ns + diff_time;
    hcnt = scl_high_time_ns / ic_clk_time_ns;
    lcnt = scl_low_time_ns / ic_clk_time_ns;

    // lcnt = ((scl_time_ns - fall_and_rise_time_ns)/ ic_clk_time_ns) / 2 - 1;
    // hcnt = lcnt + 1 - (ic_xx_spklen + 7);

    if (speed < I2C_STANDARD_SPEED_MAX) {
        scl_sda_cfg->ss_hcnt = hcnt;
        scl_sda_cfg->ss_lcnt = lcnt;
        scl_sda_cfg->ic_hs_spklen = ic_xx_spklen;
    } else if (speed < I2C_FAST_PLUS_SPEED_MAX) {
        scl_sda_cfg->fs_hcnt = hcnt;
        scl_sda_cfg->fs_lcnt = lcnt;
        scl_sda_cfg->ic_fs_spklen = ic_xx_spklen;
    } else { /* fast 的极限 */
		scl_sda_cfg->fs_hcnt = 14;
		scl_sda_cfg->fs_lcnt = 16;
		scl_sda_cfg->ic_fs_spklen = ic_xx_spklen;
    }

    scl_sda_cfg->sda_hold = 0x00010001;
	i2c_debug_dbg("hcnt = %d, lcnt = %d!\n", hcnt, lcnt);
#endif
}

uint32_t __drv_i2c_calculate_speed(i2c_regs *i2c_base, dw_scl_sda_cfg *scl_sda_cfg, uint32_t speed)
{
	uint32_t cntl;
	uint32_t hcnt, lcnt;
	int i2c_spd;

	if (speed >= I2C_HIGH_SPEED_MAX) {
		i2c_spd = IC_SPEED_MODE_MAX;
	} else if (speed >= I2C_STANDARD_SPEED_MAX) {
		i2c_spd = IC_SPEED_MODE_FAST_PLUS;
	} else if (speed >= I2C_STANDARD_SPEED_MAX) {
		i2c_spd = IC_SPEED_MODE_FAST;
	} else {
		i2c_spd = IC_SPEED_MODE_STANDARD;
	}

	/* to set speed cltr must be disabled */
	_i2c_enable(i2c_base, 0);

	cntl = (readl(&i2c_base->IC_CON) & (~IC_CON_SPD_MSK));

	switch (i2c_spd) {
	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;
		if (scl_sda_cfg) {
			hcnt = scl_sda_cfg->ss_hcnt;
			lcnt = scl_sda_cfg->ss_lcnt;
		} else {
			hcnt = ((IC_CLK / 1000000) * MIN_SS_SCL_HIGHTIME) / NANO_TO_MICRO;
			lcnt = ((IC_CLK / 1000000) * MIN_SS_SCL_LOWTIME) / NANO_TO_MICRO;
		}
		writel(hcnt, &i2c_base->IC_SS_SCL_HCNT);
		writel(lcnt, &i2c_base->IC_SS_SCL_LCNT);
		writel(scl_sda_cfg->ic_hs_spklen, &i2c_base->IC_FS_SPKLEN);
		break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
		if (scl_sda_cfg) {
			hcnt = scl_sda_cfg->fs_hcnt;
			lcnt = scl_sda_cfg->fs_lcnt;
		} else {
			hcnt = ((IC_CLK / 1000000) * MIN_FS_SCL_HIGHTIME) / NANO_TO_MICRO;
			lcnt = ((IC_CLK / 1000000) * MIN_FS_SCL_LOWTIME) / NANO_TO_MICRO;
		}

		writel(0x00, &i2c_base->IC_SLV_DATA_NACK_ONL);
		writel(scl_sda_cfg->ic_fs_spklen, &i2c_base->IC_FS_SPKLEN);
		writel(hcnt, &i2c_base->IC_FS_SCL_HCNT);
		writel(lcnt, &i2c_base->IC_FS_SCL_LCNT);
		break;
	}

	writel(cntl, &i2c_base->IC_CON);

	/* Configure SDA Hold Time if required */
	if (scl_sda_cfg)
		writel(scl_sda_cfg->sda_hold, &i2c_base->IC_SDA_HOLD);

	/* Enable back i2c now speed set */
	_i2c_enable(i2c_base, 1);

	return 0;
}

static void i2c_setaddress(i2c_regs *i2c_base, uint32_t i2c_addr)
{
	/* Disable i2c */
	_i2c_enable(i2c_base, 0);
	writel(i2c_addr, &i2c_base->IC_TAR);

	/* Enable i2c */
	_i2c_enable(i2c_base, 1);
}

static void i2c_flush_rxfifo(i2c_regs *i2c_base)
{
	while (readl(&i2c_base->IC_STATUS) & IC_STATUS_RFNE)
		readl(&i2c_base->IC_CMD_DATA);
}

static int i2c_wait_for_bb(i2c_regs *i2c_base)
{
	int timeout_cnt = 0;

	while ((readl(&i2c_base->IC_STATUS) & IC_STATUS_MA) || !(readl(&i2c_base->IC_STATUS) & IC_STATUS_TFE)) {
		/* Evaluate timeout, 320ms */
		if (timeout_cnt > 320000) {
			i2c_debug_err("timeout !\n");
			return 1;
		}
		timeout_cnt++;
		i2c_udelay(1);
	}
	return 0;
}

static int i2c_xfer_init(i2c_regs *i2c_base, uint16_t chip, uint32_t addr, int alen)
{
	if (i2c_wait_for_bb(i2c_base)) {
		i2c_debug_err("i2c_wait_for_bb err!\n");
		return 1;
	}

	i2c_setaddress(i2c_base, chip);
	while (alen > 0) {
		alen--;
		/* high byte address going out first */
		writel((addr >> (alen * 8)) & 0xff, &i2c_base->IC_CMD_DATA);
	}
	return 0;
}

static int i2c_xfer_finish(i2c_regs *i2c_base)
{
	int timeout_cnt = 0;

	while (1) {
		if ((readl(&i2c_base->IC_RAW_INTR_STAT) & IC_STOP_DET)) {
			readl(&i2c_base->IC_CLR_STOP_DET);
			break;
		} else if (timeout_cnt > 100000) { // 100ms
			i2c_debug_err("timeout\n");
			break;
		}
		timeout_cnt++;
		i2c_udelay(1);
	}

	if (i2c_wait_for_bb(i2c_base)) {
		i2c_debug_err("Timed out waiting for bus\n");
		return 1;
	}

	i2c_flush_rxfifo(i2c_base);

	return 0;
}

typedef struct {
	uint32_t reg_addr;
	int alen;
	int one_adata_len;
} reg_addr_info;

static int __i2c_read(i2c_regs *i2c_base, uint16_t dev, reg_addr_info *addr_info, uint8_t *buffer, int len)
{
	uint32_t active = 0;
	int timeout_cnt = 0;

	if (addr_info == NULL) {
		return -1;
	}

	if (i2c_xfer_init(i2c_base, dev, addr_info->reg_addr, addr_info->alen)) {
		return 1;
	}
	timeout_cnt = 0;
	uint32_t one_data_len = addr_info->one_adata_len;
	uint32_t tmp = one_data_len;
	uint32_t i = 0;

	while (len > 0) {
		if (len <= (int)one_data_len) {
			tmp = (uint32_t)len;
		}

		if (!active) {
			/*
             * Avoid writing to ic_cmd_data multiple times
             * in case this loop spins too quickly and the
             * ic_status RFNE bit isn't set after the first
             * write. Subsequent writes to ic_cmd_data can
             * trigger spurious i2c transfer.
             */

			for (i = 0; i < tmp; i++) {
				if ((len - i) == 1) {
					writel(IC_CMD | IC_STOP, &i2c_base->IC_CMD_DATA);
				} else {
					writel(IC_CMD, &i2c_base->IC_CMD_DATA);
				}
			}

			active = 1;
		}

		for (i = 0; i < tmp; i++) {
			if (readl(&i2c_base->IC_STATUS) & IC_STATUS_RFNE) {
				*buffer++ = (uint8_t)readl(&i2c_base->IC_CMD_DATA);
				len--;
				timeout_cnt = 0;
				active = 0;
				continue;
			}

			timeout_cnt++;
			i2c_udelay(1);
		}

		if (timeout_cnt > 100000) { /* xxl: delay 100 ms for fpga */
			i2c_debug_err("timeout\n");
			break;
		}
	}

	return i2c_xfer_finish(i2c_base);
}

static int __i2c_write(i2c_regs *i2c_base, uint16_t dev, uint32_t addr, int alen, uint8_t *buffer, int len)
{
	int nb = len;
	int timeout_cnt = 0;

	if (i2c_xfer_init(i2c_base, dev, addr, alen))
		return 1;

	timeout_cnt = 0;
	while (len) {
		if (readl(&i2c_base->IC_STATUS) & IC_STATUS_TFNF) {
			if (--len == 0) {
				writel(*buffer | IC_STOP, &i2c_base->IC_CMD_DATA);
			} else {
				writel(*buffer, &i2c_base->IC_CMD_DATA);
			}
			buffer++;
			timeout_cnt = 0;
			continue;
		} else if (timeout_cnt > (nb * 20000)) {
			i2c_debug_err("Timed out. i2c write Failed\n");
			return 1;
		}
		timeout_cnt++;
		i2c_udelay(1);
	}

	return i2c_xfer_finish(i2c_base);
}

int dma_send_data(uint32_t *data, uint32_t data_len, uint32_t tar_addr, bool *const timeout);
static void __i2c_dma_cfg(i2c_regs *i2c_base, uint16_t dev)
{
	i2c_setaddress(i2c_base, dev);
	writel(0x3, &i2c_base->IC_DMA_CR);
	writel(0x4, &i2c_base->IC_DMA_RDLR);
	writel(0x4, &i2c_base->IC_DMA_TDLR);
}
static int __i2c_write_for_dma(i2c_regs *i2c_base, uint16_t dev, uint32_t addr, int alen, uint8_t *buffer, int len)
{
	// i2c_debug_dbg("dma i2c test !\n");
	uint32_t sr_data[258] = { 0 };
	int i = 0;
	__i2c_dma_cfg(i2c_base, dev);
	unsigned tmp_len = alen;
	while (tmp_len > 0) {
		tmp_len--;
		/* high byte address going out first */
		writel((addr >> (tmp_len * 8)) & 0xff, &i2c_base->IC_CMD_DATA);
		sr_data[i++] = (addr >> (tmp_len * 8)) & 0xff;
		//i2c_debug_dbg("r_data[%d] = 0x%02x !\n", i-1, sr_data[i - 1]);
	}

	for (i = alen; i < len + alen; i++) {
		sr_data[i] = (uint32_t)buffer[i - alen];
		//i2c_debug_dbg("r_data[%d] = 0x%02x !\n", i, sr_data[i]);
	}

	bool timeout = false;
	tmp_len = alen + len;
	uint32_t tar_addr = (uint32_t)(uintptr_t)(&i2c_base->IC_CMD_DATA);

	dma_send_data(sr_data, tmp_len, tar_addr, &timeout);
	if (timeout != false) {
		i2c_debug_err("Timed out. i2c write Failed\n");
		return -1;
	}

	return 0;
}

static void __i2c_init(struct i2c_adapter *adap)
{
	/* Disable i2c */
	i2c_regs *i2c_base = i2c_get_base(adap->hwadapnr);
	int speed = adap->speed;
	int fifo_tl = adap->fifo_tl;
	bool addr_10bit = adap->addr_10bit;
	uint32_t con_data = 0;
	uint32_t intr_mask = 0;

	_i2c_enable(i2c_base, 0);
	if (adap->is_slave == false) {
		intr_mask = IC_STOP_DET;
		con_data = IC_CON_SD | IC_CON_RE | IC_CON_SPD_FS | IC_CON_MM;
		if (addr_10bit == true) {
			con_data |= IC_CON_10BITADDRMASTER;
		}
	} else {
		con_data = IC_CON_SM | IC_CON_RE | IC_CON_SPD_FS | IC_CON_MD;
		intr_mask = DW_IC_INTR_RX_FULL | DW_IC_INTR_RD_REQ | DW_IC_INTR_RX_DONE | DW_IC_INTR_STOP_DET | DW_IC_INTR_START_DET | DW_IC_INTR_TX_ABRT;
		if (addr_10bit == true) {
			con_data |= IC_CON_10BITADDR_SLAVE;
		}
	}

	writel(con_data, &i2c_base->IC_CON);
	writel(fifo_tl - 1, &i2c_base->IC_RX_TL);
	writel(fifo_tl - 1, &i2c_base->IC_TX_TL);
	writel(intr_mask, &i2c_base->IC_INTR_MASK);
	if (adap->is_slave) {
		writel(adap->slaveaddr, &i2c_base->IC_SAR);
	}

	dw_scl_sda_cfg scl_sda_cfg = { 0 };
	__i2c_calculate_speed(&scl_sda_cfg, speed);
	__drv_i2c_calculate_speed(i2c_base, &scl_sda_cfg, speed);

	/* Enable i2c */
	_i2c_enable(i2c_base, 1);
}

static uint32_t i2c_set_bus_speed(struct i2c_adapter *adap, uint32_t speed)
{
	adap->speed = speed;
	dw_scl_sda_cfg scl_sda_cfg = { 0 };
	__i2c_calculate_speed(&scl_sda_cfg, speed);
	return __drv_i2c_calculate_speed(i2c_get_base(adap->hwadapnr), &scl_sda_cfg, speed);
}

static void drv_i2c_init(struct i2c_adapter *adap)
{
	__i2c_init(adap);
}

static int drv_i2c_read(struct i2c_adapter *adap, uint16_t dev, uint32_t addr, int alen, uint8_t *buffer, int len)
{
	adap->reg_addr = addr;
	adap->reg_val = 0;
	reg_addr_info addr_info;
	addr_info.reg_addr = addr;
	addr_info.alen = alen;
	addr_info.one_adata_len = adap->one_adata_len;
	return __i2c_read(i2c_get_base(adap->hwadapnr), dev, &addr_info, buffer, len);
}

static int drv_i2c_write(struct i2c_adapter *adap, uint16_t dev, uint32_t addr, int alen, uint8_t *buffer, int len)
{
	adap->reg_addr = addr;
	adap->reg_val = *buffer;
	int ret = 0;
	if (adap->dma_use == 1) {
		ret = __i2c_write_for_dma(i2c_get_base(adap->hwadapnr), dev, addr, alen, buffer, len);
	} else {
		ret = __i2c_write(i2c_get_base(adap->hwadapnr), dev, addr, alen, buffer, len);
	}
	return ret;
}

/* drv_i2c_probe - Probe the i2c chip */
static int drv_i2c_probe(struct i2c_adapter *adap, uint16_t dev)
{
	i2c_regs *i2c_base = i2c_get_base(adap->hwadapnr);
	uint32_t tmp;
	int ret;

	/*
     * Try to read the first location of the chip.
     */
	reg_addr_info addr_info;
	addr_info.reg_addr = 0;
	addr_info.alen = 1;
	addr_info.one_adata_len = adap->one_adata_len;

	ret = __i2c_read(i2c_base, dev, &addr_info, (uint8_t *)&tmp, 1);
	if (ret != 0) {
		drv_i2c_init(adap);
		ret = __i2c_read(i2c_base, dev, &addr_info, (uint8_t *)&tmp, 1);
	}

	return ret;
}

struct i2c_adapter *ape_i2c_init(int busindex)
{
	struct i2c_adapter *i2c_ad = &g_i2c_adapter[busindex];
	if (i2c_ad == NULL) {
		return i2c_ad;
	}

	i2c_ad->init = drv_i2c_init;
	i2c_ad->probe = drv_i2c_probe;
	i2c_ad->read = drv_i2c_read;
	i2c_ad->write = drv_i2c_write;
	i2c_ad->set_bus_speed = i2c_set_bus_speed;
	i2c_ad->speed = 50000;
	i2c_ad->slaveaddr = 0x21;
	i2c_ad->init_done = 0;
	i2c_ad->hwadapnr = busindex; // i2c0--0 i2c1--1 i2c2--2
	i2c_ad->name = NULL;
	i2c_ad->reg_addr = 0;
	i2c_ad->reg_val = 0;
	i2c_ad->one_adata_len = 1;
	i2c_ad->dma_use = 0;
	i2c_ad->fifo_tl = FIFO_TL;
	i2c_ad->addr_10bit = false;
	i2c_ad->is_slave = false;

	// i2c_ad->init(i2c_ad, i2c_ad->speed);
	/*
    int ret = -1;
    ret = i2c_ad->probe(i2c_ad, devaddr);
    if (ret) {
        i2c_debug_err(">>> %s[%d] probe failed\n", __func__, __LINE__);
    }
*/
	return i2c_ad;
}

int ape_i2c_uninit(struct i2c_adapter *i2c_ad)
{
	memset(i2c_ad, 0, sizeof(struct i2c_adapter));
	return 0;
}

int dma_send_data(uint32_t *data, uint32_t data_len, uint32_t tar_addr, bool *const timeout)
{
#if 0
    if (data == NULL) {
        return -1;
    }

    // REG32(I2CM_BASE + IC_DMA_CR) = 0x3;// transmit FIFO DMA channel enabled
    // REG32(I2CM_BASE + IC_DMA_RDLR) = 0x4;//transmit Data Level is 0x3. This bit field controls the level at which a DMA request is made by the transmit logic
    // REG32(I2CM_BASE + IC_DMA_TDLR) = 0x4;//receive Data Level is 0x3. This bit field controls the level at which a DMA request is made by the transmit logic

    systimer_delay(1000, IN_US);
    uint32_t temp;
    temp = REG32(DMA_BASE + DMAC_CHENREG_0);

    REG32(DMA_BASE + CH1_SAR_0) = (uint32_t)data;

    REG32(DMA_BASE + CH1_DAR_0) = tar_addr;

    REG32(DMA_BASE + CH1_CTL_0) = AXI_MASTER_0                         << 0|
                                   AXI_MASTER_0                         << 2|
                                   SRC_ADDR_INCREMENT                   << 4|
                                   DST_ADDR_NOCHANGE                    << 6|
                                   SRC_TRANSFER_WIDTH_32                << 8|
                                   DST_TRANSFER_WIDTH_32                <<11|
                                   //SRC_MSIZE_1                        <<14|
                                   DST_MSIZE_1                          <<18|
                                   NONPOSTED_LASTWRITE_EN               <<30;

    REG32(DMA_BASE + CH1_CTL_32) = ARLEN_EN                               <<(38-32)|
                                    0X0                                    <<(39-32)|//source burst length
                                    AWLEN_EN                               <<(47-32)|
                                    0x0                                    <<(48-32)|//destination burst length
                                    SRC_STATUS_DISABLE                     <<(56-32)|
                                    DST_STATUS_DISABLE                     <<(57-32)|
                                    INTDISABLE_COMPLETOFBLKTRANS_SHADORLLI <<(58-32)|
                                    NOTLAST_SHADORLLI                      <<(62-32)|
                                    SHADORLLI_INVALID                      <<(63-32);



    REG32(DMA_BASE + CH1_BLOCK_TS_0) = data_len - 1;  // b:32 BLOCK_TS //write data number

    REG32(DMA_BASE + CH1_CFG2_0) =  (SRC_CONTIGUOUS        << 0)|
                                    (DST_CONTIGUOUS        << 2);


    REG32(DMA_BASE + CH1_CFG2_32) =  MEM_TO_PER_DMAC       <<(32-32)|
                                     DST_HARDWARE_HS       <<(36-32)|
                                     0x8                   <<(44-32)|//dst handshake
                                     CHANNEL_PRIORITY7     <<(49-32)|
                                     CHANNEL_LOCK_DISABLE  <<(52-32)|
                                     0x4                   <<(55-32)|//Source Outstanding Request Limit == 3
                                     0x4                   <<(59-32);//Destination Outstanding Request Limit == 3

    REG32(DMA_BASE + CH1_INTSTATUS_ENABLEREG_0) = 0xffffffff;//Enable interrupt generation bit is valid
    REG32(DMA_BASE + CH1_INTSIGNAL_ENABLEREG_0) = 0xffffffff;//Enable interrupt generation bit is valid
    REG32(DMA_BASE + DMAC_CFGREG_0)  = 0x3;//enable DMAC and its interrupt logic
    REG32(DMA_BASE + DMAC_CHENREG_0) = 0x101;//EN channel0  while(1)



    // REG32(DMA_BASE + CH1_SAR_0) = (uint32_t)data;

    // REG32(DMA_BASE + CH1_DAR_0) = tar_addr;
    // REG32(DMA_BASE + CH1_BLOCK_TS_0) = data_len - 1;

    uint32_t time_cnt = 0;


    while(1) {
        temp = REG32(DMA_BASE+CH1_INTSTATUS_0);
        if((temp & 0x00000002) ==  0x00000002) {
            break;
        } else {
            time_cnt++;
            systimer_delay(1000, IN_US);
            /* 延时 */
         }

        if (time_cnt > 1000) {
            *timeout = 1;
            return -1;
        }
    }

    systimer_delay(1000, IN_US);
#endif
	return 0;
}
