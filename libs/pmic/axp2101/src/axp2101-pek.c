/*
 * axp20x power button driver.
 *
 * Copyright (C) 2013 Carlo Caione <carlo@caione.org>
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License. See the file "COPYING" in the main directory of this
 * archive for more details.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
// #define pr_fmt(x) KBUILD_MODNAME ": " x

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "axp2101.h"


#define AXP20X_PEK_STARTUP_MASK		(0x03)
#define AXP20X_PEK_SHUTDOWN_MASK	(0x0c)

#define axp2101_PONLEVEL        (0x27)
#define axp2101_PWROFF_EN       (0x22)
#define axp2101_PWR_TIME_CTRL   (0x25)
#define axp2101_VBAT_H          (0x34)
#define axp2101_INTSTS2         (0x49)

struct pk_dts {
	uint32_t pmu_powkey_off_time;
	uint32_t pmu_powkey_off_func;
	uint32_t pmu_powkey_off_en;
	uint32_t pmu_powkey_off_delay_time;
	uint32_t pmu_powkey_long_time;
	uint32_t pmu_powkey_on_time;
	uint32_t pmu_pwrok_time;
	uint32_t pmu_pwrnoe_time;
	uint32_t pmu_powkey_wakeup_rising;
	uint32_t pmu_powkey_wakeup_falling;
};

struct axp20x_pek {
	struct axp20x_dev *axp20x;
	struct pk_dts pk_dts;
	int irq_dbr;
	int irq_dbf;
};

struct axp20x_time {
	unsigned int time;
	unsigned int idx;
};

#if 0
static const struct axp20x_time startup_time[] = {
	{ .time = 128,  .idx = 0 },
	{ .time = 512,  .idx = 1 },
	{ .time = 1000, .idx = 2 },
	{ .time = 2000, .idx = 3 },
};

static const struct axp20x_time shutdown_time[] = {
	{ .time = 4000,  .idx = 0 },
	{ .time = 6000,  .idx = 1 },
	{ .time = 8000,  .idx = 2 },
	{ .time = 10000, .idx = 3 },
};
#endif

static int axp_powerkey_dt_parse(struct pk_dts *pk_dts)
{

	pk_dts->pmu_powkey_off_time       = 6000;
	pk_dts->pmu_powkey_off_func       = 0;
	pk_dts->pmu_powkey_off_en         = 1;
	pk_dts->pmu_powkey_off_delay_time = 0;
	pk_dts->pmu_powkey_long_time      = 1500;
	pk_dts->pmu_powkey_on_time        = 1000;
	pk_dts->pmu_pwrok_time            = 64;
	pk_dts->pmu_pwrnoe_time           = 2000;

	pk_dts->pmu_powkey_wakeup_rising = 0;
	pk_dts->pmu_powkey_wakeup_falling = 0;

	return 0;
}

#if 0
struct axp20x_pek_ext_attr {
	const struct axp20x_time *p_time;
	unsigned int mask;
};

static struct axp20x_pek_ext_attr axp20x_pek_startup_ext_attr = {
	.p_time	= startup_time,
	.mask	= AXP20X_PEK_STARTUP_MASK,
};

static struct axp20x_pek_ext_attr axp20x_pek_shutdown_ext_attr = {
	.p_time	= shutdown_time,
	.mask	= AXP20X_PEK_SHUTDOWN_MASK,
};

static ssize_t axp20x_show_ext_attr(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct axp20x_pek *axp20x_pek = dev_get_drvdata(dev);
	struct axp20x_pek_ext_attr *axp20x_ea = get_axp_ext_attr(attr);
	unsigned int val;
	int ret, i;

	ret = regmap_read(axp20x_pek->axp20x->regmap, axp2101_PONLEVEL, &val);
	if (ret != 0)
		return ret;

	val &= axp20x_ea->mask;
	val >>= ffs(axp20x_ea->mask) - 1;

	for (i = 0; i < 4; i++)
		if (val == axp20x_ea->p_time[i].idx)
			val = axp20x_ea->p_time[i].time;

	return sprintf(buf, "%u\n", val);
}

static ssize_t axp20x_store_ext_attr(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t count)
{
	struct axp20x_pek *axp20x_pek = dev_get_drvdata(dev);
	struct axp20x_pek_ext_attr *axp20x_ea = get_axp_ext_attr(attr);
	char val_str[20];
	size_t len;
	int ret, i;
	unsigned int val, idx = 0;
	unsigned int best_err = UINT_MAX;

	val_str[sizeof(val_str) - 1] = '\0';
	strncpy(val_str, buf, sizeof(val_str) - 1);
	len = strlen(val_str);

	if (len && val_str[len - 1] == '\n')
		val_str[len - 1] = '\0';

	ret = kstrtouint(val_str, 10, &val);
	if (ret)
		return ret;

	for (i = 3; i >= 0; i--) {
		unsigned int err;

		err = abs(axp20x_ea->p_time[i].time - val);
		if (err < best_err) {
			best_err = err;
			idx = axp20x_ea->p_time[i].idx;
		}

		if (!err)
			break;
	}

	idx <<= ffs(axp20x_ea->mask) - 1;
	ret = regmap_update_bits(axp20x_pek->axp20x->regmap,
				 axp2101_PONLEVEL,
				 axp20x_ea->mask, idx);
	if (ret != 0)
		return -1;

	return count;
}
#endif

__nouse__ static int axp20x_pek_irq(int irq, void *pwr)
{
#if 0
	struct input_dev *idev = pwr;
	struct axp20x_pek *axp20x_pek = input_get_drvdata(idev);

	/*
	 * The power-button is connected to ground so a falling edge (dbf)
	 * means it is pressed.
	 */
	if (irq == axp20x_pek->irq_dbf)
		input_report_key(idev, KEY_POWER, true);
	else if (irq == axp20x_pek->irq_dbr)
		input_report_key(idev, KEY_POWER, false);

	input_sync(idev);
