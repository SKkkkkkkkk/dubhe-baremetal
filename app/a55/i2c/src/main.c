#ifndef QEMU
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "systimer.h"
#include "i2c.h"
// #include "systimer.h"
// #ifdef TEST_HDMI
#define writel(d, a) *(volatile uint32_t *)(a) = (d)
#define readl(a) (*(volatile uint32_t *)(a))

#define parem_invalid() printf("%s[%d] error: param Invalid !\n", __FUNCTION__, __LINE__)
#define pointer_invalid(ptr) printf("%s[%d] error: %s is null pointer !\n", __FUNCTION__, __LINE__, #ptr)
#define function_err(func) printf("%s[%d] error: %s faild !\n", __FUNCTION__, __LINE__, #func)
#define check_init(init) do {                                                                                                           \
		if ((init) == false) {                                                                                 \
			printf("%s[%d] error: i2c is not init !\n", __FUNCTION__, __LINE__);                     \
			return -1;                                                                                     \
		}                                                                                                      \
	} while (0)

#define ADV7511_CFG_DATA {                                                                                                              \
		0x00, 0x18, 0x00, 0x00, 0x61, 0x46, 0x80, 0x10, 0x48, 0xa8, 0x06, 0x00, 0x08, 0x20, 0x03, 0x30, 0x61,  \
		0xa4, 0xa4, 0x16, 0x60, 0x9c, 0x60, 0x7d                                                       \
	}

#define ADV7511_I2C_CONFIG(buf) {                                                                                                              \
		{ 0x39, RT_I2C_WR, 0x01, 1, &buf[0], 1 }, { 0x39, RT_I2C_WR, 0x02, 1, &buf[1], 1 },                    \
		{ 0x39, RT_I2C_WR, 0x03, 1, &buf[2], 1 }, { 0x39, RT_I2C_WR, 0x15, 1, &buf[3], 1 },            \
		{ 0x39, RT_I2C_WR, 0x16, 1, &buf[4], 1 }, { 0x39, RT_I2C_WR, 0x18, 1, &buf[5], 1 },            \
		{ 0x39, RT_I2C_WR, 0x40, 1, &buf[6], 1 }, { 0x39, RT_I2C_WR, 0x41, 1, &buf[7], 1 },            \
		{ 0x39, RT_I2C_WR, 0x48, 1, &buf[8], 1 }, { 0x39, RT_I2C_WR, 0x48, 1, &buf[9], 1 },            \
		{ 0x39, RT_I2C_WR, 0x4c, 1, &buf[10], 1 }, { 0x39, RT_I2C_WR, 0x55, 1, &buf[11], 1 },          \
		{ 0x39, RT_I2C_WR, 0x55, 1, &buf[12], 1 }, { 0x39, RT_I2C_WR, 0x96, 1, &buf[13], 1 },          \
		{ 0x39, RT_I2C_WR, 0x98, 1, &buf[14], 1 }, { 0x39, RT_I2C_WR, 0x9c, 1, &buf[15], 1 },          \
		{ 0x39, RT_I2C_WR, 0x9d, 1, &buf[16], 1 }, { 0x39, RT_I2C_WR, 0xa2, 1, &buf[17], 1 },          \
		{ 0x39, RT_I2C_WR, 0x43, 1, &buf[18], 1 }, { 0x39, RT_I2C_WR, 0xaf, 1, &buf[19], 1 },          \
		{ 0x39, RT_I2C_WR, 0xba, 1, &buf[20], 1 }, { 0x39, RT_I2C_WR, 0xde, 1, &buf[21], 1 },          \
		{ 0x39, RT_I2C_WR, 0xe4, 1, &buf[22], 1 }, { 0x39, RT_I2C_WR, 0xfa, 1, &buf[23], 1 },          \
	}

