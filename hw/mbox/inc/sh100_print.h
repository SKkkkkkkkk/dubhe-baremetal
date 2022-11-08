#ifndef __SEEHI_PRINT_H__
#define __SEEHI_PRINT_H__

#include <stdarg.h> 
#include "seehi_print.h"
// #include "snprintf.h"

#define ELEVENTH_ARGUMENT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define COUNT_ARGUMENTS(...) ELEVENTH_ARGUMENT(dummy, ##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)



#if (FPGA == 1)
#define seehi_printf(fmt, ...) mprintf(format, ##__VA_ARGS__)
#else
#define seehi_printf(fmt, ...) _SHPRINT(COUNT_ARGUMENTS(__VA_ARGS__), __LINE__, ##__VA_ARGS__)
#endif

#if defined A55
/*{{{*/
#define seehi_cmd(d) asm volatile("mov w29,#0x0200; movk w29,#0x55aa,lsl #16;nop;mov w29, w0;nop"::"r"(d):"w29")
#define seehi_pass   asm volatile("mov w29,#0x0300; movk w29,#0x55aa,lsl #16;nop;mov w29, #0;nop":::"w29")
#define seehi_fail   asm volatile("mov w29,#0x0400; movk w29,#0x55aa,lsl #16;nop;mov w29, #0;nop":::"w29")

#define _SHPRINT(n, fmt, ...) _SHPRINTN(n)(fmt, ##__VA_ARGS__)
#define _SHPRINTN(n) _SHPRINT ## n
#define _SHPRINT_HEAD0 asm volatile("mov w29, #0x0100; movk w29,#0x55aa,lsl #16":::"w29")
#define _SHPRINT_HEAD1 asm volatile("mov w29, #0x0101; movk w29,#0x55aa,lsl #16":::"w29")
#define _SHPRINT_HEAD2 asm volatile("mov w29, #0x0102; movk w29,#0x55aa,lsl #16":::"w29")
#define _SHPRINT_HEAD3 asm volatile("mov w29, #0x0103; movk w29,#0x55aa,lsl #16":::"w29")
#define _SHPRINT_HEAD4 asm volatile("mov w29, #0x0104; movk w29,#0x55aa,lsl #16":::"w29")
#define _SHPRINT_HEAD5 asm volatile("mov w29, #0x0105; movk w29,#0x55aa,lsl #16":::"w29")
#define _SHPRINT_HEAD6 asm volatile("mov w29, #0x0106; movk w29,#0x55aa,lsl #16":::"w29")
#define _SHPRINT_HEAD7 asm volatile("mov w29, #0x0107; movk w29,#0x55aa,lsl #16":::"w29")
#define _SHPRINT_HEAD8 asm volatile("mov w29, #0x0108; movk w29,#0x55aa,lsl #16":::"w29")
#define _SHPRINT_HEAD9 asm volatile("mov w29, #0x0109; movk w29,#0x55aa,lsl #16":::"w29")
#define _SHPRINT_DATA(d) asm volatile("mov w29, w0"::"r"(d):"w29")
#define _SHPRINT_TAIL asm volatile("nop; mov w29, #0; nop":::"w29")
/*}}}*/
#elif defined M3// M3
/*{{{*/
#define seehi_cmd(d) asm volatile("movw r12,#0x0200; movt r12,#0x55aa;nop;mov r12, %0;nop"::"r"(d):"r12")
#define seehi_pass   asm volatile("movw r12,#0x0300; movt r12,#0x55aa;nop;mov r12, #0;nop":::"r12")
#define seehi_fail   asm volatile("movw r12,#0x0400; movt r12,#0x55aa;nop;mov r12, #0;nop":::"r12")

