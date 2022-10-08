/*
 * MFD core driver for the X-Powers' Power Management ICs
 *
 * AXP20x typically comprises an adaptive USB-Compatible PWM charger, BUCK DC-DC
 * converters, LDOs, multiple 12-bit ADCs of voltage, current and temperature
 * as well as configurable GPIOs.
 *
 * This file contains the interface independent core functions.
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
#include "utils_def.h"

#define AXP20X_OFF	0x01

static const char *const axp20x_model_names[] = {
	"AXP152", "AXP202", "AXP209", "AXP221",  "AXP223",
	"AXP288", "AXP806", "AXP809", "AXP2101", "AXP15",
	"AXP1530", "AXP858", "AXP803", "AXP2202", "AXP2585",
};

static const struct regmap_range axp2101_writeable_ranges[] = {
	regmap_reg_range(AXP2101_COMM_STAT0, AXP2101_BUFFERC),
};

static const struct regmap_range axp2101_volatile_ranges[] = {
	regmap_reg_range(AXP2101_COMM_STAT0, AXP2101_BUFFERC),
};

static const struct regmap_access_table axp2101_writeable_table = {
	.yes_ranges	= axp2101_writeable_ranges,
	.n_yes_ranges	= ARRAY_SIZE(axp2101_writeable_ranges),
};

static const struct regmap_access_table axp2101_volatile_table = {
	.yes_ranges	= axp2101_volatile_ranges,
	.n_yes_ranges	= ARRAY_SIZE(axp2101_volatile_ranges),
};
/***********************/

static const struct regmap_config axp2101_regmap_config = {
	.reg_bits	= 8,
	.val_bits	= 8,
	.wr_table	= &axp2101_writeable_table,
	.volatile_table	= &axp2101_volatile_table,
	.max_register	= AXP2101_BUFFERC,
	.use_single_rw = true,
	.cache_type	= REGCACHE_RBTREE,
};
/******************************/
/*------------------*/
#define INIT_REGMAP_IRQ(_variant, _irq, _off, _mask)			\
	[_variant##_IRQ_##_irq] = { .reg_offset = (_off), .mask = BIT(_mask) }


static const struct regmap_irq axp2101_regmap_irqs[] = {
	INIT_REGMAP_IRQ(AXP2101, SOCWL2,		0, 7),
	INIT_REGMAP_IRQ(AXP2101, SOCWL1,		0, 6),
	INIT_REGMAP_IRQ(AXP2101, GWDT,			0, 5),
	INIT_REGMAP_IRQ(AXP2101, NEWSOC,		0, 4),
	INIT_REGMAP_IRQ(AXP2101, BCOT,			0, 3),
	INIT_REGMAP_IRQ(AXP2101, BCUT,			0, 2),
	INIT_REGMAP_IRQ(AXP2101, BWOT,			0, 1),
	INIT_REGMAP_IRQ(AXP2101, BWUT,			0, 0),
	INIT_REGMAP_IRQ(AXP2101, VINSET,		1, 7),
	INIT_REGMAP_IRQ(AXP2101, VREMOV,		1, 6),
	INIT_REGMAP_IRQ(AXP2101, BINSERT,		1, 5),
	INIT_REGMAP_IRQ(AXP2101, BREMOV,		1, 4),
	INIT_REGMAP_IRQ(AXP2101, PONS,			1, 3),
	INIT_REGMAP_IRQ(AXP2101, PONL,			1, 2),
	INIT_REGMAP_IRQ(AXP2101, PONN,			1, 1),
	INIT_REGMAP_IRQ(AXP2101, PONP,			1, 0),
	INIT_REGMAP_IRQ(AXP2101, WDEXP,			2, 7),
	INIT_REGMAP_IRQ(AXP2101, LDOOC,			2, 6),
	INIT_REGMAP_IRQ(AXP2101, BOCP,			2, 5),
	INIT_REGMAP_IRQ(AXP2101, CHGDN,			2, 4),
	INIT_REGMAP_IRQ(AXP2101, CHGST,			2, 3),
	INIT_REGMAP_IRQ(AXP2101, DOTL1,			2, 2),
	INIT_REGMAP_IRQ(AXP2101, CHGTE,			2, 1),
	INIT_REGMAP_IRQ(AXP2101, BOVP,			2, 0),
};
/********************************/
static const struct regmap_irq_chip axp2101_regmap_irq_chip = {
	.name			= "axp2101_irq_chip",
	.status_base		= AXP2101_INTSTS1,
	.ack_base		= AXP2101_INTSTS1,
	.mask_base		= AXP2101_INTEN1,
	.mask_invert		= true,
	.init_ack_masked	= true,
	.irqs			= axp2101_regmap_irqs,
	.num_irqs		= ARRAY_SIZE(axp2101_regmap_irqs),
	.num_regs		= 3,

};
/********************************/

static struct resource axp2101_power_supply_resources[] = {
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_BWUT, "bat untemp work"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_BWOT, "bat ovtemp work"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_BCUT, "bat untemp chg"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_BCOT, "bat ovtemp chg"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_NEWSOC, "CHG_NEWSOC"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_GWDT, "CHG_GWDT"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_SOCWL1, "low warning1"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_SOCWL2, "low warning2"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_BREMOV, "bat out"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_BINSERT, "bat in"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_VINSET, "usb in"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_VREMOV, "usb out"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_BOVP, "CHG_BOVP"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_CHGTE, "CHG_CHGTE"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_DOTL1, "CHG_DOTL1"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_CHGST, "charging"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_CHGDN, "charge over"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_BOCP, "CHG_BOCP"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_LDOOC, "CHG_LDOOC"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_WDEXP, "CHG_WDEXP"),
};

