#ifndef __DUBHE_PINMUX_H__
#define __DUBHE_PINMUX_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
    extern "C" {
#endif

void pinmux(uint8_t pin_num, uint8_t iomux);

#ifdef __cplusplus
    }
#endif

#endif