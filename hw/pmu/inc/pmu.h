#ifndef __PMU_H__
#define __PMU_H__

#if defined M3
#include "_cm3_chip_define.h"
#else
#include "_ca55_chip_define.h"
#endif

#include "pmu_regs.h"
#include "ppu_regs.h"
#include <stdio.h>
#include <string.h>

#define DEBUG_PMU   1
#define PPU_CORE0_BASE      0x00000
#define PPU_CORE1_BASE      0x01000
#define PPU_CORE2_BASE      0x02000
#define PPU_CORE3_BASE      0x03000
#define PPU_AP_BASE         0x04000
#define PPU_VPU_BASE        0x05000
#define PPU_IMG_BASE        0x06000
#define PPU_DEWARP_BASE     0x07000
#define PPU_ISP_BASE        0x08000
#define PPU_DDR0_BASE       0x09000
#define PPU_DDR1_BASE       0x0a000
#define PPU_GPU_BASE        0x0b000
#define PPU_LP_BASE         0x0c000
#define PPU_NPU0_BASE       0x0d000
#define PPU_NPU1_BASE       0x0e000
#define PPU_PERI0_BASE      0x0f000
#define PPU_PERI1_BASE      0x10000
#define PMU_REG_BASE        0x11000

#define CORE0   0x0
#define CORE1   0x1
#define CORE2   0x2
#define CORE3   0x3
#define AP      0x4
#define VPU     0x5
#define IMG     0x6
#define DEWARP  0x7
#define ISP     0x8
#define DDR0    0x9
#define DDR1    0xa
#define GPU     0xb
#define LP      0xc
#define NPU0    0xd
#define NPU1    0xe
#define PERI0   0xf
#define PERI1   0x10
#define PMU     0x11
#define PDNUM   17
//PWR_POLICY
#define ON    0x8
#define OFF   0x0
#define RST   0x9
//OP_POLICY
#define OP0   0x0
#define OP1   0x1
#define OP2   0x2
#define OP3   0x3
#define OP4   0x4

extern char* get_pmu_name(uint8_t pid);
extern int get_pmu_base(uint8_t pid);
extern int get_pmu_isr(uint8_t pid);;
extern int get_pmu_pwsr(uint8_t pid, uint8_t stat);
extern int get_pmu_default_pwsr(uint8_t pid);
extern int get_pmu_cr(uint8_t pid, uint8_t stat);
extern int get_pmu_default_cr(uint8_t pid);
extern int check_pmu_irq(uint8_t pid, uint32_t isr, uint32_t aisr, uint8_t clr);
extern void set_pmu_reg(uint8_t pid, uint32_t addr, uint32_t value);
extern int  get_pmu_reg(uint8_t pid, uint32_t addr);
extern int  check_pmu_reg(uint8_t pid, uint32_t addr, uint32_t value);
extern void set_pmu_wakeup(uint8_t target);
extern void set_pmu_off_pd2on();
extern void set_pmu_power_on(uint8_t pid);
extern void set_pmu_power_off(uint8_t pid);
extern void set_pmu_warm_rst(uint8_t pid);
extern void clear_all_ppu_isr(void);

#ifdef FPGA
    #define TEST_PASS do { \
        printf("PASS\n"); \
        while(1) {} \
    } while(0)
    #define TEST_FAIL do { \
        printf("FAIL\n"); \
        while(1) {} \
    } while(0)
#else
    #define TEST_PASS GPASS
    #define TEST_FAIL FAIL
#endif

#endif // __PMU_H__

// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
