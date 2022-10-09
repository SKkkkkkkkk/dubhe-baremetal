#ifndef _I2C_H_
#define _I2C_H_
#include <stdbool.h>
#include <stdint.h>

#define RT_I2C_WR 0x0000
#define RT_I2C_RD (1u << 0)
#define RT_I2C_ADDR_10BIT (1u << 2) /* this is a ten bit chip address */
#define RT_I2C_NO_START (1u << 4)
#define RT_I2C_IGNORE_NACK (1u << 5)
#define RT_I2C_NO_READ_ACK (1u << 6) /* when I2C reading, we do not ACK */
#define RT_I2C_NO_STOP (1u << 7)

/* i2c 读写数据消息结构 */
typedef struct {
	uint16_t addr; /* i2c 从设备地址 */
	uint16_t flags; /* 读写标志 RT_I2C_WR or RT_I2C_RD */
	uint16_t reg_addr; /* i2c 从设备寄存器地址 */
	uint16_t alen; /* i2c 从设备寄存器地址长度 */
	uint8_t *buf; /* 数据buffer */
	uint16_t buf_len; /* 数据buffer 大小 */
} i2c_msg;

typedef struct {
	uint32_t slaveaddr;
	int speed; /* i2c 速率 */
	int one_adata_len; /* reg_addr 单个地址有多少长度的数据 */
	bool addr_10bit; /* 是否使用10bit地址 */
	int dma_use;
	int timeout;
	int fifo_tl; /* fifo 深度 */
} i2c_control;

/**
 * @brief 初始化某一i2c
 * @param i2cbus i2c0 ~ i2c4
 * @return 0: success; other: failed
 */
int i2c_master_init(int i2cbus);

/**
 * @brief 配置/修改i2c控制参数信息
 * @param i2cbus i2c0 ~ i2c4
 * @param slaveaddr i2c 从设备地址
 * @param control i2c 控制参数信息
 * @return 0: success; other: failed
 */
int i2c_config_control(int i2cbus, i2c_control *control);

/**
 * @brief 探测某一 i2c slave 设备信息
 * @param i2cbus i2c0 ~ i2c4
 * @param slaveaddr i2c 从设备地址
 * @return 0: success; other: failed
 */
int i2c_master_probe(int i2cbus, uint16_t slaveaddr);

/**
 * @brief i2c master 读/写 操作
 * @param i2cbus i2c0 ~ i2c4
 * @param msgs i2c 消息指针
 * @param num 消息个数
 * @return 0: success; other: failed
 */
int i2c_master_transfer(int i2cbus, i2c_msg *msgs, int num);

/**
 * @brief i2c master 接收 slave 数据
 * @param i2cbus i2c0 ~ i2c4
 * @param msgs i2c 消息指针
 * @return 0: success; other: failed
 */
int i2c_master_recv(int i2cbus, i2c_msg *msg);

/**
 * @brief i2c master 向 slave 发送数据
 * @param i2cbus i2c0 ~ i2c4
 * @param msgs i2c 消息指针
 * @return 0: success; other: failed
 */
int i2c_master_send(int i2cbus, i2c_msg *msg);

/**
 * @brief 去初始化某一 i2c
 * @param i2cbus i2c0 ~ i2c4
 * @return 0: success; other: failed
 */
void i2c_master_deinit(int i2cbus);

#ifdef SUPPORT_I2C_SLAVE
/**
 * @brief 初始化某一i2c
 * @param i2cbus i2c0 ~ i2c4
 * @param slaveaddr 设置 slave 设备地址
 * @return 0: success; other: failed
 */
int i2c_slave_init(int i2cbus, uint32_t slaveaddr);
#endif
#endif
