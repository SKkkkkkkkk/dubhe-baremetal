/*
 * I2C driver for the X-Powers' Power Management ICs
 *
 * AXP20x typically comprises an adaptive USB-Compatible PWM charger, BUCK DC-DC
 * converters, LDOs, multiple 12-bit ADCs of voltage, current and temperature
 * as well as configurable GPIOs.
 *
 * This driver supports the I2C variants.
 *
 * Copyright (C) 2014 Carlo Caione
 *
 * Author: Carlo Caione <carlo@caione.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "axp2101.h"


struct i2c_driver axp2101_i2c_driver;
static void axp2101_i2c_exit(void);

struct i2c_driver {
	char *name;
	struct of_device_id *of_match_table;
	int (*probe)(void *, void *);
	int (*remove)(void *);
	struct of_device_id *id_table;
};


static struct of_device_id axp20x_i2c_of_match[] = {
	{ .compatible = "x-powers,axp152", .data = (void *)AXP152_ID },
	{ .compatible = "x-powers,axp202", .data = (void *)AXP202_ID },
	{ .compatible = "x-powers,axp209", .data = (void *)AXP209_ID },
	{ .compatible = "x-powers,axp221", .data = (void *)AXP221_ID },
	{ .compatible = "x-powers,axp2101", .data = (void *)AXP2101_ID },
	{ .compatible = "x-powers,axp15", .data = (void *)AXP15_ID },
	{ .compatible = "x-powers,axp1530", .data = (void *)AXP1530_ID },
	{ .compatible = "x-powers,axp858", .data = (void *)AXP858_ID },
	{ .compatible = "x-powers,axp803", .data = (void *)AXP803_ID },
	{ .compatible = "x-powers,axp2202", .data = (void *)AXP2202_ID },
	{ .compatible = "x-powers,axp806", .data = (void *)AXP806_ID },
	{ .compatible = "x-powers,axp2585", .data = (void *)AXP2585_ID },
	{ },
};

#if 0
__nouse__ static int i2c_write_reg(struct sensor_cfg *cfg, uint16_t reg, uint32_t len, uint32_t val)
{
	i2c_msg msgs;
	int ret = 0;
	uint8_t tmp8 = 0;
	uint8_t tmp[2];

	msgs.addr = cfg->reg_addr;
	msgs.flags = RT_I2C_WR;
	msgs.reg_addr = reg;
	msgs.alen = 1;
	if (len == 1) {
		tmp8 = (uint8_t)val;
		msgs.buf = &tmp8;
	} else if (len == 2) {
		tmp[0] = (uint8_t)val >> 8;
		tmp[1] = (uint8_t)val;
		msgs.buf = tmp;
	} else {
		printf("i2c write unkonw i2c len");
	}
	msgs.buf_len = len;
	ret = i2c_master_transfer(cfg->i2c_bus, &msgs, 1);
	if (ret != 0) {
		printf("i2c_write_reg err");
		return -1;
	}

	return 0;
}

__nouse__ static int i2c_read_reg(struct sensor_cfg *cfg, uint16_t reg, unsigned int len, uint32_t *val)
{
	i2c_msg msgs;
	int ret = 0;
	uint8_t tmp[2];

	msgs.addr = cfg->reg_addr;
	msgs.flags = RT_I2C_RD;
	msgs.reg_addr = reg;
	msgs.alen = 1;
	msgs.buf = tmp;
	msgs.buf_len = len;
	ret = i2c_master_transfer(cfg->i2c_bus, &msgs, 1);
	if (ret != 0) {
		printf("i2c_read_reg err");
		return -1;
	}

	if (len == 1) {
		*val = tmp[0];
	} else if (len == 2) {
		*val = (uint16_t)(tmp[0] << 8 | tmp[1]);
	} else {
		printf("i2c read unkonw i2c len");
	}

	return 0;
}
#endif

__nouse__ static int axp20x_i2c_probe(void *i2c,
			    void *id)
{
	struct axp20x_dev *axp20x;
	__nouse__ int ret;

	axp20x = malloc(sizeof(struct axp20x_dev));
	if (!axp20x)
		return -1;

	axp20x->variant = (long)axp2101_i2c_driver.id_table->data;

	ret = axp20x_match_device(axp20x);
	if (ret)
		return ret;

	return axp20x_device_probe(axp20x);
}

__nouse__ static int axp20x_i2c_remove(void *i2c)
{

	axp2101_i2c_exit();
	return 0;
}

__nouse__ static int axp2101_i2c_init(void)
{
	// int err = 0;
	// struct sensor_cfg *cfg;

	// err = i2c_master_init(cfg->i2c_bus);
	// if (err) {
		// printf("i2c_master_init err");
		// return err;
	// }

    // i2c_control control = {0};
    // control.speed = 400000;
    // err = i2c_config_control(cfg->i2c_bus, &control);
    // if (err != 0) {
        // printf("i2c_config_control failed, ret=%d\r\n", err);
        // return -1;
    // }

	return 0;
}

__nouse__ static void axp2101_i2c_exit(void)
{
	// i2c_master_deinit(cfg->i2c_bus);
}

__nouse__  struct i2c_driver axp2101_i2c_driver = {
	.name	= "axp20x-i2c",
	.of_match_table	= axp20x_i2c_of_match,
	.probe		= axp20x_i2c_probe,
	.remove		= axp20x_i2c_remove,
	.id_table   = &axp20x_i2c_of_match[4]
};