#endif

	return 0;
}

#if 0
static int axp2202_config_set(struct axp20x_pek *axp20x_pek)
{
	struct axp20x_dev *axp20x_dev = axp20x_pek->axp20x;
	struct regmap *regmap = axp20x_dev->regmap;
	struct pk_dts *pk_dts = &axp20x_pek->pk_dts;
	unsigned int val;

	regmap_read(regmap, AXP2202_PONLEVEL, &val);
	if (pk_dts->pmu_powkey_on_time < 128)
		val &= 0x3C;
	else if (pk_dts->pmu_powkey_on_time < 512) {
		val &= 0x3C;
		val |= 0x01;
	} else if (pk_dts->pmu_powkey_on_time < 1000) {
		val &= 0x3C;
		val |= 0x10;
	} else {
		val &= 0x3C;
		val |= 0x11;
	}
	regmap_write(regmap, AXP2202_PONLEVEL, val);

	/* pok long time set*/
	if (pk_dts->pmu_powkey_long_time < 1000)
		pk_dts->pmu_powkey_long_time = 1000;

	if (pk_dts->pmu_powkey_long_time > 2500)
		pk_dts->pmu_powkey_long_time = 2500;

	regmap_read(regmap, AXP2202_PONLEVEL, &val);
	val &= 0xcf;
	val |= (((pk_dts->pmu_powkey_long_time - 1000) / 500)
		<< 4);
	regmap_write(regmap, AXP2202_PONLEVEL, val);

	/* pek offlevel poweroff en set*/
	if (pk_dts->pmu_powkey_off_en)
		pk_dts->pmu_powkey_off_en = 1;
	else
		pk_dts->pmu_powkey_off_en = 0;

	regmap_read(regmap, AXP2202_PWROFF_EN, &val);
	val &= 0xfD;
	val |= (pk_dts->pmu_powkey_off_en << 1);
	regmap_write(regmap, AXP2202_PWROFF_EN, val);

	/*Init offlevel restart or not */
	if (pk_dts->pmu_powkey_off_func)
		regmap_update_bits(regmap, AXP2202_PWROFF_EN, 0x01,
				   0x01); /* restart */
	else
		regmap_update_bits(regmap, AXP2202_PWROFF_EN, 0x01,
				   0x00); /* power off */

	/* pek delay set */
	/* regmap_read(regmap, AXP2202_PWR_TIME_CTRL, &val); */
	/* val &= 0xfc; */
	/* if (pk_dts->pmu_pwrok_time < 32) */
		/* val |= ((pk_dts->pmu_pwrok_time / 8) - 1); */
	/* else */
		/* val |= ((pk_dts->pmu_pwrok_time / 32) + 1); */
	/* regmap_write(regmap, AXP2202_PWR_TIME_CTRL, val); */

	/* pek offlevel time set */
	if (pk_dts->pmu_powkey_off_time < 4000)
		pk_dts->pmu_powkey_off_time = 4000;

	if (pk_dts->pmu_powkey_off_time > 10000)
		pk_dts->pmu_powkey_off_time = 10000;

	regmap_read(regmap, AXP2202_PONLEVEL, &val);
	val &= 0xf3;
	val |= ((pk_dts->pmu_powkey_off_time - 4000) / 2000
		<< 2);
	regmap_write(regmap, AXP2202_PONLEVEL, val);

	/* vbat use all channels */
	/* regmap_write(regmap, AXP2202_VBAT_H, 0x40); */

	return 0;
}
#endif
static int axp152_config_set(struct axp20x_pek *axp20x_pek)
{
#if 0
	struct axp20x_dev *axp20x_dev = axp20x_pek->axp20x;
	struct pk_dts *pk_dts = &axp20x_pek->pk_dts;
	unsigned int val;

	regmap_read(regmap, AXP152_PEK_KEY, &val);
	if (pk_dts->pmu_powkey_on_time <= 128)
		val &= 0x3f;
	else if (pk_dts->pmu_powkey_on_time <= 1000) {
		val &= 0x3f;
		val |= 0x10;
	} else if (pk_dts->pmu_powkey_on_time <= 2000) {
		val &= 0x3f;
		val |= 0x11;
	} else {
		val &= 0x3f;
		val |= 0x01;
	}
	regmap_write(regmap, AXP152_PEK_KEY, val);

	/* pok long time set*/
	if (pk_dts->pmu_powkey_long_time < 1000)
		pk_dts->pmu_powkey_long_time = 1000;

	if (pk_dts->pmu_powkey_long_time > 2500)
		pk_dts->pmu_powkey_long_time = 2500;

	regmap_read(regmap, AXP152_PEK_KEY, &val);
	val &= 0xcf;
	val |= (((pk_dts->pmu_powkey_long_time - 1000) / 500)
		<< 4);
	regmap_write(regmap, AXP152_PEK_KEY, val);

	/* pek offlevel poweroff en set*/
	if (pk_dts->pmu_powkey_off_en)
		pk_dts->pmu_powkey_off_en = 1;
	else
		pk_dts->pmu_powkey_off_en = 0;

	regmap_read(regmap, AXP152_PEK_KEY, &val);
	val &= 0xf7;
	val |= (pk_dts->pmu_powkey_off_en << 3);
	regmap_write(regmap, AXP152_PEK_KEY, val);

	/* pwrok time */
	if (pk_dts->pmu_pwrok_time == 64)
		regmap_update_bits(regmap, AXP152_PEK_KEY, 0x04,
				   0x04); /* 64ms */
	else
		regmap_update_bits(regmap, AXP152_PEK_KEY, 0x04,
				   0x00); /* 8ms */

	/* pek offlevel time set */
	if (pk_dts->pmu_powkey_off_time < 4000)
		pk_dts->pmu_powkey_off_time = 4000;

	if (pk_dts->pmu_powkey_off_time > 10000)
		pk_dts->pmu_powkey_off_time = 10000;

	regmap_read(regmap, AXP152_PEK_KEY, &val);
	val &= 0xfc;
	val |= ((pk_dts->pmu_powkey_off_time - 4000) / 2000);
	regmap_write(regmap, AXP152_PEK_KEY, val);

#endif
	return 0;
}

