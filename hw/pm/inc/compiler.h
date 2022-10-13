/*
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

/* GNU Compiler */

#include "defs.h"

// #define inline    inline
#define __inline   inline
#define __inline__ inline

#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif

#ifndef __noinline
#define __noinline __attribute__((__noinline__))
#endif

#define __packed __attribute__((__packed__))
#define __asm    asm
#define __weak   __attribute__((weak))

#ifdef CONFIG_SECTION_ATTRIBUTE_XIP
#define __xip_text \
    __attribute__((section(".xip_text"))) __attribute__((aligned(16)))
#define __xip_rodata \
    __attribute__((section(".xip_rodata"))) __attribute__((aligned(16)))
#else
#define __xip_text
#define __xip_rodata
#endif

#ifdef CONFIG_SECTION_ATTRIBUTE_NONXIP
#define __nonxip_text   __attribute__((section(".nonxip_text")))
#define __nonxip_rodata __attribute__((section(".nonxip_rodata")))
#define __nonxip_data   __attribute__((section(".nonxip_data")))
#define __nonxip_bss    __attribute__((section(".nonxip_bss")))
#else
#define __nonxip_text
#define __nonxip_rodata
#define __nonxip_data
#define __nonxip_bss
#endif

#ifdef CONFIG_SECTION_ATTRIBUTE_SRAM
#define __sram_text   __attribute__((section(".sram_text")))
#define __sram_rodata __attribute__((section(".sram_rodata")))
#define __sram_data   __attribute__((section(".sram_data")))
#define __sram_bss    __attribute__((section(".sram_bss")))
#else
#define __sram_text
#define __sram_rodata
#define __sram_data
#define __sram_bss
#endif

#ifdef CONFIG_SECTION_ATTRIBUTE_PSRAM
#define __psram_text \
    __attribute__((section(".psram_text"))) __attribute__((aligned(16)))
#define __psram_rodata \
    __attribute__((section(".psram_rodata"))) __attribute__((aligned(16)))
#define __psram_data \
    __attribute__((section(".psram_data"))) __attribute__((aligned(16)))
#define __psram_bss \
    __attribute__((section(".psram_bss"))) __attribute__((aligned(16)))
#else
#define __psram_text
#define __psram_rodata
#define __psram_data
#define __psram_bss
#endif

#endif /* _COMPILER_H_ */
