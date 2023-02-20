#include <stdio.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
#include "i2c_wo.h"
#include "i2c.h"
#include "pinmux.h"

#define STOP             1

struct cfg {
    char name[ 128 ];
    int  i2c_bus;
    int  reg_addr;
    int  check_addr;
    int  check_len;
};

static int i2c_write_reg(struct cfg *cfg, uint16_t reg, uint32_t len, uint32_t val)
{
    i2c_msg msgs;
    int     ret  = 0;
    uint8_t tmp8 = 0;
    uint8_t tmp[ 2 ];

    msgs.addr     = cfg->reg_addr;
    msgs.flags    = RT_I2C_WR;
    msgs.reg_addr = reg;
    msgs.alen     = 1;
    if (len == 1) {
        tmp8     = (uint8_t) val;
        msgs.buf = &tmp8;
    } else if (len == 2) {
        tmp[ 0 ] = (uint8_t) val >> 8;
        tmp[ 1 ] = (uint8_t) val;
        msgs.buf = tmp;
    } else {
        printf("i2c write unkonw i2c len\n");
    }
    msgs.buf_len = len;
    ret          = i2c_master_transfer(cfg->i2c_bus, &msgs, 1);
    if (ret != 0) {
        printf("i2c_write_reg err\n");
        return -1;
    }

    return 0;
}

void i2c4_test(void)
{
    struct cfg cfg;
	uint16_t reg = 0x40;
	uint32_t val = 0x55;

    uint32_t tmp;
    tmp = REG32(SYS_BASE + 0x8dc);
    tmp &= ~(7 << 4);
    tmp |= 0 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8dc) = tmp;

    tmp = REG32(SYS_BASE + 0x8e0);
    tmp &= ~(7 << 4);
    tmp |= 0 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8e0) = tmp;

    cfg.reg_addr   = 0x34;
    cfg.i2c_bus    = 4;
    cfg.check_addr = 0x00;
    cfg.check_len  = 1;

    i2c_master_init(cfg.i2c_bus);

	while(1){
		i2c_write_reg(&cfg, reg, 1, val);
		systimer_delay(1, IN_S);
		printf("i2c4 write 0x40 0x55\n");
	}
}

int pmic_to_sleep_delay(int ms)
{
    uint32_t tmp;
    tmp = REG32(SYS_BASE + 0x8dc);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8dc) = tmp;

    tmp = REG32(SYS_BASE + 0x8e0);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8e0) = tmp;
    i2c_wo_init(0x34);

    i2c_wo_delay(ms); // 1s
    // i2c_wo_fifo(0x10);  //poweroff
    // i2c_wo_fifo(0x01 | STOP<<8);
    // i2c_wo_fifo(0x40);
    // i2c_wo_fifo(0x00 | STOP << 8);
    // i2c_wo_fifo(0x41);
    // i2c_wo_fifo(0x00 | STOP << 8);
    // i2c_wo_fifo(0x42);
    // i2c_wo_fifo(0x00 | STOP << 8);
    // i2c_wo_fifo(0x26);
    // i2c_wo_fifo(0x09 | STOP << 8);
    // i2c_wo_fifo(0x80);
    // i2c_wo_fifo(0x00 | STOP << 8);
    // i2c_wo_fifo(0x90);
    // i2c_wo_fifo(0x00 | STOP << 8);
    // i2c_wo_fifo(0x91);
    // i2c_wo_fifo(0x00 | STOP << 8);
    // i2c_wo_fifo(0x26);
    // i2c_wo_fifo(0x19 | STOP << 8);
    // i2c_wo_fifo(0x41);
    // i2c_wo_fifo(0x0c | STOP << 8);

    printf("axp2101_powerkey_suspend !!! \n");
	return 0;
}

int main()
{
    // int ret;
    printf("test %s ...\n", __FILE__);
    systimer_init();

	// pmic_to_sleep_delay(5);
	// i2c_wo_appower_enable(0);
    // i2c_wo_start();

    while (1) {
#if 0
		i2c_wo_fifo(0x40);
		i2c_wo_fifo(0x55 | STOP << 8);
		i2c_wo_start();
        ret = i2c_wo_status();
        if ((ret & 0x3) == 0) {
            // break;
        }
        systimer_delay(1, IN_S);
		i2c_wo_debug();
#else
		i2c4_test();
#endif
    }

    printf("i2c_wo status 0x%x\n", i2c_wo_status());
    printf("i2c_wo status fifo level 0x%x\n", i2c_wo_status_fifo_level());

    return 0;
}