static struct resource axp2101_pek_resources[] = {
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_PONN, "PEK_DBF"),
	DEFINE_RES_IRQ_NAMED(AXP2101_IRQ_PONP, "PEK_DBR"),
};


#define AXP2101_DCDC1 "dcdc1"
#define AXP2101_DCDC2 "dcdc2"
#define AXP2101_DCDC3 "dcdc3"
#define AXP2101_DCDC4 "dcdc4"
#define AXP2101_DCDC5 "dcdc5"
#define AXP2101_ALDO1 "aldo1"
#define AXP2101_ALDO2 "aldo2"
#define AXP2101_ALDO3 "aldo3"
#define AXP2101_ALDO4 "aldo4"
#define AXP2101_BLDO1 "bldo1"
#define AXP2101_BLDO2 "bldo2"
#define AXP2101_DLDO1 "dldo1"
#define AXP2101_DLDO2 "dldo2"

static struct mfd_cell axp2101_cells[] = {
	/*{
.name		= "axp2101-gpio",
.of_compatible	= "x-powers,axp2101-gpio",
}, */
	{
		.name = "axp2101-pek",
		.num_resources = ARRAY_SIZE(axp2101_pek_resources),
		.resources = axp2101_pek_resources,
		.of_compatible = "x-powers,axp2101-pek",
	},
	{
		.name = "axp2101-regulator",
	},
	{
		.name = "axp2101-power-supply",
		.of_compatible = "x-powers,axp2101-power-supply",
		.num_resources = ARRAY_SIZE(axp2101_power_supply_resources),
		.resources = axp2101_power_supply_resources,
	},
	{
		.name = "axp2xx-watchdog",
		.of_compatible = "x-powers,axp2xx-watchdog",
	},
	{
		.of_compatible = "xpower-vregulator,dcdc1",
		.name = "reg-virt-consumer",
		.id = 1,
		.platform_data = AXP2101_DCDC1,
		.pdata_size = sizeof(AXP2101_DCDC1),

	},
	{
		.of_compatible = "xpower-vregulator,dcdc2",
		.name = "reg-virt-consumer",
		.id = 2,
		.platform_data = AXP2101_DCDC2,
		.pdata_size = sizeof(AXP2101_DCDC2),
	},
	{
		.of_compatible = "xpower-vregulator,dcdc3",
		.name = "reg-virt-consumer",
		.id = 3,
		.platform_data = AXP2101_DCDC3,
		.pdata_size = sizeof(AXP2101_DCDC3),
	},
	{
		.of_compatible = "xpower-vregulator,dcdc4",
		.name = "reg-virt-consumer",
		.id = 4,
		.platform_data = AXP2101_DCDC4,
		.pdata_size = sizeof(AXP2101_DCDC4),
	},
	{
		.of_compatible = "xpower-vregulator,dcdc5",
		.name = "reg-virt-consumer",
		.id = 5,
		.platform_data = AXP2101_DCDC5,
		.pdata_size = sizeof(AXP2101_DCDC5),
	},