#define _SHPRINT(n, fmt, ...) _SHPRINTN(n)(fmt, ##__VA_ARGS__)
#define _SHPRINTN(n) _SHPRINT ## n
#define _SHPRINT_HEAD0 asm volatile("movw r12, #0x0100; movt r12,#0x55aa;":::"r12")
#define _SHPRINT_HEAD1 asm volatile("movw r12, #0x0101; movt r12,#0x55aa;":::"r12")
#define _SHPRINT_HEAD2 asm volatile("movw r12, #0x0102; movt r12,#0x55aa;":::"r12")
#define _SHPRINT_HEAD3 asm volatile("movw r12, #0x0103; movt r12,#0x55aa;":::"r12")
#define _SHPRINT_HEAD4 asm volatile("movw r12, #0x0104; movt r12,#0x55aa;":::"r12")
#define _SHPRINT_HEAD5 asm volatile("movw r12, #0x0105; movt r12,#0x55aa;":::"r12")
#define _SHPRINT_HEAD6 asm volatile("movw r12, #0x0106; movt r12,#0x55aa;":::"r12")
#define _SHPRINT_HEAD7 asm volatile("movw r12, #0x0107; movt r12,#0x55aa;":::"r12")
#define _SHPRINT_HEAD8 asm volatile("movw r12, #0x0108; movt r12,#0x55aa;":::"r12")
#define _SHPRINT_HEAD9 asm volatile("movw r12, #0x0109; movt r12,#0x55aa;":::"r12")
#define _SHPRINT_DATA(d) asm volatile("mov r12, %0"::"r"(d):"r12")
#define _SHPRINT_TAIL asm volatile("nop; mov r12, #0; nop":::"r12")

/*}}}*/
#else
/*{{{*/
#define seehi_cmd(d) asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x200; nop; mv x29, %0; nop"::"r"(d):"x29")
#define seehi_pass   asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x300; nop; li x29, 0; nop":::"x29")
#define seehi_fail   asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x400; nop; li x29, 0; nop":::"x29")

