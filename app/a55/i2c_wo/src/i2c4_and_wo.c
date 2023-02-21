#include <stdio.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
#include "i2c_wo.h"
#include "i2c.h"
#include "pinmux.h"

#define STOP             1
#define COUNT             20

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
	uint16_t reg = 0x26;
	uint32_t val = 0x55;
	uint32_t cnt = 0;

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
		if(cnt++ == COUNT)
			break;
	}
    printf("i2c4_test test cnt %d////////////////\n", COUNT);
	return;
}

int i2c_wo_test(void)
{
    uint32_t tmp;
	uint32_t cnt = 0;
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
	i2c_wo_appower_enable(0);

    i2c_wo_delay(1); // 1s

	while(1){
		i2c_wo_start();
		i2c_wo_fifo(0x26);
		i2c_wo_fifo(0x09 | STOP << 8);
		if(cnt++ == COUNT)
			break;
        systimer_delay(1, IN_MS);
	}
    printf("i2c_wo_test test cnt %d///////////////////////////\n", COUNT);

	return 0;
}

int main()
{
    printf("test %s ...\n", __FILE__);
    systimer_init();

    while (1) {
		i2c4_test();
		i2c_wo_test();
		i2c_wo_debug();
        systimer_delay(100, IN_MS);
    }

    return 0;
}
