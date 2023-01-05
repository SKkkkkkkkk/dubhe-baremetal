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
#include "i2c.h"
#include "utils_def.h"

struct axp20x_dev *axp20x;
struct axp20x_dev _axp20x;

static struct of_device_id axp20x_i2c_of_match[] = {
    {.compatible = "x-powers,axp152", .data = (void *) AXP152_ID},
    {.compatible = "x-powers,axp202", .data = (void *) AXP202_ID},
    {.compatible = "x-powers,axp209", .data = (void *) AXP209_ID},
    {.compatible = "x-powers,axp221", .data = (void *) AXP221_ID},
    {.compatible = "x-powers,axp2101", .data = (void *) AXP2101_ID},
    {.compatible = "x-powers,axp15", .data = (void *) AXP15_ID},
    {.compatible = "x-powers,axp1530", .data = (void *) AXP1530_ID},
    {.compatible = "x-powers,axp858", .data = (void *) AXP858_ID},
    {.compatible = "x-powers,axp803", .data = (void *) AXP803_ID},
    {.compatible = "x-powers,axp2202", .data = (void *) AXP2202_ID},
    {.compatible = "x-powers,axp806", .data = (void *) AXP806_ID},
    {.compatible = "x-powers,axp2585", .data = (void *) AXP2585_ID},
    {},
};

static int i2c_write_reg(struct pmic_cfg *cfg, uint16_t reg, u32 len, u32 val)
{
    i2c_msg msgs;
    int     ret  = 0;
    uint8_t tmp8 = 0;
    uint8_t tmp[ 2 ];

    msgs.addr     = cfg->reg_addr;
    msgs.flags    = RT_I2C_WR;
    msgs.reg_addr = reg;
    msgs.alen     = 1;
    if (len == 1) {
        tmp8     = (uint8_t) val;
        msgs.buf = &tmp8;
    } else if (len == 2) {
        tmp[ 0 ] = (uint8_t) val >> 8;
        tmp[ 1 ] = (uint8_t) val;
        msgs.buf = tmp;
    } else {
        printf("i2c write unkonw i2c len\n");
    }
    msgs.buf_len = len;
    ret          = i2c_master_transfer(cfg->i2c_bus, &msgs, 1);
    if (ret != 0) {
        printf("i2c_write_reg err\n");
        return -1;
    }

    return 0;
}

static int i2c_read_reg(struct pmic_cfg *cfg, uint16_t reg, unsigned int len,
                        u32 *val)
{
    i2c_msg msgs;
    int     ret = 0;
    uint8_t tmp[ 2 ];

    msgs.addr     = cfg->reg_addr;
    msgs.flags    = RT_I2C_RD;
    msgs.reg_addr = reg;
    msgs.alen     = 1;
    msgs.buf      = tmp;
    msgs.buf_len  = len;
    ret           = i2c_master_transfer(cfg->i2c_bus, &msgs, 1);
    if (ret != 0) {
        printf("i2c_read_reg err\n");
        return -1;
    }

    if (len == 1) {
        *val = tmp[ 0 ];
    } else if (len == 2) {
        *val = (uint16_t) (tmp[ 0 ] << 8 | tmp[ 1 ]);
    } else {
        printf("i2c read unkonw i2c len\n");
    }

    return 0;
}

inline int axp20x_i2c_write(u32 reg, u32 val)
{
    struct pmic_cfg *cfg = &axp20x->cfg;

    // printf("-->%s line %d i2c bus %d reg 0x%x val 0x%x\n", __func__,
    // __LINE__, cfg->i2c_bus, reg, val);
    return i2c_write_reg(cfg, reg, 1, val);
}

inline int axp20x_i2c_read(u32 reg, u32 *val)
{
    struct pmic_cfg *cfg = &axp20x->cfg;

    // printf("-->%s line %d i2c bus %d\n", __func__, __LINE__, cfg->i2c_bus);
    return i2c_read_reg(cfg, reg, 1, val);
}

static int axp20x_i2c_probe(void *i2c, void *id)
{
    __nouse__ int ret;

    axp20x = &_axp20x;
    if (!axp20x) return -1;

    axp20x->sts = STATUS_NONE;

    memcpy(&axp20x->cfg, i2c, sizeof(struct pmic_cfg));

    for (int i = 0; i < ARRAY_SIZE(axp20x_i2c_of_match); i++) {
        if (strcmp(axp20x_i2c_of_match[ i ].compatible, "x-powers,axp2101") ==
            0) {
            axp20x->variant = (long) axp20x_i2c_of_match[ i ].data;
            break;
        }
    }

    ret = axp20x_match_device(axp20x);
    if (ret) return ret;

    return axp20x_device_probe(axp20x);
}

static int axp20x_i2c_remove(void *i2c)
{
    axp20x_device_remove(axp20x);

    // if (axp20x != NULL) free(axp20x);

    return 0;
}

static int pmic_check(struct pmic_cfg *cfg)
{
    int           ret = 0;
    unsigned char buf[ 2 ];
    // unsigned short id = 0;
    i2c_msg msgs = {cfg->reg_addr, RT_I2C_RD,     cfg->check_addr, 1,
                    buf,           cfg->check_len};

    ret = i2c_master_transfer(cfg->i2c_bus, &msgs, 1);
    if (ret != 0) {
        printf("i2c_master_transfer failed, ret=%d\n", ret);
        ret = -1;
    }

    return ret;
}

int axp2101_i2c_init(struct pmic_cfg *cfg)
{
    int err = 0;

    if (cfg == NULL) {
        printf("%s pmic_cfg NULL err\n", __func__);
        err = -1;
        return err;
    }

    err = i2c_master_init(cfg->i2c_bus);
    if (err) {
        printf("%s i2c_master_init err\n", __func__);
        return err;
    }

    // i2c_control control = {0};
    // control.speed = 400000;
    // err = i2c_config_control(cfg->i2c_bus, &control);
    // if (err != 0) {
    // printf("i2c_config_control failed, ret=%d\r\n", err);
    // return -1;
    // }

    err = pmic_check(cfg);
    if (err) {
        printf("pmic check err\n");
        return err;
    }

    err = axp20x_i2c_probe(cfg, NULL);
    return err;
}

void axp2101_i2c_exit(struct pmic_cfg *cfg)
{
    i2c_master_deinit(cfg->i2c_bus);
    axp20x_i2c_remove(NULL);
}
