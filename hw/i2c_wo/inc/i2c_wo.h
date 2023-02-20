#ifndef __I2C_WO_H__
#define __I2C_WO_H__

void i2c_wo_init(uint8_t slave);
void i2c_wo_delay(uint32_t ms);
void i2c_wo_start(void);
void i2c_wo_fifo(uint32_t data);
uint32_t i2c_wo_status(void);
uint32_t i2c_wo_status_fifo_level(void);
void i2c_wo_delay_unit(uint32_t unit);
void i2c_wo_appower_enable(uint8_t enable);
void i2c_wo_debug(void);

#endif // __I2C_WO_H__