static int axp803_config_set(struct axp20x_pek *axp20x_pek)
{
#if 0
	struct axp20x_dev *axp20x_dev = axp20x_pek->axp20x;
	struct regmap *regmap = axp20x_dev->regmap;
	struct pk_dts *pk_dts = &axp20x_pek->pk_dts;
	unsigned int val;

	/* onlevel setting */
	regmap_read(regmap, AXP803_POK_SET, &val);
	if (pk_dts->pmu_powkey_on_time <= 128)
		val &= 0x3f;
	else if (pk_dts->pmu_powkey_on_time <= 1000) {
		val &= 0x3f;
		val |= 0x40;
	} else if (pk_dts->pmu_powkey_on_time <= 2000) {
		val &= 0x3f;
		val |= 0x80;
	} else {
		val &= 0x3f;
		val |= 0xc0;
	}
	regmap_write(regmap, AXP803_POK_SET, val);

	/* pok long time set*/
	if (pk_dts->pmu_powkey_long_time < 1000)
		pk_dts->pmu_powkey_long_time = 1000;

	if (pk_dts->pmu_powkey_long_time > 2500)
		pk_dts->pmu_powkey_long_time = 2500;

	regmap_read(regmap, AXP803_POK_SET, &val);
	val &= 0xcf;
	val |= (((pk_dts->pmu_powkey_long_time - 1000) / 500)
		<< 4);
	regmap_write(regmap, AXP803_POK_SET, val);

	/* pek offlevel poweroff en set*/
	if (pk_dts->pmu_powkey_off_en)
		pk_dts->pmu_powkey_off_en = 1;
	else
		pk_dts->pmu_powkey_off_en = 0;

	regmap_read(regmap, AXP803_POK_SET, &val);
	val &= 0xf7;
	val |= (pk_dts->pmu_powkey_off_en << 3);
	regmap_write(regmap, AXP803_POK_SET, val);

	/*Init offlevel restart or not */
	if (pk_dts->pmu_powkey_off_func)
		regmap_update_bits(regmap, AXP803_POK_SET, 0x04,
				   0x01); /* restart */
	else
		regmap_update_bits(regmap, AXP803_POK_SET, 0x04,
				   0x00); /* not restart*/

	/* pek offlevel poweroff time set */
	if (pk_dts->pmu_powkey_off_time < 4000)
		pk_dts->pmu_powkey_off_time = 4000;

	if (pk_dts->pmu_powkey_off_time > 10000)
		pk_dts->pmu_powkey_off_time = 10000;

	regmap_read(regmap, AXP803_POK_SET, &val);
	val &= 0xfc;
	val |= ((pk_dts->pmu_powkey_off_time - 4000) / 2000);
	regmap_write(regmap, AXP803_POK_SET, val);
#endif

	return 0;
}


