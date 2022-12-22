#include <stdio.h>
#include <assert.h>
#include "chip_mem_layout.h"
#include "regs_type.h"
#include "i2c_wo_regs.h"
#include "i2c_wo.h"

#define I2C_WO_CLK 24000000

void i2c_wo_init(uint8_t slave)
{
    assert(slave);

    I2C_WO->FIFO_SOFT_RST = 0x1;
    I2C_WO->FIFO_SOFT_RST = 0x0;
    I2C_WO->DEVICE_ADDR   = slave;
    I2C_WO->SCL_HCNT      = 0x78;
    I2C_WO->SCL_LCNT      = 0x78;
}

void i2c_wo_delay(uint32_t ms)
{
    assert(ms > 0);

    uint32_t step = I2C_WO_CLK / 1000;

    I2C_WO->DELAY = ms * step;
}

void i2c_wo_start(void) { I2C_WO->START = 0x1; }

void i2c_wo_fifo(uint32_t data) { I2C_WO->TX_DATA = data; }

uint32_t i2c_wo_status(void) { return (I2C_WO->STATUS); }

uint32_t i2c_wo_status_fifo_level(void) { return (I2C_WO->STATUS_FIFO_LEVEL); }