	{
		.of_compatible = "xpower-vregulator,aldo1",
		.name = "reg-virt-consumer",
		.id = 8,
		.platform_data = AXP2101_ALDO1,
		.pdata_size = sizeof(AXP2101_ALDO1),
	},
	{
		.of_compatible = "xpower-vregulator,aldo2",
		.name = "reg-virt-consumer",
		.id = 9,
		.platform_data = AXP2101_ALDO2,
		.pdata_size = sizeof(AXP2101_ALDO2),
	},
	{
		.of_compatible = "xpower-vregulator,aldo3",
		.name = "reg-virt-consumer",
		.id = 10,
		.platform_data = AXP2101_ALDO3,
		.pdata_size = sizeof(AXP2101_ALDO3),
	},
	{
		.of_compatible = "xpower-vregulator,aldo4",
		.name = "reg-virt-consumer",
		.id = 11,
		.platform_data = AXP2101_ALDO4,
		.pdata_size = sizeof(AXP2101_ALDO4),
	},

	{
		.of_compatible = "xpower-vregulator,bldo1",
		.name = "reg-virt-consumer",
		.id = 12,
		.platform_data = AXP2101_BLDO1,
		.pdata_size = sizeof(AXP2101_BLDO1),
	},
	{
		.of_compatible = "xpower-vregulator,bldo2",
		.name = "reg-virt-consumer",
		.id = 13,
		.platform_data = AXP2101_BLDO2,
		.pdata_size = sizeof(AXP2101_BLDO2),
	},

	{
		.of_compatible = "xpower-vregulator,dldo1",
		.name = "reg-virt-consumer",
		.id = 14,
		.platform_data = AXP2101_DLDO1,
		.pdata_size = sizeof(AXP2101_DLDO1),
	},
	{
		.of_compatible = "xpower-vregulator,dldo2",
		.name = "reg-virt-consumer",
		.id = 15,
		.platform_data = AXP2101_DLDO2,
		.pdata_size = sizeof(AXP2101_DLDO2),
	},
};
/*----------------------*/
static struct axp20x_dev *axp20x_pm_power_off;
__nouse__ static void axp20x_power_off(void)
{
	if (axp20x_pm_power_off->variant == AXP2101_ID)
		axp20x_i2c_write(AXP2101_COMM_CFG, AXP20X_OFF);

	/* Give capacitors etc. time to drain to avoid kernel panic msg. */
	// msleep(500);
}

/*
 * axp2101_dts_parse - axp2101 device tree parse
 */
