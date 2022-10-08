#ifndef __I2C_SLAVE_H__
#define __I2C_SLAVE_H__
#ifdef SUPPORT_I2C_SLAVE
// #include <FreeRTOS.h>

// typedef struct {
// 	int (*cmd_init)(uint32_t reg_addr);
// 	int (*respond_write)(uint8_t var, bool is_last);
// 	int (*respond_read)(uint8_t *const val);
// } i2c_slave_call_proto;

// int i2c_irq_handler_slave(void);
// void i2c_slave_init(int hwadapnr, uint16_t devaddr, i2c_slave_call_proto *call_func);
// void i2c_slave_deinit(void);
// int i2c_slave_set_bus_speed(unsigned speed);
int i2c_dw_irq_handler_slave(struct i2c_adapter *i2c_ad);
#endif
#endif
