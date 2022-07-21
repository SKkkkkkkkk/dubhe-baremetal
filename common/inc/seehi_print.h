#ifndef __SEEHI_PRINT_H__
#define __SEEHI_PRINT_H__

#include <stdarg.h>
#include <stdio.h>

#if defined(RTL)
	#include "rtl_print.h"
	#define seehi_printf(fmt, ...)
#else
	#define seehi_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

#endif