#define ADV7511_I2C_GET(buf) {                                                                                                               \
		{ 0x39, RT_I2C_RD, 0x01, 1, &buf[0], 1 }, { 0x39, RT_I2C_RD, 0x02, 1, &buf[1], 1 },                    \
		{ 0x39, RT_I2C_RD, 0x03, 1, &buf[2], 1 }, { 0x39, RT_I2C_RD, 0x15, 1, &buf[3], 1 },            \
		{ 0x39, RT_I2C_RD, 0x16, 1, &buf[4], 1 }, { 0x39, RT_I2C_RD, 0x18, 1, &buf[5], 1 },            \
		{ 0x39, RT_I2C_RD, 0x40, 1, &buf[6], 1 }, { 0x39, RT_I2C_RD, 0x41, 1, &buf[7], 1 },            \
		{ 0x39, RT_I2C_RD, 0x48, 1, &buf[8], 1 }, { 0x39, RT_I2C_RD, 0x48, 1, &buf[9], 1 },            \
		{ 0x39, RT_I2C_RD, 0x4c, 1, &buf[10], 1 }, { 0x39, RT_I2C_RD, 0x55, 1, &buf[11], 1 },          \
		{ 0x39, RT_I2C_RD, 0x55, 1, &buf[12], 1 }, { 0x39, RT_I2C_RD, 0x96, 1, &buf[13], 1 },          \
		{ 0x39, RT_I2C_RD, 0x98, 1, &buf[14], 1 }, { 0x39, RT_I2C_RD, 0x9c, 1, &buf[15], 1 },          \
		{ 0x39, RT_I2C_RD, 0x9d, 1, &buf[16], 1 }, { 0x39, RT_I2C_RD, 0xa2, 1, &buf[17], 1 },          \
		{ 0x39, RT_I2C_RD, 0x43, 1, &buf[18], 1 }, { 0x39, RT_I2C_RD, 0xaf, 1, &buf[19], 1 },          \
		{ 0x39, RT_I2C_RD, 0xba, 1, &buf[20], 1 }, { 0x39, RT_I2C_RD, 0xde, 1, &buf[21], 1 },          \
		{ 0x39, RT_I2C_RD, 0xe4, 1, &buf[22], 1 }, { 0x39, RT_I2C_RD, 0xfa, 1, &buf[23], 1 },          \
	}

#define ADV7511_ADDR 0x3c

static int adv7511_init(void)
{
	int ret;
	int i2cbus = 2;
	printf("i2c test ====> \n");
	writel(0x05, 0x2c050304);

	printf("i2c init ====> \n");
	ret = i2c_master_init(i2cbus);
	if (ret != 0) {
		function_err(i2c_master_init);
		return -1;
	}

	/*
    i2c_control i2c_ctl = {
        .speed = 400000,
    };

    ret = i2c_config_control(i2cbus, ADV7511_ADDR, &i2c_ctl);
    if (ret != 0) {
        function_err(i2c_config_control);
        return -1;
    }
 */

	// ret = i2c_master_probe(i2cbus, ADV7511_ADDR);
	// if (ret != 0) {
	//     function_err(i2c_master_probe);
	//     i2c_master_deinit(i2cbus);
	//     return -1;
	// }

	uint8_t buf[] = ADV7511_CFG_DATA;
	uint8_t rd_buf[24] = { 0 };
	i2c_msg msgs[] = ADV7511_I2C_CONFIG(buf);
	// uint8_t buf[3] = {1};

	i2c_msg msgs_r[] = ADV7511_I2C_GET(rd_buf);
	// i2c_msg msgs[] = {0x36, RT_I2C_RD, 0x300a, 2, buf, 3};
	// i2c_msg msgs[] = {0x36, RT_I2C_WR, 0x0100, 2, buf, 1};
	int i = 0;
	//  while (1) {
	ret = i2c_master_transfer(i2cbus, msgs, sizeof(msgs) / sizeof(i2c_msg));
	if (ret != 0) {
		function_err(i2c_master_transfer);
		return -1;
	}

	ret = i2c_master_transfer(i2cbus, msgs_r, sizeof(msgs_r) / sizeof(i2c_msg));
	if (ret != 0) {
		function_err(i2c_master_transfer);
		return -1;
	}
	// printf("sensor read 0x300a:0x");
	// for (i = 0; i < 3; i++) {
	//     printf("%02x", buf[i]);
	// }
	// printf("\n");
	// i2c_master_send(i2cbus, &msgs[0]);
	for (i = 0; i < 24; i++) {
		printf("0x%02x: 0x%02x  \n", msgs_r[i].reg_addr, rd_buf[i]);
	}
	//  }

	i2c_master_deinit(i2cbus);
	return 0;
}
// #endif

int main()
{
	printf(" ****************************** \n");
	systimer_init();
	printf(" ****************************** \n");

	int ret = adv7511_init();
	while (1) {
		/* code */
	}return ret;
}

#else
#include "main_qemu.c"
#endif