static int axp2585_config_set(struct axp20x_pek *axp20x_pek)
{
#if 0
	struct axp20x_dev *axp20x_dev = axp20x_pek->axp20x;
	struct regmap *regmap = axp20x_dev->regmap;
	struct pk_dts *pk_dts = &axp20x_pek->pk_dts;
	unsigned int val;

	/* onlevel setting */
	regmap_read(regmap, AXP2585_POK_SET, &val);
	if (pk_dts->pmu_powkey_on_time <= 628)
		val &= 0xf3;
	else if (pk_dts->pmu_powkey_on_time <= 1500) {
		val &= 0xf3;
		val |= 0x04;
	} else if (pk_dts->pmu_powkey_on_time <= 2500) {
		val &= 0xf3;
		val |= 0x08;
	} else {
		val &= 0xf3;
		val |= 0x0c;
	}
	regmap_write(regmap, AXP2585_POK_SET, val);

	/* pok long time set*/
	if (pk_dts->pmu_powkey_long_time < 1000)
		pk_dts->pmu_powkey_long_time = 1000;

	if (pk_dts->pmu_powkey_long_time > 2500)
		pk_dts->pmu_powkey_long_time = 2500;

	regmap_read(regmap, AXP2585_POK_SET, &val);
	val &= 0x3f;
	val |= (((pk_dts->pmu_powkey_long_time - 1000) / 500)
		<< 6);
	regmap_write(regmap, AXP2585_POK_SET, val);

	/* pek offlevel poweroff time set */
	if (pk_dts->pmu_powkey_off_time < 4500)
		pk_dts->pmu_powkey_off_time = 4500;

	if (pk_dts->pmu_powkey_off_time > 10500)
		pk_dts->pmu_powkey_off_time = 10500;

	regmap_read(regmap, AXP2585_POK_SET, &val);
	val &= 0xfc;
	val |= ((pk_dts->pmu_powkey_off_time - 4500) / 2000);
	regmap_write(regmap, AXP2585_POK_SET, val);
#endif
	return 0;
}

