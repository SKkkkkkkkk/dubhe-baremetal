#ifndef _CHIP_DEFINE_H_
#define _CHIP_DEFINE_H_
#ifdef __cplusplus
extern C {
#endif

#if defined M3
#include "cm3_chip_define.h"
#else
#include "ca55_chip_define.h"
#endif
#ifdef __cplusplus
    }
#endif

#include "regs_type.h"
#endif
