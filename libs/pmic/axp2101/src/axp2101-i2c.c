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


extern __nouse__  struct i2c_driver axp2101_i2c_driver;

struct i2c_driver {
	char *name;
	struct of_device_id *of_match_table;
	int (*probe)(void *, void *);
	int (*remove)(void *);
	struct of_device_id *id_table;
};

struct of_device_id {
	char *compatible;
	void *data;
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

	// return axp20x_device_probe(axp20x);
	return 0;
}

__nouse__ static int axp20x_i2c_remove(void *i2c)
{

	// return axp20x_device_remove(axp20x);
	return 0;
}

__nouse__ static int axp2101_i2c_init(void)
{
	__nouse__ int ret = 1;

	// ret = i2c_add_driver(&axp2101_i2c_driver);
	if (ret != 0) {
		printf("axp2101 i2c registration failed %d\n", ret);
		return ret;
	}

	return 0;
}

__nouse__ static void axp2101_i2c_exit(void)
{
	// i2c_del_driver(&axp2101_i2c_driver);
}

__nouse__  struct i2c_driver axp2101_i2c_driver = {
	.name	= "axp20x-i2c",
	.of_match_table	= axp20x_i2c_of_match,
	.probe		= axp20x_i2c_probe,
	.remove		= axp20x_i2c_remove,
	.id_table   = &axp20x_i2c_of_match[4]
};