static int axp2202_config_set(struct axp20x_pek *axp20x_pek)
{
#if 0
	struct axp20x_dev *axp20x_dev = axp20x_pek->axp20x;
	struct regmap *regmap = axp20x_dev->regmap;
	struct pk_dts *pk_dts = &axp20x_pek->pk_dts;
	unsigned int val;

	regmap_read(regmap, AXP2202_PONLEVEL, &val);
	if (pk_dts->pmu_powkey_on_time < 128)
		val &= 0x3C;
	else if (pk_dts->pmu_powkey_on_time < 512) {
		val &= 0x3C;
		val |= 0x01;
	} else if (pk_dts->pmu_powkey_on_time < 1000) {
		val &= 0x3C;
		val |= 0x10;
	} else {
		val &= 0x3C;
		val |= 0x11;
	}
	regmap_write(regmap, AXP2202_PONLEVEL, val);

	/* pok long time set*/
	if (pk_dts->pmu_powkey_long_time < 1000)
		pk_dts->pmu_powkey_long_time = 1000;

	if (pk_dts->pmu_powkey_long_time > 2500)
		pk_dts->pmu_powkey_long_time = 2500;

	regmap_read(regmap, AXP2202_PONLEVEL, &val);
	val &= 0xcf;
	val |= (((pk_dts->pmu_powkey_long_time - 1000) / 500)
		<< 4);
	regmap_write(regmap, AXP2202_PONLEVEL, val);

	/* pek offlevel poweroff en set*/
	if (pk_dts->pmu_powkey_off_en)
		pk_dts->pmu_powkey_off_en = 1;
	else
		pk_dts->pmu_powkey_off_en = 0;

	regmap_read(regmap, AXP2202_PWROFF_EN, &val);
	val &= 0xfD;
	val |= (pk_dts->pmu_powkey_off_en << 1);
	regmap_write(regmap, AXP2202_PWROFF_EN, val);

	/*Init offlevel restart or not */
	if (pk_dts->pmu_powkey_off_func)
		regmap_update_bits(regmap, AXP2202_PWROFF_EN, 0x01,
				   0x01); /* restart */
	else
		regmap_update_bits(regmap, AXP2202_PWROFF_EN, 0x01,
				   0x00); /* power off */

	/* pek delay set */
	/* regmap_read(regmap, AXP2202_PWR_TIME_CTRL, &val); */
	/* val &= 0xfc; */
	/* if (pk_dts->pmu_pwrok_time < 32) */
		/* val |= ((pk_dts->pmu_pwrok_time / 8) - 1); */
	/* else */
		/* val |= ((pk_dts->pmu_pwrok_time / 32) + 1); */
	/* regmap_write(regmap, AXP2202_PWR_TIME_CTRL, val); */

	/* pek offlevel time set */
	if (pk_dts->pmu_powkey_off_time < 4000)
		pk_dts->pmu_powkey_off_time = 4000;

	if (pk_dts->pmu_powkey_off_time > 10000)
		pk_dts->pmu_powkey_off_time = 10000;

	regmap_read(regmap, AXP2202_PONLEVEL, &val);
	val &= 0xf3;
	val |= ((pk_dts->pmu_powkey_off_time - 4000) / 2000
		<< 2);
	regmap_write(regmap, AXP2202_PONLEVEL, val);

	/* vbat use all channels */
	/* regmap_write(regmap, AXP2202_VBAT_H, 0x40); */

#endif
	return 0;
}