#define _SHPRINT(n, fmt, ...) _SHPRINTN(n)(fmt, ##__VA_ARGS__)
#define _SHPRINTN(n) _SHPRINT ## n
#define _SHPRINT_HEAD0 asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x100; ":::"x29")
#define _SHPRINT_HEAD1 asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x101; ":::"x29")
#define _SHPRINT_HEAD2 asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x102; ":::"x29")
#define _SHPRINT_HEAD3 asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x103; ":::"x29")
#define _SHPRINT_HEAD4 asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x104; ":::"x29")
#define _SHPRINT_HEAD5 asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x105; ":::"x29")
#define _SHPRINT_HEAD6 asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x106; ":::"x29")
#define _SHPRINT_HEAD7 asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x107; ":::"x29")
#define _SHPRINT_HEAD8 asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x108; ":::"x29")
#define _SHPRINT_HEAD9 asm volatile("lui x29, 0x55aa0; addi x29, x29, 0x109; ":::"x29")
#define _SHPRINT_DATA(d) asm volatile("mv x29, %0"::"r"(d):"x29")
#define _SHPRINT_TAIL asm volatile("nop; li x29, 0; nop":::"x29")
/*}}}*/
#endif
#define _SHPRINT0(fmt) do { \
    _SHPRINT_HEAD0; \
    _SHPRINT_DATA(fmt); \
    _SHPRINT_TAIL; \
} while(0)
#define _SHPRINT1(fmt, p0) do { \
    _SHPRINT_HEAD1; \
    _SHPRINT_DATA(fmt); \
    _SHPRINT_DATA(p0); \
    _SHPRINT_TAIL; \
} while(0)
#define _SHPRINT2(fmt, p0, p1) do { \
    _SHPRINT_HEAD2; \
    _SHPRINT_DATA(fmt); \
    _SHPRINT_DATA(p0); \
    _SHPRINT_DATA(p1); \
    _SHPRINT_TAIL; \
} while(0)
#define _SHPRINT3(fmt, p0, p1, p2) do { \
    _SHPRINT_HEAD3; \
    _SHPRINT_DATA(fmt); \
    _SHPRINT_DATA(p0); \
    _SHPRINT_DATA(p1); \
    _SHPRINT_DATA(p2); \
    _SHPRINT_TAIL; \
} while(0)
#define _SHPRINT4(fmt, p0, p1, p2, p3) do { \
    _SHPRINT_HEAD4; \
    _SHPRINT_DATA(fmt); \
    _SHPRINT_DATA(p0); \
    _SHPRINT_DATA(p1); \
    _SHPRINT_DATA(p2); \
    _SHPRINT_DATA(p3); \
    _SHPRINT_TAIL; \
} while(0)
#define _SHPRINT5(fmt, p0, p1, p2, p3, p4) do { \
    _SHPRINT_HEAD5; \
    _SHPRINT_DATA(fmt); \
    _SHPRINT_DATA(p0); \
    _SHPRINT_DATA(p1); \
    _SHPRINT_DATA(p2); \
    _SHPRINT_DATA(p3); \
    _SHPRINT_DATA(p4); \
    _SHPRINT_TAIL; \
} while(0)
#define _SHPRINT6(fmt, p0, p1, p2, p3, p4, p5) do { \
    _SHPRINT_HEAD6; \
    _SHPRINT_DATA(fmt); \
    _SHPRINT_DATA(p0); \
    _SHPRINT_DATA(p1); \
    _SHPRINT_DATA(p2); \
    _SHPRINT_DATA(p3); \
    _SHPRINT_DATA(p4); \
    _SHPRINT_DATA(p5); \
    _SHPRINT_TAIL; \
} while(0)
#define _SHPRINT7(fmt, p0, p1, p2, p3, p4, p5, p6) do { \
    _SHPRINT_HEAD7; \
    _SHPRINT_DATA(fmt); \
    _SHPRINT_DATA(p0); \
    _SHPRINT_DATA(p1); \
    _SHPRINT_DATA(p2); \
    _SHPRINT_DATA(p3); \
    _SHPRINT_DATA(p4); \
    _SHPRINT_DATA(p5); \
    _SHPRINT_DATA(p6); \
    _SHPRINT_TAIL; \
} while(0)
#define _SHPRINT8(fmt, p0, p1, p2, p3, p4, p5, p6, p7) do { \
    _SHPRINT_HEAD8; \
    _SHPRINT_DATA(fmt); \
    _SHPRINT_DATA(p0); \
    _SHPRINT_DATA(p1); \
    _SHPRINT_DATA(p2); \
    _SHPRINT_DATA(p3); \
    _SHPRINT_DATA(p4); \
    _SHPRINT_DATA(p5); \
    _SHPRINT_DATA(p6); \
    _SHPRINT_DATA(p7); \
    _SHPRINT_TAIL; \
} while(0)
#define _SHPRINT9(fmt, p0, p1, p2, p3, p4, p5, p6, p7, p8) do { \
    _SHPRINT_HEAD9; \
    _SHPRINT_DATA(fmt); \
    _SHPRINT_DATA(p0); \
    _SHPRINT_DATA(p1); \
    _SHPRINT_DATA(p2); \
    _SHPRINT_DATA(p3); \
    _SHPRINT_DATA(p4); \
    _SHPRINT_DATA(p5); \
    _SHPRINT_DATA(p6); \
    _SHPRINT_DATA(p7); \
    _SHPRINT_DATA(p8); \
    _SHPRINT_TAIL; \
} while(0)


// used in many testcases
#define writel(d, a) *(uint32_t volatile *)(uintptr_t)(a) = (d)
#define readl(a) (*(uint32_t volatile *)(uintptr_t)(a))

// misc
#define GPASS seehi_pass
#define PASS  seehi_pass
#define FAIL  seehi_fail
#define BACK_COMPDDR  seehi_cmd(0xf0000000)


#endif

// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
