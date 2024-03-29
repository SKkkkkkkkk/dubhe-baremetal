// Generated by gen_regs.tcl
// DO NOT EDIT THIS FILE

#ifndef __IDMA_REGS_H__
#define __IDMA_REGS_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "systimer.h"

////////////////////////////////////////////////////////////////
// baisc reg info                                             //
////////////////////////////////////////////////////////////////

#define IDMA_SOFT_RST_ADDR              (IDMA_BASE+0x000)

#define IDMA_RST_STATUS_ADDR            (IDMA_BASE+0x004)

#define IDMA_CFG_IMG_WIDTH_ADDR         (IDMA_BASE+0x008)
#define IDMA_CFG_IMG_WIDTH_LSB          0
#define IDMA_CFG_IMG_WIDTH_MASK         0x00000fff

#define IDMA_CFG_IMG_HEIGHT_ADDR        (IDMA_BASE+0x008)
#define IDMA_CFG_IMG_HEIGHT_LSB         12
#define IDMA_CFG_IMG_HEIGHT_MASK        0x00fff000

#define IDMA_CFG_IN_OUT_FORMAT_ADDR     (IDMA_BASE+0x008)
#define IDMA_CFG_IN_OUT_FORMAT_LSB      24
#define IDMA_CFG_IN_OUT_FORMAT_MASK     0x3f000000

#define IDMA_CFG_IMG_STRIDE_ADDR        (IDMA_BASE+0x00c)

#define IDMA_CFG_ARLEN_ADDR             (IDMA_BASE+0x010)
#define IDMA_CFG_ARLEN_LSB              0
#define IDMA_CFG_ARLEN_MASK             0x00000007

#define IDMA_CFG_OUTSTANDING_ADDR       (IDMA_BASE+0x010)
#define IDMA_CFG_OUTSTANDING_LSB        3
#define IDMA_CFG_OUTSTANDING_MASK       0x00000038

#define IDMA_CFG_FRAME_BLANKING_ADDR    (IDMA_BASE+0x014)
#define IDMA_CFG_FRAME_BLANKING_LSB     0
#define IDMA_CFG_FRAME_BLANKING_MASK    0x00000fff

#define IDMA_CFG_LINE_BLANKING_ADDR     (IDMA_BASE+0x014)
#define IDMA_CFG_LINE_BLANKING_LSB      12
#define IDMA_CFG_LINE_BLANKING_MASK     0x00fff000

#define IDMA_EBD_LINES_ADDR             (IDMA_BASE+0x018)

#define IDMA_IDI_VC_ID_ADDR             (IDMA_BASE+0x01c)

#define IDMA_WORK_ADDR_ADDR             (IDMA_BASE+0x020)

#define IDMA_WORK_START_ADDR            (IDMA_BASE+0x024)

#define IDMA_FINISH_WORK_ADDR_ADDR      (IDMA_BASE+0x028)

#define IDMA_WORK_DONE_ADDR             (IDMA_BASE+0x100)
#define IDMA_WORK_DONE_LSB              0
#define IDMA_WORK_DONE_MASK             0x00000001

#define IDMA_WORK_LOST_ADDR             (IDMA_BASE+0x100)
#define IDMA_WORK_LOST_LSB              1
#define IDMA_WORK_LOST_MASK             0x00000002

#define IDMA_INT0_MASK_ADDR             (IDMA_BASE+0x104)

////////////////////////////////////////////////////////////////
// reg access                                                 //
////////////////////////////////////////////////////////////////
#define IDMA_REG32(name)                       (readl((void*)name##_ADDR))
#define IDMA_WEG32(name,x)                       (writel(x,(void*)name##_ADDR))

