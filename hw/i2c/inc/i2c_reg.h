#ifndef __I2C_REG_H__
#define __I2C_REG_H_

#include "regs_type.h"
// #define CONFIG_SYS_I2C0_BASE I2C0_BASE
// #define CONFIG_SYS_I2C1_BASE I2C1_BASE
// #define CONFIG_SYS_I2C2_BASE I2C2_BASE
// #define CONFIG_SYS_I2C3_BASE I2C3_BASE
// #define CONFIG_SYS_I2C4_BASE I2C4_BASE

#if !defined(IC_CLK)
#define IC_CLK 20000000
#endif
#define NANO_TO_MICRO 1000
/* High and low times in different speed modes (in ns) */
#define MIN_SS_SCL_HIGHTIME 4000
#define MIN_SS_SCL_LOWTIME 4700
#define MIN_FS_SCL_HIGHTIME 600
#define MIN_FS_SCL_LOWTIME 1300
#define MIN_HS_SCL_HIGHTIME 60
#define MIN_HS_SCL_LOWTIME 160
/* Worst case timeout for 1 byte is kept as 2ms */
#define I2C_BYTE_TO (CONFIG_SYS_HZ / 500)
#define I2C_STOPDET_TO (CONFIG_SYS_HZ / 500)
#define I2C_BYTE_TO_BB (I2C_BYTE_TO * 16)
/* i2c control register definitions */
#define IC_CON_SD 0x0040
#define IC_CON_SM 0x0000
#define IC_CON_RE 0x0020
#define IC_CON_10BITADDRMASTER 0x0010
#define IC_CON_10BITADDR_SLAVE 0x0008
#define IC_CON_SPD_MSK 0x0006
#define IC_CON_SPD_SS 0x0002
#define IC_CON_SPD_FS 0x0004
#define IC_CON_SPD_HS 0x0006
#define IC_CON_MM 0x0001
#define IC_CON_MD 0x0000
/* i2c target address register definitions */
#define TAR_ADDR 0x0050
/* i2c slave address register definitions */
#define IC_SLAVE_ADDR 0x0002
/* i2c data buffer and command register definitions */
#define IC_CMD 0x0100
#define IC_STOP 0x0200
/* i2c interrupt status register definitions */
#define IC_GEN_CALL 0x0800
#define IC_START_DET 0x0400
#define IC_STOP_DET 0x0200
#define IC_ACTIVITY 0x0100
#define IC_RX_DONE 0x0080
#define IC_TX_ABRT 0x0040
#define IC_RD_REQ 0x0020
#define IC_TX_EMPTY 0x0010
#define IC_TX_OVER 0x0008
#define IC_RX_FULL 0x0004
#define IC_RX_OVER 0x0002
#define IC_RX_UNDER 0x0001
/* fifo threshold register definitions */

#define FIFO_TL 8

#define IC_TL0 0x00
#define IC_TL1 0x01
#define IC_TL2 0x02
#define IC_TL3 0x03
#define IC_TL4 0x04
#define IC_TL5 0x05
#define IC_TL6 0x06
#define IC_TL7 0x07

/* i2c enable register definitions */
#define IC_ENABLE_0B 0x0001
/* i2c status register  definitions */
#define IC_STATUS_SA 0x0040
#define IC_STATUS_MA 0x0020
#define IC_STATUS_RFF 0x0010
#define IC_STATUS_RFNE 0x0008
#define IC_STATUS_TFE 0x0004
#define IC_STATUS_TFNF 0x0002
#define IC_STATUS_ACT 0x0001
/* Speed Selection */

#define I2C_HIGH_SPEED_MAX 3400000
#define I2C_FAST_PLUS_SPEED_MAX 1000000
#define I2C_FAST_SPEED_MAX 400000
#define I2C_STANDARD_SPEED_MAX 100000

#define I2C_STANDARD_FALL_RISE_TIME 1300 /* 150ns */
#define I2C_FAST_FALL_RISE_TIME 600 /* 130ns */
#define I2C_FAST_P_FALL_RISE_TIME 240

#define I2C_MASTER_ENABLE (1 << 0)
#define I2C_MASTER_DISABLE (0 << 0)
#define I2C_SLAVE_ENABLE (0 << 6)
#define I2C_SLAVE_DISABLE (1 << 6)

#define DW_IC_INTR_RX_UNDER (1 << 0)
#define DW_IC_INTR_RX_OVER (1 << 1)
#define DW_IC_INTR_RX_FULL (1 << 2)
#define DW_IC_INTR_TX_OVER (1 << 3)
#define DW_IC_INTR_TX_ABRT (1 << 6)
#define DW_IC_INTR_RD_REQ (1 << 5)
#define DW_IC_INTR_RX_DONE (1 << 7)
#define DW_IC_STATUS_SLAVE_ACTIVITY (1 << 6)
#define DW_IC_INTR_ACTIVITY (1 << 8)
#define DW_IC_INTR_STOP_DET (1 << 9)
#define DW_IC_INTR_START_DET (1 << 10)
#define DW_IC_INTR_GEN_CALL (1 << 11)

#define writel(d, a) *(volatile uint32_t *)(a) = (d)
#define readl(a) (*(volatile uint32_t *)(a))
#define udelay(x)                                                                                                      \
	{                                                                                                              \
		unsigned xx = x;                                                                                       \
		while (xx--) {                                                                                         \
			asm volatile("nop");                                                                           \
		}                                                                                                      \
	}

