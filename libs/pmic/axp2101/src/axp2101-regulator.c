/*
 * AXP20x regulators driver.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "axp2101.h"
#include "utils_def.h"

struct regulator_delay *g_rdev_delay;
struct regulator_delay _rdev_delay;

#define of_match_ptr(_ptr)            (_ptr)

// #define UINT8_C(x)                    x
// #define UINT16_C(x)                   x
// #define UINT32_C(x)                   x##U

#define AXP20X_IO_ENABLED             0x03
#define AXP20X_IO_DISABLED            0x07

#define AXP22X_IO_ENABLED             0x03
#define AXP22X_IO_DISABLED            0x04

#define AXP20X_WORKMODE_DCDC2_MASK    BIT(2)
#define AXP20X_WORKMODE_DCDC3_MASK    BIT(1)
#define AXP22X_WORKMODE_DCDCX_MASK(x) BIT(x)

#define AXP20X_FREQ_DCDC_MASK         0x0f

#define AXP22X_MISC_N_VBUSEN_FUNC     BIT(4)

#define AXP803_MISC_N_VBUSEN_FUNC     BIT(4)

#define AXP_DESC_IO(_family, _id, _match, _supply, _min, _max, _step, _vreg, \
                    _vmask, _ereg, _emask, _enable_val, _disable_val)        \
    [_family##_##_id] = {                                                    \
        .name            = (_match),                                         \
        .supply_name     = (_supply),                                        \
        .of_match        = of_match_ptr(_match),                             \
        .regulators_node = of_match_ptr("regulators"),                       \
        .type            = REGULATOR_VOLTAGE,                                \
        .id              = _family##_##_id,                                  \
        .n_voltages      = (((_max) - (_min)) / (_step) + 1),                \
        .min_uV          = (_min) *1000,                                     \
        .uV_step         = (_step) *1000,                                    \
        .vsel_reg        = (_vreg),                                          \
        .vsel_mask       = (_vmask),                                         \
        .enable_reg      = (_ereg),                                          \
        .enable_mask     = (_emask),                                         \
        .enable_val      = (_enable_val),                                    \
        .disable_val     = (_disable_val),                                   \
        .ops             = &axp20x_ops,                                      \
    }

#define AXP_DESC(_family, _id, _match, _supply, _min, _max, _step, _vreg, \
                 _vmask, _ereg, _emask)                                   \
    [_family##_##_id] = {                                                 \
        .name            = (_match),                                      \
        .supply_name     = (_supply),                                     \
        .of_match        = of_match_ptr(_match),                          \
        .regulators_node = of_match_ptr("regulators"),                    \
        .type            = REGULATOR_VOLTAGE,                             \
        .id              = _family##_##_id,                               \
        .n_voltages      = (((_max) - (_min)) / (_step) + 1),             \
        .min_mV          = (_min),                                        \
        .max_mV          = (_max),                                        \
        .min_uV          = (_min) *1000,                                  \
        .uV_step         = (_step) *1000,                                 \
        .vsel_reg        = (_vreg),                                       \
        .vsel_mask       = (_vmask),                                      \
        .enable_reg      = (_ereg),                                       \
        .enable_mask     = (_emask),                                      \
        .ops             = &axp20x_ops,                                   \
    }

#define AXP_DESC_SW(_family, _id, _match, _supply, _ereg, _emask) \
    [_family##_##_id] = {                                         \
        .name            = (_match),                              \
        .supply_name     = (_supply),                             \
        .of_match        = of_match_ptr(_match),                  \
        .regulators_node = of_match_ptr("regulators"),            \
        .type            = REGULATOR_VOLTAGE,                     \
        .id              = _family##_##_id,                       \
        .enable_reg      = (_ereg),                               \
        .enable_mask     = (_emask),                              \
        .ops             = &axp20x_ops_sw,                        \
    }

#define AXP_DESC_FIXED(_family, _id, _match, _supply, _volt)            \
    [_family##_##_id] = {.name            = (_match),                   \
                         .supply_name     = (_supply),                  \
                         .of_match        = of_match_ptr(_match),       \
                         .regulators_node = of_match_ptr("regulators"), \
                         .type            = REGULATOR_VOLTAGE,          \
                         .id              = _family##_##_id,            \
                         .n_voltages      = 1,                          \
                         .min_uV          = (_volt) *1000,              \
                         .ops             = &axp20x_ops_fixed}

#define AXP_DESC_RANGES(_family, _id, _match, _supply, _ranges, _n_voltages, \
                        _vreg, _vmask, _ereg, _emask)                        \
    [_family##_##_id] = {                                                    \
        .name            = (_match),                                         \
        .supply_name     = (_supply),                                        \
        .of_match        = of_match_ptr(_match),                             \
        .regulators_node = of_match_ptr("regulators"),                       \
        .type            = REGULATOR_VOLTAGE,                                \
        .id              = _family##_##_id,                                  \
        .n_voltages      = (_n_voltages),                                    \
        .vsel_reg        = (_vreg),                                          \
        .vsel_mask       = (_vmask),                                         \
        .enable_reg      = (_ereg),                                          \
        .enable_mask     = (_emask),                                         \
        .linear_ranges   = (_ranges),                                        \
        .n_linear_ranges = ARRAY_SIZE(_ranges),                              \
        .ops             = &axp20x_ops_range,                                \
    }

#define AXP_DESC_RANGES_VOL_DELAY(_family, _id, _match, _supply, _ranges,    \
                                  _n_voltages, _vreg, _vmask, _ereg, _emask) \
    [_family##_##_id] = {                                                    \
        .name            = (_match),                                         \
        .supply_name     = (_supply),                                        \
        .of_match        = of_match_ptr(_match),                             \
        .regulators_node = of_match_ptr("regulators"),                       \
        .type            = REGULATOR_VOLTAGE,                                \
        .id              = _family##_##_id,                                  \
        .n_voltages      = (_n_voltages),                                    \
        .vsel_reg        = (_vreg),                                          \
        .vsel_mask       = (_vmask),                                         \
        .enable_reg      = (_ereg),                                          \
        .enable_mask     = (_emask),                                         \
        .linear_ranges   = (_ranges),                                        \
        .n_linear_ranges = ARRAY_SIZE(_ranges),                              \
        .ops             = &axp20x_ops_range_vol_delay,                      \
    }

struct regulator_delay {
    u32 step;
    u32 final;
};


static int _abs(int num)
{
    return num < 0 ? -num : num;
}

static int axp2101_set_voltage_time_sel(struct regulator_desc *desc,
                                        unsigned int           old_selector,
                                        unsigned int           new_selector)
{
    struct regulator_delay *delay = (struct regulator_delay *) desc->reg_data;

    return _abs(new_selector - old_selector) * delay->step + delay->final;
};

int regulator_enable_regmap(struct regulator_desc *desc)
{
    u32 val;
    int ret = 0;

    ret = axp20x_i2c_read(desc->enable_reg, &val);
    val |= desc->enable_mask;
    ret = axp20x_i2c_write(desc->enable_reg, val);
    return ret;
}

int regulator_disable_regmap(struct regulator_desc *desc)
{
    u32 val;
    int ret = 0;

    ret = axp20x_i2c_read(desc->enable_reg, &val);
    val &= ~desc->enable_mask;
    ret = axp20x_i2c_write(desc->enable_reg, val);
    return ret;
}

int regulator_is_enabled_regmap(struct regulator_desc *desc)
{
    u32           val = 0;
    __nouse__ int ret = 0;

    ret = axp20x_i2c_read(desc->enable_reg, &val);
    if (ret != 0) return ret;

    val &= desc->enable_mask;

    if (desc->enable_is_inverted) {
        if (desc->enable_val) return val != desc->enable_val;
        return val == 0;
    } else {
        if (desc->enable_val) return val == desc->enable_val;
        return val != 0;
    }
}

static int __ffs(unsigned int v)
{
    int n = 1;

    if (!v) return -1;
    if (!(v & 0x0000FFFF)) { v >>= 16; n += 16; }
    if (!(v & 0x000000FF)) { v >>=  8; n += 8;  }
    if (!(v & 0x0000000F)) { v >>=  4; n += 4;  }
    if (!(v & 0x00000003)) { v >>=  2; n += 2;  }
    if (!(v & 0x00000001)) { v >>=  1; n += 1;  }

    return n - 1;
}


int regulator_set_voltage_sel_regmap(struct regulator_desc *desc, unsigned sel)
{
    int ret = 0;
    u32 val = 0;

    // sel <<= ffs(desc->vsel_mask) - 1;

    ret = axp20x_i2c_read(desc->vsel_reg, &val);
    val &= ~desc->vsel_mask;
    sel <<= __ffs(desc->vsel_mask) - 1;
    val |= sel;
    ret = axp20x_i2c_write(desc->vsel_reg, val);

    return ret;
}

int regulator_get_voltage_sel_regmap(struct regulator_desc *desc)
{
    u32           val = 0;
    __nouse__ int ret = 0;

    ret = axp20x_i2c_read(desc->vsel_reg, &val);

    val &= desc->vsel_mask;
    val >>= __ffs(desc->vsel_mask) - 1;

    return val;
}

int regulator_list_voltage_linear(struct regulator_desc *desc,
                                  unsigned int           selector)
{
    if (selector >= desc->n_voltages) return -1;
    if (selector < desc->linear_min_sel) return 0;

    selector -= desc->linear_min_sel;

    return desc->min_uV + (desc->uV_step * selector);
}

int regulator_list_voltage_linear_range(struct regulator_desc *desc,
                                        unsigned int           selector)
{
    const struct regulator_linear_range *range;
    int                                  i;

    if (!desc->n_linear_ranges) {
        // BUG_ON(!rdev->desc->n_linear_ranges);
        return -1;
    }

    for (i = 0; i < desc->n_linear_ranges; i++) {
        range = &desc->linear_ranges[ i ];

        if (!(selector >= range->min_sel && selector <= range->max_sel))
            continue;

        selector -= range->min_sel;

        return range->min_uV + (range->uV_step * selector);
    }

    return -1;
}

static struct regulator_ops axp20x_ops_fixed = {
    .list_voltage = regulator_list_voltage_linear,
};

static struct regulator_ops axp20x_ops_range = {
    .set_voltage_sel      = regulator_set_voltage_sel_regmap,
    .get_voltage_sel      = regulator_get_voltage_sel_regmap,
    .list_voltage         = regulator_list_voltage_linear_range,
    .enable               = regulator_enable_regmap,
    .disable              = regulator_disable_regmap,
    .is_enabled           = regulator_is_enabled_regmap,
    .set_voltage_time_sel = axp2101_set_voltage_time_sel,
};

__nouse__ static struct regulator_ops axp20x_ops_range_vol_delay = {
    .set_voltage_sel = regulator_set_voltage_sel_regmap,
    .get_voltage_sel = regulator_get_voltage_sel_regmap,
    .list_voltage    = regulator_list_voltage_linear_range,
    .enable          = regulator_enable_regmap,
    .disable         = regulator_disable_regmap,
    .is_enabled      = regulator_is_enabled_regmap,
};

static struct regulator_ops axp20x_ops = {
    .set_voltage_sel      = regulator_set_voltage_sel_regmap,
    .get_voltage_sel      = regulator_get_voltage_sel_regmap,
    .list_voltage         = regulator_list_voltage_linear,
    .enable               = regulator_enable_regmap,
    .disable              = regulator_disable_regmap,
    .is_enabled           = regulator_is_enabled_regmap,
    .set_voltage_time_sel = axp2101_set_voltage_time_sel,
};

__nouse__ static struct regulator_ops axp20x_ops_sw = {
    .enable     = regulator_enable_regmap,
    .disable    = regulator_disable_regmap,
    .is_enabled = regulator_is_enabled_regmap,
};

static const struct regulator_linear_range axp2101_dcdc2_ranges[] = {
    REGULATOR_LINEAR_RANGE(500000, 0x0, 0x46, 10000),
    REGULATOR_LINEAR_RANGE(1220000, 0x47, 0x57, 20000),
};

static const struct regulator_linear_range axp2101_dcdc3_ranges[] = {
    REGULATOR_LINEAR_RANGE(500000, 0x0, 0x46, 10000),
    REGULATOR_LINEAR_RANGE(1220000, 0x47, 0x57, 20000),
    REGULATOR_LINEAR_RANGE(1600000, 0x58, 0x6a, 100000),
};

static const struct regulator_linear_range axp2101_dcdc4_ranges[] = {
    REGULATOR_LINEAR_RANGE(500000, 0x0, 0x46, 10000),
    REGULATOR_LINEAR_RANGE(1220000, 0x47, 0x66, 20000),
};

__nouse__ static const struct regulator_linear_range axp2101_rtcldo_ranges[] = {
    REGULATOR_LINEAR_RANGE(1800000, 0x0, 0x0, 0),
    REGULATOR_LINEAR_RANGE(2500000, 0x1, 0x1, 0),
    REGULATOR_LINEAR_RANGE(2800000, 0x2, 0x2, 0),
    REGULATOR_LINEAR_RANGE(3300000, 0x3, 0x3, 0),
};

static const struct regulator_linear_range axp2101_dcdc5_ranges[] = {
    REGULATOR_LINEAR_RANGE(1400000, 0x0, 0x17, 100000),
    REGULATOR_LINEAR_RANGE(1200000, 0x19, 0x19, 0),
};

static struct regulator_desc axp2101_regulators[] = {
    AXP_DESC(AXP2101, DCDC1, "dcdc1", "vin1", 1500, 3400, 100,
             AXP2101_DCDC1_CFG, 0x1f, AXP2101_DCDC_CFG0, BIT(0)),
    AXP_DESC_RANGES(AXP2101, DCDC2, "dcdc2", "vin2", axp2101_dcdc2_ranges, 0x58,
                    AXP2101_DCDC2_CFG, 0x7f, AXP2101_DCDC_CFG0, BIT(1)),
    AXP_DESC_RANGES(AXP2101, DCDC3, "dcdc3", "vin3", axp2101_dcdc3_ranges, 0x6b,
                    AXP2101_DCDC3_CFG, 0x7f, AXP2101_DCDC_CFG0, BIT(2)),
    AXP_DESC_RANGES(AXP2101, DCDC4, "dcdc4", "vin4", axp2101_dcdc4_ranges, 0x67,
                    AXP2101_DCDC4_CFG, 0x7f, AXP2101_DCDC_CFG0, BIT(3)),
    AXP_DESC_RANGES(AXP2101, DCDC5, "dcdc5", "vin5", axp2101_dcdc5_ranges, 0x19,
                    AXP2101_DCDC5_CFG, 0x1f, AXP2101_DCDC_CFG0, BIT(4)),
    AXP_DESC_FIXED(AXP2101, LDO1, "rtcldo", "rtcldoin", 1800),
    AXP_DESC_FIXED(AXP2101, LDO2, "rtcldo1", "rtcldo1in", 1800),
    AXP_DESC(AXP2101, LDO3, "aldo1", "aldoin", 500, 3500, 100,
             AXP2101_ALDO1_CFG, 0x1f, AXP2101_LDO_EN_CFG0, BIT(0)),
    AXP_DESC(AXP2101, LDO4, "aldo2", "aldoin", 500, 3500, 100,
             AXP2101_ALDO2_CFG, 0x1f, AXP2101_LDO_EN_CFG0, BIT(1)),
    AXP_DESC(AXP2101, LDO5, "aldo3", "aldoin", 500, 3500, 100,
             AXP2101_ALDO3_CFG, 0x1f, AXP2101_LDO_EN_CFG0, BIT(2)),
    AXP_DESC(AXP2101, LDO6, "aldo4", "aldoin", 500, 3500, 100,
             AXP2101_ALDO4_CFG, 0x1f, AXP2101_LDO_EN_CFG0, BIT(3)),
    AXP_DESC(AXP2101, LDO7, "bldo1", "bldoin", 500, 3500, 100,
             AXP2101_BLDO1_CFG, 0x1f, AXP2101_LDO_EN_CFG0, BIT(4)),
    AXP_DESC(AXP2101, LDO8, "bldo2", "bldoin", 500, 3500, 100,
             AXP2101_BLDO2_CFG, 0x1f, AXP2101_LDO_EN_CFG0, BIT(5)),
    AXP_DESC(AXP2101, LDO9, "dldo1", "dldoin", 500, 3500, 100,
             AXP2101_DLDO1_CFG, 0x1f, AXP2101_LDO_EN_CFG0, BIT(7)),
    AXP_DESC(AXP2101, LDO10, "dldo2", "dldoin", 500, 1400, 50,
             AXP2101_DLDO2_CFG, 0x1f, AXP2101_LDO_EN_CFG1, BIT(0)),
    AXP_DESC(AXP2101, LDO11, "cpusldo", "cpusldoin", 500, 1400, 50,
             AXP2101_CPUSLD_CFG, 0x1f, AXP2101_LDO_EN_CFG0, BIT(6)),
};

static unsigned int axp20x_mvolt_to_cfg(int mvolt, int min, int max, int div)
{
    if (mvolt < min)
        mvolt = min;
    else if (mvolt > max)
        mvolt = max;

    return (mvolt - min) / div;
}

int axp20x_set_dcdc1(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_DCDC1 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, desc->min_mV, desc->max_mV,
                              desc->uV_step / 1000);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_dcdc2(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_DCDC2 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 1200, 10);
    if (mvolt >= 1220) cfg = axp20x_mvolt_to_cfg(mvolt, 1220, 1540, 20);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_dcdc3(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_DCDC3 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 1200, 10);
    if (mvolt >= 1220) cfg = axp20x_mvolt_to_cfg(mvolt, 1220, 1540, 20);
    if (mvolt >= 1600) cfg = axp20x_mvolt_to_cfg(mvolt, 1600, 3400, 100);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_dcdc4(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_DCDC4 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 1200, 10);
    if (mvolt >= 1220) cfg = axp20x_mvolt_to_cfg(mvolt, 1220, 1840, 20);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_dcdc5(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_DCDC5 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 1400, 3700, 100);
    if (mvolt == 1200) cfg = 0x19;

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_aldo1(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_LDO3 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 3500, 100);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_aldo2(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_LDO4 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 3500, 100);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_aldo3(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_LDO5 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 3500, 100);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_aldo4(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_LDO6 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 3500, 100);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_bldo1(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_LDO7 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 3500, 100);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_bldo2(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_LDO8 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 3500, 100);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_dldo1(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_LDO9 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 3500, 100);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_dldo2(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_LDO10 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 1400, 50);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

int axp20x_set_cpusldo(unsigned int mvolt)
{
    struct regulator_desc *desc = &axp2101_regulators[ AXP2101_LDO11 ];
    u32                    cfg;

    if (mvolt == 0) {
        return regulator_disable_regmap(desc);
    }

    cfg = axp20x_mvolt_to_cfg(mvolt, 500, 1400, 50);

    regulator_set_voltage_sel_regmap(desc, cfg);
    return regulator_enable_regmap(desc);
}

static int axp20x_set_dcdc_workmode(void *rdev, int id, u32 workmode)
{
    struct axp20x_dev     *axp20x = (struct axp20x_dev *) rdev;
    __nouse__ unsigned int reg    = AXP20X_DCDC_MODE;
    __nouse__ unsigned int mask;

    switch (axp20x->variant) {
    case AXP2101_ID:
        reg  = AXP2101_DCDC_CFG1;
        mask = GENMASK(5, 2);
        workmode <<= id + 2;
        break;
    default:
        /* should not happen */
        printf("Unsupported AXP variant: %ld in workmode\n", axp20x->variant);
        return -1;
    }

    // return regmap_update_bits(rdev->regmap, reg, mask, workmode);
    return 0;
}

int axp2101_regulator_probe(void *dev, void *config)
{
    struct axp20x_dev                     *axp20x = (struct axp20x_dev *) dev;
    __nouse__ const struct regulator_desc *regulators;

    __nouse__ int ret, i, nregulators;
    u32           workmode;
    __nouse__ u32 dval;

    switch (axp20x->variant) {
    case AXP2101_ID:
        regulators  = axp2101_regulators;
        nregulators = AXP2101_REG_ID_MAX;
        break;
    default:
        printf("Unsupported AXP variant: %ld in probe\n", axp20x->variant);
        return -1;
    }

    axp20x->rdev->desc = regulators;
    g_rdev_delay = &_rdev_delay;
    g_rdev_delay->step = 0;

    // "regulator-final-delay-us"
    g_rdev_delay->final = 0;

    axp20x->desc->reg_data = g_rdev_delay;

    // "x-powers,dcdc-workmode",
    workmode = 0;
    if (axp20x_set_dcdc_workmode(dev, axp20x->variant, workmode))
        printf("Failed to set workmode on %s\n", axp20x->rdev->desc->name);

    return 0;
}

void axp2101_regulator_remove(void)
{
    // if (g_rdev_delay != NULL) free(g_rdev_delay);
}