#define IDMA_GET_SOFT_RST               IDMA_REG32(IDMA_SOFT_RST)
#define IDMA_SET_SOFT_RST(x)            IDMA_WEG32(IDMA_SOFT_RST, (uint64_t)(x))
#define IDMA_GET_RST_STATUS             IDMA_REG32(IDMA_RST_STATUS)
#define IDMA_GET_CFG_IMG_STRIDE         IDMA_REG32(IDMA_CFG_IMG_STRIDE)
#define IDMA_SET_CFG_IMG_STRIDE(x)      IDMA_WEG32(IDMA_CFG_IMG_STRIDE, (uint64_t)(x))
#define IDMA_GET_EBD_LINES              IDMA_REG32(IDMA_EBD_LINES)
#define IDMA_SET_EBD_LINES(x)           IDMA_WEG32(IDMA_EBD_LINES, (uint64_t)(x))
#define IDMA_GET_IDI_VC_ID              IDMA_REG32(IDMA_IDI_VC_ID)
#define IDMA_SET_IDI_VC_ID(x)           IDMA_WEG32(IDMA_IDI_VC_ID, (uint64_t)(x))
#define IDMA_GET_WORK_ADDR              IDMA_REG32(IDMA_WORK_ADDR)
#define IDMA_SET_WORK_ADDR(x)           IDMA_WEG32(IDMA_WORK_ADDR, (uint64_t)(x))
#define IDMA_SET_WORK_START(x)          IDMA_WEG32(IDMA_WORK_START, (uint64_t)(x))
#define IDMA_GET_FINISH_WORK_ADDR       IDMA_REG32(IDMA_FINISH_WORK_ADDR)
#define IDMA_GET_INT0_MASK              IDMA_REG32(IDMA_INT0_MASK)
#define IDMA_SET_INT0_MASK(x)           IDMA_WEG32(IDMA_INT0_MASK, (uint64_t)(x))

////////////////////////////////////////////////////////////////
// reg bits access                                            //
////////////////////////////////////////////////////////////////
#define IDMA_GET_REG32_BITS(name)       ((IDMA_REG32(name) & name##_MASK) >> name##_LSB)
#define IDMA_SET_REG32_BITS(name,x)     do { \
    uint64_t val = IDMA_REG32(name); \
    val &= ~name##_MASK; \
    val |= ((x) << name##_LSB) & name##_MASK; \
    IDMA_WEG32(name, val); \
} while(0)

#define IDMA_GET_CFG_IMG_WIDTH          IDMA_GET_REG32_BITS(IDMA_CFG_IMG_WIDTH)
#define IDMA_SET_CFG_IMG_WIDTH(x)       IDMA_SET_REG32_BITS(IDMA_CFG_IMG_WIDTH,x)
#define IDMA_GET_CFG_IMG_HEIGHT         IDMA_GET_REG32_BITS(IDMA_CFG_IMG_HEIGHT)
#define IDMA_SET_CFG_IMG_HEIGHT(x)      IDMA_SET_REG32_BITS(IDMA_CFG_IMG_HEIGHT,x)
#define IDMA_GET_CFG_IN_OUT_FORMAT      IDMA_GET_REG32_BITS(IDMA_CFG_IN_OUT_FORMAT)
#define IDMA_SET_CFG_IN_OUT_FORMAT(x)   IDMA_SET_REG32_BITS(IDMA_CFG_IN_OUT_FORMAT,x)
#define IDMA_GET_CFG_ARLEN              IDMA_GET_REG32_BITS(IDMA_CFG_ARLEN)
#define IDMA_SET_CFG_ARLEN(x)           IDMA_SET_REG32_BITS(IDMA_CFG_ARLEN,x)
#define IDMA_GET_CFG_OUTSTANDING        IDMA_GET_REG32_BITS(IDMA_CFG_OUTSTANDING)
#define IDMA_SET_CFG_OUTSTANDING(x)     IDMA_SET_REG32_BITS(IDMA_CFG_OUTSTANDING,x)
#define IDMA_GET_CFG_FRAME_BLANKING     IDMA_GET_REG32_BITS(IDMA_CFG_FRAME_BLANKING)
#define IDMA_SET_CFG_FRAME_BLANKING(x)  IDMA_SET_REG32_BITS(IDMA_CFG_FRAME_BLANKING,x)
#define IDMA_GET_CFG_LINE_BLANKING      IDMA_GET_REG32_BITS(IDMA_CFG_LINE_BLANKING)
#define IDMA_SET_CFG_LINE_BLANKING(x)   IDMA_SET_REG32_BITS(IDMA_CFG_LINE_BLANKING,x)

////////////////////////////////////////////////////////////////
// reg group                                                  //
////////////////////////////////////////////////////////////////
#define IDMA_REGS_ADDR (IDMA_BASE+0x000)

