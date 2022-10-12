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

#define AXP20X_PEK_STARTUP_MASK  (0x03)
#define AXP20X_PEK_SHUTDOWN_MASK (0x0c)

#define axp2101_PONLEVEL         (0x27)
#define axp2101_PWROFF_EN        (0x22)
#define axp2101_PWR_TIME_CTRL    (0x25)
#define axp2101_VBAT_H           (0x34)
#define axp2101_INTSTS2          (0x49)

struct axp20x_pek *g_axp20x_pek;

struct pk_dts {
    u32 pmu_powkey_off_time;
    u32 pmu_powkey_off_func;
    u32 pmu_powkey_off_en;
    u32 pmu_powkey_off_delay_time;
    u32 pmu_powkey_long_time;
    u32 pmu_powkey_on_time;
    u32 pmu_pwrok_time;
    u32 pmu_pwrnoe_time;
    u32 pmu_powkey_wakeup_rising;
    u32 pmu_powkey_wakeup_falling;
};

struct axp20x_pek {
    struct axp20x_dev *axp20x;
    struct pk_dts      pk_dts;
    int                irq_dbr;
    int                irq_dbf;
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

static int axp_powerkey_dt_parse(struct pk_dts *pk_dts) {

    pk_dts->pmu_powkey_off_time       = 6000;
    pk_dts->pmu_powkey_off_func       = 0;
    pk_dts->pmu_powkey_off_en         = 1;
    pk_dts->pmu_powkey_off_delay_time = 0;
    pk_dts->pmu_powkey_long_time      = 1500;
    pk_dts->pmu_powkey_on_time        = 1000;
    pk_dts->pmu_pwrok_time            = 64;
    pk_dts->pmu_pwrnoe_time           = 2000;

    pk_dts->pmu_powkey_wakeup_rising  = 0;
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

__nouse__ static int axp20x_pek_irq(int irq, void *pwr) {
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

static int axp2201_config_set(struct axp20x_pek *axp20x_pek) {
    __nouse__ struct axp20x_dev *axp20x_dev = axp20x_pek->axp20x;
    struct pk_dts               *pk_dts     = &axp20x_pek->pk_dts;
    u32                          val        = 0;

    axp20x_i2c_read(axp2101_PONLEVEL, &val);
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
    axp20x_i2c_write(axp2101_PONLEVEL, val);

    /* pok long time set*/
    if (pk_dts->pmu_powkey_long_time < 1000)
        pk_dts->pmu_powkey_long_time = 1000;

    if (pk_dts->pmu_powkey_long_time > 2500)
        pk_dts->pmu_powkey_long_time = 2500;

    axp20x_i2c_read(axp2101_PONLEVEL, &val);
    val &= 0xcf;
    val |= (((pk_dts->pmu_powkey_long_time - 1000) / 500) << 4);
    axp20x_i2c_write(axp2101_PONLEVEL, val);

    /* pek offlevel poweroff en set*/
    if (pk_dts->pmu_powkey_off_en)
        pk_dts->pmu_powkey_off_en = 1;
    else
        pk_dts->pmu_powkey_off_en = 0;

    axp20x_i2c_read(axp2101_PWROFF_EN, &val);
    val &= 0x0D;
    val |= (pk_dts->pmu_powkey_off_en << 1);
    axp20x_i2c_write(axp2101_PWROFF_EN, val);

    /*Init offlevel restart or not */
    axp20x_i2c_read(axp2101_PWROFF_EN, &val);
    if (pk_dts->pmu_powkey_off_func)
        axp20x_i2c_write(axp2101_PWROFF_EN, val | 0x1);
    else
        axp20x_i2c_write(axp2101_PWROFF_EN, val & (~0x1));

    /* pek delay set */
    axp20x_i2c_read(axp2101_PWR_TIME_CTRL, &val);
    val &= 0xfc;
    if (pk_dts->pmu_pwrok_time < 32)
        val |= ((pk_dts->pmu_pwrok_time / 8) - 1);
    else
        val |= ((pk_dts->pmu_pwrok_time / 32) + 1);
    axp20x_i2c_write(axp2101_PWR_TIME_CTRL, val);

    /* pek offlevel time set */
    if (pk_dts->pmu_powkey_off_time < 4000) pk_dts->pmu_powkey_off_time = 4000;

    if (pk_dts->pmu_powkey_off_time > 10000)
        pk_dts->pmu_powkey_off_time = 10000;

    axp20x_i2c_read(axp2101_PONLEVEL, &val);
    val &= 0x33;
    val |= ((pk_dts->pmu_powkey_off_time - 4000) / 2000 << 2);
    axp20x_i2c_write(axp2101_PONLEVEL, val);

    /* vbat use all channels */
    // regmap_write(regmap, axp2101_VBAT_H, 0x40);
    axp20x_i2c_write(axp2101_VBAT_H, 0x40);

    return 0;
}

static void axp20x_dts_param_set(struct axp20x_pek *axp20x_pek) {
    struct axp20x_dev *axp20x_dev = axp20x_pek->axp20x;

    if (!axp_powerkey_dt_parse(&axp20x_pek->pk_dts)) {
        switch (axp20x_dev->variant) {
        case AXP2101_ID:
            axp2201_config_set(axp20x_pek);
            break;
        default:
            printf("Setting power key for unsupported AXP variant\n");
        }
    }
}

int axp20x_pek_probe(void *pdev, void *config) {
    __nouse__ struct axp20x_dev *axp20x = (struct axp20x_dev *) pdev;

    g_axp20x_pek = (struct axp20x_pek *) malloc(sizeof(struct axp20x_pek));
    if (!g_axp20x_pek) return -1;

    g_axp20x_pek->axp20x = axp20x;

    // if (!axp20x->irq) {
    // pr_err("axp2101-pek can not register without irq\n");
    // return -1;
    // }

    axp20x_dts_param_set(g_axp20x_pek);
    // error = devm_request_any_context_irq(&pdev->dev, axp20x_pek->irq_dbr,
    // axp20x_pek_irq, 0,
    // "axp20x-pek-dbr", idev);

    // error = devm_request_any_context_irq(&pdev->dev, axp20x_pek->irq_dbf,
    // axp20x_pek_irq, 0,
    // "axp20x-pek-dbf", idev);

    return 0;
}

void axp20x_pek_remove(void) {
    if (g_axp20x_pek != NULL) free(g_axp20x_pek);
}

int axp2101_powerkey_suspend(void) {
    u32 val = 0;

    // SLEEP enable
    axp20x_i2c_read(AXP2101_SLEEP_CFG, &val);
    val |= 0x01;
    axp20x_i2c_write(AXP2101_SLEEP_CFG, val);

    // close voltage 80H 90H 91H

    axp20x_set_dcdc1(0);
    axp20x_set_dcdc2(0);
    axp20x_set_dcdc3(0);
    axp20x_set_dcdc4(0);
    axp20x_set_dcdc5(0);
    axp20x_set_aldo1(0);
    axp20x_set_aldo2(0);
    axp20x_set_aldo3(0);
    axp20x_set_aldo4(0);
    axp20x_set_bldo1(0);
    axp20x_set_bldo2(0);
    axp20x_set_dldo1(0);
    axp20x_set_dldo2(0);
    axp20x_set_cpusldo(0);

    // IRQ Pin low to Wakeup
    axp20x_i2c_read(AXP2101_SLEEP_CFG, &val);
    val |= 0x10;
    axp20x_i2c_write(AXP2101_SLEEP_CFG, val);

    // POWERON Short Long IRQ
    val = 0x0c;
    axp20x_i2c_write(AXP2101_INTEN2, val);

    return 0;
}

int axp2101_powerkey_resume(void) {

    axp20x_set_dcdc1(1200);
    axp20x_set_dcdc2(1200);
    axp20x_set_dcdc3(1200);
    axp20x_set_dcdc4(1200);
    axp20x_set_dcdc5(1200);
    axp20x_set_aldo1(1200);
    axp20x_set_aldo2(1200);
    axp20x_set_aldo3(1200);
    axp20x_set_aldo4(1200);
    axp20x_set_bldo1(1200);
    axp20x_set_bldo2(1200);
    axp20x_set_dldo1(1200);
    axp20x_set_dldo2(1200);
    axp20x_set_cpusldo(1200);

    return 0;
}
