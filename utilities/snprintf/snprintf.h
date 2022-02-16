/* Copyright (c) 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */
/* SPDX-License-Identifier: GPL-2.0-or-later */
/* See the file LICENSE for further information */

#ifndef _SNPRINTF_H
#define _SNPRINTF_H


#ifndef __ASSEMBLER__

#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>


int mprintf(const char *format, ...);

int mputs(const char *s);

#endif /* !__ASSEMBLER__ */

#endif /* _SNPRINTF_H */
