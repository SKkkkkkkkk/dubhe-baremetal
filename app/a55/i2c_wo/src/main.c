#include <stdio.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
#include "i2c_wo.h"
#include "pinmux.h"

int main()
{
    int ret;
    printf("test %s ...\n", __FILE__);
    systimer_init();

    pinmux(84, 3);
    pinmux(85, 3);

    i2c_wo_init(0x34); // AXP2101_SLAVE_ADDR
    i2c_wo_delay(500);
    i2c_wo_fifo(0x26); // AXP2101_SLEEP_CFG
    i2c_wo_fifo(0x101);
    i2c_wo_fifo(0x80); // AXP2101_DCDC_CFG0
    i2c_wo_fifo(0x11f);
    i2c_wo_fifo(0x90); // AXP2101_LDO_EN_CFG0
    i2c_wo_fifo(0x1ff);
    i2c_wo_fifo(0x91); // AXP2101_LDO_EN_CFG1
    i2c_wo_fifo(0x101);
    i2c_wo_fifo(0x26); // AXP2101_SLEEP_CFG
    i2c_wo_fifo(0x110);
    i2c_wo_fifo(0x41); // AXP2101_INTEN2
    i2c_wo_fifo(0x10c);
    i2c_wo_start();

    while (1) {
        ret = i2c_wo_status();
        if ((ret & 0x3) == 0) {
            break;
        }
        systimer_delay(1, IN_MS);
    }

    printf("i2c_wo status 0x%x\n", i2c_wo_status());
    printf("i2c_wo status fifo level 0x%x\n", i2c_wo_status_fifo_level());

    return 0;
}
