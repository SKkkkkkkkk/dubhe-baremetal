#include <stdio.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
#include "i2c_wo.h"
#include "pinmux.h"

#define STOP             1
#define DELAY             1

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
	i2c_wo_fifo(0x40);
	i2c_wo_fifo(0x00 | STOP << 8);
	i2c_wo_fifo(0x01 | DELAY << 9);
	i2c_wo_fifo(0x41);
	i2c_wo_fifo(0x00 | STOP << 8);
	i2c_wo_fifo(0x01 | DELAY << 9);
	i2c_wo_fifo(0x42);
	i2c_wo_fifo(0x00 | STOP << 8);
	i2c_wo_fifo(0x01 | DELAY << 9);
	i2c_wo_fifo(0x26);
	i2c_wo_fifo(0x09 | STOP << 8);
	i2c_wo_fifo(0x01 | DELAY << 9);
	i2c_wo_fifo(0x80);
	i2c_wo_fifo(0x00 | STOP << 8);
	i2c_wo_fifo(0x01 | DELAY << 9);
	i2c_wo_fifo(0x90);
	i2c_wo_fifo(0x00 | STOP << 8);
	i2c_wo_fifo(0x01 | DELAY << 9);
	i2c_wo_fifo(0x91);
	i2c_wo_fifo(0x00 | STOP << 8);
	i2c_wo_fifo(0x01 | DELAY << 9);
	i2c_wo_fifo(0x26);
	i2c_wo_fifo(0x19 | STOP << 8);
	i2c_wo_fifo(0x01 | DELAY << 9);
	i2c_wo_fifo(0x41);
	i2c_wo_fifo(0x0c | STOP << 8);

    printf("axp2101_powerkey_suspend !!! \n");
	return 0;
}

int main()
{
	int ret;
    printf("test %s ...\n", __FILE__);
    systimer_init();

	pmic_to_sleep_delay(5);
	i2c_wo_appower_enable(0);
	i2c_wo_start();

    while (1) {
        ret = i2c_wo_status();
        if ((ret & 0x3) == 0) {
			break;
        }
        systimer_delay(1, IN_S);
		i2c_wo_debug();
    }

    printf("i2c_wo status 0x%x\n", i2c_wo_status());
    printf("i2c_wo status fifo level 0x%x\n", i2c_wo_status_fifo_level());

    return 0;
}
