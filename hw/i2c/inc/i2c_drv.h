#ifndef _I2C_DRV_H_
#define _I2C_DRV_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
struct i2c_adapter {
	void (*init)(struct i2c_adapter *adap);
	int (*probe)(struct i2c_adapter *adap, uint16_t chip);
	int (*read)(struct i2c_adapter *adap, uint16_t chip, uint32_t addr, int alen, uint8_t *buffer, int len);
	int (*write)(struct i2c_adapter *adap, uint16_t chip, uint32_t addr, int alen, uint8_t *buffer, int len);
	uint32_t (*set_bus_speed)(struct i2c_adapter *adap, uint32_t speed);
	int (*irq_handle)(struct i2c_adapter *adap);
	uint32_t irq_num;
	int speed;
	int waitdelay;
	uint16_t slaveaddr;
	int init_done;
	int hwadapnr;
	char *name;
	uint8_t reg_addr; // lambda
	uint8_t reg_val; // lambda
	int one_adata_len; /* reg_addr 单个地址有多少长度的数据 */
	int dma_use;
	bool addr_10bit;
	int timeout;
	int fifo_tl;
	bool is_slave;
};
struct i2c_adapter *ape_i2c_init(int busindex);
int ape_i2c_uninit(struct i2c_adapter *i2c_ad);

#ifdef __cplusplus
}
#endif
#endif