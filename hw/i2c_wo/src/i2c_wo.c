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

    I2C_WO->FIFO_SOFT_RST = 0x0;
    I2C_WO->FIFO_SOFT_RST = 0x1;
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

void i2c_wo_delay_unit(uint32_t unit)
{
    if (unit >= 0xfffff) unit = 0xfffff;

    I2C_WO->DELAY_UNIT = unit;
}

void i2c_wo_appower_enable(uint8_t enable)
{
    assert(enable < 2);

    I2C_WO->APPOWER_STATUS_DISABLE = enable;
}

uint32_t i2c_wo_status(void) { return (I2C_WO->STATUS); }

uint32_t i2c_wo_status_fifo_level(void) { return (I2C_WO->STATUS_FIFO_LEVEL); }

void i2c_wo_debug(void)
{
    printf("I2C_WO->DEVICE_ADDR 0x%x\n", I2C_WO->DEVICE_ADDR);
    printf("I2C_WO->STATUS 0x%x\n", I2C_WO->STATUS);
    printf("I2C_WO->STATUS_FIFO_LEVEL 0x%x\n", I2C_WO->STATUS_FIFO_LEVEL);
    printf("I2C_WO->DELAY_UNIT 0x%x\n", I2C_WO->DELAY_UNIT);
    printf("I2C_WO->APPOWER_STATUS_DISABLE 0x%x\n",
           I2C_WO->APPOWER_STATUS_DISABLE);
	printf("I2C_WO->DELAY 0x%x\n", I2C_WO->DELAY);
}