static int axp2201_config_set(struct axp20x_pek *axp20x_pek)
{
	__nouse__ struct axp20x_dev *axp20x_dev = axp20x_pek->axp20x;
	struct pk_dts *pk_dts = &axp20x_pek->pk_dts;
	unsigned int val = 0;

	// regmap_read(regmap, axp2101_PONLEVEL, &val);
	if (pk_dts->pmu_powkey_on_time < 128)
		val &= 0x3C;
	else if (pk_dts->pmu_powkey_on_time < 512) {
		val &= 0x3C;
		val |= 0x01;
	} else if (pk_dts->pmu_powkey_on_time < 1000) {
		val &= 0x3C;
		val |= 0x02;
	} else {
		val &= 0x3C;
		val |= 0x03;
	}
	// regmap_write(regmap, axp2101_PONLEVEL, val);

	/* pok long time set*/
	if (pk_dts->pmu_powkey_long_time < 1000)
		pk_dts->pmu_powkey_long_time = 1000;

	if (pk_dts->pmu_powkey_long_time > 2500)
		pk_dts->pmu_powkey_long_time = 2500;

	// regmap_read(regmap, axp2101_PONLEVEL, &val);
	val &= 0xcf;
	val |= (((pk_dts->pmu_powkey_long_time - 1000) / 500)
		<< 4);
	// regmap_write(regmap, axp2101_PONLEVEL, val);

	/* pek offlevel poweroff en set*/
	if (pk_dts->pmu_powkey_off_en)
		pk_dts->pmu_powkey_off_en = 1;
	else
		pk_dts->pmu_powkey_off_en = 0;

	// regmap_read(regmap, axp2101_PWROFF_EN, &val);
	val &= 0x0D;
	val |= (pk_dts->pmu_powkey_off_en << 1);
	// regmap_write(regmap, axp2101_PWROFF_EN, val);

	/*Init offlevel restart or not */
	// if (pk_dts->pmu_powkey_off_func)
		// regmap_update_bits(regmap, axp2101_PWROFF_EN, 0x01,
				   // 0x01); /* restart */
	// else
		// regmap_update_bits(regmap, axp2101_PWROFF_EN, 0x01,
				   // 0x00); /* not restart*/

	/* pek delay set */
	// regmap_read(regmap, axp2101_PWR_TIME_CTRL, &val);
	val &= 0xfc;
	if (pk_dts->pmu_pwrok_time < 32)
		val |= ((pk_dts->pmu_pwrok_time / 8) - 1);
	else
		val |= ((pk_dts->pmu_pwrok_time / 32) + 1);
	// regmap_write(regmap, axp2101_PWR_TIME_CTRL, val);

	/* pek offlevel time set */
	if (pk_dts->pmu_powkey_off_time < 4000)
		pk_dts->pmu_powkey_off_time = 4000;

	if (pk_dts->pmu_powkey_off_time > 10000)
		pk_dts->pmu_powkey_off_time = 10000;

	// regmap_read(regmap, axp2101_PONLEVEL, &val);
	val &= 0x33;
	val |= ((pk_dts->pmu_powkey_off_time - 4000) / 2000
		<< 2);
	// regmap_write(regmap, axp2101_PONLEVEL, val);

	/* vbat use all channels */
	// regmap_write(regmap, axp2101_VBAT_H, 0x40);

	return 0;
}

static int axp806_config_set(struct axp20x_pek *axp20x_pek)
{
#if 0
	struct axp20x_dev *axp20x_dev = axp20x_pek->axp20x;
	struct regmap *regmap = axp20x_dev->regmap;
	struct pk_dts *pk_dts = &axp20x_pek->pk_dts;
	unsigned int val;

	regmap_read(regmap, AXP806_POK_SET, &val);
	if (pk_dts->pmu_powkey_on_time < 128)
		val &= 0x3f;
	else if (pk_dts->pmu_powkey_on_time < 1000) {
		val &= 0x3f;
		val |= 0x40;
	} else if (pk_dts->pmu_powkey_on_time < 2000) {
		val &= 0x3f;
		val |= 0x80;
	} else {
		val &= 0x3f;
		val |= 0xc0;
	}
	regmap_write(regmap, AXP806_POK_SET, val);

	/* pok long time set*/
	if (pk_dts->pmu_powkey_long_time < 1000)
		pk_dts->pmu_powkey_long_time = 1000;

	if (pk_dts->pmu_powkey_long_time > 2500)
		pk_dts->pmu_powkey_long_time = 2500;

	regmap_read(regmap, AXP806_POK_SET, &val);
	val &= 0xcf;
	val |= (((pk_dts->pmu_powkey_long_time - 1000) / 500) << 4);
	regmap_write(regmap, AXP806_POK_SET, val);

	/*Init offlevel restart or not */
	if (pk_dts->pmu_powkey_off_func)
		regmap_update_bits(regmap, AXP806_POK_SET, 0x04, 0x04); /* restart */
	else
		regmap_update_bits(regmap, AXP806_POK_SET, 0x04, 0x00); /* not restart*/

	/* pek offlevel poweroff en set*/
	if (pk_dts->pmu_powkey_off_en)
		pk_dts->pmu_powkey_off_en = 1;
	else
		pk_dts->pmu_powkey_off_en = 0;

	regmap_read(regmap, AXP806_POK_SET, &val);
	val &= 0xf7;
	val |= (pk_dts->pmu_powkey_off_en << 3);
	regmap_write(regmap, AXP806_POK_SET, val);

	/* pek offlevel time set */
	if (pk_dts->pmu_powkey_off_time < 4000)
		pk_dts->pmu_powkey_off_time = 4000;

	if (pk_dts->pmu_powkey_off_time > 10000)
		pk_dts->pmu_powkey_off_time = 10000;

	regmap_read(regmap, AXP806_POK_SET, &val);
	val &= 0xfc;
	val |= (pk_dts->pmu_powkey_off_time - 4000) / 2000;
	regmap_write(regmap, AXP806_POK_SET, val);

#endif
	return 0;
}