typedef struct _I2C_REGS {
	__IOM uint32_t IC_CON; /* 0x00 */
	__IOM uint32_t IC_TAR; /* 0x04 */
	__IOM uint32_t IC_SAR; /* 0x08 */
	__IOM uint32_t IC_HS_MADDR; /* 0x0c */
	__IOM uint32_t IC_CMD_DATA; /* 0x10 */
	__IOM uint32_t IC_SS_SCL_HCNT; /* 0x14 */
	__IOM uint32_t IC_SS_SCL_LCNT; /* 0x18 */
	__IOM uint32_t IC_FS_SCL_HCNT; /* 0x1c */
	__IOM uint32_t IC_FS_SCL_LCNT; /* 0x20 */
	__IOM uint32_t IC_HS_SCL_HCNT; /* 0x24 */
	__IOM uint32_t IC_HS_SCL_LCNT; /* 0x28 */
	__IM uint32_t IC_INTR_STAT; /* 0x2c */
	__IOM uint32_t IC_INTR_MASK; /* 0x30 */
	__IM uint32_t IC_RAW_INTR_STAT; /* 0x34 */
	__IOM uint32_t IC_RX_TL; /* 0x38 */
	__IOM uint32_t IC_TX_TL; /* 0x3c */
	__IM uint32_t IC_CLR_INTR; /* 0x40 */
	__IM uint32_t IC_CLR_RX_UNDER; /* 0x44 */
	__IM uint32_t IC_CLR_RX_OVER; /* 0x48 */
	__IM uint32_t IC_CLR_TX_OVER; /* 0x4c */
	__IM uint32_t IC_CLR_RD_REQ; /* 0x50 */
	__IM uint32_t IC_CLR_TX_ABRT; /* 0x54 */
	__IM uint32_t IC_CLR_RX_DONE; /* 0x58 */
	__IM uint32_t IC_CLR_ACTIVITY; /* 0x5c */
	__IM uint32_t IC_CLR_STOP_DET; /* 0x60 */
	__IM uint32_t IC_CLR_START_DET; /* 0x64 */
	__IM uint32_t IC_CLR_GEN_CALL; /* 0x68 */
	__IOM uint32_t IC_ENABLE; /* 0x6c */
	__IM uint32_t IC_STATUS; /* 0x70 */
	__IOM uint32_t IC_TXFLR; /* 0x74 */
	__IOM uint32_t IC_RXFLR; /* 0x78 */
	__IOM uint32_t IC_SDA_HOLD; /* 0x7c */
	__IM uint32_t IC_TX_ABRT_SOURCE; /* 0x80 */
	__IOM uint32_t IC_SLV_DATA_NACK_ONL; /* 0x84 */
	__IOM uint32_t IC_DMA_CR; /* 0x88 */
	__IOM uint32_t IC_DMA_TDLR; /* 0x8c */
	__IOM uint32_t IC_DMA_RDLR; /* 0x90 */
	__IOM uint32_t IC_SDA_SETUP; /* 0x94 */
	__IOM uint32_t IC_ACK_GENERAL_CALL; /* 0x98 */
	__IM uint32_t IC_ENABLE_STATUS; /* 0x9c */
	__IOM uint32_t IC_FS_SPKLEN; /* 0xa0 */
	__IOM uint32_t IC_HS_SPKLEN; /* 0xa4 */
} i2c_regs;

#ifdef A55
#define CONFIG_SYS_I2C0_BASE 0x21080000
#define CONFIG_SYS_I2C1_BASE 0x27070000
#define CONFIG_SYS_I2C2_BASE 0x27080000
#define CONFIG_SYS_I2C3_BASE 0x27090000
#else
#define CONFIG_SYS_I2C0_BASE 0x41080000
#define CONFIG_SYS_I2C1_BASE 0x47070000
#define CONFIG_SYS_I2C2_BASE 0x47080000
#define CONFIG_SYS_I2C3_BASE 0x47090000

#endif
typedef struct {
	uint32_t ss_hcnt;
	uint32_t fs_hcnt;
	uint32_t ss_lcnt;
	uint32_t fs_lcnt;
	uint32_t sda_hold;
	uint32_t ic_fs_spklen;
	uint32_t ic_hs_spklen;
} dw_scl_sda_cfg;

typedef enum {
	IC_SPEED_MODE_STANDARD = 0,
	IC_SPEED_MODE_FAST = 1,
	IC_SPEED_MODE_FAST_PLUS = 2,
	IC_SPEED_MODE_MAX = 3
} ic_speed_mode;

// struct dw_i2c {
// 	struct i2c_regs *regs;
// 	dw_scl_sda_cfg *scl_sda_cfg;
// };

// void dump_reg(uint32_t base, uint32_t reg_cnt);
// void dw_i2c_enable(i2c_regs *i2c_base, int enable);
// void __dw_i2c_calculate_speed(dw_scl_sda_cfg *scl_sda_cfg, uint32_t speed);
// uint32_t __dw_i2c_set_bus_speed(i2c_regs *i2c_base, dw_scl_sda_cfg *scl_sda_cfg, uint32_t speed);
// i2c_regs *i2c_get_base(int hwadapnr);
i2c_regs *i2c_get_base(int hwadapnr);

#endif
