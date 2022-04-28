#ifndef __SEEHI_PRINT__
#define __SEEHI_PRINT__

#include <stdarg.h>
#include "snprintf.h"

// #define FPGA

void seehi_printf(const char *fmt, ...);
void simprintf(const char *fmt, ...);
int rw_check(int data,int addr);

#ifndef SIM
  #define seehi_printf(format, ...) mprintf(format, ##__VA_ARGS__)
#else
  #define seehi_printf(format, ...) simprintf(format, ##__VA_ARGS__)
#endif

// #ifdef FPGA
//   #define seehi_printf(format, ...) mprintf(format, ##__VA_ARGS__)
// #endif

// #ifdef SIM
//   #define seehi_printf(format, ...) mprintf(format, ##__VA_ARGS__)
// #else



#endif