typedef struct {
    // 0x000
    uint32_t soft_rst;
    // 0x004
    uint32_t rst_status;
    // 0x008
    uint32_t cfg_img_width      : 12;  // 11 :  0
    uint32_t cfg_img_height     : 12;  // 23 : 12
    uint32_t cfg_in_out_format  :  6;  // 29 : 24
    uint32_t _reserved0         :  2;  // 31 : 30
    // 0x00c
    uint32_t cfg_img_stride;
    // 0x010
    uint32_t cfg_arlen          :  3;  //  2 :  0
    uint32_t cfg_outstanding    :  3;  //  5 :  3
    uint32_t _reserved1         : 26;  // 31 :  6
    // 0x014
    uint32_t cfg_frame_blanking : 12;  // 11 :  0
    uint32_t cfg_line_blanking  : 12;  // 23 : 12
    uint32_t _reserved2         :  8;  // 31 : 24
    // 0x018
    uint32_t ebd_lines;
    // 0x01c
    uint32_t idi_vc_id;
    // 0x020
    uint32_t work_addr;
    // 0x024
    uint32_t _reserved3;
    // 0x028
    uint32_t finish_work_addr;
    // 0x02c
    uint32_t _reserved4;
    // 0x030
    uint32_t _reserved5;
    // 0x034
    uint32_t _reserved6;
    // 0x038
    uint32_t _reserved7;
    // 0x03c
    uint32_t _reserved8;
    // 0x040
    uint32_t _reserved9;
    // 0x044
    uint32_t _reserved10;
    // 0x048
    uint32_t _reserved11;
    // 0x04c
    uint32_t _reserved12;
    // 0x050
    uint32_t _reserved13;
    // 0x054
    uint32_t _reserved14;
    // 0x058
    uint32_t _reserved15;
    // 0x05c
    uint32_t _reserved16;
    // 0x060
    uint32_t _reserved17;
    // 0x064
    uint32_t _reserved18;
    // 0x068
    uint32_t _reserved19;
    // 0x06c
    uint32_t _reserved20;
    // 0x070
    uint32_t _reserved21;
    // 0x074
    uint32_t _reserved22;
    // 0x078
    uint32_t _reserved23;
    // 0x07c
    uint32_t _reserved24;
    // 0x080
    uint32_t _reserved25;
    // 0x084
    uint32_t _reserved26;
    // 0x088
    uint32_t _reserved27;
    // 0x08c
    uint32_t _reserved28;
    // 0x090
    uint32_t _reserved29;
    // 0x094
    uint32_t _reserved30;
    // 0x098
    uint32_t _reserved31;
    // 0x09c
    uint32_t _reserved32;
    // 0x0a0
    uint32_t _reserved33;
    // 0x0a4
    uint32_t _reserved34;
    // 0x0a8
    uint32_t _reserved35;
    // 0x0ac
    uint32_t _reserved36;
    // 0x0b0
    uint32_t _reserved37;
    // 0x0b4
    uint32_t _reserved38;
    // 0x0b8
    uint32_t _reserved39;
    // 0x0bc
    uint32_t _reserved40;
    // 0x0c0
    uint32_t _reserved41;
    // 0x0c4
    uint32_t _reserved42;
    // 0x0c8
    uint32_t _reserved43;
    // 0x0cc
    uint32_t _reserved44;
    // 0x0d0
    uint32_t _reserved45;
    // 0x0d4
    uint32_t _reserved46;
    // 0x0d8
    uint32_t _reserved47;
    // 0x0dc
    uint32_t _reserved48;
    // 0x0e0
    uint32_t _reserved49;
    // 0x0e4
    uint32_t _reserved50;
    // 0x0e8
    uint32_t _reserved51;
    // 0x0ec
    uint32_t _reserved52;
    // 0x0f0
    uint32_t _reserved53;
    // 0x0f4
    uint32_t _reserved54;
    // 0x0f8
    uint32_t _reserved55;
    // 0x0fc
    uint32_t _reserved56;
    // 0x100
    uint32_t work_done          :  1;  //  0 :  0
    uint32_t work_lost          :  1;  //  1 :  1
    uint32_t _reserved57        : 30;  // 31 :  2
    // 0x104
    uint32_t int0_mask;
} idma_regs;

#define IDMA_REGS ((volatile idma_regs*)IDMA_REGS_ADDR)

#endif // __IDMA_REGS_H__

// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