static void axp20x_dts_param_set(struct axp20x_pek *axp20x_pek)
{
	struct axp20x_dev *axp20x_dev = axp20x_pek->axp20x;

	if (!axp_powerkey_dt_parse(&axp20x_pek->pk_dts)) {
		switch (axp20x_dev->variant) {
		case AXP152_ID:
			axp152_config_set(axp20x_pek);
			break;
		case AXP2202_ID:
			axp2202_config_set(axp20x_pek);
			break;
		case AXP2585_ID:
			axp2585_config_set(axp20x_pek);
			break;
		case AXP803_ID:
			axp803_config_set(axp20x_pek);
			break;
		case AXP2101_ID:
			axp2201_config_set(axp20x_pek);
			break;
		case AXP806_ID:
			axp806_config_set(axp20x_pek);
			break;
		default:
			printf("Setting power key for unsupported AXP variant\n");
		}
	}
}

static int axp20x_pek_probe(void *pdev, void *config)
{
	__nouse__ struct axp20x_pek *axp20x_pek;
	__nouse__ struct axp20x_dev *axp20x = (struct axp20x_dev *)pdev;

	axp20x_pek = (struct axp20x_pek *)malloc(sizeof(struct axp20x_pek));
	if (!axp20x_pek)
		return -1;

	axp20x_pek->axp20x = axp20x;

	// if (!axp20x->irq) {
		// pr_err("axp2101-pek can not register without irq\n");
		// return -1;
	// }

	axp20x_dts_param_set(axp20x_pek);
	// error = devm_request_any_context_irq(&pdev->dev, axp20x_pek->irq_dbr,
						 // axp20x_pek_irq, 0,
						 // "axp20x-pek-dbr", idev);

	// error = devm_request_any_context_irq(&pdev->dev, axp20x_pek->irq_dbf,
					  // axp20x_pek_irq, 0,
					  // "axp20x-pek-dbf", idev);

	return 0;
}

static int axp2101_powerkey_suspend(void *dev)
{
	// struct platform_device *pdev = to_platform_device(dev);
	// struct axp20x_pek *axp20x_pek = platform_get_drvdata(pdev);
//
	// if (!axp20x_pek->pk_dts.pmu_powkey_wakeup_rising) {
		// disable_irq(axp20x_pek->irq_dbr);
	// }
//
	// if (!axp20x_pek->pk_dts.pmu_powkey_wakeup_falling) {
		// disable_irq(axp20x_pek->irq_dbf);
	// }

	return 0;
}

static int axp2101_powerkey_resume(void *dev)
{
	// struct platform_device *pdev = to_platform_device(dev);
	// struct axp20x_pek *axp20x_pek = platform_get_drvdata(pdev);
//
	// if (!axp20x_pek->pk_dts.pmu_powkey_wakeup_rising) {
		// enable_irq(axp20x_pek->irq_dbr);
	// }
//
	// if (!axp20x_pek->pk_dts.pmu_powkey_wakeup_falling) {
		// enable_irq(axp20x_pek->irq_dbf);
	// }

	return 0;
}

static struct dev_pm_ops axp2101_powerkey_pm_ops = {
	.suspend      = axp2101_powerkey_suspend,
	.resume       = axp2101_powerkey_resume,
};

static struct of_device_id axp_match_table[] = {
	{ .compatible = "x-powers,axp2585-pek" },
	{ .compatible = "x-powers,axp2202-pek" },
	{ .compatible = "x-powers,axp803-pek" },
	{ .compatible = "x-powers,axp806-pek" },
	{ .compatible = "x-powers,axp152-pek" },
	{ /* sentinel */ },
};

__nouse__ static struct platform_driver axp20x_pek_driver = {
	.probe		= axp20x_pek_probe,
	.of_match_table = axp_match_table,
	.name = "axp2101-pek",
	.pm = &axp2101_powerkey_pm_ops,
};