static void axp2101_dts_parse(struct axp20x_dev *axp20x)
{
#if 0
	struct device_node *node = axp20x->dev->of_node;
	struct regmap *map = axp20x->regmap;
	uint32_t val;

	if (of_property_read_bool(axp20x->dev->of_node, "pmu_powerok_noreset")) {
		regmap_update_bits(axp20x->regmap, AXP2101_COMM_CFG, BIT(3), 0);
	} else {
		regmap_update_bits(axp20x->regmap, AXP2101_COMM_CFG, BIT(3),
				   BIT(3));
	}

	/* init 16's reset pmu en */
	if (of_property_read_uint32_t(node, "pmu_reset", &val))
		val = 0;
	if (val) {
		regmap_update_bits(map, AXP2101_COMM_CFG, BIT(2), BIT(2));
	} else {
		regmap_update_bits(map, AXP2101_COMM_CFG, BIT(2), 0);
	}

	/* enable pwrok pin pull low to restart the system */
	regmap_update_bits(map, AXP2101_COMM_CFG, BIT(3), BIT(3));

	/* init pmu over temperature protection */
	if (of_property_read_u32(node, "pmu_hot_shutdown", &val))
		val = 0;
	if (val)
		regmap_update_bits(map, AXP2101_PWROFF_EN, BIT(2), BIT(2));
	else
		regmap_update_bits(map, AXP2101_PWROFF_EN, BIT(2), 0);

	/* 85% low voltage turn off pmic function */
	/* axp_regmap_write(axp2101->regmap, axp2101_DCDC_PWROFF_EN, 0x3f); */
	/* set 2.6v for battery poweroff */
	regmap_write(map, AXP2101_VOFF_THLD, 0x00);
	/* set delay of powerok after all power output good to 8ms */
	regmap_update_bits(map, AXP2101_PWR_TIME_CTRL, 0x03, 0x00);
#endif
}

int axp20x_match_device(struct axp20x_dev *axp20x)
{

	switch (axp20x->variant) {
	case AXP2101_ID:
		axp20x->nr_cells = ARRAY_SIZE(axp2101_cells);
		axp20x->cells = axp2101_cells;
		axp20x->regmap_cfg = &axp2101_regmap_config;
		axp20x->regmap_irq_chip = &axp2101_regmap_irq_chip;
		axp20x->dts_parse = axp2101_dts_parse;
		break;
	default:
		printf( "unsupported AXP20X ID %lu\n", axp20x->variant);
		return -1;
	}
	printf( "AXP20x variant %s found\n",
		 axp20x_model_names[axp20x->variant]);

	return 0;
}

static uint32_t axp_reg_addr;

__nouse__ static ssize_t axp_reg_show(char *buf)
{
	uint32_t val;

	axp20x_i2c_read(axp_reg_addr, &val);
	return sprintf(buf, "REG[0x%lx]=0x%lx\n",
				axp_reg_addr, val);
}

__nouse__ static ssize_t axp_reg_store(const char *buf, size_t count)
{
	int32_t tmp;
	uint32_t val;
	__nouse__ int err;

	tmp = *(int *)buf;

	if (tmp < 256) {
		axp_reg_addr = tmp;
	} else {
		val = tmp & 0x00FF;
		axp_reg_addr = (tmp >> 8) & 0x00FF;
		axp20x_i2c_write(axp_reg_addr, val);
	}

	return count;
}

int axp20x_device_probe(struct axp20x_dev *axp20x)
{
	__nouse__ int ret;

	/*
	 * on some board ex. qaqc test board, there's no interrupt for axp20x
	 */
	if (axp20x->irq) {
	}

	if (axp20x->dts_parse)
		axp20x->dts_parse(axp20x);

	axp20x_pm_power_off = axp20x;

	// if (!pm_power_off) {
		// pm_power_off = axp20x_power_off;
	// }
	axp20x_pek_probe(axp20x, NULL);

	axp2101_regulator_probe(axp20x, NULL);

	printf( "AXP20X driver loaded\n");

	return 0;
}

int axp20x_device_remove(struct axp20x_dev *axp20x)
{
	if (axp20x == axp20x_pm_power_off) {
		axp20x_pm_power_off = NULL;
		// pm_power_off = NULL;
	}

	return 0;
}
