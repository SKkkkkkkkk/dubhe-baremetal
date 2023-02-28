#ifndef QEMU
#include <stdio.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
#include "pmu.h"
#include "_ca55_chip_define.h"
#include "dma.h"
#include "ddr.h"
#include "regconfig_sim_CTL.h"
#include "regconfig_sim_PHY.h"

int gic_cnt = 0;
int err_cnt = 0;

// #define USE_SW_RST

#define DDR_CFG_BASE                     0x02b000000

typedef unsigned long ulong;
typedef volatile unsigned long vu_long;

#define reg32(addr) (*(volatile uint32_t *)(uintptr_t)(addr))

void delay(unsigned int dly){
//    int i=0,j=0,k=0;
//    for(i=0;i<dly;i++){
//        for(j=0;j<10;j++){
//            for(k=0;k<1000;k++);
//        }
//    }
}

int lpddr4_mode = 1;

#define TRAIN_DDR_DELAY_MIN_RD 0x00
#define TRAIN_DDR_DELAY_MAX_RD 0x1ff
#define TRAIN_DDR_DELAY_STEP_RD 10

void REGWR(unsigned int addr, unsigned int data, unsigned char mask){//{{{
    reg32(DDR_CFG_BASE + addr * 4) = data;
}//}}}

unsigned int REGREAD(unsigned int addr){//{{{
    return reg32(DDR_CFG_BASE + addr * 4);
}//}}}

void SET_PARAM(unsigned int addr, unsigned int offset, unsigned int width, unsigned int val){//{{{
    unsigned int tmp, tmp1;
    tmp1 = 0;
    tmp = reg32(DDR_CFG_BASE + addr * 4);
    for(int i=0; i<width; i++){
        tmp1 = (tmp1 | (1<<i));
    }
    tmp = (tmp & (~(tmp1<<offset))) | ((tmp1<<offset) & (val<<offset));
    reg32(DDR_CFG_BASE + addr * 4) = tmp;
}//}}}

void PI_INIT_WOLVL(){//{{{
    if (lpddr4_mode) {
        SET_PARAM(PI_INIT_LVL_EN_ADDR, PI_INIT_LVL_EN_OFFSET, PI_INIT_LVL_EN_WIDTH, 0);
    }
}//}}}

void PAD_CAL_FAST_SETUP(){//{{{
    SET_PARAM(PHY_CAL_CPTR_CNT_0_ADDR, PHY_CAL_CPTR_CNT_0_OFFSET, PHY_CAL_CPTR_CNT_0_WIDTH, 1); 
    if (lpddr4_mode) {
      SET_PARAM(PHY_CAL_CLK_SELECT_0_ADDR, PHY_CAL_CLK_SELECT_0_OFFSET, PHY_CAL_CLK_SELECT_0_WIDTH, 0); 
      SET_PARAM(PHY_LP4_BOOT_CAL_CLK_SELECT_0_ADDR, PHY_LP4_BOOT_CAL_CLK_SELECT_0_OFFSET, PHY_LP4_BOOT_CAL_CLK_SELECT_0_WIDTH, 0); 
    }
} //}}} 

void START(int pi_init_lvl_on){//{{{
    unsigned int regdata;
    SET_PARAM(SWAP_EN_ADDR, SWAP_EN_OFFSET, SWAP_EN_WIDTH, 0);
    SET_PARAM(IN_ORDER_ACCEPT_ADDR, IN_ORDER_ACCEPT_OFFSET, IN_ORDER_ACCEPT_WIDTH, 1);
    // Disable write reordering for verilog tests to avoid issues with SRAM
    SET_PARAM(WR_ORDER_REQ_ADDR, WR_ORDER_REQ_OFFSET, WR_ORDER_REQ_WIDTH, 2);

    // For verilog patterns, keep the init freq same as boot freq
    SET_PARAM(INIT_FREQ_ADDR, INIT_FREQ_OFFSET, INIT_FREQ_WIDTH, 0);

    // Reduce time for IO pad calibration
//    PAD_CAL_FAST_SETUP();
    // Disable Inline ECC for verilog tests as the testbench doesn't handle
    // the addressing requirements.
    SET_PARAM(ECC_ENABLE_ADDR, ECC_ENABLE_OFFSET, ECC_ENABLE_WIDTH, 0);

//    if (!pi_init_lvl_on) {
//       SET_PARAM(PI_INIT_LVL_EN_ADDR, PI_INIT_LVL_EN_OFFSET, PI_INIT_LVL_EN_WIDTH, 0);
//       SET_PARAM(PI_START_ADDR, PI_START_OFFSET, PI_START_WIDTH, 1);
//    } 
//    else {
//       SET_PARAM(PI_START_ADDR, PI_START_OFFSET, PI_START_WIDTH, 1);
//    }


//    regdata = reg32(DDR_CFG_BASE + PI_START_ADDR*4);
//    if (!(regdata & 0x1)) {
//        if (lpddr4_mode) {
//                SET_PARAM(DFIBUS_BOOT_FREQ_ADDR, DFIBUS_BOOT_FREQ_OFFSET, DFIBUS_BOOT_FREQ_WIDTH, 2); 
//                SET_PARAM(PHY_LP4_BOOT_DISABLE_ADDR, PHY_LP4_BOOT_DISABLE_OFFSET, PHY_LP4_BOOT_DISABLE_WIDTH, 1);
//        }
//    }

//   //override work frequency 0 PLL setting, make sure PLL can lock at 400MHz(F0 always is 400MHz)
//    SET_PARAM(PHY_FREQ_SEL_MULTICAST_EN_ADDR, PHY_FREQ_SEL_MULTICAST_EN_OFFSET, PHY_FREQ_SEL_MULTICAST_EN_WIDTH, 0);
//    SET_PARAM(PHY_PLL_CTRL_ADDR, PHY_PLL_CTRL_OFFSET, PHY_PLL_CTRL_WIDTH, 0x1142);
//    SET_PARAM(PHY_FREQ_SEL_MULTICAST_EN_ADDR, PHY_FREQ_SEL_MULTICAST_EN_OFFSET, PHY_FREQ_SEL_MULTICAST_EN_WIDTH, 1);

    // Asserting param_start
    regdata = reg32(DDR_CFG_BASE + START_ADDR*4);
    regdata = regdata | 0x1;
    REGWR(START_ADDR,regdata,0);


    //wait(TIER_PHY_TOP.cdn_hs_phy.inst_pi.dfi_init_complete_phy);
    //wait(test_top.u_dut.u_ddr_sys_wrapper.u_ddr_top.u_ddr_subsystem.cdn_hs_phy_top.cdn_hs_phy.inst_pi.dfi_init_complete_phy);
    while(1) {
        unsigned int tmp;
        //tmp = reg32(0x554);
        tmp = reg32(DDR_CFG_BASE + 0x550);
        if((tmp>>25)&0x1){
            printf("START Done\n");
            break;
        }
    }

    //repeat (100) @(posedge clk);
    delay(1);
}//}}}

void config_ctl_pi(){//{{{
#ifdef DENALI_CTL_00_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(0,DENALI_CTL_00_DATA,0);
       #else
         REGWR(0,DENALI_CTL_00_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_01_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(1,DENALI_CTL_01_DATA,0);
       #else
         REGWR(1,DENALI_CTL_01_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_02_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(2,DENALI_CTL_02_DATA,0);
       #else
         REGWR(2,DENALI_CTL_02_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_03_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(3,DENALI_CTL_03_DATA,0);
       #else
         REGWR(3,DENALI_CTL_03_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_04_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(4,DENALI_CTL_04_DATA,0);
       #else
         REGWR(4,DENALI_CTL_04_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_05_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(5,DENALI_CTL_05_DATA,0);
       #else
         REGWR(5,DENALI_CTL_05_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_06_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(6,DENALI_CTL_06_DATA,0);
       #else
         REGWR(6,DENALI_CTL_06_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_07_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(7,DENALI_CTL_07_DATA,0);
       #else
         REGWR(7,DENALI_CTL_07_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_08_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(8,DENALI_CTL_08_DATA,0);
       #else
         REGWR(8,DENALI_CTL_08_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_09_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(9,DENALI_CTL_09_DATA,0);
       #else
         REGWR(9,DENALI_CTL_09_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_10_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(10,DENALI_CTL_10_DATA,0);
       #else
         REGWR(10,DENALI_CTL_10_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_11_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(11,DENALI_CTL_11_DATA,0);
       #else
         REGWR(11,DENALI_CTL_11_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_12_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(12,DENALI_CTL_12_DATA,0);
       #else
         REGWR(12,DENALI_CTL_12_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_13_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(13,DENALI_CTL_13_DATA,0);
       #else
         REGWR(13,DENALI_CTL_13_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_14_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(14,DENALI_CTL_14_DATA,0);
       #else
         REGWR(14,DENALI_CTL_14_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_15_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(15,DENALI_CTL_15_DATA,0);
       #else
         REGWR(15,DENALI_CTL_15_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_16_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(16,DENALI_CTL_16_DATA,0);
       #else
         REGWR(16,DENALI_CTL_16_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_17_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(17,DENALI_CTL_17_DATA,0);
       #else
         REGWR(17,DENALI_CTL_17_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_18_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(18,DENALI_CTL_18_DATA,0);
       #else
         REGWR(18,DENALI_CTL_18_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_19_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(19,DENALI_CTL_19_DATA,0);
       #else
         REGWR(19,DENALI_CTL_19_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_20_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(20,DENALI_CTL_20_DATA,0);
       #else
         REGWR(20,DENALI_CTL_20_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_21_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(21,DENALI_CTL_21_DATA,0);
       #else
         REGWR(21,DENALI_CTL_21_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_22_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(22,DENALI_CTL_22_DATA,0);
       #else
         REGWR(22,DENALI_CTL_22_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_23_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(23,DENALI_CTL_23_DATA,0);
       #else
         REGWR(23,DENALI_CTL_23_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_24_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(24,DENALI_CTL_24_DATA,0);
       #else
         REGWR(24,DENALI_CTL_24_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_25_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(25,DENALI_CTL_25_DATA,0);
       #else
         REGWR(25,DENALI_CTL_25_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_26_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(26,DENALI_CTL_26_DATA,0);
       #else
         REGWR(26,DENALI_CTL_26_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_27_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(27,DENALI_CTL_27_DATA,0);
       #else
         REGWR(27,DENALI_CTL_27_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_28_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(28,DENALI_CTL_28_DATA,0);
       #else
         REGWR(28,DENALI_CTL_28_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_29_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(29,DENALI_CTL_29_DATA,0);
       #else
         REGWR(29,DENALI_CTL_29_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_30_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(30,DENALI_CTL_30_DATA,0);
       #else
         REGWR(30,DENALI_CTL_30_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_31_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(31,DENALI_CTL_31_DATA,0);
       #else
         REGWR(31,DENALI_CTL_31_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_32_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(32,DENALI_CTL_32_DATA,0);
       #else
         REGWR(32,DENALI_CTL_32_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_33_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(33,DENALI_CTL_33_DATA,0);
       #else
         REGWR(33,DENALI_CTL_33_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_34_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(34,DENALI_CTL_34_DATA,0);
       #else
         REGWR(34,DENALI_CTL_34_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_35_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(35,DENALI_CTL_35_DATA,0);
       #else
         REGWR(35,DENALI_CTL_35_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_36_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(36,DENALI_CTL_36_DATA,0);
       #else
         REGWR(36,DENALI_CTL_36_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_37_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(37,DENALI_CTL_37_DATA,0);
       #else
         REGWR(37,DENALI_CTL_37_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_38_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(38,DENALI_CTL_38_DATA,0);
       #else
         REGWR(38,DENALI_CTL_38_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_39_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(39,DENALI_CTL_39_DATA,0);
       #else
         REGWR(39,DENALI_CTL_39_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_40_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(40,DENALI_CTL_40_DATA,0);
       #else
         REGWR(40,DENALI_CTL_40_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_41_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(41,DENALI_CTL_41_DATA,0);
       #else
         REGWR(41,DENALI_CTL_41_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_42_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(42,DENALI_CTL_42_DATA,0);
       #else
         REGWR(42,DENALI_CTL_42_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_43_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(43,DENALI_CTL_43_DATA,0);
       #else
         REGWR(43,DENALI_CTL_43_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_44_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(44,DENALI_CTL_44_DATA,0);
       #else
         REGWR(44,DENALI_CTL_44_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_45_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(45,DENALI_CTL_45_DATA,0);
       #else
         REGWR(45,DENALI_CTL_45_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_46_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(46,DENALI_CTL_46_DATA,0);
       #else
         REGWR(46,DENALI_CTL_46_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_47_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(47,DENALI_CTL_47_DATA,0);
       #else
         REGWR(47,DENALI_CTL_47_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_48_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(48,DENALI_CTL_48_DATA,0);
       #else
         REGWR(48,DENALI_CTL_48_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_49_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(49,DENALI_CTL_49_DATA,0);
       #else
         REGWR(49,DENALI_CTL_49_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_50_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(50,DENALI_CTL_50_DATA,0);
       #else
         REGWR(50,DENALI_CTL_50_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_51_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(51,DENALI_CTL_51_DATA,0);
       #else
         REGWR(51,DENALI_CTL_51_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_52_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(52,DENALI_CTL_52_DATA,0);
       #else
         REGWR(52,DENALI_CTL_52_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_53_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(53,DENALI_CTL_53_DATA,0);
       #else
         REGWR(53,DENALI_CTL_53_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_54_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(54,DENALI_CTL_54_DATA,0);
       #else
         REGWR(54,DENALI_CTL_54_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_55_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(55,DENALI_CTL_55_DATA,0);
       #else
         REGWR(55,DENALI_CTL_55_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_56_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(56,DENALI_CTL_56_DATA,0);
       #else
         REGWR(56,DENALI_CTL_56_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_57_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(57,DENALI_CTL_57_DATA,0);
       #else
         REGWR(57,DENALI_CTL_57_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_58_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(58,DENALI_CTL_58_DATA,0);
       #else
         REGWR(58,DENALI_CTL_58_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_59_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(59,DENALI_CTL_59_DATA,0);
       #else
         REGWR(59,DENALI_CTL_59_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_60_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(60,DENALI_CTL_60_DATA,0);
       #else
         REGWR(60,DENALI_CTL_60_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_61_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(61,DENALI_CTL_61_DATA,0);
       #else
         REGWR(61,DENALI_CTL_61_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_62_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(62,DENALI_CTL_62_DATA,0);
       #else
         REGWR(62,DENALI_CTL_62_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_63_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(63,DENALI_CTL_63_DATA,0);
       #else
         REGWR(63,DENALI_CTL_63_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_64_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(64,DENALI_CTL_64_DATA,0);
       #else
         REGWR(64,DENALI_CTL_64_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_65_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(65,DENALI_CTL_65_DATA,0);
       #else
         REGWR(65,DENALI_CTL_65_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_66_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(66,DENALI_CTL_66_DATA,0);
       #else
         REGWR(66,DENALI_CTL_66_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_67_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(67,DENALI_CTL_67_DATA,0);
       #else
         REGWR(67,DENALI_CTL_67_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_68_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(68,DENALI_CTL_68_DATA,0);
       #else
         REGWR(68,DENALI_CTL_68_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_69_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(69,DENALI_CTL_69_DATA,0);
       #else
         REGWR(69,DENALI_CTL_69_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_70_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(70,DENALI_CTL_70_DATA,0);
       #else
         REGWR(70,DENALI_CTL_70_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_71_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(71,DENALI_CTL_71_DATA,0);
       #else
         REGWR(71,DENALI_CTL_71_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_72_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(72,DENALI_CTL_72_DATA,0);
       #else
         REGWR(72,DENALI_CTL_72_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_73_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(73,DENALI_CTL_73_DATA,0);
       #else
         REGWR(73,DENALI_CTL_73_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_74_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(74,DENALI_CTL_74_DATA,0);
       #else
         REGWR(74,DENALI_CTL_74_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_75_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(75,DENALI_CTL_75_DATA,0);
       #else
         REGWR(75,DENALI_CTL_75_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_76_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(76,DENALI_CTL_76_DATA,0);
       #else
         REGWR(76,DENALI_CTL_76_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_77_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(77,DENALI_CTL_77_DATA,0);
       #else
         REGWR(77,DENALI_CTL_77_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_78_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(78,DENALI_CTL_78_DATA,0);
       #else
         REGWR(78,DENALI_CTL_78_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_79_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(79,DENALI_CTL_79_DATA,0);
       #else
         REGWR(79,DENALI_CTL_79_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_80_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(80,DENALI_CTL_80_DATA,0);
       #else
         REGWR(80,DENALI_CTL_80_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_81_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(81,DENALI_CTL_81_DATA,0);
       #else
         REGWR(81,DENALI_CTL_81_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_82_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(82,DENALI_CTL_82_DATA,0);
       #else
         REGWR(82,DENALI_CTL_82_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_83_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(83,DENALI_CTL_83_DATA,0);
       #else
         REGWR(83,DENALI_CTL_83_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_84_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(84,DENALI_CTL_84_DATA,0);
       #else
         REGWR(84,DENALI_CTL_84_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_85_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(85,DENALI_CTL_85_DATA,0);
       #else
         REGWR(85,DENALI_CTL_85_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_86_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(86,DENALI_CTL_86_DATA,0);
       #else
         REGWR(86,DENALI_CTL_86_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_87_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(87,DENALI_CTL_87_DATA,0);
       #else
         REGWR(87,DENALI_CTL_87_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_88_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(88,DENALI_CTL_88_DATA,0);
       #else
         REGWR(88,DENALI_CTL_88_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_89_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(89,DENALI_CTL_89_DATA,0);
       #else
         REGWR(89,DENALI_CTL_89_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_90_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(90,DENALI_CTL_90_DATA,0);
       #else
         REGWR(90,DENALI_CTL_90_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_91_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(91,DENALI_CTL_91_DATA,0);
       #else
         REGWR(91,DENALI_CTL_91_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_92_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(92,DENALI_CTL_92_DATA,0);
       #else
         REGWR(92,DENALI_CTL_92_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_93_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(93,DENALI_CTL_93_DATA,0);
       #else
         REGWR(93,DENALI_CTL_93_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_94_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(94,DENALI_CTL_94_DATA,0);
       #else
         REGWR(94,DENALI_CTL_94_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_95_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(95,DENALI_CTL_95_DATA,0);
       #else
         REGWR(95,DENALI_CTL_95_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_96_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(96,DENALI_CTL_96_DATA,0);
       #else
         REGWR(96,DENALI_CTL_96_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_97_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(97,DENALI_CTL_97_DATA,0);
       #else
         REGWR(97,DENALI_CTL_97_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_98_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(98,DENALI_CTL_98_DATA,0);
       #else
         REGWR(98,DENALI_CTL_98_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_99_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(99,DENALI_CTL_99_DATA,0);
       #else
         REGWR(99,DENALI_CTL_99_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_100_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(100,DENALI_CTL_100_DATA,0);
       #else
         REGWR(100,DENALI_CTL_100_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_101_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(101,DENALI_CTL_101_DATA,0);
       #else
         REGWR(101,DENALI_CTL_101_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_102_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(102,DENALI_CTL_102_DATA,0);
       #else
         REGWR(102,DENALI_CTL_102_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_103_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(103,DENALI_CTL_103_DATA,0);
       #else
         REGWR(103,DENALI_CTL_103_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_104_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(104,DENALI_CTL_104_DATA,0);
       #else
         REGWR(104,DENALI_CTL_104_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_105_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(105,DENALI_CTL_105_DATA,0);
       #else
         REGWR(105,DENALI_CTL_105_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_106_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(106,DENALI_CTL_106_DATA,0);
       #else
         REGWR(106,DENALI_CTL_106_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_107_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(107,DENALI_CTL_107_DATA,0);
       #else
         REGWR(107,DENALI_CTL_107_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_108_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(108,DENALI_CTL_108_DATA,0);
       #else
         REGWR(108,DENALI_CTL_108_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_109_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(109,DENALI_CTL_109_DATA,0);
       #else
         REGWR(109,DENALI_CTL_109_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_110_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(110,DENALI_CTL_110_DATA,0);
       #else
         REGWR(110,DENALI_CTL_110_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_111_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(111,DENALI_CTL_111_DATA,0);
       #else
         REGWR(111,DENALI_CTL_111_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_112_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(112,DENALI_CTL_112_DATA,0);
       #else
         REGWR(112,DENALI_CTL_112_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_113_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(113,DENALI_CTL_113_DATA,0);
       #else
         REGWR(113,DENALI_CTL_113_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_114_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(114,DENALI_CTL_114_DATA,0);
       #else
         REGWR(114,DENALI_CTL_114_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_115_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(115,DENALI_CTL_115_DATA,0);
       #else
         REGWR(115,DENALI_CTL_115_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_116_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(116,DENALI_CTL_116_DATA,0);
       #else
         REGWR(116,DENALI_CTL_116_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_117_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(117,DENALI_CTL_117_DATA,0);
       #else
         REGWR(117,DENALI_CTL_117_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_118_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(118,DENALI_CTL_118_DATA,0);
       #else
         REGWR(118,DENALI_CTL_118_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_119_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(119,DENALI_CTL_119_DATA,0);
       #else
         REGWR(119,DENALI_CTL_119_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_120_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(120,DENALI_CTL_120_DATA,0);
       #else
         REGWR(120,DENALI_CTL_120_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_121_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(121,DENALI_CTL_121_DATA,0);
       #else
         REGWR(121,DENALI_CTL_121_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_122_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(122,DENALI_CTL_122_DATA,0);
       #else
         REGWR(122,DENALI_CTL_122_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_123_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(123,DENALI_CTL_123_DATA,0);
       #else
         REGWR(123,DENALI_CTL_123_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_124_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(124,DENALI_CTL_124_DATA,0);
       #else
         REGWR(124,DENALI_CTL_124_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_125_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(125,DENALI_CTL_125_DATA,0);
       #else
         REGWR(125,DENALI_CTL_125_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_126_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(126,DENALI_CTL_126_DATA,0);
       #else
         REGWR(126,DENALI_CTL_126_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_127_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(127,DENALI_CTL_127_DATA,0);
       #else
         REGWR(127,DENALI_CTL_127_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_128_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(128,DENALI_CTL_128_DATA,0);
       #else
         REGWR(128,DENALI_CTL_128_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_129_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(129,DENALI_CTL_129_DATA,0);
       #else
         REGWR(129,DENALI_CTL_129_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_130_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(130,DENALI_CTL_130_DATA,0);
       #else
         REGWR(130,DENALI_CTL_130_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_131_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(131,DENALI_CTL_131_DATA,0);
       #else
         REGWR(131,DENALI_CTL_131_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_132_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(132,DENALI_CTL_132_DATA,0);
       #else
         REGWR(132,DENALI_CTL_132_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_133_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(133,DENALI_CTL_133_DATA,0);
       #else
         REGWR(133,DENALI_CTL_133_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_134_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(134,DENALI_CTL_134_DATA,0);
       #else
         REGWR(134,DENALI_CTL_134_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_135_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(135,DENALI_CTL_135_DATA,0);
       #else
         REGWR(135,DENALI_CTL_135_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_136_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(136,DENALI_CTL_136_DATA,0);
       #else
         REGWR(136,DENALI_CTL_136_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_137_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(137,DENALI_CTL_137_DATA,0);
       #else
         REGWR(137,DENALI_CTL_137_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_138_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(138,DENALI_CTL_138_DATA,0);
       #else
         REGWR(138,DENALI_CTL_138_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_139_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(139,DENALI_CTL_139_DATA,0);
       #else
         REGWR(139,DENALI_CTL_139_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_140_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(140,DENALI_CTL_140_DATA,0);
       #else
         REGWR(140,DENALI_CTL_140_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_141_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(141,DENALI_CTL_141_DATA,0);
       #else
         REGWR(141,DENALI_CTL_141_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_142_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(142,DENALI_CTL_142_DATA,0);
       #else
         REGWR(142,DENALI_CTL_142_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_143_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(143,DENALI_CTL_143_DATA,0);
       #else
         REGWR(143,DENALI_CTL_143_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_144_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(144,DENALI_CTL_144_DATA,0);
       #else
         REGWR(144,DENALI_CTL_144_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_145_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(145,DENALI_CTL_145_DATA,0);
       #else
         REGWR(145,DENALI_CTL_145_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_146_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(146,DENALI_CTL_146_DATA,0);
       #else
         REGWR(146,DENALI_CTL_146_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_147_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(147,DENALI_CTL_147_DATA,0);
       #else
         REGWR(147,DENALI_CTL_147_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_148_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(148,DENALI_CTL_148_DATA,0);
       #else
         REGWR(148,DENALI_CTL_148_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_149_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(149,DENALI_CTL_149_DATA,0);
       #else
         REGWR(149,DENALI_CTL_149_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_150_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(150,DENALI_CTL_150_DATA,0);
       #else
         REGWR(150,DENALI_CTL_150_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_151_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(151,DENALI_CTL_151_DATA,0);
       #else
         REGWR(151,DENALI_CTL_151_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_152_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(152,DENALI_CTL_152_DATA,0);
       #else
         REGWR(152,DENALI_CTL_152_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_153_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(153,DENALI_CTL_153_DATA,0);
       #else
         REGWR(153,DENALI_CTL_153_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_154_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(154,DENALI_CTL_154_DATA,0);
       #else
         REGWR(154,DENALI_CTL_154_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_155_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(155,DENALI_CTL_155_DATA,0);
       #else
         REGWR(155,DENALI_CTL_155_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_156_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(156,DENALI_CTL_156_DATA,0);
       #else
         REGWR(156,DENALI_CTL_156_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_157_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(157,DENALI_CTL_157_DATA,0);
       #else
         REGWR(157,DENALI_CTL_157_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_158_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(158,DENALI_CTL_158_DATA,0);
       #else
         REGWR(158,DENALI_CTL_158_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_159_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(159,DENALI_CTL_159_DATA,0);
       #else
         REGWR(159,DENALI_CTL_159_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_160_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(160,DENALI_CTL_160_DATA,0);
       #else
         REGWR(160,DENALI_CTL_160_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_161_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(161,DENALI_CTL_161_DATA,0);
       #else
         REGWR(161,DENALI_CTL_161_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_162_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(162,DENALI_CTL_162_DATA,0);
       #else
         REGWR(162,DENALI_CTL_162_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_163_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(163,DENALI_CTL_163_DATA,0);
       #else
         REGWR(163,DENALI_CTL_163_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_164_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(164,DENALI_CTL_164_DATA,0);
       #else
         REGWR(164,DENALI_CTL_164_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_165_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(165,DENALI_CTL_165_DATA,0);
       #else
         REGWR(165,DENALI_CTL_165_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_166_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(166,DENALI_CTL_166_DATA,0);
       #else
         REGWR(166,DENALI_CTL_166_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_167_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(167,DENALI_CTL_167_DATA,0);
       #else
         REGWR(167,DENALI_CTL_167_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_168_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(168,DENALI_CTL_168_DATA,0);
       #else
         REGWR(168,DENALI_CTL_168_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_169_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(169,DENALI_CTL_169_DATA,0);
       #else
         REGWR(169,DENALI_CTL_169_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_170_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(170,DENALI_CTL_170_DATA,0);
       #else
         REGWR(170,DENALI_CTL_170_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_171_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(171,DENALI_CTL_171_DATA,0);
       #else
         REGWR(171,DENALI_CTL_171_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_172_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(172,DENALI_CTL_172_DATA,0);
       #else
         REGWR(172,DENALI_CTL_172_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_173_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(173,DENALI_CTL_173_DATA,0);
       #else
         REGWR(173,DENALI_CTL_173_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_174_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(174,DENALI_CTL_174_DATA,0);
       #else
         REGWR(174,DENALI_CTL_174_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_175_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(175,DENALI_CTL_175_DATA,0);
       #else
         REGWR(175,DENALI_CTL_175_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_176_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(176,DENALI_CTL_176_DATA,0);
       #else
         REGWR(176,DENALI_CTL_176_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_177_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(177,DENALI_CTL_177_DATA,0);
       #else
         REGWR(177,DENALI_CTL_177_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_178_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(178,DENALI_CTL_178_DATA,0);
       #else
         REGWR(178,DENALI_CTL_178_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_179_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(179,DENALI_CTL_179_DATA,0);
       #else
         REGWR(179,DENALI_CTL_179_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_180_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(180,DENALI_CTL_180_DATA,0);
       #else
         REGWR(180,DENALI_CTL_180_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_181_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(181,DENALI_CTL_181_DATA,0);
       #else
         REGWR(181,DENALI_CTL_181_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_182_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(182,DENALI_CTL_182_DATA,0);
       #else
         REGWR(182,DENALI_CTL_182_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_183_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(183,DENALI_CTL_183_DATA,0);
       #else
         REGWR(183,DENALI_CTL_183_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_184_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(184,DENALI_CTL_184_DATA,0);
       #else
         REGWR(184,DENALI_CTL_184_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_185_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(185,DENALI_CTL_185_DATA,0);
       #else
         REGWR(185,DENALI_CTL_185_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_186_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(186,DENALI_CTL_186_DATA,0);
       #else
         REGWR(186,DENALI_CTL_186_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_187_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(187,DENALI_CTL_187_DATA,0);
       #else
         REGWR(187,DENALI_CTL_187_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_188_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(188,DENALI_CTL_188_DATA,0);
       #else
         REGWR(188,DENALI_CTL_188_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_189_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(189,DENALI_CTL_189_DATA,0);
       #else
         REGWR(189,DENALI_CTL_189_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_190_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(190,DENALI_CTL_190_DATA,0);
       #else
         REGWR(190,DENALI_CTL_190_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_191_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(191,DENALI_CTL_191_DATA,0);
       #else
         REGWR(191,DENALI_CTL_191_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_192_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(192,DENALI_CTL_192_DATA,0);
       #else
         REGWR(192,DENALI_CTL_192_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_193_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(193,DENALI_CTL_193_DATA,0);
       #else
         REGWR(193,DENALI_CTL_193_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_194_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(194,DENALI_CTL_194_DATA,0);
       #else
         REGWR(194,DENALI_CTL_194_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_195_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(195,DENALI_CTL_195_DATA,0);
       #else
         REGWR(195,DENALI_CTL_195_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_196_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(196,DENALI_CTL_196_DATA,0);
       #else
         REGWR(196,DENALI_CTL_196_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_197_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(197,DENALI_CTL_197_DATA,0);
       #else
         REGWR(197,DENALI_CTL_197_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_198_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(198,DENALI_CTL_198_DATA,0);
       #else
         REGWR(198,DENALI_CTL_198_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_199_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(199,DENALI_CTL_199_DATA,0);
       #else
         REGWR(199,DENALI_CTL_199_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_200_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(200,DENALI_CTL_200_DATA,0);
       #else
         REGWR(200,DENALI_CTL_200_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_201_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(201,DENALI_CTL_201_DATA,0);
       #else
         REGWR(201,DENALI_CTL_201_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_202_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(202,DENALI_CTL_202_DATA,0);
       #else
         REGWR(202,DENALI_CTL_202_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_203_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(203,DENALI_CTL_203_DATA,0);
       #else
         REGWR(203,DENALI_CTL_203_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_204_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(204,DENALI_CTL_204_DATA,0);
       #else
         REGWR(204,DENALI_CTL_204_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_205_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(205,DENALI_CTL_205_DATA,0);
       #else
         REGWR(205,DENALI_CTL_205_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_206_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(206,DENALI_CTL_206_DATA,0);
       #else
         REGWR(206,DENALI_CTL_206_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_207_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(207,DENALI_CTL_207_DATA,0);
       #else
         REGWR(207,DENALI_CTL_207_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_208_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(208,DENALI_CTL_208_DATA,0);
       #else
         REGWR(208,DENALI_CTL_208_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_209_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(209,DENALI_CTL_209_DATA,0);
       #else
         REGWR(209,DENALI_CTL_209_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_210_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(210,DENALI_CTL_210_DATA,0);
       #else
         REGWR(210,DENALI_CTL_210_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_211_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(211,DENALI_CTL_211_DATA,0);
       #else
         REGWR(211,DENALI_CTL_211_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_212_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(212,DENALI_CTL_212_DATA,0);
       #else
         REGWR(212,DENALI_CTL_212_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_213_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(213,DENALI_CTL_213_DATA,0);
       #else
         REGWR(213,DENALI_CTL_213_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_214_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(214,DENALI_CTL_214_DATA,0);
       #else
         REGWR(214,DENALI_CTL_214_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_215_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(215,DENALI_CTL_215_DATA,0);
       #else
         REGWR(215,DENALI_CTL_215_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_216_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(216,DENALI_CTL_216_DATA,0);
       #else
         REGWR(216,DENALI_CTL_216_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_217_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(217,DENALI_CTL_217_DATA,0);
       #else
         REGWR(217,DENALI_CTL_217_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_218_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(218,DENALI_CTL_218_DATA,0);
       #else
         REGWR(218,DENALI_CTL_218_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_219_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(219,DENALI_CTL_219_DATA,0);
       #else
         REGWR(219,DENALI_CTL_219_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_220_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(220,DENALI_CTL_220_DATA,0);
       #else
         REGWR(220,DENALI_CTL_220_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_221_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(221,DENALI_CTL_221_DATA,0);
       #else
         REGWR(221,DENALI_CTL_221_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_222_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(222,DENALI_CTL_222_DATA,0);
       #else
         REGWR(222,DENALI_CTL_222_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_223_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(223,DENALI_CTL_223_DATA,0);
       #else
         REGWR(223,DENALI_CTL_223_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_224_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(224,DENALI_CTL_224_DATA,0);
       #else
         REGWR(224,DENALI_CTL_224_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_225_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(225,DENALI_CTL_225_DATA,0);
       #else
         REGWR(225,DENALI_CTL_225_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_226_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(226,DENALI_CTL_226_DATA,0);
       #else
         REGWR(226,DENALI_CTL_226_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_227_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(227,DENALI_CTL_227_DATA,0);
       #else
         REGWR(227,DENALI_CTL_227_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_228_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(228,DENALI_CTL_228_DATA,0);
       #else
         REGWR(228,DENALI_CTL_228_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_229_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(229,DENALI_CTL_229_DATA,0);
       #else
         REGWR(229,DENALI_CTL_229_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_230_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(230,DENALI_CTL_230_DATA,0);
       #else
         REGWR(230,DENALI_CTL_230_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_231_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(231,DENALI_CTL_231_DATA,0);
       #else
         REGWR(231,DENALI_CTL_231_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_232_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(232,DENALI_CTL_232_DATA,0);
       #else
         REGWR(232,DENALI_CTL_232_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_233_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(233,DENALI_CTL_233_DATA,0);
       #else
         REGWR(233,DENALI_CTL_233_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_234_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(234,DENALI_CTL_234_DATA,0);
       #else
         REGWR(234,DENALI_CTL_234_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_235_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(235,DENALI_CTL_235_DATA,0);
       #else
         REGWR(235,DENALI_CTL_235_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_236_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(236,DENALI_CTL_236_DATA,0);
       #else
         REGWR(236,DENALI_CTL_236_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_237_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(237,DENALI_CTL_237_DATA,0);
       #else
         REGWR(237,DENALI_CTL_237_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_238_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(238,DENALI_CTL_238_DATA,0);
       #else
         REGWR(238,DENALI_CTL_238_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_239_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(239,DENALI_CTL_239_DATA,0);
       #else
         REGWR(239,DENALI_CTL_239_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_240_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(240,DENALI_CTL_240_DATA,0);
       #else
         REGWR(240,DENALI_CTL_240_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_241_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(241,DENALI_CTL_241_DATA,0);
       #else
         REGWR(241,DENALI_CTL_241_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_242_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(242,DENALI_CTL_242_DATA,0);
       #else
         REGWR(242,DENALI_CTL_242_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_243_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(243,DENALI_CTL_243_DATA,0);
       #else
         REGWR(243,DENALI_CTL_243_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_244_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(244,DENALI_CTL_244_DATA,0);
       #else
         REGWR(244,DENALI_CTL_244_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_245_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(245,DENALI_CTL_245_DATA,0);
       #else
         REGWR(245,DENALI_CTL_245_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_246_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(246,DENALI_CTL_246_DATA,0);
       #else
         REGWR(246,DENALI_CTL_246_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_247_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(247,DENALI_CTL_247_DATA,0);
       #else
         REGWR(247,DENALI_CTL_247_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_248_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(248,DENALI_CTL_248_DATA,0);
       #else
         REGWR(248,DENALI_CTL_248_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_249_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(249,DENALI_CTL_249_DATA,0);
       #else
         REGWR(249,DENALI_CTL_249_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_250_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(250,DENALI_CTL_250_DATA,0);
       #else
         REGWR(250,DENALI_CTL_250_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_251_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(251,DENALI_CTL_251_DATA,0);
       #else
         REGWR(251,DENALI_CTL_251_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_252_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(252,DENALI_CTL_252_DATA,0);
       #else
         REGWR(252,DENALI_CTL_252_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_253_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(253,DENALI_CTL_253_DATA,0);
       #else
         REGWR(253,DENALI_CTL_253_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_254_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(254,DENALI_CTL_254_DATA,0);
       #else
         REGWR(254,DENALI_CTL_254_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_255_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(255,DENALI_CTL_255_DATA,0);
       #else
         REGWR(255,DENALI_CTL_255_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_256_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(256,DENALI_CTL_256_DATA,0);
       #else
         REGWR(256,DENALI_CTL_256_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_257_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(257,DENALI_CTL_257_DATA,0);
       #else
         REGWR(257,DENALI_CTL_257_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_258_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(258,DENALI_CTL_258_DATA,0);
       #else
         REGWR(258,DENALI_CTL_258_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_259_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(259,DENALI_CTL_259_DATA,0);
       #else
         REGWR(259,DENALI_CTL_259_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_260_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(260,DENALI_CTL_260_DATA,0);
       #else
         REGWR(260,DENALI_CTL_260_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_261_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(261,DENALI_CTL_261_DATA,0);
       #else
         REGWR(261,DENALI_CTL_261_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_262_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(262,DENALI_CTL_262_DATA,0);
       #else
         REGWR(262,DENALI_CTL_262_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_263_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(263,DENALI_CTL_263_DATA,0);
       #else
         REGWR(263,DENALI_CTL_263_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_264_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(264,DENALI_CTL_264_DATA,0);
       #else
         REGWR(264,DENALI_CTL_264_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_265_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(265,DENALI_CTL_265_DATA,0);
       #else
         REGWR(265,DENALI_CTL_265_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_266_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(266,DENALI_CTL_266_DATA,0);
       #else
         REGWR(266,DENALI_CTL_266_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_267_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(267,DENALI_CTL_267_DATA,0);
       #else
         REGWR(267,DENALI_CTL_267_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_268_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(268,DENALI_CTL_268_DATA,0);
       #else
         REGWR(268,DENALI_CTL_268_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_269_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(269,DENALI_CTL_269_DATA,0);
       #else
         REGWR(269,DENALI_CTL_269_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_270_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(270,DENALI_CTL_270_DATA,0);
       #else
         REGWR(270,DENALI_CTL_270_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_271_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(271,DENALI_CTL_271_DATA,0);
       #else
         REGWR(271,DENALI_CTL_271_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_272_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(272,DENALI_CTL_272_DATA,0);
       #else
         REGWR(272,DENALI_CTL_272_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_273_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(273,DENALI_CTL_273_DATA,0);
       #else
         REGWR(273,DENALI_CTL_273_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_274_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(274,DENALI_CTL_274_DATA,0);
       #else
         REGWR(274,DENALI_CTL_274_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_275_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(275,DENALI_CTL_275_DATA,0);
       #else
         REGWR(275,DENALI_CTL_275_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_276_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(276,DENALI_CTL_276_DATA,0);
       #else
         REGWR(276,DENALI_CTL_276_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_277_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(277,DENALI_CTL_277_DATA,0);
       #else
         REGWR(277,DENALI_CTL_277_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_278_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(278,DENALI_CTL_278_DATA,0);
       #else
         REGWR(278,DENALI_CTL_278_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_279_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(279,DENALI_CTL_279_DATA,0);
       #else
         REGWR(279,DENALI_CTL_279_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_280_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(280,DENALI_CTL_280_DATA,0);
       #else
         REGWR(280,DENALI_CTL_280_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_281_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(281,DENALI_CTL_281_DATA,0);
       #else
         REGWR(281,DENALI_CTL_281_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_282_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(282,DENALI_CTL_282_DATA,0);
       #else
         REGWR(282,DENALI_CTL_282_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_283_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(283,DENALI_CTL_283_DATA,0);
       #else
         REGWR(283,DENALI_CTL_283_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_284_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(284,DENALI_CTL_284_DATA,0);
       #else
         REGWR(284,DENALI_CTL_284_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_285_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(285,DENALI_CTL_285_DATA,0);
       #else
         REGWR(285,DENALI_CTL_285_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_286_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(286,DENALI_CTL_286_DATA,0);
       #else
         REGWR(286,DENALI_CTL_286_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_287_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(287,DENALI_CTL_287_DATA,0);
       #else
         REGWR(287,DENALI_CTL_287_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_288_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(288,DENALI_CTL_288_DATA,0);
       #else
         REGWR(288,DENALI_CTL_288_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_289_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(289,DENALI_CTL_289_DATA,0);
       #else
         REGWR(289,DENALI_CTL_289_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_290_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(290,DENALI_CTL_290_DATA,0);
       #else
         REGWR(290,DENALI_CTL_290_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_291_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(291,DENALI_CTL_291_DATA,0);
       #else
         REGWR(291,DENALI_CTL_291_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_292_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(292,DENALI_CTL_292_DATA,0);
       #else
         REGWR(292,DENALI_CTL_292_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_293_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(293,DENALI_CTL_293_DATA,0);
       #else
         REGWR(293,DENALI_CTL_293_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_294_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(294,DENALI_CTL_294_DATA,0);
       #else
         REGWR(294,DENALI_CTL_294_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_295_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(295,DENALI_CTL_295_DATA,0);
       #else
         REGWR(295,DENALI_CTL_295_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_296_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(296,DENALI_CTL_296_DATA,0);
       #else
         REGWR(296,DENALI_CTL_296_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_297_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(297,DENALI_CTL_297_DATA,0);
       #else
         REGWR(297,DENALI_CTL_297_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_298_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(298,DENALI_CTL_298_DATA,0);
       #else
         REGWR(298,DENALI_CTL_298_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_299_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(299,DENALI_CTL_299_DATA,0);
       #else
         REGWR(299,DENALI_CTL_299_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_300_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(300,DENALI_CTL_300_DATA,0);
       #else
         REGWR(300,DENALI_CTL_300_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_301_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(301,DENALI_CTL_301_DATA,0);
       #else
         REGWR(301,DENALI_CTL_301_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_302_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(302,DENALI_CTL_302_DATA,0);
       #else
         REGWR(302,DENALI_CTL_302_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_303_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(303,DENALI_CTL_303_DATA,0);
       #else
         REGWR(303,DENALI_CTL_303_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_304_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(304,DENALI_CTL_304_DATA,0);
       #else
         REGWR(304,DENALI_CTL_304_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_305_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(305,DENALI_CTL_305_DATA,0);
       #else
         REGWR(305,DENALI_CTL_305_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_306_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(306,DENALI_CTL_306_DATA,0);
       #else
         REGWR(306,DENALI_CTL_306_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_307_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(307,DENALI_CTL_307_DATA,0);
       #else
         REGWR(307,DENALI_CTL_307_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_308_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(308,DENALI_CTL_308_DATA,0);
       #else
         REGWR(308,DENALI_CTL_308_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_309_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(309,DENALI_CTL_309_DATA,0);
       #else
         REGWR(309,DENALI_CTL_309_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_310_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(310,DENALI_CTL_310_DATA,0);
       #else
         REGWR(310,DENALI_CTL_310_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_311_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(311,DENALI_CTL_311_DATA,0);
       #else
         REGWR(311,DENALI_CTL_311_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_312_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(312,DENALI_CTL_312_DATA,0);
       #else
         REGWR(312,DENALI_CTL_312_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_313_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(313,DENALI_CTL_313_DATA,0);
       #else
         REGWR(313,DENALI_CTL_313_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_314_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(314,DENALI_CTL_314_DATA,0);
       #else
         REGWR(314,DENALI_CTL_314_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_315_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(315,DENALI_CTL_315_DATA,0);
       #else
         REGWR(315,DENALI_CTL_315_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_316_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(316,DENALI_CTL_316_DATA,0);
       #else
         REGWR(316,DENALI_CTL_316_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_317_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(317,DENALI_CTL_317_DATA,0);
       #else
         REGWR(317,DENALI_CTL_317_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_318_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(318,DENALI_CTL_318_DATA,0);
       #else
         REGWR(318,DENALI_CTL_318_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_319_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(319,DENALI_CTL_319_DATA,0);
       #else
         REGWR(319,DENALI_CTL_319_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_320_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(320,DENALI_CTL_320_DATA,0);
       #else
         REGWR(320,DENALI_CTL_320_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_321_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(321,DENALI_CTL_321_DATA,0);
       #else
         REGWR(321,DENALI_CTL_321_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_322_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(322,DENALI_CTL_322_DATA,0);
       #else
         REGWR(322,DENALI_CTL_322_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_323_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(323,DENALI_CTL_323_DATA,0);
       #else
         REGWR(323,DENALI_CTL_323_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_324_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(324,DENALI_CTL_324_DATA,0);
       #else
         REGWR(324,DENALI_CTL_324_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_325_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(325,DENALI_CTL_325_DATA,0);
       #else
         REGWR(325,DENALI_CTL_325_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_326_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(326,DENALI_CTL_326_DATA,0);
       #else
         REGWR(326,DENALI_CTL_326_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_327_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(327,DENALI_CTL_327_DATA,0);
       #else
         REGWR(327,DENALI_CTL_327_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_328_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(328,DENALI_CTL_328_DATA,0);
       #else
         REGWR(328,DENALI_CTL_328_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_329_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(329,DENALI_CTL_329_DATA,0);
       #else
         REGWR(329,DENALI_CTL_329_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_330_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(330,DENALI_CTL_330_DATA,0);
       #else
         REGWR(330,DENALI_CTL_330_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_331_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(331,DENALI_CTL_331_DATA,0);
       #else
         REGWR(331,DENALI_CTL_331_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_332_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(332,DENALI_CTL_332_DATA,0);
       #else
         REGWR(332,DENALI_CTL_332_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_333_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(333,DENALI_CTL_333_DATA,0);
       #else
         REGWR(333,DENALI_CTL_333_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_334_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(334,DENALI_CTL_334_DATA,0);
       #else
         REGWR(334,DENALI_CTL_334_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_335_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(335,DENALI_CTL_335_DATA,0);
       #else
         REGWR(335,DENALI_CTL_335_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_336_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(336,DENALI_CTL_336_DATA,0);
       #else
         REGWR(336,DENALI_CTL_336_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_337_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(337,DENALI_CTL_337_DATA,0);
       #else
         REGWR(337,DENALI_CTL_337_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_338_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(338,DENALI_CTL_338_DATA,0);
       #else
         REGWR(338,DENALI_CTL_338_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_339_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(339,DENALI_CTL_339_DATA,0);
       #else
         REGWR(339,DENALI_CTL_339_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_340_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(340,DENALI_CTL_340_DATA,0);
       #else
         REGWR(340,DENALI_CTL_340_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_341_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(341,DENALI_CTL_341_DATA,0);
       #else
         REGWR(341,DENALI_CTL_341_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_342_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(342,DENALI_CTL_342_DATA,0);
       #else
         REGWR(342,DENALI_CTL_342_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_343_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(343,DENALI_CTL_343_DATA,0);
       #else
         REGWR(343,DENALI_CTL_343_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_344_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(344,DENALI_CTL_344_DATA,0);
       #else
         REGWR(344,DENALI_CTL_344_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_345_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(345,DENALI_CTL_345_DATA,0);
       #else
         REGWR(345,DENALI_CTL_345_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_346_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(346,DENALI_CTL_346_DATA,0);
       #else
         REGWR(346,DENALI_CTL_346_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_347_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(347,DENALI_CTL_347_DATA,0);
       #else
         REGWR(347,DENALI_CTL_347_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_348_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(348,DENALI_CTL_348_DATA,0);
       #else
         REGWR(348,DENALI_CTL_348_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_349_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(349,DENALI_CTL_349_DATA,0);
       #else
         REGWR(349,DENALI_CTL_349_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_350_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(350,DENALI_CTL_350_DATA,0);
       #else
         REGWR(350,DENALI_CTL_350_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_351_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(351,DENALI_CTL_351_DATA,0);
       #else
         REGWR(351,DENALI_CTL_351_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_352_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(352,DENALI_CTL_352_DATA,0);
       #else
         REGWR(352,DENALI_CTL_352_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_353_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(353,DENALI_CTL_353_DATA,0);
       #else
         REGWR(353,DENALI_CTL_353_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_354_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(354,DENALI_CTL_354_DATA,0);
       #else
         REGWR(354,DENALI_CTL_354_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_355_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(355,DENALI_CTL_355_DATA,0);
       #else
         REGWR(355,DENALI_CTL_355_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_356_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(356,DENALI_CTL_356_DATA,0);
       #else
         REGWR(356,DENALI_CTL_356_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_357_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(357,DENALI_CTL_357_DATA,0);
       #else
         REGWR(357,DENALI_CTL_357_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_358_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(358,DENALI_CTL_358_DATA,0);
       #else
         REGWR(358,DENALI_CTL_358_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_359_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(359,DENALI_CTL_359_DATA,0);
       #else
         REGWR(359,DENALI_CTL_359_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_360_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(360,DENALI_CTL_360_DATA,0);
       #else
         REGWR(360,DENALI_CTL_360_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_361_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(361,DENALI_CTL_361_DATA,0);
       #else
         REGWR(361,DENALI_CTL_361_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_362_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(362,DENALI_CTL_362_DATA,0);
       #else
         REGWR(362,DENALI_CTL_362_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_363_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(363,DENALI_CTL_363_DATA,0);
       #else
         REGWR(363,DENALI_CTL_363_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_364_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(364,DENALI_CTL_364_DATA,0);
       #else
         REGWR(364,DENALI_CTL_364_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_365_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(365,DENALI_CTL_365_DATA,0);
       #else
         REGWR(365,DENALI_CTL_365_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_366_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(366,DENALI_CTL_366_DATA,0);
       #else
         REGWR(366,DENALI_CTL_366_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_367_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(367,DENALI_CTL_367_DATA,0);
       #else
         REGWR(367,DENALI_CTL_367_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_368_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(368,DENALI_CTL_368_DATA,0);
       #else
         REGWR(368,DENALI_CTL_368_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_369_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(369,DENALI_CTL_369_DATA,0);
       #else
         REGWR(369,DENALI_CTL_369_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_370_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(370,DENALI_CTL_370_DATA,0);
       #else
         REGWR(370,DENALI_CTL_370_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_371_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(371,DENALI_CTL_371_DATA,0);
       #else
         REGWR(371,DENALI_CTL_371_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_372_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(372,DENALI_CTL_372_DATA,0);
       #else
         REGWR(372,DENALI_CTL_372_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_373_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(373,DENALI_CTL_373_DATA,0);
       #else
         REGWR(373,DENALI_CTL_373_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_374_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(374,DENALI_CTL_374_DATA,0);
       #else
         REGWR(374,DENALI_CTL_374_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_375_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(375,DENALI_CTL_375_DATA,0);
       #else
         REGWR(375,DENALI_CTL_375_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_376_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(376,DENALI_CTL_376_DATA,0);
       #else
         REGWR(376,DENALI_CTL_376_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_377_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(377,DENALI_CTL_377_DATA,0);
       #else
         REGWR(377,DENALI_CTL_377_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_378_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(378,DENALI_CTL_378_DATA,0);
       #else
         REGWR(378,DENALI_CTL_378_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_379_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(379,DENALI_CTL_379_DATA,0);
       #else
         REGWR(379,DENALI_CTL_379_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_380_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(380,DENALI_CTL_380_DATA,0);
       #else
         REGWR(380,DENALI_CTL_380_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_381_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(381,DENALI_CTL_381_DATA,0);
       #else
         REGWR(381,DENALI_CTL_381_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_382_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(382,DENALI_CTL_382_DATA,0);
       #else
         REGWR(382,DENALI_CTL_382_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_383_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(383,DENALI_CTL_383_DATA,0);
       #else
         REGWR(383,DENALI_CTL_383_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_384_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(384,DENALI_CTL_384_DATA,0);
       #else
         REGWR(384,DENALI_CTL_384_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_385_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(385,DENALI_CTL_385_DATA,0);
       #else
         REGWR(385,DENALI_CTL_385_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_386_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(386,DENALI_CTL_386_DATA,0);
       #else
         REGWR(386,DENALI_CTL_386_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_387_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(387,DENALI_CTL_387_DATA,0);
       #else
         REGWR(387,DENALI_CTL_387_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_388_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(388,DENALI_CTL_388_DATA,0);
       #else
         REGWR(388,DENALI_CTL_388_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_389_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(389,DENALI_CTL_389_DATA,0);
       #else
         REGWR(389,DENALI_CTL_389_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_390_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(390,DENALI_CTL_390_DATA,0);
       #else
         REGWR(390,DENALI_CTL_390_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_391_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(391,DENALI_CTL_391_DATA,0);
       #else
         REGWR(391,DENALI_CTL_391_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_392_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(392,DENALI_CTL_392_DATA,0);
       #else
         REGWR(392,DENALI_CTL_392_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_393_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(393,DENALI_CTL_393_DATA,0);
       #else
         REGWR(393,DENALI_CTL_393_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_394_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(394,DENALI_CTL_394_DATA,0);
       #else
         REGWR(394,DENALI_CTL_394_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_395_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(395,DENALI_CTL_395_DATA,0);
       #else
         REGWR(395,DENALI_CTL_395_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_396_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(396,DENALI_CTL_396_DATA,0);
       #else
         REGWR(396,DENALI_CTL_396_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_397_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(397,DENALI_CTL_397_DATA,0);
       #else
         REGWR(397,DENALI_CTL_397_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_398_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(398,DENALI_CTL_398_DATA,0);
       #else
         REGWR(398,DENALI_CTL_398_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_399_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(399,DENALI_CTL_399_DATA,0);
       #else
         REGWR(399,DENALI_CTL_399_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_400_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(400,DENALI_CTL_400_DATA,0);
       #else
         REGWR(400,DENALI_CTL_400_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_401_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(401,DENALI_CTL_401_DATA,0);
       #else
         REGWR(401,DENALI_CTL_401_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_402_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(402,DENALI_CTL_402_DATA,0);
       #else
         REGWR(402,DENALI_CTL_402_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_403_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(403,DENALI_CTL_403_DATA,0);
       #else
         REGWR(403,DENALI_CTL_403_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_404_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(404,DENALI_CTL_404_DATA,0);
       #else
         REGWR(404,DENALI_CTL_404_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_405_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(405,DENALI_CTL_405_DATA,0);
       #else
         REGWR(405,DENALI_CTL_405_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_406_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(406,DENALI_CTL_406_DATA,0);
       #else
         REGWR(406,DENALI_CTL_406_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_407_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(407,DENALI_CTL_407_DATA,0);
       #else
         REGWR(407,DENALI_CTL_407_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_408_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(408,DENALI_CTL_408_DATA,0);
       #else
         REGWR(408,DENALI_CTL_408_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_409_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(409,DENALI_CTL_409_DATA,0);
       #else
         REGWR(409,DENALI_CTL_409_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_410_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(410,DENALI_CTL_410_DATA,0);
       #else
         REGWR(410,DENALI_CTL_410_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_411_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(411,DENALI_CTL_411_DATA,0);
       #else
         REGWR(411,DENALI_CTL_411_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_412_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(412,DENALI_CTL_412_DATA,0);
       #else
         REGWR(412,DENALI_CTL_412_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_413_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(413,DENALI_CTL_413_DATA,0);
       #else
         REGWR(413,DENALI_CTL_413_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_414_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(414,DENALI_CTL_414_DATA,0);
       #else
         REGWR(414,DENALI_CTL_414_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_415_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(415,DENALI_CTL_415_DATA,0);
       #else
         REGWR(415,DENALI_CTL_415_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_416_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(416,DENALI_CTL_416_DATA,0);
       #else
         REGWR(416,DENALI_CTL_416_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_417_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(417,DENALI_CTL_417_DATA,0);
       #else
         REGWR(417,DENALI_CTL_417_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_418_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(418,DENALI_CTL_418_DATA,0);
       #else
         REGWR(418,DENALI_CTL_418_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_419_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(419,DENALI_CTL_419_DATA,0);
       #else
         REGWR(419,DENALI_CTL_419_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_420_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(420,DENALI_CTL_420_DATA,0);
       #else
         REGWR(420,DENALI_CTL_420_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_421_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(421,DENALI_CTL_421_DATA,0);
       #else
         REGWR(421,DENALI_CTL_421_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_422_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(422,DENALI_CTL_422_DATA,0);
       #else
         REGWR(422,DENALI_CTL_422_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_423_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(423,DENALI_CTL_423_DATA,0);
       #else
         REGWR(423,DENALI_CTL_423_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_424_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(424,DENALI_CTL_424_DATA,0);
       #else
         REGWR(424,DENALI_CTL_424_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_425_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(425,DENALI_CTL_425_DATA,0);
       #else
         REGWR(425,DENALI_CTL_425_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_426_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(426,DENALI_CTL_426_DATA,0);
       #else
         REGWR(426,DENALI_CTL_426_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_427_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(427,DENALI_CTL_427_DATA,0);
       #else
         REGWR(427,DENALI_CTL_427_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_428_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(428,DENALI_CTL_428_DATA,0);
       #else
         REGWR(428,DENALI_CTL_428_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_429_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(429,DENALI_CTL_429_DATA,0);
       #else
         REGWR(429,DENALI_CTL_429_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_430_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(430,DENALI_CTL_430_DATA,0);
       #else
         REGWR(430,DENALI_CTL_430_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_431_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(431,DENALI_CTL_431_DATA,0);
       #else
         REGWR(431,DENALI_CTL_431_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_432_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(432,DENALI_CTL_432_DATA,0);
       #else
         REGWR(432,DENALI_CTL_432_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_433_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(433,DENALI_CTL_433_DATA,0);
       #else
         REGWR(433,DENALI_CTL_433_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_434_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(434,DENALI_CTL_434_DATA,0);
       #else
         REGWR(434,DENALI_CTL_434_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_435_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(435,DENALI_CTL_435_DATA,0);
       #else
         REGWR(435,DENALI_CTL_435_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_436_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(436,DENALI_CTL_436_DATA,0);
       #else
         REGWR(436,DENALI_CTL_436_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_437_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(437,DENALI_CTL_437_DATA,0);
       #else
         REGWR(437,DENALI_CTL_437_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_438_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(438,DENALI_CTL_438_DATA,0);
       #else
         REGWR(438,DENALI_CTL_438_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_439_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(439,DENALI_CTL_439_DATA,0);
       #else
         REGWR(439,DENALI_CTL_439_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_440_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(440,DENALI_CTL_440_DATA,0);
       #else
         REGWR(440,DENALI_CTL_440_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_441_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(441,DENALI_CTL_441_DATA,0);
       #else
         REGWR(441,DENALI_CTL_441_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_442_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(442,DENALI_CTL_442_DATA,0);
       #else
         REGWR(442,DENALI_CTL_442_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_443_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(443,DENALI_CTL_443_DATA,0);
       #else
         REGWR(443,DENALI_CTL_443_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_444_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(444,DENALI_CTL_444_DATA,0);
       #else
         REGWR(444,DENALI_CTL_444_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_445_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(445,DENALI_CTL_445_DATA,0);
       #else
         REGWR(445,DENALI_CTL_445_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_446_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(446,DENALI_CTL_446_DATA,0);
       #else
         REGWR(446,DENALI_CTL_446_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_447_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(447,DENALI_CTL_447_DATA,0);
       #else
         REGWR(447,DENALI_CTL_447_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_448_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(448,DENALI_CTL_448_DATA,0);
       #else
         REGWR(448,DENALI_CTL_448_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_449_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(449,DENALI_CTL_449_DATA,0);
       #else
         REGWR(449,DENALI_CTL_449_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_450_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(450,DENALI_CTL_450_DATA,0);
       #else
         REGWR(450,DENALI_CTL_450_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_451_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(451,DENALI_CTL_451_DATA,0);
       #else
         REGWR(451,DENALI_CTL_451_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_452_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(452,DENALI_CTL_452_DATA,0);
       #else
         REGWR(452,DENALI_CTL_452_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_453_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(453,DENALI_CTL_453_DATA,0);
       #else
         REGWR(453,DENALI_CTL_453_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_454_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(454,DENALI_CTL_454_DATA,0);
       #else
         REGWR(454,DENALI_CTL_454_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_455_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(455,DENALI_CTL_455_DATA,0);
       #else
         REGWR(455,DENALI_CTL_455_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_456_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(456,DENALI_CTL_456_DATA,0);
       #else
         REGWR(456,DENALI_CTL_456_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_457_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(457,DENALI_CTL_457_DATA,0);
       #else
         REGWR(457,DENALI_CTL_457_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_458_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(458,DENALI_CTL_458_DATA,0);
       #else
         REGWR(458,DENALI_CTL_458_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_459_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(459,DENALI_CTL_459_DATA,0);
       #else
         REGWR(459,DENALI_CTL_459_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_460_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(460,DENALI_CTL_460_DATA,0);
       #else
         REGWR(460,DENALI_CTL_460_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_461_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(461,DENALI_CTL_461_DATA,0);
       #else
         REGWR(461,DENALI_CTL_461_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_462_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(462,DENALI_CTL_462_DATA,0);
       #else
         REGWR(462,DENALI_CTL_462_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_463_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(463,DENALI_CTL_463_DATA,0);
       #else
         REGWR(463,DENALI_CTL_463_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_464_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(464,DENALI_CTL_464_DATA,0);
       #else
         REGWR(464,DENALI_CTL_464_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_465_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(465,DENALI_CTL_465_DATA,0);
       #else
         REGWR(465,DENALI_CTL_465_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_466_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(466,DENALI_CTL_466_DATA,0);
       #else
         REGWR(466,DENALI_CTL_466_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_467_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(467,DENALI_CTL_467_DATA,0);
       #else
         REGWR(467,DENALI_CTL_467_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_468_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(468,DENALI_CTL_468_DATA,0);
       #else
         REGWR(468,DENALI_CTL_468_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_469_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(469,DENALI_CTL_469_DATA,0);
       #else
         REGWR(469,DENALI_CTL_469_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_470_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(470,DENALI_CTL_470_DATA,0);
       #else
         REGWR(470,DENALI_CTL_470_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_471_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(471,DENALI_CTL_471_DATA,0);
       #else
         REGWR(471,DENALI_CTL_471_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_472_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(472,DENALI_CTL_472_DATA,0);
       #else
         REGWR(472,DENALI_CTL_472_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_473_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(473,DENALI_CTL_473_DATA,0);
       #else
         REGWR(473,DENALI_CTL_473_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_474_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(474,DENALI_CTL_474_DATA,0);
       #else
         REGWR(474,DENALI_CTL_474_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_475_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(475,DENALI_CTL_475_DATA,0);
       #else
         REGWR(475,DENALI_CTL_475_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_476_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(476,DENALI_CTL_476_DATA,0);
       #else
         REGWR(476,DENALI_CTL_476_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_477_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(477,DENALI_CTL_477_DATA,0);
       #else
         REGWR(477,DENALI_CTL_477_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_478_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(478,DENALI_CTL_478_DATA,0);
       #else
         REGWR(478,DENALI_CTL_478_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_479_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(479,DENALI_CTL_479_DATA,0);
       #else
         REGWR(479,DENALI_CTL_479_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_480_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(480,DENALI_CTL_480_DATA,0);
       #else
         REGWR(480,DENALI_CTL_480_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_481_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(481,DENALI_CTL_481_DATA,0);
       #else
         REGWR(481,DENALI_CTL_481_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_482_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(482,DENALI_CTL_482_DATA,0);
       #else
         REGWR(482,DENALI_CTL_482_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_483_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(483,DENALI_CTL_483_DATA,0);
       #else
         REGWR(483,DENALI_CTL_483_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_484_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(484,DENALI_CTL_484_DATA,0);
       #else
         REGWR(484,DENALI_CTL_484_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_485_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(485,DENALI_CTL_485_DATA,0);
       #else
         REGWR(485,DENALI_CTL_485_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_486_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(486,DENALI_CTL_486_DATA,0);
       #else
         REGWR(486,DENALI_CTL_486_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_487_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(487,DENALI_CTL_487_DATA,0);
       #else
         REGWR(487,DENALI_CTL_487_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_488_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(488,DENALI_CTL_488_DATA,0);
       #else
         REGWR(488,DENALI_CTL_488_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_489_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(489,DENALI_CTL_489_DATA,0);
       #else
         REGWR(489,DENALI_CTL_489_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_490_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(490,DENALI_CTL_490_DATA,0);
       #else
         REGWR(490,DENALI_CTL_490_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_491_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(491,DENALI_CTL_491_DATA,0);
       #else
         REGWR(491,DENALI_CTL_491_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_492_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(492,DENALI_CTL_492_DATA,0);
       #else
         REGWR(492,DENALI_CTL_492_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_493_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(493,DENALI_CTL_493_DATA,0);
       #else
         REGWR(493,DENALI_CTL_493_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_494_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(494,DENALI_CTL_494_DATA,0);
       #else
         REGWR(494,DENALI_CTL_494_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_495_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(495,DENALI_CTL_495_DATA,0);
       #else
         REGWR(495,DENALI_CTL_495_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_496_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(496,DENALI_CTL_496_DATA,0);
       #else
         REGWR(496,DENALI_CTL_496_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_497_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(497,DENALI_CTL_497_DATA,0);
       #else
         REGWR(497,DENALI_CTL_497_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_498_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(498,DENALI_CTL_498_DATA,0);
       #else
         REGWR(498,DENALI_CTL_498_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_499_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(499,DENALI_CTL_499_DATA,0);
       #else
         REGWR(499,DENALI_CTL_499_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_500_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(500,DENALI_CTL_500_DATA,0);
       #else
         REGWR(500,DENALI_CTL_500_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_501_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(501,DENALI_CTL_501_DATA,0);
       #else
         REGWR(501,DENALI_CTL_501_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_502_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(502,DENALI_CTL_502_DATA,0);
       #else
         REGWR(502,DENALI_CTL_502_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_503_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(503,DENALI_CTL_503_DATA,0);
       #else
         REGWR(503,DENALI_CTL_503_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_504_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(504,DENALI_CTL_504_DATA,0);
       #else
         REGWR(504,DENALI_CTL_504_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_505_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(505,DENALI_CTL_505_DATA,0);
       #else
         REGWR(505,DENALI_CTL_505_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_506_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(506,DENALI_CTL_506_DATA,0);
       #else
         REGWR(506,DENALI_CTL_506_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_507_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(507,DENALI_CTL_507_DATA,0);
       #else
         REGWR(507,DENALI_CTL_507_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_508_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(508,DENALI_CTL_508_DATA,0);
       #else
         REGWR(508,DENALI_CTL_508_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_509_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(509,DENALI_CTL_509_DATA,0);
       #else
         REGWR(509,DENALI_CTL_509_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_510_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(510,DENALI_CTL_510_DATA,0);
       #else
         REGWR(510,DENALI_CTL_510_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_511_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(511,DENALI_CTL_511_DATA,0);
       #else
         REGWR(511,DENALI_CTL_511_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_512_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(512,DENALI_CTL_512_DATA,0);
       #else
         REGWR(512,DENALI_CTL_512_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_513_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(513,DENALI_CTL_513_DATA,0);
       #else
         REGWR(513,DENALI_CTL_513_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_514_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(514,DENALI_CTL_514_DATA,0);
       #else
         REGWR(514,DENALI_CTL_514_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_515_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(515,DENALI_CTL_515_DATA,0);
       #else
         REGWR(515,DENALI_CTL_515_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_516_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(516,DENALI_CTL_516_DATA,0);
       #else
         REGWR(516,DENALI_CTL_516_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_517_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(517,DENALI_CTL_517_DATA,0);
       #else
         REGWR(517,DENALI_CTL_517_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_518_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(518,DENALI_CTL_518_DATA,0);
       #else
         REGWR(518,DENALI_CTL_518_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_519_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(519,DENALI_CTL_519_DATA,0);
       #else
         REGWR(519,DENALI_CTL_519_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_520_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(520,DENALI_CTL_520_DATA,0);
       #else
         REGWR(520,DENALI_CTL_520_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_521_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(521,DENALI_CTL_521_DATA,0);
       #else
         REGWR(521,DENALI_CTL_521_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_522_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(522,DENALI_CTL_522_DATA,0);
       #else
         REGWR(522,DENALI_CTL_522_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_523_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(523,DENALI_CTL_523_DATA,0);
       #else
         REGWR(523,DENALI_CTL_523_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_524_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(524,DENALI_CTL_524_DATA,0);
       #else
         REGWR(524,DENALI_CTL_524_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_525_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(525,DENALI_CTL_525_DATA,0);
       #else
         REGWR(525,DENALI_CTL_525_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_526_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(526,DENALI_CTL_526_DATA,0);
       #else
         REGWR(526,DENALI_CTL_526_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_527_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(527,DENALI_CTL_527_DATA,0);
       #else
         REGWR(527,DENALI_CTL_527_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_528_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(528,DENALI_CTL_528_DATA,0);
       #else
         REGWR(528,DENALI_CTL_528_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_529_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(529,DENALI_CTL_529_DATA,0);
       #else
         REGWR(529,DENALI_CTL_529_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_530_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(530,DENALI_CTL_530_DATA,0);
       #else
         REGWR(530,DENALI_CTL_530_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_531_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(531,DENALI_CTL_531_DATA,0);
       #else
         REGWR(531,DENALI_CTL_531_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_532_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(532,DENALI_CTL_532_DATA,0);
       #else
         REGWR(532,DENALI_CTL_532_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_533_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(533,DENALI_CTL_533_DATA,0);
       #else
         REGWR(533,DENALI_CTL_533_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_534_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(534,DENALI_CTL_534_DATA,0);
       #else
         REGWR(534,DENALI_CTL_534_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_535_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(535,DENALI_CTL_535_DATA,0);
       #else
         REGWR(535,DENALI_CTL_535_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_536_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(536,DENALI_CTL_536_DATA,0);
       #else
         REGWR(536,DENALI_CTL_536_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_537_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(537,DENALI_CTL_537_DATA,0);
       #else
         REGWR(537,DENALI_CTL_537_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_538_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(538,DENALI_CTL_538_DATA,0);
       #else
         REGWR(538,DENALI_CTL_538_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_539_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(539,DENALI_CTL_539_DATA,0);
       #else
         REGWR(539,DENALI_CTL_539_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_540_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(540,DENALI_CTL_540_DATA,0);
       #else
         REGWR(540,DENALI_CTL_540_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_541_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(541,DENALI_CTL_541_DATA,0);
       #else
         REGWR(541,DENALI_CTL_541_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_542_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(542,DENALI_CTL_542_DATA,0);
       #else
         REGWR(542,DENALI_CTL_542_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_543_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(543,DENALI_CTL_543_DATA,0);
       #else
         REGWR(543,DENALI_CTL_543_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_544_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(544,DENALI_CTL_544_DATA,0);
       #else
         REGWR(544,DENALI_CTL_544_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_545_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(545,DENALI_CTL_545_DATA,0);
       #else
         REGWR(545,DENALI_CTL_545_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_546_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(546,DENALI_CTL_546_DATA,0);
       #else
         REGWR(546,DENALI_CTL_546_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_547_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(547,DENALI_CTL_547_DATA,0);
       #else
         REGWR(547,DENALI_CTL_547_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_548_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(548,DENALI_CTL_548_DATA,0);
       #else
         REGWR(548,DENALI_CTL_548_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_549_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(549,DENALI_CTL_549_DATA,0);
       #else
         REGWR(549,DENALI_CTL_549_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_550_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(550,DENALI_CTL_550_DATA,0);
       #else
         REGWR(550,DENALI_CTL_550_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_551_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(551,DENALI_CTL_551_DATA,0);
       #else
         REGWR(551,DENALI_CTL_551_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_552_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(552,DENALI_CTL_552_DATA,0);
       #else
         REGWR(552,DENALI_CTL_552_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_553_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(553,DENALI_CTL_553_DATA,0);
       #else
         REGWR(553,DENALI_CTL_553_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_554_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(554,DENALI_CTL_554_DATA,0);
       #else
         REGWR(554,DENALI_CTL_554_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_555_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(555,DENALI_CTL_555_DATA,0);
       #else
         REGWR(555,DENALI_CTL_555_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_556_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(556,DENALI_CTL_556_DATA,0);
       #else
         REGWR(556,DENALI_CTL_556_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_557_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(557,DENALI_CTL_557_DATA,0);
       #else
         REGWR(557,DENALI_CTL_557_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_558_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(558,DENALI_CTL_558_DATA,0);
       #else
         REGWR(558,DENALI_CTL_558_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_559_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(559,DENALI_CTL_559_DATA,0);
       #else
         REGWR(559,DENALI_CTL_559_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_560_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(560,DENALI_CTL_560_DATA,0);
       #else
         REGWR(560,DENALI_CTL_560_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_561_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(561,DENALI_CTL_561_DATA,0);
       #else
         REGWR(561,DENALI_CTL_561_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_562_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(562,DENALI_CTL_562_DATA,0);
       #else
         REGWR(562,DENALI_CTL_562_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_563_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(563,DENALI_CTL_563_DATA,0);
       #else
         REGWR(563,DENALI_CTL_563_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_564_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(564,DENALI_CTL_564_DATA,0);
       #else
         REGWR(564,DENALI_CTL_564_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_565_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(565,DENALI_CTL_565_DATA,0);
       #else
         REGWR(565,DENALI_CTL_565_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_566_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(566,DENALI_CTL_566_DATA,0);
       #else
         REGWR(566,DENALI_CTL_566_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_567_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(567,DENALI_CTL_567_DATA,0);
       #else
         REGWR(567,DENALI_CTL_567_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_568_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(568,DENALI_CTL_568_DATA,0);
       #else
         REGWR(568,DENALI_CTL_568_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_569_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(569,DENALI_CTL_569_DATA,0);
       #else
         REGWR(569,DENALI_CTL_569_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_570_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(570,DENALI_CTL_570_DATA,0);
       #else
         REGWR(570,DENALI_CTL_570_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_571_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(571,DENALI_CTL_571_DATA,0);
       #else
         REGWR(571,DENALI_CTL_571_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_572_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(572,DENALI_CTL_572_DATA,0);
       #else
         REGWR(572,DENALI_CTL_572_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_573_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(573,DENALI_CTL_573_DATA,0);
       #else
         REGWR(573,DENALI_CTL_573_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_574_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(574,DENALI_CTL_574_DATA,0);
       #else
         REGWR(574,DENALI_CTL_574_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_575_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(575,DENALI_CTL_575_DATA,0);
       #else
         REGWR(575,DENALI_CTL_575_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_576_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(576,DENALI_CTL_576_DATA,0);
       #else
         REGWR(576,DENALI_CTL_576_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_577_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(577,DENALI_CTL_577_DATA,0);
       #else
         REGWR(577,DENALI_CTL_577_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_578_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(578,DENALI_CTL_578_DATA,0);
       #else
         REGWR(578,DENALI_CTL_578_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_579_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(579,DENALI_CTL_579_DATA,0);
       #else
         REGWR(579,DENALI_CTL_579_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_580_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(580,DENALI_CTL_580_DATA,0);
       #else
         REGWR(580,DENALI_CTL_580_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_581_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(581,DENALI_CTL_581_DATA,0);
       #else
         REGWR(581,DENALI_CTL_581_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_582_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(582,DENALI_CTL_582_DATA,0);
       #else
         REGWR(582,DENALI_CTL_582_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_583_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(583,DENALI_CTL_583_DATA,0);
       #else
         REGWR(583,DENALI_CTL_583_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_584_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(584,DENALI_CTL_584_DATA,0);
       #else
         REGWR(584,DENALI_CTL_584_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_585_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(585,DENALI_CTL_585_DATA,0);
       #else
         REGWR(585,DENALI_CTL_585_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_586_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(586,DENALI_CTL_586_DATA,0);
       #else
         REGWR(586,DENALI_CTL_586_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_587_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(587,DENALI_CTL_587_DATA,0);
       #else
         REGWR(587,DENALI_CTL_587_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_588_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(588,DENALI_CTL_588_DATA,0);
       #else
         REGWR(588,DENALI_CTL_588_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_589_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(589,DENALI_CTL_589_DATA,0);
       #else
         REGWR(589,DENALI_CTL_589_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_590_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(590,DENALI_CTL_590_DATA,0);
       #else
         REGWR(590,DENALI_CTL_590_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_591_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(591,DENALI_CTL_591_DATA,0);
       #else
         REGWR(591,DENALI_CTL_591_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_592_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(592,DENALI_CTL_592_DATA,0);
       #else
         REGWR(592,DENALI_CTL_592_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_593_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(593,DENALI_CTL_593_DATA,0);
       #else
         REGWR(593,DENALI_CTL_593_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_594_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(594,DENALI_CTL_594_DATA,0);
       #else
         REGWR(594,DENALI_CTL_594_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_595_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(595,DENALI_CTL_595_DATA,0);
       #else
         REGWR(595,DENALI_CTL_595_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_596_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(596,DENALI_CTL_596_DATA,0);
       #else
         REGWR(596,DENALI_CTL_596_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_597_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(597,DENALI_CTL_597_DATA,0);
       #else
         REGWR(597,DENALI_CTL_597_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_598_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(598,DENALI_CTL_598_DATA,0);
       #else
         REGWR(598,DENALI_CTL_598_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_599_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(599,DENALI_CTL_599_DATA,0);
       #else
         REGWR(599,DENALI_CTL_599_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_600_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(600,DENALI_CTL_600_DATA,0);
       #else
         REGWR(600,DENALI_CTL_600_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_601_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(601,DENALI_CTL_601_DATA,0);
       #else
         REGWR(601,DENALI_CTL_601_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_602_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(602,DENALI_CTL_602_DATA,0);
       #else
         REGWR(602,DENALI_CTL_602_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_603_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(603,DENALI_CTL_603_DATA,0);
       #else
         REGWR(603,DENALI_CTL_603_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_604_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(604,DENALI_CTL_604_DATA,0);
       #else
         REGWR(604,DENALI_CTL_604_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_605_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(605,DENALI_CTL_605_DATA,0);
       #else
         REGWR(605,DENALI_CTL_605_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_606_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(606,DENALI_CTL_606_DATA,0);
       #else
         REGWR(606,DENALI_CTL_606_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_607_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(607,DENALI_CTL_607_DATA,0);
       #else
         REGWR(607,DENALI_CTL_607_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_608_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(608,DENALI_CTL_608_DATA,0);
       #else
         REGWR(608,DENALI_CTL_608_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_609_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(609,DENALI_CTL_609_DATA,0);
       #else
         REGWR(609,DENALI_CTL_609_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_610_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(610,DENALI_CTL_610_DATA,0);
       #else
         REGWR(610,DENALI_CTL_610_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_611_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(611,DENALI_CTL_611_DATA,0);
       #else
         REGWR(611,DENALI_CTL_611_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_612_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(612,DENALI_CTL_612_DATA,0);
       #else
         REGWR(612,DENALI_CTL_612_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_613_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(613,DENALI_CTL_613_DATA,0);
       #else
         REGWR(613,DENALI_CTL_613_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_614_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(614,DENALI_CTL_614_DATA,0);
       #else
         REGWR(614,DENALI_CTL_614_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_615_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(615,DENALI_CTL_615_DATA,0);
       #else
         REGWR(615,DENALI_CTL_615_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_616_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(616,DENALI_CTL_616_DATA,0);
       #else
         REGWR(616,DENALI_CTL_616_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_617_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(617,DENALI_CTL_617_DATA,0);
       #else
         REGWR(617,DENALI_CTL_617_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_618_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(618,DENALI_CTL_618_DATA,0);
       #else
         REGWR(618,DENALI_CTL_618_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_619_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(619,DENALI_CTL_619_DATA,0);
       #else
         REGWR(619,DENALI_CTL_619_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_620_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(620,DENALI_CTL_620_DATA,0);
       #else
         REGWR(620,DENALI_CTL_620_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_621_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(621,DENALI_CTL_621_DATA,0);
       #else
         REGWR(621,DENALI_CTL_621_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_622_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(622,DENALI_CTL_622_DATA,0);
       #else
         REGWR(622,DENALI_CTL_622_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_623_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(623,DENALI_CTL_623_DATA,0);
       #else
         REGWR(623,DENALI_CTL_623_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_624_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(624,DENALI_CTL_624_DATA,0);
       #else
         REGWR(624,DENALI_CTL_624_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_625_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(625,DENALI_CTL_625_DATA,0);
       #else
         REGWR(625,DENALI_CTL_625_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_626_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(626,DENALI_CTL_626_DATA,0);
       #else
         REGWR(626,DENALI_CTL_626_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_627_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(627,DENALI_CTL_627_DATA,0);
       #else
         REGWR(627,DENALI_CTL_627_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_628_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(628,DENALI_CTL_628_DATA,0);
       #else
         REGWR(628,DENALI_CTL_628_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_629_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(629,DENALI_CTL_629_DATA,0);
       #else
         REGWR(629,DENALI_CTL_629_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_630_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(630,DENALI_CTL_630_DATA,0);
       #else
         REGWR(630,DENALI_CTL_630_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_631_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(631,DENALI_CTL_631_DATA,0);
       #else
         REGWR(631,DENALI_CTL_631_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_632_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(632,DENALI_CTL_632_DATA,0);
       #else
         REGWR(632,DENALI_CTL_632_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_633_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(633,DENALI_CTL_633_DATA,0);
       #else
         REGWR(633,DENALI_CTL_633_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_634_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(634,DENALI_CTL_634_DATA,0);
       #else
         REGWR(634,DENALI_CTL_634_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_635_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(635,DENALI_CTL_635_DATA,0);
       #else
         REGWR(635,DENALI_CTL_635_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_636_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(636,DENALI_CTL_636_DATA,0);
       #else
         REGWR(636,DENALI_CTL_636_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_637_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(637,DENALI_CTL_637_DATA,0);
       #else
         REGWR(637,DENALI_CTL_637_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_638_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(638,DENALI_CTL_638_DATA,0);
       #else
         REGWR(638,DENALI_CTL_638_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_639_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(639,DENALI_CTL_639_DATA,0);
       #else
         REGWR(639,DENALI_CTL_639_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_640_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(640,DENALI_CTL_640_DATA,0);
       #else
         REGWR(640,DENALI_CTL_640_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_641_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(641,DENALI_CTL_641_DATA,0);
       #else
         REGWR(641,DENALI_CTL_641_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_642_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(642,DENALI_CTL_642_DATA,0);
       #else
         REGWR(642,DENALI_CTL_642_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_643_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(643,DENALI_CTL_643_DATA,0);
       #else
         REGWR(643,DENALI_CTL_643_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_644_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(644,DENALI_CTL_644_DATA,0);
       #else
         REGWR(644,DENALI_CTL_644_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_645_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(645,DENALI_CTL_645_DATA,0);
       #else
         REGWR(645,DENALI_CTL_645_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_646_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(646,DENALI_CTL_646_DATA,0);
       #else
         REGWR(646,DENALI_CTL_646_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_647_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(647,DENALI_CTL_647_DATA,0);
       #else
         REGWR(647,DENALI_CTL_647_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_648_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(648,DENALI_CTL_648_DATA,0);
       #else
         REGWR(648,DENALI_CTL_648_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_649_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(649,DENALI_CTL_649_DATA,0);
       #else
         REGWR(649,DENALI_CTL_649_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_650_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(650,DENALI_CTL_650_DATA,0);
       #else
         REGWR(650,DENALI_CTL_650_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_651_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(651,DENALI_CTL_651_DATA,0);
       #else
         REGWR(651,DENALI_CTL_651_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_652_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(652,DENALI_CTL_652_DATA,0);
       #else
         REGWR(652,DENALI_CTL_652_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_653_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(653,DENALI_CTL_653_DATA,0);
       #else
         REGWR(653,DENALI_CTL_653_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_654_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(654,DENALI_CTL_654_DATA,0);
       #else
         REGWR(654,DENALI_CTL_654_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_655_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(655,DENALI_CTL_655_DATA,0);
       #else
         REGWR(655,DENALI_CTL_655_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_656_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(656,DENALI_CTL_656_DATA,0);
       #else
         REGWR(656,DENALI_CTL_656_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_657_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(657,DENALI_CTL_657_DATA,0);
       #else
         REGWR(657,DENALI_CTL_657_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_658_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(658,DENALI_CTL_658_DATA,0);
       #else
         REGWR(658,DENALI_CTL_658_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_659_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(659,DENALI_CTL_659_DATA,0);
       #else
         REGWR(659,DENALI_CTL_659_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_660_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(660,DENALI_CTL_660_DATA,0);
       #else
         REGWR(660,DENALI_CTL_660_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_661_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(661,DENALI_CTL_661_DATA,0);
       #else
         REGWR(661,DENALI_CTL_661_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_662_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(662,DENALI_CTL_662_DATA,0);
       #else
         REGWR(662,DENALI_CTL_662_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_663_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(663,DENALI_CTL_663_DATA,0);
       #else
         REGWR(663,DENALI_CTL_663_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_664_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(664,DENALI_CTL_664_DATA,0);
       #else
         REGWR(664,DENALI_CTL_664_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_665_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(665,DENALI_CTL_665_DATA,0);
       #else
         REGWR(665,DENALI_CTL_665_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_666_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(666,DENALI_CTL_666_DATA,0);
       #else
         REGWR(666,DENALI_CTL_666_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_667_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(667,DENALI_CTL_667_DATA,0);
       #else
         REGWR(667,DENALI_CTL_667_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_668_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(668,DENALI_CTL_668_DATA,0);
       #else
         REGWR(668,DENALI_CTL_668_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_669_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(669,DENALI_CTL_669_DATA,0);
       #else
         REGWR(669,DENALI_CTL_669_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_670_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(670,DENALI_CTL_670_DATA,0);
       #else
         REGWR(670,DENALI_CTL_670_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_671_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(671,DENALI_CTL_671_DATA,0);
       #else
         REGWR(671,DENALI_CTL_671_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_672_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(672,DENALI_CTL_672_DATA,0);
       #else
         REGWR(672,DENALI_CTL_672_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_673_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(673,DENALI_CTL_673_DATA,0);
       #else
         REGWR(673,DENALI_CTL_673_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_674_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(674,DENALI_CTL_674_DATA,0);
       #else
         REGWR(674,DENALI_CTL_674_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_675_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(675,DENALI_CTL_675_DATA,0);
       #else
         REGWR(675,DENALI_CTL_675_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_676_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(676,DENALI_CTL_676_DATA,0);
       #else
         REGWR(676,DENALI_CTL_676_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_677_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(677,DENALI_CTL_677_DATA,0);
       #else
         REGWR(677,DENALI_CTL_677_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_678_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(678,DENALI_CTL_678_DATA,0);
       #else
         REGWR(678,DENALI_CTL_678_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_679_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(679,DENALI_CTL_679_DATA,0);
       #else
         REGWR(679,DENALI_CTL_679_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_680_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(680,DENALI_CTL_680_DATA,0);
       #else
         REGWR(680,DENALI_CTL_680_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_681_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(681,DENALI_CTL_681_DATA,0);
       #else
         REGWR(681,DENALI_CTL_681_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_682_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(682,DENALI_CTL_682_DATA,0);
       #else
         REGWR(682,DENALI_CTL_682_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_683_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(683,DENALI_CTL_683_DATA,0);
       #else
         REGWR(683,DENALI_CTL_683_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_684_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(684,DENALI_CTL_684_DATA,0);
       #else
         REGWR(684,DENALI_CTL_684_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_685_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(685,DENALI_CTL_685_DATA,0);
       #else
         REGWR(685,DENALI_CTL_685_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_686_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(686,DENALI_CTL_686_DATA,0);
       #else
         REGWR(686,DENALI_CTL_686_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_687_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(687,DENALI_CTL_687_DATA,0);
       #else
         REGWR(687,DENALI_CTL_687_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_688_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(688,DENALI_CTL_688_DATA,0);
       #else
         REGWR(688,DENALI_CTL_688_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_689_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(689,DENALI_CTL_689_DATA,0);
       #else
         REGWR(689,DENALI_CTL_689_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_690_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(690,DENALI_CTL_690_DATA,0);
       #else
         REGWR(690,DENALI_CTL_690_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_691_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(691,DENALI_CTL_691_DATA,0);
       #else
         REGWR(691,DENALI_CTL_691_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_692_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(692,DENALI_CTL_692_DATA,0);
       #else
         REGWR(692,DENALI_CTL_692_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_693_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(693,DENALI_CTL_693_DATA,0);
       #else
         REGWR(693,DENALI_CTL_693_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_694_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(694,DENALI_CTL_694_DATA,0);
       #else
         REGWR(694,DENALI_CTL_694_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_695_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(695,DENALI_CTL_695_DATA,0);
       #else
         REGWR(695,DENALI_CTL_695_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_696_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(696,DENALI_CTL_696_DATA,0);
       #else
         REGWR(696,DENALI_CTL_696_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_697_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(697,DENALI_CTL_697_DATA,0);
       #else
         REGWR(697,DENALI_CTL_697_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_698_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(698,DENALI_CTL_698_DATA,0);
       #else
         REGWR(698,DENALI_CTL_698_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_699_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(699,DENALI_CTL_699_DATA,0);
       #else
         REGWR(699,DENALI_CTL_699_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_700_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(700,DENALI_CTL_700_DATA,0);
       #else
         REGWR(700,DENALI_CTL_700_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_701_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(701,DENALI_CTL_701_DATA,0);
       #else
         REGWR(701,DENALI_CTL_701_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_702_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(702,DENALI_CTL_702_DATA,0);
       #else
         REGWR(702,DENALI_CTL_702_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_703_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(703,DENALI_CTL_703_DATA,0);
       #else
         REGWR(703,DENALI_CTL_703_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_704_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(704,DENALI_CTL_704_DATA,0);
       #else
         REGWR(704,DENALI_CTL_704_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_705_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(705,DENALI_CTL_705_DATA,0);
       #else
         REGWR(705,DENALI_CTL_705_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_706_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(706,DENALI_CTL_706_DATA,0);
       #else
         REGWR(706,DENALI_CTL_706_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_707_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(707,DENALI_CTL_707_DATA,0);
       #else
         REGWR(707,DENALI_CTL_707_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_708_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(708,DENALI_CTL_708_DATA,0);
       #else
         REGWR(708,DENALI_CTL_708_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_709_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(709,DENALI_CTL_709_DATA,0);
       #else
         REGWR(709,DENALI_CTL_709_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_710_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(710,DENALI_CTL_710_DATA,0);
       #else
         REGWR(710,DENALI_CTL_710_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_711_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(711,DENALI_CTL_711_DATA,0);
       #else
         REGWR(711,DENALI_CTL_711_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_712_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(712,DENALI_CTL_712_DATA,0);
       #else
         REGWR(712,DENALI_CTL_712_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_713_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(713,DENALI_CTL_713_DATA,0);
       #else
         REGWR(713,DENALI_CTL_713_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_714_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(714,DENALI_CTL_714_DATA,0);
       #else
         REGWR(714,DENALI_CTL_714_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_715_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(715,DENALI_CTL_715_DATA,0);
       #else
         REGWR(715,DENALI_CTL_715_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_716_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(716,DENALI_CTL_716_DATA,0);
       #else
         REGWR(716,DENALI_CTL_716_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_717_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(717,DENALI_CTL_717_DATA,0);
       #else
         REGWR(717,DENALI_CTL_717_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_718_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(718,DENALI_CTL_718_DATA,0);
       #else
         REGWR(718,DENALI_CTL_718_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_719_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(719,DENALI_CTL_719_DATA,0);
       #else
         REGWR(719,DENALI_CTL_719_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_720_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(720,DENALI_CTL_720_DATA,0);
       #else
         REGWR(720,DENALI_CTL_720_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_721_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(721,DENALI_CTL_721_DATA,0);
       #else
         REGWR(721,DENALI_CTL_721_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_722_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(722,DENALI_CTL_722_DATA,0);
       #else
         REGWR(722,DENALI_CTL_722_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_723_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(723,DENALI_CTL_723_DATA,0);
       #else
         REGWR(723,DENALI_CTL_723_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_724_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(724,DENALI_CTL_724_DATA,0);
       #else
         REGWR(724,DENALI_CTL_724_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_725_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(725,DENALI_CTL_725_DATA,0);
       #else
         REGWR(725,DENALI_CTL_725_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_726_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(726,DENALI_CTL_726_DATA,0);
       #else
         REGWR(726,DENALI_CTL_726_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_727_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(727,DENALI_CTL_727_DATA,0);
       #else
         REGWR(727,DENALI_CTL_727_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_728_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(728,DENALI_CTL_728_DATA,0);
       #else
         REGWR(728,DENALI_CTL_728_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_729_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(729,DENALI_CTL_729_DATA,0);
       #else
         REGWR(729,DENALI_CTL_729_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_730_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(730,DENALI_CTL_730_DATA,0);
       #else
         REGWR(730,DENALI_CTL_730_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_731_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(731,DENALI_CTL_731_DATA,0);
       #else
         REGWR(731,DENALI_CTL_731_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_732_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(732,DENALI_CTL_732_DATA,0);
       #else
         REGWR(732,DENALI_CTL_732_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_733_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(733,DENALI_CTL_733_DATA,0);
       #else
         REGWR(733,DENALI_CTL_733_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_734_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(734,DENALI_CTL_734_DATA,0);
       #else
         REGWR(734,DENALI_CTL_734_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_735_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(735,DENALI_CTL_735_DATA,0);
       #else
         REGWR(735,DENALI_CTL_735_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_736_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(736,DENALI_CTL_736_DATA,0);
       #else
         REGWR(736,DENALI_CTL_736_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_737_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(737,DENALI_CTL_737_DATA,0);
       #else
         REGWR(737,DENALI_CTL_737_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_738_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(738,DENALI_CTL_738_DATA,0);
       #else
         REGWR(738,DENALI_CTL_738_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_739_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(739,DENALI_CTL_739_DATA,0);
       #else
         REGWR(739,DENALI_CTL_739_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_740_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(740,DENALI_CTL_740_DATA,0);
       #else
         REGWR(740,DENALI_CTL_740_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_741_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(741,DENALI_CTL_741_DATA,0);
       #else
         REGWR(741,DENALI_CTL_741_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_742_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(742,DENALI_CTL_742_DATA,0);
       #else
         REGWR(742,DENALI_CTL_742_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_743_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(743,DENALI_CTL_743_DATA,0);
       #else
         REGWR(743,DENALI_CTL_743_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_744_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(744,DENALI_CTL_744_DATA,0);
       #else
         REGWR(744,DENALI_CTL_744_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_745_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(745,DENALI_CTL_745_DATA,0);
       #else
         REGWR(745,DENALI_CTL_745_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_746_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(746,DENALI_CTL_746_DATA,0);
       #else
         REGWR(746,DENALI_CTL_746_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_747_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(747,DENALI_CTL_747_DATA,0);
       #else
         REGWR(747,DENALI_CTL_747_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_748_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(748,DENALI_CTL_748_DATA,0);
       #else
         REGWR(748,DENALI_CTL_748_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_749_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(749,DENALI_CTL_749_DATA,0);
       #else
         REGWR(749,DENALI_CTL_749_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_750_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(750,DENALI_CTL_750_DATA,0);
       #else
         REGWR(750,DENALI_CTL_750_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_751_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(751,DENALI_CTL_751_DATA,0);
       #else
         REGWR(751,DENALI_CTL_751_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_752_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(752,DENALI_CTL_752_DATA,0);
       #else
         REGWR(752,DENALI_CTL_752_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_753_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(753,DENALI_CTL_753_DATA,0);
       #else
         REGWR(753,DENALI_CTL_753_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_754_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(754,DENALI_CTL_754_DATA,0);
       #else
         REGWR(754,DENALI_CTL_754_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_755_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(755,DENALI_CTL_755_DATA,0);
       #else
         REGWR(755,DENALI_CTL_755_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_756_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(756,DENALI_CTL_756_DATA,0);
       #else
         REGWR(756,DENALI_CTL_756_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_757_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(757,DENALI_CTL_757_DATA,0);
       #else
         REGWR(757,DENALI_CTL_757_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_758_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(758,DENALI_CTL_758_DATA,0);
       #else
         REGWR(758,DENALI_CTL_758_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_759_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(759,DENALI_CTL_759_DATA,0);
       #else
         REGWR(759,DENALI_CTL_759_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_760_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(760,DENALI_CTL_760_DATA,0);
       #else
         REGWR(760,DENALI_CTL_760_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_761_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(761,DENALI_CTL_761_DATA,0);
       #else
         REGWR(761,DENALI_CTL_761_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_762_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(762,DENALI_CTL_762_DATA,0);
       #else
         REGWR(762,DENALI_CTL_762_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_763_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(763,DENALI_CTL_763_DATA,0);
       #else
         REGWR(763,DENALI_CTL_763_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_764_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(764,DENALI_CTL_764_DATA,0);
       #else
         REGWR(764,DENALI_CTL_764_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_765_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(765,DENALI_CTL_765_DATA,0);
       #else
         REGWR(765,DENALI_CTL_765_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_766_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(766,DENALI_CTL_766_DATA,0);
       #else
         REGWR(766,DENALI_CTL_766_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_767_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(767,DENALI_CTL_767_DATA,0);
       #else
         REGWR(767,DENALI_CTL_767_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_768_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(768,DENALI_CTL_768_DATA,0);
       #else
         REGWR(768,DENALI_CTL_768_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_769_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(769,DENALI_CTL_769_DATA,0);
       #else
         REGWR(769,DENALI_CTL_769_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_770_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(770,DENALI_CTL_770_DATA,0);
       #else
         REGWR(770,DENALI_CTL_770_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_771_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(771,DENALI_CTL_771_DATA,0);
       #else
         REGWR(771,DENALI_CTL_771_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_772_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(772,DENALI_CTL_772_DATA,0);
       #else
         REGWR(772,DENALI_CTL_772_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_773_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(773,DENALI_CTL_773_DATA,0);
       #else
         REGWR(773,DENALI_CTL_773_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_774_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(774,DENALI_CTL_774_DATA,0);
       #else
         REGWR(774,DENALI_CTL_774_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_775_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(775,DENALI_CTL_775_DATA,0);
       #else
         REGWR(775,DENALI_CTL_775_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_776_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(776,DENALI_CTL_776_DATA,0);
       #else
         REGWR(776,DENALI_CTL_776_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_777_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(777,DENALI_CTL_777_DATA,0);
       #else
         REGWR(777,DENALI_CTL_777_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_778_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(778,DENALI_CTL_778_DATA,0);
       #else
         REGWR(778,DENALI_CTL_778_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_779_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(779,DENALI_CTL_779_DATA,0);
       #else
         REGWR(779,DENALI_CTL_779_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_780_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(780,DENALI_CTL_780_DATA,0);
       #else
         REGWR(780,DENALI_CTL_780_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_781_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(781,DENALI_CTL_781_DATA,0);
       #else
         REGWR(781,DENALI_CTL_781_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_782_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(782,DENALI_CTL_782_DATA,0);
       #else
         REGWR(782,DENALI_CTL_782_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_783_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(783,DENALI_CTL_783_DATA,0);
       #else
         REGWR(783,DENALI_CTL_783_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_784_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(784,DENALI_CTL_784_DATA,0);
       #else
         REGWR(784,DENALI_CTL_784_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_785_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(785,DENALI_CTL_785_DATA,0);
       #else
         REGWR(785,DENALI_CTL_785_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_786_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(786,DENALI_CTL_786_DATA,0);
       #else
         REGWR(786,DENALI_CTL_786_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_787_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(787,DENALI_CTL_787_DATA,0);
       #else
         REGWR(787,DENALI_CTL_787_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_788_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(788,DENALI_CTL_788_DATA,0);
       #else
         REGWR(788,DENALI_CTL_788_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_789_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(789,DENALI_CTL_789_DATA,0);
       #else
         REGWR(789,DENALI_CTL_789_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_790_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(790,DENALI_CTL_790_DATA,0);
       #else
         REGWR(790,DENALI_CTL_790_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_791_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(791,DENALI_CTL_791_DATA,0);
       #else
         REGWR(791,DENALI_CTL_791_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_792_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(792,DENALI_CTL_792_DATA,0);
       #else
         REGWR(792,DENALI_CTL_792_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_793_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(793,DENALI_CTL_793_DATA,0);
       #else
         REGWR(793,DENALI_CTL_793_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_794_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(794,DENALI_CTL_794_DATA,0);
       #else
         REGWR(794,DENALI_CTL_794_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_795_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(795,DENALI_CTL_795_DATA,0);
       #else
         REGWR(795,DENALI_CTL_795_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_796_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(796,DENALI_CTL_796_DATA,0);
       #else
         REGWR(796,DENALI_CTL_796_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_797_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(797,DENALI_CTL_797_DATA,0);
       #else
         REGWR(797,DENALI_CTL_797_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_798_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(798,DENALI_CTL_798_DATA,0);
       #else
         REGWR(798,DENALI_CTL_798_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_799_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(799,DENALI_CTL_799_DATA,0);
       #else
         REGWR(799,DENALI_CTL_799_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_800_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(800,DENALI_CTL_800_DATA,0);
       #else
         REGWR(800,DENALI_CTL_800_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_801_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(801,DENALI_CTL_801_DATA,0);
       #else
         REGWR(801,DENALI_CTL_801_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_802_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(802,DENALI_CTL_802_DATA,0);
       #else
         REGWR(802,DENALI_CTL_802_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_803_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(803,DENALI_CTL_803_DATA,0);
       #else
         REGWR(803,DENALI_CTL_803_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_804_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(804,DENALI_CTL_804_DATA,0);
       #else
         REGWR(804,DENALI_CTL_804_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_805_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(805,DENALI_CTL_805_DATA,0);
       #else
         REGWR(805,DENALI_CTL_805_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_806_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(806,DENALI_CTL_806_DATA,0);
       #else
         REGWR(806,DENALI_CTL_806_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_807_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(807,DENALI_CTL_807_DATA,0);
       #else
         REGWR(807,DENALI_CTL_807_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_808_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(808,DENALI_CTL_808_DATA,0);
       #else
         REGWR(808,DENALI_CTL_808_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_809_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(809,DENALI_CTL_809_DATA,0);
       #else
         REGWR(809,DENALI_CTL_809_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_810_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(810,DENALI_CTL_810_DATA,0);
       #else
         REGWR(810,DENALI_CTL_810_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_811_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(811,DENALI_CTL_811_DATA,0);
       #else
         REGWR(811,DENALI_CTL_811_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_812_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(812,DENALI_CTL_812_DATA,0);
       #else
         REGWR(812,DENALI_CTL_812_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_813_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(813,DENALI_CTL_813_DATA,0);
       #else
         REGWR(813,DENALI_CTL_813_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_814_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(814,DENALI_CTL_814_DATA,0);
       #else
         REGWR(814,DENALI_CTL_814_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_815_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(815,DENALI_CTL_815_DATA,0);
       #else
         REGWR(815,DENALI_CTL_815_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_816_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(816,DENALI_CTL_816_DATA,0);
       #else
         REGWR(816,DENALI_CTL_816_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_817_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(817,DENALI_CTL_817_DATA,0);
       #else
         REGWR(817,DENALI_CTL_817_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_818_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(818,DENALI_CTL_818_DATA,0);
       #else
         REGWR(818,DENALI_CTL_818_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_819_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(819,DENALI_CTL_819_DATA,0);
       #else
         REGWR(819,DENALI_CTL_819_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_820_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(820,DENALI_CTL_820_DATA,0);
       #else
         REGWR(820,DENALI_CTL_820_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_821_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(821,DENALI_CTL_821_DATA,0);
       #else
         REGWR(821,DENALI_CTL_821_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_822_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(822,DENALI_CTL_822_DATA,0);
       #else
         REGWR(822,DENALI_CTL_822_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_823_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(823,DENALI_CTL_823_DATA,0);
       #else
         REGWR(823,DENALI_CTL_823_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_824_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(824,DENALI_CTL_824_DATA,0);
       #else
         REGWR(824,DENALI_CTL_824_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_825_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(825,DENALI_CTL_825_DATA,0);
       #else
         REGWR(825,DENALI_CTL_825_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_826_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(826,DENALI_CTL_826_DATA,0);
       #else
         REGWR(826,DENALI_CTL_826_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_827_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(827,DENALI_CTL_827_DATA,0);
       #else
         REGWR(827,DENALI_CTL_827_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_828_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(828,DENALI_CTL_828_DATA,0);
       #else
         REGWR(828,DENALI_CTL_828_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_829_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(829,DENALI_CTL_829_DATA,0);
       #else
         REGWR(829,DENALI_CTL_829_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_830_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(830,DENALI_CTL_830_DATA,0);
       #else
         REGWR(830,DENALI_CTL_830_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_831_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(831,DENALI_CTL_831_DATA,0);
       #else
         REGWR(831,DENALI_CTL_831_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_832_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(832,DENALI_CTL_832_DATA,0);
       #else
         REGWR(832,DENALI_CTL_832_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_833_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(833,DENALI_CTL_833_DATA,0);
       #else
         REGWR(833,DENALI_CTL_833_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_834_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(834,DENALI_CTL_834_DATA,0);
       #else
         REGWR(834,DENALI_CTL_834_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_835_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(835,DENALI_CTL_835_DATA,0);
       #else
         REGWR(835,DENALI_CTL_835_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_836_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(836,DENALI_CTL_836_DATA,0);
       #else
         REGWR(836,DENALI_CTL_836_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_837_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(837,DENALI_CTL_837_DATA,0);
       #else
         REGWR(837,DENALI_CTL_837_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_838_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(838,DENALI_CTL_838_DATA,0);
       #else
         REGWR(838,DENALI_CTL_838_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_839_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(839,DENALI_CTL_839_DATA,0);
       #else
         REGWR(839,DENALI_CTL_839_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_840_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(840,DENALI_CTL_840_DATA,0);
       #else
         REGWR(840,DENALI_CTL_840_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_841_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(841,DENALI_CTL_841_DATA,0);
       #else
         REGWR(841,DENALI_CTL_841_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_842_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(842,DENALI_CTL_842_DATA,0);
       #else
         REGWR(842,DENALI_CTL_842_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_843_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(843,DENALI_CTL_843_DATA,0);
       #else
         REGWR(843,DENALI_CTL_843_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_844_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(844,DENALI_CTL_844_DATA,0);
       #else
         REGWR(844,DENALI_CTL_844_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_845_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(845,DENALI_CTL_845_DATA,0);
       #else
         REGWR(845,DENALI_CTL_845_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_846_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(846,DENALI_CTL_846_DATA,0);
       #else
         REGWR(846,DENALI_CTL_846_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_847_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(847,DENALI_CTL_847_DATA,0);
       #else
         REGWR(847,DENALI_CTL_847_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_848_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(848,DENALI_CTL_848_DATA,0);
       #else
         REGWR(848,DENALI_CTL_848_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_849_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(849,DENALI_CTL_849_DATA,0);
       #else
         REGWR(849,DENALI_CTL_849_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_850_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(850,DENALI_CTL_850_DATA,0);
       #else
         REGWR(850,DENALI_CTL_850_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_851_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(851,DENALI_CTL_851_DATA,0);
       #else
         REGWR(851,DENALI_CTL_851_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_852_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(852,DENALI_CTL_852_DATA,0);
       #else
         REGWR(852,DENALI_CTL_852_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_853_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(853,DENALI_CTL_853_DATA,0);
       #else
         REGWR(853,DENALI_CTL_853_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_854_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(854,DENALI_CTL_854_DATA,0);
       #else
         REGWR(854,DENALI_CTL_854_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_855_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(855,DENALI_CTL_855_DATA,0);
       #else
         REGWR(855,DENALI_CTL_855_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_856_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(856,DENALI_CTL_856_DATA,0);
       #else
         REGWR(856,DENALI_CTL_856_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_857_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(857,DENALI_CTL_857_DATA,0);
       #else
         REGWR(857,DENALI_CTL_857_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_858_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(858,DENALI_CTL_858_DATA,0);
       #else
         REGWR(858,DENALI_CTL_858_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_859_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(859,DENALI_CTL_859_DATA,0);
       #else
         REGWR(859,DENALI_CTL_859_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_860_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(860,DENALI_CTL_860_DATA,0);
       #else
         REGWR(860,DENALI_CTL_860_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_861_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(861,DENALI_CTL_861_DATA,0);
       #else
         REGWR(861,DENALI_CTL_861_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_862_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(862,DENALI_CTL_862_DATA,0);
       #else
         REGWR(862,DENALI_CTL_862_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_863_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(863,DENALI_CTL_863_DATA,0);
       #else
         REGWR(863,DENALI_CTL_863_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_864_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(864,DENALI_CTL_864_DATA,0);
       #else
         REGWR(864,DENALI_CTL_864_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_865_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(865,DENALI_CTL_865_DATA,0);
       #else
         REGWR(865,DENALI_CTL_865_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_866_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(866,DENALI_CTL_866_DATA,0);
       #else
         REGWR(866,DENALI_CTL_866_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_867_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(867,DENALI_CTL_867_DATA,0);
       #else
         REGWR(867,DENALI_CTL_867_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_868_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(868,DENALI_CTL_868_DATA,0);
       #else
         REGWR(868,DENALI_CTL_868_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_869_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(869,DENALI_CTL_869_DATA,0);
       #else
         REGWR(869,DENALI_CTL_869_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_870_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(870,DENALI_CTL_870_DATA,0);
       #else
         REGWR(870,DENALI_CTL_870_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_871_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(871,DENALI_CTL_871_DATA,0);
       #else
         REGWR(871,DENALI_CTL_871_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_872_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(872,DENALI_CTL_872_DATA,0);
       #else
         REGWR(872,DENALI_CTL_872_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_873_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(873,DENALI_CTL_873_DATA,0);
       #else
         REGWR(873,DENALI_CTL_873_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_874_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(874,DENALI_CTL_874_DATA,0);
       #else
         REGWR(874,DENALI_CTL_874_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_875_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(875,DENALI_CTL_875_DATA,0);
       #else
         REGWR(875,DENALI_CTL_875_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_876_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(876,DENALI_CTL_876_DATA,0);
       #else
         REGWR(876,DENALI_CTL_876_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_877_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(877,DENALI_CTL_877_DATA,0);
       #else
         REGWR(877,DENALI_CTL_877_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_878_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(878,DENALI_CTL_878_DATA,0);
       #else
         REGWR(878,DENALI_CTL_878_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_879_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(879,DENALI_CTL_879_DATA,0);
       #else
         REGWR(879,DENALI_CTL_879_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_880_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(880,DENALI_CTL_880_DATA,0);
       #else
         REGWR(880,DENALI_CTL_880_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_881_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(881,DENALI_CTL_881_DATA,0);
       #else
         REGWR(881,DENALI_CTL_881_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_882_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(882,DENALI_CTL_882_DATA,0);
       #else
         REGWR(882,DENALI_CTL_882_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_883_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(883,DENALI_CTL_883_DATA,0);
       #else
         REGWR(883,DENALI_CTL_883_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_884_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(884,DENALI_CTL_884_DATA,0);
       #else
         REGWR(884,DENALI_CTL_884_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_885_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(885,DENALI_CTL_885_DATA,0);
       #else
         REGWR(885,DENALI_CTL_885_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_886_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(886,DENALI_CTL_886_DATA,0);
       #else
         REGWR(886,DENALI_CTL_886_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_887_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(887,DENALI_CTL_887_DATA,0);
       #else
         REGWR(887,DENALI_CTL_887_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_888_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(888,DENALI_CTL_888_DATA,0);
       #else
         REGWR(888,DENALI_CTL_888_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_889_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(889,DENALI_CTL_889_DATA,0);
       #else
         REGWR(889,DENALI_CTL_889_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_890_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(890,DENALI_CTL_890_DATA,0);
       #else
         REGWR(890,DENALI_CTL_890_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_891_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(891,DENALI_CTL_891_DATA,0);
       #else
         REGWR(891,DENALI_CTL_891_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_892_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(892,DENALI_CTL_892_DATA,0);
       #else
         REGWR(892,DENALI_CTL_892_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_893_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(893,DENALI_CTL_893_DATA,0);
       #else
         REGWR(893,DENALI_CTL_893_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_894_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(894,DENALI_CTL_894_DATA,0);
       #else
         REGWR(894,DENALI_CTL_894_DATA,0);
       #endif
#endif
#ifdef DENALI_CTL_895_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_CTL(895,DENALI_CTL_895_DATA,0);
       #else
         REGWR(895,DENALI_CTL_895_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_00_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(0 + PI_BASE_ADDR,DENALI_PI_00_DATA,0);
       #else
         REGWR(0 + PI_BASE_ADDR,DENALI_PI_00_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_01_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1 + PI_BASE_ADDR,DENALI_PI_01_DATA,0);
       #else
         REGWR(1 + PI_BASE_ADDR,DENALI_PI_01_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_02_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(2 + PI_BASE_ADDR,DENALI_PI_02_DATA,0);
       #else
         REGWR(2 + PI_BASE_ADDR,DENALI_PI_02_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_03_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(3 + PI_BASE_ADDR,DENALI_PI_03_DATA,0);
       #else
         REGWR(3 + PI_BASE_ADDR,DENALI_PI_03_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_04_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(4 + PI_BASE_ADDR,DENALI_PI_04_DATA,0);
       #else
         REGWR(4 + PI_BASE_ADDR,DENALI_PI_04_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_05_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(5 + PI_BASE_ADDR,DENALI_PI_05_DATA,0);
       #else
         REGWR(5 + PI_BASE_ADDR,DENALI_PI_05_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_06_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(6 + PI_BASE_ADDR,DENALI_PI_06_DATA,0);
       #else
         REGWR(6 + PI_BASE_ADDR,DENALI_PI_06_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_07_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(7 + PI_BASE_ADDR,DENALI_PI_07_DATA,0);
       #else
         REGWR(7 + PI_BASE_ADDR,DENALI_PI_07_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_08_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(8 + PI_BASE_ADDR,DENALI_PI_08_DATA,0);
       #else
         REGWR(8 + PI_BASE_ADDR,DENALI_PI_08_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_09_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(9 + PI_BASE_ADDR,DENALI_PI_09_DATA,0);
       #else
         REGWR(9 + PI_BASE_ADDR,DENALI_PI_09_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_10_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(10 + PI_BASE_ADDR,DENALI_PI_10_DATA,0);
       #else
         REGWR(10 + PI_BASE_ADDR,DENALI_PI_10_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_11_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(11 + PI_BASE_ADDR,DENALI_PI_11_DATA,0);
       #else
         REGWR(11 + PI_BASE_ADDR,DENALI_PI_11_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_12_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(12 + PI_BASE_ADDR,DENALI_PI_12_DATA,0);
       #else
         REGWR(12 + PI_BASE_ADDR,DENALI_PI_12_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_13_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(13 + PI_BASE_ADDR,DENALI_PI_13_DATA,0);
       #else
         REGWR(13 + PI_BASE_ADDR,DENALI_PI_13_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_14_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(14 + PI_BASE_ADDR,DENALI_PI_14_DATA,0);
       #else
         REGWR(14 + PI_BASE_ADDR,DENALI_PI_14_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_15_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(15 + PI_BASE_ADDR,DENALI_PI_15_DATA,0);
       #else
         REGWR(15 + PI_BASE_ADDR,DENALI_PI_15_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_16_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(16 + PI_BASE_ADDR,DENALI_PI_16_DATA,0);
       #else
         REGWR(16 + PI_BASE_ADDR,DENALI_PI_16_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_17_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(17 + PI_BASE_ADDR,DENALI_PI_17_DATA,0);
       #else
         REGWR(17 + PI_BASE_ADDR,DENALI_PI_17_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_18_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(18 + PI_BASE_ADDR,DENALI_PI_18_DATA,0);
       #else
         REGWR(18 + PI_BASE_ADDR,DENALI_PI_18_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_19_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(19 + PI_BASE_ADDR,DENALI_PI_19_DATA,0);
       #else
         REGWR(19 + PI_BASE_ADDR,DENALI_PI_19_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_20_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(20 + PI_BASE_ADDR,DENALI_PI_20_DATA,0);
       #else
         REGWR(20 + PI_BASE_ADDR,DENALI_PI_20_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_21_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(21 + PI_BASE_ADDR,DENALI_PI_21_DATA,0);
       #else
         REGWR(21 + PI_BASE_ADDR,DENALI_PI_21_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_22_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(22 + PI_BASE_ADDR,DENALI_PI_22_DATA,0);
       #else
         REGWR(22 + PI_BASE_ADDR,DENALI_PI_22_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_23_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(23 + PI_BASE_ADDR,DENALI_PI_23_DATA,0);
       #else
         REGWR(23 + PI_BASE_ADDR,DENALI_PI_23_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_24_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(24 + PI_BASE_ADDR,DENALI_PI_24_DATA,0);
       #else
         REGWR(24 + PI_BASE_ADDR,DENALI_PI_24_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_25_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(25 + PI_BASE_ADDR,DENALI_PI_25_DATA,0);
       #else
         REGWR(25 + PI_BASE_ADDR,DENALI_PI_25_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_26_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(26 + PI_BASE_ADDR,DENALI_PI_26_DATA,0);
       #else
         REGWR(26 + PI_BASE_ADDR,DENALI_PI_26_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_27_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(27 + PI_BASE_ADDR,DENALI_PI_27_DATA,0);
       #else
         REGWR(27 + PI_BASE_ADDR,DENALI_PI_27_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_28_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(28 + PI_BASE_ADDR,DENALI_PI_28_DATA,0);
       #else
         REGWR(28 + PI_BASE_ADDR,DENALI_PI_28_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_29_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(29 + PI_BASE_ADDR,DENALI_PI_29_DATA,0);
       #else
         REGWR(29 + PI_BASE_ADDR,DENALI_PI_29_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_30_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(30 + PI_BASE_ADDR,DENALI_PI_30_DATA,0);
       #else
         REGWR(30 + PI_BASE_ADDR,DENALI_PI_30_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_31_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(31 + PI_BASE_ADDR,DENALI_PI_31_DATA,0);
       #else
         REGWR(31 + PI_BASE_ADDR,DENALI_PI_31_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_32_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(32 + PI_BASE_ADDR,DENALI_PI_32_DATA,0);
       #else
         REGWR(32 + PI_BASE_ADDR,DENALI_PI_32_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_33_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(33 + PI_BASE_ADDR,DENALI_PI_33_DATA,0);
       #else
         REGWR(33 + PI_BASE_ADDR,DENALI_PI_33_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_34_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(34 + PI_BASE_ADDR,DENALI_PI_34_DATA,0);
       #else
         REGWR(34 + PI_BASE_ADDR,DENALI_PI_34_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_35_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(35 + PI_BASE_ADDR,DENALI_PI_35_DATA,0);
       #else
         REGWR(35 + PI_BASE_ADDR,DENALI_PI_35_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_36_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(36 + PI_BASE_ADDR,DENALI_PI_36_DATA,0);
       #else
         REGWR(36 + PI_BASE_ADDR,DENALI_PI_36_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_37_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(37 + PI_BASE_ADDR,DENALI_PI_37_DATA,0);
       #else
         REGWR(37 + PI_BASE_ADDR,DENALI_PI_37_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_38_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(38 + PI_BASE_ADDR,DENALI_PI_38_DATA,0);
       #else
         REGWR(38 + PI_BASE_ADDR,DENALI_PI_38_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_39_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(39 + PI_BASE_ADDR,DENALI_PI_39_DATA,0);
       #else
         REGWR(39 + PI_BASE_ADDR,DENALI_PI_39_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_40_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(40 + PI_BASE_ADDR,DENALI_PI_40_DATA,0);
       #else
         REGWR(40 + PI_BASE_ADDR,DENALI_PI_40_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_41_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(41 + PI_BASE_ADDR,DENALI_PI_41_DATA,0);
       #else
         REGWR(41 + PI_BASE_ADDR,DENALI_PI_41_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_42_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(42 + PI_BASE_ADDR,DENALI_PI_42_DATA,0);
       #else
         REGWR(42 + PI_BASE_ADDR,DENALI_PI_42_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_43_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(43 + PI_BASE_ADDR,DENALI_PI_43_DATA,0);
       #else
         REGWR(43 + PI_BASE_ADDR,DENALI_PI_43_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_44_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(44 + PI_BASE_ADDR,DENALI_PI_44_DATA,0);
       #else
         REGWR(44 + PI_BASE_ADDR,DENALI_PI_44_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_45_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(45 + PI_BASE_ADDR,DENALI_PI_45_DATA,0);
       #else
         REGWR(45 + PI_BASE_ADDR,DENALI_PI_45_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_46_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(46 + PI_BASE_ADDR,DENALI_PI_46_DATA,0);
       #else
         REGWR(46 + PI_BASE_ADDR,DENALI_PI_46_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_47_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(47 + PI_BASE_ADDR,DENALI_PI_47_DATA,0);
       #else
         REGWR(47 + PI_BASE_ADDR,DENALI_PI_47_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_48_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(48 + PI_BASE_ADDR,DENALI_PI_48_DATA,0);
       #else
         REGWR(48 + PI_BASE_ADDR,DENALI_PI_48_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_49_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(49 + PI_BASE_ADDR,DENALI_PI_49_DATA,0);
       #else
         REGWR(49 + PI_BASE_ADDR,DENALI_PI_49_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_50_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(50 + PI_BASE_ADDR,DENALI_PI_50_DATA,0);
       #else
         REGWR(50 + PI_BASE_ADDR,DENALI_PI_50_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_51_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(51 + PI_BASE_ADDR,DENALI_PI_51_DATA,0);
       #else
         REGWR(51 + PI_BASE_ADDR,DENALI_PI_51_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_52_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(52 + PI_BASE_ADDR,DENALI_PI_52_DATA,0);
       #else
         REGWR(52 + PI_BASE_ADDR,DENALI_PI_52_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_53_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(53 + PI_BASE_ADDR,DENALI_PI_53_DATA,0);
       #else
         REGWR(53 + PI_BASE_ADDR,DENALI_PI_53_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_54_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(54 + PI_BASE_ADDR,DENALI_PI_54_DATA,0);
       #else
         REGWR(54 + PI_BASE_ADDR,DENALI_PI_54_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_55_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(55 + PI_BASE_ADDR,DENALI_PI_55_DATA,0);
       #else
         REGWR(55 + PI_BASE_ADDR,DENALI_PI_55_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_56_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(56 + PI_BASE_ADDR,DENALI_PI_56_DATA,0);
       #else
         REGWR(56 + PI_BASE_ADDR,DENALI_PI_56_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_57_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(57 + PI_BASE_ADDR,DENALI_PI_57_DATA,0);
       #else
         REGWR(57 + PI_BASE_ADDR,DENALI_PI_57_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_58_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(58 + PI_BASE_ADDR,DENALI_PI_58_DATA,0);
       #else
         REGWR(58 + PI_BASE_ADDR,DENALI_PI_58_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_59_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(59 + PI_BASE_ADDR,DENALI_PI_59_DATA,0);
       #else
         REGWR(59 + PI_BASE_ADDR,DENALI_PI_59_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_60_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(60 + PI_BASE_ADDR,DENALI_PI_60_DATA,0);
       #else
         REGWR(60 + PI_BASE_ADDR,DENALI_PI_60_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_61_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(61 + PI_BASE_ADDR,DENALI_PI_61_DATA,0);
       #else
         REGWR(61 + PI_BASE_ADDR,DENALI_PI_61_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_62_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(62 + PI_BASE_ADDR,DENALI_PI_62_DATA,0);
       #else
         REGWR(62 + PI_BASE_ADDR,DENALI_PI_62_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_63_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(63 + PI_BASE_ADDR,DENALI_PI_63_DATA,0);
       #else
         REGWR(63 + PI_BASE_ADDR,DENALI_PI_63_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_64_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(64 + PI_BASE_ADDR,DENALI_PI_64_DATA,0);
       #else
         REGWR(64 + PI_BASE_ADDR,DENALI_PI_64_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_65_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(65 + PI_BASE_ADDR,DENALI_PI_65_DATA,0);
       #else
         REGWR(65 + PI_BASE_ADDR,DENALI_PI_65_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_66_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(66 + PI_BASE_ADDR,DENALI_PI_66_DATA,0);
       #else
         REGWR(66 + PI_BASE_ADDR,DENALI_PI_66_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_67_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(67 + PI_BASE_ADDR,DENALI_PI_67_DATA,0);
       #else
         REGWR(67 + PI_BASE_ADDR,DENALI_PI_67_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_68_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(68 + PI_BASE_ADDR,DENALI_PI_68_DATA,0);
       #else
         REGWR(68 + PI_BASE_ADDR,DENALI_PI_68_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_69_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(69 + PI_BASE_ADDR,DENALI_PI_69_DATA,0);
       #else
         REGWR(69 + PI_BASE_ADDR,DENALI_PI_69_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_70_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(70 + PI_BASE_ADDR,DENALI_PI_70_DATA,0);
       #else
         REGWR(70 + PI_BASE_ADDR,DENALI_PI_70_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_71_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(71 + PI_BASE_ADDR,DENALI_PI_71_DATA,0);
       #else
         REGWR(71 + PI_BASE_ADDR,DENALI_PI_71_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_72_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(72 + PI_BASE_ADDR,DENALI_PI_72_DATA,0);
       #else
         REGWR(72 + PI_BASE_ADDR,DENALI_PI_72_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_73_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(73 + PI_BASE_ADDR,DENALI_PI_73_DATA,0);
       #else
         REGWR(73 + PI_BASE_ADDR,DENALI_PI_73_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_74_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(74 + PI_BASE_ADDR,DENALI_PI_74_DATA,0);
       #else
         REGWR(74 + PI_BASE_ADDR,DENALI_PI_74_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_75_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(75 + PI_BASE_ADDR,DENALI_PI_75_DATA,0);
       #else
         REGWR(75 + PI_BASE_ADDR,DENALI_PI_75_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_76_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(76 + PI_BASE_ADDR,DENALI_PI_76_DATA,0);
       #else
         REGWR(76 + PI_BASE_ADDR,DENALI_PI_76_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_77_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(77 + PI_BASE_ADDR,DENALI_PI_77_DATA,0);
       #else
         REGWR(77 + PI_BASE_ADDR,DENALI_PI_77_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_78_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(78 + PI_BASE_ADDR,DENALI_PI_78_DATA,0);
       #else
         REGWR(78 + PI_BASE_ADDR,DENALI_PI_78_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_79_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(79 + PI_BASE_ADDR,DENALI_PI_79_DATA,0);
       #else
         REGWR(79 + PI_BASE_ADDR,DENALI_PI_79_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_80_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(80 + PI_BASE_ADDR,DENALI_PI_80_DATA,0);
       #else
         REGWR(80 + PI_BASE_ADDR,DENALI_PI_80_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_81_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(81 + PI_BASE_ADDR,DENALI_PI_81_DATA,0);
       #else
         REGWR(81 + PI_BASE_ADDR,DENALI_PI_81_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_82_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(82 + PI_BASE_ADDR,DENALI_PI_82_DATA,0);
       #else
         REGWR(82 + PI_BASE_ADDR,DENALI_PI_82_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_83_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(83 + PI_BASE_ADDR,DENALI_PI_83_DATA,0);
       #else
         REGWR(83 + PI_BASE_ADDR,DENALI_PI_83_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_84_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(84 + PI_BASE_ADDR,DENALI_PI_84_DATA,0);
       #else
         REGWR(84 + PI_BASE_ADDR,DENALI_PI_84_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_85_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(85 + PI_BASE_ADDR,DENALI_PI_85_DATA,0);
       #else
         REGWR(85 + PI_BASE_ADDR,DENALI_PI_85_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_86_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(86 + PI_BASE_ADDR,DENALI_PI_86_DATA,0);
       #else
         REGWR(86 + PI_BASE_ADDR,DENALI_PI_86_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_87_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(87 + PI_BASE_ADDR,DENALI_PI_87_DATA,0);
       #else
         REGWR(87 + PI_BASE_ADDR,DENALI_PI_87_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_88_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(88 + PI_BASE_ADDR,DENALI_PI_88_DATA,0);
       #else
         REGWR(88 + PI_BASE_ADDR,DENALI_PI_88_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_89_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(89 + PI_BASE_ADDR,DENALI_PI_89_DATA,0);
       #else
         REGWR(89 + PI_BASE_ADDR,DENALI_PI_89_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_90_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(90 + PI_BASE_ADDR,DENALI_PI_90_DATA,0);
       #else
         REGWR(90 + PI_BASE_ADDR,DENALI_PI_90_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_91_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(91 + PI_BASE_ADDR,DENALI_PI_91_DATA,0);
       #else
         REGWR(91 + PI_BASE_ADDR,DENALI_PI_91_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_92_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(92 + PI_BASE_ADDR,DENALI_PI_92_DATA,0);
       #else
         REGWR(92 + PI_BASE_ADDR,DENALI_PI_92_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_93_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(93 + PI_BASE_ADDR,DENALI_PI_93_DATA,0);
       #else
         REGWR(93 + PI_BASE_ADDR,DENALI_PI_93_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_94_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(94 + PI_BASE_ADDR,DENALI_PI_94_DATA,0);
       #else
         REGWR(94 + PI_BASE_ADDR,DENALI_PI_94_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_95_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(95 + PI_BASE_ADDR,DENALI_PI_95_DATA,0);
       #else
         REGWR(95 + PI_BASE_ADDR,DENALI_PI_95_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_96_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(96 + PI_BASE_ADDR,DENALI_PI_96_DATA,0);
       #else
         REGWR(96 + PI_BASE_ADDR,DENALI_PI_96_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_97_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(97 + PI_BASE_ADDR,DENALI_PI_97_DATA,0);
       #else
         REGWR(97 + PI_BASE_ADDR,DENALI_PI_97_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_98_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(98 + PI_BASE_ADDR,DENALI_PI_98_DATA,0);
       #else
         REGWR(98 + PI_BASE_ADDR,DENALI_PI_98_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_99_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(99 + PI_BASE_ADDR,DENALI_PI_99_DATA,0);
       #else
         REGWR(99 + PI_BASE_ADDR,DENALI_PI_99_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_100_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(100 + PI_BASE_ADDR,DENALI_PI_100_DATA,0);
       #else
         REGWR(100 + PI_BASE_ADDR,DENALI_PI_100_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_101_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(101 + PI_BASE_ADDR,DENALI_PI_101_DATA,0);
       #else
         REGWR(101 + PI_BASE_ADDR,DENALI_PI_101_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_102_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(102 + PI_BASE_ADDR,DENALI_PI_102_DATA,0);
       #else
         REGWR(102 + PI_BASE_ADDR,DENALI_PI_102_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_103_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(103 + PI_BASE_ADDR,DENALI_PI_103_DATA,0);
       #else
         REGWR(103 + PI_BASE_ADDR,DENALI_PI_103_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_104_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(104 + PI_BASE_ADDR,DENALI_PI_104_DATA,0);
       #else
         REGWR(104 + PI_BASE_ADDR,DENALI_PI_104_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_105_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(105 + PI_BASE_ADDR,DENALI_PI_105_DATA,0);
       #else
         REGWR(105 + PI_BASE_ADDR,DENALI_PI_105_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_106_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(106 + PI_BASE_ADDR,DENALI_PI_106_DATA,0);
       #else
         REGWR(106 + PI_BASE_ADDR,DENALI_PI_106_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_107_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(107 + PI_BASE_ADDR,DENALI_PI_107_DATA,0);
       #else
         REGWR(107 + PI_BASE_ADDR,DENALI_PI_107_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_108_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(108 + PI_BASE_ADDR,DENALI_PI_108_DATA,0);
       #else
         REGWR(108 + PI_BASE_ADDR,DENALI_PI_108_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_109_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(109 + PI_BASE_ADDR,DENALI_PI_109_DATA,0);
       #else
         REGWR(109 + PI_BASE_ADDR,DENALI_PI_109_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_110_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(110 + PI_BASE_ADDR,DENALI_PI_110_DATA,0);
       #else
         REGWR(110 + PI_BASE_ADDR,DENALI_PI_110_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_111_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(111 + PI_BASE_ADDR,DENALI_PI_111_DATA,0);
       #else
         REGWR(111 + PI_BASE_ADDR,DENALI_PI_111_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_112_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(112 + PI_BASE_ADDR,DENALI_PI_112_DATA,0);
       #else
         REGWR(112 + PI_BASE_ADDR,DENALI_PI_112_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_113_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(113 + PI_BASE_ADDR,DENALI_PI_113_DATA,0);
       #else
         REGWR(113 + PI_BASE_ADDR,DENALI_PI_113_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_114_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(114 + PI_BASE_ADDR,DENALI_PI_114_DATA,0);
       #else
         REGWR(114 + PI_BASE_ADDR,DENALI_PI_114_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_115_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(115 + PI_BASE_ADDR,DENALI_PI_115_DATA,0);
       #else
         REGWR(115 + PI_BASE_ADDR,DENALI_PI_115_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_116_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(116 + PI_BASE_ADDR,DENALI_PI_116_DATA,0);
       #else
         REGWR(116 + PI_BASE_ADDR,DENALI_PI_116_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_117_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(117 + PI_BASE_ADDR,DENALI_PI_117_DATA,0);
       #else
         REGWR(117 + PI_BASE_ADDR,DENALI_PI_117_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_118_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(118 + PI_BASE_ADDR,DENALI_PI_118_DATA,0);
       #else
         REGWR(118 + PI_BASE_ADDR,DENALI_PI_118_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_119_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(119 + PI_BASE_ADDR,DENALI_PI_119_DATA,0);
       #else
         REGWR(119 + PI_BASE_ADDR,DENALI_PI_119_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_120_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(120 + PI_BASE_ADDR,DENALI_PI_120_DATA,0);
       #else
         REGWR(120 + PI_BASE_ADDR,DENALI_PI_120_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_121_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(121 + PI_BASE_ADDR,DENALI_PI_121_DATA,0);
       #else
         REGWR(121 + PI_BASE_ADDR,DENALI_PI_121_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_122_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(122 + PI_BASE_ADDR,DENALI_PI_122_DATA,0);
       #else
         REGWR(122 + PI_BASE_ADDR,DENALI_PI_122_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_123_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(123 + PI_BASE_ADDR,DENALI_PI_123_DATA,0);
       #else
         REGWR(123 + PI_BASE_ADDR,DENALI_PI_123_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_124_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(124 + PI_BASE_ADDR,DENALI_PI_124_DATA,0);
       #else
         REGWR(124 + PI_BASE_ADDR,DENALI_PI_124_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_125_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(125 + PI_BASE_ADDR,DENALI_PI_125_DATA,0);
       #else
         REGWR(125 + PI_BASE_ADDR,DENALI_PI_125_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_126_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(126 + PI_BASE_ADDR,DENALI_PI_126_DATA,0);
       #else
         REGWR(126 + PI_BASE_ADDR,DENALI_PI_126_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_127_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(127 + PI_BASE_ADDR,DENALI_PI_127_DATA,0);
       #else
         REGWR(127 + PI_BASE_ADDR,DENALI_PI_127_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_128_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(128 + PI_BASE_ADDR,DENALI_PI_128_DATA,0);
       #else
         REGWR(128 + PI_BASE_ADDR,DENALI_PI_128_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_129_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(129 + PI_BASE_ADDR,DENALI_PI_129_DATA,0);
       #else
         REGWR(129 + PI_BASE_ADDR,DENALI_PI_129_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_130_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(130 + PI_BASE_ADDR,DENALI_PI_130_DATA,0);
       #else
         REGWR(130 + PI_BASE_ADDR,DENALI_PI_130_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_131_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(131 + PI_BASE_ADDR,DENALI_PI_131_DATA,0);
       #else
         REGWR(131 + PI_BASE_ADDR,DENALI_PI_131_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_132_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(132 + PI_BASE_ADDR,DENALI_PI_132_DATA,0);
       #else
         REGWR(132 + PI_BASE_ADDR,DENALI_PI_132_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_133_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(133 + PI_BASE_ADDR,DENALI_PI_133_DATA,0);
       #else
         REGWR(133 + PI_BASE_ADDR,DENALI_PI_133_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_134_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(134 + PI_BASE_ADDR,DENALI_PI_134_DATA,0);
       #else
         REGWR(134 + PI_BASE_ADDR,DENALI_PI_134_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_135_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(135 + PI_BASE_ADDR,DENALI_PI_135_DATA,0);
       #else
         REGWR(135 + PI_BASE_ADDR,DENALI_PI_135_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_136_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(136 + PI_BASE_ADDR,DENALI_PI_136_DATA,0);
       #else
         REGWR(136 + PI_BASE_ADDR,DENALI_PI_136_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_137_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(137 + PI_BASE_ADDR,DENALI_PI_137_DATA,0);
       #else
         REGWR(137 + PI_BASE_ADDR,DENALI_PI_137_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_138_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(138 + PI_BASE_ADDR,DENALI_PI_138_DATA,0);
       #else
         REGWR(138 + PI_BASE_ADDR,DENALI_PI_138_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_139_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(139 + PI_BASE_ADDR,DENALI_PI_139_DATA,0);
       #else
         REGWR(139 + PI_BASE_ADDR,DENALI_PI_139_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_140_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(140 + PI_BASE_ADDR,DENALI_PI_140_DATA,0);
       #else
         REGWR(140 + PI_BASE_ADDR,DENALI_PI_140_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_141_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(141 + PI_BASE_ADDR,DENALI_PI_141_DATA,0);
       #else
         REGWR(141 + PI_BASE_ADDR,DENALI_PI_141_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_142_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(142 + PI_BASE_ADDR,DENALI_PI_142_DATA,0);
       #else
         REGWR(142 + PI_BASE_ADDR,DENALI_PI_142_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_143_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(143 + PI_BASE_ADDR,DENALI_PI_143_DATA,0);
       #else
         REGWR(143 + PI_BASE_ADDR,DENALI_PI_143_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_144_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(144 + PI_BASE_ADDR,DENALI_PI_144_DATA,0);
       #else
         REGWR(144 + PI_BASE_ADDR,DENALI_PI_144_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_145_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(145 + PI_BASE_ADDR,DENALI_PI_145_DATA,0);
       #else
         REGWR(145 + PI_BASE_ADDR,DENALI_PI_145_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_146_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(146 + PI_BASE_ADDR,DENALI_PI_146_DATA,0);
       #else
         REGWR(146 + PI_BASE_ADDR,DENALI_PI_146_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_147_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(147 + PI_BASE_ADDR,DENALI_PI_147_DATA,0);
       #else
         REGWR(147 + PI_BASE_ADDR,DENALI_PI_147_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_148_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(148 + PI_BASE_ADDR,DENALI_PI_148_DATA,0);
       #else
         REGWR(148 + PI_BASE_ADDR,DENALI_PI_148_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_149_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(149 + PI_BASE_ADDR,DENALI_PI_149_DATA,0);
       #else
         REGWR(149 + PI_BASE_ADDR,DENALI_PI_149_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_150_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(150 + PI_BASE_ADDR,DENALI_PI_150_DATA,0);
       #else
         REGWR(150 + PI_BASE_ADDR,DENALI_PI_150_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_151_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(151 + PI_BASE_ADDR,DENALI_PI_151_DATA,0);
       #else
         REGWR(151 + PI_BASE_ADDR,DENALI_PI_151_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_152_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(152 + PI_BASE_ADDR,DENALI_PI_152_DATA,0);
       #else
         REGWR(152 + PI_BASE_ADDR,DENALI_PI_152_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_153_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(153 + PI_BASE_ADDR,DENALI_PI_153_DATA,0);
       #else
         REGWR(153 + PI_BASE_ADDR,DENALI_PI_153_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_154_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(154 + PI_BASE_ADDR,DENALI_PI_154_DATA,0);
       #else
         REGWR(154 + PI_BASE_ADDR,DENALI_PI_154_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_155_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(155 + PI_BASE_ADDR,DENALI_PI_155_DATA,0);
       #else
         REGWR(155 + PI_BASE_ADDR,DENALI_PI_155_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_156_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(156 + PI_BASE_ADDR,DENALI_PI_156_DATA,0);
       #else
         REGWR(156 + PI_BASE_ADDR,DENALI_PI_156_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_157_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(157 + PI_BASE_ADDR,DENALI_PI_157_DATA,0);
       #else
         REGWR(157 + PI_BASE_ADDR,DENALI_PI_157_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_158_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(158 + PI_BASE_ADDR,DENALI_PI_158_DATA,0);
       #else
         REGWR(158 + PI_BASE_ADDR,DENALI_PI_158_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_159_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(159 + PI_BASE_ADDR,DENALI_PI_159_DATA,0);
       #else
         REGWR(159 + PI_BASE_ADDR,DENALI_PI_159_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_160_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(160 + PI_BASE_ADDR,DENALI_PI_160_DATA,0);
       #else
         REGWR(160 + PI_BASE_ADDR,DENALI_PI_160_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_161_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(161 + PI_BASE_ADDR,DENALI_PI_161_DATA,0);
       #else
         REGWR(161 + PI_BASE_ADDR,DENALI_PI_161_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_162_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(162 + PI_BASE_ADDR,DENALI_PI_162_DATA,0);
       #else
         REGWR(162 + PI_BASE_ADDR,DENALI_PI_162_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_163_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(163 + PI_BASE_ADDR,DENALI_PI_163_DATA,0);
       #else
         REGWR(163 + PI_BASE_ADDR,DENALI_PI_163_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_164_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(164 + PI_BASE_ADDR,DENALI_PI_164_DATA,0);
       #else
         REGWR(164 + PI_BASE_ADDR,DENALI_PI_164_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_165_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(165 + PI_BASE_ADDR,DENALI_PI_165_DATA,0);
       #else
         REGWR(165 + PI_BASE_ADDR,DENALI_PI_165_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_166_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(166 + PI_BASE_ADDR,DENALI_PI_166_DATA,0);
       #else
         REGWR(166 + PI_BASE_ADDR,DENALI_PI_166_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_167_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(167 + PI_BASE_ADDR,DENALI_PI_167_DATA,0);
       #else
         REGWR(167 + PI_BASE_ADDR,DENALI_PI_167_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_168_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(168 + PI_BASE_ADDR,DENALI_PI_168_DATA,0);
       #else
         REGWR(168 + PI_BASE_ADDR,DENALI_PI_168_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_169_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(169 + PI_BASE_ADDR,DENALI_PI_169_DATA,0);
       #else
         REGWR(169 + PI_BASE_ADDR,DENALI_PI_169_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_170_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(170 + PI_BASE_ADDR,DENALI_PI_170_DATA,0);
       #else
         REGWR(170 + PI_BASE_ADDR,DENALI_PI_170_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_171_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(171 + PI_BASE_ADDR,DENALI_PI_171_DATA,0);
       #else
         REGWR(171 + PI_BASE_ADDR,DENALI_PI_171_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_172_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(172 + PI_BASE_ADDR,DENALI_PI_172_DATA,0);
       #else
         REGWR(172 + PI_BASE_ADDR,DENALI_PI_172_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_173_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(173 + PI_BASE_ADDR,DENALI_PI_173_DATA,0);
       #else
         REGWR(173 + PI_BASE_ADDR,DENALI_PI_173_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_174_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(174 + PI_BASE_ADDR,DENALI_PI_174_DATA,0);
       #else
         REGWR(174 + PI_BASE_ADDR,DENALI_PI_174_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_175_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(175 + PI_BASE_ADDR,DENALI_PI_175_DATA,0);
       #else
         REGWR(175 + PI_BASE_ADDR,DENALI_PI_175_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_176_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(176 + PI_BASE_ADDR,DENALI_PI_176_DATA,0);
       #else
         REGWR(176 + PI_BASE_ADDR,DENALI_PI_176_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_177_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(177 + PI_BASE_ADDR,DENALI_PI_177_DATA,0);
       #else
         REGWR(177 + PI_BASE_ADDR,DENALI_PI_177_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_178_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(178 + PI_BASE_ADDR,DENALI_PI_178_DATA,0);
       #else
         REGWR(178 + PI_BASE_ADDR,DENALI_PI_178_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_179_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(179 + PI_BASE_ADDR,DENALI_PI_179_DATA,0);
       #else
         REGWR(179 + PI_BASE_ADDR,DENALI_PI_179_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_180_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(180 + PI_BASE_ADDR,DENALI_PI_180_DATA,0);
       #else
         REGWR(180 + PI_BASE_ADDR,DENALI_PI_180_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_181_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(181 + PI_BASE_ADDR,DENALI_PI_181_DATA,0);
       #else
         REGWR(181 + PI_BASE_ADDR,DENALI_PI_181_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_182_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(182 + PI_BASE_ADDR,DENALI_PI_182_DATA,0);
       #else
         REGWR(182 + PI_BASE_ADDR,DENALI_PI_182_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_183_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(183 + PI_BASE_ADDR,DENALI_PI_183_DATA,0);
       #else
         REGWR(183 + PI_BASE_ADDR,DENALI_PI_183_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_184_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(184 + PI_BASE_ADDR,DENALI_PI_184_DATA,0);
       #else
         REGWR(184 + PI_BASE_ADDR,DENALI_PI_184_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_185_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(185 + PI_BASE_ADDR,DENALI_PI_185_DATA,0);
       #else
         REGWR(185 + PI_BASE_ADDR,DENALI_PI_185_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_186_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(186 + PI_BASE_ADDR,DENALI_PI_186_DATA,0);
       #else
         REGWR(186 + PI_BASE_ADDR,DENALI_PI_186_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_187_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(187 + PI_BASE_ADDR,DENALI_PI_187_DATA,0);
       #else
         REGWR(187 + PI_BASE_ADDR,DENALI_PI_187_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_188_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(188 + PI_BASE_ADDR,DENALI_PI_188_DATA,0);
       #else
         REGWR(188 + PI_BASE_ADDR,DENALI_PI_188_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_189_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(189 + PI_BASE_ADDR,DENALI_PI_189_DATA,0);
       #else
         REGWR(189 + PI_BASE_ADDR,DENALI_PI_189_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_190_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(190 + PI_BASE_ADDR,DENALI_PI_190_DATA,0);
       #else
         REGWR(190 + PI_BASE_ADDR,DENALI_PI_190_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_191_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(191 + PI_BASE_ADDR,DENALI_PI_191_DATA,0);
       #else
         REGWR(191 + PI_BASE_ADDR,DENALI_PI_191_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_192_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(192 + PI_BASE_ADDR,DENALI_PI_192_DATA,0);
       #else
         REGWR(192 + PI_BASE_ADDR,DENALI_PI_192_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_193_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(193 + PI_BASE_ADDR,DENALI_PI_193_DATA,0);
       #else
         REGWR(193 + PI_BASE_ADDR,DENALI_PI_193_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_194_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(194 + PI_BASE_ADDR,DENALI_PI_194_DATA,0);
       #else
         REGWR(194 + PI_BASE_ADDR,DENALI_PI_194_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_195_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(195 + PI_BASE_ADDR,DENALI_PI_195_DATA,0);
       #else
         REGWR(195 + PI_BASE_ADDR,DENALI_PI_195_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_196_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(196 + PI_BASE_ADDR,DENALI_PI_196_DATA,0);
       #else
         REGWR(196 + PI_BASE_ADDR,DENALI_PI_196_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_197_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(197 + PI_BASE_ADDR,DENALI_PI_197_DATA,0);
       #else
         REGWR(197 + PI_BASE_ADDR,DENALI_PI_197_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_198_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(198 + PI_BASE_ADDR,DENALI_PI_198_DATA,0);
       #else
         REGWR(198 + PI_BASE_ADDR,DENALI_PI_198_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_199_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(199 + PI_BASE_ADDR,DENALI_PI_199_DATA,0);
       #else
         REGWR(199 + PI_BASE_ADDR,DENALI_PI_199_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_200_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(200 + PI_BASE_ADDR,DENALI_PI_200_DATA,0);
       #else
         REGWR(200 + PI_BASE_ADDR,DENALI_PI_200_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_201_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(201 + PI_BASE_ADDR,DENALI_PI_201_DATA,0);
       #else
         REGWR(201 + PI_BASE_ADDR,DENALI_PI_201_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_202_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(202 + PI_BASE_ADDR,DENALI_PI_202_DATA,0);
       #else
         REGWR(202 + PI_BASE_ADDR,DENALI_PI_202_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_203_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(203 + PI_BASE_ADDR,DENALI_PI_203_DATA,0);
       #else
         REGWR(203 + PI_BASE_ADDR,DENALI_PI_203_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_204_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(204 + PI_BASE_ADDR,DENALI_PI_204_DATA,0);
       #else
         REGWR(204 + PI_BASE_ADDR,DENALI_PI_204_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_205_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(205 + PI_BASE_ADDR,DENALI_PI_205_DATA,0);
       #else
         REGWR(205 + PI_BASE_ADDR,DENALI_PI_205_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_206_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(206 + PI_BASE_ADDR,DENALI_PI_206_DATA,0);
       #else
         REGWR(206 + PI_BASE_ADDR,DENALI_PI_206_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_207_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(207 + PI_BASE_ADDR,DENALI_PI_207_DATA,0);
       #else
         REGWR(207 + PI_BASE_ADDR,DENALI_PI_207_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_208_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(208 + PI_BASE_ADDR,DENALI_PI_208_DATA,0);
       #else
         REGWR(208 + PI_BASE_ADDR,DENALI_PI_208_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_209_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(209 + PI_BASE_ADDR,DENALI_PI_209_DATA,0);
       #else
         REGWR(209 + PI_BASE_ADDR,DENALI_PI_209_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_210_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(210 + PI_BASE_ADDR,DENALI_PI_210_DATA,0);
       #else
         REGWR(210 + PI_BASE_ADDR,DENALI_PI_210_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_211_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(211 + PI_BASE_ADDR,DENALI_PI_211_DATA,0);
       #else
         REGWR(211 + PI_BASE_ADDR,DENALI_PI_211_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_212_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(212 + PI_BASE_ADDR,DENALI_PI_212_DATA,0);
       #else
         REGWR(212 + PI_BASE_ADDR,DENALI_PI_212_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_213_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(213 + PI_BASE_ADDR,DENALI_PI_213_DATA,0);
       #else
         REGWR(213 + PI_BASE_ADDR,DENALI_PI_213_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_214_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(214 + PI_BASE_ADDR,DENALI_PI_214_DATA,0);
       #else
         REGWR(214 + PI_BASE_ADDR,DENALI_PI_214_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_215_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(215 + PI_BASE_ADDR,DENALI_PI_215_DATA,0);
       #else
         REGWR(215 + PI_BASE_ADDR,DENALI_PI_215_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_216_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(216 + PI_BASE_ADDR,DENALI_PI_216_DATA,0);
       #else
         REGWR(216 + PI_BASE_ADDR,DENALI_PI_216_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_217_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(217 + PI_BASE_ADDR,DENALI_PI_217_DATA,0);
       #else
         REGWR(217 + PI_BASE_ADDR,DENALI_PI_217_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_218_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(218 + PI_BASE_ADDR,DENALI_PI_218_DATA,0);
       #else
         REGWR(218 + PI_BASE_ADDR,DENALI_PI_218_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_219_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(219 + PI_BASE_ADDR,DENALI_PI_219_DATA,0);
       #else
         REGWR(219 + PI_BASE_ADDR,DENALI_PI_219_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_220_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(220 + PI_BASE_ADDR,DENALI_PI_220_DATA,0);
       #else
         REGWR(220 + PI_BASE_ADDR,DENALI_PI_220_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_221_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(221 + PI_BASE_ADDR,DENALI_PI_221_DATA,0);
       #else
         REGWR(221 + PI_BASE_ADDR,DENALI_PI_221_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_222_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(222 + PI_BASE_ADDR,DENALI_PI_222_DATA,0);
       #else
         REGWR(222 + PI_BASE_ADDR,DENALI_PI_222_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_223_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(223 + PI_BASE_ADDR,DENALI_PI_223_DATA,0);
       #else
         REGWR(223 + PI_BASE_ADDR,DENALI_PI_223_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_224_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(224 + PI_BASE_ADDR,DENALI_PI_224_DATA,0);
       #else
         REGWR(224 + PI_BASE_ADDR,DENALI_PI_224_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_225_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(225 + PI_BASE_ADDR,DENALI_PI_225_DATA,0);
       #else
         REGWR(225 + PI_BASE_ADDR,DENALI_PI_225_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_226_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(226 + PI_BASE_ADDR,DENALI_PI_226_DATA,0);
       #else
         REGWR(226 + PI_BASE_ADDR,DENALI_PI_226_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_227_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(227 + PI_BASE_ADDR,DENALI_PI_227_DATA,0);
       #else
         REGWR(227 + PI_BASE_ADDR,DENALI_PI_227_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_228_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(228 + PI_BASE_ADDR,DENALI_PI_228_DATA,0);
       #else
         REGWR(228 + PI_BASE_ADDR,DENALI_PI_228_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_229_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(229 + PI_BASE_ADDR,DENALI_PI_229_DATA,0);
       #else
         REGWR(229 + PI_BASE_ADDR,DENALI_PI_229_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_230_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(230 + PI_BASE_ADDR,DENALI_PI_230_DATA,0);
       #else
         REGWR(230 + PI_BASE_ADDR,DENALI_PI_230_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_231_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(231 + PI_BASE_ADDR,DENALI_PI_231_DATA,0);
       #else
         REGWR(231 + PI_BASE_ADDR,DENALI_PI_231_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_232_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(232 + PI_BASE_ADDR,DENALI_PI_232_DATA,0);
       #else
         REGWR(232 + PI_BASE_ADDR,DENALI_PI_232_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_233_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(233 + PI_BASE_ADDR,DENALI_PI_233_DATA,0);
       #else
         REGWR(233 + PI_BASE_ADDR,DENALI_PI_233_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_234_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(234 + PI_BASE_ADDR,DENALI_PI_234_DATA,0);
       #else
         REGWR(234 + PI_BASE_ADDR,DENALI_PI_234_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_235_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(235 + PI_BASE_ADDR,DENALI_PI_235_DATA,0);
       #else
         REGWR(235 + PI_BASE_ADDR,DENALI_PI_235_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_236_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(236 + PI_BASE_ADDR,DENALI_PI_236_DATA,0);
       #else
         REGWR(236 + PI_BASE_ADDR,DENALI_PI_236_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_237_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(237 + PI_BASE_ADDR,DENALI_PI_237_DATA,0);
       #else
         REGWR(237 + PI_BASE_ADDR,DENALI_PI_237_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_238_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(238 + PI_BASE_ADDR,DENALI_PI_238_DATA,0);
       #else
         REGWR(238 + PI_BASE_ADDR,DENALI_PI_238_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_239_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(239 + PI_BASE_ADDR,DENALI_PI_239_DATA,0);
       #else
         REGWR(239 + PI_BASE_ADDR,DENALI_PI_239_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_240_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(240 + PI_BASE_ADDR,DENALI_PI_240_DATA,0);
       #else
         REGWR(240 + PI_BASE_ADDR,DENALI_PI_240_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_241_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(241 + PI_BASE_ADDR,DENALI_PI_241_DATA,0);
       #else
         REGWR(241 + PI_BASE_ADDR,DENALI_PI_241_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_242_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(242 + PI_BASE_ADDR,DENALI_PI_242_DATA,0);
       #else
         REGWR(242 + PI_BASE_ADDR,DENALI_PI_242_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_243_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(243 + PI_BASE_ADDR,DENALI_PI_243_DATA,0);
       #else
         REGWR(243 + PI_BASE_ADDR,DENALI_PI_243_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_244_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(244 + PI_BASE_ADDR,DENALI_PI_244_DATA,0);
       #else
         REGWR(244 + PI_BASE_ADDR,DENALI_PI_244_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_245_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(245 + PI_BASE_ADDR,DENALI_PI_245_DATA,0);
       #else
         REGWR(245 + PI_BASE_ADDR,DENALI_PI_245_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_246_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(246 + PI_BASE_ADDR,DENALI_PI_246_DATA,0);
       #else
         REGWR(246 + PI_BASE_ADDR,DENALI_PI_246_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_247_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(247 + PI_BASE_ADDR,DENALI_PI_247_DATA,0);
       #else
         REGWR(247 + PI_BASE_ADDR,DENALI_PI_247_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_248_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(248 + PI_BASE_ADDR,DENALI_PI_248_DATA,0);
       #else
         REGWR(248 + PI_BASE_ADDR,DENALI_PI_248_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_249_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(249 + PI_BASE_ADDR,DENALI_PI_249_DATA,0);
       #else
         REGWR(249 + PI_BASE_ADDR,DENALI_PI_249_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_250_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(250 + PI_BASE_ADDR,DENALI_PI_250_DATA,0);
       #else
         REGWR(250 + PI_BASE_ADDR,DENALI_PI_250_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_251_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(251 + PI_BASE_ADDR,DENALI_PI_251_DATA,0);
       #else
         REGWR(251 + PI_BASE_ADDR,DENALI_PI_251_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_252_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(252 + PI_BASE_ADDR,DENALI_PI_252_DATA,0);
       #else
         REGWR(252 + PI_BASE_ADDR,DENALI_PI_252_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_253_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(253 + PI_BASE_ADDR,DENALI_PI_253_DATA,0);
       #else
         REGWR(253 + PI_BASE_ADDR,DENALI_PI_253_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_254_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(254 + PI_BASE_ADDR,DENALI_PI_254_DATA,0);
       #else
         REGWR(254 + PI_BASE_ADDR,DENALI_PI_254_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_255_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(255 + PI_BASE_ADDR,DENALI_PI_255_DATA,0);
       #else
         REGWR(255 + PI_BASE_ADDR,DENALI_PI_255_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_256_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(256 + PI_BASE_ADDR,DENALI_PI_256_DATA,0);
       #else
         REGWR(256 + PI_BASE_ADDR,DENALI_PI_256_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_257_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(257 + PI_BASE_ADDR,DENALI_PI_257_DATA,0);
       #else
         REGWR(257 + PI_BASE_ADDR,DENALI_PI_257_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_258_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(258 + PI_BASE_ADDR,DENALI_PI_258_DATA,0);
       #else
         REGWR(258 + PI_BASE_ADDR,DENALI_PI_258_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_259_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(259 + PI_BASE_ADDR,DENALI_PI_259_DATA,0);
       #else
         REGWR(259 + PI_BASE_ADDR,DENALI_PI_259_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_260_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(260 + PI_BASE_ADDR,DENALI_PI_260_DATA,0);
       #else
         REGWR(260 + PI_BASE_ADDR,DENALI_PI_260_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_261_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(261 + PI_BASE_ADDR,DENALI_PI_261_DATA,0);
       #else
         REGWR(261 + PI_BASE_ADDR,DENALI_PI_261_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_262_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(262 + PI_BASE_ADDR,DENALI_PI_262_DATA,0);
       #else
         REGWR(262 + PI_BASE_ADDR,DENALI_PI_262_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_263_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(263 + PI_BASE_ADDR,DENALI_PI_263_DATA,0);
       #else
         REGWR(263 + PI_BASE_ADDR,DENALI_PI_263_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_264_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(264 + PI_BASE_ADDR,DENALI_PI_264_DATA,0);
       #else
         REGWR(264 + PI_BASE_ADDR,DENALI_PI_264_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_265_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(265 + PI_BASE_ADDR,DENALI_PI_265_DATA,0);
       #else
         REGWR(265 + PI_BASE_ADDR,DENALI_PI_265_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_266_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(266 + PI_BASE_ADDR,DENALI_PI_266_DATA,0);
       #else
         REGWR(266 + PI_BASE_ADDR,DENALI_PI_266_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_267_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(267 + PI_BASE_ADDR,DENALI_PI_267_DATA,0);
       #else
         REGWR(267 + PI_BASE_ADDR,DENALI_PI_267_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_268_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(268 + PI_BASE_ADDR,DENALI_PI_268_DATA,0);
       #else
         REGWR(268 + PI_BASE_ADDR,DENALI_PI_268_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_269_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(269 + PI_BASE_ADDR,DENALI_PI_269_DATA,0);
       #else
         REGWR(269 + PI_BASE_ADDR,DENALI_PI_269_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_270_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(270 + PI_BASE_ADDR,DENALI_PI_270_DATA,0);
       #else
         REGWR(270 + PI_BASE_ADDR,DENALI_PI_270_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_271_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(271 + PI_BASE_ADDR,DENALI_PI_271_DATA,0);
       #else
         REGWR(271 + PI_BASE_ADDR,DENALI_PI_271_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_272_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(272 + PI_BASE_ADDR,DENALI_PI_272_DATA,0);
       #else
         REGWR(272 + PI_BASE_ADDR,DENALI_PI_272_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_273_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(273 + PI_BASE_ADDR,DENALI_PI_273_DATA,0);
       #else
         REGWR(273 + PI_BASE_ADDR,DENALI_PI_273_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_274_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(274 + PI_BASE_ADDR,DENALI_PI_274_DATA,0);
       #else
         REGWR(274 + PI_BASE_ADDR,DENALI_PI_274_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_275_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(275 + PI_BASE_ADDR,DENALI_PI_275_DATA,0);
       #else
         REGWR(275 + PI_BASE_ADDR,DENALI_PI_275_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_276_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(276 + PI_BASE_ADDR,DENALI_PI_276_DATA,0);
       #else
         REGWR(276 + PI_BASE_ADDR,DENALI_PI_276_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_277_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(277 + PI_BASE_ADDR,DENALI_PI_277_DATA,0);
       #else
         REGWR(277 + PI_BASE_ADDR,DENALI_PI_277_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_278_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(278 + PI_BASE_ADDR,DENALI_PI_278_DATA,0);
       #else
         REGWR(278 + PI_BASE_ADDR,DENALI_PI_278_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_279_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(279 + PI_BASE_ADDR,DENALI_PI_279_DATA,0);
       #else
         REGWR(279 + PI_BASE_ADDR,DENALI_PI_279_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_280_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(280 + PI_BASE_ADDR,DENALI_PI_280_DATA,0);
       #else
         REGWR(280 + PI_BASE_ADDR,DENALI_PI_280_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_281_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(281 + PI_BASE_ADDR,DENALI_PI_281_DATA,0);
       #else
         REGWR(281 + PI_BASE_ADDR,DENALI_PI_281_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_282_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(282 + PI_BASE_ADDR,DENALI_PI_282_DATA,0);
       #else
         REGWR(282 + PI_BASE_ADDR,DENALI_PI_282_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_283_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(283 + PI_BASE_ADDR,DENALI_PI_283_DATA,0);
       #else
         REGWR(283 + PI_BASE_ADDR,DENALI_PI_283_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_284_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(284 + PI_BASE_ADDR,DENALI_PI_284_DATA,0);
       #else
         REGWR(284 + PI_BASE_ADDR,DENALI_PI_284_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_285_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(285 + PI_BASE_ADDR,DENALI_PI_285_DATA,0);
       #else
         REGWR(285 + PI_BASE_ADDR,DENALI_PI_285_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_286_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(286 + PI_BASE_ADDR,DENALI_PI_286_DATA,0);
       #else
         REGWR(286 + PI_BASE_ADDR,DENALI_PI_286_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_287_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(287 + PI_BASE_ADDR,DENALI_PI_287_DATA,0);
       #else
         REGWR(287 + PI_BASE_ADDR,DENALI_PI_287_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_288_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(288 + PI_BASE_ADDR,DENALI_PI_288_DATA,0);
       #else
         REGWR(288 + PI_BASE_ADDR,DENALI_PI_288_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_289_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(289 + PI_BASE_ADDR,DENALI_PI_289_DATA,0);
       #else
         REGWR(289 + PI_BASE_ADDR,DENALI_PI_289_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_290_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(290 + PI_BASE_ADDR,DENALI_PI_290_DATA,0);
       #else
         REGWR(290 + PI_BASE_ADDR,DENALI_PI_290_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_291_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(291 + PI_BASE_ADDR,DENALI_PI_291_DATA,0);
       #else
         REGWR(291 + PI_BASE_ADDR,DENALI_PI_291_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_292_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(292 + PI_BASE_ADDR,DENALI_PI_292_DATA,0);
       #else
         REGWR(292 + PI_BASE_ADDR,DENALI_PI_292_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_293_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(293 + PI_BASE_ADDR,DENALI_PI_293_DATA,0);
       #else
         REGWR(293 + PI_BASE_ADDR,DENALI_PI_293_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_294_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(294 + PI_BASE_ADDR,DENALI_PI_294_DATA,0);
       #else
         REGWR(294 + PI_BASE_ADDR,DENALI_PI_294_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_295_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(295 + PI_BASE_ADDR,DENALI_PI_295_DATA,0);
       #else
         REGWR(295 + PI_BASE_ADDR,DENALI_PI_295_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_296_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(296 + PI_BASE_ADDR,DENALI_PI_296_DATA,0);
       #else
         REGWR(296 + PI_BASE_ADDR,DENALI_PI_296_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_297_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(297 + PI_BASE_ADDR,DENALI_PI_297_DATA,0);
       #else
         REGWR(297 + PI_BASE_ADDR,DENALI_PI_297_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_298_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(298 + PI_BASE_ADDR,DENALI_PI_298_DATA,0);
       #else
         REGWR(298 + PI_BASE_ADDR,DENALI_PI_298_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_299_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(299 + PI_BASE_ADDR,DENALI_PI_299_DATA,0);
       #else
         REGWR(299 + PI_BASE_ADDR,DENALI_PI_299_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_300_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(300 + PI_BASE_ADDR,DENALI_PI_300_DATA,0);
       #else
         REGWR(300 + PI_BASE_ADDR,DENALI_PI_300_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_301_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(301 + PI_BASE_ADDR,DENALI_PI_301_DATA,0);
       #else
         REGWR(301 + PI_BASE_ADDR,DENALI_PI_301_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_302_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(302 + PI_BASE_ADDR,DENALI_PI_302_DATA,0);
       #else
         REGWR(302 + PI_BASE_ADDR,DENALI_PI_302_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_303_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(303 + PI_BASE_ADDR,DENALI_PI_303_DATA,0);
       #else
         REGWR(303 + PI_BASE_ADDR,DENALI_PI_303_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_304_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(304 + PI_BASE_ADDR,DENALI_PI_304_DATA,0);
       #else
         REGWR(304 + PI_BASE_ADDR,DENALI_PI_304_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_305_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(305 + PI_BASE_ADDR,DENALI_PI_305_DATA,0);
       #else
         REGWR(305 + PI_BASE_ADDR,DENALI_PI_305_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_306_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(306 + PI_BASE_ADDR,DENALI_PI_306_DATA,0);
       #else
         REGWR(306 + PI_BASE_ADDR,DENALI_PI_306_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_307_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(307 + PI_BASE_ADDR,DENALI_PI_307_DATA,0);
       #else
         REGWR(307 + PI_BASE_ADDR,DENALI_PI_307_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_308_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(308 + PI_BASE_ADDR,DENALI_PI_308_DATA,0);
       #else
         REGWR(308 + PI_BASE_ADDR,DENALI_PI_308_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_309_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(309 + PI_BASE_ADDR,DENALI_PI_309_DATA,0);
       #else
         REGWR(309 + PI_BASE_ADDR,DENALI_PI_309_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_310_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(310 + PI_BASE_ADDR,DENALI_PI_310_DATA,0);
       #else
         REGWR(310 + PI_BASE_ADDR,DENALI_PI_310_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_311_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(311 + PI_BASE_ADDR,DENALI_PI_311_DATA,0);
       #else
         REGWR(311 + PI_BASE_ADDR,DENALI_PI_311_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_312_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(312 + PI_BASE_ADDR,DENALI_PI_312_DATA,0);
       #else
         REGWR(312 + PI_BASE_ADDR,DENALI_PI_312_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_313_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(313 + PI_BASE_ADDR,DENALI_PI_313_DATA,0);
       #else
         REGWR(313 + PI_BASE_ADDR,DENALI_PI_313_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_314_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(314 + PI_BASE_ADDR,DENALI_PI_314_DATA,0);
       #else
         REGWR(314 + PI_BASE_ADDR,DENALI_PI_314_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_315_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(315 + PI_BASE_ADDR,DENALI_PI_315_DATA,0);
       #else
         REGWR(315 + PI_BASE_ADDR,DENALI_PI_315_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_316_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(316 + PI_BASE_ADDR,DENALI_PI_316_DATA,0);
       #else
         REGWR(316 + PI_BASE_ADDR,DENALI_PI_316_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_317_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(317 + PI_BASE_ADDR,DENALI_PI_317_DATA,0);
       #else
         REGWR(317 + PI_BASE_ADDR,DENALI_PI_317_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_318_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(318 + PI_BASE_ADDR,DENALI_PI_318_DATA,0);
       #else
         REGWR(318 + PI_BASE_ADDR,DENALI_PI_318_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_319_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(319 + PI_BASE_ADDR,DENALI_PI_319_DATA,0);
       #else
         REGWR(319 + PI_BASE_ADDR,DENALI_PI_319_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_320_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(320 + PI_BASE_ADDR,DENALI_PI_320_DATA,0);
       #else
         REGWR(320 + PI_BASE_ADDR,DENALI_PI_320_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_321_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(321 + PI_BASE_ADDR,DENALI_PI_321_DATA,0);
       #else
         REGWR(321 + PI_BASE_ADDR,DENALI_PI_321_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_322_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(322 + PI_BASE_ADDR,DENALI_PI_322_DATA,0);
       #else
         REGWR(322 + PI_BASE_ADDR,DENALI_PI_322_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_323_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(323 + PI_BASE_ADDR,DENALI_PI_323_DATA,0);
       #else
         REGWR(323 + PI_BASE_ADDR,DENALI_PI_323_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_324_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(324 + PI_BASE_ADDR,DENALI_PI_324_DATA,0);
       #else
         REGWR(324 + PI_BASE_ADDR,DENALI_PI_324_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_325_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(325 + PI_BASE_ADDR,DENALI_PI_325_DATA,0);
       #else
         REGWR(325 + PI_BASE_ADDR,DENALI_PI_325_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_326_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(326 + PI_BASE_ADDR,DENALI_PI_326_DATA,0);
       #else
         REGWR(326 + PI_BASE_ADDR,DENALI_PI_326_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_327_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(327 + PI_BASE_ADDR,DENALI_PI_327_DATA,0);
       #else
         REGWR(327 + PI_BASE_ADDR,DENALI_PI_327_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_328_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(328 + PI_BASE_ADDR,DENALI_PI_328_DATA,0);
       #else
         REGWR(328 + PI_BASE_ADDR,DENALI_PI_328_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_329_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(329 + PI_BASE_ADDR,DENALI_PI_329_DATA,0);
       #else
         REGWR(329 + PI_BASE_ADDR,DENALI_PI_329_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_330_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(330 + PI_BASE_ADDR,DENALI_PI_330_DATA,0);
       #else
         REGWR(330 + PI_BASE_ADDR,DENALI_PI_330_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_331_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(331 + PI_BASE_ADDR,DENALI_PI_331_DATA,0);
       #else
         REGWR(331 + PI_BASE_ADDR,DENALI_PI_331_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_332_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(332 + PI_BASE_ADDR,DENALI_PI_332_DATA,0);
       #else
         REGWR(332 + PI_BASE_ADDR,DENALI_PI_332_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_333_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(333 + PI_BASE_ADDR,DENALI_PI_333_DATA,0);
       #else
         REGWR(333 + PI_BASE_ADDR,DENALI_PI_333_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_334_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(334 + PI_BASE_ADDR,DENALI_PI_334_DATA,0);
       #else
         REGWR(334 + PI_BASE_ADDR,DENALI_PI_334_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_335_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(335 + PI_BASE_ADDR,DENALI_PI_335_DATA,0);
       #else
         REGWR(335 + PI_BASE_ADDR,DENALI_PI_335_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_336_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(336 + PI_BASE_ADDR,DENALI_PI_336_DATA,0);
       #else
         REGWR(336 + PI_BASE_ADDR,DENALI_PI_336_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_337_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(337 + PI_BASE_ADDR,DENALI_PI_337_DATA,0);
       #else
         REGWR(337 + PI_BASE_ADDR,DENALI_PI_337_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_338_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(338 + PI_BASE_ADDR,DENALI_PI_338_DATA,0);
       #else
         REGWR(338 + PI_BASE_ADDR,DENALI_PI_338_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_339_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(339 + PI_BASE_ADDR,DENALI_PI_339_DATA,0);
       #else
         REGWR(339 + PI_BASE_ADDR,DENALI_PI_339_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_340_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(340 + PI_BASE_ADDR,DENALI_PI_340_DATA,0);
       #else
         REGWR(340 + PI_BASE_ADDR,DENALI_PI_340_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_341_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(341 + PI_BASE_ADDR,DENALI_PI_341_DATA,0);
       #else
         REGWR(341 + PI_BASE_ADDR,DENALI_PI_341_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_342_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(342 + PI_BASE_ADDR,DENALI_PI_342_DATA,0);
       #else
         REGWR(342 + PI_BASE_ADDR,DENALI_PI_342_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_343_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(343 + PI_BASE_ADDR,DENALI_PI_343_DATA,0);
       #else
         REGWR(343 + PI_BASE_ADDR,DENALI_PI_343_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_344_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(344 + PI_BASE_ADDR,DENALI_PI_344_DATA,0);
       #else
         REGWR(344 + PI_BASE_ADDR,DENALI_PI_344_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_345_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(345 + PI_BASE_ADDR,DENALI_PI_345_DATA,0);
       #else
         REGWR(345 + PI_BASE_ADDR,DENALI_PI_345_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_346_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(346 + PI_BASE_ADDR,DENALI_PI_346_DATA,0);
       #else
         REGWR(346 + PI_BASE_ADDR,DENALI_PI_346_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_347_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(347 + PI_BASE_ADDR,DENALI_PI_347_DATA,0);
       #else
         REGWR(347 + PI_BASE_ADDR,DENALI_PI_347_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_348_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(348 + PI_BASE_ADDR,DENALI_PI_348_DATA,0);
       #else
         REGWR(348 + PI_BASE_ADDR,DENALI_PI_348_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_349_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(349 + PI_BASE_ADDR,DENALI_PI_349_DATA,0);
       #else
         REGWR(349 + PI_BASE_ADDR,DENALI_PI_349_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_350_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(350 + PI_BASE_ADDR,DENALI_PI_350_DATA,0);
       #else
         REGWR(350 + PI_BASE_ADDR,DENALI_PI_350_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_351_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(351 + PI_BASE_ADDR,DENALI_PI_351_DATA,0);
       #else
         REGWR(351 + PI_BASE_ADDR,DENALI_PI_351_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_352_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(352 + PI_BASE_ADDR,DENALI_PI_352_DATA,0);
       #else
         REGWR(352 + PI_BASE_ADDR,DENALI_PI_352_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_353_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(353 + PI_BASE_ADDR,DENALI_PI_353_DATA,0);
       #else
         REGWR(353 + PI_BASE_ADDR,DENALI_PI_353_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_354_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(354 + PI_BASE_ADDR,DENALI_PI_354_DATA,0);
       #else
         REGWR(354 + PI_BASE_ADDR,DENALI_PI_354_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_355_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(355 + PI_BASE_ADDR,DENALI_PI_355_DATA,0);
       #else
         REGWR(355 + PI_BASE_ADDR,DENALI_PI_355_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_356_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(356 + PI_BASE_ADDR,DENALI_PI_356_DATA,0);
       #else
         REGWR(356 + PI_BASE_ADDR,DENALI_PI_356_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_357_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(357 + PI_BASE_ADDR,DENALI_PI_357_DATA,0);
       #else
         REGWR(357 + PI_BASE_ADDR,DENALI_PI_357_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_358_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(358 + PI_BASE_ADDR,DENALI_PI_358_DATA,0);
       #else
         REGWR(358 + PI_BASE_ADDR,DENALI_PI_358_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_359_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(359 + PI_BASE_ADDR,DENALI_PI_359_DATA,0);
       #else
         REGWR(359 + PI_BASE_ADDR,DENALI_PI_359_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_360_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(360 + PI_BASE_ADDR,DENALI_PI_360_DATA,0);
       #else
         REGWR(360 + PI_BASE_ADDR,DENALI_PI_360_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_361_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(361 + PI_BASE_ADDR,DENALI_PI_361_DATA,0);
       #else
         REGWR(361 + PI_BASE_ADDR,DENALI_PI_361_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_362_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(362 + PI_BASE_ADDR,DENALI_PI_362_DATA,0);
       #else
         REGWR(362 + PI_BASE_ADDR,DENALI_PI_362_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_363_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(363 + PI_BASE_ADDR,DENALI_PI_363_DATA,0);
       #else
         REGWR(363 + PI_BASE_ADDR,DENALI_PI_363_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_364_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(364 + PI_BASE_ADDR,DENALI_PI_364_DATA,0);
       #else
         REGWR(364 + PI_BASE_ADDR,DENALI_PI_364_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_365_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(365 + PI_BASE_ADDR,DENALI_PI_365_DATA,0);
       #else
         REGWR(365 + PI_BASE_ADDR,DENALI_PI_365_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_366_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(366 + PI_BASE_ADDR,DENALI_PI_366_DATA,0);
       #else
         REGWR(366 + PI_BASE_ADDR,DENALI_PI_366_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_367_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(367 + PI_BASE_ADDR,DENALI_PI_367_DATA,0);
       #else
         REGWR(367 + PI_BASE_ADDR,DENALI_PI_367_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_368_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(368 + PI_BASE_ADDR,DENALI_PI_368_DATA,0);
       #else
         REGWR(368 + PI_BASE_ADDR,DENALI_PI_368_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_369_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(369 + PI_BASE_ADDR,DENALI_PI_369_DATA,0);
       #else
         REGWR(369 + PI_BASE_ADDR,DENALI_PI_369_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_370_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(370 + PI_BASE_ADDR,DENALI_PI_370_DATA,0);
       #else
         REGWR(370 + PI_BASE_ADDR,DENALI_PI_370_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_371_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(371 + PI_BASE_ADDR,DENALI_PI_371_DATA,0);
       #else
         REGWR(371 + PI_BASE_ADDR,DENALI_PI_371_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_372_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(372 + PI_BASE_ADDR,DENALI_PI_372_DATA,0);
       #else
         REGWR(372 + PI_BASE_ADDR,DENALI_PI_372_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_373_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(373 + PI_BASE_ADDR,DENALI_PI_373_DATA,0);
       #else
         REGWR(373 + PI_BASE_ADDR,DENALI_PI_373_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_374_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(374 + PI_BASE_ADDR,DENALI_PI_374_DATA,0);
       #else
         REGWR(374 + PI_BASE_ADDR,DENALI_PI_374_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_375_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(375 + PI_BASE_ADDR,DENALI_PI_375_DATA,0);
       #else
         REGWR(375 + PI_BASE_ADDR,DENALI_PI_375_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_376_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(376 + PI_BASE_ADDR,DENALI_PI_376_DATA,0);
       #else
         REGWR(376 + PI_BASE_ADDR,DENALI_PI_376_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_377_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(377 + PI_BASE_ADDR,DENALI_PI_377_DATA,0);
       #else
         REGWR(377 + PI_BASE_ADDR,DENALI_PI_377_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_378_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(378 + PI_BASE_ADDR,DENALI_PI_378_DATA,0);
       #else
         REGWR(378 + PI_BASE_ADDR,DENALI_PI_378_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_379_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(379 + PI_BASE_ADDR,DENALI_PI_379_DATA,0);
       #else
         REGWR(379 + PI_BASE_ADDR,DENALI_PI_379_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_380_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(380 + PI_BASE_ADDR,DENALI_PI_380_DATA,0);
       #else
         REGWR(380 + PI_BASE_ADDR,DENALI_PI_380_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_381_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(381 + PI_BASE_ADDR,DENALI_PI_381_DATA,0);
       #else
         REGWR(381 + PI_BASE_ADDR,DENALI_PI_381_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_382_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(382 + PI_BASE_ADDR,DENALI_PI_382_DATA,0);
       #else
         REGWR(382 + PI_BASE_ADDR,DENALI_PI_382_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_383_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(383 + PI_BASE_ADDR,DENALI_PI_383_DATA,0);
       #else
         REGWR(383 + PI_BASE_ADDR,DENALI_PI_383_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_384_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(384 + PI_BASE_ADDR,DENALI_PI_384_DATA,0);
       #else
         REGWR(384 + PI_BASE_ADDR,DENALI_PI_384_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_385_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(385 + PI_BASE_ADDR,DENALI_PI_385_DATA,0);
       #else
         REGWR(385 + PI_BASE_ADDR,DENALI_PI_385_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_386_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(386 + PI_BASE_ADDR,DENALI_PI_386_DATA,0);
       #else
         REGWR(386 + PI_BASE_ADDR,DENALI_PI_386_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_387_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(387 + PI_BASE_ADDR,DENALI_PI_387_DATA,0);
       #else
         REGWR(387 + PI_BASE_ADDR,DENALI_PI_387_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_388_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(388 + PI_BASE_ADDR,DENALI_PI_388_DATA,0);
       #else
         REGWR(388 + PI_BASE_ADDR,DENALI_PI_388_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_389_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(389 + PI_BASE_ADDR,DENALI_PI_389_DATA,0);
       #else
         REGWR(389 + PI_BASE_ADDR,DENALI_PI_389_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_390_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(390 + PI_BASE_ADDR,DENALI_PI_390_DATA,0);
       #else
         REGWR(390 + PI_BASE_ADDR,DENALI_PI_390_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_391_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(391 + PI_BASE_ADDR,DENALI_PI_391_DATA,0);
       #else
         REGWR(391 + PI_BASE_ADDR,DENALI_PI_391_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_392_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(392 + PI_BASE_ADDR,DENALI_PI_392_DATA,0);
       #else
         REGWR(392 + PI_BASE_ADDR,DENALI_PI_392_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_393_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(393 + PI_BASE_ADDR,DENALI_PI_393_DATA,0);
       #else
         REGWR(393 + PI_BASE_ADDR,DENALI_PI_393_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_394_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(394 + PI_BASE_ADDR,DENALI_PI_394_DATA,0);
       #else
         REGWR(394 + PI_BASE_ADDR,DENALI_PI_394_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_395_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(395 + PI_BASE_ADDR,DENALI_PI_395_DATA,0);
       #else
         REGWR(395 + PI_BASE_ADDR,DENALI_PI_395_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_396_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(396 + PI_BASE_ADDR,DENALI_PI_396_DATA,0);
       #else
         REGWR(396 + PI_BASE_ADDR,DENALI_PI_396_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_397_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(397 + PI_BASE_ADDR,DENALI_PI_397_DATA,0);
       #else
         REGWR(397 + PI_BASE_ADDR,DENALI_PI_397_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_398_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(398 + PI_BASE_ADDR,DENALI_PI_398_DATA,0);
       #else
         REGWR(398 + PI_BASE_ADDR,DENALI_PI_398_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_399_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(399 + PI_BASE_ADDR,DENALI_PI_399_DATA,0);
       #else
         REGWR(399 + PI_BASE_ADDR,DENALI_PI_399_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_400_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(400 + PI_BASE_ADDR,DENALI_PI_400_DATA,0);
       #else
         REGWR(400 + PI_BASE_ADDR,DENALI_PI_400_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_401_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(401 + PI_BASE_ADDR,DENALI_PI_401_DATA,0);
       #else
         REGWR(401 + PI_BASE_ADDR,DENALI_PI_401_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_402_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(402 + PI_BASE_ADDR,DENALI_PI_402_DATA,0);
       #else
         REGWR(402 + PI_BASE_ADDR,DENALI_PI_402_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_403_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(403 + PI_BASE_ADDR,DENALI_PI_403_DATA,0);
       #else
         REGWR(403 + PI_BASE_ADDR,DENALI_PI_403_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_404_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(404 + PI_BASE_ADDR,DENALI_PI_404_DATA,0);
       #else
         REGWR(404 + PI_BASE_ADDR,DENALI_PI_404_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_405_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(405 + PI_BASE_ADDR,DENALI_PI_405_DATA,0);
       #else
         REGWR(405 + PI_BASE_ADDR,DENALI_PI_405_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_406_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(406 + PI_BASE_ADDR,DENALI_PI_406_DATA,0);
       #else
         REGWR(406 + PI_BASE_ADDR,DENALI_PI_406_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_407_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(407 + PI_BASE_ADDR,DENALI_PI_407_DATA,0);
       #else
         REGWR(407 + PI_BASE_ADDR,DENALI_PI_407_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_408_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(408 + PI_BASE_ADDR,DENALI_PI_408_DATA,0);
       #else
         REGWR(408 + PI_BASE_ADDR,DENALI_PI_408_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_409_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(409 + PI_BASE_ADDR,DENALI_PI_409_DATA,0);
       #else
         REGWR(409 + PI_BASE_ADDR,DENALI_PI_409_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_410_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(410 + PI_BASE_ADDR,DENALI_PI_410_DATA,0);
       #else
         REGWR(410 + PI_BASE_ADDR,DENALI_PI_410_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_411_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(411 + PI_BASE_ADDR,DENALI_PI_411_DATA,0);
       #else
         REGWR(411 + PI_BASE_ADDR,DENALI_PI_411_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_412_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(412 + PI_BASE_ADDR,DENALI_PI_412_DATA,0);
       #else
         REGWR(412 + PI_BASE_ADDR,DENALI_PI_412_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_413_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(413 + PI_BASE_ADDR,DENALI_PI_413_DATA,0);
       #else
         REGWR(413 + PI_BASE_ADDR,DENALI_PI_413_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_414_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(414 + PI_BASE_ADDR,DENALI_PI_414_DATA,0);
       #else
         REGWR(414 + PI_BASE_ADDR,DENALI_PI_414_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_415_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(415 + PI_BASE_ADDR,DENALI_PI_415_DATA,0);
       #else
         REGWR(415 + PI_BASE_ADDR,DENALI_PI_415_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_416_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(416 + PI_BASE_ADDR,DENALI_PI_416_DATA,0);
       #else
         REGWR(416 + PI_BASE_ADDR,DENALI_PI_416_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_417_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(417 + PI_BASE_ADDR,DENALI_PI_417_DATA,0);
       #else
         REGWR(417 + PI_BASE_ADDR,DENALI_PI_417_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_418_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(418 + PI_BASE_ADDR,DENALI_PI_418_DATA,0);
       #else
         REGWR(418 + PI_BASE_ADDR,DENALI_PI_418_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_419_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(419 + PI_BASE_ADDR,DENALI_PI_419_DATA,0);
       #else
         REGWR(419 + PI_BASE_ADDR,DENALI_PI_419_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_420_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(420 + PI_BASE_ADDR,DENALI_PI_420_DATA,0);
       #else
         REGWR(420 + PI_BASE_ADDR,DENALI_PI_420_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_421_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(421 + PI_BASE_ADDR,DENALI_PI_421_DATA,0);
       #else
         REGWR(421 + PI_BASE_ADDR,DENALI_PI_421_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_422_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(422 + PI_BASE_ADDR,DENALI_PI_422_DATA,0);
       #else
         REGWR(422 + PI_BASE_ADDR,DENALI_PI_422_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_423_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(423 + PI_BASE_ADDR,DENALI_PI_423_DATA,0);
       #else
         REGWR(423 + PI_BASE_ADDR,DENALI_PI_423_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_424_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(424 + PI_BASE_ADDR,DENALI_PI_424_DATA,0);
       #else
         REGWR(424 + PI_BASE_ADDR,DENALI_PI_424_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_425_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(425 + PI_BASE_ADDR,DENALI_PI_425_DATA,0);
       #else
         REGWR(425 + PI_BASE_ADDR,DENALI_PI_425_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_426_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(426 + PI_BASE_ADDR,DENALI_PI_426_DATA,0);
       #else
         REGWR(426 + PI_BASE_ADDR,DENALI_PI_426_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_427_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(427 + PI_BASE_ADDR,DENALI_PI_427_DATA,0);
       #else
         REGWR(427 + PI_BASE_ADDR,DENALI_PI_427_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_428_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(428 + PI_BASE_ADDR,DENALI_PI_428_DATA,0);
       #else
         REGWR(428 + PI_BASE_ADDR,DENALI_PI_428_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_429_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(429 + PI_BASE_ADDR,DENALI_PI_429_DATA,0);
       #else
         REGWR(429 + PI_BASE_ADDR,DENALI_PI_429_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_430_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(430 + PI_BASE_ADDR,DENALI_PI_430_DATA,0);
       #else
         REGWR(430 + PI_BASE_ADDR,DENALI_PI_430_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_431_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(431 + PI_BASE_ADDR,DENALI_PI_431_DATA,0);
       #else
         REGWR(431 + PI_BASE_ADDR,DENALI_PI_431_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_432_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(432 + PI_BASE_ADDR,DENALI_PI_432_DATA,0);
       #else
         REGWR(432 + PI_BASE_ADDR,DENALI_PI_432_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_433_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(433 + PI_BASE_ADDR,DENALI_PI_433_DATA,0);
       #else
         REGWR(433 + PI_BASE_ADDR,DENALI_PI_433_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_434_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(434 + PI_BASE_ADDR,DENALI_PI_434_DATA,0);
       #else
         REGWR(434 + PI_BASE_ADDR,DENALI_PI_434_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_435_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(435 + PI_BASE_ADDR,DENALI_PI_435_DATA,0);
       #else
         REGWR(435 + PI_BASE_ADDR,DENALI_PI_435_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_436_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(436 + PI_BASE_ADDR,DENALI_PI_436_DATA,0);
       #else
         REGWR(436 + PI_BASE_ADDR,DENALI_PI_436_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_437_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(437 + PI_BASE_ADDR,DENALI_PI_437_DATA,0);
       #else
         REGWR(437 + PI_BASE_ADDR,DENALI_PI_437_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_438_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(438 + PI_BASE_ADDR,DENALI_PI_438_DATA,0);
       #else
         REGWR(438 + PI_BASE_ADDR,DENALI_PI_438_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_439_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(439 + PI_BASE_ADDR,DENALI_PI_439_DATA,0);
       #else
         REGWR(439 + PI_BASE_ADDR,DENALI_PI_439_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_440_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(440 + PI_BASE_ADDR,DENALI_PI_440_DATA,0);
       #else
         REGWR(440 + PI_BASE_ADDR,DENALI_PI_440_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_441_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(441 + PI_BASE_ADDR,DENALI_PI_441_DATA,0);
       #else
         REGWR(441 + PI_BASE_ADDR,DENALI_PI_441_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_442_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(442 + PI_BASE_ADDR,DENALI_PI_442_DATA,0);
       #else
         REGWR(442 + PI_BASE_ADDR,DENALI_PI_442_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_443_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(443 + PI_BASE_ADDR,DENALI_PI_443_DATA,0);
       #else
         REGWR(443 + PI_BASE_ADDR,DENALI_PI_443_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_444_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(444 + PI_BASE_ADDR,DENALI_PI_444_DATA,0);
       #else
         REGWR(444 + PI_BASE_ADDR,DENALI_PI_444_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_445_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(445 + PI_BASE_ADDR,DENALI_PI_445_DATA,0);
       #else
         REGWR(445 + PI_BASE_ADDR,DENALI_PI_445_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_446_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(446 + PI_BASE_ADDR,DENALI_PI_446_DATA,0);
       #else
         REGWR(446 + PI_BASE_ADDR,DENALI_PI_446_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_447_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(447 + PI_BASE_ADDR,DENALI_PI_447_DATA,0);
       #else
         REGWR(447 + PI_BASE_ADDR,DENALI_PI_447_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_448_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(448 + PI_BASE_ADDR,DENALI_PI_448_DATA,0);
       #else
         REGWR(448 + PI_BASE_ADDR,DENALI_PI_448_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_449_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(449 + PI_BASE_ADDR,DENALI_PI_449_DATA,0);
       #else
         REGWR(449 + PI_BASE_ADDR,DENALI_PI_449_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_450_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(450 + PI_BASE_ADDR,DENALI_PI_450_DATA,0);
       #else
         REGWR(450 + PI_BASE_ADDR,DENALI_PI_450_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_451_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(451 + PI_BASE_ADDR,DENALI_PI_451_DATA,0);
       #else
         REGWR(451 + PI_BASE_ADDR,DENALI_PI_451_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_452_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(452 + PI_BASE_ADDR,DENALI_PI_452_DATA,0);
       #else
         REGWR(452 + PI_BASE_ADDR,DENALI_PI_452_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_453_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(453 + PI_BASE_ADDR,DENALI_PI_453_DATA,0);
       #else
         REGWR(453 + PI_BASE_ADDR,DENALI_PI_453_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_454_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(454 + PI_BASE_ADDR,DENALI_PI_454_DATA,0);
       #else
         REGWR(454 + PI_BASE_ADDR,DENALI_PI_454_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_455_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(455 + PI_BASE_ADDR,DENALI_PI_455_DATA,0);
       #else
         REGWR(455 + PI_BASE_ADDR,DENALI_PI_455_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_456_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(456 + PI_BASE_ADDR,DENALI_PI_456_DATA,0);
       #else
         REGWR(456 + PI_BASE_ADDR,DENALI_PI_456_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_457_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(457 + PI_BASE_ADDR,DENALI_PI_457_DATA,0);
       #else
         REGWR(457 + PI_BASE_ADDR,DENALI_PI_457_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_458_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(458 + PI_BASE_ADDR,DENALI_PI_458_DATA,0);
       #else
         REGWR(458 + PI_BASE_ADDR,DENALI_PI_458_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_459_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(459 + PI_BASE_ADDR,DENALI_PI_459_DATA,0);
       #else
         REGWR(459 + PI_BASE_ADDR,DENALI_PI_459_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_460_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(460 + PI_BASE_ADDR,DENALI_PI_460_DATA,0);
       #else
         REGWR(460 + PI_BASE_ADDR,DENALI_PI_460_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_461_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(461 + PI_BASE_ADDR,DENALI_PI_461_DATA,0);
       #else
         REGWR(461 + PI_BASE_ADDR,DENALI_PI_461_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_462_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(462 + PI_BASE_ADDR,DENALI_PI_462_DATA,0);
       #else
         REGWR(462 + PI_BASE_ADDR,DENALI_PI_462_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_463_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(463 + PI_BASE_ADDR,DENALI_PI_463_DATA,0);
       #else
         REGWR(463 + PI_BASE_ADDR,DENALI_PI_463_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_464_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(464 + PI_BASE_ADDR,DENALI_PI_464_DATA,0);
       #else
         REGWR(464 + PI_BASE_ADDR,DENALI_PI_464_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_465_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(465 + PI_BASE_ADDR,DENALI_PI_465_DATA,0);
       #else
         REGWR(465 + PI_BASE_ADDR,DENALI_PI_465_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_466_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(466 + PI_BASE_ADDR,DENALI_PI_466_DATA,0);
       #else
         REGWR(466 + PI_BASE_ADDR,DENALI_PI_466_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_467_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(467 + PI_BASE_ADDR,DENALI_PI_467_DATA,0);
       #else
         REGWR(467 + PI_BASE_ADDR,DENALI_PI_467_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_468_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(468 + PI_BASE_ADDR,DENALI_PI_468_DATA,0);
       #else
         REGWR(468 + PI_BASE_ADDR,DENALI_PI_468_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_469_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(469 + PI_BASE_ADDR,DENALI_PI_469_DATA,0);
       #else
         REGWR(469 + PI_BASE_ADDR,DENALI_PI_469_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_470_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(470 + PI_BASE_ADDR,DENALI_PI_470_DATA,0);
       #else
         REGWR(470 + PI_BASE_ADDR,DENALI_PI_470_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_471_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(471 + PI_BASE_ADDR,DENALI_PI_471_DATA,0);
       #else
         REGWR(471 + PI_BASE_ADDR,DENALI_PI_471_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_472_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(472 + PI_BASE_ADDR,DENALI_PI_472_DATA,0);
       #else
         REGWR(472 + PI_BASE_ADDR,DENALI_PI_472_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_473_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(473 + PI_BASE_ADDR,DENALI_PI_473_DATA,0);
       #else
         REGWR(473 + PI_BASE_ADDR,DENALI_PI_473_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_474_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(474 + PI_BASE_ADDR,DENALI_PI_474_DATA,0);
       #else
         REGWR(474 + PI_BASE_ADDR,DENALI_PI_474_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_475_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(475 + PI_BASE_ADDR,DENALI_PI_475_DATA,0);
       #else
         REGWR(475 + PI_BASE_ADDR,DENALI_PI_475_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_476_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(476 + PI_BASE_ADDR,DENALI_PI_476_DATA,0);
       #else
         REGWR(476 + PI_BASE_ADDR,DENALI_PI_476_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_477_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(477 + PI_BASE_ADDR,DENALI_PI_477_DATA,0);
       #else
         REGWR(477 + PI_BASE_ADDR,DENALI_PI_477_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_478_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(478 + PI_BASE_ADDR,DENALI_PI_478_DATA,0);
       #else
         REGWR(478 + PI_BASE_ADDR,DENALI_PI_478_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_479_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(479 + PI_BASE_ADDR,DENALI_PI_479_DATA,0);
       #else
         REGWR(479 + PI_BASE_ADDR,DENALI_PI_479_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_480_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(480 + PI_BASE_ADDR,DENALI_PI_480_DATA,0);
       #else
         REGWR(480 + PI_BASE_ADDR,DENALI_PI_480_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_481_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(481 + PI_BASE_ADDR,DENALI_PI_481_DATA,0);
       #else
         REGWR(481 + PI_BASE_ADDR,DENALI_PI_481_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_482_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(482 + PI_BASE_ADDR,DENALI_PI_482_DATA,0);
       #else
         REGWR(482 + PI_BASE_ADDR,DENALI_PI_482_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_483_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(483 + PI_BASE_ADDR,DENALI_PI_483_DATA,0);
       #else
         REGWR(483 + PI_BASE_ADDR,DENALI_PI_483_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_484_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(484 + PI_BASE_ADDR,DENALI_PI_484_DATA,0);
       #else
         REGWR(484 + PI_BASE_ADDR,DENALI_PI_484_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_485_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(485 + PI_BASE_ADDR,DENALI_PI_485_DATA,0);
       #else
         REGWR(485 + PI_BASE_ADDR,DENALI_PI_485_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_486_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(486 + PI_BASE_ADDR,DENALI_PI_486_DATA,0);
       #else
         REGWR(486 + PI_BASE_ADDR,DENALI_PI_486_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_487_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(487 + PI_BASE_ADDR,DENALI_PI_487_DATA,0);
       #else
         REGWR(487 + PI_BASE_ADDR,DENALI_PI_487_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_488_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(488 + PI_BASE_ADDR,DENALI_PI_488_DATA,0);
       #else
         REGWR(488 + PI_BASE_ADDR,DENALI_PI_488_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_489_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(489 + PI_BASE_ADDR,DENALI_PI_489_DATA,0);
       #else
         REGWR(489 + PI_BASE_ADDR,DENALI_PI_489_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_490_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(490 + PI_BASE_ADDR,DENALI_PI_490_DATA,0);
       #else
         REGWR(490 + PI_BASE_ADDR,DENALI_PI_490_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_491_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(491 + PI_BASE_ADDR,DENALI_PI_491_DATA,0);
       #else
         REGWR(491 + PI_BASE_ADDR,DENALI_PI_491_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_492_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(492 + PI_BASE_ADDR,DENALI_PI_492_DATA,0);
       #else
         REGWR(492 + PI_BASE_ADDR,DENALI_PI_492_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_493_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(493 + PI_BASE_ADDR,DENALI_PI_493_DATA,0);
       #else
         REGWR(493 + PI_BASE_ADDR,DENALI_PI_493_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_494_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(494 + PI_BASE_ADDR,DENALI_PI_494_DATA,0);
       #else
         REGWR(494 + PI_BASE_ADDR,DENALI_PI_494_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_495_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(495 + PI_BASE_ADDR,DENALI_PI_495_DATA,0);
       #else
         REGWR(495 + PI_BASE_ADDR,DENALI_PI_495_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_496_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(496 + PI_BASE_ADDR,DENALI_PI_496_DATA,0);
       #else
         REGWR(496 + PI_BASE_ADDR,DENALI_PI_496_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_497_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(497 + PI_BASE_ADDR,DENALI_PI_497_DATA,0);
       #else
         REGWR(497 + PI_BASE_ADDR,DENALI_PI_497_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_498_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(498 + PI_BASE_ADDR,DENALI_PI_498_DATA,0);
       #else
         REGWR(498 + PI_BASE_ADDR,DENALI_PI_498_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_499_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(499 + PI_BASE_ADDR,DENALI_PI_499_DATA,0);
       #else
         REGWR(499 + PI_BASE_ADDR,DENALI_PI_499_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_500_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(500 + PI_BASE_ADDR,DENALI_PI_500_DATA,0);
       #else
         REGWR(500 + PI_BASE_ADDR,DENALI_PI_500_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_501_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(501 + PI_BASE_ADDR,DENALI_PI_501_DATA,0);
       #else
         REGWR(501 + PI_BASE_ADDR,DENALI_PI_501_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_502_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(502 + PI_BASE_ADDR,DENALI_PI_502_DATA,0);
       #else
         REGWR(502 + PI_BASE_ADDR,DENALI_PI_502_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_503_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(503 + PI_BASE_ADDR,DENALI_PI_503_DATA,0);
       #else
         REGWR(503 + PI_BASE_ADDR,DENALI_PI_503_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_504_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(504 + PI_BASE_ADDR,DENALI_PI_504_DATA,0);
       #else
         REGWR(504 + PI_BASE_ADDR,DENALI_PI_504_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_505_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(505 + PI_BASE_ADDR,DENALI_PI_505_DATA,0);
       #else
         REGWR(505 + PI_BASE_ADDR,DENALI_PI_505_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_506_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(506 + PI_BASE_ADDR,DENALI_PI_506_DATA,0);
       #else
         REGWR(506 + PI_BASE_ADDR,DENALI_PI_506_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_507_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(507 + PI_BASE_ADDR,DENALI_PI_507_DATA,0);
       #else
         REGWR(507 + PI_BASE_ADDR,DENALI_PI_507_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_508_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(508 + PI_BASE_ADDR,DENALI_PI_508_DATA,0);
       #else
         REGWR(508 + PI_BASE_ADDR,DENALI_PI_508_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_509_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(509 + PI_BASE_ADDR,DENALI_PI_509_DATA,0);
       #else
         REGWR(509 + PI_BASE_ADDR,DENALI_PI_509_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_510_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(510 + PI_BASE_ADDR,DENALI_PI_510_DATA,0);
       #else
         REGWR(510 + PI_BASE_ADDR,DENALI_PI_510_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_511_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(511 + PI_BASE_ADDR,DENALI_PI_511_DATA,0);
       #else
         REGWR(511 + PI_BASE_ADDR,DENALI_PI_511_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_512_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(512 + PI_BASE_ADDR,DENALI_PI_512_DATA,0);
       #else
         REGWR(512 + PI_BASE_ADDR,DENALI_PI_512_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_513_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(513 + PI_BASE_ADDR,DENALI_PI_513_DATA,0);
       #else
         REGWR(513 + PI_BASE_ADDR,DENALI_PI_513_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_514_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(514 + PI_BASE_ADDR,DENALI_PI_514_DATA,0);
       #else
         REGWR(514 + PI_BASE_ADDR,DENALI_PI_514_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_515_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(515 + PI_BASE_ADDR,DENALI_PI_515_DATA,0);
       #else
         REGWR(515 + PI_BASE_ADDR,DENALI_PI_515_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_516_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(516 + PI_BASE_ADDR,DENALI_PI_516_DATA,0);
       #else
         REGWR(516 + PI_BASE_ADDR,DENALI_PI_516_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_517_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(517 + PI_BASE_ADDR,DENALI_PI_517_DATA,0);
       #else
         REGWR(517 + PI_BASE_ADDR,DENALI_PI_517_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_518_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(518 + PI_BASE_ADDR,DENALI_PI_518_DATA,0);
       #else
         REGWR(518 + PI_BASE_ADDR,DENALI_PI_518_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_519_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(519 + PI_BASE_ADDR,DENALI_PI_519_DATA,0);
       #else
         REGWR(519 + PI_BASE_ADDR,DENALI_PI_519_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_520_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(520 + PI_BASE_ADDR,DENALI_PI_520_DATA,0);
       #else
         REGWR(520 + PI_BASE_ADDR,DENALI_PI_520_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_521_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(521 + PI_BASE_ADDR,DENALI_PI_521_DATA,0);
       #else
         REGWR(521 + PI_BASE_ADDR,DENALI_PI_521_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_522_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(522 + PI_BASE_ADDR,DENALI_PI_522_DATA,0);
       #else
         REGWR(522 + PI_BASE_ADDR,DENALI_PI_522_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_523_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(523 + PI_BASE_ADDR,DENALI_PI_523_DATA,0);
       #else
         REGWR(523 + PI_BASE_ADDR,DENALI_PI_523_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_524_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(524 + PI_BASE_ADDR,DENALI_PI_524_DATA,0);
       #else
         REGWR(524 + PI_BASE_ADDR,DENALI_PI_524_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_525_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(525 + PI_BASE_ADDR,DENALI_PI_525_DATA,0);
       #else
         REGWR(525 + PI_BASE_ADDR,DENALI_PI_525_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_526_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(526 + PI_BASE_ADDR,DENALI_PI_526_DATA,0);
       #else
         REGWR(526 + PI_BASE_ADDR,DENALI_PI_526_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_527_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(527 + PI_BASE_ADDR,DENALI_PI_527_DATA,0);
       #else
         REGWR(527 + PI_BASE_ADDR,DENALI_PI_527_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_528_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(528 + PI_BASE_ADDR,DENALI_PI_528_DATA,0);
       #else
         REGWR(528 + PI_BASE_ADDR,DENALI_PI_528_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_529_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(529 + PI_BASE_ADDR,DENALI_PI_529_DATA,0);
       #else
         REGWR(529 + PI_BASE_ADDR,DENALI_PI_529_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_530_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(530 + PI_BASE_ADDR,DENALI_PI_530_DATA,0);
       #else
         REGWR(530 + PI_BASE_ADDR,DENALI_PI_530_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_531_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(531 + PI_BASE_ADDR,DENALI_PI_531_DATA,0);
       #else
         REGWR(531 + PI_BASE_ADDR,DENALI_PI_531_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_532_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(532 + PI_BASE_ADDR,DENALI_PI_532_DATA,0);
       #else
         REGWR(532 + PI_BASE_ADDR,DENALI_PI_532_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_533_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(533 + PI_BASE_ADDR,DENALI_PI_533_DATA,0);
       #else
         REGWR(533 + PI_BASE_ADDR,DENALI_PI_533_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_534_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(534 + PI_BASE_ADDR,DENALI_PI_534_DATA,0);
       #else
         REGWR(534 + PI_BASE_ADDR,DENALI_PI_534_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_535_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(535 + PI_BASE_ADDR,DENALI_PI_535_DATA,0);
       #else
         REGWR(535 + PI_BASE_ADDR,DENALI_PI_535_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_536_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(536 + PI_BASE_ADDR,DENALI_PI_536_DATA,0);
       #else
         REGWR(536 + PI_BASE_ADDR,DENALI_PI_536_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_537_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(537 + PI_BASE_ADDR,DENALI_PI_537_DATA,0);
       #else
         REGWR(537 + PI_BASE_ADDR,DENALI_PI_537_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_538_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(538 + PI_BASE_ADDR,DENALI_PI_538_DATA,0);
       #else
         REGWR(538 + PI_BASE_ADDR,DENALI_PI_538_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_539_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(539 + PI_BASE_ADDR,DENALI_PI_539_DATA,0);
       #else
         REGWR(539 + PI_BASE_ADDR,DENALI_PI_539_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_540_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(540 + PI_BASE_ADDR,DENALI_PI_540_DATA,0);
       #else
         REGWR(540 + PI_BASE_ADDR,DENALI_PI_540_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_541_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(541 + PI_BASE_ADDR,DENALI_PI_541_DATA,0);
       #else
         REGWR(541 + PI_BASE_ADDR,DENALI_PI_541_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_542_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(542 + PI_BASE_ADDR,DENALI_PI_542_DATA,0);
       #else
         REGWR(542 + PI_BASE_ADDR,DENALI_PI_542_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_543_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(543 + PI_BASE_ADDR,DENALI_PI_543_DATA,0);
       #else
         REGWR(543 + PI_BASE_ADDR,DENALI_PI_543_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_544_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(544 + PI_BASE_ADDR,DENALI_PI_544_DATA,0);
       #else
         REGWR(544 + PI_BASE_ADDR,DENALI_PI_544_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_545_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(545 + PI_BASE_ADDR,DENALI_PI_545_DATA,0);
       #else
         REGWR(545 + PI_BASE_ADDR,DENALI_PI_545_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_546_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(546 + PI_BASE_ADDR,DENALI_PI_546_DATA,0);
       #else
         REGWR(546 + PI_BASE_ADDR,DENALI_PI_546_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_547_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(547 + PI_BASE_ADDR,DENALI_PI_547_DATA,0);
       #else
         REGWR(547 + PI_BASE_ADDR,DENALI_PI_547_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_548_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(548 + PI_BASE_ADDR,DENALI_PI_548_DATA,0);
       #else
         REGWR(548 + PI_BASE_ADDR,DENALI_PI_548_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_549_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(549 + PI_BASE_ADDR,DENALI_PI_549_DATA,0);
       #else
         REGWR(549 + PI_BASE_ADDR,DENALI_PI_549_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_550_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(550 + PI_BASE_ADDR,DENALI_PI_550_DATA,0);
       #else
         REGWR(550 + PI_BASE_ADDR,DENALI_PI_550_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_551_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(551 + PI_BASE_ADDR,DENALI_PI_551_DATA,0);
       #else
         REGWR(551 + PI_BASE_ADDR,DENALI_PI_551_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_552_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(552 + PI_BASE_ADDR,DENALI_PI_552_DATA,0);
       #else
         REGWR(552 + PI_BASE_ADDR,DENALI_PI_552_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_553_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(553 + PI_BASE_ADDR,DENALI_PI_553_DATA,0);
       #else
         REGWR(553 + PI_BASE_ADDR,DENALI_PI_553_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_554_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(554 + PI_BASE_ADDR,DENALI_PI_554_DATA,0);
       #else
         REGWR(554 + PI_BASE_ADDR,DENALI_PI_554_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_555_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(555 + PI_BASE_ADDR,DENALI_PI_555_DATA,0);
       #else
         REGWR(555 + PI_BASE_ADDR,DENALI_PI_555_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_556_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(556 + PI_BASE_ADDR,DENALI_PI_556_DATA,0);
       #else
         REGWR(556 + PI_BASE_ADDR,DENALI_PI_556_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_557_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(557 + PI_BASE_ADDR,DENALI_PI_557_DATA,0);
       #else
         REGWR(557 + PI_BASE_ADDR,DENALI_PI_557_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_558_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(558 + PI_BASE_ADDR,DENALI_PI_558_DATA,0);
       #else
         REGWR(558 + PI_BASE_ADDR,DENALI_PI_558_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_559_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(559 + PI_BASE_ADDR,DENALI_PI_559_DATA,0);
       #else
         REGWR(559 + PI_BASE_ADDR,DENALI_PI_559_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_560_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(560 + PI_BASE_ADDR,DENALI_PI_560_DATA,0);
       #else
         REGWR(560 + PI_BASE_ADDR,DENALI_PI_560_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_561_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(561 + PI_BASE_ADDR,DENALI_PI_561_DATA,0);
       #else
         REGWR(561 + PI_BASE_ADDR,DENALI_PI_561_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_562_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(562 + PI_BASE_ADDR,DENALI_PI_562_DATA,0);
       #else
         REGWR(562 + PI_BASE_ADDR,DENALI_PI_562_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_563_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(563 + PI_BASE_ADDR,DENALI_PI_563_DATA,0);
       #else
         REGWR(563 + PI_BASE_ADDR,DENALI_PI_563_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_564_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(564 + PI_BASE_ADDR,DENALI_PI_564_DATA,0);
       #else
         REGWR(564 + PI_BASE_ADDR,DENALI_PI_564_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_565_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(565 + PI_BASE_ADDR,DENALI_PI_565_DATA,0);
       #else
         REGWR(565 + PI_BASE_ADDR,DENALI_PI_565_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_566_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(566 + PI_BASE_ADDR,DENALI_PI_566_DATA,0);
       #else
         REGWR(566 + PI_BASE_ADDR,DENALI_PI_566_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_567_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(567 + PI_BASE_ADDR,DENALI_PI_567_DATA,0);
       #else
         REGWR(567 + PI_BASE_ADDR,DENALI_PI_567_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_568_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(568 + PI_BASE_ADDR,DENALI_PI_568_DATA,0);
       #else
         REGWR(568 + PI_BASE_ADDR,DENALI_PI_568_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_569_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(569 + PI_BASE_ADDR,DENALI_PI_569_DATA,0);
       #else
         REGWR(569 + PI_BASE_ADDR,DENALI_PI_569_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_570_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(570 + PI_BASE_ADDR,DENALI_PI_570_DATA,0);
       #else
         REGWR(570 + PI_BASE_ADDR,DENALI_PI_570_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_571_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(571 + PI_BASE_ADDR,DENALI_PI_571_DATA,0);
       #else
         REGWR(571 + PI_BASE_ADDR,DENALI_PI_571_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_572_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(572 + PI_BASE_ADDR,DENALI_PI_572_DATA,0);
       #else
         REGWR(572 + PI_BASE_ADDR,DENALI_PI_572_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_573_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(573 + PI_BASE_ADDR,DENALI_PI_573_DATA,0);
       #else
         REGWR(573 + PI_BASE_ADDR,DENALI_PI_573_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_574_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(574 + PI_BASE_ADDR,DENALI_PI_574_DATA,0);
       #else
         REGWR(574 + PI_BASE_ADDR,DENALI_PI_574_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_575_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(575 + PI_BASE_ADDR,DENALI_PI_575_DATA,0);
       #else
         REGWR(575 + PI_BASE_ADDR,DENALI_PI_575_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_576_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(576 + PI_BASE_ADDR,DENALI_PI_576_DATA,0);
       #else
         REGWR(576 + PI_BASE_ADDR,DENALI_PI_576_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_577_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(577 + PI_BASE_ADDR,DENALI_PI_577_DATA,0);
       #else
         REGWR(577 + PI_BASE_ADDR,DENALI_PI_577_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_578_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(578 + PI_BASE_ADDR,DENALI_PI_578_DATA,0);
       #else
         REGWR(578 + PI_BASE_ADDR,DENALI_PI_578_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_579_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(579 + PI_BASE_ADDR,DENALI_PI_579_DATA,0);
       #else
         REGWR(579 + PI_BASE_ADDR,DENALI_PI_579_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_580_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(580 + PI_BASE_ADDR,DENALI_PI_580_DATA,0);
       #else
         REGWR(580 + PI_BASE_ADDR,DENALI_PI_580_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_581_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(581 + PI_BASE_ADDR,DENALI_PI_581_DATA,0);
       #else
         REGWR(581 + PI_BASE_ADDR,DENALI_PI_581_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_582_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(582 + PI_BASE_ADDR,DENALI_PI_582_DATA,0);
       #else
         REGWR(582 + PI_BASE_ADDR,DENALI_PI_582_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_583_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(583 + PI_BASE_ADDR,DENALI_PI_583_DATA,0);
       #else
         REGWR(583 + PI_BASE_ADDR,DENALI_PI_583_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_584_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(584 + PI_BASE_ADDR,DENALI_PI_584_DATA,0);
       #else
         REGWR(584 + PI_BASE_ADDR,DENALI_PI_584_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_585_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(585 + PI_BASE_ADDR,DENALI_PI_585_DATA,0);
       #else
         REGWR(585 + PI_BASE_ADDR,DENALI_PI_585_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_586_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(586 + PI_BASE_ADDR,DENALI_PI_586_DATA,0);
       #else
         REGWR(586 + PI_BASE_ADDR,DENALI_PI_586_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_587_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(587 + PI_BASE_ADDR,DENALI_PI_587_DATA,0);
       #else
         REGWR(587 + PI_BASE_ADDR,DENALI_PI_587_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_588_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(588 + PI_BASE_ADDR,DENALI_PI_588_DATA,0);
       #else
         REGWR(588 + PI_BASE_ADDR,DENALI_PI_588_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_589_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(589 + PI_BASE_ADDR,DENALI_PI_589_DATA,0);
       #else
         REGWR(589 + PI_BASE_ADDR,DENALI_PI_589_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_590_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(590 + PI_BASE_ADDR,DENALI_PI_590_DATA,0);
       #else
         REGWR(590 + PI_BASE_ADDR,DENALI_PI_590_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_591_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(591 + PI_BASE_ADDR,DENALI_PI_591_DATA,0);
       #else
         REGWR(591 + PI_BASE_ADDR,DENALI_PI_591_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_592_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(592 + PI_BASE_ADDR,DENALI_PI_592_DATA,0);
       #else
         REGWR(592 + PI_BASE_ADDR,DENALI_PI_592_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_593_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(593 + PI_BASE_ADDR,DENALI_PI_593_DATA,0);
       #else
         REGWR(593 + PI_BASE_ADDR,DENALI_PI_593_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_594_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(594 + PI_BASE_ADDR,DENALI_PI_594_DATA,0);
       #else
         REGWR(594 + PI_BASE_ADDR,DENALI_PI_594_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_595_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(595 + PI_BASE_ADDR,DENALI_PI_595_DATA,0);
       #else
         REGWR(595 + PI_BASE_ADDR,DENALI_PI_595_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_596_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(596 + PI_BASE_ADDR,DENALI_PI_596_DATA,0);
       #else
         REGWR(596 + PI_BASE_ADDR,DENALI_PI_596_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_597_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(597 + PI_BASE_ADDR,DENALI_PI_597_DATA,0);
       #else
         REGWR(597 + PI_BASE_ADDR,DENALI_PI_597_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_598_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(598 + PI_BASE_ADDR,DENALI_PI_598_DATA,0);
       #else
         REGWR(598 + PI_BASE_ADDR,DENALI_PI_598_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_599_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(599 + PI_BASE_ADDR,DENALI_PI_599_DATA,0);
       #else
         REGWR(599 + PI_BASE_ADDR,DENALI_PI_599_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_600_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(600 + PI_BASE_ADDR,DENALI_PI_600_DATA,0);
       #else
         REGWR(600 + PI_BASE_ADDR,DENALI_PI_600_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_601_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(601 + PI_BASE_ADDR,DENALI_PI_601_DATA,0);
       #else
         REGWR(601 + PI_BASE_ADDR,DENALI_PI_601_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_602_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(602 + PI_BASE_ADDR,DENALI_PI_602_DATA,0);
       #else
         REGWR(602 + PI_BASE_ADDR,DENALI_PI_602_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_603_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(603 + PI_BASE_ADDR,DENALI_PI_603_DATA,0);
       #else
         REGWR(603 + PI_BASE_ADDR,DENALI_PI_603_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_604_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(604 + PI_BASE_ADDR,DENALI_PI_604_DATA,0);
       #else
         REGWR(604 + PI_BASE_ADDR,DENALI_PI_604_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_605_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(605 + PI_BASE_ADDR,DENALI_PI_605_DATA,0);
       #else
         REGWR(605 + PI_BASE_ADDR,DENALI_PI_605_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_606_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(606 + PI_BASE_ADDR,DENALI_PI_606_DATA,0);
       #else
         REGWR(606 + PI_BASE_ADDR,DENALI_PI_606_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_607_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(607 + PI_BASE_ADDR,DENALI_PI_607_DATA,0);
       #else
         REGWR(607 + PI_BASE_ADDR,DENALI_PI_607_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_608_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(608 + PI_BASE_ADDR,DENALI_PI_608_DATA,0);
       #else
         REGWR(608 + PI_BASE_ADDR,DENALI_PI_608_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_609_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(609 + PI_BASE_ADDR,DENALI_PI_609_DATA,0);
       #else
         REGWR(609 + PI_BASE_ADDR,DENALI_PI_609_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_610_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(610 + PI_BASE_ADDR,DENALI_PI_610_DATA,0);
       #else
         REGWR(610 + PI_BASE_ADDR,DENALI_PI_610_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_611_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(611 + PI_BASE_ADDR,DENALI_PI_611_DATA,0);
       #else
         REGWR(611 + PI_BASE_ADDR,DENALI_PI_611_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_612_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(612 + PI_BASE_ADDR,DENALI_PI_612_DATA,0);
       #else
         REGWR(612 + PI_BASE_ADDR,DENALI_PI_612_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_613_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(613 + PI_BASE_ADDR,DENALI_PI_613_DATA,0);
       #else
         REGWR(613 + PI_BASE_ADDR,DENALI_PI_613_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_614_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(614 + PI_BASE_ADDR,DENALI_PI_614_DATA,0);
       #else
         REGWR(614 + PI_BASE_ADDR,DENALI_PI_614_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_615_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(615 + PI_BASE_ADDR,DENALI_PI_615_DATA,0);
       #else
         REGWR(615 + PI_BASE_ADDR,DENALI_PI_615_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_616_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(616 + PI_BASE_ADDR,DENALI_PI_616_DATA,0);
       #else
         REGWR(616 + PI_BASE_ADDR,DENALI_PI_616_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_617_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(617 + PI_BASE_ADDR,DENALI_PI_617_DATA,0);
       #else
         REGWR(617 + PI_BASE_ADDR,DENALI_PI_617_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_618_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(618 + PI_BASE_ADDR,DENALI_PI_618_DATA,0);
       #else
         REGWR(618 + PI_BASE_ADDR,DENALI_PI_618_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_619_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(619 + PI_BASE_ADDR,DENALI_PI_619_DATA,0);
       #else
         REGWR(619 + PI_BASE_ADDR,DENALI_PI_619_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_620_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(620 + PI_BASE_ADDR,DENALI_PI_620_DATA,0);
       #else
         REGWR(620 + PI_BASE_ADDR,DENALI_PI_620_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_621_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(621 + PI_BASE_ADDR,DENALI_PI_621_DATA,0);
       #else
         REGWR(621 + PI_BASE_ADDR,DENALI_PI_621_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_622_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(622 + PI_BASE_ADDR,DENALI_PI_622_DATA,0);
       #else
         REGWR(622 + PI_BASE_ADDR,DENALI_PI_622_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_623_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(623 + PI_BASE_ADDR,DENALI_PI_623_DATA,0);
       #else
         REGWR(623 + PI_BASE_ADDR,DENALI_PI_623_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_624_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(624 + PI_BASE_ADDR,DENALI_PI_624_DATA,0);
       #else
         REGWR(624 + PI_BASE_ADDR,DENALI_PI_624_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_625_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(625 + PI_BASE_ADDR,DENALI_PI_625_DATA,0);
       #else
         REGWR(625 + PI_BASE_ADDR,DENALI_PI_625_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_626_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(626 + PI_BASE_ADDR,DENALI_PI_626_DATA,0);
       #else
         REGWR(626 + PI_BASE_ADDR,DENALI_PI_626_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_627_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(627 + PI_BASE_ADDR,DENALI_PI_627_DATA,0);
       #else
         REGWR(627 + PI_BASE_ADDR,DENALI_PI_627_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_628_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(628 + PI_BASE_ADDR,DENALI_PI_628_DATA,0);
       #else
         REGWR(628 + PI_BASE_ADDR,DENALI_PI_628_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_629_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(629 + PI_BASE_ADDR,DENALI_PI_629_DATA,0);
       #else
         REGWR(629 + PI_BASE_ADDR,DENALI_PI_629_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_630_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(630 + PI_BASE_ADDR,DENALI_PI_630_DATA,0);
       #else
         REGWR(630 + PI_BASE_ADDR,DENALI_PI_630_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_631_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(631 + PI_BASE_ADDR,DENALI_PI_631_DATA,0);
       #else
         REGWR(631 + PI_BASE_ADDR,DENALI_PI_631_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_632_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(632 + PI_BASE_ADDR,DENALI_PI_632_DATA,0);
       #else
         REGWR(632 + PI_BASE_ADDR,DENALI_PI_632_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_633_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(633 + PI_BASE_ADDR,DENALI_PI_633_DATA,0);
       #else
         REGWR(633 + PI_BASE_ADDR,DENALI_PI_633_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_634_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(634 + PI_BASE_ADDR,DENALI_PI_634_DATA,0);
       #else
         REGWR(634 + PI_BASE_ADDR,DENALI_PI_634_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_635_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(635 + PI_BASE_ADDR,DENALI_PI_635_DATA,0);
       #else
         REGWR(635 + PI_BASE_ADDR,DENALI_PI_635_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_636_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(636 + PI_BASE_ADDR,DENALI_PI_636_DATA,0);
       #else
         REGWR(636 + PI_BASE_ADDR,DENALI_PI_636_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_637_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(637 + PI_BASE_ADDR,DENALI_PI_637_DATA,0);
       #else
         REGWR(637 + PI_BASE_ADDR,DENALI_PI_637_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_638_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(638 + PI_BASE_ADDR,DENALI_PI_638_DATA,0);
       #else
         REGWR(638 + PI_BASE_ADDR,DENALI_PI_638_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_639_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(639 + PI_BASE_ADDR,DENALI_PI_639_DATA,0);
       #else
         REGWR(639 + PI_BASE_ADDR,DENALI_PI_639_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_640_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(640 + PI_BASE_ADDR,DENALI_PI_640_DATA,0);
       #else
         REGWR(640 + PI_BASE_ADDR,DENALI_PI_640_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_641_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(641 + PI_BASE_ADDR,DENALI_PI_641_DATA,0);
       #else
         REGWR(641 + PI_BASE_ADDR,DENALI_PI_641_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_642_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(642 + PI_BASE_ADDR,DENALI_PI_642_DATA,0);
       #else
         REGWR(642 + PI_BASE_ADDR,DENALI_PI_642_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_643_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(643 + PI_BASE_ADDR,DENALI_PI_643_DATA,0);
       #else
         REGWR(643 + PI_BASE_ADDR,DENALI_PI_643_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_644_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(644 + PI_BASE_ADDR,DENALI_PI_644_DATA,0);
       #else
         REGWR(644 + PI_BASE_ADDR,DENALI_PI_644_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_645_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(645 + PI_BASE_ADDR,DENALI_PI_645_DATA,0);
       #else
         REGWR(645 + PI_BASE_ADDR,DENALI_PI_645_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_646_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(646 + PI_BASE_ADDR,DENALI_PI_646_DATA,0);
       #else
         REGWR(646 + PI_BASE_ADDR,DENALI_PI_646_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_647_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(647 + PI_BASE_ADDR,DENALI_PI_647_DATA,0);
       #else
         REGWR(647 + PI_BASE_ADDR,DENALI_PI_647_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_648_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(648 + PI_BASE_ADDR,DENALI_PI_648_DATA,0);
       #else
         REGWR(648 + PI_BASE_ADDR,DENALI_PI_648_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_649_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(649 + PI_BASE_ADDR,DENALI_PI_649_DATA,0);
       #else
         REGWR(649 + PI_BASE_ADDR,DENALI_PI_649_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_650_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(650 + PI_BASE_ADDR,DENALI_PI_650_DATA,0);
       #else
         REGWR(650 + PI_BASE_ADDR,DENALI_PI_650_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_651_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(651 + PI_BASE_ADDR,DENALI_PI_651_DATA,0);
       #else
         REGWR(651 + PI_BASE_ADDR,DENALI_PI_651_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_652_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(652 + PI_BASE_ADDR,DENALI_PI_652_DATA,0);
       #else
         REGWR(652 + PI_BASE_ADDR,DENALI_PI_652_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_653_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(653 + PI_BASE_ADDR,DENALI_PI_653_DATA,0);
       #else
         REGWR(653 + PI_BASE_ADDR,DENALI_PI_653_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_654_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(654 + PI_BASE_ADDR,DENALI_PI_654_DATA,0);
       #else
         REGWR(654 + PI_BASE_ADDR,DENALI_PI_654_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_655_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(655 + PI_BASE_ADDR,DENALI_PI_655_DATA,0);
       #else
         REGWR(655 + PI_BASE_ADDR,DENALI_PI_655_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_656_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(656 + PI_BASE_ADDR,DENALI_PI_656_DATA,0);
       #else
         REGWR(656 + PI_BASE_ADDR,DENALI_PI_656_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_657_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(657 + PI_BASE_ADDR,DENALI_PI_657_DATA,0);
       #else
         REGWR(657 + PI_BASE_ADDR,DENALI_PI_657_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_658_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(658 + PI_BASE_ADDR,DENALI_PI_658_DATA,0);
       #else
         REGWR(658 + PI_BASE_ADDR,DENALI_PI_658_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_659_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(659 + PI_BASE_ADDR,DENALI_PI_659_DATA,0);
       #else
         REGWR(659 + PI_BASE_ADDR,DENALI_PI_659_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_660_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(660 + PI_BASE_ADDR,DENALI_PI_660_DATA,0);
       #else
         REGWR(660 + PI_BASE_ADDR,DENALI_PI_660_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_661_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(661 + PI_BASE_ADDR,DENALI_PI_661_DATA,0);
       #else
         REGWR(661 + PI_BASE_ADDR,DENALI_PI_661_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_662_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(662 + PI_BASE_ADDR,DENALI_PI_662_DATA,0);
       #else
         REGWR(662 + PI_BASE_ADDR,DENALI_PI_662_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_663_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(663 + PI_BASE_ADDR,DENALI_PI_663_DATA,0);
       #else
         REGWR(663 + PI_BASE_ADDR,DENALI_PI_663_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_664_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(664 + PI_BASE_ADDR,DENALI_PI_664_DATA,0);
       #else
         REGWR(664 + PI_BASE_ADDR,DENALI_PI_664_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_665_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(665 + PI_BASE_ADDR,DENALI_PI_665_DATA,0);
       #else
         REGWR(665 + PI_BASE_ADDR,DENALI_PI_665_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_666_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(666 + PI_BASE_ADDR,DENALI_PI_666_DATA,0);
       #else
         REGWR(666 + PI_BASE_ADDR,DENALI_PI_666_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_667_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(667 + PI_BASE_ADDR,DENALI_PI_667_DATA,0);
       #else
         REGWR(667 + PI_BASE_ADDR,DENALI_PI_667_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_668_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(668 + PI_BASE_ADDR,DENALI_PI_668_DATA,0);
       #else
         REGWR(668 + PI_BASE_ADDR,DENALI_PI_668_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_669_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(669 + PI_BASE_ADDR,DENALI_PI_669_DATA,0);
       #else
         REGWR(669 + PI_BASE_ADDR,DENALI_PI_669_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_670_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(670 + PI_BASE_ADDR,DENALI_PI_670_DATA,0);
       #else
         REGWR(670 + PI_BASE_ADDR,DENALI_PI_670_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_671_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(671 + PI_BASE_ADDR,DENALI_PI_671_DATA,0);
       #else
         REGWR(671 + PI_BASE_ADDR,DENALI_PI_671_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_672_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(672 + PI_BASE_ADDR,DENALI_PI_672_DATA,0);
       #else
         REGWR(672 + PI_BASE_ADDR,DENALI_PI_672_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_673_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(673 + PI_BASE_ADDR,DENALI_PI_673_DATA,0);
       #else
         REGWR(673 + PI_BASE_ADDR,DENALI_PI_673_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_674_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(674 + PI_BASE_ADDR,DENALI_PI_674_DATA,0);
       #else
         REGWR(674 + PI_BASE_ADDR,DENALI_PI_674_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_675_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(675 + PI_BASE_ADDR,DENALI_PI_675_DATA,0);
       #else
         REGWR(675 + PI_BASE_ADDR,DENALI_PI_675_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_676_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(676 + PI_BASE_ADDR,DENALI_PI_676_DATA,0);
       #else
         REGWR(676 + PI_BASE_ADDR,DENALI_PI_676_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_677_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(677 + PI_BASE_ADDR,DENALI_PI_677_DATA,0);
       #else
         REGWR(677 + PI_BASE_ADDR,DENALI_PI_677_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_678_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(678 + PI_BASE_ADDR,DENALI_PI_678_DATA,0);
       #else
         REGWR(678 + PI_BASE_ADDR,DENALI_PI_678_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_679_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(679 + PI_BASE_ADDR,DENALI_PI_679_DATA,0);
       #else
         REGWR(679 + PI_BASE_ADDR,DENALI_PI_679_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_680_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(680 + PI_BASE_ADDR,DENALI_PI_680_DATA,0);
       #else
         REGWR(680 + PI_BASE_ADDR,DENALI_PI_680_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_681_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(681 + PI_BASE_ADDR,DENALI_PI_681_DATA,0);
       #else
         REGWR(681 + PI_BASE_ADDR,DENALI_PI_681_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_682_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(682 + PI_BASE_ADDR,DENALI_PI_682_DATA,0);
       #else
         REGWR(682 + PI_BASE_ADDR,DENALI_PI_682_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_683_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(683 + PI_BASE_ADDR,DENALI_PI_683_DATA,0);
       #else
         REGWR(683 + PI_BASE_ADDR,DENALI_PI_683_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_684_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(684 + PI_BASE_ADDR,DENALI_PI_684_DATA,0);
       #else
         REGWR(684 + PI_BASE_ADDR,DENALI_PI_684_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_685_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(685 + PI_BASE_ADDR,DENALI_PI_685_DATA,0);
       #else
         REGWR(685 + PI_BASE_ADDR,DENALI_PI_685_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_686_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(686 + PI_BASE_ADDR,DENALI_PI_686_DATA,0);
       #else
         REGWR(686 + PI_BASE_ADDR,DENALI_PI_686_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_687_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(687 + PI_BASE_ADDR,DENALI_PI_687_DATA,0);
       #else
         REGWR(687 + PI_BASE_ADDR,DENALI_PI_687_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_688_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(688 + PI_BASE_ADDR,DENALI_PI_688_DATA,0);
       #else
         REGWR(688 + PI_BASE_ADDR,DENALI_PI_688_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_689_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(689 + PI_BASE_ADDR,DENALI_PI_689_DATA,0);
       #else
         REGWR(689 + PI_BASE_ADDR,DENALI_PI_689_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_690_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(690 + PI_BASE_ADDR,DENALI_PI_690_DATA,0);
       #else
         REGWR(690 + PI_BASE_ADDR,DENALI_PI_690_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_691_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(691 + PI_BASE_ADDR,DENALI_PI_691_DATA,0);
       #else
         REGWR(691 + PI_BASE_ADDR,DENALI_PI_691_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_692_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(692 + PI_BASE_ADDR,DENALI_PI_692_DATA,0);
       #else
         REGWR(692 + PI_BASE_ADDR,DENALI_PI_692_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_693_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(693 + PI_BASE_ADDR,DENALI_PI_693_DATA,0);
       #else
         REGWR(693 + PI_BASE_ADDR,DENALI_PI_693_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_694_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(694 + PI_BASE_ADDR,DENALI_PI_694_DATA,0);
       #else
         REGWR(694 + PI_BASE_ADDR,DENALI_PI_694_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_695_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(695 + PI_BASE_ADDR,DENALI_PI_695_DATA,0);
       #else
         REGWR(695 + PI_BASE_ADDR,DENALI_PI_695_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_696_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(696 + PI_BASE_ADDR,DENALI_PI_696_DATA,0);
       #else
         REGWR(696 + PI_BASE_ADDR,DENALI_PI_696_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_697_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(697 + PI_BASE_ADDR,DENALI_PI_697_DATA,0);
       #else
         REGWR(697 + PI_BASE_ADDR,DENALI_PI_697_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_698_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(698 + PI_BASE_ADDR,DENALI_PI_698_DATA,0);
       #else
         REGWR(698 + PI_BASE_ADDR,DENALI_PI_698_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_699_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(699 + PI_BASE_ADDR,DENALI_PI_699_DATA,0);
       #else
         REGWR(699 + PI_BASE_ADDR,DENALI_PI_699_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_700_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(700 + PI_BASE_ADDR,DENALI_PI_700_DATA,0);
       #else
         REGWR(700 + PI_BASE_ADDR,DENALI_PI_700_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_701_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(701 + PI_BASE_ADDR,DENALI_PI_701_DATA,0);
       #else
         REGWR(701 + PI_BASE_ADDR,DENALI_PI_701_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_702_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(702 + PI_BASE_ADDR,DENALI_PI_702_DATA,0);
       #else
         REGWR(702 + PI_BASE_ADDR,DENALI_PI_702_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_703_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(703 + PI_BASE_ADDR,DENALI_PI_703_DATA,0);
       #else
         REGWR(703 + PI_BASE_ADDR,DENALI_PI_703_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_704_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(704 + PI_BASE_ADDR,DENALI_PI_704_DATA,0);
       #else
         REGWR(704 + PI_BASE_ADDR,DENALI_PI_704_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_705_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(705 + PI_BASE_ADDR,DENALI_PI_705_DATA,0);
       #else
         REGWR(705 + PI_BASE_ADDR,DENALI_PI_705_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_706_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(706 + PI_BASE_ADDR,DENALI_PI_706_DATA,0);
       #else
         REGWR(706 + PI_BASE_ADDR,DENALI_PI_706_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_707_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(707 + PI_BASE_ADDR,DENALI_PI_707_DATA,0);
       #else
         REGWR(707 + PI_BASE_ADDR,DENALI_PI_707_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_708_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(708 + PI_BASE_ADDR,DENALI_PI_708_DATA,0);
       #else
         REGWR(708 + PI_BASE_ADDR,DENALI_PI_708_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_709_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(709 + PI_BASE_ADDR,DENALI_PI_709_DATA,0);
       #else
         REGWR(709 + PI_BASE_ADDR,DENALI_PI_709_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_710_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(710 + PI_BASE_ADDR,DENALI_PI_710_DATA,0);
       #else
         REGWR(710 + PI_BASE_ADDR,DENALI_PI_710_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_711_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(711 + PI_BASE_ADDR,DENALI_PI_711_DATA,0);
       #else
         REGWR(711 + PI_BASE_ADDR,DENALI_PI_711_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_712_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(712 + PI_BASE_ADDR,DENALI_PI_712_DATA,0);
       #else
         REGWR(712 + PI_BASE_ADDR,DENALI_PI_712_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_713_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(713 + PI_BASE_ADDR,DENALI_PI_713_DATA,0);
       #else
         REGWR(713 + PI_BASE_ADDR,DENALI_PI_713_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_714_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(714 + PI_BASE_ADDR,DENALI_PI_714_DATA,0);
       #else
         REGWR(714 + PI_BASE_ADDR,DENALI_PI_714_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_715_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(715 + PI_BASE_ADDR,DENALI_PI_715_DATA,0);
       #else
         REGWR(715 + PI_BASE_ADDR,DENALI_PI_715_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_716_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(716 + PI_BASE_ADDR,DENALI_PI_716_DATA,0);
       #else
         REGWR(716 + PI_BASE_ADDR,DENALI_PI_716_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_717_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(717 + PI_BASE_ADDR,DENALI_PI_717_DATA,0);
       #else
         REGWR(717 + PI_BASE_ADDR,DENALI_PI_717_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_718_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(718 + PI_BASE_ADDR,DENALI_PI_718_DATA,0);
       #else
         REGWR(718 + PI_BASE_ADDR,DENALI_PI_718_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_719_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(719 + PI_BASE_ADDR,DENALI_PI_719_DATA,0);
       #else
         REGWR(719 + PI_BASE_ADDR,DENALI_PI_719_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_720_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(720 + PI_BASE_ADDR,DENALI_PI_720_DATA,0);
       #else
         REGWR(720 + PI_BASE_ADDR,DENALI_PI_720_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_721_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(721 + PI_BASE_ADDR,DENALI_PI_721_DATA,0);
       #else
         REGWR(721 + PI_BASE_ADDR,DENALI_PI_721_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_722_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(722 + PI_BASE_ADDR,DENALI_PI_722_DATA,0);
       #else
         REGWR(722 + PI_BASE_ADDR,DENALI_PI_722_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_723_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(723 + PI_BASE_ADDR,DENALI_PI_723_DATA,0);
       #else
         REGWR(723 + PI_BASE_ADDR,DENALI_PI_723_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_724_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(724 + PI_BASE_ADDR,DENALI_PI_724_DATA,0);
       #else
         REGWR(724 + PI_BASE_ADDR,DENALI_PI_724_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_725_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(725 + PI_BASE_ADDR,DENALI_PI_725_DATA,0);
       #else
         REGWR(725 + PI_BASE_ADDR,DENALI_PI_725_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_726_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(726 + PI_BASE_ADDR,DENALI_PI_726_DATA,0);
       #else
         REGWR(726 + PI_BASE_ADDR,DENALI_PI_726_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_727_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(727 + PI_BASE_ADDR,DENALI_PI_727_DATA,0);
       #else
         REGWR(727 + PI_BASE_ADDR,DENALI_PI_727_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_728_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(728 + PI_BASE_ADDR,DENALI_PI_728_DATA,0);
       #else
         REGWR(728 + PI_BASE_ADDR,DENALI_PI_728_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_729_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(729 + PI_BASE_ADDR,DENALI_PI_729_DATA,0);
       #else
         REGWR(729 + PI_BASE_ADDR,DENALI_PI_729_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_730_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(730 + PI_BASE_ADDR,DENALI_PI_730_DATA,0);
       #else
         REGWR(730 + PI_BASE_ADDR,DENALI_PI_730_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_731_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(731 + PI_BASE_ADDR,DENALI_PI_731_DATA,0);
       #else
         REGWR(731 + PI_BASE_ADDR,DENALI_PI_731_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_732_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(732 + PI_BASE_ADDR,DENALI_PI_732_DATA,0);
       #else
         REGWR(732 + PI_BASE_ADDR,DENALI_PI_732_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_733_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(733 + PI_BASE_ADDR,DENALI_PI_733_DATA,0);
       #else
         REGWR(733 + PI_BASE_ADDR,DENALI_PI_733_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_734_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(734 + PI_BASE_ADDR,DENALI_PI_734_DATA,0);
       #else
         REGWR(734 + PI_BASE_ADDR,DENALI_PI_734_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_735_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(735 + PI_BASE_ADDR,DENALI_PI_735_DATA,0);
       #else
         REGWR(735 + PI_BASE_ADDR,DENALI_PI_735_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_736_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(736 + PI_BASE_ADDR,DENALI_PI_736_DATA,0);
       #else
         REGWR(736 + PI_BASE_ADDR,DENALI_PI_736_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_737_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(737 + PI_BASE_ADDR,DENALI_PI_737_DATA,0);
       #else
         REGWR(737 + PI_BASE_ADDR,DENALI_PI_737_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_738_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(738 + PI_BASE_ADDR,DENALI_PI_738_DATA,0);
       #else
         REGWR(738 + PI_BASE_ADDR,DENALI_PI_738_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_739_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(739 + PI_BASE_ADDR,DENALI_PI_739_DATA,0);
       #else
         REGWR(739 + PI_BASE_ADDR,DENALI_PI_739_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_740_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(740 + PI_BASE_ADDR,DENALI_PI_740_DATA,0);
       #else
         REGWR(740 + PI_BASE_ADDR,DENALI_PI_740_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_741_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(741 + PI_BASE_ADDR,DENALI_PI_741_DATA,0);
       #else
         REGWR(741 + PI_BASE_ADDR,DENALI_PI_741_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_742_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(742 + PI_BASE_ADDR,DENALI_PI_742_DATA,0);
       #else
         REGWR(742 + PI_BASE_ADDR,DENALI_PI_742_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_743_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(743 + PI_BASE_ADDR,DENALI_PI_743_DATA,0);
       #else
         REGWR(743 + PI_BASE_ADDR,DENALI_PI_743_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_744_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(744 + PI_BASE_ADDR,DENALI_PI_744_DATA,0);
       #else
         REGWR(744 + PI_BASE_ADDR,DENALI_PI_744_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_745_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(745 + PI_BASE_ADDR,DENALI_PI_745_DATA,0);
       #else
         REGWR(745 + PI_BASE_ADDR,DENALI_PI_745_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_746_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(746 + PI_BASE_ADDR,DENALI_PI_746_DATA,0);
       #else
         REGWR(746 + PI_BASE_ADDR,DENALI_PI_746_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_747_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(747 + PI_BASE_ADDR,DENALI_PI_747_DATA,0);
       #else
         REGWR(747 + PI_BASE_ADDR,DENALI_PI_747_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_748_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(748 + PI_BASE_ADDR,DENALI_PI_748_DATA,0);
       #else
         REGWR(748 + PI_BASE_ADDR,DENALI_PI_748_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_749_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(749 + PI_BASE_ADDR,DENALI_PI_749_DATA,0);
       #else
         REGWR(749 + PI_BASE_ADDR,DENALI_PI_749_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_750_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(750 + PI_BASE_ADDR,DENALI_PI_750_DATA,0);
       #else
         REGWR(750 + PI_BASE_ADDR,DENALI_PI_750_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_751_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(751 + PI_BASE_ADDR,DENALI_PI_751_DATA,0);
       #else
         REGWR(751 + PI_BASE_ADDR,DENALI_PI_751_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_752_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(752 + PI_BASE_ADDR,DENALI_PI_752_DATA,0);
       #else
         REGWR(752 + PI_BASE_ADDR,DENALI_PI_752_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_753_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(753 + PI_BASE_ADDR,DENALI_PI_753_DATA,0);
       #else
         REGWR(753 + PI_BASE_ADDR,DENALI_PI_753_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_754_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(754 + PI_BASE_ADDR,DENALI_PI_754_DATA,0);
       #else
         REGWR(754 + PI_BASE_ADDR,DENALI_PI_754_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_755_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(755 + PI_BASE_ADDR,DENALI_PI_755_DATA,0);
       #else
         REGWR(755 + PI_BASE_ADDR,DENALI_PI_755_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_756_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(756 + PI_BASE_ADDR,DENALI_PI_756_DATA,0);
       #else
         REGWR(756 + PI_BASE_ADDR,DENALI_PI_756_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_757_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(757 + PI_BASE_ADDR,DENALI_PI_757_DATA,0);
       #else
         REGWR(757 + PI_BASE_ADDR,DENALI_PI_757_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_758_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(758 + PI_BASE_ADDR,DENALI_PI_758_DATA,0);
       #else
         REGWR(758 + PI_BASE_ADDR,DENALI_PI_758_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_759_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(759 + PI_BASE_ADDR,DENALI_PI_759_DATA,0);
       #else
         REGWR(759 + PI_BASE_ADDR,DENALI_PI_759_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_760_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(760 + PI_BASE_ADDR,DENALI_PI_760_DATA,0);
       #else
         REGWR(760 + PI_BASE_ADDR,DENALI_PI_760_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_761_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(761 + PI_BASE_ADDR,DENALI_PI_761_DATA,0);
       #else
         REGWR(761 + PI_BASE_ADDR,DENALI_PI_761_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_762_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(762 + PI_BASE_ADDR,DENALI_PI_762_DATA,0);
       #else
         REGWR(762 + PI_BASE_ADDR,DENALI_PI_762_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_763_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(763 + PI_BASE_ADDR,DENALI_PI_763_DATA,0);
       #else
         REGWR(763 + PI_BASE_ADDR,DENALI_PI_763_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_764_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(764 + PI_BASE_ADDR,DENALI_PI_764_DATA,0);
       #else
         REGWR(764 + PI_BASE_ADDR,DENALI_PI_764_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_765_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(765 + PI_BASE_ADDR,DENALI_PI_765_DATA,0);
       #else
         REGWR(765 + PI_BASE_ADDR,DENALI_PI_765_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_766_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(766 + PI_BASE_ADDR,DENALI_PI_766_DATA,0);
       #else
         REGWR(766 + PI_BASE_ADDR,DENALI_PI_766_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_767_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(767 + PI_BASE_ADDR,DENALI_PI_767_DATA,0);
       #else
         REGWR(767 + PI_BASE_ADDR,DENALI_PI_767_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_768_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(768 + PI_BASE_ADDR,DENALI_PI_768_DATA,0);
       #else
         REGWR(768 + PI_BASE_ADDR,DENALI_PI_768_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_769_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(769 + PI_BASE_ADDR,DENALI_PI_769_DATA,0);
       #else
         REGWR(769 + PI_BASE_ADDR,DENALI_PI_769_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_770_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(770 + PI_BASE_ADDR,DENALI_PI_770_DATA,0);
       #else
         REGWR(770 + PI_BASE_ADDR,DENALI_PI_770_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_771_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(771 + PI_BASE_ADDR,DENALI_PI_771_DATA,0);
       #else
         REGWR(771 + PI_BASE_ADDR,DENALI_PI_771_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_772_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(772 + PI_BASE_ADDR,DENALI_PI_772_DATA,0);
       #else
         REGWR(772 + PI_BASE_ADDR,DENALI_PI_772_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_773_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(773 + PI_BASE_ADDR,DENALI_PI_773_DATA,0);
       #else
         REGWR(773 + PI_BASE_ADDR,DENALI_PI_773_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_774_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(774 + PI_BASE_ADDR,DENALI_PI_774_DATA,0);
       #else
         REGWR(774 + PI_BASE_ADDR,DENALI_PI_774_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_775_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(775 + PI_BASE_ADDR,DENALI_PI_775_DATA,0);
       #else
         REGWR(775 + PI_BASE_ADDR,DENALI_PI_775_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_776_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(776 + PI_BASE_ADDR,DENALI_PI_776_DATA,0);
       #else
         REGWR(776 + PI_BASE_ADDR,DENALI_PI_776_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_777_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(777 + PI_BASE_ADDR,DENALI_PI_777_DATA,0);
       #else
         REGWR(777 + PI_BASE_ADDR,DENALI_PI_777_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_778_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(778 + PI_BASE_ADDR,DENALI_PI_778_DATA,0);
       #else
         REGWR(778 + PI_BASE_ADDR,DENALI_PI_778_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_779_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(779 + PI_BASE_ADDR,DENALI_PI_779_DATA,0);
       #else
         REGWR(779 + PI_BASE_ADDR,DENALI_PI_779_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_780_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(780 + PI_BASE_ADDR,DENALI_PI_780_DATA,0);
       #else
         REGWR(780 + PI_BASE_ADDR,DENALI_PI_780_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_781_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(781 + PI_BASE_ADDR,DENALI_PI_781_DATA,0);
       #else
         REGWR(781 + PI_BASE_ADDR,DENALI_PI_781_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_782_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(782 + PI_BASE_ADDR,DENALI_PI_782_DATA,0);
       #else
         REGWR(782 + PI_BASE_ADDR,DENALI_PI_782_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_783_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(783 + PI_BASE_ADDR,DENALI_PI_783_DATA,0);
       #else
         REGWR(783 + PI_BASE_ADDR,DENALI_PI_783_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_784_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(784 + PI_BASE_ADDR,DENALI_PI_784_DATA,0);
       #else
         REGWR(784 + PI_BASE_ADDR,DENALI_PI_784_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_785_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(785 + PI_BASE_ADDR,DENALI_PI_785_DATA,0);
       #else
         REGWR(785 + PI_BASE_ADDR,DENALI_PI_785_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_786_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(786 + PI_BASE_ADDR,DENALI_PI_786_DATA,0);
       #else
         REGWR(786 + PI_BASE_ADDR,DENALI_PI_786_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_787_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(787 + PI_BASE_ADDR,DENALI_PI_787_DATA,0);
       #else
         REGWR(787 + PI_BASE_ADDR,DENALI_PI_787_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_788_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(788 + PI_BASE_ADDR,DENALI_PI_788_DATA,0);
       #else
         REGWR(788 + PI_BASE_ADDR,DENALI_PI_788_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_789_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(789 + PI_BASE_ADDR,DENALI_PI_789_DATA,0);
       #else
         REGWR(789 + PI_BASE_ADDR,DENALI_PI_789_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_790_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(790 + PI_BASE_ADDR,DENALI_PI_790_DATA,0);
       #else
         REGWR(790 + PI_BASE_ADDR,DENALI_PI_790_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_791_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(791 + PI_BASE_ADDR,DENALI_PI_791_DATA,0);
       #else
         REGWR(791 + PI_BASE_ADDR,DENALI_PI_791_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_792_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(792 + PI_BASE_ADDR,DENALI_PI_792_DATA,0);
       #else
         REGWR(792 + PI_BASE_ADDR,DENALI_PI_792_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_793_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(793 + PI_BASE_ADDR,DENALI_PI_793_DATA,0);
       #else
         REGWR(793 + PI_BASE_ADDR,DENALI_PI_793_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_794_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(794 + PI_BASE_ADDR,DENALI_PI_794_DATA,0);
       #else
         REGWR(794 + PI_BASE_ADDR,DENALI_PI_794_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_795_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(795 + PI_BASE_ADDR,DENALI_PI_795_DATA,0);
       #else
         REGWR(795 + PI_BASE_ADDR,DENALI_PI_795_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_796_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(796 + PI_BASE_ADDR,DENALI_PI_796_DATA,0);
       #else
         REGWR(796 + PI_BASE_ADDR,DENALI_PI_796_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_797_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(797 + PI_BASE_ADDR,DENALI_PI_797_DATA,0);
       #else
         REGWR(797 + PI_BASE_ADDR,DENALI_PI_797_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_798_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(798 + PI_BASE_ADDR,DENALI_PI_798_DATA,0);
       #else
         REGWR(798 + PI_BASE_ADDR,DENALI_PI_798_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_799_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(799 + PI_BASE_ADDR,DENALI_PI_799_DATA,0);
       #else
         REGWR(799 + PI_BASE_ADDR,DENALI_PI_799_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_800_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(800 + PI_BASE_ADDR,DENALI_PI_800_DATA,0);
       #else
         REGWR(800 + PI_BASE_ADDR,DENALI_PI_800_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_801_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(801 + PI_BASE_ADDR,DENALI_PI_801_DATA,0);
       #else
         REGWR(801 + PI_BASE_ADDR,DENALI_PI_801_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_802_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(802 + PI_BASE_ADDR,DENALI_PI_802_DATA,0);
       #else
         REGWR(802 + PI_BASE_ADDR,DENALI_PI_802_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_803_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(803 + PI_BASE_ADDR,DENALI_PI_803_DATA,0);
       #else
         REGWR(803 + PI_BASE_ADDR,DENALI_PI_803_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_804_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(804 + PI_BASE_ADDR,DENALI_PI_804_DATA,0);
       #else
         REGWR(804 + PI_BASE_ADDR,DENALI_PI_804_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_805_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(805 + PI_BASE_ADDR,DENALI_PI_805_DATA,0);
       #else
         REGWR(805 + PI_BASE_ADDR,DENALI_PI_805_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_806_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(806 + PI_BASE_ADDR,DENALI_PI_806_DATA,0);
       #else
         REGWR(806 + PI_BASE_ADDR,DENALI_PI_806_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_807_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(807 + PI_BASE_ADDR,DENALI_PI_807_DATA,0);
       #else
         REGWR(807 + PI_BASE_ADDR,DENALI_PI_807_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_808_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(808 + PI_BASE_ADDR,DENALI_PI_808_DATA,0);
       #else
         REGWR(808 + PI_BASE_ADDR,DENALI_PI_808_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_809_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(809 + PI_BASE_ADDR,DENALI_PI_809_DATA,0);
       #else
         REGWR(809 + PI_BASE_ADDR,DENALI_PI_809_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_810_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(810 + PI_BASE_ADDR,DENALI_PI_810_DATA,0);
       #else
         REGWR(810 + PI_BASE_ADDR,DENALI_PI_810_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_811_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(811 + PI_BASE_ADDR,DENALI_PI_811_DATA,0);
       #else
         REGWR(811 + PI_BASE_ADDR,DENALI_PI_811_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_812_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(812 + PI_BASE_ADDR,DENALI_PI_812_DATA,0);
       #else
         REGWR(812 + PI_BASE_ADDR,DENALI_PI_812_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_813_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(813 + PI_BASE_ADDR,DENALI_PI_813_DATA,0);
       #else
         REGWR(813 + PI_BASE_ADDR,DENALI_PI_813_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_814_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(814 + PI_BASE_ADDR,DENALI_PI_814_DATA,0);
       #else
         REGWR(814 + PI_BASE_ADDR,DENALI_PI_814_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_815_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(815 + PI_BASE_ADDR,DENALI_PI_815_DATA,0);
       #else
         REGWR(815 + PI_BASE_ADDR,DENALI_PI_815_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_816_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(816 + PI_BASE_ADDR,DENALI_PI_816_DATA,0);
       #else
         REGWR(816 + PI_BASE_ADDR,DENALI_PI_816_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_817_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(817 + PI_BASE_ADDR,DENALI_PI_817_DATA,0);
       #else
         REGWR(817 + PI_BASE_ADDR,DENALI_PI_817_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_818_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(818 + PI_BASE_ADDR,DENALI_PI_818_DATA,0);
       #else
         REGWR(818 + PI_BASE_ADDR,DENALI_PI_818_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_819_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(819 + PI_BASE_ADDR,DENALI_PI_819_DATA,0);
       #else
         REGWR(819 + PI_BASE_ADDR,DENALI_PI_819_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_820_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(820 + PI_BASE_ADDR,DENALI_PI_820_DATA,0);
       #else
         REGWR(820 + PI_BASE_ADDR,DENALI_PI_820_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_821_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(821 + PI_BASE_ADDR,DENALI_PI_821_DATA,0);
       #else
         REGWR(821 + PI_BASE_ADDR,DENALI_PI_821_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_822_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(822 + PI_BASE_ADDR,DENALI_PI_822_DATA,0);
       #else
         REGWR(822 + PI_BASE_ADDR,DENALI_PI_822_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_823_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(823 + PI_BASE_ADDR,DENALI_PI_823_DATA,0);
       #else
         REGWR(823 + PI_BASE_ADDR,DENALI_PI_823_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_824_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(824 + PI_BASE_ADDR,DENALI_PI_824_DATA,0);
       #else
         REGWR(824 + PI_BASE_ADDR,DENALI_PI_824_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_825_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(825 + PI_BASE_ADDR,DENALI_PI_825_DATA,0);
       #else
         REGWR(825 + PI_BASE_ADDR,DENALI_PI_825_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_826_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(826 + PI_BASE_ADDR,DENALI_PI_826_DATA,0);
       #else
         REGWR(826 + PI_BASE_ADDR,DENALI_PI_826_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_827_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(827 + PI_BASE_ADDR,DENALI_PI_827_DATA,0);
       #else
         REGWR(827 + PI_BASE_ADDR,DENALI_PI_827_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_828_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(828 + PI_BASE_ADDR,DENALI_PI_828_DATA,0);
       #else
         REGWR(828 + PI_BASE_ADDR,DENALI_PI_828_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_829_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(829 + PI_BASE_ADDR,DENALI_PI_829_DATA,0);
       #else
         REGWR(829 + PI_BASE_ADDR,DENALI_PI_829_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_830_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(830 + PI_BASE_ADDR,DENALI_PI_830_DATA,0);
       #else
         REGWR(830 + PI_BASE_ADDR,DENALI_PI_830_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_831_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(831 + PI_BASE_ADDR,DENALI_PI_831_DATA,0);
       #else
         REGWR(831 + PI_BASE_ADDR,DENALI_PI_831_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_832_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(832 + PI_BASE_ADDR,DENALI_PI_832_DATA,0);
       #else
         REGWR(832 + PI_BASE_ADDR,DENALI_PI_832_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_833_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(833 + PI_BASE_ADDR,DENALI_PI_833_DATA,0);
       #else
         REGWR(833 + PI_BASE_ADDR,DENALI_PI_833_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_834_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(834 + PI_BASE_ADDR,DENALI_PI_834_DATA,0);
       #else
         REGWR(834 + PI_BASE_ADDR,DENALI_PI_834_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_835_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(835 + PI_BASE_ADDR,DENALI_PI_835_DATA,0);
       #else
         REGWR(835 + PI_BASE_ADDR,DENALI_PI_835_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_836_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(836 + PI_BASE_ADDR,DENALI_PI_836_DATA,0);
       #else
         REGWR(836 + PI_BASE_ADDR,DENALI_PI_836_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_837_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(837 + PI_BASE_ADDR,DENALI_PI_837_DATA,0);
       #else
         REGWR(837 + PI_BASE_ADDR,DENALI_PI_837_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_838_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(838 + PI_BASE_ADDR,DENALI_PI_838_DATA,0);
       #else
         REGWR(838 + PI_BASE_ADDR,DENALI_PI_838_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_839_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(839 + PI_BASE_ADDR,DENALI_PI_839_DATA,0);
       #else
         REGWR(839 + PI_BASE_ADDR,DENALI_PI_839_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_840_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(840 + PI_BASE_ADDR,DENALI_PI_840_DATA,0);
       #else
         REGWR(840 + PI_BASE_ADDR,DENALI_PI_840_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_841_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(841 + PI_BASE_ADDR,DENALI_PI_841_DATA,0);
       #else
         REGWR(841 + PI_BASE_ADDR,DENALI_PI_841_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_842_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(842 + PI_BASE_ADDR,DENALI_PI_842_DATA,0);
       #else
         REGWR(842 + PI_BASE_ADDR,DENALI_PI_842_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_843_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(843 + PI_BASE_ADDR,DENALI_PI_843_DATA,0);
       #else
         REGWR(843 + PI_BASE_ADDR,DENALI_PI_843_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_844_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(844 + PI_BASE_ADDR,DENALI_PI_844_DATA,0);
       #else
         REGWR(844 + PI_BASE_ADDR,DENALI_PI_844_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_845_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(845 + PI_BASE_ADDR,DENALI_PI_845_DATA,0);
       #else
         REGWR(845 + PI_BASE_ADDR,DENALI_PI_845_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_846_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(846 + PI_BASE_ADDR,DENALI_PI_846_DATA,0);
       #else
         REGWR(846 + PI_BASE_ADDR,DENALI_PI_846_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_847_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(847 + PI_BASE_ADDR,DENALI_PI_847_DATA,0);
       #else
         REGWR(847 + PI_BASE_ADDR,DENALI_PI_847_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_848_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(848 + PI_BASE_ADDR,DENALI_PI_848_DATA,0);
       #else
         REGWR(848 + PI_BASE_ADDR,DENALI_PI_848_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_849_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(849 + PI_BASE_ADDR,DENALI_PI_849_DATA,0);
       #else
         REGWR(849 + PI_BASE_ADDR,DENALI_PI_849_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_850_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(850 + PI_BASE_ADDR,DENALI_PI_850_DATA,0);
       #else
         REGWR(850 + PI_BASE_ADDR,DENALI_PI_850_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_851_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(851 + PI_BASE_ADDR,DENALI_PI_851_DATA,0);
       #else
         REGWR(851 + PI_BASE_ADDR,DENALI_PI_851_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_852_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(852 + PI_BASE_ADDR,DENALI_PI_852_DATA,0);
       #else
         REGWR(852 + PI_BASE_ADDR,DENALI_PI_852_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_853_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(853 + PI_BASE_ADDR,DENALI_PI_853_DATA,0);
       #else
         REGWR(853 + PI_BASE_ADDR,DENALI_PI_853_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_854_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(854 + PI_BASE_ADDR,DENALI_PI_854_DATA,0);
       #else
         REGWR(854 + PI_BASE_ADDR,DENALI_PI_854_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_855_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(855 + PI_BASE_ADDR,DENALI_PI_855_DATA,0);
       #else
         REGWR(855 + PI_BASE_ADDR,DENALI_PI_855_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_856_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(856 + PI_BASE_ADDR,DENALI_PI_856_DATA,0);
       #else
         REGWR(856 + PI_BASE_ADDR,DENALI_PI_856_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_857_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(857 + PI_BASE_ADDR,DENALI_PI_857_DATA,0);
       #else
         REGWR(857 + PI_BASE_ADDR,DENALI_PI_857_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_858_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(858 + PI_BASE_ADDR,DENALI_PI_858_DATA,0);
       #else
         REGWR(858 + PI_BASE_ADDR,DENALI_PI_858_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_859_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(859 + PI_BASE_ADDR,DENALI_PI_859_DATA,0);
       #else
         REGWR(859 + PI_BASE_ADDR,DENALI_PI_859_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_860_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(860 + PI_BASE_ADDR,DENALI_PI_860_DATA,0);
       #else
         REGWR(860 + PI_BASE_ADDR,DENALI_PI_860_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_861_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(861 + PI_BASE_ADDR,DENALI_PI_861_DATA,0);
       #else
         REGWR(861 + PI_BASE_ADDR,DENALI_PI_861_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_862_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(862 + PI_BASE_ADDR,DENALI_PI_862_DATA,0);
       #else
         REGWR(862 + PI_BASE_ADDR,DENALI_PI_862_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_863_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(863 + PI_BASE_ADDR,DENALI_PI_863_DATA,0);
       #else
         REGWR(863 + PI_BASE_ADDR,DENALI_PI_863_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_864_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(864 + PI_BASE_ADDR,DENALI_PI_864_DATA,0);
       #else
         REGWR(864 + PI_BASE_ADDR,DENALI_PI_864_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_865_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(865 + PI_BASE_ADDR,DENALI_PI_865_DATA,0);
       #else
         REGWR(865 + PI_BASE_ADDR,DENALI_PI_865_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_866_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(866 + PI_BASE_ADDR,DENALI_PI_866_DATA,0);
       #else
         REGWR(866 + PI_BASE_ADDR,DENALI_PI_866_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_867_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(867 + PI_BASE_ADDR,DENALI_PI_867_DATA,0);
       #else
         REGWR(867 + PI_BASE_ADDR,DENALI_PI_867_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_868_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(868 + PI_BASE_ADDR,DENALI_PI_868_DATA,0);
       #else
         REGWR(868 + PI_BASE_ADDR,DENALI_PI_868_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_869_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(869 + PI_BASE_ADDR,DENALI_PI_869_DATA,0);
       #else
         REGWR(869 + PI_BASE_ADDR,DENALI_PI_869_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_870_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(870 + PI_BASE_ADDR,DENALI_PI_870_DATA,0);
       #else
         REGWR(870 + PI_BASE_ADDR,DENALI_PI_870_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_871_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(871 + PI_BASE_ADDR,DENALI_PI_871_DATA,0);
       #else
         REGWR(871 + PI_BASE_ADDR,DENALI_PI_871_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_872_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(872 + PI_BASE_ADDR,DENALI_PI_872_DATA,0);
       #else
         REGWR(872 + PI_BASE_ADDR,DENALI_PI_872_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_873_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(873 + PI_BASE_ADDR,DENALI_PI_873_DATA,0);
       #else
         REGWR(873 + PI_BASE_ADDR,DENALI_PI_873_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_874_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(874 + PI_BASE_ADDR,DENALI_PI_874_DATA,0);
       #else
         REGWR(874 + PI_BASE_ADDR,DENALI_PI_874_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_875_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(875 + PI_BASE_ADDR,DENALI_PI_875_DATA,0);
       #else
         REGWR(875 + PI_BASE_ADDR,DENALI_PI_875_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_876_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(876 + PI_BASE_ADDR,DENALI_PI_876_DATA,0);
       #else
         REGWR(876 + PI_BASE_ADDR,DENALI_PI_876_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_877_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(877 + PI_BASE_ADDR,DENALI_PI_877_DATA,0);
       #else
         REGWR(877 + PI_BASE_ADDR,DENALI_PI_877_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_878_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(878 + PI_BASE_ADDR,DENALI_PI_878_DATA,0);
       #else
         REGWR(878 + PI_BASE_ADDR,DENALI_PI_878_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_879_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(879 + PI_BASE_ADDR,DENALI_PI_879_DATA,0);
       #else
         REGWR(879 + PI_BASE_ADDR,DENALI_PI_879_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_880_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(880 + PI_BASE_ADDR,DENALI_PI_880_DATA,0);
       #else
         REGWR(880 + PI_BASE_ADDR,DENALI_PI_880_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_881_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(881 + PI_BASE_ADDR,DENALI_PI_881_DATA,0);
       #else
         REGWR(881 + PI_BASE_ADDR,DENALI_PI_881_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_882_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(882 + PI_BASE_ADDR,DENALI_PI_882_DATA,0);
       #else
         REGWR(882 + PI_BASE_ADDR,DENALI_PI_882_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_883_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(883 + PI_BASE_ADDR,DENALI_PI_883_DATA,0);
       #else
         REGWR(883 + PI_BASE_ADDR,DENALI_PI_883_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_884_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(884 + PI_BASE_ADDR,DENALI_PI_884_DATA,0);
       #else
         REGWR(884 + PI_BASE_ADDR,DENALI_PI_884_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_885_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(885 + PI_BASE_ADDR,DENALI_PI_885_DATA,0);
       #else
         REGWR(885 + PI_BASE_ADDR,DENALI_PI_885_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_886_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(886 + PI_BASE_ADDR,DENALI_PI_886_DATA,0);
       #else
         REGWR(886 + PI_BASE_ADDR,DENALI_PI_886_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_887_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(887 + PI_BASE_ADDR,DENALI_PI_887_DATA,0);
       #else
         REGWR(887 + PI_BASE_ADDR,DENALI_PI_887_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_888_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(888 + PI_BASE_ADDR,DENALI_PI_888_DATA,0);
       #else
         REGWR(888 + PI_BASE_ADDR,DENALI_PI_888_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_889_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(889 + PI_BASE_ADDR,DENALI_PI_889_DATA,0);
       #else
         REGWR(889 + PI_BASE_ADDR,DENALI_PI_889_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_890_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(890 + PI_BASE_ADDR,DENALI_PI_890_DATA,0);
       #else
         REGWR(890 + PI_BASE_ADDR,DENALI_PI_890_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_891_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(891 + PI_BASE_ADDR,DENALI_PI_891_DATA,0);
       #else
         REGWR(891 + PI_BASE_ADDR,DENALI_PI_891_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_892_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(892 + PI_BASE_ADDR,DENALI_PI_892_DATA,0);
       #else
         REGWR(892 + PI_BASE_ADDR,DENALI_PI_892_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_893_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(893 + PI_BASE_ADDR,DENALI_PI_893_DATA,0);
       #else
         REGWR(893 + PI_BASE_ADDR,DENALI_PI_893_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_894_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(894 + PI_BASE_ADDR,DENALI_PI_894_DATA,0);
       #else
         REGWR(894 + PI_BASE_ADDR,DENALI_PI_894_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_895_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(895 + PI_BASE_ADDR,DENALI_PI_895_DATA,0);
       #else
         REGWR(895 + PI_BASE_ADDR,DENALI_PI_895_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_896_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(896 + PI_BASE_ADDR,DENALI_PI_896_DATA,0);
       #else
         REGWR(896 + PI_BASE_ADDR,DENALI_PI_896_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_897_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(897 + PI_BASE_ADDR,DENALI_PI_897_DATA,0);
       #else
         REGWR(897 + PI_BASE_ADDR,DENALI_PI_897_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_898_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(898 + PI_BASE_ADDR,DENALI_PI_898_DATA,0);
       #else
         REGWR(898 + PI_BASE_ADDR,DENALI_PI_898_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_899_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(899 + PI_BASE_ADDR,DENALI_PI_899_DATA,0);
       #else
         REGWR(899 + PI_BASE_ADDR,DENALI_PI_899_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_900_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(900 + PI_BASE_ADDR,DENALI_PI_900_DATA,0);
       #else
         REGWR(900 + PI_BASE_ADDR,DENALI_PI_900_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_901_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(901 + PI_BASE_ADDR,DENALI_PI_901_DATA,0);
       #else
         REGWR(901 + PI_BASE_ADDR,DENALI_PI_901_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_902_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(902 + PI_BASE_ADDR,DENALI_PI_902_DATA,0);
       #else
         REGWR(902 + PI_BASE_ADDR,DENALI_PI_902_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_903_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(903 + PI_BASE_ADDR,DENALI_PI_903_DATA,0);
       #else
         REGWR(903 + PI_BASE_ADDR,DENALI_PI_903_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_904_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(904 + PI_BASE_ADDR,DENALI_PI_904_DATA,0);
       #else
         REGWR(904 + PI_BASE_ADDR,DENALI_PI_904_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_905_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(905 + PI_BASE_ADDR,DENALI_PI_905_DATA,0);
       #else
         REGWR(905 + PI_BASE_ADDR,DENALI_PI_905_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_906_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(906 + PI_BASE_ADDR,DENALI_PI_906_DATA,0);
       #else
         REGWR(906 + PI_BASE_ADDR,DENALI_PI_906_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_907_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(907 + PI_BASE_ADDR,DENALI_PI_907_DATA,0);
       #else
         REGWR(907 + PI_BASE_ADDR,DENALI_PI_907_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_908_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(908 + PI_BASE_ADDR,DENALI_PI_908_DATA,0);
       #else
         REGWR(908 + PI_BASE_ADDR,DENALI_PI_908_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_909_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(909 + PI_BASE_ADDR,DENALI_PI_909_DATA,0);
       #else
         REGWR(909 + PI_BASE_ADDR,DENALI_PI_909_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_910_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(910 + PI_BASE_ADDR,DENALI_PI_910_DATA,0);
       #else
         REGWR(910 + PI_BASE_ADDR,DENALI_PI_910_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_911_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(911 + PI_BASE_ADDR,DENALI_PI_911_DATA,0);
       #else
         REGWR(911 + PI_BASE_ADDR,DENALI_PI_911_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_912_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(912 + PI_BASE_ADDR,DENALI_PI_912_DATA,0);
       #else
         REGWR(912 + PI_BASE_ADDR,DENALI_PI_912_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_913_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(913 + PI_BASE_ADDR,DENALI_PI_913_DATA,0);
       #else
         REGWR(913 + PI_BASE_ADDR,DENALI_PI_913_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_914_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(914 + PI_BASE_ADDR,DENALI_PI_914_DATA,0);
       #else
         REGWR(914 + PI_BASE_ADDR,DENALI_PI_914_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_915_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(915 + PI_BASE_ADDR,DENALI_PI_915_DATA,0);
       #else
         REGWR(915 + PI_BASE_ADDR,DENALI_PI_915_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_916_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(916 + PI_BASE_ADDR,DENALI_PI_916_DATA,0);
       #else
         REGWR(916 + PI_BASE_ADDR,DENALI_PI_916_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_917_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(917 + PI_BASE_ADDR,DENALI_PI_917_DATA,0);
       #else
         REGWR(917 + PI_BASE_ADDR,DENALI_PI_917_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_918_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(918 + PI_BASE_ADDR,DENALI_PI_918_DATA,0);
       #else
         REGWR(918 + PI_BASE_ADDR,DENALI_PI_918_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_919_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(919 + PI_BASE_ADDR,DENALI_PI_919_DATA,0);
       #else
         REGWR(919 + PI_BASE_ADDR,DENALI_PI_919_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_920_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(920 + PI_BASE_ADDR,DENALI_PI_920_DATA,0);
       #else
         REGWR(920 + PI_BASE_ADDR,DENALI_PI_920_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_921_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(921 + PI_BASE_ADDR,DENALI_PI_921_DATA,0);
       #else
         REGWR(921 + PI_BASE_ADDR,DENALI_PI_921_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_922_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(922 + PI_BASE_ADDR,DENALI_PI_922_DATA,0);
       #else
         REGWR(922 + PI_BASE_ADDR,DENALI_PI_922_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_923_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(923 + PI_BASE_ADDR,DENALI_PI_923_DATA,0);
       #else
         REGWR(923 + PI_BASE_ADDR,DENALI_PI_923_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_924_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(924 + PI_BASE_ADDR,DENALI_PI_924_DATA,0);
       #else
         REGWR(924 + PI_BASE_ADDR,DENALI_PI_924_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_925_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(925 + PI_BASE_ADDR,DENALI_PI_925_DATA,0);
       #else
         REGWR(925 + PI_BASE_ADDR,DENALI_PI_925_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_926_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(926 + PI_BASE_ADDR,DENALI_PI_926_DATA,0);
       #else
         REGWR(926 + PI_BASE_ADDR,DENALI_PI_926_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_927_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(927 + PI_BASE_ADDR,DENALI_PI_927_DATA,0);
       #else
         REGWR(927 + PI_BASE_ADDR,DENALI_PI_927_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_928_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(928 + PI_BASE_ADDR,DENALI_PI_928_DATA,0);
       #else
         REGWR(928 + PI_BASE_ADDR,DENALI_PI_928_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_929_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(929 + PI_BASE_ADDR,DENALI_PI_929_DATA,0);
       #else
         REGWR(929 + PI_BASE_ADDR,DENALI_PI_929_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_930_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(930 + PI_BASE_ADDR,DENALI_PI_930_DATA,0);
       #else
         REGWR(930 + PI_BASE_ADDR,DENALI_PI_930_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_931_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(931 + PI_BASE_ADDR,DENALI_PI_931_DATA,0);
       #else
         REGWR(931 + PI_BASE_ADDR,DENALI_PI_931_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_932_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(932 + PI_BASE_ADDR,DENALI_PI_932_DATA,0);
       #else
         REGWR(932 + PI_BASE_ADDR,DENALI_PI_932_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_933_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(933 + PI_BASE_ADDR,DENALI_PI_933_DATA,0);
       #else
         REGWR(933 + PI_BASE_ADDR,DENALI_PI_933_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_934_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(934 + PI_BASE_ADDR,DENALI_PI_934_DATA,0);
       #else
         REGWR(934 + PI_BASE_ADDR,DENALI_PI_934_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_935_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(935 + PI_BASE_ADDR,DENALI_PI_935_DATA,0);
       #else
         REGWR(935 + PI_BASE_ADDR,DENALI_PI_935_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_936_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(936 + PI_BASE_ADDR,DENALI_PI_936_DATA,0);
       #else
         REGWR(936 + PI_BASE_ADDR,DENALI_PI_936_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_937_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(937 + PI_BASE_ADDR,DENALI_PI_937_DATA,0);
       #else
         REGWR(937 + PI_BASE_ADDR,DENALI_PI_937_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_938_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(938 + PI_BASE_ADDR,DENALI_PI_938_DATA,0);
       #else
         REGWR(938 + PI_BASE_ADDR,DENALI_PI_938_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_939_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(939 + PI_BASE_ADDR,DENALI_PI_939_DATA,0);
       #else
         REGWR(939 + PI_BASE_ADDR,DENALI_PI_939_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_940_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(940 + PI_BASE_ADDR,DENALI_PI_940_DATA,0);
       #else
         REGWR(940 + PI_BASE_ADDR,DENALI_PI_940_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_941_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(941 + PI_BASE_ADDR,DENALI_PI_941_DATA,0);
       #else
         REGWR(941 + PI_BASE_ADDR,DENALI_PI_941_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_942_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(942 + PI_BASE_ADDR,DENALI_PI_942_DATA,0);
       #else
         REGWR(942 + PI_BASE_ADDR,DENALI_PI_942_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_943_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(943 + PI_BASE_ADDR,DENALI_PI_943_DATA,0);
       #else
         REGWR(943 + PI_BASE_ADDR,DENALI_PI_943_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_944_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(944 + PI_BASE_ADDR,DENALI_PI_944_DATA,0);
       #else
         REGWR(944 + PI_BASE_ADDR,DENALI_PI_944_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_945_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(945 + PI_BASE_ADDR,DENALI_PI_945_DATA,0);
       #else
         REGWR(945 + PI_BASE_ADDR,DENALI_PI_945_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_946_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(946 + PI_BASE_ADDR,DENALI_PI_946_DATA,0);
       #else
         REGWR(946 + PI_BASE_ADDR,DENALI_PI_946_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_947_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(947 + PI_BASE_ADDR,DENALI_PI_947_DATA,0);
       #else
         REGWR(947 + PI_BASE_ADDR,DENALI_PI_947_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_948_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(948 + PI_BASE_ADDR,DENALI_PI_948_DATA,0);
       #else
         REGWR(948 + PI_BASE_ADDR,DENALI_PI_948_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_949_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(949 + PI_BASE_ADDR,DENALI_PI_949_DATA,0);
       #else
         REGWR(949 + PI_BASE_ADDR,DENALI_PI_949_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_950_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(950 + PI_BASE_ADDR,DENALI_PI_950_DATA,0);
       #else
         REGWR(950 + PI_BASE_ADDR,DENALI_PI_950_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_951_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(951 + PI_BASE_ADDR,DENALI_PI_951_DATA,0);
       #else
         REGWR(951 + PI_BASE_ADDR,DENALI_PI_951_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_952_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(952 + PI_BASE_ADDR,DENALI_PI_952_DATA,0);
       #else
         REGWR(952 + PI_BASE_ADDR,DENALI_PI_952_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_953_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(953 + PI_BASE_ADDR,DENALI_PI_953_DATA,0);
       #else
         REGWR(953 + PI_BASE_ADDR,DENALI_PI_953_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_954_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(954 + PI_BASE_ADDR,DENALI_PI_954_DATA,0);
       #else
         REGWR(954 + PI_BASE_ADDR,DENALI_PI_954_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_955_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(955 + PI_BASE_ADDR,DENALI_PI_955_DATA,0);
       #else
         REGWR(955 + PI_BASE_ADDR,DENALI_PI_955_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_956_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(956 + PI_BASE_ADDR,DENALI_PI_956_DATA,0);
       #else
         REGWR(956 + PI_BASE_ADDR,DENALI_PI_956_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_957_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(957 + PI_BASE_ADDR,DENALI_PI_957_DATA,0);
       #else
         REGWR(957 + PI_BASE_ADDR,DENALI_PI_957_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_958_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(958 + PI_BASE_ADDR,DENALI_PI_958_DATA,0);
       #else
         REGWR(958 + PI_BASE_ADDR,DENALI_PI_958_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_959_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(959 + PI_BASE_ADDR,DENALI_PI_959_DATA,0);
       #else
         REGWR(959 + PI_BASE_ADDR,DENALI_PI_959_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_960_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(960 + PI_BASE_ADDR,DENALI_PI_960_DATA,0);
       #else
         REGWR(960 + PI_BASE_ADDR,DENALI_PI_960_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_961_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(961 + PI_BASE_ADDR,DENALI_PI_961_DATA,0);
       #else
         REGWR(961 + PI_BASE_ADDR,DENALI_PI_961_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_962_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(962 + PI_BASE_ADDR,DENALI_PI_962_DATA,0);
       #else
         REGWR(962 + PI_BASE_ADDR,DENALI_PI_962_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_963_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(963 + PI_BASE_ADDR,DENALI_PI_963_DATA,0);
       #else
         REGWR(963 + PI_BASE_ADDR,DENALI_PI_963_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_964_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(964 + PI_BASE_ADDR,DENALI_PI_964_DATA,0);
       #else
         REGWR(964 + PI_BASE_ADDR,DENALI_PI_964_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_965_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(965 + PI_BASE_ADDR,DENALI_PI_965_DATA,0);
       #else
         REGWR(965 + PI_BASE_ADDR,DENALI_PI_965_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_966_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(966 + PI_BASE_ADDR,DENALI_PI_966_DATA,0);
       #else
         REGWR(966 + PI_BASE_ADDR,DENALI_PI_966_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_967_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(967 + PI_BASE_ADDR,DENALI_PI_967_DATA,0);
       #else
         REGWR(967 + PI_BASE_ADDR,DENALI_PI_967_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_968_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(968 + PI_BASE_ADDR,DENALI_PI_968_DATA,0);
       #else
         REGWR(968 + PI_BASE_ADDR,DENALI_PI_968_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_969_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(969 + PI_BASE_ADDR,DENALI_PI_969_DATA,0);
       #else
         REGWR(969 + PI_BASE_ADDR,DENALI_PI_969_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_970_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(970 + PI_BASE_ADDR,DENALI_PI_970_DATA,0);
       #else
         REGWR(970 + PI_BASE_ADDR,DENALI_PI_970_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_971_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(971 + PI_BASE_ADDR,DENALI_PI_971_DATA,0);
       #else
         REGWR(971 + PI_BASE_ADDR,DENALI_PI_971_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_972_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(972 + PI_BASE_ADDR,DENALI_PI_972_DATA,0);
       #else
         REGWR(972 + PI_BASE_ADDR,DENALI_PI_972_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_973_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(973 + PI_BASE_ADDR,DENALI_PI_973_DATA,0);
       #else
         REGWR(973 + PI_BASE_ADDR,DENALI_PI_973_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_974_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(974 + PI_BASE_ADDR,DENALI_PI_974_DATA,0);
       #else
         REGWR(974 + PI_BASE_ADDR,DENALI_PI_974_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_975_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(975 + PI_BASE_ADDR,DENALI_PI_975_DATA,0);
       #else
         REGWR(975 + PI_BASE_ADDR,DENALI_PI_975_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_976_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(976 + PI_BASE_ADDR,DENALI_PI_976_DATA,0);
       #else
         REGWR(976 + PI_BASE_ADDR,DENALI_PI_976_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_977_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(977 + PI_BASE_ADDR,DENALI_PI_977_DATA,0);
       #else
         REGWR(977 + PI_BASE_ADDR,DENALI_PI_977_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_978_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(978 + PI_BASE_ADDR,DENALI_PI_978_DATA,0);
       #else
         REGWR(978 + PI_BASE_ADDR,DENALI_PI_978_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_979_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(979 + PI_BASE_ADDR,DENALI_PI_979_DATA,0);
       #else
         REGWR(979 + PI_BASE_ADDR,DENALI_PI_979_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_980_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(980 + PI_BASE_ADDR,DENALI_PI_980_DATA,0);
       #else
         REGWR(980 + PI_BASE_ADDR,DENALI_PI_980_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_981_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(981 + PI_BASE_ADDR,DENALI_PI_981_DATA,0);
       #else
         REGWR(981 + PI_BASE_ADDR,DENALI_PI_981_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_982_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(982 + PI_BASE_ADDR,DENALI_PI_982_DATA,0);
       #else
         REGWR(982 + PI_BASE_ADDR,DENALI_PI_982_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_983_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(983 + PI_BASE_ADDR,DENALI_PI_983_DATA,0);
       #else
         REGWR(983 + PI_BASE_ADDR,DENALI_PI_983_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_984_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(984 + PI_BASE_ADDR,DENALI_PI_984_DATA,0);
       #else
         REGWR(984 + PI_BASE_ADDR,DENALI_PI_984_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_985_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(985 + PI_BASE_ADDR,DENALI_PI_985_DATA,0);
       #else
         REGWR(985 + PI_BASE_ADDR,DENALI_PI_985_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_986_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(986 + PI_BASE_ADDR,DENALI_PI_986_DATA,0);
       #else
         REGWR(986 + PI_BASE_ADDR,DENALI_PI_986_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_987_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(987 + PI_BASE_ADDR,DENALI_PI_987_DATA,0);
       #else
         REGWR(987 + PI_BASE_ADDR,DENALI_PI_987_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_988_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(988 + PI_BASE_ADDR,DENALI_PI_988_DATA,0);
       #else
         REGWR(988 + PI_BASE_ADDR,DENALI_PI_988_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_989_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(989 + PI_BASE_ADDR,DENALI_PI_989_DATA,0);
       #else
         REGWR(989 + PI_BASE_ADDR,DENALI_PI_989_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_990_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(990 + PI_BASE_ADDR,DENALI_PI_990_DATA,0);
       #else
         REGWR(990 + PI_BASE_ADDR,DENALI_PI_990_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_991_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(991 + PI_BASE_ADDR,DENALI_PI_991_DATA,0);
       #else
         REGWR(991 + PI_BASE_ADDR,DENALI_PI_991_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_992_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(992 + PI_BASE_ADDR,DENALI_PI_992_DATA,0);
       #else
         REGWR(992 + PI_BASE_ADDR,DENALI_PI_992_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_993_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(993 + PI_BASE_ADDR,DENALI_PI_993_DATA,0);
       #else
         REGWR(993 + PI_BASE_ADDR,DENALI_PI_993_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_994_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(994 + PI_BASE_ADDR,DENALI_PI_994_DATA,0);
       #else
         REGWR(994 + PI_BASE_ADDR,DENALI_PI_994_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_995_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(995 + PI_BASE_ADDR,DENALI_PI_995_DATA,0);
       #else
         REGWR(995 + PI_BASE_ADDR,DENALI_PI_995_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_996_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(996 + PI_BASE_ADDR,DENALI_PI_996_DATA,0);
       #else
         REGWR(996 + PI_BASE_ADDR,DENALI_PI_996_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_997_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(997 + PI_BASE_ADDR,DENALI_PI_997_DATA,0);
       #else
         REGWR(997 + PI_BASE_ADDR,DENALI_PI_997_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_998_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(998 + PI_BASE_ADDR,DENALI_PI_998_DATA,0);
       #else
         REGWR(998 + PI_BASE_ADDR,DENALI_PI_998_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_999_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(999 + PI_BASE_ADDR,DENALI_PI_999_DATA,0);
       #else
         REGWR(999 + PI_BASE_ADDR,DENALI_PI_999_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1000_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1000 + PI_BASE_ADDR,DENALI_PI_1000_DATA,0);
       #else
         REGWR(1000 + PI_BASE_ADDR,DENALI_PI_1000_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1001_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1001 + PI_BASE_ADDR,DENALI_PI_1001_DATA,0);
       #else
         REGWR(1001 + PI_BASE_ADDR,DENALI_PI_1001_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1002_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1002 + PI_BASE_ADDR,DENALI_PI_1002_DATA,0);
       #else
         REGWR(1002 + PI_BASE_ADDR,DENALI_PI_1002_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1003_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1003 + PI_BASE_ADDR,DENALI_PI_1003_DATA,0);
       #else
         REGWR(1003 + PI_BASE_ADDR,DENALI_PI_1003_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1004_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1004 + PI_BASE_ADDR,DENALI_PI_1004_DATA,0);
       #else
         REGWR(1004 + PI_BASE_ADDR,DENALI_PI_1004_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1005_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1005 + PI_BASE_ADDR,DENALI_PI_1005_DATA,0);
       #else
         REGWR(1005 + PI_BASE_ADDR,DENALI_PI_1005_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1006_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1006 + PI_BASE_ADDR,DENALI_PI_1006_DATA,0);
       #else
         REGWR(1006 + PI_BASE_ADDR,DENALI_PI_1006_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1007_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1007 + PI_BASE_ADDR,DENALI_PI_1007_DATA,0);
       #else
         REGWR(1007 + PI_BASE_ADDR,DENALI_PI_1007_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1008_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1008 + PI_BASE_ADDR,DENALI_PI_1008_DATA,0);
       #else
         REGWR(1008 + PI_BASE_ADDR,DENALI_PI_1008_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1009_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1009 + PI_BASE_ADDR,DENALI_PI_1009_DATA,0);
       #else
         REGWR(1009 + PI_BASE_ADDR,DENALI_PI_1009_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1010_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1010 + PI_BASE_ADDR,DENALI_PI_1010_DATA,0);
       #else
         REGWR(1010 + PI_BASE_ADDR,DENALI_PI_1010_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1011_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1011 + PI_BASE_ADDR,DENALI_PI_1011_DATA,0);
       #else
         REGWR(1011 + PI_BASE_ADDR,DENALI_PI_1011_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1012_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1012 + PI_BASE_ADDR,DENALI_PI_1012_DATA,0);
       #else
         REGWR(1012 + PI_BASE_ADDR,DENALI_PI_1012_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1013_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1013 + PI_BASE_ADDR,DENALI_PI_1013_DATA,0);
       #else
         REGWR(1013 + PI_BASE_ADDR,DENALI_PI_1013_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1014_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1014 + PI_BASE_ADDR,DENALI_PI_1014_DATA,0);
       #else
         REGWR(1014 + PI_BASE_ADDR,DENALI_PI_1014_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1015_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1015 + PI_BASE_ADDR,DENALI_PI_1015_DATA,0);
       #else
         REGWR(1015 + PI_BASE_ADDR,DENALI_PI_1015_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1016_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1016 + PI_BASE_ADDR,DENALI_PI_1016_DATA,0);
       #else
         REGWR(1016 + PI_BASE_ADDR,DENALI_PI_1016_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1017_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1017 + PI_BASE_ADDR,DENALI_PI_1017_DATA,0);
       #else
         REGWR(1017 + PI_BASE_ADDR,DENALI_PI_1017_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1018_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1018 + PI_BASE_ADDR,DENALI_PI_1018_DATA,0);
       #else
         REGWR(1018 + PI_BASE_ADDR,DENALI_PI_1018_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1019_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1019 + PI_BASE_ADDR,DENALI_PI_1019_DATA,0);
       #else
         REGWR(1019 + PI_BASE_ADDR,DENALI_PI_1019_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1020_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1020 + PI_BASE_ADDR,DENALI_PI_1020_DATA,0);
       #else
         REGWR(1020 + PI_BASE_ADDR,DENALI_PI_1020_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1021_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1021 + PI_BASE_ADDR,DENALI_PI_1021_DATA,0);
       #else
         REGWR(1021 + PI_BASE_ADDR,DENALI_PI_1021_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1022_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1022 + PI_BASE_ADDR,DENALI_PI_1022_DATA,0);
       #else
         REGWR(1022 + PI_BASE_ADDR,DENALI_PI_1022_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1023_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1023 + PI_BASE_ADDR,DENALI_PI_1023_DATA,0);
       #else
         REGWR(1023 + PI_BASE_ADDR,DENALI_PI_1023_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1024_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1024 + PI_BASE_ADDR,DENALI_PI_1024_DATA,0);
       #else
         REGWR(1024 + PI_BASE_ADDR,DENALI_PI_1024_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1025_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1025 + PI_BASE_ADDR,DENALI_PI_1025_DATA,0);
       #else
         REGWR(1025 + PI_BASE_ADDR,DENALI_PI_1025_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1026_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1026 + PI_BASE_ADDR,DENALI_PI_1026_DATA,0);
       #else
         REGWR(1026 + PI_BASE_ADDR,DENALI_PI_1026_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1027_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1027 + PI_BASE_ADDR,DENALI_PI_1027_DATA,0);
       #else
         REGWR(1027 + PI_BASE_ADDR,DENALI_PI_1027_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1028_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1028 + PI_BASE_ADDR,DENALI_PI_1028_DATA,0);
       #else
         REGWR(1028 + PI_BASE_ADDR,DENALI_PI_1028_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1029_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1029 + PI_BASE_ADDR,DENALI_PI_1029_DATA,0);
       #else
         REGWR(1029 + PI_BASE_ADDR,DENALI_PI_1029_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1030_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1030 + PI_BASE_ADDR,DENALI_PI_1030_DATA,0);
       #else
         REGWR(1030 + PI_BASE_ADDR,DENALI_PI_1030_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1031_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1031 + PI_BASE_ADDR,DENALI_PI_1031_DATA,0);
       #else
         REGWR(1031 + PI_BASE_ADDR,DENALI_PI_1031_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1032_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1032 + PI_BASE_ADDR,DENALI_PI_1032_DATA,0);
       #else
         REGWR(1032 + PI_BASE_ADDR,DENALI_PI_1032_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1033_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1033 + PI_BASE_ADDR,DENALI_PI_1033_DATA,0);
       #else
         REGWR(1033 + PI_BASE_ADDR,DENALI_PI_1033_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1034_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1034 + PI_BASE_ADDR,DENALI_PI_1034_DATA,0);
       #else
         REGWR(1034 + PI_BASE_ADDR,DENALI_PI_1034_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1035_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1035 + PI_BASE_ADDR,DENALI_PI_1035_DATA,0);
       #else
         REGWR(1035 + PI_BASE_ADDR,DENALI_PI_1035_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1036_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1036 + PI_BASE_ADDR,DENALI_PI_1036_DATA,0);
       #else
         REGWR(1036 + PI_BASE_ADDR,DENALI_PI_1036_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1037_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1037 + PI_BASE_ADDR,DENALI_PI_1037_DATA,0);
       #else
         REGWR(1037 + PI_BASE_ADDR,DENALI_PI_1037_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1038_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1038 + PI_BASE_ADDR,DENALI_PI_1038_DATA,0);
       #else
         REGWR(1038 + PI_BASE_ADDR,DENALI_PI_1038_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1039_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1039 + PI_BASE_ADDR,DENALI_PI_1039_DATA,0);
       #else
         REGWR(1039 + PI_BASE_ADDR,DENALI_PI_1039_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1040_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1040 + PI_BASE_ADDR,DENALI_PI_1040_DATA,0);
       #else
         REGWR(1040 + PI_BASE_ADDR,DENALI_PI_1040_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1041_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1041 + PI_BASE_ADDR,DENALI_PI_1041_DATA,0);
       #else
         REGWR(1041 + PI_BASE_ADDR,DENALI_PI_1041_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1042_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1042 + PI_BASE_ADDR,DENALI_PI_1042_DATA,0);
       #else
         REGWR(1042 + PI_BASE_ADDR,DENALI_PI_1042_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1043_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1043 + PI_BASE_ADDR,DENALI_PI_1043_DATA,0);
       #else
         REGWR(1043 + PI_BASE_ADDR,DENALI_PI_1043_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1044_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1044 + PI_BASE_ADDR,DENALI_PI_1044_DATA,0);
       #else
         REGWR(1044 + PI_BASE_ADDR,DENALI_PI_1044_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1045_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1045 + PI_BASE_ADDR,DENALI_PI_1045_DATA,0);
       #else
         REGWR(1045 + PI_BASE_ADDR,DENALI_PI_1045_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1046_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1046 + PI_BASE_ADDR,DENALI_PI_1046_DATA,0);
       #else
         REGWR(1046 + PI_BASE_ADDR,DENALI_PI_1046_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1047_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1047 + PI_BASE_ADDR,DENALI_PI_1047_DATA,0);
       #else
         REGWR(1047 + PI_BASE_ADDR,DENALI_PI_1047_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1048_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1048 + PI_BASE_ADDR,DENALI_PI_1048_DATA,0);
       #else
         REGWR(1048 + PI_BASE_ADDR,DENALI_PI_1048_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1049_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1049 + PI_BASE_ADDR,DENALI_PI_1049_DATA,0);
       #else
         REGWR(1049 + PI_BASE_ADDR,DENALI_PI_1049_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1050_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1050 + PI_BASE_ADDR,DENALI_PI_1050_DATA,0);
       #else
         REGWR(1050 + PI_BASE_ADDR,DENALI_PI_1050_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1051_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1051 + PI_BASE_ADDR,DENALI_PI_1051_DATA,0);
       #else
         REGWR(1051 + PI_BASE_ADDR,DENALI_PI_1051_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1052_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1052 + PI_BASE_ADDR,DENALI_PI_1052_DATA,0);
       #else
         REGWR(1052 + PI_BASE_ADDR,DENALI_PI_1052_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1053_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1053 + PI_BASE_ADDR,DENALI_PI_1053_DATA,0);
       #else
         REGWR(1053 + PI_BASE_ADDR,DENALI_PI_1053_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1054_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1054 + PI_BASE_ADDR,DENALI_PI_1054_DATA,0);
       #else
         REGWR(1054 + PI_BASE_ADDR,DENALI_PI_1054_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1055_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1055 + PI_BASE_ADDR,DENALI_PI_1055_DATA,0);
       #else
         REGWR(1055 + PI_BASE_ADDR,DENALI_PI_1055_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1056_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1056 + PI_BASE_ADDR,DENALI_PI_1056_DATA,0);
       #else
         REGWR(1056 + PI_BASE_ADDR,DENALI_PI_1056_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1057_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1057 + PI_BASE_ADDR,DENALI_PI_1057_DATA,0);
       #else
         REGWR(1057 + PI_BASE_ADDR,DENALI_PI_1057_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1058_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1058 + PI_BASE_ADDR,DENALI_PI_1058_DATA,0);
       #else
         REGWR(1058 + PI_BASE_ADDR,DENALI_PI_1058_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1059_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1059 + PI_BASE_ADDR,DENALI_PI_1059_DATA,0);
       #else
         REGWR(1059 + PI_BASE_ADDR,DENALI_PI_1059_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1060_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1060 + PI_BASE_ADDR,DENALI_PI_1060_DATA,0);
       #else
         REGWR(1060 + PI_BASE_ADDR,DENALI_PI_1060_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1061_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1061 + PI_BASE_ADDR,DENALI_PI_1061_DATA,0);
       #else
         REGWR(1061 + PI_BASE_ADDR,DENALI_PI_1061_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1062_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1062 + PI_BASE_ADDR,DENALI_PI_1062_DATA,0);
       #else
         REGWR(1062 + PI_BASE_ADDR,DENALI_PI_1062_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1063_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1063 + PI_BASE_ADDR,DENALI_PI_1063_DATA,0);
       #else
         REGWR(1063 + PI_BASE_ADDR,DENALI_PI_1063_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1064_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1064 + PI_BASE_ADDR,DENALI_PI_1064_DATA,0);
       #else
         REGWR(1064 + PI_BASE_ADDR,DENALI_PI_1064_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1065_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1065 + PI_BASE_ADDR,DENALI_PI_1065_DATA,0);
       #else
         REGWR(1065 + PI_BASE_ADDR,DENALI_PI_1065_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1066_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1066 + PI_BASE_ADDR,DENALI_PI_1066_DATA,0);
       #else
         REGWR(1066 + PI_BASE_ADDR,DENALI_PI_1066_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1067_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1067 + PI_BASE_ADDR,DENALI_PI_1067_DATA,0);
       #else
         REGWR(1067 + PI_BASE_ADDR,DENALI_PI_1067_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1068_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1068 + PI_BASE_ADDR,DENALI_PI_1068_DATA,0);
       #else
         REGWR(1068 + PI_BASE_ADDR,DENALI_PI_1068_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1069_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1069 + PI_BASE_ADDR,DENALI_PI_1069_DATA,0);
       #else
         REGWR(1069 + PI_BASE_ADDR,DENALI_PI_1069_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1070_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1070 + PI_BASE_ADDR,DENALI_PI_1070_DATA,0);
       #else
         REGWR(1070 + PI_BASE_ADDR,DENALI_PI_1070_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1071_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1071 + PI_BASE_ADDR,DENALI_PI_1071_DATA,0);
       #else
         REGWR(1071 + PI_BASE_ADDR,DENALI_PI_1071_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1072_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1072 + PI_BASE_ADDR,DENALI_PI_1072_DATA,0);
       #else
         REGWR(1072 + PI_BASE_ADDR,DENALI_PI_1072_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1073_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1073 + PI_BASE_ADDR,DENALI_PI_1073_DATA,0);
       #else
         REGWR(1073 + PI_BASE_ADDR,DENALI_PI_1073_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1074_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1074 + PI_BASE_ADDR,DENALI_PI_1074_DATA,0);
       #else
         REGWR(1074 + PI_BASE_ADDR,DENALI_PI_1074_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1075_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1075 + PI_BASE_ADDR,DENALI_PI_1075_DATA,0);
       #else
         REGWR(1075 + PI_BASE_ADDR,DENALI_PI_1075_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1076_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1076 + PI_BASE_ADDR,DENALI_PI_1076_DATA,0);
       #else
         REGWR(1076 + PI_BASE_ADDR,DENALI_PI_1076_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1077_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1077 + PI_BASE_ADDR,DENALI_PI_1077_DATA,0);
       #else
         REGWR(1077 + PI_BASE_ADDR,DENALI_PI_1077_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1078_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1078 + PI_BASE_ADDR,DENALI_PI_1078_DATA,0);
       #else
         REGWR(1078 + PI_BASE_ADDR,DENALI_PI_1078_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1079_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1079 + PI_BASE_ADDR,DENALI_PI_1079_DATA,0);
       #else
         REGWR(1079 + PI_BASE_ADDR,DENALI_PI_1079_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1080_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1080 + PI_BASE_ADDR,DENALI_PI_1080_DATA,0);
       #else
         REGWR(1080 + PI_BASE_ADDR,DENALI_PI_1080_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1081_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1081 + PI_BASE_ADDR,DENALI_PI_1081_DATA,0);
       #else
         REGWR(1081 + PI_BASE_ADDR,DENALI_PI_1081_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1082_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1082 + PI_BASE_ADDR,DENALI_PI_1082_DATA,0);
       #else
         REGWR(1082 + PI_BASE_ADDR,DENALI_PI_1082_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1083_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1083 + PI_BASE_ADDR,DENALI_PI_1083_DATA,0);
       #else
         REGWR(1083 + PI_BASE_ADDR,DENALI_PI_1083_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1084_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1084 + PI_BASE_ADDR,DENALI_PI_1084_DATA,0);
       #else
         REGWR(1084 + PI_BASE_ADDR,DENALI_PI_1084_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1085_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1085 + PI_BASE_ADDR,DENALI_PI_1085_DATA,0);
       #else
         REGWR(1085 + PI_BASE_ADDR,DENALI_PI_1085_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1086_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1086 + PI_BASE_ADDR,DENALI_PI_1086_DATA,0);
       #else
         REGWR(1086 + PI_BASE_ADDR,DENALI_PI_1086_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1087_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1087 + PI_BASE_ADDR,DENALI_PI_1087_DATA,0);
       #else
         REGWR(1087 + PI_BASE_ADDR,DENALI_PI_1087_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1088_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1088 + PI_BASE_ADDR,DENALI_PI_1088_DATA,0);
       #else
         REGWR(1088 + PI_BASE_ADDR,DENALI_PI_1088_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1089_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1089 + PI_BASE_ADDR,DENALI_PI_1089_DATA,0);
       #else
         REGWR(1089 + PI_BASE_ADDR,DENALI_PI_1089_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1090_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1090 + PI_BASE_ADDR,DENALI_PI_1090_DATA,0);
       #else
         REGWR(1090 + PI_BASE_ADDR,DENALI_PI_1090_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1091_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1091 + PI_BASE_ADDR,DENALI_PI_1091_DATA,0);
       #else
         REGWR(1091 + PI_BASE_ADDR,DENALI_PI_1091_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1092_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1092 + PI_BASE_ADDR,DENALI_PI_1092_DATA,0);
       #else
         REGWR(1092 + PI_BASE_ADDR,DENALI_PI_1092_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1093_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1093 + PI_BASE_ADDR,DENALI_PI_1093_DATA,0);
       #else
         REGWR(1093 + PI_BASE_ADDR,DENALI_PI_1093_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1094_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1094 + PI_BASE_ADDR,DENALI_PI_1094_DATA,0);
       #else
         REGWR(1094 + PI_BASE_ADDR,DENALI_PI_1094_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1095_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1095 + PI_BASE_ADDR,DENALI_PI_1095_DATA,0);
       #else
         REGWR(1095 + PI_BASE_ADDR,DENALI_PI_1095_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1096_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1096 + PI_BASE_ADDR,DENALI_PI_1096_DATA,0);
       #else
         REGWR(1096 + PI_BASE_ADDR,DENALI_PI_1096_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1097_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1097 + PI_BASE_ADDR,DENALI_PI_1097_DATA,0);
       #else
         REGWR(1097 + PI_BASE_ADDR,DENALI_PI_1097_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1098_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1098 + PI_BASE_ADDR,DENALI_PI_1098_DATA,0);
       #else
         REGWR(1098 + PI_BASE_ADDR,DENALI_PI_1098_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1099_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1099 + PI_BASE_ADDR,DENALI_PI_1099_DATA,0);
       #else
         REGWR(1099 + PI_BASE_ADDR,DENALI_PI_1099_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1100_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1100 + PI_BASE_ADDR,DENALI_PI_1100_DATA,0);
       #else
         REGWR(1100 + PI_BASE_ADDR,DENALI_PI_1100_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1101_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1101 + PI_BASE_ADDR,DENALI_PI_1101_DATA,0);
       #else
         REGWR(1101 + PI_BASE_ADDR,DENALI_PI_1101_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1102_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1102 + PI_BASE_ADDR,DENALI_PI_1102_DATA,0);
       #else
         REGWR(1102 + PI_BASE_ADDR,DENALI_PI_1102_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1103_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1103 + PI_BASE_ADDR,DENALI_PI_1103_DATA,0);
       #else
         REGWR(1103 + PI_BASE_ADDR,DENALI_PI_1103_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1104_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1104 + PI_BASE_ADDR,DENALI_PI_1104_DATA,0);
       #else
         REGWR(1104 + PI_BASE_ADDR,DENALI_PI_1104_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1105_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1105 + PI_BASE_ADDR,DENALI_PI_1105_DATA,0);
       #else
         REGWR(1105 + PI_BASE_ADDR,DENALI_PI_1105_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1106_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1106 + PI_BASE_ADDR,DENALI_PI_1106_DATA,0);
       #else
         REGWR(1106 + PI_BASE_ADDR,DENALI_PI_1106_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1107_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1107 + PI_BASE_ADDR,DENALI_PI_1107_DATA,0);
       #else
         REGWR(1107 + PI_BASE_ADDR,DENALI_PI_1107_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1108_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1108 + PI_BASE_ADDR,DENALI_PI_1108_DATA,0);
       #else
         REGWR(1108 + PI_BASE_ADDR,DENALI_PI_1108_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1109_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1109 + PI_BASE_ADDR,DENALI_PI_1109_DATA,0);
       #else
         REGWR(1109 + PI_BASE_ADDR,DENALI_PI_1109_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1110_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1110 + PI_BASE_ADDR,DENALI_PI_1110_DATA,0);
       #else
         REGWR(1110 + PI_BASE_ADDR,DENALI_PI_1110_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1111_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1111 + PI_BASE_ADDR,DENALI_PI_1111_DATA,0);
       #else
         REGWR(1111 + PI_BASE_ADDR,DENALI_PI_1111_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1112_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1112 + PI_BASE_ADDR,DENALI_PI_1112_DATA,0);
       #else
         REGWR(1112 + PI_BASE_ADDR,DENALI_PI_1112_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1113_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1113 + PI_BASE_ADDR,DENALI_PI_1113_DATA,0);
       #else
         REGWR(1113 + PI_BASE_ADDR,DENALI_PI_1113_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1114_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1114 + PI_BASE_ADDR,DENALI_PI_1114_DATA,0);
       #else
         REGWR(1114 + PI_BASE_ADDR,DENALI_PI_1114_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1115_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1115 + PI_BASE_ADDR,DENALI_PI_1115_DATA,0);
       #else
         REGWR(1115 + PI_BASE_ADDR,DENALI_PI_1115_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1116_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1116 + PI_BASE_ADDR,DENALI_PI_1116_DATA,0);
       #else
         REGWR(1116 + PI_BASE_ADDR,DENALI_PI_1116_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1117_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1117 + PI_BASE_ADDR,DENALI_PI_1117_DATA,0);
       #else
         REGWR(1117 + PI_BASE_ADDR,DENALI_PI_1117_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1118_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1118 + PI_BASE_ADDR,DENALI_PI_1118_DATA,0);
       #else
         REGWR(1118 + PI_BASE_ADDR,DENALI_PI_1118_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1119_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1119 + PI_BASE_ADDR,DENALI_PI_1119_DATA,0);
       #else
         REGWR(1119 + PI_BASE_ADDR,DENALI_PI_1119_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1120_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1120 + PI_BASE_ADDR,DENALI_PI_1120_DATA,0);
       #else
         REGWR(1120 + PI_BASE_ADDR,DENALI_PI_1120_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1121_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1121 + PI_BASE_ADDR,DENALI_PI_1121_DATA,0);
       #else
         REGWR(1121 + PI_BASE_ADDR,DENALI_PI_1121_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1122_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1122 + PI_BASE_ADDR,DENALI_PI_1122_DATA,0);
       #else
         REGWR(1122 + PI_BASE_ADDR,DENALI_PI_1122_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1123_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1123 + PI_BASE_ADDR,DENALI_PI_1123_DATA,0);
       #else
         REGWR(1123 + PI_BASE_ADDR,DENALI_PI_1123_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1124_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1124 + PI_BASE_ADDR,DENALI_PI_1124_DATA,0);
       #else
         REGWR(1124 + PI_BASE_ADDR,DENALI_PI_1124_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1125_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1125 + PI_BASE_ADDR,DENALI_PI_1125_DATA,0);
       #else
         REGWR(1125 + PI_BASE_ADDR,DENALI_PI_1125_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1126_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1126 + PI_BASE_ADDR,DENALI_PI_1126_DATA,0);
       #else
         REGWR(1126 + PI_BASE_ADDR,DENALI_PI_1126_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1127_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1127 + PI_BASE_ADDR,DENALI_PI_1127_DATA,0);
       #else
         REGWR(1127 + PI_BASE_ADDR,DENALI_PI_1127_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1128_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1128 + PI_BASE_ADDR,DENALI_PI_1128_DATA,0);
       #else
         REGWR(1128 + PI_BASE_ADDR,DENALI_PI_1128_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1129_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1129 + PI_BASE_ADDR,DENALI_PI_1129_DATA,0);
       #else
         REGWR(1129 + PI_BASE_ADDR,DENALI_PI_1129_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1130_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1130 + PI_BASE_ADDR,DENALI_PI_1130_DATA,0);
       #else
         REGWR(1130 + PI_BASE_ADDR,DENALI_PI_1130_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1131_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1131 + PI_BASE_ADDR,DENALI_PI_1131_DATA,0);
       #else
         REGWR(1131 + PI_BASE_ADDR,DENALI_PI_1131_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1132_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1132 + PI_BASE_ADDR,DENALI_PI_1132_DATA,0);
       #else
         REGWR(1132 + PI_BASE_ADDR,DENALI_PI_1132_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1133_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1133 + PI_BASE_ADDR,DENALI_PI_1133_DATA,0);
       #else
         REGWR(1133 + PI_BASE_ADDR,DENALI_PI_1133_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1134_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1134 + PI_BASE_ADDR,DENALI_PI_1134_DATA,0);
       #else
         REGWR(1134 + PI_BASE_ADDR,DENALI_PI_1134_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1135_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1135 + PI_BASE_ADDR,DENALI_PI_1135_DATA,0);
       #else
         REGWR(1135 + PI_BASE_ADDR,DENALI_PI_1135_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1136_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1136 + PI_BASE_ADDR,DENALI_PI_1136_DATA,0);
       #else
         REGWR(1136 + PI_BASE_ADDR,DENALI_PI_1136_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1137_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1137 + PI_BASE_ADDR,DENALI_PI_1137_DATA,0);
       #else
         REGWR(1137 + PI_BASE_ADDR,DENALI_PI_1137_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1138_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1138 + PI_BASE_ADDR,DENALI_PI_1138_DATA,0);
       #else
         REGWR(1138 + PI_BASE_ADDR,DENALI_PI_1138_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1139_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1139 + PI_BASE_ADDR,DENALI_PI_1139_DATA,0);
       #else
         REGWR(1139 + PI_BASE_ADDR,DENALI_PI_1139_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1140_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1140 + PI_BASE_ADDR,DENALI_PI_1140_DATA,0);
       #else
         REGWR(1140 + PI_BASE_ADDR,DENALI_PI_1140_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1141_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1141 + PI_BASE_ADDR,DENALI_PI_1141_DATA,0);
       #else
         REGWR(1141 + PI_BASE_ADDR,DENALI_PI_1141_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1142_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1142 + PI_BASE_ADDR,DENALI_PI_1142_DATA,0);
       #else
         REGWR(1142 + PI_BASE_ADDR,DENALI_PI_1142_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1143_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1143 + PI_BASE_ADDR,DENALI_PI_1143_DATA,0);
       #else
         REGWR(1143 + PI_BASE_ADDR,DENALI_PI_1143_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1144_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1144 + PI_BASE_ADDR,DENALI_PI_1144_DATA,0);
       #else
         REGWR(1144 + PI_BASE_ADDR,DENALI_PI_1144_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1145_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1145 + PI_BASE_ADDR,DENALI_PI_1145_DATA,0);
       #else
         REGWR(1145 + PI_BASE_ADDR,DENALI_PI_1145_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1146_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1146 + PI_BASE_ADDR,DENALI_PI_1146_DATA,0);
       #else
         REGWR(1146 + PI_BASE_ADDR,DENALI_PI_1146_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1147_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1147 + PI_BASE_ADDR,DENALI_PI_1147_DATA,0);
       #else
         REGWR(1147 + PI_BASE_ADDR,DENALI_PI_1147_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1148_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1148 + PI_BASE_ADDR,DENALI_PI_1148_DATA,0);
       #else
         REGWR(1148 + PI_BASE_ADDR,DENALI_PI_1148_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1149_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1149 + PI_BASE_ADDR,DENALI_PI_1149_DATA,0);
       #else
         REGWR(1149 + PI_BASE_ADDR,DENALI_PI_1149_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1150_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1150 + PI_BASE_ADDR,DENALI_PI_1150_DATA,0);
       #else
         REGWR(1150 + PI_BASE_ADDR,DENALI_PI_1150_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1151_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1151 + PI_BASE_ADDR,DENALI_PI_1151_DATA,0);
       #else
         REGWR(1151 + PI_BASE_ADDR,DENALI_PI_1151_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1152_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1152 + PI_BASE_ADDR,DENALI_PI_1152_DATA,0);
       #else
         REGWR(1152 + PI_BASE_ADDR,DENALI_PI_1152_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1153_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1153 + PI_BASE_ADDR,DENALI_PI_1153_DATA,0);
       #else
         REGWR(1153 + PI_BASE_ADDR,DENALI_PI_1153_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1154_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1154 + PI_BASE_ADDR,DENALI_PI_1154_DATA,0);
       #else
         REGWR(1154 + PI_BASE_ADDR,DENALI_PI_1154_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1155_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1155 + PI_BASE_ADDR,DENALI_PI_1155_DATA,0);
       #else
         REGWR(1155 + PI_BASE_ADDR,DENALI_PI_1155_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1156_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1156 + PI_BASE_ADDR,DENALI_PI_1156_DATA,0);
       #else
         REGWR(1156 + PI_BASE_ADDR,DENALI_PI_1156_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1157_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1157 + PI_BASE_ADDR,DENALI_PI_1157_DATA,0);
       #else
         REGWR(1157 + PI_BASE_ADDR,DENALI_PI_1157_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1158_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1158 + PI_BASE_ADDR,DENALI_PI_1158_DATA,0);
       #else
         REGWR(1158 + PI_BASE_ADDR,DENALI_PI_1158_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1159_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1159 + PI_BASE_ADDR,DENALI_PI_1159_DATA,0);
       #else
         REGWR(1159 + PI_BASE_ADDR,DENALI_PI_1159_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1160_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1160 + PI_BASE_ADDR,DENALI_PI_1160_DATA,0);
       #else
         REGWR(1160 + PI_BASE_ADDR,DENALI_PI_1160_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1161_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1161 + PI_BASE_ADDR,DENALI_PI_1161_DATA,0);
       #else
         REGWR(1161 + PI_BASE_ADDR,DENALI_PI_1161_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1162_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1162 + PI_BASE_ADDR,DENALI_PI_1162_DATA,0);
       #else
         REGWR(1162 + PI_BASE_ADDR,DENALI_PI_1162_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1163_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1163 + PI_BASE_ADDR,DENALI_PI_1163_DATA,0);
       #else
         REGWR(1163 + PI_BASE_ADDR,DENALI_PI_1163_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1164_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1164 + PI_BASE_ADDR,DENALI_PI_1164_DATA,0);
       #else
         REGWR(1164 + PI_BASE_ADDR,DENALI_PI_1164_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1165_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1165 + PI_BASE_ADDR,DENALI_PI_1165_DATA,0);
       #else
         REGWR(1165 + PI_BASE_ADDR,DENALI_PI_1165_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1166_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1166 + PI_BASE_ADDR,DENALI_PI_1166_DATA,0);
       #else
         REGWR(1166 + PI_BASE_ADDR,DENALI_PI_1166_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1167_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1167 + PI_BASE_ADDR,DENALI_PI_1167_DATA,0);
       #else
         REGWR(1167 + PI_BASE_ADDR,DENALI_PI_1167_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1168_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1168 + PI_BASE_ADDR,DENALI_PI_1168_DATA,0);
       #else
         REGWR(1168 + PI_BASE_ADDR,DENALI_PI_1168_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1169_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1169 + PI_BASE_ADDR,DENALI_PI_1169_DATA,0);
       #else
         REGWR(1169 + PI_BASE_ADDR,DENALI_PI_1169_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1170_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1170 + PI_BASE_ADDR,DENALI_PI_1170_DATA,0);
       #else
         REGWR(1170 + PI_BASE_ADDR,DENALI_PI_1170_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1171_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1171 + PI_BASE_ADDR,DENALI_PI_1171_DATA,0);
       #else
         REGWR(1171 + PI_BASE_ADDR,DENALI_PI_1171_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1172_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1172 + PI_BASE_ADDR,DENALI_PI_1172_DATA,0);
       #else
         REGWR(1172 + PI_BASE_ADDR,DENALI_PI_1172_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1173_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1173 + PI_BASE_ADDR,DENALI_PI_1173_DATA,0);
       #else
         REGWR(1173 + PI_BASE_ADDR,DENALI_PI_1173_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1174_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1174 + PI_BASE_ADDR,DENALI_PI_1174_DATA,0);
       #else
         REGWR(1174 + PI_BASE_ADDR,DENALI_PI_1174_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1175_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1175 + PI_BASE_ADDR,DENALI_PI_1175_DATA,0);
       #else
         REGWR(1175 + PI_BASE_ADDR,DENALI_PI_1175_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1176_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1176 + PI_BASE_ADDR,DENALI_PI_1176_DATA,0);
       #else
         REGWR(1176 + PI_BASE_ADDR,DENALI_PI_1176_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1177_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1177 + PI_BASE_ADDR,DENALI_PI_1177_DATA,0);
       #else
         REGWR(1177 + PI_BASE_ADDR,DENALI_PI_1177_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1178_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1178 + PI_BASE_ADDR,DENALI_PI_1178_DATA,0);
       #else
         REGWR(1178 + PI_BASE_ADDR,DENALI_PI_1178_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1179_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1179 + PI_BASE_ADDR,DENALI_PI_1179_DATA,0);
       #else
         REGWR(1179 + PI_BASE_ADDR,DENALI_PI_1179_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1180_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1180 + PI_BASE_ADDR,DENALI_PI_1180_DATA,0);
       #else
         REGWR(1180 + PI_BASE_ADDR,DENALI_PI_1180_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1181_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1181 + PI_BASE_ADDR,DENALI_PI_1181_DATA,0);
       #else
         REGWR(1181 + PI_BASE_ADDR,DENALI_PI_1181_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1182_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1182 + PI_BASE_ADDR,DENALI_PI_1182_DATA,0);
       #else
         REGWR(1182 + PI_BASE_ADDR,DENALI_PI_1182_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1183_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1183 + PI_BASE_ADDR,DENALI_PI_1183_DATA,0);
       #else
         REGWR(1183 + PI_BASE_ADDR,DENALI_PI_1183_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1184_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1184 + PI_BASE_ADDR,DENALI_PI_1184_DATA,0);
       #else
         REGWR(1184 + PI_BASE_ADDR,DENALI_PI_1184_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1185_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1185 + PI_BASE_ADDR,DENALI_PI_1185_DATA,0);
       #else
         REGWR(1185 + PI_BASE_ADDR,DENALI_PI_1185_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1186_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1186 + PI_BASE_ADDR,DENALI_PI_1186_DATA,0);
       #else
         REGWR(1186 + PI_BASE_ADDR,DENALI_PI_1186_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1187_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1187 + PI_BASE_ADDR,DENALI_PI_1187_DATA,0);
       #else
         REGWR(1187 + PI_BASE_ADDR,DENALI_PI_1187_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1188_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1188 + PI_BASE_ADDR,DENALI_PI_1188_DATA,0);
       #else
         REGWR(1188 + PI_BASE_ADDR,DENALI_PI_1188_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1189_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1189 + PI_BASE_ADDR,DENALI_PI_1189_DATA,0);
       #else
         REGWR(1189 + PI_BASE_ADDR,DENALI_PI_1189_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1190_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1190 + PI_BASE_ADDR,DENALI_PI_1190_DATA,0);
       #else
         REGWR(1190 + PI_BASE_ADDR,DENALI_PI_1190_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1191_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1191 + PI_BASE_ADDR,DENALI_PI_1191_DATA,0);
       #else
         REGWR(1191 + PI_BASE_ADDR,DENALI_PI_1191_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1192_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1192 + PI_BASE_ADDR,DENALI_PI_1192_DATA,0);
       #else
         REGWR(1192 + PI_BASE_ADDR,DENALI_PI_1192_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1193_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1193 + PI_BASE_ADDR,DENALI_PI_1193_DATA,0);
       #else
         REGWR(1193 + PI_BASE_ADDR,DENALI_PI_1193_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1194_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1194 + PI_BASE_ADDR,DENALI_PI_1194_DATA,0);
       #else
         REGWR(1194 + PI_BASE_ADDR,DENALI_PI_1194_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1195_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1195 + PI_BASE_ADDR,DENALI_PI_1195_DATA,0);
       #else
         REGWR(1195 + PI_BASE_ADDR,DENALI_PI_1195_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1196_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1196 + PI_BASE_ADDR,DENALI_PI_1196_DATA,0);
       #else
         REGWR(1196 + PI_BASE_ADDR,DENALI_PI_1196_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1197_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1197 + PI_BASE_ADDR,DENALI_PI_1197_DATA,0);
       #else
         REGWR(1197 + PI_BASE_ADDR,DENALI_PI_1197_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1198_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1198 + PI_BASE_ADDR,DENALI_PI_1198_DATA,0);
       #else
         REGWR(1198 + PI_BASE_ADDR,DENALI_PI_1198_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1199_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1199 + PI_BASE_ADDR,DENALI_PI_1199_DATA,0);
       #else
         REGWR(1199 + PI_BASE_ADDR,DENALI_PI_1199_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1200_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1200 + PI_BASE_ADDR,DENALI_PI_1200_DATA,0);
       #else
         REGWR(1200 + PI_BASE_ADDR,DENALI_PI_1200_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1201_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1201 + PI_BASE_ADDR,DENALI_PI_1201_DATA,0);
       #else
         REGWR(1201 + PI_BASE_ADDR,DENALI_PI_1201_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1202_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1202 + PI_BASE_ADDR,DENALI_PI_1202_DATA,0);
       #else
         REGWR(1202 + PI_BASE_ADDR,DENALI_PI_1202_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1203_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1203 + PI_BASE_ADDR,DENALI_PI_1203_DATA,0);
       #else
         REGWR(1203 + PI_BASE_ADDR,DENALI_PI_1203_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1204_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1204 + PI_BASE_ADDR,DENALI_PI_1204_DATA,0);
       #else
         REGWR(1204 + PI_BASE_ADDR,DENALI_PI_1204_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1205_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1205 + PI_BASE_ADDR,DENALI_PI_1205_DATA,0);
       #else
         REGWR(1205 + PI_BASE_ADDR,DENALI_PI_1205_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1206_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1206 + PI_BASE_ADDR,DENALI_PI_1206_DATA,0);
       #else
         REGWR(1206 + PI_BASE_ADDR,DENALI_PI_1206_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1207_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1207 + PI_BASE_ADDR,DENALI_PI_1207_DATA,0);
       #else
         REGWR(1207 + PI_BASE_ADDR,DENALI_PI_1207_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1208_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1208 + PI_BASE_ADDR,DENALI_PI_1208_DATA,0);
       #else
         REGWR(1208 + PI_BASE_ADDR,DENALI_PI_1208_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1209_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1209 + PI_BASE_ADDR,DENALI_PI_1209_DATA,0);
       #else
         REGWR(1209 + PI_BASE_ADDR,DENALI_PI_1209_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1210_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1210 + PI_BASE_ADDR,DENALI_PI_1210_DATA,0);
       #else
         REGWR(1210 + PI_BASE_ADDR,DENALI_PI_1210_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1211_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1211 + PI_BASE_ADDR,DENALI_PI_1211_DATA,0);
       #else
         REGWR(1211 + PI_BASE_ADDR,DENALI_PI_1211_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1212_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1212 + PI_BASE_ADDR,DENALI_PI_1212_DATA,0);
       #else
         REGWR(1212 + PI_BASE_ADDR,DENALI_PI_1212_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1213_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1213 + PI_BASE_ADDR,DENALI_PI_1213_DATA,0);
       #else
         REGWR(1213 + PI_BASE_ADDR,DENALI_PI_1213_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1214_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1214 + PI_BASE_ADDR,DENALI_PI_1214_DATA,0);
       #else
         REGWR(1214 + PI_BASE_ADDR,DENALI_PI_1214_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1215_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1215 + PI_BASE_ADDR,DENALI_PI_1215_DATA,0);
       #else
         REGWR(1215 + PI_BASE_ADDR,DENALI_PI_1215_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1216_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1216 + PI_BASE_ADDR,DENALI_PI_1216_DATA,0);
       #else
         REGWR(1216 + PI_BASE_ADDR,DENALI_PI_1216_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1217_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1217 + PI_BASE_ADDR,DENALI_PI_1217_DATA,0);
       #else
         REGWR(1217 + PI_BASE_ADDR,DENALI_PI_1217_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1218_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1218 + PI_BASE_ADDR,DENALI_PI_1218_DATA,0);
       #else
         REGWR(1218 + PI_BASE_ADDR,DENALI_PI_1218_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1219_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1219 + PI_BASE_ADDR,DENALI_PI_1219_DATA,0);
       #else
         REGWR(1219 + PI_BASE_ADDR,DENALI_PI_1219_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1220_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1220 + PI_BASE_ADDR,DENALI_PI_1220_DATA,0);
       #else
         REGWR(1220 + PI_BASE_ADDR,DENALI_PI_1220_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1221_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1221 + PI_BASE_ADDR,DENALI_PI_1221_DATA,0);
       #else
         REGWR(1221 + PI_BASE_ADDR,DENALI_PI_1221_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1222_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1222 + PI_BASE_ADDR,DENALI_PI_1222_DATA,0);
       #else
         REGWR(1222 + PI_BASE_ADDR,DENALI_PI_1222_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1223_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1223 + PI_BASE_ADDR,DENALI_PI_1223_DATA,0);
       #else
         REGWR(1223 + PI_BASE_ADDR,DENALI_PI_1223_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1224_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1224 + PI_BASE_ADDR,DENALI_PI_1224_DATA,0);
       #else
         REGWR(1224 + PI_BASE_ADDR,DENALI_PI_1224_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1225_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1225 + PI_BASE_ADDR,DENALI_PI_1225_DATA,0);
       #else
         REGWR(1225 + PI_BASE_ADDR,DENALI_PI_1225_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1226_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1226 + PI_BASE_ADDR,DENALI_PI_1226_DATA,0);
       #else
         REGWR(1226 + PI_BASE_ADDR,DENALI_PI_1226_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1227_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1227 + PI_BASE_ADDR,DENALI_PI_1227_DATA,0);
       #else
         REGWR(1227 + PI_BASE_ADDR,DENALI_PI_1227_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1228_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1228 + PI_BASE_ADDR,DENALI_PI_1228_DATA,0);
       #else
         REGWR(1228 + PI_BASE_ADDR,DENALI_PI_1228_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1229_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1229 + PI_BASE_ADDR,DENALI_PI_1229_DATA,0);
       #else
         REGWR(1229 + PI_BASE_ADDR,DENALI_PI_1229_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1230_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1230 + PI_BASE_ADDR,DENALI_PI_1230_DATA,0);
       #else
         REGWR(1230 + PI_BASE_ADDR,DENALI_PI_1230_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1231_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1231 + PI_BASE_ADDR,DENALI_PI_1231_DATA,0);
       #else
         REGWR(1231 + PI_BASE_ADDR,DENALI_PI_1231_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1232_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1232 + PI_BASE_ADDR,DENALI_PI_1232_DATA,0);
       #else
         REGWR(1232 + PI_BASE_ADDR,DENALI_PI_1232_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1233_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1233 + PI_BASE_ADDR,DENALI_PI_1233_DATA,0);
       #else
         REGWR(1233 + PI_BASE_ADDR,DENALI_PI_1233_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1234_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1234 + PI_BASE_ADDR,DENALI_PI_1234_DATA,0);
       #else
         REGWR(1234 + PI_BASE_ADDR,DENALI_PI_1234_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1235_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1235 + PI_BASE_ADDR,DENALI_PI_1235_DATA,0);
       #else
         REGWR(1235 + PI_BASE_ADDR,DENALI_PI_1235_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1236_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1236 + PI_BASE_ADDR,DENALI_PI_1236_DATA,0);
       #else
         REGWR(1236 + PI_BASE_ADDR,DENALI_PI_1236_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1237_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1237 + PI_BASE_ADDR,DENALI_PI_1237_DATA,0);
       #else
         REGWR(1237 + PI_BASE_ADDR,DENALI_PI_1237_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1238_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1238 + PI_BASE_ADDR,DENALI_PI_1238_DATA,0);
       #else
         REGWR(1238 + PI_BASE_ADDR,DENALI_PI_1238_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1239_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1239 + PI_BASE_ADDR,DENALI_PI_1239_DATA,0);
       #else
         REGWR(1239 + PI_BASE_ADDR,DENALI_PI_1239_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1240_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1240 + PI_BASE_ADDR,DENALI_PI_1240_DATA,0);
       #else
         REGWR(1240 + PI_BASE_ADDR,DENALI_PI_1240_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1241_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1241 + PI_BASE_ADDR,DENALI_PI_1241_DATA,0);
       #else
         REGWR(1241 + PI_BASE_ADDR,DENALI_PI_1241_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1242_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1242 + PI_BASE_ADDR,DENALI_PI_1242_DATA,0);
       #else
         REGWR(1242 + PI_BASE_ADDR,DENALI_PI_1242_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1243_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1243 + PI_BASE_ADDR,DENALI_PI_1243_DATA,0);
       #else
         REGWR(1243 + PI_BASE_ADDR,DENALI_PI_1243_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1244_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1244 + PI_BASE_ADDR,DENALI_PI_1244_DATA,0);
       #else
         REGWR(1244 + PI_BASE_ADDR,DENALI_PI_1244_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1245_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1245 + PI_BASE_ADDR,DENALI_PI_1245_DATA,0);
       #else
         REGWR(1245 + PI_BASE_ADDR,DENALI_PI_1245_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1246_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1246 + PI_BASE_ADDR,DENALI_PI_1246_DATA,0);
       #else
         REGWR(1246 + PI_BASE_ADDR,DENALI_PI_1246_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1247_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1247 + PI_BASE_ADDR,DENALI_PI_1247_DATA,0);
       #else
         REGWR(1247 + PI_BASE_ADDR,DENALI_PI_1247_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1248_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1248 + PI_BASE_ADDR,DENALI_PI_1248_DATA,0);
       #else
         REGWR(1248 + PI_BASE_ADDR,DENALI_PI_1248_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1249_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1249 + PI_BASE_ADDR,DENALI_PI_1249_DATA,0);
       #else
         REGWR(1249 + PI_BASE_ADDR,DENALI_PI_1249_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1250_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1250 + PI_BASE_ADDR,DENALI_PI_1250_DATA,0);
       #else
         REGWR(1250 + PI_BASE_ADDR,DENALI_PI_1250_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1251_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1251 + PI_BASE_ADDR,DENALI_PI_1251_DATA,0);
       #else
         REGWR(1251 + PI_BASE_ADDR,DENALI_PI_1251_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1252_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1252 + PI_BASE_ADDR,DENALI_PI_1252_DATA,0);
       #else
         REGWR(1252 + PI_BASE_ADDR,DENALI_PI_1252_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1253_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1253 + PI_BASE_ADDR,DENALI_PI_1253_DATA,0);
       #else
         REGWR(1253 + PI_BASE_ADDR,DENALI_PI_1253_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1254_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1254 + PI_BASE_ADDR,DENALI_PI_1254_DATA,0);
       #else
         REGWR(1254 + PI_BASE_ADDR,DENALI_PI_1254_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1255_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1255 + PI_BASE_ADDR,DENALI_PI_1255_DATA,0);
       #else
         REGWR(1255 + PI_BASE_ADDR,DENALI_PI_1255_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1256_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1256 + PI_BASE_ADDR,DENALI_PI_1256_DATA,0);
       #else
         REGWR(1256 + PI_BASE_ADDR,DENALI_PI_1256_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1257_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1257 + PI_BASE_ADDR,DENALI_PI_1257_DATA,0);
       #else
         REGWR(1257 + PI_BASE_ADDR,DENALI_PI_1257_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1258_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1258 + PI_BASE_ADDR,DENALI_PI_1258_DATA,0);
       #else
         REGWR(1258 + PI_BASE_ADDR,DENALI_PI_1258_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1259_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1259 + PI_BASE_ADDR,DENALI_PI_1259_DATA,0);
       #else
         REGWR(1259 + PI_BASE_ADDR,DENALI_PI_1259_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1260_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1260 + PI_BASE_ADDR,DENALI_PI_1260_DATA,0);
       #else
         REGWR(1260 + PI_BASE_ADDR,DENALI_PI_1260_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1261_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1261 + PI_BASE_ADDR,DENALI_PI_1261_DATA,0);
       #else
         REGWR(1261 + PI_BASE_ADDR,DENALI_PI_1261_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1262_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1262 + PI_BASE_ADDR,DENALI_PI_1262_DATA,0);
       #else
         REGWR(1262 + PI_BASE_ADDR,DENALI_PI_1262_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1263_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1263 + PI_BASE_ADDR,DENALI_PI_1263_DATA,0);
       #else
         REGWR(1263 + PI_BASE_ADDR,DENALI_PI_1263_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1264_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1264 + PI_BASE_ADDR,DENALI_PI_1264_DATA,0);
       #else
         REGWR(1264 + PI_BASE_ADDR,DENALI_PI_1264_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1265_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1265 + PI_BASE_ADDR,DENALI_PI_1265_DATA,0);
       #else
         REGWR(1265 + PI_BASE_ADDR,DENALI_PI_1265_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1266_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1266 + PI_BASE_ADDR,DENALI_PI_1266_DATA,0);
       #else
         REGWR(1266 + PI_BASE_ADDR,DENALI_PI_1266_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1267_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1267 + PI_BASE_ADDR,DENALI_PI_1267_DATA,0);
       #else
         REGWR(1267 + PI_BASE_ADDR,DENALI_PI_1267_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1268_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1268 + PI_BASE_ADDR,DENALI_PI_1268_DATA,0);
       #else
         REGWR(1268 + PI_BASE_ADDR,DENALI_PI_1268_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1269_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1269 + PI_BASE_ADDR,DENALI_PI_1269_DATA,0);
       #else
         REGWR(1269 + PI_BASE_ADDR,DENALI_PI_1269_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1270_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1270 + PI_BASE_ADDR,DENALI_PI_1270_DATA,0);
       #else
         REGWR(1270 + PI_BASE_ADDR,DENALI_PI_1270_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1271_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1271 + PI_BASE_ADDR,DENALI_PI_1271_DATA,0);
       #else
         REGWR(1271 + PI_BASE_ADDR,DENALI_PI_1271_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1272_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1272 + PI_BASE_ADDR,DENALI_PI_1272_DATA,0);
       #else
         REGWR(1272 + PI_BASE_ADDR,DENALI_PI_1272_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1273_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1273 + PI_BASE_ADDR,DENALI_PI_1273_DATA,0);
       #else
         REGWR(1273 + PI_BASE_ADDR,DENALI_PI_1273_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1274_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1274 + PI_BASE_ADDR,DENALI_PI_1274_DATA,0);
       #else
         REGWR(1274 + PI_BASE_ADDR,DENALI_PI_1274_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1275_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1275 + PI_BASE_ADDR,DENALI_PI_1275_DATA,0);
       #else
         REGWR(1275 + PI_BASE_ADDR,DENALI_PI_1275_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1276_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1276 + PI_BASE_ADDR,DENALI_PI_1276_DATA,0);
       #else
         REGWR(1276 + PI_BASE_ADDR,DENALI_PI_1276_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1277_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1277 + PI_BASE_ADDR,DENALI_PI_1277_DATA,0);
       #else
         REGWR(1277 + PI_BASE_ADDR,DENALI_PI_1277_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1278_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1278 + PI_BASE_ADDR,DENALI_PI_1278_DATA,0);
       #else
         REGWR(1278 + PI_BASE_ADDR,DENALI_PI_1278_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1279_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1279 + PI_BASE_ADDR,DENALI_PI_1279_DATA,0);
       #else
         REGWR(1279 + PI_BASE_ADDR,DENALI_PI_1279_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1280_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1280 + PI_BASE_ADDR,DENALI_PI_1280_DATA,0);
       #else
         REGWR(1280 + PI_BASE_ADDR,DENALI_PI_1280_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1281_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1281 + PI_BASE_ADDR,DENALI_PI_1281_DATA,0);
       #else
         REGWR(1281 + PI_BASE_ADDR,DENALI_PI_1281_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1282_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1282 + PI_BASE_ADDR,DENALI_PI_1282_DATA,0);
       #else
         REGWR(1282 + PI_BASE_ADDR,DENALI_PI_1282_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1283_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1283 + PI_BASE_ADDR,DENALI_PI_1283_DATA,0);
       #else
         REGWR(1283 + PI_BASE_ADDR,DENALI_PI_1283_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1284_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1284 + PI_BASE_ADDR,DENALI_PI_1284_DATA,0);
       #else
         REGWR(1284 + PI_BASE_ADDR,DENALI_PI_1284_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1285_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1285 + PI_BASE_ADDR,DENALI_PI_1285_DATA,0);
       #else
         REGWR(1285 + PI_BASE_ADDR,DENALI_PI_1285_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1286_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1286 + PI_BASE_ADDR,DENALI_PI_1286_DATA,0);
       #else
         REGWR(1286 + PI_BASE_ADDR,DENALI_PI_1286_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1287_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1287 + PI_BASE_ADDR,DENALI_PI_1287_DATA,0);
       #else
         REGWR(1287 + PI_BASE_ADDR,DENALI_PI_1287_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1288_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1288 + PI_BASE_ADDR,DENALI_PI_1288_DATA,0);
       #else
         REGWR(1288 + PI_BASE_ADDR,DENALI_PI_1288_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1289_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1289 + PI_BASE_ADDR,DENALI_PI_1289_DATA,0);
       #else
         REGWR(1289 + PI_BASE_ADDR,DENALI_PI_1289_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1290_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1290 + PI_BASE_ADDR,DENALI_PI_1290_DATA,0);
       #else
         REGWR(1290 + PI_BASE_ADDR,DENALI_PI_1290_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1291_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1291 + PI_BASE_ADDR,DENALI_PI_1291_DATA,0);
       #else
         REGWR(1291 + PI_BASE_ADDR,DENALI_PI_1291_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1292_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1292 + PI_BASE_ADDR,DENALI_PI_1292_DATA,0);
       #else
         REGWR(1292 + PI_BASE_ADDR,DENALI_PI_1292_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1293_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1293 + PI_BASE_ADDR,DENALI_PI_1293_DATA,0);
       #else
         REGWR(1293 + PI_BASE_ADDR,DENALI_PI_1293_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1294_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1294 + PI_BASE_ADDR,DENALI_PI_1294_DATA,0);
       #else
         REGWR(1294 + PI_BASE_ADDR,DENALI_PI_1294_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1295_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1295 + PI_BASE_ADDR,DENALI_PI_1295_DATA,0);
       #else
         REGWR(1295 + PI_BASE_ADDR,DENALI_PI_1295_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1296_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1296 + PI_BASE_ADDR,DENALI_PI_1296_DATA,0);
       #else
         REGWR(1296 + PI_BASE_ADDR,DENALI_PI_1296_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1297_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1297 + PI_BASE_ADDR,DENALI_PI_1297_DATA,0);
       #else
         REGWR(1297 + PI_BASE_ADDR,DENALI_PI_1297_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1298_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1298 + PI_BASE_ADDR,DENALI_PI_1298_DATA,0);
       #else
         REGWR(1298 + PI_BASE_ADDR,DENALI_PI_1298_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1299_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1299 + PI_BASE_ADDR,DENALI_PI_1299_DATA,0);
       #else
         REGWR(1299 + PI_BASE_ADDR,DENALI_PI_1299_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1300_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1300 + PI_BASE_ADDR,DENALI_PI_1300_DATA,0);
       #else
         REGWR(1300 + PI_BASE_ADDR,DENALI_PI_1300_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1301_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1301 + PI_BASE_ADDR,DENALI_PI_1301_DATA,0);
       #else
         REGWR(1301 + PI_BASE_ADDR,DENALI_PI_1301_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1302_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1302 + PI_BASE_ADDR,DENALI_PI_1302_DATA,0);
       #else
         REGWR(1302 + PI_BASE_ADDR,DENALI_PI_1302_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1303_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1303 + PI_BASE_ADDR,DENALI_PI_1303_DATA,0);
       #else
         REGWR(1303 + PI_BASE_ADDR,DENALI_PI_1303_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1304_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1304 + PI_BASE_ADDR,DENALI_PI_1304_DATA,0);
       #else
         REGWR(1304 + PI_BASE_ADDR,DENALI_PI_1304_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1305_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1305 + PI_BASE_ADDR,DENALI_PI_1305_DATA,0);
       #else
         REGWR(1305 + PI_BASE_ADDR,DENALI_PI_1305_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1306_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1306 + PI_BASE_ADDR,DENALI_PI_1306_DATA,0);
       #else
         REGWR(1306 + PI_BASE_ADDR,DENALI_PI_1306_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1307_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1307 + PI_BASE_ADDR,DENALI_PI_1307_DATA,0);
       #else
         REGWR(1307 + PI_BASE_ADDR,DENALI_PI_1307_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1308_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1308 + PI_BASE_ADDR,DENALI_PI_1308_DATA,0);
       #else
         REGWR(1308 + PI_BASE_ADDR,DENALI_PI_1308_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1309_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1309 + PI_BASE_ADDR,DENALI_PI_1309_DATA,0);
       #else
         REGWR(1309 + PI_BASE_ADDR,DENALI_PI_1309_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1310_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1310 + PI_BASE_ADDR,DENALI_PI_1310_DATA,0);
       #else
         REGWR(1310 + PI_BASE_ADDR,DENALI_PI_1310_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1311_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1311 + PI_BASE_ADDR,DENALI_PI_1311_DATA,0);
       #else
         REGWR(1311 + PI_BASE_ADDR,DENALI_PI_1311_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1312_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1312 + PI_BASE_ADDR,DENALI_PI_1312_DATA,0);
       #else
         REGWR(1312 + PI_BASE_ADDR,DENALI_PI_1312_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1313_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1313 + PI_BASE_ADDR,DENALI_PI_1313_DATA,0);
       #else
         REGWR(1313 + PI_BASE_ADDR,DENALI_PI_1313_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1314_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1314 + PI_BASE_ADDR,DENALI_PI_1314_DATA,0);
       #else
         REGWR(1314 + PI_BASE_ADDR,DENALI_PI_1314_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1315_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1315 + PI_BASE_ADDR,DENALI_PI_1315_DATA,0);
       #else
         REGWR(1315 + PI_BASE_ADDR,DENALI_PI_1315_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1316_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1316 + PI_BASE_ADDR,DENALI_PI_1316_DATA,0);
       #else
         REGWR(1316 + PI_BASE_ADDR,DENALI_PI_1316_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1317_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1317 + PI_BASE_ADDR,DENALI_PI_1317_DATA,0);
       #else
         REGWR(1317 + PI_BASE_ADDR,DENALI_PI_1317_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1318_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1318 + PI_BASE_ADDR,DENALI_PI_1318_DATA,0);
       #else
         REGWR(1318 + PI_BASE_ADDR,DENALI_PI_1318_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1319_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1319 + PI_BASE_ADDR,DENALI_PI_1319_DATA,0);
       #else
         REGWR(1319 + PI_BASE_ADDR,DENALI_PI_1319_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1320_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1320 + PI_BASE_ADDR,DENALI_PI_1320_DATA,0);
       #else
         REGWR(1320 + PI_BASE_ADDR,DENALI_PI_1320_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1321_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1321 + PI_BASE_ADDR,DENALI_PI_1321_DATA,0);
       #else
         REGWR(1321 + PI_BASE_ADDR,DENALI_PI_1321_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1322_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1322 + PI_BASE_ADDR,DENALI_PI_1322_DATA,0);
       #else
         REGWR(1322 + PI_BASE_ADDR,DENALI_PI_1322_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1323_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1323 + PI_BASE_ADDR,DENALI_PI_1323_DATA,0);
       #else
         REGWR(1323 + PI_BASE_ADDR,DENALI_PI_1323_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1324_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1324 + PI_BASE_ADDR,DENALI_PI_1324_DATA,0);
       #else
         REGWR(1324 + PI_BASE_ADDR,DENALI_PI_1324_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1325_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1325 + PI_BASE_ADDR,DENALI_PI_1325_DATA,0);
       #else
         REGWR(1325 + PI_BASE_ADDR,DENALI_PI_1325_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1326_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1326 + PI_BASE_ADDR,DENALI_PI_1326_DATA,0);
       #else
         REGWR(1326 + PI_BASE_ADDR,DENALI_PI_1326_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1327_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1327 + PI_BASE_ADDR,DENALI_PI_1327_DATA,0);
       #else
         REGWR(1327 + PI_BASE_ADDR,DENALI_PI_1327_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1328_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1328 + PI_BASE_ADDR,DENALI_PI_1328_DATA,0);
       #else
         REGWR(1328 + PI_BASE_ADDR,DENALI_PI_1328_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1329_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1329 + PI_BASE_ADDR,DENALI_PI_1329_DATA,0);
       #else
         REGWR(1329 + PI_BASE_ADDR,DENALI_PI_1329_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1330_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1330 + PI_BASE_ADDR,DENALI_PI_1330_DATA,0);
       #else
         REGWR(1330 + PI_BASE_ADDR,DENALI_PI_1330_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1331_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1331 + PI_BASE_ADDR,DENALI_PI_1331_DATA,0);
       #else
         REGWR(1331 + PI_BASE_ADDR,DENALI_PI_1331_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1332_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1332 + PI_BASE_ADDR,DENALI_PI_1332_DATA,0);
       #else
         REGWR(1332 + PI_BASE_ADDR,DENALI_PI_1332_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1333_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1333 + PI_BASE_ADDR,DENALI_PI_1333_DATA,0);
       #else
         REGWR(1333 + PI_BASE_ADDR,DENALI_PI_1333_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1334_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1334 + PI_BASE_ADDR,DENALI_PI_1334_DATA,0);
       #else
         REGWR(1334 + PI_BASE_ADDR,DENALI_PI_1334_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1335_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1335 + PI_BASE_ADDR,DENALI_PI_1335_DATA,0);
       #else
         REGWR(1335 + PI_BASE_ADDR,DENALI_PI_1335_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1336_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1336 + PI_BASE_ADDR,DENALI_PI_1336_DATA,0);
       #else
         REGWR(1336 + PI_BASE_ADDR,DENALI_PI_1336_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1337_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1337 + PI_BASE_ADDR,DENALI_PI_1337_DATA,0);
       #else
         REGWR(1337 + PI_BASE_ADDR,DENALI_PI_1337_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1338_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1338 + PI_BASE_ADDR,DENALI_PI_1338_DATA,0);
       #else
         REGWR(1338 + PI_BASE_ADDR,DENALI_PI_1338_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1339_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1339 + PI_BASE_ADDR,DENALI_PI_1339_DATA,0);
       #else
         REGWR(1339 + PI_BASE_ADDR,DENALI_PI_1339_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1340_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1340 + PI_BASE_ADDR,DENALI_PI_1340_DATA,0);
       #else
         REGWR(1340 + PI_BASE_ADDR,DENALI_PI_1340_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1341_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1341 + PI_BASE_ADDR,DENALI_PI_1341_DATA,0);
       #else
         REGWR(1341 + PI_BASE_ADDR,DENALI_PI_1341_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1342_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1342 + PI_BASE_ADDR,DENALI_PI_1342_DATA,0);
       #else
         REGWR(1342 + PI_BASE_ADDR,DENALI_PI_1342_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1343_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1343 + PI_BASE_ADDR,DENALI_PI_1343_DATA,0);
       #else
         REGWR(1343 + PI_BASE_ADDR,DENALI_PI_1343_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1344_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1344 + PI_BASE_ADDR,DENALI_PI_1344_DATA,0);
       #else
         REGWR(1344 + PI_BASE_ADDR,DENALI_PI_1344_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1345_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1345 + PI_BASE_ADDR,DENALI_PI_1345_DATA,0);
       #else
         REGWR(1345 + PI_BASE_ADDR,DENALI_PI_1345_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1346_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1346 + PI_BASE_ADDR,DENALI_PI_1346_DATA,0);
       #else
         REGWR(1346 + PI_BASE_ADDR,DENALI_PI_1346_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1347_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1347 + PI_BASE_ADDR,DENALI_PI_1347_DATA,0);
       #else
         REGWR(1347 + PI_BASE_ADDR,DENALI_PI_1347_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1348_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1348 + PI_BASE_ADDR,DENALI_PI_1348_DATA,0);
       #else
         REGWR(1348 + PI_BASE_ADDR,DENALI_PI_1348_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1349_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1349 + PI_BASE_ADDR,DENALI_PI_1349_DATA,0);
       #else
         REGWR(1349 + PI_BASE_ADDR,DENALI_PI_1349_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1350_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1350 + PI_BASE_ADDR,DENALI_PI_1350_DATA,0);
       #else
         REGWR(1350 + PI_BASE_ADDR,DENALI_PI_1350_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1351_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1351 + PI_BASE_ADDR,DENALI_PI_1351_DATA,0);
       #else
         REGWR(1351 + PI_BASE_ADDR,DENALI_PI_1351_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1352_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1352 + PI_BASE_ADDR,DENALI_PI_1352_DATA,0);
       #else
         REGWR(1352 + PI_BASE_ADDR,DENALI_PI_1352_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1353_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1353 + PI_BASE_ADDR,DENALI_PI_1353_DATA,0);
       #else
         REGWR(1353 + PI_BASE_ADDR,DENALI_PI_1353_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1354_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1354 + PI_BASE_ADDR,DENALI_PI_1354_DATA,0);
       #else
         REGWR(1354 + PI_BASE_ADDR,DENALI_PI_1354_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1355_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1355 + PI_BASE_ADDR,DENALI_PI_1355_DATA,0);
       #else
         REGWR(1355 + PI_BASE_ADDR,DENALI_PI_1355_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1356_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1356 + PI_BASE_ADDR,DENALI_PI_1356_DATA,0);
       #else
         REGWR(1356 + PI_BASE_ADDR,DENALI_PI_1356_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1357_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1357 + PI_BASE_ADDR,DENALI_PI_1357_DATA,0);
       #else
         REGWR(1357 + PI_BASE_ADDR,DENALI_PI_1357_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1358_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1358 + PI_BASE_ADDR,DENALI_PI_1358_DATA,0);
       #else
         REGWR(1358 + PI_BASE_ADDR,DENALI_PI_1358_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1359_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1359 + PI_BASE_ADDR,DENALI_PI_1359_DATA,0);
       #else
         REGWR(1359 + PI_BASE_ADDR,DENALI_PI_1359_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1360_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1360 + PI_BASE_ADDR,DENALI_PI_1360_DATA,0);
       #else
         REGWR(1360 + PI_BASE_ADDR,DENALI_PI_1360_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1361_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1361 + PI_BASE_ADDR,DENALI_PI_1361_DATA,0);
       #else
         REGWR(1361 + PI_BASE_ADDR,DENALI_PI_1361_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1362_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1362 + PI_BASE_ADDR,DENALI_PI_1362_DATA,0);
       #else
         REGWR(1362 + PI_BASE_ADDR,DENALI_PI_1362_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1363_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1363 + PI_BASE_ADDR,DENALI_PI_1363_DATA,0);
       #else
         REGWR(1363 + PI_BASE_ADDR,DENALI_PI_1363_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1364_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1364 + PI_BASE_ADDR,DENALI_PI_1364_DATA,0);
       #else
         REGWR(1364 + PI_BASE_ADDR,DENALI_PI_1364_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1365_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1365 + PI_BASE_ADDR,DENALI_PI_1365_DATA,0);
       #else
         REGWR(1365 + PI_BASE_ADDR,DENALI_PI_1365_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1366_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1366 + PI_BASE_ADDR,DENALI_PI_1366_DATA,0);
       #else
         REGWR(1366 + PI_BASE_ADDR,DENALI_PI_1366_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1367_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1367 + PI_BASE_ADDR,DENALI_PI_1367_DATA,0);
       #else
         REGWR(1367 + PI_BASE_ADDR,DENALI_PI_1367_DATA,0);
       #endif
#endif
#ifdef DENALI_PI_1368_DATA
       #ifdef REG_BACKDOOR_ACCESS
         REGWR_PI(1368 + PI_BASE_ADDR,DENALI_PI_1368_DATA,0);
       #else
         REGWR(1368 + PI_BASE_ADDR,DENALI_PI_1368_DATA,0);
       #endif
#endif

}//}}}

void config_phy(){//{{{
#ifdef DENALI_PHY_1280_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1280 + PHY_BASE_ADDR,DENALI_PHY_1280_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1280 + PHY_BASE_ADDR,DENALI_PHY_1280_DATA,0);
  #else
         REGWR(1280 + PHY_BASE_ADDR,DENALI_PHY_1280_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1281_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1281 + PHY_BASE_ADDR,DENALI_PHY_1281_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1281 + PHY_BASE_ADDR,DENALI_PHY_1281_DATA,0);
  #else
         REGWR(1281 + PHY_BASE_ADDR,DENALI_PHY_1281_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1282_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1282 + PHY_BASE_ADDR,DENALI_PHY_1282_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1282 + PHY_BASE_ADDR,DENALI_PHY_1282_DATA,0);
  #else
         REGWR(1282 + PHY_BASE_ADDR,DENALI_PHY_1282_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1283_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1283 + PHY_BASE_ADDR,DENALI_PHY_1283_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1283 + PHY_BASE_ADDR,DENALI_PHY_1283_DATA,0);
  #else
         REGWR(1283 + PHY_BASE_ADDR,DENALI_PHY_1283_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1284_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1284 + PHY_BASE_ADDR,DENALI_PHY_1284_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1284 + PHY_BASE_ADDR,DENALI_PHY_1284_DATA,0);
  #else
         REGWR(1284 + PHY_BASE_ADDR,DENALI_PHY_1284_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1285_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1285 + PHY_BASE_ADDR,DENALI_PHY_1285_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1285 + PHY_BASE_ADDR,DENALI_PHY_1285_DATA,0);
  #else
         REGWR(1285 + PHY_BASE_ADDR,DENALI_PHY_1285_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1286_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1286 + PHY_BASE_ADDR,DENALI_PHY_1286_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1286 + PHY_BASE_ADDR,DENALI_PHY_1286_DATA,0);
  #else
         REGWR(1286 + PHY_BASE_ADDR,DENALI_PHY_1286_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1287_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1287 + PHY_BASE_ADDR,DENALI_PHY_1287_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1287 + PHY_BASE_ADDR,DENALI_PHY_1287_DATA,0);
  #else
         REGWR(1287 + PHY_BASE_ADDR,DENALI_PHY_1287_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1288_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1288 + PHY_BASE_ADDR,DENALI_PHY_1288_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1288 + PHY_BASE_ADDR,DENALI_PHY_1288_DATA,0);
  #else
         REGWR(1288 + PHY_BASE_ADDR,DENALI_PHY_1288_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1289_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1289 + PHY_BASE_ADDR,DENALI_PHY_1289_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1289 + PHY_BASE_ADDR,DENALI_PHY_1289_DATA,0);
  #else
         REGWR(1289 + PHY_BASE_ADDR,DENALI_PHY_1289_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1290_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1290 + PHY_BASE_ADDR,DENALI_PHY_1290_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1290 + PHY_BASE_ADDR,DENALI_PHY_1290_DATA,0);
  #else
         REGWR(1290 + PHY_BASE_ADDR,DENALI_PHY_1290_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1291_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1291 + PHY_BASE_ADDR,DENALI_PHY_1291_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1291 + PHY_BASE_ADDR,DENALI_PHY_1291_DATA,0);
  #else
         REGWR(1291 + PHY_BASE_ADDR,DENALI_PHY_1291_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1292_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1292 + PHY_BASE_ADDR,DENALI_PHY_1292_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1292 + PHY_BASE_ADDR,DENALI_PHY_1292_DATA,0);
  #else
         REGWR(1292 + PHY_BASE_ADDR,DENALI_PHY_1292_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1293_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1293 + PHY_BASE_ADDR,DENALI_PHY_1293_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1293 + PHY_BASE_ADDR,DENALI_PHY_1293_DATA,0);
  #else
         REGWR(1293 + PHY_BASE_ADDR,DENALI_PHY_1293_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1294_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1294 + PHY_BASE_ADDR,DENALI_PHY_1294_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1294 + PHY_BASE_ADDR,DENALI_PHY_1294_DATA,0);
  #else
         REGWR(1294 + PHY_BASE_ADDR,DENALI_PHY_1294_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1295_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1295 + PHY_BASE_ADDR,DENALI_PHY_1295_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1295 + PHY_BASE_ADDR,DENALI_PHY_1295_DATA,0);
  #else
         REGWR(1295 + PHY_BASE_ADDR,DENALI_PHY_1295_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1296_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1296 + PHY_BASE_ADDR,DENALI_PHY_1296_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1296 + PHY_BASE_ADDR,DENALI_PHY_1296_DATA,0);
  #else
         REGWR(1296 + PHY_BASE_ADDR,DENALI_PHY_1296_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1297_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1297 + PHY_BASE_ADDR,DENALI_PHY_1297_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1297 + PHY_BASE_ADDR,DENALI_PHY_1297_DATA,0);
  #else
         REGWR(1297 + PHY_BASE_ADDR,DENALI_PHY_1297_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1298_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1298 + PHY_BASE_ADDR,DENALI_PHY_1298_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1298 + PHY_BASE_ADDR,DENALI_PHY_1298_DATA,0);
  #else
         REGWR(1298 + PHY_BASE_ADDR,DENALI_PHY_1298_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1299_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1299 + PHY_BASE_ADDR,DENALI_PHY_1299_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1299 + PHY_BASE_ADDR,DENALI_PHY_1299_DATA,0);
  #else
         REGWR(1299 + PHY_BASE_ADDR,DENALI_PHY_1299_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1300_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1300 + PHY_BASE_ADDR,DENALI_PHY_1300_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1300 + PHY_BASE_ADDR,DENALI_PHY_1300_DATA,0);
  #else
         REGWR(1300 + PHY_BASE_ADDR,DENALI_PHY_1300_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1301_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1301 + PHY_BASE_ADDR,DENALI_PHY_1301_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1301 + PHY_BASE_ADDR,DENALI_PHY_1301_DATA,0);
  #else
         REGWR(1301 + PHY_BASE_ADDR,DENALI_PHY_1301_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1302_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1302 + PHY_BASE_ADDR,DENALI_PHY_1302_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1302 + PHY_BASE_ADDR,DENALI_PHY_1302_DATA,0);
  #else
         REGWR(1302 + PHY_BASE_ADDR,DENALI_PHY_1302_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1303_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1303 + PHY_BASE_ADDR,DENALI_PHY_1303_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1303 + PHY_BASE_ADDR,DENALI_PHY_1303_DATA,0);
  #else
         REGWR(1303 + PHY_BASE_ADDR,DENALI_PHY_1303_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1304_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1304 + PHY_BASE_ADDR,DENALI_PHY_1304_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1304 + PHY_BASE_ADDR,DENALI_PHY_1304_DATA,0);
  #else
         REGWR(1304 + PHY_BASE_ADDR,DENALI_PHY_1304_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1305_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1305 + PHY_BASE_ADDR,DENALI_PHY_1305_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1305 + PHY_BASE_ADDR,DENALI_PHY_1305_DATA,0);
  #else
         REGWR(1305 + PHY_BASE_ADDR,DENALI_PHY_1305_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1306_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1306 + PHY_BASE_ADDR,DENALI_PHY_1306_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1306 + PHY_BASE_ADDR,DENALI_PHY_1306_DATA,0);
  #else
         REGWR(1306 + PHY_BASE_ADDR,DENALI_PHY_1306_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1307_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1307 + PHY_BASE_ADDR,DENALI_PHY_1307_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1307 + PHY_BASE_ADDR,DENALI_PHY_1307_DATA,0);
  #else
         REGWR(1307 + PHY_BASE_ADDR,DENALI_PHY_1307_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1308_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1308 + PHY_BASE_ADDR,DENALI_PHY_1308_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1308 + PHY_BASE_ADDR,DENALI_PHY_1308_DATA,0);
  #else
         REGWR(1308 + PHY_BASE_ADDR,DENALI_PHY_1308_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1309_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1309 + PHY_BASE_ADDR,DENALI_PHY_1309_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1309 + PHY_BASE_ADDR,DENALI_PHY_1309_DATA,0);
  #else
         REGWR(1309 + PHY_BASE_ADDR,DENALI_PHY_1309_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1310_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1310 + PHY_BASE_ADDR,DENALI_PHY_1310_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1310 + PHY_BASE_ADDR,DENALI_PHY_1310_DATA,0);
  #else
         REGWR(1310 + PHY_BASE_ADDR,DENALI_PHY_1310_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1311_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1311 + PHY_BASE_ADDR,DENALI_PHY_1311_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1311 + PHY_BASE_ADDR,DENALI_PHY_1311_DATA,0);
  #else
         REGWR(1311 + PHY_BASE_ADDR,DENALI_PHY_1311_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1312_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1312 + PHY_BASE_ADDR,DENALI_PHY_1312_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1312 + PHY_BASE_ADDR,DENALI_PHY_1312_DATA,0);
  #else
         REGWR(1312 + PHY_BASE_ADDR,DENALI_PHY_1312_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1313_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1313 + PHY_BASE_ADDR,DENALI_PHY_1313_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1313 + PHY_BASE_ADDR,DENALI_PHY_1313_DATA,0);
  #else
         REGWR(1313 + PHY_BASE_ADDR,DENALI_PHY_1313_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1314_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1314 + PHY_BASE_ADDR,DENALI_PHY_1314_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1314 + PHY_BASE_ADDR,DENALI_PHY_1314_DATA,0);
  #else
         REGWR(1314 + PHY_BASE_ADDR,DENALI_PHY_1314_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1315_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1315 + PHY_BASE_ADDR,DENALI_PHY_1315_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1315 + PHY_BASE_ADDR,DENALI_PHY_1315_DATA,0);
  #else
         REGWR(1315 + PHY_BASE_ADDR,DENALI_PHY_1315_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1316_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1316 + PHY_BASE_ADDR,DENALI_PHY_1316_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1316 + PHY_BASE_ADDR,DENALI_PHY_1316_DATA,0);
  #else
         REGWR(1316 + PHY_BASE_ADDR,DENALI_PHY_1316_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1317_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1317 + PHY_BASE_ADDR,DENALI_PHY_1317_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1317 + PHY_BASE_ADDR,DENALI_PHY_1317_DATA,0);
  #else
         REGWR(1317 + PHY_BASE_ADDR,DENALI_PHY_1317_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1318_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1318 + PHY_BASE_ADDR,DENALI_PHY_1318_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1318 + PHY_BASE_ADDR,DENALI_PHY_1318_DATA,0);
  #else
         REGWR(1318 + PHY_BASE_ADDR,DENALI_PHY_1318_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1319_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1319 + PHY_BASE_ADDR,DENALI_PHY_1319_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1319 + PHY_BASE_ADDR,DENALI_PHY_1319_DATA,0);
  #else
         REGWR(1319 + PHY_BASE_ADDR,DENALI_PHY_1319_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1320_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1320 + PHY_BASE_ADDR,DENALI_PHY_1320_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1320 + PHY_BASE_ADDR,DENALI_PHY_1320_DATA,0);
  #else
         REGWR(1320 + PHY_BASE_ADDR,DENALI_PHY_1320_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1321_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1321 + PHY_BASE_ADDR,DENALI_PHY_1321_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1321 + PHY_BASE_ADDR,DENALI_PHY_1321_DATA,0);
  #else
         REGWR(1321 + PHY_BASE_ADDR,DENALI_PHY_1321_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1322_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1322 + PHY_BASE_ADDR,DENALI_PHY_1322_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1322 + PHY_BASE_ADDR,DENALI_PHY_1322_DATA,0);
  #else
         REGWR(1322 + PHY_BASE_ADDR,DENALI_PHY_1322_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1323_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1323 + PHY_BASE_ADDR,DENALI_PHY_1323_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1323 + PHY_BASE_ADDR,DENALI_PHY_1323_DATA,0);
  #else
         REGWR(1323 + PHY_BASE_ADDR,DENALI_PHY_1323_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1324_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1324 + PHY_BASE_ADDR,DENALI_PHY_1324_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1324 + PHY_BASE_ADDR,DENALI_PHY_1324_DATA,0);
  #else
         REGWR(1324 + PHY_BASE_ADDR,DENALI_PHY_1324_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1325_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1325 + PHY_BASE_ADDR,DENALI_PHY_1325_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1325 + PHY_BASE_ADDR,DENALI_PHY_1325_DATA,0);
  #else
         REGWR(1325 + PHY_BASE_ADDR,DENALI_PHY_1325_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1326_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1326 + PHY_BASE_ADDR,DENALI_PHY_1326_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1326 + PHY_BASE_ADDR,DENALI_PHY_1326_DATA,0);
  #else
         REGWR(1326 + PHY_BASE_ADDR,DENALI_PHY_1326_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1327_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1327 + PHY_BASE_ADDR,DENALI_PHY_1327_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1327 + PHY_BASE_ADDR,DENALI_PHY_1327_DATA,0);
  #else
         REGWR(1327 + PHY_BASE_ADDR,DENALI_PHY_1327_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1328_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1328 + PHY_BASE_ADDR,DENALI_PHY_1328_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1328 + PHY_BASE_ADDR,DENALI_PHY_1328_DATA,0);
  #else
         REGWR(1328 + PHY_BASE_ADDR,DENALI_PHY_1328_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1329_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1329 + PHY_BASE_ADDR,DENALI_PHY_1329_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1329 + PHY_BASE_ADDR,DENALI_PHY_1329_DATA,0);
  #else
         REGWR(1329 + PHY_BASE_ADDR,DENALI_PHY_1329_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1330_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1330 + PHY_BASE_ADDR,DENALI_PHY_1330_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1330 + PHY_BASE_ADDR,DENALI_PHY_1330_DATA,0);
  #else
         REGWR(1330 + PHY_BASE_ADDR,DENALI_PHY_1330_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1331_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1331 + PHY_BASE_ADDR,DENALI_PHY_1331_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1331 + PHY_BASE_ADDR,DENALI_PHY_1331_DATA,0);
  #else
         REGWR(1331 + PHY_BASE_ADDR,DENALI_PHY_1331_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1332_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1332 + PHY_BASE_ADDR,DENALI_PHY_1332_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1332 + PHY_BASE_ADDR,DENALI_PHY_1332_DATA,0);
  #else
         REGWR(1332 + PHY_BASE_ADDR,DENALI_PHY_1332_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1333_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1333 + PHY_BASE_ADDR,DENALI_PHY_1333_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1333 + PHY_BASE_ADDR,DENALI_PHY_1333_DATA,0);
  #else
         REGWR(1333 + PHY_BASE_ADDR,DENALI_PHY_1333_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1334_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1334 + PHY_BASE_ADDR,DENALI_PHY_1334_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1334 + PHY_BASE_ADDR,DENALI_PHY_1334_DATA,0);
  #else
         REGWR(1334 + PHY_BASE_ADDR,DENALI_PHY_1334_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1335_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1335 + PHY_BASE_ADDR,DENALI_PHY_1335_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1335 + PHY_BASE_ADDR,DENALI_PHY_1335_DATA,0);
  #else
         REGWR(1335 + PHY_BASE_ADDR,DENALI_PHY_1335_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1336_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1336 + PHY_BASE_ADDR,DENALI_PHY_1336_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1336 + PHY_BASE_ADDR,DENALI_PHY_1336_DATA,0);
  #else
         REGWR(1336 + PHY_BASE_ADDR,DENALI_PHY_1336_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1337_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1337 + PHY_BASE_ADDR,DENALI_PHY_1337_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1337 + PHY_BASE_ADDR,DENALI_PHY_1337_DATA,0);
  #else
         REGWR(1337 + PHY_BASE_ADDR,DENALI_PHY_1337_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1338_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1338 + PHY_BASE_ADDR,DENALI_PHY_1338_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1338 + PHY_BASE_ADDR,DENALI_PHY_1338_DATA,0);
  #else
         REGWR(1338 + PHY_BASE_ADDR,DENALI_PHY_1338_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1339_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1339 + PHY_BASE_ADDR,DENALI_PHY_1339_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1339 + PHY_BASE_ADDR,DENALI_PHY_1339_DATA,0);
  #else
         REGWR(1339 + PHY_BASE_ADDR,DENALI_PHY_1339_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1340_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1340 + PHY_BASE_ADDR,DENALI_PHY_1340_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1340 + PHY_BASE_ADDR,DENALI_PHY_1340_DATA,0);
  #else
         REGWR(1340 + PHY_BASE_ADDR,DENALI_PHY_1340_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1341_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1341 + PHY_BASE_ADDR,DENALI_PHY_1341_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1341 + PHY_BASE_ADDR,DENALI_PHY_1341_DATA,0);
  #else
         REGWR(1341 + PHY_BASE_ADDR,DENALI_PHY_1341_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1342_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1342 + PHY_BASE_ADDR,DENALI_PHY_1342_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1342 + PHY_BASE_ADDR,DENALI_PHY_1342_DATA,0);
  #else
         REGWR(1342 + PHY_BASE_ADDR,DENALI_PHY_1342_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1343_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1343 + PHY_BASE_ADDR,DENALI_PHY_1343_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1343 + PHY_BASE_ADDR,DENALI_PHY_1343_DATA,0);
  #else
         REGWR(1343 + PHY_BASE_ADDR,DENALI_PHY_1343_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1344_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1344 + PHY_BASE_ADDR,DENALI_PHY_1344_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1344 + PHY_BASE_ADDR,DENALI_PHY_1344_DATA,0);
  #else
         REGWR(1344 + PHY_BASE_ADDR,DENALI_PHY_1344_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1345_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1345 + PHY_BASE_ADDR,DENALI_PHY_1345_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1345 + PHY_BASE_ADDR,DENALI_PHY_1345_DATA,0);
  #else
         REGWR(1345 + PHY_BASE_ADDR,DENALI_PHY_1345_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1346_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1346 + PHY_BASE_ADDR,DENALI_PHY_1346_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1346 + PHY_BASE_ADDR,DENALI_PHY_1346_DATA,0);
  #else
         REGWR(1346 + PHY_BASE_ADDR,DENALI_PHY_1346_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1347_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1347 + PHY_BASE_ADDR,DENALI_PHY_1347_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1347 + PHY_BASE_ADDR,DENALI_PHY_1347_DATA,0);
  #else
         REGWR(1347 + PHY_BASE_ADDR,DENALI_PHY_1347_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1348_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1348 + PHY_BASE_ADDR,DENALI_PHY_1348_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1348 + PHY_BASE_ADDR,DENALI_PHY_1348_DATA,0);
  #else
         REGWR(1348 + PHY_BASE_ADDR,DENALI_PHY_1348_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1349_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1349 + PHY_BASE_ADDR,DENALI_PHY_1349_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1349 + PHY_BASE_ADDR,DENALI_PHY_1349_DATA,0);
  #else
         REGWR(1349 + PHY_BASE_ADDR,DENALI_PHY_1349_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1350_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1350 + PHY_BASE_ADDR,DENALI_PHY_1350_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1350 + PHY_BASE_ADDR,DENALI_PHY_1350_DATA,0);
  #else
         REGWR(1350 + PHY_BASE_ADDR,DENALI_PHY_1350_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1351_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1351 + PHY_BASE_ADDR,DENALI_PHY_1351_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1351 + PHY_BASE_ADDR,DENALI_PHY_1351_DATA,0);
  #else
         REGWR(1351 + PHY_BASE_ADDR,DENALI_PHY_1351_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1352_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1352 + PHY_BASE_ADDR,DENALI_PHY_1352_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1352 + PHY_BASE_ADDR,DENALI_PHY_1352_DATA,0);
  #else
         REGWR(1352 + PHY_BASE_ADDR,DENALI_PHY_1352_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1353_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1353 + PHY_BASE_ADDR,DENALI_PHY_1353_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1353 + PHY_BASE_ADDR,DENALI_PHY_1353_DATA,0);
  #else
         REGWR(1353 + PHY_BASE_ADDR,DENALI_PHY_1353_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1354_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1354 + PHY_BASE_ADDR,DENALI_PHY_1354_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1354 + PHY_BASE_ADDR,DENALI_PHY_1354_DATA,0);
  #else
         REGWR(1354 + PHY_BASE_ADDR,DENALI_PHY_1354_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1355_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1355 + PHY_BASE_ADDR,DENALI_PHY_1355_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1355 + PHY_BASE_ADDR,DENALI_PHY_1355_DATA,0);
  #else
         REGWR(1355 + PHY_BASE_ADDR,DENALI_PHY_1355_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1356_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1356 + PHY_BASE_ADDR,DENALI_PHY_1356_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1356 + PHY_BASE_ADDR,DENALI_PHY_1356_DATA,0);
  #else
         REGWR(1356 + PHY_BASE_ADDR,DENALI_PHY_1356_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1357_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1357 + PHY_BASE_ADDR,DENALI_PHY_1357_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1357 + PHY_BASE_ADDR,DENALI_PHY_1357_DATA,0);
  #else
         REGWR(1357 + PHY_BASE_ADDR,DENALI_PHY_1357_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1358_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1358 + PHY_BASE_ADDR,DENALI_PHY_1358_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1358 + PHY_BASE_ADDR,DENALI_PHY_1358_DATA,0);
  #else
         REGWR(1358 + PHY_BASE_ADDR,DENALI_PHY_1358_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1359_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1359 + PHY_BASE_ADDR,DENALI_PHY_1359_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1359 + PHY_BASE_ADDR,DENALI_PHY_1359_DATA,0);
  #else
         REGWR(1359 + PHY_BASE_ADDR,DENALI_PHY_1359_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1360_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1360 + PHY_BASE_ADDR,DENALI_PHY_1360_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1360 + PHY_BASE_ADDR,DENALI_PHY_1360_DATA,0);
  #else
         REGWR(1360 + PHY_BASE_ADDR,DENALI_PHY_1360_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1361_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1361 + PHY_BASE_ADDR,DENALI_PHY_1361_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1361 + PHY_BASE_ADDR,DENALI_PHY_1361_DATA,0);
  #else
         REGWR(1361 + PHY_BASE_ADDR,DENALI_PHY_1361_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1362_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1362 + PHY_BASE_ADDR,DENALI_PHY_1362_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1362 + PHY_BASE_ADDR,DENALI_PHY_1362_DATA,0);
  #else
         REGWR(1362 + PHY_BASE_ADDR,DENALI_PHY_1362_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1363_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1363 + PHY_BASE_ADDR,DENALI_PHY_1363_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1363 + PHY_BASE_ADDR,DENALI_PHY_1363_DATA,0);
  #else
         REGWR(1363 + PHY_BASE_ADDR,DENALI_PHY_1363_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1364_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1364 + PHY_BASE_ADDR,DENALI_PHY_1364_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1364 + PHY_BASE_ADDR,DENALI_PHY_1364_DATA,0);
  #else
         REGWR(1364 + PHY_BASE_ADDR,DENALI_PHY_1364_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1365_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1365 + PHY_BASE_ADDR,DENALI_PHY_1365_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1365 + PHY_BASE_ADDR,DENALI_PHY_1365_DATA,0);
  #else
         REGWR(1365 + PHY_BASE_ADDR,DENALI_PHY_1365_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1366_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1366 + PHY_BASE_ADDR,DENALI_PHY_1366_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1366 + PHY_BASE_ADDR,DENALI_PHY_1366_DATA,0);
  #else
         REGWR(1366 + PHY_BASE_ADDR,DENALI_PHY_1366_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1367_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1367 + PHY_BASE_ADDR,DENALI_PHY_1367_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1367 + PHY_BASE_ADDR,DENALI_PHY_1367_DATA,0);
  #else
         REGWR(1367 + PHY_BASE_ADDR,DENALI_PHY_1367_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_00_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(0 + PHY_BASE_ADDR,DENALI_PHY_00_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(0 + PHY_BASE_ADDR,DENALI_PHY_00_DATA,0);
  #else
         REGWR(0 + PHY_BASE_ADDR,DENALI_PHY_00_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_01_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1 + PHY_BASE_ADDR,DENALI_PHY_01_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1 + PHY_BASE_ADDR,DENALI_PHY_01_DATA,0);
  #else
         REGWR(1 + PHY_BASE_ADDR,DENALI_PHY_01_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_02_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(2 + PHY_BASE_ADDR,DENALI_PHY_02_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(2 + PHY_BASE_ADDR,DENALI_PHY_02_DATA,0);
  #else
         REGWR(2 + PHY_BASE_ADDR,DENALI_PHY_02_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_03_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(3 + PHY_BASE_ADDR,DENALI_PHY_03_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(3 + PHY_BASE_ADDR,DENALI_PHY_03_DATA,0);
  #else
         REGWR(3 + PHY_BASE_ADDR,DENALI_PHY_03_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_04_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(4 + PHY_BASE_ADDR,DENALI_PHY_04_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(4 + PHY_BASE_ADDR,DENALI_PHY_04_DATA,0);
  #else
         REGWR(4 + PHY_BASE_ADDR,DENALI_PHY_04_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_05_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(5 + PHY_BASE_ADDR,DENALI_PHY_05_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(5 + PHY_BASE_ADDR,DENALI_PHY_05_DATA,0);
  #else
         REGWR(5 + PHY_BASE_ADDR,DENALI_PHY_05_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_06_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(6 + PHY_BASE_ADDR,DENALI_PHY_06_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(6 + PHY_BASE_ADDR,DENALI_PHY_06_DATA,0);
  #else
         REGWR(6 + PHY_BASE_ADDR,DENALI_PHY_06_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_07_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(7 + PHY_BASE_ADDR,DENALI_PHY_07_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(7 + PHY_BASE_ADDR,DENALI_PHY_07_DATA,0);
  #else
         REGWR(7 + PHY_BASE_ADDR,DENALI_PHY_07_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_08_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(8 + PHY_BASE_ADDR,DENALI_PHY_08_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(8 + PHY_BASE_ADDR,DENALI_PHY_08_DATA,0);
  #else
         REGWR(8 + PHY_BASE_ADDR,DENALI_PHY_08_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_09_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(9 + PHY_BASE_ADDR,DENALI_PHY_09_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(9 + PHY_BASE_ADDR,DENALI_PHY_09_DATA,0);
  #else
         REGWR(9 + PHY_BASE_ADDR,DENALI_PHY_09_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_10_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(10 + PHY_BASE_ADDR,DENALI_PHY_10_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(10 + PHY_BASE_ADDR,DENALI_PHY_10_DATA,0);
  #else
         REGWR(10 + PHY_BASE_ADDR,DENALI_PHY_10_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_11_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(11 + PHY_BASE_ADDR,DENALI_PHY_11_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(11 + PHY_BASE_ADDR,DENALI_PHY_11_DATA,0);
  #else
         REGWR(11 + PHY_BASE_ADDR,DENALI_PHY_11_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_12_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(12 + PHY_BASE_ADDR,DENALI_PHY_12_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(12 + PHY_BASE_ADDR,DENALI_PHY_12_DATA,0);
  #else
         REGWR(12 + PHY_BASE_ADDR,DENALI_PHY_12_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_13_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(13 + PHY_BASE_ADDR,DENALI_PHY_13_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(13 + PHY_BASE_ADDR,DENALI_PHY_13_DATA,0);
  #else
         REGWR(13 + PHY_BASE_ADDR,DENALI_PHY_13_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_14_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(14 + PHY_BASE_ADDR,DENALI_PHY_14_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(14 + PHY_BASE_ADDR,DENALI_PHY_14_DATA,0);
  #else
         REGWR(14 + PHY_BASE_ADDR,DENALI_PHY_14_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_15_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(15 + PHY_BASE_ADDR,DENALI_PHY_15_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(15 + PHY_BASE_ADDR,DENALI_PHY_15_DATA,0);
  #else
         REGWR(15 + PHY_BASE_ADDR,DENALI_PHY_15_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_16_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(16 + PHY_BASE_ADDR,DENALI_PHY_16_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(16 + PHY_BASE_ADDR,DENALI_PHY_16_DATA,0);
  #else
         REGWR(16 + PHY_BASE_ADDR,DENALI_PHY_16_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_17_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(17 + PHY_BASE_ADDR,DENALI_PHY_17_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(17 + PHY_BASE_ADDR,DENALI_PHY_17_DATA,0);
  #else
         REGWR(17 + PHY_BASE_ADDR,DENALI_PHY_17_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_18_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(18 + PHY_BASE_ADDR,DENALI_PHY_18_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(18 + PHY_BASE_ADDR,DENALI_PHY_18_DATA,0);
  #else
         REGWR(18 + PHY_BASE_ADDR,DENALI_PHY_18_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_19_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(19 + PHY_BASE_ADDR,DENALI_PHY_19_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(19 + PHY_BASE_ADDR,DENALI_PHY_19_DATA,0);
  #else
         REGWR(19 + PHY_BASE_ADDR,DENALI_PHY_19_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_20_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(20 + PHY_BASE_ADDR,DENALI_PHY_20_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(20 + PHY_BASE_ADDR,DENALI_PHY_20_DATA,0);
  #else
         REGWR(20 + PHY_BASE_ADDR,DENALI_PHY_20_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_21_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(21 + PHY_BASE_ADDR,DENALI_PHY_21_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(21 + PHY_BASE_ADDR,DENALI_PHY_21_DATA,0);
  #else
         REGWR(21 + PHY_BASE_ADDR,DENALI_PHY_21_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_22_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(22 + PHY_BASE_ADDR,DENALI_PHY_22_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(22 + PHY_BASE_ADDR,DENALI_PHY_22_DATA,0);
  #else
         REGWR(22 + PHY_BASE_ADDR,DENALI_PHY_22_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_23_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(23 + PHY_BASE_ADDR,DENALI_PHY_23_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(23 + PHY_BASE_ADDR,DENALI_PHY_23_DATA,0);
  #else
         REGWR(23 + PHY_BASE_ADDR,DENALI_PHY_23_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_24_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(24 + PHY_BASE_ADDR,DENALI_PHY_24_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(24 + PHY_BASE_ADDR,DENALI_PHY_24_DATA,0);
  #else
         REGWR(24 + PHY_BASE_ADDR,DENALI_PHY_24_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_25_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(25 + PHY_BASE_ADDR,DENALI_PHY_25_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(25 + PHY_BASE_ADDR,DENALI_PHY_25_DATA,0);
  #else
         REGWR(25 + PHY_BASE_ADDR,DENALI_PHY_25_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_26_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(26 + PHY_BASE_ADDR,DENALI_PHY_26_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(26 + PHY_BASE_ADDR,DENALI_PHY_26_DATA,0);
  #else
         REGWR(26 + PHY_BASE_ADDR,DENALI_PHY_26_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_27_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(27 + PHY_BASE_ADDR,DENALI_PHY_27_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(27 + PHY_BASE_ADDR,DENALI_PHY_27_DATA,0);
  #else
         REGWR(27 + PHY_BASE_ADDR,DENALI_PHY_27_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_28_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(28 + PHY_BASE_ADDR,DENALI_PHY_28_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(28 + PHY_BASE_ADDR,DENALI_PHY_28_DATA,0);
  #else
         REGWR(28 + PHY_BASE_ADDR,DENALI_PHY_28_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_29_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(29 + PHY_BASE_ADDR,DENALI_PHY_29_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(29 + PHY_BASE_ADDR,DENALI_PHY_29_DATA,0);
  #else
         REGWR(29 + PHY_BASE_ADDR,DENALI_PHY_29_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_30_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(30 + PHY_BASE_ADDR,DENALI_PHY_30_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(30 + PHY_BASE_ADDR,DENALI_PHY_30_DATA,0);
  #else
         REGWR(30 + PHY_BASE_ADDR,DENALI_PHY_30_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_31_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(31 + PHY_BASE_ADDR,DENALI_PHY_31_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(31 + PHY_BASE_ADDR,DENALI_PHY_31_DATA,0);
  #else
         REGWR(31 + PHY_BASE_ADDR,DENALI_PHY_31_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_32_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(32 + PHY_BASE_ADDR,DENALI_PHY_32_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(32 + PHY_BASE_ADDR,DENALI_PHY_32_DATA,0);
  #else
         REGWR(32 + PHY_BASE_ADDR,DENALI_PHY_32_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_33_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(33 + PHY_BASE_ADDR,DENALI_PHY_33_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(33 + PHY_BASE_ADDR,DENALI_PHY_33_DATA,0);
  #else
         REGWR(33 + PHY_BASE_ADDR,DENALI_PHY_33_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_34_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(34 + PHY_BASE_ADDR,DENALI_PHY_34_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(34 + PHY_BASE_ADDR,DENALI_PHY_34_DATA,0);
  #else
         REGWR(34 + PHY_BASE_ADDR,DENALI_PHY_34_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_35_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(35 + PHY_BASE_ADDR,DENALI_PHY_35_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(35 + PHY_BASE_ADDR,DENALI_PHY_35_DATA,0);
  #else
         REGWR(35 + PHY_BASE_ADDR,DENALI_PHY_35_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_36_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(36 + PHY_BASE_ADDR,DENALI_PHY_36_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(36 + PHY_BASE_ADDR,DENALI_PHY_36_DATA,0);
  #else
         REGWR(36 + PHY_BASE_ADDR,DENALI_PHY_36_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_37_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(37 + PHY_BASE_ADDR,DENALI_PHY_37_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(37 + PHY_BASE_ADDR,DENALI_PHY_37_DATA,0);
  #else
         REGWR(37 + PHY_BASE_ADDR,DENALI_PHY_37_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_38_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(38 + PHY_BASE_ADDR,DENALI_PHY_38_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(38 + PHY_BASE_ADDR,DENALI_PHY_38_DATA,0);
  #else
         REGWR(38 + PHY_BASE_ADDR,DENALI_PHY_38_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_39_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(39 + PHY_BASE_ADDR,DENALI_PHY_39_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(39 + PHY_BASE_ADDR,DENALI_PHY_39_DATA,0);
  #else
         REGWR(39 + PHY_BASE_ADDR,DENALI_PHY_39_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_40_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(40 + PHY_BASE_ADDR,DENALI_PHY_40_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(40 + PHY_BASE_ADDR,DENALI_PHY_40_DATA,0);
  #else
         REGWR(40 + PHY_BASE_ADDR,DENALI_PHY_40_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_41_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(41 + PHY_BASE_ADDR,DENALI_PHY_41_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(41 + PHY_BASE_ADDR,DENALI_PHY_41_DATA,0);
  #else
         REGWR(41 + PHY_BASE_ADDR,DENALI_PHY_41_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_42_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(42 + PHY_BASE_ADDR,DENALI_PHY_42_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(42 + PHY_BASE_ADDR,DENALI_PHY_42_DATA,0);
  #else
         REGWR(42 + PHY_BASE_ADDR,DENALI_PHY_42_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_43_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(43 + PHY_BASE_ADDR,DENALI_PHY_43_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(43 + PHY_BASE_ADDR,DENALI_PHY_43_DATA,0);
  #else
         REGWR(43 + PHY_BASE_ADDR,DENALI_PHY_43_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_44_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(44 + PHY_BASE_ADDR,DENALI_PHY_44_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(44 + PHY_BASE_ADDR,DENALI_PHY_44_DATA,0);
  #else
         REGWR(44 + PHY_BASE_ADDR,DENALI_PHY_44_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_45_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(45 + PHY_BASE_ADDR,DENALI_PHY_45_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(45 + PHY_BASE_ADDR,DENALI_PHY_45_DATA,0);
  #else
         REGWR(45 + PHY_BASE_ADDR,DENALI_PHY_45_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_46_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(46 + PHY_BASE_ADDR,DENALI_PHY_46_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(46 + PHY_BASE_ADDR,DENALI_PHY_46_DATA,0);
  #else
         REGWR(46 + PHY_BASE_ADDR,DENALI_PHY_46_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_47_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(47 + PHY_BASE_ADDR,DENALI_PHY_47_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(47 + PHY_BASE_ADDR,DENALI_PHY_47_DATA,0);
  #else
         REGWR(47 + PHY_BASE_ADDR,DENALI_PHY_47_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_48_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(48 + PHY_BASE_ADDR,DENALI_PHY_48_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(48 + PHY_BASE_ADDR,DENALI_PHY_48_DATA,0);
  #else
         REGWR(48 + PHY_BASE_ADDR,DENALI_PHY_48_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_49_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(49 + PHY_BASE_ADDR,DENALI_PHY_49_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(49 + PHY_BASE_ADDR,DENALI_PHY_49_DATA,0);
  #else
         REGWR(49 + PHY_BASE_ADDR,DENALI_PHY_49_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_50_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(50 + PHY_BASE_ADDR,DENALI_PHY_50_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(50 + PHY_BASE_ADDR,DENALI_PHY_50_DATA,0);
  #else
         REGWR(50 + PHY_BASE_ADDR,DENALI_PHY_50_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_51_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(51 + PHY_BASE_ADDR,DENALI_PHY_51_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(51 + PHY_BASE_ADDR,DENALI_PHY_51_DATA,0);
  #else
         REGWR(51 + PHY_BASE_ADDR,DENALI_PHY_51_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_52_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(52 + PHY_BASE_ADDR,DENALI_PHY_52_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(52 + PHY_BASE_ADDR,DENALI_PHY_52_DATA,0);
  #else
         REGWR(52 + PHY_BASE_ADDR,DENALI_PHY_52_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_53_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(53 + PHY_BASE_ADDR,DENALI_PHY_53_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(53 + PHY_BASE_ADDR,DENALI_PHY_53_DATA,0);
  #else
         REGWR(53 + PHY_BASE_ADDR,DENALI_PHY_53_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_54_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(54 + PHY_BASE_ADDR,DENALI_PHY_54_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(54 + PHY_BASE_ADDR,DENALI_PHY_54_DATA,0);
  #else
         REGWR(54 + PHY_BASE_ADDR,DENALI_PHY_54_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_55_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(55 + PHY_BASE_ADDR,DENALI_PHY_55_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(55 + PHY_BASE_ADDR,DENALI_PHY_55_DATA,0);
  #else
         REGWR(55 + PHY_BASE_ADDR,DENALI_PHY_55_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_56_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(56 + PHY_BASE_ADDR,DENALI_PHY_56_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(56 + PHY_BASE_ADDR,DENALI_PHY_56_DATA,0);
  #else
         REGWR(56 + PHY_BASE_ADDR,DENALI_PHY_56_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_57_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(57 + PHY_BASE_ADDR,DENALI_PHY_57_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(57 + PHY_BASE_ADDR,DENALI_PHY_57_DATA,0);
  #else
         REGWR(57 + PHY_BASE_ADDR,DENALI_PHY_57_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_58_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(58 + PHY_BASE_ADDR,DENALI_PHY_58_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(58 + PHY_BASE_ADDR,DENALI_PHY_58_DATA,0);
  #else
         REGWR(58 + PHY_BASE_ADDR,DENALI_PHY_58_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_59_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(59 + PHY_BASE_ADDR,DENALI_PHY_59_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(59 + PHY_BASE_ADDR,DENALI_PHY_59_DATA,0);
  #else
         REGWR(59 + PHY_BASE_ADDR,DENALI_PHY_59_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_60_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(60 + PHY_BASE_ADDR,DENALI_PHY_60_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(60 + PHY_BASE_ADDR,DENALI_PHY_60_DATA,0);
  #else
         REGWR(60 + PHY_BASE_ADDR,DENALI_PHY_60_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_61_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(61 + PHY_BASE_ADDR,DENALI_PHY_61_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(61 + PHY_BASE_ADDR,DENALI_PHY_61_DATA,0);
  #else
         REGWR(61 + PHY_BASE_ADDR,DENALI_PHY_61_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_62_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(62 + PHY_BASE_ADDR,DENALI_PHY_62_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(62 + PHY_BASE_ADDR,DENALI_PHY_62_DATA,0);
  #else
         REGWR(62 + PHY_BASE_ADDR,DENALI_PHY_62_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_63_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(63 + PHY_BASE_ADDR,DENALI_PHY_63_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(63 + PHY_BASE_ADDR,DENALI_PHY_63_DATA,0);
  #else
         REGWR(63 + PHY_BASE_ADDR,DENALI_PHY_63_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_64_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(64 + PHY_BASE_ADDR,DENALI_PHY_64_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(64 + PHY_BASE_ADDR,DENALI_PHY_64_DATA,0);
  #else
         REGWR(64 + PHY_BASE_ADDR,DENALI_PHY_64_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_65_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(65 + PHY_BASE_ADDR,DENALI_PHY_65_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(65 + PHY_BASE_ADDR,DENALI_PHY_65_DATA,0);
  #else
         REGWR(65 + PHY_BASE_ADDR,DENALI_PHY_65_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_66_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(66 + PHY_BASE_ADDR,DENALI_PHY_66_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(66 + PHY_BASE_ADDR,DENALI_PHY_66_DATA,0);
  #else
         REGWR(66 + PHY_BASE_ADDR,DENALI_PHY_66_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_67_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(67 + PHY_BASE_ADDR,DENALI_PHY_67_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(67 + PHY_BASE_ADDR,DENALI_PHY_67_DATA,0);
  #else
         REGWR(67 + PHY_BASE_ADDR,DENALI_PHY_67_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_68_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(68 + PHY_BASE_ADDR,DENALI_PHY_68_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(68 + PHY_BASE_ADDR,DENALI_PHY_68_DATA,0);
  #else
         REGWR(68 + PHY_BASE_ADDR,DENALI_PHY_68_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_69_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(69 + PHY_BASE_ADDR,DENALI_PHY_69_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(69 + PHY_BASE_ADDR,DENALI_PHY_69_DATA,0);
  #else
         REGWR(69 + PHY_BASE_ADDR,DENALI_PHY_69_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_70_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(70 + PHY_BASE_ADDR,DENALI_PHY_70_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(70 + PHY_BASE_ADDR,DENALI_PHY_70_DATA,0);
  #else
         REGWR(70 + PHY_BASE_ADDR,DENALI_PHY_70_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_71_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(71 + PHY_BASE_ADDR,DENALI_PHY_71_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(71 + PHY_BASE_ADDR,DENALI_PHY_71_DATA,0);
  #else
         REGWR(71 + PHY_BASE_ADDR,DENALI_PHY_71_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_72_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(72 + PHY_BASE_ADDR,DENALI_PHY_72_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(72 + PHY_BASE_ADDR,DENALI_PHY_72_DATA,0);
  #else
         REGWR(72 + PHY_BASE_ADDR,DENALI_PHY_72_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_73_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(73 + PHY_BASE_ADDR,DENALI_PHY_73_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(73 + PHY_BASE_ADDR,DENALI_PHY_73_DATA,0);
  #else
         REGWR(73 + PHY_BASE_ADDR,DENALI_PHY_73_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_74_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(74 + PHY_BASE_ADDR,DENALI_PHY_74_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(74 + PHY_BASE_ADDR,DENALI_PHY_74_DATA,0);
  #else
         REGWR(74 + PHY_BASE_ADDR,DENALI_PHY_74_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_75_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(75 + PHY_BASE_ADDR,DENALI_PHY_75_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(75 + PHY_BASE_ADDR,DENALI_PHY_75_DATA,0);
  #else
         REGWR(75 + PHY_BASE_ADDR,DENALI_PHY_75_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_76_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(76 + PHY_BASE_ADDR,DENALI_PHY_76_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(76 + PHY_BASE_ADDR,DENALI_PHY_76_DATA,0);
  #else
         REGWR(76 + PHY_BASE_ADDR,DENALI_PHY_76_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_77_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(77 + PHY_BASE_ADDR,DENALI_PHY_77_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(77 + PHY_BASE_ADDR,DENALI_PHY_77_DATA,0);
  #else
         REGWR(77 + PHY_BASE_ADDR,DENALI_PHY_77_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_78_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(78 + PHY_BASE_ADDR,DENALI_PHY_78_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(78 + PHY_BASE_ADDR,DENALI_PHY_78_DATA,0);
  #else
         REGWR(78 + PHY_BASE_ADDR,DENALI_PHY_78_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_79_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(79 + PHY_BASE_ADDR,DENALI_PHY_79_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(79 + PHY_BASE_ADDR,DENALI_PHY_79_DATA,0);
  #else
         REGWR(79 + PHY_BASE_ADDR,DENALI_PHY_79_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_80_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(80 + PHY_BASE_ADDR,DENALI_PHY_80_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(80 + PHY_BASE_ADDR,DENALI_PHY_80_DATA,0);
  #else
         REGWR(80 + PHY_BASE_ADDR,DENALI_PHY_80_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_81_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(81 + PHY_BASE_ADDR,DENALI_PHY_81_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(81 + PHY_BASE_ADDR,DENALI_PHY_81_DATA,0);
  #else
         REGWR(81 + PHY_BASE_ADDR,DENALI_PHY_81_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_82_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(82 + PHY_BASE_ADDR,DENALI_PHY_82_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(82 + PHY_BASE_ADDR,DENALI_PHY_82_DATA,0);
  #else
         REGWR(82 + PHY_BASE_ADDR,DENALI_PHY_82_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_83_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(83 + PHY_BASE_ADDR,DENALI_PHY_83_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(83 + PHY_BASE_ADDR,DENALI_PHY_83_DATA,0);
  #else
         REGWR(83 + PHY_BASE_ADDR,DENALI_PHY_83_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_84_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(84 + PHY_BASE_ADDR,DENALI_PHY_84_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(84 + PHY_BASE_ADDR,DENALI_PHY_84_DATA,0);
  #else
         REGWR(84 + PHY_BASE_ADDR,DENALI_PHY_84_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_85_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(85 + PHY_BASE_ADDR,DENALI_PHY_85_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(85 + PHY_BASE_ADDR,DENALI_PHY_85_DATA,0);
  #else
         REGWR(85 + PHY_BASE_ADDR,DENALI_PHY_85_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_86_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(86 + PHY_BASE_ADDR,DENALI_PHY_86_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(86 + PHY_BASE_ADDR,DENALI_PHY_86_DATA,0);
  #else
         REGWR(86 + PHY_BASE_ADDR,DENALI_PHY_86_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_87_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(87 + PHY_BASE_ADDR,DENALI_PHY_87_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(87 + PHY_BASE_ADDR,DENALI_PHY_87_DATA,0);
  #else
         REGWR(87 + PHY_BASE_ADDR,DENALI_PHY_87_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_88_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(88 + PHY_BASE_ADDR,DENALI_PHY_88_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(88 + PHY_BASE_ADDR,DENALI_PHY_88_DATA,0);
  #else
         REGWR(88 + PHY_BASE_ADDR,DENALI_PHY_88_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_89_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(89 + PHY_BASE_ADDR,DENALI_PHY_89_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(89 + PHY_BASE_ADDR,DENALI_PHY_89_DATA,0);
  #else
         REGWR(89 + PHY_BASE_ADDR,DENALI_PHY_89_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_90_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(90 + PHY_BASE_ADDR,DENALI_PHY_90_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(90 + PHY_BASE_ADDR,DENALI_PHY_90_DATA,0);
  #else
         REGWR(90 + PHY_BASE_ADDR,DENALI_PHY_90_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_91_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(91 + PHY_BASE_ADDR,DENALI_PHY_91_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(91 + PHY_BASE_ADDR,DENALI_PHY_91_DATA,0);
  #else
         REGWR(91 + PHY_BASE_ADDR,DENALI_PHY_91_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_92_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(92 + PHY_BASE_ADDR,DENALI_PHY_92_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(92 + PHY_BASE_ADDR,DENALI_PHY_92_DATA,0);
  #else
         REGWR(92 + PHY_BASE_ADDR,DENALI_PHY_92_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_93_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(93 + PHY_BASE_ADDR,DENALI_PHY_93_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(93 + PHY_BASE_ADDR,DENALI_PHY_93_DATA,0);
  #else
         REGWR(93 + PHY_BASE_ADDR,DENALI_PHY_93_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_94_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(94 + PHY_BASE_ADDR,DENALI_PHY_94_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(94 + PHY_BASE_ADDR,DENALI_PHY_94_DATA,0);
  #else
         REGWR(94 + PHY_BASE_ADDR,DENALI_PHY_94_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_95_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(95 + PHY_BASE_ADDR,DENALI_PHY_95_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(95 + PHY_BASE_ADDR,DENALI_PHY_95_DATA,0);
  #else
         REGWR(95 + PHY_BASE_ADDR,DENALI_PHY_95_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_96_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(96 + PHY_BASE_ADDR,DENALI_PHY_96_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(96 + PHY_BASE_ADDR,DENALI_PHY_96_DATA,0);
  #else
         REGWR(96 + PHY_BASE_ADDR,DENALI_PHY_96_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_97_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(97 + PHY_BASE_ADDR,DENALI_PHY_97_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(97 + PHY_BASE_ADDR,DENALI_PHY_97_DATA,0);
  #else
         REGWR(97 + PHY_BASE_ADDR,DENALI_PHY_97_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_98_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(98 + PHY_BASE_ADDR,DENALI_PHY_98_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(98 + PHY_BASE_ADDR,DENALI_PHY_98_DATA,0);
  #else
         REGWR(98 + PHY_BASE_ADDR,DENALI_PHY_98_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_99_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(99 + PHY_BASE_ADDR,DENALI_PHY_99_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(99 + PHY_BASE_ADDR,DENALI_PHY_99_DATA,0);
  #else
         REGWR(99 + PHY_BASE_ADDR,DENALI_PHY_99_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_100_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(100 + PHY_BASE_ADDR,DENALI_PHY_100_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(100 + PHY_BASE_ADDR,DENALI_PHY_100_DATA,0);
  #else
         REGWR(100 + PHY_BASE_ADDR,DENALI_PHY_100_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_101_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(101 + PHY_BASE_ADDR,DENALI_PHY_101_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(101 + PHY_BASE_ADDR,DENALI_PHY_101_DATA,0);
  #else
         REGWR(101 + PHY_BASE_ADDR,DENALI_PHY_101_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_102_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(102 + PHY_BASE_ADDR,DENALI_PHY_102_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(102 + PHY_BASE_ADDR,DENALI_PHY_102_DATA,0);
  #else
         REGWR(102 + PHY_BASE_ADDR,DENALI_PHY_102_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_103_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(103 + PHY_BASE_ADDR,DENALI_PHY_103_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(103 + PHY_BASE_ADDR,DENALI_PHY_103_DATA,0);
  #else
         REGWR(103 + PHY_BASE_ADDR,DENALI_PHY_103_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_104_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(104 + PHY_BASE_ADDR,DENALI_PHY_104_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(104 + PHY_BASE_ADDR,DENALI_PHY_104_DATA,0);
  #else
         REGWR(104 + PHY_BASE_ADDR,DENALI_PHY_104_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_105_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(105 + PHY_BASE_ADDR,DENALI_PHY_105_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(105 + PHY_BASE_ADDR,DENALI_PHY_105_DATA,0);
  #else
         REGWR(105 + PHY_BASE_ADDR,DENALI_PHY_105_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_106_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(106 + PHY_BASE_ADDR,DENALI_PHY_106_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(106 + PHY_BASE_ADDR,DENALI_PHY_106_DATA,0);
  #else
         REGWR(106 + PHY_BASE_ADDR,DENALI_PHY_106_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_107_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(107 + PHY_BASE_ADDR,DENALI_PHY_107_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(107 + PHY_BASE_ADDR,DENALI_PHY_107_DATA,0);
  #else
         REGWR(107 + PHY_BASE_ADDR,DENALI_PHY_107_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_108_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(108 + PHY_BASE_ADDR,DENALI_PHY_108_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(108 + PHY_BASE_ADDR,DENALI_PHY_108_DATA,0);
  #else
         REGWR(108 + PHY_BASE_ADDR,DENALI_PHY_108_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_109_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(109 + PHY_BASE_ADDR,DENALI_PHY_109_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(109 + PHY_BASE_ADDR,DENALI_PHY_109_DATA,0);
  #else
         REGWR(109 + PHY_BASE_ADDR,DENALI_PHY_109_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_110_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(110 + PHY_BASE_ADDR,DENALI_PHY_110_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(110 + PHY_BASE_ADDR,DENALI_PHY_110_DATA,0);
  #else
         REGWR(110 + PHY_BASE_ADDR,DENALI_PHY_110_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_111_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(111 + PHY_BASE_ADDR,DENALI_PHY_111_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(111 + PHY_BASE_ADDR,DENALI_PHY_111_DATA,0);
  #else
         REGWR(111 + PHY_BASE_ADDR,DENALI_PHY_111_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_112_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(112 + PHY_BASE_ADDR,DENALI_PHY_112_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(112 + PHY_BASE_ADDR,DENALI_PHY_112_DATA,0);
  #else
         REGWR(112 + PHY_BASE_ADDR,DENALI_PHY_112_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_113_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(113 + PHY_BASE_ADDR,DENALI_PHY_113_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(113 + PHY_BASE_ADDR,DENALI_PHY_113_DATA,0);
  #else
         REGWR(113 + PHY_BASE_ADDR,DENALI_PHY_113_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_114_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(114 + PHY_BASE_ADDR,DENALI_PHY_114_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(114 + PHY_BASE_ADDR,DENALI_PHY_114_DATA,0);
  #else
         REGWR(114 + PHY_BASE_ADDR,DENALI_PHY_114_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_115_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(115 + PHY_BASE_ADDR,DENALI_PHY_115_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(115 + PHY_BASE_ADDR,DENALI_PHY_115_DATA,0);
  #else
         REGWR(115 + PHY_BASE_ADDR,DENALI_PHY_115_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_116_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(116 + PHY_BASE_ADDR,DENALI_PHY_116_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(116 + PHY_BASE_ADDR,DENALI_PHY_116_DATA,0);
  #else
         REGWR(116 + PHY_BASE_ADDR,DENALI_PHY_116_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_117_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(117 + PHY_BASE_ADDR,DENALI_PHY_117_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(117 + PHY_BASE_ADDR,DENALI_PHY_117_DATA,0);
  #else
         REGWR(117 + PHY_BASE_ADDR,DENALI_PHY_117_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_118_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(118 + PHY_BASE_ADDR,DENALI_PHY_118_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(118 + PHY_BASE_ADDR,DENALI_PHY_118_DATA,0);
  #else
         REGWR(118 + PHY_BASE_ADDR,DENALI_PHY_118_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_119_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(119 + PHY_BASE_ADDR,DENALI_PHY_119_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(119 + PHY_BASE_ADDR,DENALI_PHY_119_DATA,0);
  #else
         REGWR(119 + PHY_BASE_ADDR,DENALI_PHY_119_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_120_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(120 + PHY_BASE_ADDR,DENALI_PHY_120_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(120 + PHY_BASE_ADDR,DENALI_PHY_120_DATA,0);
  #else
         REGWR(120 + PHY_BASE_ADDR,DENALI_PHY_120_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_121_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(121 + PHY_BASE_ADDR,DENALI_PHY_121_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(121 + PHY_BASE_ADDR,DENALI_PHY_121_DATA,0);
  #else
         REGWR(121 + PHY_BASE_ADDR,DENALI_PHY_121_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_122_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(122 + PHY_BASE_ADDR,DENALI_PHY_122_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(122 + PHY_BASE_ADDR,DENALI_PHY_122_DATA,0);
  #else
         REGWR(122 + PHY_BASE_ADDR,DENALI_PHY_122_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_123_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(123 + PHY_BASE_ADDR,DENALI_PHY_123_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(123 + PHY_BASE_ADDR,DENALI_PHY_123_DATA,0);
  #else
         REGWR(123 + PHY_BASE_ADDR,DENALI_PHY_123_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_124_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(124 + PHY_BASE_ADDR,DENALI_PHY_124_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(124 + PHY_BASE_ADDR,DENALI_PHY_124_DATA,0);
  #else
         REGWR(124 + PHY_BASE_ADDR,DENALI_PHY_124_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_125_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(125 + PHY_BASE_ADDR,DENALI_PHY_125_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(125 + PHY_BASE_ADDR,DENALI_PHY_125_DATA,0);
  #else
         REGWR(125 + PHY_BASE_ADDR,DENALI_PHY_125_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_126_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(126 + PHY_BASE_ADDR,DENALI_PHY_126_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(126 + PHY_BASE_ADDR,DENALI_PHY_126_DATA,0);
  #else
         REGWR(126 + PHY_BASE_ADDR,DENALI_PHY_126_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_127_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(127 + PHY_BASE_ADDR,DENALI_PHY_127_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(127 + PHY_BASE_ADDR,DENALI_PHY_127_DATA,0);
  #else
         REGWR(127 + PHY_BASE_ADDR,DENALI_PHY_127_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_128_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(128 + PHY_BASE_ADDR,DENALI_PHY_128_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(128 + PHY_BASE_ADDR,DENALI_PHY_128_DATA,0);
  #else
         REGWR(128 + PHY_BASE_ADDR,DENALI_PHY_128_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_129_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(129 + PHY_BASE_ADDR,DENALI_PHY_129_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(129 + PHY_BASE_ADDR,DENALI_PHY_129_DATA,0);
  #else
         REGWR(129 + PHY_BASE_ADDR,DENALI_PHY_129_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_130_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(130 + PHY_BASE_ADDR,DENALI_PHY_130_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(130 + PHY_BASE_ADDR,DENALI_PHY_130_DATA,0);
  #else
         REGWR(130 + PHY_BASE_ADDR,DENALI_PHY_130_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_131_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(131 + PHY_BASE_ADDR,DENALI_PHY_131_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(131 + PHY_BASE_ADDR,DENALI_PHY_131_DATA,0);
  #else
         REGWR(131 + PHY_BASE_ADDR,DENALI_PHY_131_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_132_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(132 + PHY_BASE_ADDR,DENALI_PHY_132_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(132 + PHY_BASE_ADDR,DENALI_PHY_132_DATA,0);
  #else
         REGWR(132 + PHY_BASE_ADDR,DENALI_PHY_132_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_133_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(133 + PHY_BASE_ADDR,DENALI_PHY_133_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(133 + PHY_BASE_ADDR,DENALI_PHY_133_DATA,0);
  #else
         REGWR(133 + PHY_BASE_ADDR,DENALI_PHY_133_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_134_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(134 + PHY_BASE_ADDR,DENALI_PHY_134_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(134 + PHY_BASE_ADDR,DENALI_PHY_134_DATA,0);
  #else
         REGWR(134 + PHY_BASE_ADDR,DENALI_PHY_134_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_135_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(135 + PHY_BASE_ADDR,DENALI_PHY_135_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(135 + PHY_BASE_ADDR,DENALI_PHY_135_DATA,0);
  #else
         REGWR(135 + PHY_BASE_ADDR,DENALI_PHY_135_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_136_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(136 + PHY_BASE_ADDR,DENALI_PHY_136_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(136 + PHY_BASE_ADDR,DENALI_PHY_136_DATA,0);
  #else
         REGWR(136 + PHY_BASE_ADDR,DENALI_PHY_136_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_137_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(137 + PHY_BASE_ADDR,DENALI_PHY_137_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(137 + PHY_BASE_ADDR,DENALI_PHY_137_DATA,0);
  #else
         REGWR(137 + PHY_BASE_ADDR,DENALI_PHY_137_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_138_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(138 + PHY_BASE_ADDR,DENALI_PHY_138_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(138 + PHY_BASE_ADDR,DENALI_PHY_138_DATA,0);
  #else
         REGWR(138 + PHY_BASE_ADDR,DENALI_PHY_138_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_139_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(139 + PHY_BASE_ADDR,DENALI_PHY_139_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(139 + PHY_BASE_ADDR,DENALI_PHY_139_DATA,0);
  #else
         REGWR(139 + PHY_BASE_ADDR,DENALI_PHY_139_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_140_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(140 + PHY_BASE_ADDR,DENALI_PHY_140_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(140 + PHY_BASE_ADDR,DENALI_PHY_140_DATA,0);
  #else
         REGWR(140 + PHY_BASE_ADDR,DENALI_PHY_140_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_141_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(141 + PHY_BASE_ADDR,DENALI_PHY_141_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(141 + PHY_BASE_ADDR,DENALI_PHY_141_DATA,0);
  #else
         REGWR(141 + PHY_BASE_ADDR,DENALI_PHY_141_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_142_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(142 + PHY_BASE_ADDR,DENALI_PHY_142_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(142 + PHY_BASE_ADDR,DENALI_PHY_142_DATA,0);
  #else
         REGWR(142 + PHY_BASE_ADDR,DENALI_PHY_142_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_143_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(143 + PHY_BASE_ADDR,DENALI_PHY_143_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(143 + PHY_BASE_ADDR,DENALI_PHY_143_DATA,0);
  #else
         REGWR(143 + PHY_BASE_ADDR,DENALI_PHY_143_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_144_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(144 + PHY_BASE_ADDR,DENALI_PHY_144_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(144 + PHY_BASE_ADDR,DENALI_PHY_144_DATA,0);
  #else
         REGWR(144 + PHY_BASE_ADDR,DENALI_PHY_144_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_145_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(145 + PHY_BASE_ADDR,DENALI_PHY_145_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(145 + PHY_BASE_ADDR,DENALI_PHY_145_DATA,0);
  #else
         REGWR(145 + PHY_BASE_ADDR,DENALI_PHY_145_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_146_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(146 + PHY_BASE_ADDR,DENALI_PHY_146_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(146 + PHY_BASE_ADDR,DENALI_PHY_146_DATA,0);
  #else
         REGWR(146 + PHY_BASE_ADDR,DENALI_PHY_146_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_147_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(147 + PHY_BASE_ADDR,DENALI_PHY_147_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(147 + PHY_BASE_ADDR,DENALI_PHY_147_DATA,0);
  #else
         REGWR(147 + PHY_BASE_ADDR,DENALI_PHY_147_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_148_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(148 + PHY_BASE_ADDR,DENALI_PHY_148_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(148 + PHY_BASE_ADDR,DENALI_PHY_148_DATA,0);
  #else
         REGWR(148 + PHY_BASE_ADDR,DENALI_PHY_148_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_149_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(149 + PHY_BASE_ADDR,DENALI_PHY_149_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(149 + PHY_BASE_ADDR,DENALI_PHY_149_DATA,0);
  #else
         REGWR(149 + PHY_BASE_ADDR,DENALI_PHY_149_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_150_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(150 + PHY_BASE_ADDR,DENALI_PHY_150_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(150 + PHY_BASE_ADDR,DENALI_PHY_150_DATA,0);
  #else
         REGWR(150 + PHY_BASE_ADDR,DENALI_PHY_150_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_151_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(151 + PHY_BASE_ADDR,DENALI_PHY_151_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(151 + PHY_BASE_ADDR,DENALI_PHY_151_DATA,0);
  #else
         REGWR(151 + PHY_BASE_ADDR,DENALI_PHY_151_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_152_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(152 + PHY_BASE_ADDR,DENALI_PHY_152_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(152 + PHY_BASE_ADDR,DENALI_PHY_152_DATA,0);
  #else
         REGWR(152 + PHY_BASE_ADDR,DENALI_PHY_152_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_153_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(153 + PHY_BASE_ADDR,DENALI_PHY_153_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(153 + PHY_BASE_ADDR,DENALI_PHY_153_DATA,0);
  #else
         REGWR(153 + PHY_BASE_ADDR,DENALI_PHY_153_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_154_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(154 + PHY_BASE_ADDR,DENALI_PHY_154_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(154 + PHY_BASE_ADDR,DENALI_PHY_154_DATA,0);
  #else
         REGWR(154 + PHY_BASE_ADDR,DENALI_PHY_154_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_155_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(155 + PHY_BASE_ADDR,DENALI_PHY_155_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(155 + PHY_BASE_ADDR,DENALI_PHY_155_DATA,0);
  #else
         REGWR(155 + PHY_BASE_ADDR,DENALI_PHY_155_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_156_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(156 + PHY_BASE_ADDR,DENALI_PHY_156_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(156 + PHY_BASE_ADDR,DENALI_PHY_156_DATA,0);
  #else
         REGWR(156 + PHY_BASE_ADDR,DENALI_PHY_156_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_157_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(157 + PHY_BASE_ADDR,DENALI_PHY_157_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(157 + PHY_BASE_ADDR,DENALI_PHY_157_DATA,0);
  #else
         REGWR(157 + PHY_BASE_ADDR,DENALI_PHY_157_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_158_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(158 + PHY_BASE_ADDR,DENALI_PHY_158_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(158 + PHY_BASE_ADDR,DENALI_PHY_158_DATA,0);
  #else
         REGWR(158 + PHY_BASE_ADDR,DENALI_PHY_158_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_159_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(159 + PHY_BASE_ADDR,DENALI_PHY_159_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(159 + PHY_BASE_ADDR,DENALI_PHY_159_DATA,0);
  #else
         REGWR(159 + PHY_BASE_ADDR,DENALI_PHY_159_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_160_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(160 + PHY_BASE_ADDR,DENALI_PHY_160_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(160 + PHY_BASE_ADDR,DENALI_PHY_160_DATA,0);
  #else
         REGWR(160 + PHY_BASE_ADDR,DENALI_PHY_160_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_161_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(161 + PHY_BASE_ADDR,DENALI_PHY_161_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(161 + PHY_BASE_ADDR,DENALI_PHY_161_DATA,0);
  #else
         REGWR(161 + PHY_BASE_ADDR,DENALI_PHY_161_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_162_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(162 + PHY_BASE_ADDR,DENALI_PHY_162_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(162 + PHY_BASE_ADDR,DENALI_PHY_162_DATA,0);
  #else
         REGWR(162 + PHY_BASE_ADDR,DENALI_PHY_162_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_163_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(163 + PHY_BASE_ADDR,DENALI_PHY_163_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(163 + PHY_BASE_ADDR,DENALI_PHY_163_DATA,0);
  #else
         REGWR(163 + PHY_BASE_ADDR,DENALI_PHY_163_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_164_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(164 + PHY_BASE_ADDR,DENALI_PHY_164_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(164 + PHY_BASE_ADDR,DENALI_PHY_164_DATA,0);
  #else
         REGWR(164 + PHY_BASE_ADDR,DENALI_PHY_164_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_165_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(165 + PHY_BASE_ADDR,DENALI_PHY_165_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(165 + PHY_BASE_ADDR,DENALI_PHY_165_DATA,0);
  #else
         REGWR(165 + PHY_BASE_ADDR,DENALI_PHY_165_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_166_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(166 + PHY_BASE_ADDR,DENALI_PHY_166_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(166 + PHY_BASE_ADDR,DENALI_PHY_166_DATA,0);
  #else
         REGWR(166 + PHY_BASE_ADDR,DENALI_PHY_166_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_167_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(167 + PHY_BASE_ADDR,DENALI_PHY_167_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(167 + PHY_BASE_ADDR,DENALI_PHY_167_DATA,0);
  #else
         REGWR(167 + PHY_BASE_ADDR,DENALI_PHY_167_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_168_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(168 + PHY_BASE_ADDR,DENALI_PHY_168_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(168 + PHY_BASE_ADDR,DENALI_PHY_168_DATA,0);
  #else
         REGWR(168 + PHY_BASE_ADDR,DENALI_PHY_168_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_169_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(169 + PHY_BASE_ADDR,DENALI_PHY_169_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(169 + PHY_BASE_ADDR,DENALI_PHY_169_DATA,0);
  #else
         REGWR(169 + PHY_BASE_ADDR,DENALI_PHY_169_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_170_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(170 + PHY_BASE_ADDR,DENALI_PHY_170_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(170 + PHY_BASE_ADDR,DENALI_PHY_170_DATA,0);
  #else
         REGWR(170 + PHY_BASE_ADDR,DENALI_PHY_170_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_171_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(171 + PHY_BASE_ADDR,DENALI_PHY_171_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(171 + PHY_BASE_ADDR,DENALI_PHY_171_DATA,0);
  #else
         REGWR(171 + PHY_BASE_ADDR,DENALI_PHY_171_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_172_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(172 + PHY_BASE_ADDR,DENALI_PHY_172_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(172 + PHY_BASE_ADDR,DENALI_PHY_172_DATA,0);
  #else
         REGWR(172 + PHY_BASE_ADDR,DENALI_PHY_172_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_173_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(173 + PHY_BASE_ADDR,DENALI_PHY_173_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(173 + PHY_BASE_ADDR,DENALI_PHY_173_DATA,0);
  #else
         REGWR(173 + PHY_BASE_ADDR,DENALI_PHY_173_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_174_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(174 + PHY_BASE_ADDR,DENALI_PHY_174_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(174 + PHY_BASE_ADDR,DENALI_PHY_174_DATA,0);
  #else
         REGWR(174 + PHY_BASE_ADDR,DENALI_PHY_174_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_175_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(175 + PHY_BASE_ADDR,DENALI_PHY_175_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(175 + PHY_BASE_ADDR,DENALI_PHY_175_DATA,0);
  #else
         REGWR(175 + PHY_BASE_ADDR,DENALI_PHY_175_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_176_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(176 + PHY_BASE_ADDR,DENALI_PHY_176_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(176 + PHY_BASE_ADDR,DENALI_PHY_176_DATA,0);
  #else
         REGWR(176 + PHY_BASE_ADDR,DENALI_PHY_176_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_177_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(177 + PHY_BASE_ADDR,DENALI_PHY_177_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(177 + PHY_BASE_ADDR,DENALI_PHY_177_DATA,0);
  #else
         REGWR(177 + PHY_BASE_ADDR,DENALI_PHY_177_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_178_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(178 + PHY_BASE_ADDR,DENALI_PHY_178_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(178 + PHY_BASE_ADDR,DENALI_PHY_178_DATA,0);
  #else
         REGWR(178 + PHY_BASE_ADDR,DENALI_PHY_178_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_179_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(179 + PHY_BASE_ADDR,DENALI_PHY_179_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(179 + PHY_BASE_ADDR,DENALI_PHY_179_DATA,0);
  #else
         REGWR(179 + PHY_BASE_ADDR,DENALI_PHY_179_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_180_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(180 + PHY_BASE_ADDR,DENALI_PHY_180_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(180 + PHY_BASE_ADDR,DENALI_PHY_180_DATA,0);
  #else
         REGWR(180 + PHY_BASE_ADDR,DENALI_PHY_180_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_181_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(181 + PHY_BASE_ADDR,DENALI_PHY_181_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(181 + PHY_BASE_ADDR,DENALI_PHY_181_DATA,0);
  #else
         REGWR(181 + PHY_BASE_ADDR,DENALI_PHY_181_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_182_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(182 + PHY_BASE_ADDR,DENALI_PHY_182_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(182 + PHY_BASE_ADDR,DENALI_PHY_182_DATA,0);
  #else
         REGWR(182 + PHY_BASE_ADDR,DENALI_PHY_182_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_183_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(183 + PHY_BASE_ADDR,DENALI_PHY_183_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(183 + PHY_BASE_ADDR,DENALI_PHY_183_DATA,0);
  #else
         REGWR(183 + PHY_BASE_ADDR,DENALI_PHY_183_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_184_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(184 + PHY_BASE_ADDR,DENALI_PHY_184_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(184 + PHY_BASE_ADDR,DENALI_PHY_184_DATA,0);
  #else
         REGWR(184 + PHY_BASE_ADDR,DENALI_PHY_184_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_185_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(185 + PHY_BASE_ADDR,DENALI_PHY_185_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(185 + PHY_BASE_ADDR,DENALI_PHY_185_DATA,0);
  #else
         REGWR(185 + PHY_BASE_ADDR,DENALI_PHY_185_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_186_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(186 + PHY_BASE_ADDR,DENALI_PHY_186_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(186 + PHY_BASE_ADDR,DENALI_PHY_186_DATA,0);
  #else
         REGWR(186 + PHY_BASE_ADDR,DENALI_PHY_186_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_187_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(187 + PHY_BASE_ADDR,DENALI_PHY_187_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(187 + PHY_BASE_ADDR,DENALI_PHY_187_DATA,0);
  #else
         REGWR(187 + PHY_BASE_ADDR,DENALI_PHY_187_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_188_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(188 + PHY_BASE_ADDR,DENALI_PHY_188_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(188 + PHY_BASE_ADDR,DENALI_PHY_188_DATA,0);
  #else
         REGWR(188 + PHY_BASE_ADDR,DENALI_PHY_188_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_189_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(189 + PHY_BASE_ADDR,DENALI_PHY_189_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(189 + PHY_BASE_ADDR,DENALI_PHY_189_DATA,0);
  #else
         REGWR(189 + PHY_BASE_ADDR,DENALI_PHY_189_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_190_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(190 + PHY_BASE_ADDR,DENALI_PHY_190_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(190 + PHY_BASE_ADDR,DENALI_PHY_190_DATA,0);
  #else
         REGWR(190 + PHY_BASE_ADDR,DENALI_PHY_190_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_191_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(191 + PHY_BASE_ADDR,DENALI_PHY_191_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(191 + PHY_BASE_ADDR,DENALI_PHY_191_DATA,0);
  #else
         REGWR(191 + PHY_BASE_ADDR,DENALI_PHY_191_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_192_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(192 + PHY_BASE_ADDR,DENALI_PHY_192_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(192 + PHY_BASE_ADDR,DENALI_PHY_192_DATA,0);
  #else
         REGWR(192 + PHY_BASE_ADDR,DENALI_PHY_192_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_193_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(193 + PHY_BASE_ADDR,DENALI_PHY_193_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(193 + PHY_BASE_ADDR,DENALI_PHY_193_DATA,0);
  #else
         REGWR(193 + PHY_BASE_ADDR,DENALI_PHY_193_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_194_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(194 + PHY_BASE_ADDR,DENALI_PHY_194_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(194 + PHY_BASE_ADDR,DENALI_PHY_194_DATA,0);
  #else
         REGWR(194 + PHY_BASE_ADDR,DENALI_PHY_194_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_195_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(195 + PHY_BASE_ADDR,DENALI_PHY_195_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(195 + PHY_BASE_ADDR,DENALI_PHY_195_DATA,0);
  #else
         REGWR(195 + PHY_BASE_ADDR,DENALI_PHY_195_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_196_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(196 + PHY_BASE_ADDR,DENALI_PHY_196_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(196 + PHY_BASE_ADDR,DENALI_PHY_196_DATA,0);
  #else
         REGWR(196 + PHY_BASE_ADDR,DENALI_PHY_196_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_197_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(197 + PHY_BASE_ADDR,DENALI_PHY_197_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(197 + PHY_BASE_ADDR,DENALI_PHY_197_DATA,0);
  #else
         REGWR(197 + PHY_BASE_ADDR,DENALI_PHY_197_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_198_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(198 + PHY_BASE_ADDR,DENALI_PHY_198_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(198 + PHY_BASE_ADDR,DENALI_PHY_198_DATA,0);
  #else
         REGWR(198 + PHY_BASE_ADDR,DENALI_PHY_198_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_199_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(199 + PHY_BASE_ADDR,DENALI_PHY_199_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(199 + PHY_BASE_ADDR,DENALI_PHY_199_DATA,0);
  #else
         REGWR(199 + PHY_BASE_ADDR,DENALI_PHY_199_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_200_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(200 + PHY_BASE_ADDR,DENALI_PHY_200_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(200 + PHY_BASE_ADDR,DENALI_PHY_200_DATA,0);
  #else
         REGWR(200 + PHY_BASE_ADDR,DENALI_PHY_200_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_201_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(201 + PHY_BASE_ADDR,DENALI_PHY_201_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(201 + PHY_BASE_ADDR,DENALI_PHY_201_DATA,0);
  #else
         REGWR(201 + PHY_BASE_ADDR,DENALI_PHY_201_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_202_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(202 + PHY_BASE_ADDR,DENALI_PHY_202_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(202 + PHY_BASE_ADDR,DENALI_PHY_202_DATA,0);
  #else
         REGWR(202 + PHY_BASE_ADDR,DENALI_PHY_202_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_203_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(203 + PHY_BASE_ADDR,DENALI_PHY_203_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(203 + PHY_BASE_ADDR,DENALI_PHY_203_DATA,0);
  #else
         REGWR(203 + PHY_BASE_ADDR,DENALI_PHY_203_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_204_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(204 + PHY_BASE_ADDR,DENALI_PHY_204_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(204 + PHY_BASE_ADDR,DENALI_PHY_204_DATA,0);
  #else
         REGWR(204 + PHY_BASE_ADDR,DENALI_PHY_204_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_205_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(205 + PHY_BASE_ADDR,DENALI_PHY_205_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(205 + PHY_BASE_ADDR,DENALI_PHY_205_DATA,0);
  #else
         REGWR(205 + PHY_BASE_ADDR,DENALI_PHY_205_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_206_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(206 + PHY_BASE_ADDR,DENALI_PHY_206_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(206 + PHY_BASE_ADDR,DENALI_PHY_206_DATA,0);
  #else
         REGWR(206 + PHY_BASE_ADDR,DENALI_PHY_206_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_207_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(207 + PHY_BASE_ADDR,DENALI_PHY_207_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(207 + PHY_BASE_ADDR,DENALI_PHY_207_DATA,0);
  #else
         REGWR(207 + PHY_BASE_ADDR,DENALI_PHY_207_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_208_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(208 + PHY_BASE_ADDR,DENALI_PHY_208_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(208 + PHY_BASE_ADDR,DENALI_PHY_208_DATA,0);
  #else
         REGWR(208 + PHY_BASE_ADDR,DENALI_PHY_208_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_209_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(209 + PHY_BASE_ADDR,DENALI_PHY_209_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(209 + PHY_BASE_ADDR,DENALI_PHY_209_DATA,0);
  #else
         REGWR(209 + PHY_BASE_ADDR,DENALI_PHY_209_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_210_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(210 + PHY_BASE_ADDR,DENALI_PHY_210_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(210 + PHY_BASE_ADDR,DENALI_PHY_210_DATA,0);
  #else
         REGWR(210 + PHY_BASE_ADDR,DENALI_PHY_210_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_211_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(211 + PHY_BASE_ADDR,DENALI_PHY_211_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(211 + PHY_BASE_ADDR,DENALI_PHY_211_DATA,0);
  #else
         REGWR(211 + PHY_BASE_ADDR,DENALI_PHY_211_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_212_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(212 + PHY_BASE_ADDR,DENALI_PHY_212_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(212 + PHY_BASE_ADDR,DENALI_PHY_212_DATA,0);
  #else
         REGWR(212 + PHY_BASE_ADDR,DENALI_PHY_212_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_213_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(213 + PHY_BASE_ADDR,DENALI_PHY_213_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(213 + PHY_BASE_ADDR,DENALI_PHY_213_DATA,0);
  #else
         REGWR(213 + PHY_BASE_ADDR,DENALI_PHY_213_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_214_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(214 + PHY_BASE_ADDR,DENALI_PHY_214_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(214 + PHY_BASE_ADDR,DENALI_PHY_214_DATA,0);
  #else
         REGWR(214 + PHY_BASE_ADDR,DENALI_PHY_214_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_215_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(215 + PHY_BASE_ADDR,DENALI_PHY_215_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(215 + PHY_BASE_ADDR,DENALI_PHY_215_DATA,0);
  #else
         REGWR(215 + PHY_BASE_ADDR,DENALI_PHY_215_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_216_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(216 + PHY_BASE_ADDR,DENALI_PHY_216_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(216 + PHY_BASE_ADDR,DENALI_PHY_216_DATA,0);
  #else
         REGWR(216 + PHY_BASE_ADDR,DENALI_PHY_216_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_217_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(217 + PHY_BASE_ADDR,DENALI_PHY_217_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(217 + PHY_BASE_ADDR,DENALI_PHY_217_DATA,0);
  #else
         REGWR(217 + PHY_BASE_ADDR,DENALI_PHY_217_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_218_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(218 + PHY_BASE_ADDR,DENALI_PHY_218_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(218 + PHY_BASE_ADDR,DENALI_PHY_218_DATA,0);
  #else
         REGWR(218 + PHY_BASE_ADDR,DENALI_PHY_218_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_219_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(219 + PHY_BASE_ADDR,DENALI_PHY_219_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(219 + PHY_BASE_ADDR,DENALI_PHY_219_DATA,0);
  #else
         REGWR(219 + PHY_BASE_ADDR,DENALI_PHY_219_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_220_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(220 + PHY_BASE_ADDR,DENALI_PHY_220_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(220 + PHY_BASE_ADDR,DENALI_PHY_220_DATA,0);
  #else
         REGWR(220 + PHY_BASE_ADDR,DENALI_PHY_220_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_221_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(221 + PHY_BASE_ADDR,DENALI_PHY_221_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(221 + PHY_BASE_ADDR,DENALI_PHY_221_DATA,0);
  #else
         REGWR(221 + PHY_BASE_ADDR,DENALI_PHY_221_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_222_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(222 + PHY_BASE_ADDR,DENALI_PHY_222_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(222 + PHY_BASE_ADDR,DENALI_PHY_222_DATA,0);
  #else
         REGWR(222 + PHY_BASE_ADDR,DENALI_PHY_222_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_223_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(223 + PHY_BASE_ADDR,DENALI_PHY_223_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(223 + PHY_BASE_ADDR,DENALI_PHY_223_DATA,0);
  #else
         REGWR(223 + PHY_BASE_ADDR,DENALI_PHY_223_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_224_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(224 + PHY_BASE_ADDR,DENALI_PHY_224_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(224 + PHY_BASE_ADDR,DENALI_PHY_224_DATA,0);
  #else
         REGWR(224 + PHY_BASE_ADDR,DENALI_PHY_224_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_225_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(225 + PHY_BASE_ADDR,DENALI_PHY_225_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(225 + PHY_BASE_ADDR,DENALI_PHY_225_DATA,0);
  #else
         REGWR(225 + PHY_BASE_ADDR,DENALI_PHY_225_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_226_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(226 + PHY_BASE_ADDR,DENALI_PHY_226_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(226 + PHY_BASE_ADDR,DENALI_PHY_226_DATA,0);
  #else
         REGWR(226 + PHY_BASE_ADDR,DENALI_PHY_226_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_227_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(227 + PHY_BASE_ADDR,DENALI_PHY_227_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(227 + PHY_BASE_ADDR,DENALI_PHY_227_DATA,0);
  #else
         REGWR(227 + PHY_BASE_ADDR,DENALI_PHY_227_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_228_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(228 + PHY_BASE_ADDR,DENALI_PHY_228_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(228 + PHY_BASE_ADDR,DENALI_PHY_228_DATA,0);
  #else
         REGWR(228 + PHY_BASE_ADDR,DENALI_PHY_228_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_229_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(229 + PHY_BASE_ADDR,DENALI_PHY_229_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(229 + PHY_BASE_ADDR,DENALI_PHY_229_DATA,0);
  #else
         REGWR(229 + PHY_BASE_ADDR,DENALI_PHY_229_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_230_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(230 + PHY_BASE_ADDR,DENALI_PHY_230_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(230 + PHY_BASE_ADDR,DENALI_PHY_230_DATA,0);
  #else
         REGWR(230 + PHY_BASE_ADDR,DENALI_PHY_230_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_231_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(231 + PHY_BASE_ADDR,DENALI_PHY_231_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(231 + PHY_BASE_ADDR,DENALI_PHY_231_DATA,0);
  #else
         REGWR(231 + PHY_BASE_ADDR,DENALI_PHY_231_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_232_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(232 + PHY_BASE_ADDR,DENALI_PHY_232_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(232 + PHY_BASE_ADDR,DENALI_PHY_232_DATA,0);
  #else
         REGWR(232 + PHY_BASE_ADDR,DENALI_PHY_232_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_233_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(233 + PHY_BASE_ADDR,DENALI_PHY_233_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(233 + PHY_BASE_ADDR,DENALI_PHY_233_DATA,0);
  #else
         REGWR(233 + PHY_BASE_ADDR,DENALI_PHY_233_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_234_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(234 + PHY_BASE_ADDR,DENALI_PHY_234_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(234 + PHY_BASE_ADDR,DENALI_PHY_234_DATA,0);
  #else
         REGWR(234 + PHY_BASE_ADDR,DENALI_PHY_234_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_235_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(235 + PHY_BASE_ADDR,DENALI_PHY_235_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(235 + PHY_BASE_ADDR,DENALI_PHY_235_DATA,0);
  #else
         REGWR(235 + PHY_BASE_ADDR,DENALI_PHY_235_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_236_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(236 + PHY_BASE_ADDR,DENALI_PHY_236_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(236 + PHY_BASE_ADDR,DENALI_PHY_236_DATA,0);
  #else
         REGWR(236 + PHY_BASE_ADDR,DENALI_PHY_236_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_237_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(237 + PHY_BASE_ADDR,DENALI_PHY_237_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(237 + PHY_BASE_ADDR,DENALI_PHY_237_DATA,0);
  #else
         REGWR(237 + PHY_BASE_ADDR,DENALI_PHY_237_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_238_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(238 + PHY_BASE_ADDR,DENALI_PHY_238_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(238 + PHY_BASE_ADDR,DENALI_PHY_238_DATA,0);
  #else
         REGWR(238 + PHY_BASE_ADDR,DENALI_PHY_238_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_239_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(239 + PHY_BASE_ADDR,DENALI_PHY_239_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(239 + PHY_BASE_ADDR,DENALI_PHY_239_DATA,0);
  #else
         REGWR(239 + PHY_BASE_ADDR,DENALI_PHY_239_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_240_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(240 + PHY_BASE_ADDR,DENALI_PHY_240_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(240 + PHY_BASE_ADDR,DENALI_PHY_240_DATA,0);
  #else
         REGWR(240 + PHY_BASE_ADDR,DENALI_PHY_240_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_241_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(241 + PHY_BASE_ADDR,DENALI_PHY_241_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(241 + PHY_BASE_ADDR,DENALI_PHY_241_DATA,0);
  #else
         REGWR(241 + PHY_BASE_ADDR,DENALI_PHY_241_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_242_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(242 + PHY_BASE_ADDR,DENALI_PHY_242_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(242 + PHY_BASE_ADDR,DENALI_PHY_242_DATA,0);
  #else
         REGWR(242 + PHY_BASE_ADDR,DENALI_PHY_242_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_243_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(243 + PHY_BASE_ADDR,DENALI_PHY_243_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(243 + PHY_BASE_ADDR,DENALI_PHY_243_DATA,0);
  #else
         REGWR(243 + PHY_BASE_ADDR,DENALI_PHY_243_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_244_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(244 + PHY_BASE_ADDR,DENALI_PHY_244_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(244 + PHY_BASE_ADDR,DENALI_PHY_244_DATA,0);
  #else
         REGWR(244 + PHY_BASE_ADDR,DENALI_PHY_244_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_245_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(245 + PHY_BASE_ADDR,DENALI_PHY_245_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(245 + PHY_BASE_ADDR,DENALI_PHY_245_DATA,0);
  #else
         REGWR(245 + PHY_BASE_ADDR,DENALI_PHY_245_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_246_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(246 + PHY_BASE_ADDR,DENALI_PHY_246_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(246 + PHY_BASE_ADDR,DENALI_PHY_246_DATA,0);
  #else
         REGWR(246 + PHY_BASE_ADDR,DENALI_PHY_246_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_247_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(247 + PHY_BASE_ADDR,DENALI_PHY_247_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(247 + PHY_BASE_ADDR,DENALI_PHY_247_DATA,0);
  #else
         REGWR(247 + PHY_BASE_ADDR,DENALI_PHY_247_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_248_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(248 + PHY_BASE_ADDR,DENALI_PHY_248_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(248 + PHY_BASE_ADDR,DENALI_PHY_248_DATA,0);
  #else
         REGWR(248 + PHY_BASE_ADDR,DENALI_PHY_248_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_249_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(249 + PHY_BASE_ADDR,DENALI_PHY_249_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(249 + PHY_BASE_ADDR,DENALI_PHY_249_DATA,0);
  #else
         REGWR(249 + PHY_BASE_ADDR,DENALI_PHY_249_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_250_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(250 + PHY_BASE_ADDR,DENALI_PHY_250_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(250 + PHY_BASE_ADDR,DENALI_PHY_250_DATA,0);
  #else
         REGWR(250 + PHY_BASE_ADDR,DENALI_PHY_250_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_251_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(251 + PHY_BASE_ADDR,DENALI_PHY_251_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(251 + PHY_BASE_ADDR,DENALI_PHY_251_DATA,0);
  #else
         REGWR(251 + PHY_BASE_ADDR,DENALI_PHY_251_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_252_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(252 + PHY_BASE_ADDR,DENALI_PHY_252_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(252 + PHY_BASE_ADDR,DENALI_PHY_252_DATA,0);
  #else
         REGWR(252 + PHY_BASE_ADDR,DENALI_PHY_252_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_253_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(253 + PHY_BASE_ADDR,DENALI_PHY_253_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(253 + PHY_BASE_ADDR,DENALI_PHY_253_DATA,0);
  #else
         REGWR(253 + PHY_BASE_ADDR,DENALI_PHY_253_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_254_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(254 + PHY_BASE_ADDR,DENALI_PHY_254_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(254 + PHY_BASE_ADDR,DENALI_PHY_254_DATA,0);
  #else
         REGWR(254 + PHY_BASE_ADDR,DENALI_PHY_254_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_255_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(255 + PHY_BASE_ADDR,DENALI_PHY_255_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(255 + PHY_BASE_ADDR,DENALI_PHY_255_DATA,0);
  #else
         REGWR(255 + PHY_BASE_ADDR,DENALI_PHY_255_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_256_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(256 + PHY_BASE_ADDR,DENALI_PHY_256_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(256 + PHY_BASE_ADDR,DENALI_PHY_256_DATA,0);
  #else
         REGWR(256 + PHY_BASE_ADDR,DENALI_PHY_256_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_257_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(257 + PHY_BASE_ADDR,DENALI_PHY_257_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(257 + PHY_BASE_ADDR,DENALI_PHY_257_DATA,0);
  #else
         REGWR(257 + PHY_BASE_ADDR,DENALI_PHY_257_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_258_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(258 + PHY_BASE_ADDR,DENALI_PHY_258_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(258 + PHY_BASE_ADDR,DENALI_PHY_258_DATA,0);
  #else
         REGWR(258 + PHY_BASE_ADDR,DENALI_PHY_258_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_259_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(259 + PHY_BASE_ADDR,DENALI_PHY_259_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(259 + PHY_BASE_ADDR,DENALI_PHY_259_DATA,0);
  #else
         REGWR(259 + PHY_BASE_ADDR,DENALI_PHY_259_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_260_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(260 + PHY_BASE_ADDR,DENALI_PHY_260_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(260 + PHY_BASE_ADDR,DENALI_PHY_260_DATA,0);
  #else
         REGWR(260 + PHY_BASE_ADDR,DENALI_PHY_260_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_261_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(261 + PHY_BASE_ADDR,DENALI_PHY_261_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(261 + PHY_BASE_ADDR,DENALI_PHY_261_DATA,0);
  #else
         REGWR(261 + PHY_BASE_ADDR,DENALI_PHY_261_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_262_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(262 + PHY_BASE_ADDR,DENALI_PHY_262_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(262 + PHY_BASE_ADDR,DENALI_PHY_262_DATA,0);
  #else
         REGWR(262 + PHY_BASE_ADDR,DENALI_PHY_262_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_263_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(263 + PHY_BASE_ADDR,DENALI_PHY_263_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(263 + PHY_BASE_ADDR,DENALI_PHY_263_DATA,0);
  #else
         REGWR(263 + PHY_BASE_ADDR,DENALI_PHY_263_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_264_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(264 + PHY_BASE_ADDR,DENALI_PHY_264_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(264 + PHY_BASE_ADDR,DENALI_PHY_264_DATA,0);
  #else
         REGWR(264 + PHY_BASE_ADDR,DENALI_PHY_264_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_265_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(265 + PHY_BASE_ADDR,DENALI_PHY_265_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(265 + PHY_BASE_ADDR,DENALI_PHY_265_DATA,0);
  #else
         REGWR(265 + PHY_BASE_ADDR,DENALI_PHY_265_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_266_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(266 + PHY_BASE_ADDR,DENALI_PHY_266_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(266 + PHY_BASE_ADDR,DENALI_PHY_266_DATA,0);
  #else
         REGWR(266 + PHY_BASE_ADDR,DENALI_PHY_266_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_267_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(267 + PHY_BASE_ADDR,DENALI_PHY_267_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(267 + PHY_BASE_ADDR,DENALI_PHY_267_DATA,0);
  #else
         REGWR(267 + PHY_BASE_ADDR,DENALI_PHY_267_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_268_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(268 + PHY_BASE_ADDR,DENALI_PHY_268_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(268 + PHY_BASE_ADDR,DENALI_PHY_268_DATA,0);
  #else
         REGWR(268 + PHY_BASE_ADDR,DENALI_PHY_268_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_269_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(269 + PHY_BASE_ADDR,DENALI_PHY_269_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(269 + PHY_BASE_ADDR,DENALI_PHY_269_DATA,0);
  #else
         REGWR(269 + PHY_BASE_ADDR,DENALI_PHY_269_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_270_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(270 + PHY_BASE_ADDR,DENALI_PHY_270_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(270 + PHY_BASE_ADDR,DENALI_PHY_270_DATA,0);
  #else
         REGWR(270 + PHY_BASE_ADDR,DENALI_PHY_270_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_271_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(271 + PHY_BASE_ADDR,DENALI_PHY_271_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(271 + PHY_BASE_ADDR,DENALI_PHY_271_DATA,0);
  #else
         REGWR(271 + PHY_BASE_ADDR,DENALI_PHY_271_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_272_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(272 + PHY_BASE_ADDR,DENALI_PHY_272_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(272 + PHY_BASE_ADDR,DENALI_PHY_272_DATA,0);
  #else
         REGWR(272 + PHY_BASE_ADDR,DENALI_PHY_272_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_273_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(273 + PHY_BASE_ADDR,DENALI_PHY_273_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(273 + PHY_BASE_ADDR,DENALI_PHY_273_DATA,0);
  #else
         REGWR(273 + PHY_BASE_ADDR,DENALI_PHY_273_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_274_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(274 + PHY_BASE_ADDR,DENALI_PHY_274_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(274 + PHY_BASE_ADDR,DENALI_PHY_274_DATA,0);
  #else
         REGWR(274 + PHY_BASE_ADDR,DENALI_PHY_274_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_275_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(275 + PHY_BASE_ADDR,DENALI_PHY_275_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(275 + PHY_BASE_ADDR,DENALI_PHY_275_DATA,0);
  #else
         REGWR(275 + PHY_BASE_ADDR,DENALI_PHY_275_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_276_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(276 + PHY_BASE_ADDR,DENALI_PHY_276_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(276 + PHY_BASE_ADDR,DENALI_PHY_276_DATA,0);
  #else
         REGWR(276 + PHY_BASE_ADDR,DENALI_PHY_276_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_277_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(277 + PHY_BASE_ADDR,DENALI_PHY_277_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(277 + PHY_BASE_ADDR,DENALI_PHY_277_DATA,0);
  #else
         REGWR(277 + PHY_BASE_ADDR,DENALI_PHY_277_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_278_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(278 + PHY_BASE_ADDR,DENALI_PHY_278_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(278 + PHY_BASE_ADDR,DENALI_PHY_278_DATA,0);
  #else
         REGWR(278 + PHY_BASE_ADDR,DENALI_PHY_278_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_279_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(279 + PHY_BASE_ADDR,DENALI_PHY_279_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(279 + PHY_BASE_ADDR,DENALI_PHY_279_DATA,0);
  #else
         REGWR(279 + PHY_BASE_ADDR,DENALI_PHY_279_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_280_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(280 + PHY_BASE_ADDR,DENALI_PHY_280_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(280 + PHY_BASE_ADDR,DENALI_PHY_280_DATA,0);
  #else
         REGWR(280 + PHY_BASE_ADDR,DENALI_PHY_280_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_281_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(281 + PHY_BASE_ADDR,DENALI_PHY_281_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(281 + PHY_BASE_ADDR,DENALI_PHY_281_DATA,0);
  #else
         REGWR(281 + PHY_BASE_ADDR,DENALI_PHY_281_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_282_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(282 + PHY_BASE_ADDR,DENALI_PHY_282_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(282 + PHY_BASE_ADDR,DENALI_PHY_282_DATA,0);
  #else
         REGWR(282 + PHY_BASE_ADDR,DENALI_PHY_282_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_283_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(283 + PHY_BASE_ADDR,DENALI_PHY_283_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(283 + PHY_BASE_ADDR,DENALI_PHY_283_DATA,0);
  #else
         REGWR(283 + PHY_BASE_ADDR,DENALI_PHY_283_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_284_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(284 + PHY_BASE_ADDR,DENALI_PHY_284_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(284 + PHY_BASE_ADDR,DENALI_PHY_284_DATA,0);
  #else
         REGWR(284 + PHY_BASE_ADDR,DENALI_PHY_284_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_285_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(285 + PHY_BASE_ADDR,DENALI_PHY_285_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(285 + PHY_BASE_ADDR,DENALI_PHY_285_DATA,0);
  #else
         REGWR(285 + PHY_BASE_ADDR,DENALI_PHY_285_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_286_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(286 + PHY_BASE_ADDR,DENALI_PHY_286_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(286 + PHY_BASE_ADDR,DENALI_PHY_286_DATA,0);
  #else
         REGWR(286 + PHY_BASE_ADDR,DENALI_PHY_286_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_287_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(287 + PHY_BASE_ADDR,DENALI_PHY_287_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(287 + PHY_BASE_ADDR,DENALI_PHY_287_DATA,0);
  #else
         REGWR(287 + PHY_BASE_ADDR,DENALI_PHY_287_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_288_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(288 + PHY_BASE_ADDR,DENALI_PHY_288_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(288 + PHY_BASE_ADDR,DENALI_PHY_288_DATA,0);
  #else
         REGWR(288 + PHY_BASE_ADDR,DENALI_PHY_288_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_289_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(289 + PHY_BASE_ADDR,DENALI_PHY_289_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(289 + PHY_BASE_ADDR,DENALI_PHY_289_DATA,0);
  #else
         REGWR(289 + PHY_BASE_ADDR,DENALI_PHY_289_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_290_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(290 + PHY_BASE_ADDR,DENALI_PHY_290_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(290 + PHY_BASE_ADDR,DENALI_PHY_290_DATA,0);
  #else
         REGWR(290 + PHY_BASE_ADDR,DENALI_PHY_290_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_291_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(291 + PHY_BASE_ADDR,DENALI_PHY_291_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(291 + PHY_BASE_ADDR,DENALI_PHY_291_DATA,0);
  #else
         REGWR(291 + PHY_BASE_ADDR,DENALI_PHY_291_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_292_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(292 + PHY_BASE_ADDR,DENALI_PHY_292_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(292 + PHY_BASE_ADDR,DENALI_PHY_292_DATA,0);
  #else
         REGWR(292 + PHY_BASE_ADDR,DENALI_PHY_292_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_293_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(293 + PHY_BASE_ADDR,DENALI_PHY_293_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(293 + PHY_BASE_ADDR,DENALI_PHY_293_DATA,0);
  #else
         REGWR(293 + PHY_BASE_ADDR,DENALI_PHY_293_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_294_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(294 + PHY_BASE_ADDR,DENALI_PHY_294_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(294 + PHY_BASE_ADDR,DENALI_PHY_294_DATA,0);
  #else
         REGWR(294 + PHY_BASE_ADDR,DENALI_PHY_294_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_295_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(295 + PHY_BASE_ADDR,DENALI_PHY_295_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(295 + PHY_BASE_ADDR,DENALI_PHY_295_DATA,0);
  #else
         REGWR(295 + PHY_BASE_ADDR,DENALI_PHY_295_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_296_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(296 + PHY_BASE_ADDR,DENALI_PHY_296_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(296 + PHY_BASE_ADDR,DENALI_PHY_296_DATA,0);
  #else
         REGWR(296 + PHY_BASE_ADDR,DENALI_PHY_296_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_297_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(297 + PHY_BASE_ADDR,DENALI_PHY_297_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(297 + PHY_BASE_ADDR,DENALI_PHY_297_DATA,0);
  #else
         REGWR(297 + PHY_BASE_ADDR,DENALI_PHY_297_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_298_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(298 + PHY_BASE_ADDR,DENALI_PHY_298_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(298 + PHY_BASE_ADDR,DENALI_PHY_298_DATA,0);
  #else
         REGWR(298 + PHY_BASE_ADDR,DENALI_PHY_298_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_299_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(299 + PHY_BASE_ADDR,DENALI_PHY_299_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(299 + PHY_BASE_ADDR,DENALI_PHY_299_DATA,0);
  #else
         REGWR(299 + PHY_BASE_ADDR,DENALI_PHY_299_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_300_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(300 + PHY_BASE_ADDR,DENALI_PHY_300_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(300 + PHY_BASE_ADDR,DENALI_PHY_300_DATA,0);
  #else
         REGWR(300 + PHY_BASE_ADDR,DENALI_PHY_300_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_301_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(301 + PHY_BASE_ADDR,DENALI_PHY_301_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(301 + PHY_BASE_ADDR,DENALI_PHY_301_DATA,0);
  #else
         REGWR(301 + PHY_BASE_ADDR,DENALI_PHY_301_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_302_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(302 + PHY_BASE_ADDR,DENALI_PHY_302_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(302 + PHY_BASE_ADDR,DENALI_PHY_302_DATA,0);
  #else
         REGWR(302 + PHY_BASE_ADDR,DENALI_PHY_302_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_303_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(303 + PHY_BASE_ADDR,DENALI_PHY_303_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(303 + PHY_BASE_ADDR,DENALI_PHY_303_DATA,0);
  #else
         REGWR(303 + PHY_BASE_ADDR,DENALI_PHY_303_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_304_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(304 + PHY_BASE_ADDR,DENALI_PHY_304_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(304 + PHY_BASE_ADDR,DENALI_PHY_304_DATA,0);
  #else
         REGWR(304 + PHY_BASE_ADDR,DENALI_PHY_304_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_305_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(305 + PHY_BASE_ADDR,DENALI_PHY_305_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(305 + PHY_BASE_ADDR,DENALI_PHY_305_DATA,0);
  #else
         REGWR(305 + PHY_BASE_ADDR,DENALI_PHY_305_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_306_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(306 + PHY_BASE_ADDR,DENALI_PHY_306_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(306 + PHY_BASE_ADDR,DENALI_PHY_306_DATA,0);
  #else
         REGWR(306 + PHY_BASE_ADDR,DENALI_PHY_306_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_307_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(307 + PHY_BASE_ADDR,DENALI_PHY_307_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(307 + PHY_BASE_ADDR,DENALI_PHY_307_DATA,0);
  #else
         REGWR(307 + PHY_BASE_ADDR,DENALI_PHY_307_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_308_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(308 + PHY_BASE_ADDR,DENALI_PHY_308_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(308 + PHY_BASE_ADDR,DENALI_PHY_308_DATA,0);
  #else
         REGWR(308 + PHY_BASE_ADDR,DENALI_PHY_308_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_309_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(309 + PHY_BASE_ADDR,DENALI_PHY_309_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(309 + PHY_BASE_ADDR,DENALI_PHY_309_DATA,0);
  #else
         REGWR(309 + PHY_BASE_ADDR,DENALI_PHY_309_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_310_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(310 + PHY_BASE_ADDR,DENALI_PHY_310_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(310 + PHY_BASE_ADDR,DENALI_PHY_310_DATA,0);
  #else
         REGWR(310 + PHY_BASE_ADDR,DENALI_PHY_310_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_311_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(311 + PHY_BASE_ADDR,DENALI_PHY_311_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(311 + PHY_BASE_ADDR,DENALI_PHY_311_DATA,0);
  #else
         REGWR(311 + PHY_BASE_ADDR,DENALI_PHY_311_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_312_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(312 + PHY_BASE_ADDR,DENALI_PHY_312_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(312 + PHY_BASE_ADDR,DENALI_PHY_312_DATA,0);
  #else
         REGWR(312 + PHY_BASE_ADDR,DENALI_PHY_312_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_313_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(313 + PHY_BASE_ADDR,DENALI_PHY_313_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(313 + PHY_BASE_ADDR,DENALI_PHY_313_DATA,0);
  #else
         REGWR(313 + PHY_BASE_ADDR,DENALI_PHY_313_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_314_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(314 + PHY_BASE_ADDR,DENALI_PHY_314_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(314 + PHY_BASE_ADDR,DENALI_PHY_314_DATA,0);
  #else
         REGWR(314 + PHY_BASE_ADDR,DENALI_PHY_314_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_315_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(315 + PHY_BASE_ADDR,DENALI_PHY_315_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(315 + PHY_BASE_ADDR,DENALI_PHY_315_DATA,0);
  #else
         REGWR(315 + PHY_BASE_ADDR,DENALI_PHY_315_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_316_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(316 + PHY_BASE_ADDR,DENALI_PHY_316_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(316 + PHY_BASE_ADDR,DENALI_PHY_316_DATA,0);
  #else
         REGWR(316 + PHY_BASE_ADDR,DENALI_PHY_316_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_317_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(317 + PHY_BASE_ADDR,DENALI_PHY_317_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(317 + PHY_BASE_ADDR,DENALI_PHY_317_DATA,0);
  #else
         REGWR(317 + PHY_BASE_ADDR,DENALI_PHY_317_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_318_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(318 + PHY_BASE_ADDR,DENALI_PHY_318_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(318 + PHY_BASE_ADDR,DENALI_PHY_318_DATA,0);
  #else
         REGWR(318 + PHY_BASE_ADDR,DENALI_PHY_318_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_319_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(319 + PHY_BASE_ADDR,DENALI_PHY_319_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(319 + PHY_BASE_ADDR,DENALI_PHY_319_DATA,0);
  #else
         REGWR(319 + PHY_BASE_ADDR,DENALI_PHY_319_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_320_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(320 + PHY_BASE_ADDR,DENALI_PHY_320_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(320 + PHY_BASE_ADDR,DENALI_PHY_320_DATA,0);
  #else
         REGWR(320 + PHY_BASE_ADDR,DENALI_PHY_320_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_321_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(321 + PHY_BASE_ADDR,DENALI_PHY_321_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(321 + PHY_BASE_ADDR,DENALI_PHY_321_DATA,0);
  #else
         REGWR(321 + PHY_BASE_ADDR,DENALI_PHY_321_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_322_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(322 + PHY_BASE_ADDR,DENALI_PHY_322_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(322 + PHY_BASE_ADDR,DENALI_PHY_322_DATA,0);
  #else
         REGWR(322 + PHY_BASE_ADDR,DENALI_PHY_322_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_323_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(323 + PHY_BASE_ADDR,DENALI_PHY_323_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(323 + PHY_BASE_ADDR,DENALI_PHY_323_DATA,0);
  #else
         REGWR(323 + PHY_BASE_ADDR,DENALI_PHY_323_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_324_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(324 + PHY_BASE_ADDR,DENALI_PHY_324_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(324 + PHY_BASE_ADDR,DENALI_PHY_324_DATA,0);
  #else
         REGWR(324 + PHY_BASE_ADDR,DENALI_PHY_324_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_325_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(325 + PHY_BASE_ADDR,DENALI_PHY_325_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(325 + PHY_BASE_ADDR,DENALI_PHY_325_DATA,0);
  #else
         REGWR(325 + PHY_BASE_ADDR,DENALI_PHY_325_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_326_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(326 + PHY_BASE_ADDR,DENALI_PHY_326_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(326 + PHY_BASE_ADDR,DENALI_PHY_326_DATA,0);
  #else
         REGWR(326 + PHY_BASE_ADDR,DENALI_PHY_326_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_327_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(327 + PHY_BASE_ADDR,DENALI_PHY_327_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(327 + PHY_BASE_ADDR,DENALI_PHY_327_DATA,0);
  #else
         REGWR(327 + PHY_BASE_ADDR,DENALI_PHY_327_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_328_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(328 + PHY_BASE_ADDR,DENALI_PHY_328_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(328 + PHY_BASE_ADDR,DENALI_PHY_328_DATA,0);
  #else
         REGWR(328 + PHY_BASE_ADDR,DENALI_PHY_328_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_329_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(329 + PHY_BASE_ADDR,DENALI_PHY_329_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(329 + PHY_BASE_ADDR,DENALI_PHY_329_DATA,0);
  #else
         REGWR(329 + PHY_BASE_ADDR,DENALI_PHY_329_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_330_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(330 + PHY_BASE_ADDR,DENALI_PHY_330_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(330 + PHY_BASE_ADDR,DENALI_PHY_330_DATA,0);
  #else
         REGWR(330 + PHY_BASE_ADDR,DENALI_PHY_330_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_331_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(331 + PHY_BASE_ADDR,DENALI_PHY_331_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(331 + PHY_BASE_ADDR,DENALI_PHY_331_DATA,0);
  #else
         REGWR(331 + PHY_BASE_ADDR,DENALI_PHY_331_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_332_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(332 + PHY_BASE_ADDR,DENALI_PHY_332_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(332 + PHY_BASE_ADDR,DENALI_PHY_332_DATA,0);
  #else
         REGWR(332 + PHY_BASE_ADDR,DENALI_PHY_332_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_333_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(333 + PHY_BASE_ADDR,DENALI_PHY_333_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(333 + PHY_BASE_ADDR,DENALI_PHY_333_DATA,0);
  #else
         REGWR(333 + PHY_BASE_ADDR,DENALI_PHY_333_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_334_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(334 + PHY_BASE_ADDR,DENALI_PHY_334_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(334 + PHY_BASE_ADDR,DENALI_PHY_334_DATA,0);
  #else
         REGWR(334 + PHY_BASE_ADDR,DENALI_PHY_334_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_335_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(335 + PHY_BASE_ADDR,DENALI_PHY_335_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(335 + PHY_BASE_ADDR,DENALI_PHY_335_DATA,0);
  #else
         REGWR(335 + PHY_BASE_ADDR,DENALI_PHY_335_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_336_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(336 + PHY_BASE_ADDR,DENALI_PHY_336_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(336 + PHY_BASE_ADDR,DENALI_PHY_336_DATA,0);
  #else
         REGWR(336 + PHY_BASE_ADDR,DENALI_PHY_336_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_337_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(337 + PHY_BASE_ADDR,DENALI_PHY_337_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(337 + PHY_BASE_ADDR,DENALI_PHY_337_DATA,0);
  #else
         REGWR(337 + PHY_BASE_ADDR,DENALI_PHY_337_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_338_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(338 + PHY_BASE_ADDR,DENALI_PHY_338_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(338 + PHY_BASE_ADDR,DENALI_PHY_338_DATA,0);
  #else
         REGWR(338 + PHY_BASE_ADDR,DENALI_PHY_338_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_339_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(339 + PHY_BASE_ADDR,DENALI_PHY_339_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(339 + PHY_BASE_ADDR,DENALI_PHY_339_DATA,0);
  #else
         REGWR(339 + PHY_BASE_ADDR,DENALI_PHY_339_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_340_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(340 + PHY_BASE_ADDR,DENALI_PHY_340_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(340 + PHY_BASE_ADDR,DENALI_PHY_340_DATA,0);
  #else
         REGWR(340 + PHY_BASE_ADDR,DENALI_PHY_340_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_341_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(341 + PHY_BASE_ADDR,DENALI_PHY_341_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(341 + PHY_BASE_ADDR,DENALI_PHY_341_DATA,0);
  #else
         REGWR(341 + PHY_BASE_ADDR,DENALI_PHY_341_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_342_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(342 + PHY_BASE_ADDR,DENALI_PHY_342_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(342 + PHY_BASE_ADDR,DENALI_PHY_342_DATA,0);
  #else
         REGWR(342 + PHY_BASE_ADDR,DENALI_PHY_342_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_343_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(343 + PHY_BASE_ADDR,DENALI_PHY_343_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(343 + PHY_BASE_ADDR,DENALI_PHY_343_DATA,0);
  #else
         REGWR(343 + PHY_BASE_ADDR,DENALI_PHY_343_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_344_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(344 + PHY_BASE_ADDR,DENALI_PHY_344_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(344 + PHY_BASE_ADDR,DENALI_PHY_344_DATA,0);
  #else
         REGWR(344 + PHY_BASE_ADDR,DENALI_PHY_344_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_345_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(345 + PHY_BASE_ADDR,DENALI_PHY_345_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(345 + PHY_BASE_ADDR,DENALI_PHY_345_DATA,0);
  #else
         REGWR(345 + PHY_BASE_ADDR,DENALI_PHY_345_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_346_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(346 + PHY_BASE_ADDR,DENALI_PHY_346_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(346 + PHY_BASE_ADDR,DENALI_PHY_346_DATA,0);
  #else
         REGWR(346 + PHY_BASE_ADDR,DENALI_PHY_346_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_347_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(347 + PHY_BASE_ADDR,DENALI_PHY_347_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(347 + PHY_BASE_ADDR,DENALI_PHY_347_DATA,0);
  #else
         REGWR(347 + PHY_BASE_ADDR,DENALI_PHY_347_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_348_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(348 + PHY_BASE_ADDR,DENALI_PHY_348_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(348 + PHY_BASE_ADDR,DENALI_PHY_348_DATA,0);
  #else
         REGWR(348 + PHY_BASE_ADDR,DENALI_PHY_348_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_349_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(349 + PHY_BASE_ADDR,DENALI_PHY_349_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(349 + PHY_BASE_ADDR,DENALI_PHY_349_DATA,0);
  #else
         REGWR(349 + PHY_BASE_ADDR,DENALI_PHY_349_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_350_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(350 + PHY_BASE_ADDR,DENALI_PHY_350_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(350 + PHY_BASE_ADDR,DENALI_PHY_350_DATA,0);
  #else
         REGWR(350 + PHY_BASE_ADDR,DENALI_PHY_350_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_351_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(351 + PHY_BASE_ADDR,DENALI_PHY_351_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(351 + PHY_BASE_ADDR,DENALI_PHY_351_DATA,0);
  #else
         REGWR(351 + PHY_BASE_ADDR,DENALI_PHY_351_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_352_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(352 + PHY_BASE_ADDR,DENALI_PHY_352_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(352 + PHY_BASE_ADDR,DENALI_PHY_352_DATA,0);
  #else
         REGWR(352 + PHY_BASE_ADDR,DENALI_PHY_352_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_353_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(353 + PHY_BASE_ADDR,DENALI_PHY_353_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(353 + PHY_BASE_ADDR,DENALI_PHY_353_DATA,0);
  #else
         REGWR(353 + PHY_BASE_ADDR,DENALI_PHY_353_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_354_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(354 + PHY_BASE_ADDR,DENALI_PHY_354_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(354 + PHY_BASE_ADDR,DENALI_PHY_354_DATA,0);
  #else
         REGWR(354 + PHY_BASE_ADDR,DENALI_PHY_354_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_355_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(355 + PHY_BASE_ADDR,DENALI_PHY_355_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(355 + PHY_BASE_ADDR,DENALI_PHY_355_DATA,0);
  #else
         REGWR(355 + PHY_BASE_ADDR,DENALI_PHY_355_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_356_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(356 + PHY_BASE_ADDR,DENALI_PHY_356_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(356 + PHY_BASE_ADDR,DENALI_PHY_356_DATA,0);
  #else
         REGWR(356 + PHY_BASE_ADDR,DENALI_PHY_356_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_357_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(357 + PHY_BASE_ADDR,DENALI_PHY_357_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(357 + PHY_BASE_ADDR,DENALI_PHY_357_DATA,0);
  #else
         REGWR(357 + PHY_BASE_ADDR,DENALI_PHY_357_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_358_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(358 + PHY_BASE_ADDR,DENALI_PHY_358_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(358 + PHY_BASE_ADDR,DENALI_PHY_358_DATA,0);
  #else
         REGWR(358 + PHY_BASE_ADDR,DENALI_PHY_358_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_359_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(359 + PHY_BASE_ADDR,DENALI_PHY_359_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(359 + PHY_BASE_ADDR,DENALI_PHY_359_DATA,0);
  #else
         REGWR(359 + PHY_BASE_ADDR,DENALI_PHY_359_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_360_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(360 + PHY_BASE_ADDR,DENALI_PHY_360_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(360 + PHY_BASE_ADDR,DENALI_PHY_360_DATA,0);
  #else
         REGWR(360 + PHY_BASE_ADDR,DENALI_PHY_360_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_361_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(361 + PHY_BASE_ADDR,DENALI_PHY_361_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(361 + PHY_BASE_ADDR,DENALI_PHY_361_DATA,0);
  #else
         REGWR(361 + PHY_BASE_ADDR,DENALI_PHY_361_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_362_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(362 + PHY_BASE_ADDR,DENALI_PHY_362_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(362 + PHY_BASE_ADDR,DENALI_PHY_362_DATA,0);
  #else
         REGWR(362 + PHY_BASE_ADDR,DENALI_PHY_362_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_363_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(363 + PHY_BASE_ADDR,DENALI_PHY_363_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(363 + PHY_BASE_ADDR,DENALI_PHY_363_DATA,0);
  #else
         REGWR(363 + PHY_BASE_ADDR,DENALI_PHY_363_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_364_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(364 + PHY_BASE_ADDR,DENALI_PHY_364_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(364 + PHY_BASE_ADDR,DENALI_PHY_364_DATA,0);
  #else
         REGWR(364 + PHY_BASE_ADDR,DENALI_PHY_364_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_365_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(365 + PHY_BASE_ADDR,DENALI_PHY_365_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(365 + PHY_BASE_ADDR,DENALI_PHY_365_DATA,0);
  #else
         REGWR(365 + PHY_BASE_ADDR,DENALI_PHY_365_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_366_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(366 + PHY_BASE_ADDR,DENALI_PHY_366_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(366 + PHY_BASE_ADDR,DENALI_PHY_366_DATA,0);
  #else
         REGWR(366 + PHY_BASE_ADDR,DENALI_PHY_366_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_367_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(367 + PHY_BASE_ADDR,DENALI_PHY_367_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(367 + PHY_BASE_ADDR,DENALI_PHY_367_DATA,0);
  #else
         REGWR(367 + PHY_BASE_ADDR,DENALI_PHY_367_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_368_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(368 + PHY_BASE_ADDR,DENALI_PHY_368_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(368 + PHY_BASE_ADDR,DENALI_PHY_368_DATA,0);
  #else
         REGWR(368 + PHY_BASE_ADDR,DENALI_PHY_368_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_369_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(369 + PHY_BASE_ADDR,DENALI_PHY_369_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(369 + PHY_BASE_ADDR,DENALI_PHY_369_DATA,0);
  #else
         REGWR(369 + PHY_BASE_ADDR,DENALI_PHY_369_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_370_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(370 + PHY_BASE_ADDR,DENALI_PHY_370_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(370 + PHY_BASE_ADDR,DENALI_PHY_370_DATA,0);
  #else
         REGWR(370 + PHY_BASE_ADDR,DENALI_PHY_370_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_371_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(371 + PHY_BASE_ADDR,DENALI_PHY_371_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(371 + PHY_BASE_ADDR,DENALI_PHY_371_DATA,0);
  #else
         REGWR(371 + PHY_BASE_ADDR,DENALI_PHY_371_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_372_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(372 + PHY_BASE_ADDR,DENALI_PHY_372_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(372 + PHY_BASE_ADDR,DENALI_PHY_372_DATA,0);
  #else
         REGWR(372 + PHY_BASE_ADDR,DENALI_PHY_372_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_373_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(373 + PHY_BASE_ADDR,DENALI_PHY_373_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(373 + PHY_BASE_ADDR,DENALI_PHY_373_DATA,0);
  #else
         REGWR(373 + PHY_BASE_ADDR,DENALI_PHY_373_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_374_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(374 + PHY_BASE_ADDR,DENALI_PHY_374_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(374 + PHY_BASE_ADDR,DENALI_PHY_374_DATA,0);
  #else
         REGWR(374 + PHY_BASE_ADDR,DENALI_PHY_374_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_375_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(375 + PHY_BASE_ADDR,DENALI_PHY_375_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(375 + PHY_BASE_ADDR,DENALI_PHY_375_DATA,0);
  #else
         REGWR(375 + PHY_BASE_ADDR,DENALI_PHY_375_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_376_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(376 + PHY_BASE_ADDR,DENALI_PHY_376_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(376 + PHY_BASE_ADDR,DENALI_PHY_376_DATA,0);
  #else
         REGWR(376 + PHY_BASE_ADDR,DENALI_PHY_376_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_377_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(377 + PHY_BASE_ADDR,DENALI_PHY_377_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(377 + PHY_BASE_ADDR,DENALI_PHY_377_DATA,0);
  #else
         REGWR(377 + PHY_BASE_ADDR,DENALI_PHY_377_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_378_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(378 + PHY_BASE_ADDR,DENALI_PHY_378_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(378 + PHY_BASE_ADDR,DENALI_PHY_378_DATA,0);
  #else
         REGWR(378 + PHY_BASE_ADDR,DENALI_PHY_378_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_379_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(379 + PHY_BASE_ADDR,DENALI_PHY_379_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(379 + PHY_BASE_ADDR,DENALI_PHY_379_DATA,0);
  #else
         REGWR(379 + PHY_BASE_ADDR,DENALI_PHY_379_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_380_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(380 + PHY_BASE_ADDR,DENALI_PHY_380_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(380 + PHY_BASE_ADDR,DENALI_PHY_380_DATA,0);
  #else
         REGWR(380 + PHY_BASE_ADDR,DENALI_PHY_380_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_381_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(381 + PHY_BASE_ADDR,DENALI_PHY_381_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(381 + PHY_BASE_ADDR,DENALI_PHY_381_DATA,0);
  #else
         REGWR(381 + PHY_BASE_ADDR,DENALI_PHY_381_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_382_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(382 + PHY_BASE_ADDR,DENALI_PHY_382_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(382 + PHY_BASE_ADDR,DENALI_PHY_382_DATA,0);
  #else
         REGWR(382 + PHY_BASE_ADDR,DENALI_PHY_382_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_383_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(383 + PHY_BASE_ADDR,DENALI_PHY_383_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(383 + PHY_BASE_ADDR,DENALI_PHY_383_DATA,0);
  #else
         REGWR(383 + PHY_BASE_ADDR,DENALI_PHY_383_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_384_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(384 + PHY_BASE_ADDR,DENALI_PHY_384_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(384 + PHY_BASE_ADDR,DENALI_PHY_384_DATA,0);
  #else
         REGWR(384 + PHY_BASE_ADDR,DENALI_PHY_384_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_385_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(385 + PHY_BASE_ADDR,DENALI_PHY_385_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(385 + PHY_BASE_ADDR,DENALI_PHY_385_DATA,0);
  #else
         REGWR(385 + PHY_BASE_ADDR,DENALI_PHY_385_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_386_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(386 + PHY_BASE_ADDR,DENALI_PHY_386_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(386 + PHY_BASE_ADDR,DENALI_PHY_386_DATA,0);
  #else
         REGWR(386 + PHY_BASE_ADDR,DENALI_PHY_386_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_387_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(387 + PHY_BASE_ADDR,DENALI_PHY_387_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(387 + PHY_BASE_ADDR,DENALI_PHY_387_DATA,0);
  #else
         REGWR(387 + PHY_BASE_ADDR,DENALI_PHY_387_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_388_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(388 + PHY_BASE_ADDR,DENALI_PHY_388_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(388 + PHY_BASE_ADDR,DENALI_PHY_388_DATA,0);
  #else
         REGWR(388 + PHY_BASE_ADDR,DENALI_PHY_388_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_389_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(389 + PHY_BASE_ADDR,DENALI_PHY_389_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(389 + PHY_BASE_ADDR,DENALI_PHY_389_DATA,0);
  #else
         REGWR(389 + PHY_BASE_ADDR,DENALI_PHY_389_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_390_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(390 + PHY_BASE_ADDR,DENALI_PHY_390_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(390 + PHY_BASE_ADDR,DENALI_PHY_390_DATA,0);
  #else
         REGWR(390 + PHY_BASE_ADDR,DENALI_PHY_390_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_391_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(391 + PHY_BASE_ADDR,DENALI_PHY_391_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(391 + PHY_BASE_ADDR,DENALI_PHY_391_DATA,0);
  #else
         REGWR(391 + PHY_BASE_ADDR,DENALI_PHY_391_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_392_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(392 + PHY_BASE_ADDR,DENALI_PHY_392_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(392 + PHY_BASE_ADDR,DENALI_PHY_392_DATA,0);
  #else
         REGWR(392 + PHY_BASE_ADDR,DENALI_PHY_392_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_393_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(393 + PHY_BASE_ADDR,DENALI_PHY_393_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(393 + PHY_BASE_ADDR,DENALI_PHY_393_DATA,0);
  #else
         REGWR(393 + PHY_BASE_ADDR,DENALI_PHY_393_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_394_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(394 + PHY_BASE_ADDR,DENALI_PHY_394_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(394 + PHY_BASE_ADDR,DENALI_PHY_394_DATA,0);
  #else
         REGWR(394 + PHY_BASE_ADDR,DENALI_PHY_394_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_395_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(395 + PHY_BASE_ADDR,DENALI_PHY_395_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(395 + PHY_BASE_ADDR,DENALI_PHY_395_DATA,0);
  #else
         REGWR(395 + PHY_BASE_ADDR,DENALI_PHY_395_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_396_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(396 + PHY_BASE_ADDR,DENALI_PHY_396_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(396 + PHY_BASE_ADDR,DENALI_PHY_396_DATA,0);
  #else
         REGWR(396 + PHY_BASE_ADDR,DENALI_PHY_396_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_397_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(397 + PHY_BASE_ADDR,DENALI_PHY_397_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(397 + PHY_BASE_ADDR,DENALI_PHY_397_DATA,0);
  #else
         REGWR(397 + PHY_BASE_ADDR,DENALI_PHY_397_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_398_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(398 + PHY_BASE_ADDR,DENALI_PHY_398_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(398 + PHY_BASE_ADDR,DENALI_PHY_398_DATA,0);
  #else
         REGWR(398 + PHY_BASE_ADDR,DENALI_PHY_398_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_399_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(399 + PHY_BASE_ADDR,DENALI_PHY_399_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(399 + PHY_BASE_ADDR,DENALI_PHY_399_DATA,0);
  #else
         REGWR(399 + PHY_BASE_ADDR,DENALI_PHY_399_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_400_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(400 + PHY_BASE_ADDR,DENALI_PHY_400_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(400 + PHY_BASE_ADDR,DENALI_PHY_400_DATA,0);
  #else
         REGWR(400 + PHY_BASE_ADDR,DENALI_PHY_400_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_401_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(401 + PHY_BASE_ADDR,DENALI_PHY_401_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(401 + PHY_BASE_ADDR,DENALI_PHY_401_DATA,0);
  #else
         REGWR(401 + PHY_BASE_ADDR,DENALI_PHY_401_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_402_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(402 + PHY_BASE_ADDR,DENALI_PHY_402_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(402 + PHY_BASE_ADDR,DENALI_PHY_402_DATA,0);
  #else
         REGWR(402 + PHY_BASE_ADDR,DENALI_PHY_402_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_403_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(403 + PHY_BASE_ADDR,DENALI_PHY_403_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(403 + PHY_BASE_ADDR,DENALI_PHY_403_DATA,0);
  #else
         REGWR(403 + PHY_BASE_ADDR,DENALI_PHY_403_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_404_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(404 + PHY_BASE_ADDR,DENALI_PHY_404_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(404 + PHY_BASE_ADDR,DENALI_PHY_404_DATA,0);
  #else
         REGWR(404 + PHY_BASE_ADDR,DENALI_PHY_404_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_405_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(405 + PHY_BASE_ADDR,DENALI_PHY_405_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(405 + PHY_BASE_ADDR,DENALI_PHY_405_DATA,0);
  #else
         REGWR(405 + PHY_BASE_ADDR,DENALI_PHY_405_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_406_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(406 + PHY_BASE_ADDR,DENALI_PHY_406_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(406 + PHY_BASE_ADDR,DENALI_PHY_406_DATA,0);
  #else
         REGWR(406 + PHY_BASE_ADDR,DENALI_PHY_406_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_407_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(407 + PHY_BASE_ADDR,DENALI_PHY_407_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(407 + PHY_BASE_ADDR,DENALI_PHY_407_DATA,0);
  #else
         REGWR(407 + PHY_BASE_ADDR,DENALI_PHY_407_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_408_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(408 + PHY_BASE_ADDR,DENALI_PHY_408_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(408 + PHY_BASE_ADDR,DENALI_PHY_408_DATA,0);
  #else
         REGWR(408 + PHY_BASE_ADDR,DENALI_PHY_408_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_409_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(409 + PHY_BASE_ADDR,DENALI_PHY_409_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(409 + PHY_BASE_ADDR,DENALI_PHY_409_DATA,0);
  #else
         REGWR(409 + PHY_BASE_ADDR,DENALI_PHY_409_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_410_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(410 + PHY_BASE_ADDR,DENALI_PHY_410_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(410 + PHY_BASE_ADDR,DENALI_PHY_410_DATA,0);
  #else
         REGWR(410 + PHY_BASE_ADDR,DENALI_PHY_410_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_411_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(411 + PHY_BASE_ADDR,DENALI_PHY_411_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(411 + PHY_BASE_ADDR,DENALI_PHY_411_DATA,0);
  #else
         REGWR(411 + PHY_BASE_ADDR,DENALI_PHY_411_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_412_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(412 + PHY_BASE_ADDR,DENALI_PHY_412_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(412 + PHY_BASE_ADDR,DENALI_PHY_412_DATA,0);
  #else
         REGWR(412 + PHY_BASE_ADDR,DENALI_PHY_412_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_413_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(413 + PHY_BASE_ADDR,DENALI_PHY_413_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(413 + PHY_BASE_ADDR,DENALI_PHY_413_DATA,0);
  #else
         REGWR(413 + PHY_BASE_ADDR,DENALI_PHY_413_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_414_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(414 + PHY_BASE_ADDR,DENALI_PHY_414_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(414 + PHY_BASE_ADDR,DENALI_PHY_414_DATA,0);
  #else
         REGWR(414 + PHY_BASE_ADDR,DENALI_PHY_414_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_415_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(415 + PHY_BASE_ADDR,DENALI_PHY_415_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(415 + PHY_BASE_ADDR,DENALI_PHY_415_DATA,0);
  #else
         REGWR(415 + PHY_BASE_ADDR,DENALI_PHY_415_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_416_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(416 + PHY_BASE_ADDR,DENALI_PHY_416_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(416 + PHY_BASE_ADDR,DENALI_PHY_416_DATA,0);
  #else
         REGWR(416 + PHY_BASE_ADDR,DENALI_PHY_416_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_417_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(417 + PHY_BASE_ADDR,DENALI_PHY_417_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(417 + PHY_BASE_ADDR,DENALI_PHY_417_DATA,0);
  #else
         REGWR(417 + PHY_BASE_ADDR,DENALI_PHY_417_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_418_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(418 + PHY_BASE_ADDR,DENALI_PHY_418_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(418 + PHY_BASE_ADDR,DENALI_PHY_418_DATA,0);
  #else
         REGWR(418 + PHY_BASE_ADDR,DENALI_PHY_418_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_419_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(419 + PHY_BASE_ADDR,DENALI_PHY_419_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(419 + PHY_BASE_ADDR,DENALI_PHY_419_DATA,0);
  #else
         REGWR(419 + PHY_BASE_ADDR,DENALI_PHY_419_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_420_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(420 + PHY_BASE_ADDR,DENALI_PHY_420_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(420 + PHY_BASE_ADDR,DENALI_PHY_420_DATA,0);
  #else
         REGWR(420 + PHY_BASE_ADDR,DENALI_PHY_420_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_421_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(421 + PHY_BASE_ADDR,DENALI_PHY_421_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(421 + PHY_BASE_ADDR,DENALI_PHY_421_DATA,0);
  #else
         REGWR(421 + PHY_BASE_ADDR,DENALI_PHY_421_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_422_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(422 + PHY_BASE_ADDR,DENALI_PHY_422_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(422 + PHY_BASE_ADDR,DENALI_PHY_422_DATA,0);
  #else
         REGWR(422 + PHY_BASE_ADDR,DENALI_PHY_422_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_423_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(423 + PHY_BASE_ADDR,DENALI_PHY_423_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(423 + PHY_BASE_ADDR,DENALI_PHY_423_DATA,0);
  #else
         REGWR(423 + PHY_BASE_ADDR,DENALI_PHY_423_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_424_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(424 + PHY_BASE_ADDR,DENALI_PHY_424_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(424 + PHY_BASE_ADDR,DENALI_PHY_424_DATA,0);
  #else
         REGWR(424 + PHY_BASE_ADDR,DENALI_PHY_424_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_425_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(425 + PHY_BASE_ADDR,DENALI_PHY_425_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(425 + PHY_BASE_ADDR,DENALI_PHY_425_DATA,0);
  #else
         REGWR(425 + PHY_BASE_ADDR,DENALI_PHY_425_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_426_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(426 + PHY_BASE_ADDR,DENALI_PHY_426_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(426 + PHY_BASE_ADDR,DENALI_PHY_426_DATA,0);
  #else
         REGWR(426 + PHY_BASE_ADDR,DENALI_PHY_426_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_427_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(427 + PHY_BASE_ADDR,DENALI_PHY_427_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(427 + PHY_BASE_ADDR,DENALI_PHY_427_DATA,0);
  #else
         REGWR(427 + PHY_BASE_ADDR,DENALI_PHY_427_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_428_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(428 + PHY_BASE_ADDR,DENALI_PHY_428_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(428 + PHY_BASE_ADDR,DENALI_PHY_428_DATA,0);
  #else
         REGWR(428 + PHY_BASE_ADDR,DENALI_PHY_428_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_429_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(429 + PHY_BASE_ADDR,DENALI_PHY_429_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(429 + PHY_BASE_ADDR,DENALI_PHY_429_DATA,0);
  #else
         REGWR(429 + PHY_BASE_ADDR,DENALI_PHY_429_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_430_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(430 + PHY_BASE_ADDR,DENALI_PHY_430_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(430 + PHY_BASE_ADDR,DENALI_PHY_430_DATA,0);
  #else
         REGWR(430 + PHY_BASE_ADDR,DENALI_PHY_430_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_431_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(431 + PHY_BASE_ADDR,DENALI_PHY_431_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(431 + PHY_BASE_ADDR,DENALI_PHY_431_DATA,0);
  #else
         REGWR(431 + PHY_BASE_ADDR,DENALI_PHY_431_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_432_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(432 + PHY_BASE_ADDR,DENALI_PHY_432_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(432 + PHY_BASE_ADDR,DENALI_PHY_432_DATA,0);
  #else
         REGWR(432 + PHY_BASE_ADDR,DENALI_PHY_432_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_433_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(433 + PHY_BASE_ADDR,DENALI_PHY_433_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(433 + PHY_BASE_ADDR,DENALI_PHY_433_DATA,0);
  #else
         REGWR(433 + PHY_BASE_ADDR,DENALI_PHY_433_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_434_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(434 + PHY_BASE_ADDR,DENALI_PHY_434_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(434 + PHY_BASE_ADDR,DENALI_PHY_434_DATA,0);
  #else
         REGWR(434 + PHY_BASE_ADDR,DENALI_PHY_434_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_435_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(435 + PHY_BASE_ADDR,DENALI_PHY_435_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(435 + PHY_BASE_ADDR,DENALI_PHY_435_DATA,0);
  #else
         REGWR(435 + PHY_BASE_ADDR,DENALI_PHY_435_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_436_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(436 + PHY_BASE_ADDR,DENALI_PHY_436_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(436 + PHY_BASE_ADDR,DENALI_PHY_436_DATA,0);
  #else
         REGWR(436 + PHY_BASE_ADDR,DENALI_PHY_436_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_437_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(437 + PHY_BASE_ADDR,DENALI_PHY_437_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(437 + PHY_BASE_ADDR,DENALI_PHY_437_DATA,0);
  #else
         REGWR(437 + PHY_BASE_ADDR,DENALI_PHY_437_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_438_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(438 + PHY_BASE_ADDR,DENALI_PHY_438_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(438 + PHY_BASE_ADDR,DENALI_PHY_438_DATA,0);
  #else
         REGWR(438 + PHY_BASE_ADDR,DENALI_PHY_438_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_439_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(439 + PHY_BASE_ADDR,DENALI_PHY_439_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(439 + PHY_BASE_ADDR,DENALI_PHY_439_DATA,0);
  #else
         REGWR(439 + PHY_BASE_ADDR,DENALI_PHY_439_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_440_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(440 + PHY_BASE_ADDR,DENALI_PHY_440_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(440 + PHY_BASE_ADDR,DENALI_PHY_440_DATA,0);
  #else
         REGWR(440 + PHY_BASE_ADDR,DENALI_PHY_440_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_441_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(441 + PHY_BASE_ADDR,DENALI_PHY_441_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(441 + PHY_BASE_ADDR,DENALI_PHY_441_DATA,0);
  #else
         REGWR(441 + PHY_BASE_ADDR,DENALI_PHY_441_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_442_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(442 + PHY_BASE_ADDR,DENALI_PHY_442_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(442 + PHY_BASE_ADDR,DENALI_PHY_442_DATA,0);
  #else
         REGWR(442 + PHY_BASE_ADDR,DENALI_PHY_442_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_443_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(443 + PHY_BASE_ADDR,DENALI_PHY_443_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(443 + PHY_BASE_ADDR,DENALI_PHY_443_DATA,0);
  #else
         REGWR(443 + PHY_BASE_ADDR,DENALI_PHY_443_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_444_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(444 + PHY_BASE_ADDR,DENALI_PHY_444_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(444 + PHY_BASE_ADDR,DENALI_PHY_444_DATA,0);
  #else
         REGWR(444 + PHY_BASE_ADDR,DENALI_PHY_444_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_445_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(445 + PHY_BASE_ADDR,DENALI_PHY_445_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(445 + PHY_BASE_ADDR,DENALI_PHY_445_DATA,0);
  #else
         REGWR(445 + PHY_BASE_ADDR,DENALI_PHY_445_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_446_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(446 + PHY_BASE_ADDR,DENALI_PHY_446_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(446 + PHY_BASE_ADDR,DENALI_PHY_446_DATA,0);
  #else
         REGWR(446 + PHY_BASE_ADDR,DENALI_PHY_446_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_447_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(447 + PHY_BASE_ADDR,DENALI_PHY_447_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(447 + PHY_BASE_ADDR,DENALI_PHY_447_DATA,0);
  #else
         REGWR(447 + PHY_BASE_ADDR,DENALI_PHY_447_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_448_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(448 + PHY_BASE_ADDR,DENALI_PHY_448_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(448 + PHY_BASE_ADDR,DENALI_PHY_448_DATA,0);
  #else
         REGWR(448 + PHY_BASE_ADDR,DENALI_PHY_448_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_449_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(449 + PHY_BASE_ADDR,DENALI_PHY_449_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(449 + PHY_BASE_ADDR,DENALI_PHY_449_DATA,0);
  #else
         REGWR(449 + PHY_BASE_ADDR,DENALI_PHY_449_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_450_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(450 + PHY_BASE_ADDR,DENALI_PHY_450_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(450 + PHY_BASE_ADDR,DENALI_PHY_450_DATA,0);
  #else
         REGWR(450 + PHY_BASE_ADDR,DENALI_PHY_450_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_451_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(451 + PHY_BASE_ADDR,DENALI_PHY_451_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(451 + PHY_BASE_ADDR,DENALI_PHY_451_DATA,0);
  #else
         REGWR(451 + PHY_BASE_ADDR,DENALI_PHY_451_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_452_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(452 + PHY_BASE_ADDR,DENALI_PHY_452_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(452 + PHY_BASE_ADDR,DENALI_PHY_452_DATA,0);
  #else
         REGWR(452 + PHY_BASE_ADDR,DENALI_PHY_452_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_453_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(453 + PHY_BASE_ADDR,DENALI_PHY_453_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(453 + PHY_BASE_ADDR,DENALI_PHY_453_DATA,0);
  #else
         REGWR(453 + PHY_BASE_ADDR,DENALI_PHY_453_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_454_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(454 + PHY_BASE_ADDR,DENALI_PHY_454_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(454 + PHY_BASE_ADDR,DENALI_PHY_454_DATA,0);
  #else
         REGWR(454 + PHY_BASE_ADDR,DENALI_PHY_454_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_455_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(455 + PHY_BASE_ADDR,DENALI_PHY_455_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(455 + PHY_BASE_ADDR,DENALI_PHY_455_DATA,0);
  #else
         REGWR(455 + PHY_BASE_ADDR,DENALI_PHY_455_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_456_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(456 + PHY_BASE_ADDR,DENALI_PHY_456_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(456 + PHY_BASE_ADDR,DENALI_PHY_456_DATA,0);
  #else
         REGWR(456 + PHY_BASE_ADDR,DENALI_PHY_456_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_457_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(457 + PHY_BASE_ADDR,DENALI_PHY_457_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(457 + PHY_BASE_ADDR,DENALI_PHY_457_DATA,0);
  #else
         REGWR(457 + PHY_BASE_ADDR,DENALI_PHY_457_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_458_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(458 + PHY_BASE_ADDR,DENALI_PHY_458_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(458 + PHY_BASE_ADDR,DENALI_PHY_458_DATA,0);
  #else
         REGWR(458 + PHY_BASE_ADDR,DENALI_PHY_458_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_459_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(459 + PHY_BASE_ADDR,DENALI_PHY_459_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(459 + PHY_BASE_ADDR,DENALI_PHY_459_DATA,0);
  #else
         REGWR(459 + PHY_BASE_ADDR,DENALI_PHY_459_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_460_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(460 + PHY_BASE_ADDR,DENALI_PHY_460_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(460 + PHY_BASE_ADDR,DENALI_PHY_460_DATA,0);
  #else
         REGWR(460 + PHY_BASE_ADDR,DENALI_PHY_460_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_461_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(461 + PHY_BASE_ADDR,DENALI_PHY_461_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(461 + PHY_BASE_ADDR,DENALI_PHY_461_DATA,0);
  #else
         REGWR(461 + PHY_BASE_ADDR,DENALI_PHY_461_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_462_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(462 + PHY_BASE_ADDR,DENALI_PHY_462_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(462 + PHY_BASE_ADDR,DENALI_PHY_462_DATA,0);
  #else
         REGWR(462 + PHY_BASE_ADDR,DENALI_PHY_462_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_463_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(463 + PHY_BASE_ADDR,DENALI_PHY_463_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(463 + PHY_BASE_ADDR,DENALI_PHY_463_DATA,0);
  #else
         REGWR(463 + PHY_BASE_ADDR,DENALI_PHY_463_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_464_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(464 + PHY_BASE_ADDR,DENALI_PHY_464_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(464 + PHY_BASE_ADDR,DENALI_PHY_464_DATA,0);
  #else
         REGWR(464 + PHY_BASE_ADDR,DENALI_PHY_464_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_465_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(465 + PHY_BASE_ADDR,DENALI_PHY_465_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(465 + PHY_BASE_ADDR,DENALI_PHY_465_DATA,0);
  #else
         REGWR(465 + PHY_BASE_ADDR,DENALI_PHY_465_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_466_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(466 + PHY_BASE_ADDR,DENALI_PHY_466_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(466 + PHY_BASE_ADDR,DENALI_PHY_466_DATA,0);
  #else
         REGWR(466 + PHY_BASE_ADDR,DENALI_PHY_466_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_467_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(467 + PHY_BASE_ADDR,DENALI_PHY_467_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(467 + PHY_BASE_ADDR,DENALI_PHY_467_DATA,0);
  #else
         REGWR(467 + PHY_BASE_ADDR,DENALI_PHY_467_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_468_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(468 + PHY_BASE_ADDR,DENALI_PHY_468_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(468 + PHY_BASE_ADDR,DENALI_PHY_468_DATA,0);
  #else
         REGWR(468 + PHY_BASE_ADDR,DENALI_PHY_468_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_469_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(469 + PHY_BASE_ADDR,DENALI_PHY_469_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(469 + PHY_BASE_ADDR,DENALI_PHY_469_DATA,0);
  #else
         REGWR(469 + PHY_BASE_ADDR,DENALI_PHY_469_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_470_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(470 + PHY_BASE_ADDR,DENALI_PHY_470_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(470 + PHY_BASE_ADDR,DENALI_PHY_470_DATA,0);
  #else
         REGWR(470 + PHY_BASE_ADDR,DENALI_PHY_470_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_471_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(471 + PHY_BASE_ADDR,DENALI_PHY_471_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(471 + PHY_BASE_ADDR,DENALI_PHY_471_DATA,0);
  #else
         REGWR(471 + PHY_BASE_ADDR,DENALI_PHY_471_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_472_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(472 + PHY_BASE_ADDR,DENALI_PHY_472_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(472 + PHY_BASE_ADDR,DENALI_PHY_472_DATA,0);
  #else
         REGWR(472 + PHY_BASE_ADDR,DENALI_PHY_472_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_473_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(473 + PHY_BASE_ADDR,DENALI_PHY_473_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(473 + PHY_BASE_ADDR,DENALI_PHY_473_DATA,0);
  #else
         REGWR(473 + PHY_BASE_ADDR,DENALI_PHY_473_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_474_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(474 + PHY_BASE_ADDR,DENALI_PHY_474_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(474 + PHY_BASE_ADDR,DENALI_PHY_474_DATA,0);
  #else
         REGWR(474 + PHY_BASE_ADDR,DENALI_PHY_474_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_475_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(475 + PHY_BASE_ADDR,DENALI_PHY_475_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(475 + PHY_BASE_ADDR,DENALI_PHY_475_DATA,0);
  #else
         REGWR(475 + PHY_BASE_ADDR,DENALI_PHY_475_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_476_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(476 + PHY_BASE_ADDR,DENALI_PHY_476_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(476 + PHY_BASE_ADDR,DENALI_PHY_476_DATA,0);
  #else
         REGWR(476 + PHY_BASE_ADDR,DENALI_PHY_476_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_477_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(477 + PHY_BASE_ADDR,DENALI_PHY_477_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(477 + PHY_BASE_ADDR,DENALI_PHY_477_DATA,0);
  #else
         REGWR(477 + PHY_BASE_ADDR,DENALI_PHY_477_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_478_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(478 + PHY_BASE_ADDR,DENALI_PHY_478_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(478 + PHY_BASE_ADDR,DENALI_PHY_478_DATA,0);
  #else
         REGWR(478 + PHY_BASE_ADDR,DENALI_PHY_478_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_479_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(479 + PHY_BASE_ADDR,DENALI_PHY_479_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(479 + PHY_BASE_ADDR,DENALI_PHY_479_DATA,0);
  #else
         REGWR(479 + PHY_BASE_ADDR,DENALI_PHY_479_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_480_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(480 + PHY_BASE_ADDR,DENALI_PHY_480_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(480 + PHY_BASE_ADDR,DENALI_PHY_480_DATA,0);
  #else
         REGWR(480 + PHY_BASE_ADDR,DENALI_PHY_480_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_481_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(481 + PHY_BASE_ADDR,DENALI_PHY_481_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(481 + PHY_BASE_ADDR,DENALI_PHY_481_DATA,0);
  #else
         REGWR(481 + PHY_BASE_ADDR,DENALI_PHY_481_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_482_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(482 + PHY_BASE_ADDR,DENALI_PHY_482_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(482 + PHY_BASE_ADDR,DENALI_PHY_482_DATA,0);
  #else
         REGWR(482 + PHY_BASE_ADDR,DENALI_PHY_482_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_483_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(483 + PHY_BASE_ADDR,DENALI_PHY_483_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(483 + PHY_BASE_ADDR,DENALI_PHY_483_DATA,0);
  #else
         REGWR(483 + PHY_BASE_ADDR,DENALI_PHY_483_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_484_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(484 + PHY_BASE_ADDR,DENALI_PHY_484_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(484 + PHY_BASE_ADDR,DENALI_PHY_484_DATA,0);
  #else
         REGWR(484 + PHY_BASE_ADDR,DENALI_PHY_484_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_485_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(485 + PHY_BASE_ADDR,DENALI_PHY_485_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(485 + PHY_BASE_ADDR,DENALI_PHY_485_DATA,0);
  #else
         REGWR(485 + PHY_BASE_ADDR,DENALI_PHY_485_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_486_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(486 + PHY_BASE_ADDR,DENALI_PHY_486_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(486 + PHY_BASE_ADDR,DENALI_PHY_486_DATA,0);
  #else
         REGWR(486 + PHY_BASE_ADDR,DENALI_PHY_486_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_487_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(487 + PHY_BASE_ADDR,DENALI_PHY_487_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(487 + PHY_BASE_ADDR,DENALI_PHY_487_DATA,0);
  #else
         REGWR(487 + PHY_BASE_ADDR,DENALI_PHY_487_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_488_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(488 + PHY_BASE_ADDR,DENALI_PHY_488_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(488 + PHY_BASE_ADDR,DENALI_PHY_488_DATA,0);
  #else
         REGWR(488 + PHY_BASE_ADDR,DENALI_PHY_488_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_489_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(489 + PHY_BASE_ADDR,DENALI_PHY_489_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(489 + PHY_BASE_ADDR,DENALI_PHY_489_DATA,0);
  #else
         REGWR(489 + PHY_BASE_ADDR,DENALI_PHY_489_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_490_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(490 + PHY_BASE_ADDR,DENALI_PHY_490_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(490 + PHY_BASE_ADDR,DENALI_PHY_490_DATA,0);
  #else
         REGWR(490 + PHY_BASE_ADDR,DENALI_PHY_490_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_491_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(491 + PHY_BASE_ADDR,DENALI_PHY_491_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(491 + PHY_BASE_ADDR,DENALI_PHY_491_DATA,0);
  #else
         REGWR(491 + PHY_BASE_ADDR,DENALI_PHY_491_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_492_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(492 + PHY_BASE_ADDR,DENALI_PHY_492_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(492 + PHY_BASE_ADDR,DENALI_PHY_492_DATA,0);
  #else
         REGWR(492 + PHY_BASE_ADDR,DENALI_PHY_492_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_493_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(493 + PHY_BASE_ADDR,DENALI_PHY_493_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(493 + PHY_BASE_ADDR,DENALI_PHY_493_DATA,0);
  #else
         REGWR(493 + PHY_BASE_ADDR,DENALI_PHY_493_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_494_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(494 + PHY_BASE_ADDR,DENALI_PHY_494_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(494 + PHY_BASE_ADDR,DENALI_PHY_494_DATA,0);
  #else
         REGWR(494 + PHY_BASE_ADDR,DENALI_PHY_494_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_495_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(495 + PHY_BASE_ADDR,DENALI_PHY_495_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(495 + PHY_BASE_ADDR,DENALI_PHY_495_DATA,0);
  #else
         REGWR(495 + PHY_BASE_ADDR,DENALI_PHY_495_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_496_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(496 + PHY_BASE_ADDR,DENALI_PHY_496_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(496 + PHY_BASE_ADDR,DENALI_PHY_496_DATA,0);
  #else
         REGWR(496 + PHY_BASE_ADDR,DENALI_PHY_496_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_497_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(497 + PHY_BASE_ADDR,DENALI_PHY_497_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(497 + PHY_BASE_ADDR,DENALI_PHY_497_DATA,0);
  #else
         REGWR(497 + PHY_BASE_ADDR,DENALI_PHY_497_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_498_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(498 + PHY_BASE_ADDR,DENALI_PHY_498_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(498 + PHY_BASE_ADDR,DENALI_PHY_498_DATA,0);
  #else
         REGWR(498 + PHY_BASE_ADDR,DENALI_PHY_498_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_499_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(499 + PHY_BASE_ADDR,DENALI_PHY_499_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(499 + PHY_BASE_ADDR,DENALI_PHY_499_DATA,0);
  #else
         REGWR(499 + PHY_BASE_ADDR,DENALI_PHY_499_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_500_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(500 + PHY_BASE_ADDR,DENALI_PHY_500_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(500 + PHY_BASE_ADDR,DENALI_PHY_500_DATA,0);
  #else
         REGWR(500 + PHY_BASE_ADDR,DENALI_PHY_500_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_501_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(501 + PHY_BASE_ADDR,DENALI_PHY_501_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(501 + PHY_BASE_ADDR,DENALI_PHY_501_DATA,0);
  #else
         REGWR(501 + PHY_BASE_ADDR,DENALI_PHY_501_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_502_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(502 + PHY_BASE_ADDR,DENALI_PHY_502_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(502 + PHY_BASE_ADDR,DENALI_PHY_502_DATA,0);
  #else
         REGWR(502 + PHY_BASE_ADDR,DENALI_PHY_502_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_503_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(503 + PHY_BASE_ADDR,DENALI_PHY_503_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(503 + PHY_BASE_ADDR,DENALI_PHY_503_DATA,0);
  #else
         REGWR(503 + PHY_BASE_ADDR,DENALI_PHY_503_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_504_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(504 + PHY_BASE_ADDR,DENALI_PHY_504_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(504 + PHY_BASE_ADDR,DENALI_PHY_504_DATA,0);
  #else
         REGWR(504 + PHY_BASE_ADDR,DENALI_PHY_504_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_505_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(505 + PHY_BASE_ADDR,DENALI_PHY_505_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(505 + PHY_BASE_ADDR,DENALI_PHY_505_DATA,0);
  #else
         REGWR(505 + PHY_BASE_ADDR,DENALI_PHY_505_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_506_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(506 + PHY_BASE_ADDR,DENALI_PHY_506_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(506 + PHY_BASE_ADDR,DENALI_PHY_506_DATA,0);
  #else
         REGWR(506 + PHY_BASE_ADDR,DENALI_PHY_506_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_507_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(507 + PHY_BASE_ADDR,DENALI_PHY_507_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(507 + PHY_BASE_ADDR,DENALI_PHY_507_DATA,0);
  #else
         REGWR(507 + PHY_BASE_ADDR,DENALI_PHY_507_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_508_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(508 + PHY_BASE_ADDR,DENALI_PHY_508_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(508 + PHY_BASE_ADDR,DENALI_PHY_508_DATA,0);
  #else
         REGWR(508 + PHY_BASE_ADDR,DENALI_PHY_508_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_509_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(509 + PHY_BASE_ADDR,DENALI_PHY_509_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(509 + PHY_BASE_ADDR,DENALI_PHY_509_DATA,0);
  #else
         REGWR(509 + PHY_BASE_ADDR,DENALI_PHY_509_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_510_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(510 + PHY_BASE_ADDR,DENALI_PHY_510_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(510 + PHY_BASE_ADDR,DENALI_PHY_510_DATA,0);
  #else
         REGWR(510 + PHY_BASE_ADDR,DENALI_PHY_510_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_511_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(511 + PHY_BASE_ADDR,DENALI_PHY_511_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(511 + PHY_BASE_ADDR,DENALI_PHY_511_DATA,0);
  #else
         REGWR(511 + PHY_BASE_ADDR,DENALI_PHY_511_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_512_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(512 + PHY_BASE_ADDR,DENALI_PHY_512_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(512 + PHY_BASE_ADDR,DENALI_PHY_512_DATA,0);
  #else
         REGWR(512 + PHY_BASE_ADDR,DENALI_PHY_512_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_513_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(513 + PHY_BASE_ADDR,DENALI_PHY_513_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(513 + PHY_BASE_ADDR,DENALI_PHY_513_DATA,0);
  #else
         REGWR(513 + PHY_BASE_ADDR,DENALI_PHY_513_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_514_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(514 + PHY_BASE_ADDR,DENALI_PHY_514_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(514 + PHY_BASE_ADDR,DENALI_PHY_514_DATA,0);
  #else
         REGWR(514 + PHY_BASE_ADDR,DENALI_PHY_514_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_515_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(515 + PHY_BASE_ADDR,DENALI_PHY_515_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(515 + PHY_BASE_ADDR,DENALI_PHY_515_DATA,0);
  #else
         REGWR(515 + PHY_BASE_ADDR,DENALI_PHY_515_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_516_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(516 + PHY_BASE_ADDR,DENALI_PHY_516_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(516 + PHY_BASE_ADDR,DENALI_PHY_516_DATA,0);
  #else
         REGWR(516 + PHY_BASE_ADDR,DENALI_PHY_516_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_517_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(517 + PHY_BASE_ADDR,DENALI_PHY_517_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(517 + PHY_BASE_ADDR,DENALI_PHY_517_DATA,0);
  #else
         REGWR(517 + PHY_BASE_ADDR,DENALI_PHY_517_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_518_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(518 + PHY_BASE_ADDR,DENALI_PHY_518_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(518 + PHY_BASE_ADDR,DENALI_PHY_518_DATA,0);
  #else
         REGWR(518 + PHY_BASE_ADDR,DENALI_PHY_518_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_519_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(519 + PHY_BASE_ADDR,DENALI_PHY_519_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(519 + PHY_BASE_ADDR,DENALI_PHY_519_DATA,0);
  #else
         REGWR(519 + PHY_BASE_ADDR,DENALI_PHY_519_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_520_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(520 + PHY_BASE_ADDR,DENALI_PHY_520_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(520 + PHY_BASE_ADDR,DENALI_PHY_520_DATA,0);
  #else
         REGWR(520 + PHY_BASE_ADDR,DENALI_PHY_520_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_521_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(521 + PHY_BASE_ADDR,DENALI_PHY_521_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(521 + PHY_BASE_ADDR,DENALI_PHY_521_DATA,0);
  #else
         REGWR(521 + PHY_BASE_ADDR,DENALI_PHY_521_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_522_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(522 + PHY_BASE_ADDR,DENALI_PHY_522_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(522 + PHY_BASE_ADDR,DENALI_PHY_522_DATA,0);
  #else
         REGWR(522 + PHY_BASE_ADDR,DENALI_PHY_522_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_523_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(523 + PHY_BASE_ADDR,DENALI_PHY_523_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(523 + PHY_BASE_ADDR,DENALI_PHY_523_DATA,0);
  #else
         REGWR(523 + PHY_BASE_ADDR,DENALI_PHY_523_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_524_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(524 + PHY_BASE_ADDR,DENALI_PHY_524_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(524 + PHY_BASE_ADDR,DENALI_PHY_524_DATA,0);
  #else
         REGWR(524 + PHY_BASE_ADDR,DENALI_PHY_524_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_525_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(525 + PHY_BASE_ADDR,DENALI_PHY_525_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(525 + PHY_BASE_ADDR,DENALI_PHY_525_DATA,0);
  #else
         REGWR(525 + PHY_BASE_ADDR,DENALI_PHY_525_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_526_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(526 + PHY_BASE_ADDR,DENALI_PHY_526_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(526 + PHY_BASE_ADDR,DENALI_PHY_526_DATA,0);
  #else
         REGWR(526 + PHY_BASE_ADDR,DENALI_PHY_526_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_527_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(527 + PHY_BASE_ADDR,DENALI_PHY_527_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(527 + PHY_BASE_ADDR,DENALI_PHY_527_DATA,0);
  #else
         REGWR(527 + PHY_BASE_ADDR,DENALI_PHY_527_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_528_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(528 + PHY_BASE_ADDR,DENALI_PHY_528_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(528 + PHY_BASE_ADDR,DENALI_PHY_528_DATA,0);
  #else
         REGWR(528 + PHY_BASE_ADDR,DENALI_PHY_528_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_529_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(529 + PHY_BASE_ADDR,DENALI_PHY_529_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(529 + PHY_BASE_ADDR,DENALI_PHY_529_DATA,0);
  #else
         REGWR(529 + PHY_BASE_ADDR,DENALI_PHY_529_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_530_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(530 + PHY_BASE_ADDR,DENALI_PHY_530_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(530 + PHY_BASE_ADDR,DENALI_PHY_530_DATA,0);
  #else
         REGWR(530 + PHY_BASE_ADDR,DENALI_PHY_530_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_531_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(531 + PHY_BASE_ADDR,DENALI_PHY_531_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(531 + PHY_BASE_ADDR,DENALI_PHY_531_DATA,0);
  #else
         REGWR(531 + PHY_BASE_ADDR,DENALI_PHY_531_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_532_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(532 + PHY_BASE_ADDR,DENALI_PHY_532_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(532 + PHY_BASE_ADDR,DENALI_PHY_532_DATA,0);
  #else
         REGWR(532 + PHY_BASE_ADDR,DENALI_PHY_532_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_533_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(533 + PHY_BASE_ADDR,DENALI_PHY_533_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(533 + PHY_BASE_ADDR,DENALI_PHY_533_DATA,0);
  #else
         REGWR(533 + PHY_BASE_ADDR,DENALI_PHY_533_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_534_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(534 + PHY_BASE_ADDR,DENALI_PHY_534_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(534 + PHY_BASE_ADDR,DENALI_PHY_534_DATA,0);
  #else
         REGWR(534 + PHY_BASE_ADDR,DENALI_PHY_534_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_535_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(535 + PHY_BASE_ADDR,DENALI_PHY_535_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(535 + PHY_BASE_ADDR,DENALI_PHY_535_DATA,0);
  #else
         REGWR(535 + PHY_BASE_ADDR,DENALI_PHY_535_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_536_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(536 + PHY_BASE_ADDR,DENALI_PHY_536_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(536 + PHY_BASE_ADDR,DENALI_PHY_536_DATA,0);
  #else
         REGWR(536 + PHY_BASE_ADDR,DENALI_PHY_536_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_537_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(537 + PHY_BASE_ADDR,DENALI_PHY_537_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(537 + PHY_BASE_ADDR,DENALI_PHY_537_DATA,0);
  #else
         REGWR(537 + PHY_BASE_ADDR,DENALI_PHY_537_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_538_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(538 + PHY_BASE_ADDR,DENALI_PHY_538_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(538 + PHY_BASE_ADDR,DENALI_PHY_538_DATA,0);
  #else
         REGWR(538 + PHY_BASE_ADDR,DENALI_PHY_538_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_539_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(539 + PHY_BASE_ADDR,DENALI_PHY_539_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(539 + PHY_BASE_ADDR,DENALI_PHY_539_DATA,0);
  #else
         REGWR(539 + PHY_BASE_ADDR,DENALI_PHY_539_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_540_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(540 + PHY_BASE_ADDR,DENALI_PHY_540_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(540 + PHY_BASE_ADDR,DENALI_PHY_540_DATA,0);
  #else
         REGWR(540 + PHY_BASE_ADDR,DENALI_PHY_540_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_541_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(541 + PHY_BASE_ADDR,DENALI_PHY_541_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(541 + PHY_BASE_ADDR,DENALI_PHY_541_DATA,0);
  #else
         REGWR(541 + PHY_BASE_ADDR,DENALI_PHY_541_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_542_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(542 + PHY_BASE_ADDR,DENALI_PHY_542_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(542 + PHY_BASE_ADDR,DENALI_PHY_542_DATA,0);
  #else
         REGWR(542 + PHY_BASE_ADDR,DENALI_PHY_542_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_543_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(543 + PHY_BASE_ADDR,DENALI_PHY_543_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(543 + PHY_BASE_ADDR,DENALI_PHY_543_DATA,0);
  #else
         REGWR(543 + PHY_BASE_ADDR,DENALI_PHY_543_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_544_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(544 + PHY_BASE_ADDR,DENALI_PHY_544_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(544 + PHY_BASE_ADDR,DENALI_PHY_544_DATA,0);
  #else
         REGWR(544 + PHY_BASE_ADDR,DENALI_PHY_544_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_545_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(545 + PHY_BASE_ADDR,DENALI_PHY_545_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(545 + PHY_BASE_ADDR,DENALI_PHY_545_DATA,0);
  #else
         REGWR(545 + PHY_BASE_ADDR,DENALI_PHY_545_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_546_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(546 + PHY_BASE_ADDR,DENALI_PHY_546_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(546 + PHY_BASE_ADDR,DENALI_PHY_546_DATA,0);
  #else
         REGWR(546 + PHY_BASE_ADDR,DENALI_PHY_546_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_547_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(547 + PHY_BASE_ADDR,DENALI_PHY_547_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(547 + PHY_BASE_ADDR,DENALI_PHY_547_DATA,0);
  #else
         REGWR(547 + PHY_BASE_ADDR,DENALI_PHY_547_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_548_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(548 + PHY_BASE_ADDR,DENALI_PHY_548_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(548 + PHY_BASE_ADDR,DENALI_PHY_548_DATA,0);
  #else
         REGWR(548 + PHY_BASE_ADDR,DENALI_PHY_548_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_549_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(549 + PHY_BASE_ADDR,DENALI_PHY_549_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(549 + PHY_BASE_ADDR,DENALI_PHY_549_DATA,0);
  #else
         REGWR(549 + PHY_BASE_ADDR,DENALI_PHY_549_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_550_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(550 + PHY_BASE_ADDR,DENALI_PHY_550_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(550 + PHY_BASE_ADDR,DENALI_PHY_550_DATA,0);
  #else
         REGWR(550 + PHY_BASE_ADDR,DENALI_PHY_550_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_551_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(551 + PHY_BASE_ADDR,DENALI_PHY_551_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(551 + PHY_BASE_ADDR,DENALI_PHY_551_DATA,0);
  #else
         REGWR(551 + PHY_BASE_ADDR,DENALI_PHY_551_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_552_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(552 + PHY_BASE_ADDR,DENALI_PHY_552_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(552 + PHY_BASE_ADDR,DENALI_PHY_552_DATA,0);
  #else
         REGWR(552 + PHY_BASE_ADDR,DENALI_PHY_552_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_553_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(553 + PHY_BASE_ADDR,DENALI_PHY_553_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(553 + PHY_BASE_ADDR,DENALI_PHY_553_DATA,0);
  #else
         REGWR(553 + PHY_BASE_ADDR,DENALI_PHY_553_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_554_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(554 + PHY_BASE_ADDR,DENALI_PHY_554_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(554 + PHY_BASE_ADDR,DENALI_PHY_554_DATA,0);
  #else
         REGWR(554 + PHY_BASE_ADDR,DENALI_PHY_554_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_555_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(555 + PHY_BASE_ADDR,DENALI_PHY_555_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(555 + PHY_BASE_ADDR,DENALI_PHY_555_DATA,0);
  #else
         REGWR(555 + PHY_BASE_ADDR,DENALI_PHY_555_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_556_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(556 + PHY_BASE_ADDR,DENALI_PHY_556_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(556 + PHY_BASE_ADDR,DENALI_PHY_556_DATA,0);
  #else
         REGWR(556 + PHY_BASE_ADDR,DENALI_PHY_556_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_557_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(557 + PHY_BASE_ADDR,DENALI_PHY_557_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(557 + PHY_BASE_ADDR,DENALI_PHY_557_DATA,0);
  #else
         REGWR(557 + PHY_BASE_ADDR,DENALI_PHY_557_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_558_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(558 + PHY_BASE_ADDR,DENALI_PHY_558_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(558 + PHY_BASE_ADDR,DENALI_PHY_558_DATA,0);
  #else
         REGWR(558 + PHY_BASE_ADDR,DENALI_PHY_558_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_559_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(559 + PHY_BASE_ADDR,DENALI_PHY_559_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(559 + PHY_BASE_ADDR,DENALI_PHY_559_DATA,0);
  #else
         REGWR(559 + PHY_BASE_ADDR,DENALI_PHY_559_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_560_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(560 + PHY_BASE_ADDR,DENALI_PHY_560_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(560 + PHY_BASE_ADDR,DENALI_PHY_560_DATA,0);
  #else
         REGWR(560 + PHY_BASE_ADDR,DENALI_PHY_560_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_561_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(561 + PHY_BASE_ADDR,DENALI_PHY_561_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(561 + PHY_BASE_ADDR,DENALI_PHY_561_DATA,0);
  #else
         REGWR(561 + PHY_BASE_ADDR,DENALI_PHY_561_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_562_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(562 + PHY_BASE_ADDR,DENALI_PHY_562_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(562 + PHY_BASE_ADDR,DENALI_PHY_562_DATA,0);
  #else
         REGWR(562 + PHY_BASE_ADDR,DENALI_PHY_562_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_563_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(563 + PHY_BASE_ADDR,DENALI_PHY_563_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(563 + PHY_BASE_ADDR,DENALI_PHY_563_DATA,0);
  #else
         REGWR(563 + PHY_BASE_ADDR,DENALI_PHY_563_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_564_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(564 + PHY_BASE_ADDR,DENALI_PHY_564_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(564 + PHY_BASE_ADDR,DENALI_PHY_564_DATA,0);
  #else
         REGWR(564 + PHY_BASE_ADDR,DENALI_PHY_564_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_565_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(565 + PHY_BASE_ADDR,DENALI_PHY_565_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(565 + PHY_BASE_ADDR,DENALI_PHY_565_DATA,0);
  #else
         REGWR(565 + PHY_BASE_ADDR,DENALI_PHY_565_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_566_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(566 + PHY_BASE_ADDR,DENALI_PHY_566_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(566 + PHY_BASE_ADDR,DENALI_PHY_566_DATA,0);
  #else
         REGWR(566 + PHY_BASE_ADDR,DENALI_PHY_566_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_567_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(567 + PHY_BASE_ADDR,DENALI_PHY_567_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(567 + PHY_BASE_ADDR,DENALI_PHY_567_DATA,0);
  #else
         REGWR(567 + PHY_BASE_ADDR,DENALI_PHY_567_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_568_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(568 + PHY_BASE_ADDR,DENALI_PHY_568_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(568 + PHY_BASE_ADDR,DENALI_PHY_568_DATA,0);
  #else
         REGWR(568 + PHY_BASE_ADDR,DENALI_PHY_568_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_569_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(569 + PHY_BASE_ADDR,DENALI_PHY_569_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(569 + PHY_BASE_ADDR,DENALI_PHY_569_DATA,0);
  #else
         REGWR(569 + PHY_BASE_ADDR,DENALI_PHY_569_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_570_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(570 + PHY_BASE_ADDR,DENALI_PHY_570_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(570 + PHY_BASE_ADDR,DENALI_PHY_570_DATA,0);
  #else
         REGWR(570 + PHY_BASE_ADDR,DENALI_PHY_570_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_571_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(571 + PHY_BASE_ADDR,DENALI_PHY_571_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(571 + PHY_BASE_ADDR,DENALI_PHY_571_DATA,0);
  #else
         REGWR(571 + PHY_BASE_ADDR,DENALI_PHY_571_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_572_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(572 + PHY_BASE_ADDR,DENALI_PHY_572_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(572 + PHY_BASE_ADDR,DENALI_PHY_572_DATA,0);
  #else
         REGWR(572 + PHY_BASE_ADDR,DENALI_PHY_572_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_573_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(573 + PHY_BASE_ADDR,DENALI_PHY_573_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(573 + PHY_BASE_ADDR,DENALI_PHY_573_DATA,0);
  #else
         REGWR(573 + PHY_BASE_ADDR,DENALI_PHY_573_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_574_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(574 + PHY_BASE_ADDR,DENALI_PHY_574_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(574 + PHY_BASE_ADDR,DENALI_PHY_574_DATA,0);
  #else
         REGWR(574 + PHY_BASE_ADDR,DENALI_PHY_574_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_575_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(575 + PHY_BASE_ADDR,DENALI_PHY_575_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(575 + PHY_BASE_ADDR,DENALI_PHY_575_DATA,0);
  #else
         REGWR(575 + PHY_BASE_ADDR,DENALI_PHY_575_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_576_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(576 + PHY_BASE_ADDR,DENALI_PHY_576_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(576 + PHY_BASE_ADDR,DENALI_PHY_576_DATA,0);
  #else
         REGWR(576 + PHY_BASE_ADDR,DENALI_PHY_576_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_577_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(577 + PHY_BASE_ADDR,DENALI_PHY_577_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(577 + PHY_BASE_ADDR,DENALI_PHY_577_DATA,0);
  #else
         REGWR(577 + PHY_BASE_ADDR,DENALI_PHY_577_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_578_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(578 + PHY_BASE_ADDR,DENALI_PHY_578_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(578 + PHY_BASE_ADDR,DENALI_PHY_578_DATA,0);
  #else
         REGWR(578 + PHY_BASE_ADDR,DENALI_PHY_578_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_579_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(579 + PHY_BASE_ADDR,DENALI_PHY_579_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(579 + PHY_BASE_ADDR,DENALI_PHY_579_DATA,0);
  #else
         REGWR(579 + PHY_BASE_ADDR,DENALI_PHY_579_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_580_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(580 + PHY_BASE_ADDR,DENALI_PHY_580_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(580 + PHY_BASE_ADDR,DENALI_PHY_580_DATA,0);
  #else
         REGWR(580 + PHY_BASE_ADDR,DENALI_PHY_580_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_581_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(581 + PHY_BASE_ADDR,DENALI_PHY_581_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(581 + PHY_BASE_ADDR,DENALI_PHY_581_DATA,0);
  #else
         REGWR(581 + PHY_BASE_ADDR,DENALI_PHY_581_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_582_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(582 + PHY_BASE_ADDR,DENALI_PHY_582_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(582 + PHY_BASE_ADDR,DENALI_PHY_582_DATA,0);
  #else
         REGWR(582 + PHY_BASE_ADDR,DENALI_PHY_582_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_583_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(583 + PHY_BASE_ADDR,DENALI_PHY_583_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(583 + PHY_BASE_ADDR,DENALI_PHY_583_DATA,0);
  #else
         REGWR(583 + PHY_BASE_ADDR,DENALI_PHY_583_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_584_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(584 + PHY_BASE_ADDR,DENALI_PHY_584_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(584 + PHY_BASE_ADDR,DENALI_PHY_584_DATA,0);
  #else
         REGWR(584 + PHY_BASE_ADDR,DENALI_PHY_584_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_585_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(585 + PHY_BASE_ADDR,DENALI_PHY_585_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(585 + PHY_BASE_ADDR,DENALI_PHY_585_DATA,0);
  #else
         REGWR(585 + PHY_BASE_ADDR,DENALI_PHY_585_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_586_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(586 + PHY_BASE_ADDR,DENALI_PHY_586_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(586 + PHY_BASE_ADDR,DENALI_PHY_586_DATA,0);
  #else
         REGWR(586 + PHY_BASE_ADDR,DENALI_PHY_586_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_587_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(587 + PHY_BASE_ADDR,DENALI_PHY_587_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(587 + PHY_BASE_ADDR,DENALI_PHY_587_DATA,0);
  #else
         REGWR(587 + PHY_BASE_ADDR,DENALI_PHY_587_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_588_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(588 + PHY_BASE_ADDR,DENALI_PHY_588_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(588 + PHY_BASE_ADDR,DENALI_PHY_588_DATA,0);
  #else
         REGWR(588 + PHY_BASE_ADDR,DENALI_PHY_588_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_589_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(589 + PHY_BASE_ADDR,DENALI_PHY_589_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(589 + PHY_BASE_ADDR,DENALI_PHY_589_DATA,0);
  #else
         REGWR(589 + PHY_BASE_ADDR,DENALI_PHY_589_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_590_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(590 + PHY_BASE_ADDR,DENALI_PHY_590_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(590 + PHY_BASE_ADDR,DENALI_PHY_590_DATA,0);
  #else
         REGWR(590 + PHY_BASE_ADDR,DENALI_PHY_590_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_591_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(591 + PHY_BASE_ADDR,DENALI_PHY_591_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(591 + PHY_BASE_ADDR,DENALI_PHY_591_DATA,0);
  #else
         REGWR(591 + PHY_BASE_ADDR,DENALI_PHY_591_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_592_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(592 + PHY_BASE_ADDR,DENALI_PHY_592_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(592 + PHY_BASE_ADDR,DENALI_PHY_592_DATA,0);
  #else
         REGWR(592 + PHY_BASE_ADDR,DENALI_PHY_592_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_593_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(593 + PHY_BASE_ADDR,DENALI_PHY_593_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(593 + PHY_BASE_ADDR,DENALI_PHY_593_DATA,0);
  #else
         REGWR(593 + PHY_BASE_ADDR,DENALI_PHY_593_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_594_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(594 + PHY_BASE_ADDR,DENALI_PHY_594_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(594 + PHY_BASE_ADDR,DENALI_PHY_594_DATA,0);
  #else
         REGWR(594 + PHY_BASE_ADDR,DENALI_PHY_594_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_595_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(595 + PHY_BASE_ADDR,DENALI_PHY_595_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(595 + PHY_BASE_ADDR,DENALI_PHY_595_DATA,0);
  #else
         REGWR(595 + PHY_BASE_ADDR,DENALI_PHY_595_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_596_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(596 + PHY_BASE_ADDR,DENALI_PHY_596_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(596 + PHY_BASE_ADDR,DENALI_PHY_596_DATA,0);
  #else
         REGWR(596 + PHY_BASE_ADDR,DENALI_PHY_596_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_597_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(597 + PHY_BASE_ADDR,DENALI_PHY_597_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(597 + PHY_BASE_ADDR,DENALI_PHY_597_DATA,0);
  #else
         REGWR(597 + PHY_BASE_ADDR,DENALI_PHY_597_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_598_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(598 + PHY_BASE_ADDR,DENALI_PHY_598_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(598 + PHY_BASE_ADDR,DENALI_PHY_598_DATA,0);
  #else
         REGWR(598 + PHY_BASE_ADDR,DENALI_PHY_598_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_599_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(599 + PHY_BASE_ADDR,DENALI_PHY_599_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(599 + PHY_BASE_ADDR,DENALI_PHY_599_DATA,0);
  #else
         REGWR(599 + PHY_BASE_ADDR,DENALI_PHY_599_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_600_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(600 + PHY_BASE_ADDR,DENALI_PHY_600_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(600 + PHY_BASE_ADDR,DENALI_PHY_600_DATA,0);
  #else
         REGWR(600 + PHY_BASE_ADDR,DENALI_PHY_600_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_601_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(601 + PHY_BASE_ADDR,DENALI_PHY_601_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(601 + PHY_BASE_ADDR,DENALI_PHY_601_DATA,0);
  #else
         REGWR(601 + PHY_BASE_ADDR,DENALI_PHY_601_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_602_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(602 + PHY_BASE_ADDR,DENALI_PHY_602_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(602 + PHY_BASE_ADDR,DENALI_PHY_602_DATA,0);
  #else
         REGWR(602 + PHY_BASE_ADDR,DENALI_PHY_602_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_603_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(603 + PHY_BASE_ADDR,DENALI_PHY_603_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(603 + PHY_BASE_ADDR,DENALI_PHY_603_DATA,0);
  #else
         REGWR(603 + PHY_BASE_ADDR,DENALI_PHY_603_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_604_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(604 + PHY_BASE_ADDR,DENALI_PHY_604_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(604 + PHY_BASE_ADDR,DENALI_PHY_604_DATA,0);
  #else
         REGWR(604 + PHY_BASE_ADDR,DENALI_PHY_604_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_605_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(605 + PHY_BASE_ADDR,DENALI_PHY_605_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(605 + PHY_BASE_ADDR,DENALI_PHY_605_DATA,0);
  #else
         REGWR(605 + PHY_BASE_ADDR,DENALI_PHY_605_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_606_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(606 + PHY_BASE_ADDR,DENALI_PHY_606_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(606 + PHY_BASE_ADDR,DENALI_PHY_606_DATA,0);
  #else
         REGWR(606 + PHY_BASE_ADDR,DENALI_PHY_606_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_607_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(607 + PHY_BASE_ADDR,DENALI_PHY_607_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(607 + PHY_BASE_ADDR,DENALI_PHY_607_DATA,0);
  #else
         REGWR(607 + PHY_BASE_ADDR,DENALI_PHY_607_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_608_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(608 + PHY_BASE_ADDR,DENALI_PHY_608_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(608 + PHY_BASE_ADDR,DENALI_PHY_608_DATA,0);
  #else
         REGWR(608 + PHY_BASE_ADDR,DENALI_PHY_608_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_609_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(609 + PHY_BASE_ADDR,DENALI_PHY_609_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(609 + PHY_BASE_ADDR,DENALI_PHY_609_DATA,0);
  #else
         REGWR(609 + PHY_BASE_ADDR,DENALI_PHY_609_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_610_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(610 + PHY_BASE_ADDR,DENALI_PHY_610_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(610 + PHY_BASE_ADDR,DENALI_PHY_610_DATA,0);
  #else
         REGWR(610 + PHY_BASE_ADDR,DENALI_PHY_610_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_611_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(611 + PHY_BASE_ADDR,DENALI_PHY_611_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(611 + PHY_BASE_ADDR,DENALI_PHY_611_DATA,0);
  #else
         REGWR(611 + PHY_BASE_ADDR,DENALI_PHY_611_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_612_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(612 + PHY_BASE_ADDR,DENALI_PHY_612_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(612 + PHY_BASE_ADDR,DENALI_PHY_612_DATA,0);
  #else
         REGWR(612 + PHY_BASE_ADDR,DENALI_PHY_612_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_613_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(613 + PHY_BASE_ADDR,DENALI_PHY_613_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(613 + PHY_BASE_ADDR,DENALI_PHY_613_DATA,0);
  #else
         REGWR(613 + PHY_BASE_ADDR,DENALI_PHY_613_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_614_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(614 + PHY_BASE_ADDR,DENALI_PHY_614_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(614 + PHY_BASE_ADDR,DENALI_PHY_614_DATA,0);
  #else
         REGWR(614 + PHY_BASE_ADDR,DENALI_PHY_614_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_615_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(615 + PHY_BASE_ADDR,DENALI_PHY_615_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(615 + PHY_BASE_ADDR,DENALI_PHY_615_DATA,0);
  #else
         REGWR(615 + PHY_BASE_ADDR,DENALI_PHY_615_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_616_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(616 + PHY_BASE_ADDR,DENALI_PHY_616_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(616 + PHY_BASE_ADDR,DENALI_PHY_616_DATA,0);
  #else
         REGWR(616 + PHY_BASE_ADDR,DENALI_PHY_616_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_617_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(617 + PHY_BASE_ADDR,DENALI_PHY_617_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(617 + PHY_BASE_ADDR,DENALI_PHY_617_DATA,0);
  #else
         REGWR(617 + PHY_BASE_ADDR,DENALI_PHY_617_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_618_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(618 + PHY_BASE_ADDR,DENALI_PHY_618_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(618 + PHY_BASE_ADDR,DENALI_PHY_618_DATA,0);
  #else
         REGWR(618 + PHY_BASE_ADDR,DENALI_PHY_618_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_619_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(619 + PHY_BASE_ADDR,DENALI_PHY_619_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(619 + PHY_BASE_ADDR,DENALI_PHY_619_DATA,0);
  #else
         REGWR(619 + PHY_BASE_ADDR,DENALI_PHY_619_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_620_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(620 + PHY_BASE_ADDR,DENALI_PHY_620_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(620 + PHY_BASE_ADDR,DENALI_PHY_620_DATA,0);
  #else
         REGWR(620 + PHY_BASE_ADDR,DENALI_PHY_620_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_621_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(621 + PHY_BASE_ADDR,DENALI_PHY_621_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(621 + PHY_BASE_ADDR,DENALI_PHY_621_DATA,0);
  #else
         REGWR(621 + PHY_BASE_ADDR,DENALI_PHY_621_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_622_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(622 + PHY_BASE_ADDR,DENALI_PHY_622_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(622 + PHY_BASE_ADDR,DENALI_PHY_622_DATA,0);
  #else
         REGWR(622 + PHY_BASE_ADDR,DENALI_PHY_622_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_623_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(623 + PHY_BASE_ADDR,DENALI_PHY_623_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(623 + PHY_BASE_ADDR,DENALI_PHY_623_DATA,0);
  #else
         REGWR(623 + PHY_BASE_ADDR,DENALI_PHY_623_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_624_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(624 + PHY_BASE_ADDR,DENALI_PHY_624_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(624 + PHY_BASE_ADDR,DENALI_PHY_624_DATA,0);
  #else
         REGWR(624 + PHY_BASE_ADDR,DENALI_PHY_624_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_625_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(625 + PHY_BASE_ADDR,DENALI_PHY_625_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(625 + PHY_BASE_ADDR,DENALI_PHY_625_DATA,0);
  #else
         REGWR(625 + PHY_BASE_ADDR,DENALI_PHY_625_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_626_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(626 + PHY_BASE_ADDR,DENALI_PHY_626_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(626 + PHY_BASE_ADDR,DENALI_PHY_626_DATA,0);
  #else
         REGWR(626 + PHY_BASE_ADDR,DENALI_PHY_626_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_627_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(627 + PHY_BASE_ADDR,DENALI_PHY_627_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(627 + PHY_BASE_ADDR,DENALI_PHY_627_DATA,0);
  #else
         REGWR(627 + PHY_BASE_ADDR,DENALI_PHY_627_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_628_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(628 + PHY_BASE_ADDR,DENALI_PHY_628_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(628 + PHY_BASE_ADDR,DENALI_PHY_628_DATA,0);
  #else
         REGWR(628 + PHY_BASE_ADDR,DENALI_PHY_628_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_629_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(629 + PHY_BASE_ADDR,DENALI_PHY_629_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(629 + PHY_BASE_ADDR,DENALI_PHY_629_DATA,0);
  #else
         REGWR(629 + PHY_BASE_ADDR,DENALI_PHY_629_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_630_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(630 + PHY_BASE_ADDR,DENALI_PHY_630_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(630 + PHY_BASE_ADDR,DENALI_PHY_630_DATA,0);
  #else
         REGWR(630 + PHY_BASE_ADDR,DENALI_PHY_630_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_631_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(631 + PHY_BASE_ADDR,DENALI_PHY_631_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(631 + PHY_BASE_ADDR,DENALI_PHY_631_DATA,0);
  #else
         REGWR(631 + PHY_BASE_ADDR,DENALI_PHY_631_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_632_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(632 + PHY_BASE_ADDR,DENALI_PHY_632_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(632 + PHY_BASE_ADDR,DENALI_PHY_632_DATA,0);
  #else
         REGWR(632 + PHY_BASE_ADDR,DENALI_PHY_632_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_633_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(633 + PHY_BASE_ADDR,DENALI_PHY_633_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(633 + PHY_BASE_ADDR,DENALI_PHY_633_DATA,0);
  #else
         REGWR(633 + PHY_BASE_ADDR,DENALI_PHY_633_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_634_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(634 + PHY_BASE_ADDR,DENALI_PHY_634_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(634 + PHY_BASE_ADDR,DENALI_PHY_634_DATA,0);
  #else
         REGWR(634 + PHY_BASE_ADDR,DENALI_PHY_634_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_635_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(635 + PHY_BASE_ADDR,DENALI_PHY_635_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(635 + PHY_BASE_ADDR,DENALI_PHY_635_DATA,0);
  #else
         REGWR(635 + PHY_BASE_ADDR,DENALI_PHY_635_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_636_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(636 + PHY_BASE_ADDR,DENALI_PHY_636_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(636 + PHY_BASE_ADDR,DENALI_PHY_636_DATA,0);
  #else
         REGWR(636 + PHY_BASE_ADDR,DENALI_PHY_636_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_637_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(637 + PHY_BASE_ADDR,DENALI_PHY_637_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(637 + PHY_BASE_ADDR,DENALI_PHY_637_DATA,0);
  #else
         REGWR(637 + PHY_BASE_ADDR,DENALI_PHY_637_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_638_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(638 + PHY_BASE_ADDR,DENALI_PHY_638_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(638 + PHY_BASE_ADDR,DENALI_PHY_638_DATA,0);
  #else
         REGWR(638 + PHY_BASE_ADDR,DENALI_PHY_638_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_639_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(639 + PHY_BASE_ADDR,DENALI_PHY_639_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(639 + PHY_BASE_ADDR,DENALI_PHY_639_DATA,0);
  #else
         REGWR(639 + PHY_BASE_ADDR,DENALI_PHY_639_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_640_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(640 + PHY_BASE_ADDR,DENALI_PHY_640_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(640 + PHY_BASE_ADDR,DENALI_PHY_640_DATA,0);
  #else
         REGWR(640 + PHY_BASE_ADDR,DENALI_PHY_640_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_641_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(641 + PHY_BASE_ADDR,DENALI_PHY_641_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(641 + PHY_BASE_ADDR,DENALI_PHY_641_DATA,0);
  #else
         REGWR(641 + PHY_BASE_ADDR,DENALI_PHY_641_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_642_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(642 + PHY_BASE_ADDR,DENALI_PHY_642_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(642 + PHY_BASE_ADDR,DENALI_PHY_642_DATA,0);
  #else
         REGWR(642 + PHY_BASE_ADDR,DENALI_PHY_642_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_643_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(643 + PHY_BASE_ADDR,DENALI_PHY_643_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(643 + PHY_BASE_ADDR,DENALI_PHY_643_DATA,0);
  #else
         REGWR(643 + PHY_BASE_ADDR,DENALI_PHY_643_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_644_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(644 + PHY_BASE_ADDR,DENALI_PHY_644_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(644 + PHY_BASE_ADDR,DENALI_PHY_644_DATA,0);
  #else
         REGWR(644 + PHY_BASE_ADDR,DENALI_PHY_644_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_645_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(645 + PHY_BASE_ADDR,DENALI_PHY_645_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(645 + PHY_BASE_ADDR,DENALI_PHY_645_DATA,0);
  #else
         REGWR(645 + PHY_BASE_ADDR,DENALI_PHY_645_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_646_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(646 + PHY_BASE_ADDR,DENALI_PHY_646_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(646 + PHY_BASE_ADDR,DENALI_PHY_646_DATA,0);
  #else
         REGWR(646 + PHY_BASE_ADDR,DENALI_PHY_646_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_647_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(647 + PHY_BASE_ADDR,DENALI_PHY_647_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(647 + PHY_BASE_ADDR,DENALI_PHY_647_DATA,0);
  #else
         REGWR(647 + PHY_BASE_ADDR,DENALI_PHY_647_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_648_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(648 + PHY_BASE_ADDR,DENALI_PHY_648_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(648 + PHY_BASE_ADDR,DENALI_PHY_648_DATA,0);
  #else
         REGWR(648 + PHY_BASE_ADDR,DENALI_PHY_648_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_649_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(649 + PHY_BASE_ADDR,DENALI_PHY_649_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(649 + PHY_BASE_ADDR,DENALI_PHY_649_DATA,0);
  #else
         REGWR(649 + PHY_BASE_ADDR,DENALI_PHY_649_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_650_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(650 + PHY_BASE_ADDR,DENALI_PHY_650_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(650 + PHY_BASE_ADDR,DENALI_PHY_650_DATA,0);
  #else
         REGWR(650 + PHY_BASE_ADDR,DENALI_PHY_650_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_651_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(651 + PHY_BASE_ADDR,DENALI_PHY_651_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(651 + PHY_BASE_ADDR,DENALI_PHY_651_DATA,0);
  #else
         REGWR(651 + PHY_BASE_ADDR,DENALI_PHY_651_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_652_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(652 + PHY_BASE_ADDR,DENALI_PHY_652_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(652 + PHY_BASE_ADDR,DENALI_PHY_652_DATA,0);
  #else
         REGWR(652 + PHY_BASE_ADDR,DENALI_PHY_652_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_653_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(653 + PHY_BASE_ADDR,DENALI_PHY_653_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(653 + PHY_BASE_ADDR,DENALI_PHY_653_DATA,0);
  #else
         REGWR(653 + PHY_BASE_ADDR,DENALI_PHY_653_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_654_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(654 + PHY_BASE_ADDR,DENALI_PHY_654_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(654 + PHY_BASE_ADDR,DENALI_PHY_654_DATA,0);
  #else
         REGWR(654 + PHY_BASE_ADDR,DENALI_PHY_654_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_655_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(655 + PHY_BASE_ADDR,DENALI_PHY_655_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(655 + PHY_BASE_ADDR,DENALI_PHY_655_DATA,0);
  #else
         REGWR(655 + PHY_BASE_ADDR,DENALI_PHY_655_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_656_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(656 + PHY_BASE_ADDR,DENALI_PHY_656_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(656 + PHY_BASE_ADDR,DENALI_PHY_656_DATA,0);
  #else
         REGWR(656 + PHY_BASE_ADDR,DENALI_PHY_656_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_657_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(657 + PHY_BASE_ADDR,DENALI_PHY_657_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(657 + PHY_BASE_ADDR,DENALI_PHY_657_DATA,0);
  #else
         REGWR(657 + PHY_BASE_ADDR,DENALI_PHY_657_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_658_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(658 + PHY_BASE_ADDR,DENALI_PHY_658_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(658 + PHY_BASE_ADDR,DENALI_PHY_658_DATA,0);
  #else
         REGWR(658 + PHY_BASE_ADDR,DENALI_PHY_658_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_659_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(659 + PHY_BASE_ADDR,DENALI_PHY_659_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(659 + PHY_BASE_ADDR,DENALI_PHY_659_DATA,0);
  #else
         REGWR(659 + PHY_BASE_ADDR,DENALI_PHY_659_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_660_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(660 + PHY_BASE_ADDR,DENALI_PHY_660_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(660 + PHY_BASE_ADDR,DENALI_PHY_660_DATA,0);
  #else
         REGWR(660 + PHY_BASE_ADDR,DENALI_PHY_660_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_661_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(661 + PHY_BASE_ADDR,DENALI_PHY_661_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(661 + PHY_BASE_ADDR,DENALI_PHY_661_DATA,0);
  #else
         REGWR(661 + PHY_BASE_ADDR,DENALI_PHY_661_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_662_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(662 + PHY_BASE_ADDR,DENALI_PHY_662_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(662 + PHY_BASE_ADDR,DENALI_PHY_662_DATA,0);
  #else
         REGWR(662 + PHY_BASE_ADDR,DENALI_PHY_662_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_663_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(663 + PHY_BASE_ADDR,DENALI_PHY_663_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(663 + PHY_BASE_ADDR,DENALI_PHY_663_DATA,0);
  #else
         REGWR(663 + PHY_BASE_ADDR,DENALI_PHY_663_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_664_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(664 + PHY_BASE_ADDR,DENALI_PHY_664_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(664 + PHY_BASE_ADDR,DENALI_PHY_664_DATA,0);
  #else
         REGWR(664 + PHY_BASE_ADDR,DENALI_PHY_664_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_665_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(665 + PHY_BASE_ADDR,DENALI_PHY_665_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(665 + PHY_BASE_ADDR,DENALI_PHY_665_DATA,0);
  #else
         REGWR(665 + PHY_BASE_ADDR,DENALI_PHY_665_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_666_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(666 + PHY_BASE_ADDR,DENALI_PHY_666_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(666 + PHY_BASE_ADDR,DENALI_PHY_666_DATA,0);
  #else
         REGWR(666 + PHY_BASE_ADDR,DENALI_PHY_666_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_667_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(667 + PHY_BASE_ADDR,DENALI_PHY_667_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(667 + PHY_BASE_ADDR,DENALI_PHY_667_DATA,0);
  #else
         REGWR(667 + PHY_BASE_ADDR,DENALI_PHY_667_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_668_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(668 + PHY_BASE_ADDR,DENALI_PHY_668_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(668 + PHY_BASE_ADDR,DENALI_PHY_668_DATA,0);
  #else
         REGWR(668 + PHY_BASE_ADDR,DENALI_PHY_668_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_669_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(669 + PHY_BASE_ADDR,DENALI_PHY_669_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(669 + PHY_BASE_ADDR,DENALI_PHY_669_DATA,0);
  #else
         REGWR(669 + PHY_BASE_ADDR,DENALI_PHY_669_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_670_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(670 + PHY_BASE_ADDR,DENALI_PHY_670_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(670 + PHY_BASE_ADDR,DENALI_PHY_670_DATA,0);
  #else
         REGWR(670 + PHY_BASE_ADDR,DENALI_PHY_670_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_671_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(671 + PHY_BASE_ADDR,DENALI_PHY_671_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(671 + PHY_BASE_ADDR,DENALI_PHY_671_DATA,0);
  #else
         REGWR(671 + PHY_BASE_ADDR,DENALI_PHY_671_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_672_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(672 + PHY_BASE_ADDR,DENALI_PHY_672_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(672 + PHY_BASE_ADDR,DENALI_PHY_672_DATA,0);
  #else
         REGWR(672 + PHY_BASE_ADDR,DENALI_PHY_672_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_673_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(673 + PHY_BASE_ADDR,DENALI_PHY_673_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(673 + PHY_BASE_ADDR,DENALI_PHY_673_DATA,0);
  #else
         REGWR(673 + PHY_BASE_ADDR,DENALI_PHY_673_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_674_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(674 + PHY_BASE_ADDR,DENALI_PHY_674_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(674 + PHY_BASE_ADDR,DENALI_PHY_674_DATA,0);
  #else
         REGWR(674 + PHY_BASE_ADDR,DENALI_PHY_674_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_675_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(675 + PHY_BASE_ADDR,DENALI_PHY_675_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(675 + PHY_BASE_ADDR,DENALI_PHY_675_DATA,0);
  #else
         REGWR(675 + PHY_BASE_ADDR,DENALI_PHY_675_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_676_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(676 + PHY_BASE_ADDR,DENALI_PHY_676_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(676 + PHY_BASE_ADDR,DENALI_PHY_676_DATA,0);
  #else
         REGWR(676 + PHY_BASE_ADDR,DENALI_PHY_676_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_677_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(677 + PHY_BASE_ADDR,DENALI_PHY_677_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(677 + PHY_BASE_ADDR,DENALI_PHY_677_DATA,0);
  #else
         REGWR(677 + PHY_BASE_ADDR,DENALI_PHY_677_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_678_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(678 + PHY_BASE_ADDR,DENALI_PHY_678_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(678 + PHY_BASE_ADDR,DENALI_PHY_678_DATA,0);
  #else
         REGWR(678 + PHY_BASE_ADDR,DENALI_PHY_678_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_679_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(679 + PHY_BASE_ADDR,DENALI_PHY_679_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(679 + PHY_BASE_ADDR,DENALI_PHY_679_DATA,0);
  #else
         REGWR(679 + PHY_BASE_ADDR,DENALI_PHY_679_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_680_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(680 + PHY_BASE_ADDR,DENALI_PHY_680_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(680 + PHY_BASE_ADDR,DENALI_PHY_680_DATA,0);
  #else
         REGWR(680 + PHY_BASE_ADDR,DENALI_PHY_680_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_681_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(681 + PHY_BASE_ADDR,DENALI_PHY_681_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(681 + PHY_BASE_ADDR,DENALI_PHY_681_DATA,0);
  #else
         REGWR(681 + PHY_BASE_ADDR,DENALI_PHY_681_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_682_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(682 + PHY_BASE_ADDR,DENALI_PHY_682_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(682 + PHY_BASE_ADDR,DENALI_PHY_682_DATA,0);
  #else
         REGWR(682 + PHY_BASE_ADDR,DENALI_PHY_682_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_683_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(683 + PHY_BASE_ADDR,DENALI_PHY_683_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(683 + PHY_BASE_ADDR,DENALI_PHY_683_DATA,0);
  #else
         REGWR(683 + PHY_BASE_ADDR,DENALI_PHY_683_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_684_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(684 + PHY_BASE_ADDR,DENALI_PHY_684_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(684 + PHY_BASE_ADDR,DENALI_PHY_684_DATA,0);
  #else
         REGWR(684 + PHY_BASE_ADDR,DENALI_PHY_684_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_685_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(685 + PHY_BASE_ADDR,DENALI_PHY_685_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(685 + PHY_BASE_ADDR,DENALI_PHY_685_DATA,0);
  #else
         REGWR(685 + PHY_BASE_ADDR,DENALI_PHY_685_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_686_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(686 + PHY_BASE_ADDR,DENALI_PHY_686_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(686 + PHY_BASE_ADDR,DENALI_PHY_686_DATA,0);
  #else
         REGWR(686 + PHY_BASE_ADDR,DENALI_PHY_686_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_687_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(687 + PHY_BASE_ADDR,DENALI_PHY_687_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(687 + PHY_BASE_ADDR,DENALI_PHY_687_DATA,0);
  #else
         REGWR(687 + PHY_BASE_ADDR,DENALI_PHY_687_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_688_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(688 + PHY_BASE_ADDR,DENALI_PHY_688_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(688 + PHY_BASE_ADDR,DENALI_PHY_688_DATA,0);
  #else
         REGWR(688 + PHY_BASE_ADDR,DENALI_PHY_688_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_689_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(689 + PHY_BASE_ADDR,DENALI_PHY_689_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(689 + PHY_BASE_ADDR,DENALI_PHY_689_DATA,0);
  #else
         REGWR(689 + PHY_BASE_ADDR,DENALI_PHY_689_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_690_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(690 + PHY_BASE_ADDR,DENALI_PHY_690_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(690 + PHY_BASE_ADDR,DENALI_PHY_690_DATA,0);
  #else
         REGWR(690 + PHY_BASE_ADDR,DENALI_PHY_690_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_691_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(691 + PHY_BASE_ADDR,DENALI_PHY_691_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(691 + PHY_BASE_ADDR,DENALI_PHY_691_DATA,0);
  #else
         REGWR(691 + PHY_BASE_ADDR,DENALI_PHY_691_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_692_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(692 + PHY_BASE_ADDR,DENALI_PHY_692_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(692 + PHY_BASE_ADDR,DENALI_PHY_692_DATA,0);
  #else
         REGWR(692 + PHY_BASE_ADDR,DENALI_PHY_692_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_693_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(693 + PHY_BASE_ADDR,DENALI_PHY_693_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(693 + PHY_BASE_ADDR,DENALI_PHY_693_DATA,0);
  #else
         REGWR(693 + PHY_BASE_ADDR,DENALI_PHY_693_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_694_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(694 + PHY_BASE_ADDR,DENALI_PHY_694_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(694 + PHY_BASE_ADDR,DENALI_PHY_694_DATA,0);
  #else
         REGWR(694 + PHY_BASE_ADDR,DENALI_PHY_694_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_695_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(695 + PHY_BASE_ADDR,DENALI_PHY_695_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(695 + PHY_BASE_ADDR,DENALI_PHY_695_DATA,0);
  #else
         REGWR(695 + PHY_BASE_ADDR,DENALI_PHY_695_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_696_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(696 + PHY_BASE_ADDR,DENALI_PHY_696_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(696 + PHY_BASE_ADDR,DENALI_PHY_696_DATA,0);
  #else
         REGWR(696 + PHY_BASE_ADDR,DENALI_PHY_696_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_697_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(697 + PHY_BASE_ADDR,DENALI_PHY_697_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(697 + PHY_BASE_ADDR,DENALI_PHY_697_DATA,0);
  #else
         REGWR(697 + PHY_BASE_ADDR,DENALI_PHY_697_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_698_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(698 + PHY_BASE_ADDR,DENALI_PHY_698_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(698 + PHY_BASE_ADDR,DENALI_PHY_698_DATA,0);
  #else
         REGWR(698 + PHY_BASE_ADDR,DENALI_PHY_698_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_699_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(699 + PHY_BASE_ADDR,DENALI_PHY_699_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(699 + PHY_BASE_ADDR,DENALI_PHY_699_DATA,0);
  #else
         REGWR(699 + PHY_BASE_ADDR,DENALI_PHY_699_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_700_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(700 + PHY_BASE_ADDR,DENALI_PHY_700_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(700 + PHY_BASE_ADDR,DENALI_PHY_700_DATA,0);
  #else
         REGWR(700 + PHY_BASE_ADDR,DENALI_PHY_700_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_701_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(701 + PHY_BASE_ADDR,DENALI_PHY_701_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(701 + PHY_BASE_ADDR,DENALI_PHY_701_DATA,0);
  #else
         REGWR(701 + PHY_BASE_ADDR,DENALI_PHY_701_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_702_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(702 + PHY_BASE_ADDR,DENALI_PHY_702_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(702 + PHY_BASE_ADDR,DENALI_PHY_702_DATA,0);
  #else
         REGWR(702 + PHY_BASE_ADDR,DENALI_PHY_702_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_703_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(703 + PHY_BASE_ADDR,DENALI_PHY_703_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(703 + PHY_BASE_ADDR,DENALI_PHY_703_DATA,0);
  #else
         REGWR(703 + PHY_BASE_ADDR,DENALI_PHY_703_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_704_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(704 + PHY_BASE_ADDR,DENALI_PHY_704_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(704 + PHY_BASE_ADDR,DENALI_PHY_704_DATA,0);
  #else
         REGWR(704 + PHY_BASE_ADDR,DENALI_PHY_704_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_705_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(705 + PHY_BASE_ADDR,DENALI_PHY_705_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(705 + PHY_BASE_ADDR,DENALI_PHY_705_DATA,0);
  #else
         REGWR(705 + PHY_BASE_ADDR,DENALI_PHY_705_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_706_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(706 + PHY_BASE_ADDR,DENALI_PHY_706_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(706 + PHY_BASE_ADDR,DENALI_PHY_706_DATA,0);
  #else
         REGWR(706 + PHY_BASE_ADDR,DENALI_PHY_706_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_707_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(707 + PHY_BASE_ADDR,DENALI_PHY_707_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(707 + PHY_BASE_ADDR,DENALI_PHY_707_DATA,0);
  #else
         REGWR(707 + PHY_BASE_ADDR,DENALI_PHY_707_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_708_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(708 + PHY_BASE_ADDR,DENALI_PHY_708_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(708 + PHY_BASE_ADDR,DENALI_PHY_708_DATA,0);
  #else
         REGWR(708 + PHY_BASE_ADDR,DENALI_PHY_708_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_709_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(709 + PHY_BASE_ADDR,DENALI_PHY_709_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(709 + PHY_BASE_ADDR,DENALI_PHY_709_DATA,0);
  #else
         REGWR(709 + PHY_BASE_ADDR,DENALI_PHY_709_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_710_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(710 + PHY_BASE_ADDR,DENALI_PHY_710_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(710 + PHY_BASE_ADDR,DENALI_PHY_710_DATA,0);
  #else
         REGWR(710 + PHY_BASE_ADDR,DENALI_PHY_710_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_711_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(711 + PHY_BASE_ADDR,DENALI_PHY_711_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(711 + PHY_BASE_ADDR,DENALI_PHY_711_DATA,0);
  #else
         REGWR(711 + PHY_BASE_ADDR,DENALI_PHY_711_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_712_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(712 + PHY_BASE_ADDR,DENALI_PHY_712_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(712 + PHY_BASE_ADDR,DENALI_PHY_712_DATA,0);
  #else
         REGWR(712 + PHY_BASE_ADDR,DENALI_PHY_712_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_713_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(713 + PHY_BASE_ADDR,DENALI_PHY_713_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(713 + PHY_BASE_ADDR,DENALI_PHY_713_DATA,0);
  #else
         REGWR(713 + PHY_BASE_ADDR,DENALI_PHY_713_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_714_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(714 + PHY_BASE_ADDR,DENALI_PHY_714_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(714 + PHY_BASE_ADDR,DENALI_PHY_714_DATA,0);
  #else
         REGWR(714 + PHY_BASE_ADDR,DENALI_PHY_714_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_715_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(715 + PHY_BASE_ADDR,DENALI_PHY_715_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(715 + PHY_BASE_ADDR,DENALI_PHY_715_DATA,0);
  #else
         REGWR(715 + PHY_BASE_ADDR,DENALI_PHY_715_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_716_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(716 + PHY_BASE_ADDR,DENALI_PHY_716_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(716 + PHY_BASE_ADDR,DENALI_PHY_716_DATA,0);
  #else
         REGWR(716 + PHY_BASE_ADDR,DENALI_PHY_716_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_717_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(717 + PHY_BASE_ADDR,DENALI_PHY_717_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(717 + PHY_BASE_ADDR,DENALI_PHY_717_DATA,0);
  #else
         REGWR(717 + PHY_BASE_ADDR,DENALI_PHY_717_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_718_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(718 + PHY_BASE_ADDR,DENALI_PHY_718_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(718 + PHY_BASE_ADDR,DENALI_PHY_718_DATA,0);
  #else
         REGWR(718 + PHY_BASE_ADDR,DENALI_PHY_718_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_719_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(719 + PHY_BASE_ADDR,DENALI_PHY_719_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(719 + PHY_BASE_ADDR,DENALI_PHY_719_DATA,0);
  #else
         REGWR(719 + PHY_BASE_ADDR,DENALI_PHY_719_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_720_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(720 + PHY_BASE_ADDR,DENALI_PHY_720_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(720 + PHY_BASE_ADDR,DENALI_PHY_720_DATA,0);
  #else
         REGWR(720 + PHY_BASE_ADDR,DENALI_PHY_720_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_721_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(721 + PHY_BASE_ADDR,DENALI_PHY_721_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(721 + PHY_BASE_ADDR,DENALI_PHY_721_DATA,0);
  #else
         REGWR(721 + PHY_BASE_ADDR,DENALI_PHY_721_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_722_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(722 + PHY_BASE_ADDR,DENALI_PHY_722_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(722 + PHY_BASE_ADDR,DENALI_PHY_722_DATA,0);
  #else
         REGWR(722 + PHY_BASE_ADDR,DENALI_PHY_722_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_723_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(723 + PHY_BASE_ADDR,DENALI_PHY_723_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(723 + PHY_BASE_ADDR,DENALI_PHY_723_DATA,0);
  #else
         REGWR(723 + PHY_BASE_ADDR,DENALI_PHY_723_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_724_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(724 + PHY_BASE_ADDR,DENALI_PHY_724_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(724 + PHY_BASE_ADDR,DENALI_PHY_724_DATA,0);
  #else
         REGWR(724 + PHY_BASE_ADDR,DENALI_PHY_724_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_725_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(725 + PHY_BASE_ADDR,DENALI_PHY_725_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(725 + PHY_BASE_ADDR,DENALI_PHY_725_DATA,0);
  #else
         REGWR(725 + PHY_BASE_ADDR,DENALI_PHY_725_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_726_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(726 + PHY_BASE_ADDR,DENALI_PHY_726_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(726 + PHY_BASE_ADDR,DENALI_PHY_726_DATA,0);
  #else
         REGWR(726 + PHY_BASE_ADDR,DENALI_PHY_726_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_727_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(727 + PHY_BASE_ADDR,DENALI_PHY_727_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(727 + PHY_BASE_ADDR,DENALI_PHY_727_DATA,0);
  #else
         REGWR(727 + PHY_BASE_ADDR,DENALI_PHY_727_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_728_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(728 + PHY_BASE_ADDR,DENALI_PHY_728_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(728 + PHY_BASE_ADDR,DENALI_PHY_728_DATA,0);
  #else
         REGWR(728 + PHY_BASE_ADDR,DENALI_PHY_728_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_729_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(729 + PHY_BASE_ADDR,DENALI_PHY_729_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(729 + PHY_BASE_ADDR,DENALI_PHY_729_DATA,0);
  #else
         REGWR(729 + PHY_BASE_ADDR,DENALI_PHY_729_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_730_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(730 + PHY_BASE_ADDR,DENALI_PHY_730_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(730 + PHY_BASE_ADDR,DENALI_PHY_730_DATA,0);
  #else
         REGWR(730 + PHY_BASE_ADDR,DENALI_PHY_730_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_731_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(731 + PHY_BASE_ADDR,DENALI_PHY_731_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(731 + PHY_BASE_ADDR,DENALI_PHY_731_DATA,0);
  #else
         REGWR(731 + PHY_BASE_ADDR,DENALI_PHY_731_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_732_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(732 + PHY_BASE_ADDR,DENALI_PHY_732_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(732 + PHY_BASE_ADDR,DENALI_PHY_732_DATA,0);
  #else
         REGWR(732 + PHY_BASE_ADDR,DENALI_PHY_732_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_733_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(733 + PHY_BASE_ADDR,DENALI_PHY_733_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(733 + PHY_BASE_ADDR,DENALI_PHY_733_DATA,0);
  #else
         REGWR(733 + PHY_BASE_ADDR,DENALI_PHY_733_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_734_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(734 + PHY_BASE_ADDR,DENALI_PHY_734_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(734 + PHY_BASE_ADDR,DENALI_PHY_734_DATA,0);
  #else
         REGWR(734 + PHY_BASE_ADDR,DENALI_PHY_734_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_735_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(735 + PHY_BASE_ADDR,DENALI_PHY_735_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(735 + PHY_BASE_ADDR,DENALI_PHY_735_DATA,0);
  #else
         REGWR(735 + PHY_BASE_ADDR,DENALI_PHY_735_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_736_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(736 + PHY_BASE_ADDR,DENALI_PHY_736_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(736 + PHY_BASE_ADDR,DENALI_PHY_736_DATA,0);
  #else
         REGWR(736 + PHY_BASE_ADDR,DENALI_PHY_736_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_737_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(737 + PHY_BASE_ADDR,DENALI_PHY_737_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(737 + PHY_BASE_ADDR,DENALI_PHY_737_DATA,0);
  #else
         REGWR(737 + PHY_BASE_ADDR,DENALI_PHY_737_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_738_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(738 + PHY_BASE_ADDR,DENALI_PHY_738_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(738 + PHY_BASE_ADDR,DENALI_PHY_738_DATA,0);
  #else
         REGWR(738 + PHY_BASE_ADDR,DENALI_PHY_738_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_739_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(739 + PHY_BASE_ADDR,DENALI_PHY_739_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(739 + PHY_BASE_ADDR,DENALI_PHY_739_DATA,0);
  #else
         REGWR(739 + PHY_BASE_ADDR,DENALI_PHY_739_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_740_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(740 + PHY_BASE_ADDR,DENALI_PHY_740_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(740 + PHY_BASE_ADDR,DENALI_PHY_740_DATA,0);
  #else
         REGWR(740 + PHY_BASE_ADDR,DENALI_PHY_740_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_741_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(741 + PHY_BASE_ADDR,DENALI_PHY_741_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(741 + PHY_BASE_ADDR,DENALI_PHY_741_DATA,0);
  #else
         REGWR(741 + PHY_BASE_ADDR,DENALI_PHY_741_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_742_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(742 + PHY_BASE_ADDR,DENALI_PHY_742_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(742 + PHY_BASE_ADDR,DENALI_PHY_742_DATA,0);
  #else
         REGWR(742 + PHY_BASE_ADDR,DENALI_PHY_742_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_743_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(743 + PHY_BASE_ADDR,DENALI_PHY_743_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(743 + PHY_BASE_ADDR,DENALI_PHY_743_DATA,0);
  #else
         REGWR(743 + PHY_BASE_ADDR,DENALI_PHY_743_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_744_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(744 + PHY_BASE_ADDR,DENALI_PHY_744_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(744 + PHY_BASE_ADDR,DENALI_PHY_744_DATA,0);
  #else
         REGWR(744 + PHY_BASE_ADDR,DENALI_PHY_744_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_745_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(745 + PHY_BASE_ADDR,DENALI_PHY_745_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(745 + PHY_BASE_ADDR,DENALI_PHY_745_DATA,0);
  #else
         REGWR(745 + PHY_BASE_ADDR,DENALI_PHY_745_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_746_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(746 + PHY_BASE_ADDR,DENALI_PHY_746_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(746 + PHY_BASE_ADDR,DENALI_PHY_746_DATA,0);
  #else
         REGWR(746 + PHY_BASE_ADDR,DENALI_PHY_746_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_747_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(747 + PHY_BASE_ADDR,DENALI_PHY_747_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(747 + PHY_BASE_ADDR,DENALI_PHY_747_DATA,0);
  #else
         REGWR(747 + PHY_BASE_ADDR,DENALI_PHY_747_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_748_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(748 + PHY_BASE_ADDR,DENALI_PHY_748_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(748 + PHY_BASE_ADDR,DENALI_PHY_748_DATA,0);
  #else
         REGWR(748 + PHY_BASE_ADDR,DENALI_PHY_748_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_749_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(749 + PHY_BASE_ADDR,DENALI_PHY_749_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(749 + PHY_BASE_ADDR,DENALI_PHY_749_DATA,0);
  #else
         REGWR(749 + PHY_BASE_ADDR,DENALI_PHY_749_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_750_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(750 + PHY_BASE_ADDR,DENALI_PHY_750_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(750 + PHY_BASE_ADDR,DENALI_PHY_750_DATA,0);
  #else
         REGWR(750 + PHY_BASE_ADDR,DENALI_PHY_750_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_751_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(751 + PHY_BASE_ADDR,DENALI_PHY_751_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(751 + PHY_BASE_ADDR,DENALI_PHY_751_DATA,0);
  #else
         REGWR(751 + PHY_BASE_ADDR,DENALI_PHY_751_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_752_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(752 + PHY_BASE_ADDR,DENALI_PHY_752_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(752 + PHY_BASE_ADDR,DENALI_PHY_752_DATA,0);
  #else
         REGWR(752 + PHY_BASE_ADDR,DENALI_PHY_752_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_753_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(753 + PHY_BASE_ADDR,DENALI_PHY_753_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(753 + PHY_BASE_ADDR,DENALI_PHY_753_DATA,0);
  #else
         REGWR(753 + PHY_BASE_ADDR,DENALI_PHY_753_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_754_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(754 + PHY_BASE_ADDR,DENALI_PHY_754_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(754 + PHY_BASE_ADDR,DENALI_PHY_754_DATA,0);
  #else
         REGWR(754 + PHY_BASE_ADDR,DENALI_PHY_754_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_755_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(755 + PHY_BASE_ADDR,DENALI_PHY_755_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(755 + PHY_BASE_ADDR,DENALI_PHY_755_DATA,0);
  #else
         REGWR(755 + PHY_BASE_ADDR,DENALI_PHY_755_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_756_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(756 + PHY_BASE_ADDR,DENALI_PHY_756_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(756 + PHY_BASE_ADDR,DENALI_PHY_756_DATA,0);
  #else
         REGWR(756 + PHY_BASE_ADDR,DENALI_PHY_756_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_757_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(757 + PHY_BASE_ADDR,DENALI_PHY_757_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(757 + PHY_BASE_ADDR,DENALI_PHY_757_DATA,0);
  #else
         REGWR(757 + PHY_BASE_ADDR,DENALI_PHY_757_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_758_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(758 + PHY_BASE_ADDR,DENALI_PHY_758_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(758 + PHY_BASE_ADDR,DENALI_PHY_758_DATA,0);
  #else
         REGWR(758 + PHY_BASE_ADDR,DENALI_PHY_758_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_759_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(759 + PHY_BASE_ADDR,DENALI_PHY_759_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(759 + PHY_BASE_ADDR,DENALI_PHY_759_DATA,0);
  #else
         REGWR(759 + PHY_BASE_ADDR,DENALI_PHY_759_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_760_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(760 + PHY_BASE_ADDR,DENALI_PHY_760_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(760 + PHY_BASE_ADDR,DENALI_PHY_760_DATA,0);
  #else
         REGWR(760 + PHY_BASE_ADDR,DENALI_PHY_760_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_761_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(761 + PHY_BASE_ADDR,DENALI_PHY_761_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(761 + PHY_BASE_ADDR,DENALI_PHY_761_DATA,0);
  #else
         REGWR(761 + PHY_BASE_ADDR,DENALI_PHY_761_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_762_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(762 + PHY_BASE_ADDR,DENALI_PHY_762_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(762 + PHY_BASE_ADDR,DENALI_PHY_762_DATA,0);
  #else
         REGWR(762 + PHY_BASE_ADDR,DENALI_PHY_762_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_763_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(763 + PHY_BASE_ADDR,DENALI_PHY_763_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(763 + PHY_BASE_ADDR,DENALI_PHY_763_DATA,0);
  #else
         REGWR(763 + PHY_BASE_ADDR,DENALI_PHY_763_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_764_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(764 + PHY_BASE_ADDR,DENALI_PHY_764_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(764 + PHY_BASE_ADDR,DENALI_PHY_764_DATA,0);
  #else
         REGWR(764 + PHY_BASE_ADDR,DENALI_PHY_764_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_765_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(765 + PHY_BASE_ADDR,DENALI_PHY_765_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(765 + PHY_BASE_ADDR,DENALI_PHY_765_DATA,0);
  #else
         REGWR(765 + PHY_BASE_ADDR,DENALI_PHY_765_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_766_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(766 + PHY_BASE_ADDR,DENALI_PHY_766_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(766 + PHY_BASE_ADDR,DENALI_PHY_766_DATA,0);
  #else
         REGWR(766 + PHY_BASE_ADDR,DENALI_PHY_766_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_767_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(767 + PHY_BASE_ADDR,DENALI_PHY_767_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(767 + PHY_BASE_ADDR,DENALI_PHY_767_DATA,0);
  #else
         REGWR(767 + PHY_BASE_ADDR,DENALI_PHY_767_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_768_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(768 + PHY_BASE_ADDR,DENALI_PHY_768_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(768 + PHY_BASE_ADDR,DENALI_PHY_768_DATA,0);
  #else
         REGWR(768 + PHY_BASE_ADDR,DENALI_PHY_768_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_769_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(769 + PHY_BASE_ADDR,DENALI_PHY_769_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(769 + PHY_BASE_ADDR,DENALI_PHY_769_DATA,0);
  #else
         REGWR(769 + PHY_BASE_ADDR,DENALI_PHY_769_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_770_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(770 + PHY_BASE_ADDR,DENALI_PHY_770_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(770 + PHY_BASE_ADDR,DENALI_PHY_770_DATA,0);
  #else
         REGWR(770 + PHY_BASE_ADDR,DENALI_PHY_770_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_771_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(771 + PHY_BASE_ADDR,DENALI_PHY_771_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(771 + PHY_BASE_ADDR,DENALI_PHY_771_DATA,0);
  #else
         REGWR(771 + PHY_BASE_ADDR,DENALI_PHY_771_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_772_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(772 + PHY_BASE_ADDR,DENALI_PHY_772_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(772 + PHY_BASE_ADDR,DENALI_PHY_772_DATA,0);
  #else
         REGWR(772 + PHY_BASE_ADDR,DENALI_PHY_772_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_773_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(773 + PHY_BASE_ADDR,DENALI_PHY_773_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(773 + PHY_BASE_ADDR,DENALI_PHY_773_DATA,0);
  #else
         REGWR(773 + PHY_BASE_ADDR,DENALI_PHY_773_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_774_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(774 + PHY_BASE_ADDR,DENALI_PHY_774_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(774 + PHY_BASE_ADDR,DENALI_PHY_774_DATA,0);
  #else
         REGWR(774 + PHY_BASE_ADDR,DENALI_PHY_774_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_775_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(775 + PHY_BASE_ADDR,DENALI_PHY_775_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(775 + PHY_BASE_ADDR,DENALI_PHY_775_DATA,0);
  #else
         REGWR(775 + PHY_BASE_ADDR,DENALI_PHY_775_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_776_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(776 + PHY_BASE_ADDR,DENALI_PHY_776_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(776 + PHY_BASE_ADDR,DENALI_PHY_776_DATA,0);
  #else
         REGWR(776 + PHY_BASE_ADDR,DENALI_PHY_776_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_777_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(777 + PHY_BASE_ADDR,DENALI_PHY_777_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(777 + PHY_BASE_ADDR,DENALI_PHY_777_DATA,0);
  #else
         REGWR(777 + PHY_BASE_ADDR,DENALI_PHY_777_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_778_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(778 + PHY_BASE_ADDR,DENALI_PHY_778_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(778 + PHY_BASE_ADDR,DENALI_PHY_778_DATA,0);
  #else
         REGWR(778 + PHY_BASE_ADDR,DENALI_PHY_778_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_779_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(779 + PHY_BASE_ADDR,DENALI_PHY_779_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(779 + PHY_BASE_ADDR,DENALI_PHY_779_DATA,0);
  #else
         REGWR(779 + PHY_BASE_ADDR,DENALI_PHY_779_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_780_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(780 + PHY_BASE_ADDR,DENALI_PHY_780_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(780 + PHY_BASE_ADDR,DENALI_PHY_780_DATA,0);
  #else
         REGWR(780 + PHY_BASE_ADDR,DENALI_PHY_780_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_781_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(781 + PHY_BASE_ADDR,DENALI_PHY_781_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(781 + PHY_BASE_ADDR,DENALI_PHY_781_DATA,0);
  #else
         REGWR(781 + PHY_BASE_ADDR,DENALI_PHY_781_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_782_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(782 + PHY_BASE_ADDR,DENALI_PHY_782_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(782 + PHY_BASE_ADDR,DENALI_PHY_782_DATA,0);
  #else
         REGWR(782 + PHY_BASE_ADDR,DENALI_PHY_782_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_783_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(783 + PHY_BASE_ADDR,DENALI_PHY_783_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(783 + PHY_BASE_ADDR,DENALI_PHY_783_DATA,0);
  #else
         REGWR(783 + PHY_BASE_ADDR,DENALI_PHY_783_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_784_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(784 + PHY_BASE_ADDR,DENALI_PHY_784_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(784 + PHY_BASE_ADDR,DENALI_PHY_784_DATA,0);
  #else
         REGWR(784 + PHY_BASE_ADDR,DENALI_PHY_784_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_785_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(785 + PHY_BASE_ADDR,DENALI_PHY_785_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(785 + PHY_BASE_ADDR,DENALI_PHY_785_DATA,0);
  #else
         REGWR(785 + PHY_BASE_ADDR,DENALI_PHY_785_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_786_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(786 + PHY_BASE_ADDR,DENALI_PHY_786_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(786 + PHY_BASE_ADDR,DENALI_PHY_786_DATA,0);
  #else
         REGWR(786 + PHY_BASE_ADDR,DENALI_PHY_786_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_787_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(787 + PHY_BASE_ADDR,DENALI_PHY_787_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(787 + PHY_BASE_ADDR,DENALI_PHY_787_DATA,0);
  #else
         REGWR(787 + PHY_BASE_ADDR,DENALI_PHY_787_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_788_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(788 + PHY_BASE_ADDR,DENALI_PHY_788_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(788 + PHY_BASE_ADDR,DENALI_PHY_788_DATA,0);
  #else
         REGWR(788 + PHY_BASE_ADDR,DENALI_PHY_788_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_789_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(789 + PHY_BASE_ADDR,DENALI_PHY_789_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(789 + PHY_BASE_ADDR,DENALI_PHY_789_DATA,0);
  #else
         REGWR(789 + PHY_BASE_ADDR,DENALI_PHY_789_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_790_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(790 + PHY_BASE_ADDR,DENALI_PHY_790_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(790 + PHY_BASE_ADDR,DENALI_PHY_790_DATA,0);
  #else
         REGWR(790 + PHY_BASE_ADDR,DENALI_PHY_790_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_791_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(791 + PHY_BASE_ADDR,DENALI_PHY_791_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(791 + PHY_BASE_ADDR,DENALI_PHY_791_DATA,0);
  #else
         REGWR(791 + PHY_BASE_ADDR,DENALI_PHY_791_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_792_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(792 + PHY_BASE_ADDR,DENALI_PHY_792_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(792 + PHY_BASE_ADDR,DENALI_PHY_792_DATA,0);
  #else
         REGWR(792 + PHY_BASE_ADDR,DENALI_PHY_792_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_793_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(793 + PHY_BASE_ADDR,DENALI_PHY_793_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(793 + PHY_BASE_ADDR,DENALI_PHY_793_DATA,0);
  #else
         REGWR(793 + PHY_BASE_ADDR,DENALI_PHY_793_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_794_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(794 + PHY_BASE_ADDR,DENALI_PHY_794_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(794 + PHY_BASE_ADDR,DENALI_PHY_794_DATA,0);
  #else
         REGWR(794 + PHY_BASE_ADDR,DENALI_PHY_794_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_795_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(795 + PHY_BASE_ADDR,DENALI_PHY_795_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(795 + PHY_BASE_ADDR,DENALI_PHY_795_DATA,0);
  #else
         REGWR(795 + PHY_BASE_ADDR,DENALI_PHY_795_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_796_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(796 + PHY_BASE_ADDR,DENALI_PHY_796_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(796 + PHY_BASE_ADDR,DENALI_PHY_796_DATA,0);
  #else
         REGWR(796 + PHY_BASE_ADDR,DENALI_PHY_796_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_797_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(797 + PHY_BASE_ADDR,DENALI_PHY_797_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(797 + PHY_BASE_ADDR,DENALI_PHY_797_DATA,0);
  #else
         REGWR(797 + PHY_BASE_ADDR,DENALI_PHY_797_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_798_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(798 + PHY_BASE_ADDR,DENALI_PHY_798_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(798 + PHY_BASE_ADDR,DENALI_PHY_798_DATA,0);
  #else
         REGWR(798 + PHY_BASE_ADDR,DENALI_PHY_798_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_799_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(799 + PHY_BASE_ADDR,DENALI_PHY_799_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(799 + PHY_BASE_ADDR,DENALI_PHY_799_DATA,0);
  #else
         REGWR(799 + PHY_BASE_ADDR,DENALI_PHY_799_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_800_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(800 + PHY_BASE_ADDR,DENALI_PHY_800_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(800 + PHY_BASE_ADDR,DENALI_PHY_800_DATA,0);
  #else
         REGWR(800 + PHY_BASE_ADDR,DENALI_PHY_800_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_801_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(801 + PHY_BASE_ADDR,DENALI_PHY_801_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(801 + PHY_BASE_ADDR,DENALI_PHY_801_DATA,0);
  #else
         REGWR(801 + PHY_BASE_ADDR,DENALI_PHY_801_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_802_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(802 + PHY_BASE_ADDR,DENALI_PHY_802_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(802 + PHY_BASE_ADDR,DENALI_PHY_802_DATA,0);
  #else
         REGWR(802 + PHY_BASE_ADDR,DENALI_PHY_802_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_803_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(803 + PHY_BASE_ADDR,DENALI_PHY_803_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(803 + PHY_BASE_ADDR,DENALI_PHY_803_DATA,0);
  #else
         REGWR(803 + PHY_BASE_ADDR,DENALI_PHY_803_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_804_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(804 + PHY_BASE_ADDR,DENALI_PHY_804_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(804 + PHY_BASE_ADDR,DENALI_PHY_804_DATA,0);
  #else
         REGWR(804 + PHY_BASE_ADDR,DENALI_PHY_804_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_805_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(805 + PHY_BASE_ADDR,DENALI_PHY_805_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(805 + PHY_BASE_ADDR,DENALI_PHY_805_DATA,0);
  #else
         REGWR(805 + PHY_BASE_ADDR,DENALI_PHY_805_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_806_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(806 + PHY_BASE_ADDR,DENALI_PHY_806_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(806 + PHY_BASE_ADDR,DENALI_PHY_806_DATA,0);
  #else
         REGWR(806 + PHY_BASE_ADDR,DENALI_PHY_806_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_807_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(807 + PHY_BASE_ADDR,DENALI_PHY_807_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(807 + PHY_BASE_ADDR,DENALI_PHY_807_DATA,0);
  #else
         REGWR(807 + PHY_BASE_ADDR,DENALI_PHY_807_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_808_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(808 + PHY_BASE_ADDR,DENALI_PHY_808_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(808 + PHY_BASE_ADDR,DENALI_PHY_808_DATA,0);
  #else
         REGWR(808 + PHY_BASE_ADDR,DENALI_PHY_808_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_809_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(809 + PHY_BASE_ADDR,DENALI_PHY_809_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(809 + PHY_BASE_ADDR,DENALI_PHY_809_DATA,0);
  #else
         REGWR(809 + PHY_BASE_ADDR,DENALI_PHY_809_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_810_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(810 + PHY_BASE_ADDR,DENALI_PHY_810_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(810 + PHY_BASE_ADDR,DENALI_PHY_810_DATA,0);
  #else
         REGWR(810 + PHY_BASE_ADDR,DENALI_PHY_810_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_811_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(811 + PHY_BASE_ADDR,DENALI_PHY_811_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(811 + PHY_BASE_ADDR,DENALI_PHY_811_DATA,0);
  #else
         REGWR(811 + PHY_BASE_ADDR,DENALI_PHY_811_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_812_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(812 + PHY_BASE_ADDR,DENALI_PHY_812_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(812 + PHY_BASE_ADDR,DENALI_PHY_812_DATA,0);
  #else
         REGWR(812 + PHY_BASE_ADDR,DENALI_PHY_812_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_813_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(813 + PHY_BASE_ADDR,DENALI_PHY_813_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(813 + PHY_BASE_ADDR,DENALI_PHY_813_DATA,0);
  #else
         REGWR(813 + PHY_BASE_ADDR,DENALI_PHY_813_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_814_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(814 + PHY_BASE_ADDR,DENALI_PHY_814_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(814 + PHY_BASE_ADDR,DENALI_PHY_814_DATA,0);
  #else
         REGWR(814 + PHY_BASE_ADDR,DENALI_PHY_814_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_815_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(815 + PHY_BASE_ADDR,DENALI_PHY_815_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(815 + PHY_BASE_ADDR,DENALI_PHY_815_DATA,0);
  #else
         REGWR(815 + PHY_BASE_ADDR,DENALI_PHY_815_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_816_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(816 + PHY_BASE_ADDR,DENALI_PHY_816_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(816 + PHY_BASE_ADDR,DENALI_PHY_816_DATA,0);
  #else
         REGWR(816 + PHY_BASE_ADDR,DENALI_PHY_816_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_817_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(817 + PHY_BASE_ADDR,DENALI_PHY_817_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(817 + PHY_BASE_ADDR,DENALI_PHY_817_DATA,0);
  #else
         REGWR(817 + PHY_BASE_ADDR,DENALI_PHY_817_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_818_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(818 + PHY_BASE_ADDR,DENALI_PHY_818_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(818 + PHY_BASE_ADDR,DENALI_PHY_818_DATA,0);
  #else
         REGWR(818 + PHY_BASE_ADDR,DENALI_PHY_818_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_819_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(819 + PHY_BASE_ADDR,DENALI_PHY_819_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(819 + PHY_BASE_ADDR,DENALI_PHY_819_DATA,0);
  #else
         REGWR(819 + PHY_BASE_ADDR,DENALI_PHY_819_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_820_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(820 + PHY_BASE_ADDR,DENALI_PHY_820_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(820 + PHY_BASE_ADDR,DENALI_PHY_820_DATA,0);
  #else
         REGWR(820 + PHY_BASE_ADDR,DENALI_PHY_820_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_821_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(821 + PHY_BASE_ADDR,DENALI_PHY_821_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(821 + PHY_BASE_ADDR,DENALI_PHY_821_DATA,0);
  #else
         REGWR(821 + PHY_BASE_ADDR,DENALI_PHY_821_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_822_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(822 + PHY_BASE_ADDR,DENALI_PHY_822_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(822 + PHY_BASE_ADDR,DENALI_PHY_822_DATA,0);
  #else
         REGWR(822 + PHY_BASE_ADDR,DENALI_PHY_822_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_823_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(823 + PHY_BASE_ADDR,DENALI_PHY_823_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(823 + PHY_BASE_ADDR,DENALI_PHY_823_DATA,0);
  #else
         REGWR(823 + PHY_BASE_ADDR,DENALI_PHY_823_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_824_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(824 + PHY_BASE_ADDR,DENALI_PHY_824_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(824 + PHY_BASE_ADDR,DENALI_PHY_824_DATA,0);
  #else
         REGWR(824 + PHY_BASE_ADDR,DENALI_PHY_824_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_825_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(825 + PHY_BASE_ADDR,DENALI_PHY_825_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(825 + PHY_BASE_ADDR,DENALI_PHY_825_DATA,0);
  #else
         REGWR(825 + PHY_BASE_ADDR,DENALI_PHY_825_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_826_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(826 + PHY_BASE_ADDR,DENALI_PHY_826_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(826 + PHY_BASE_ADDR,DENALI_PHY_826_DATA,0);
  #else
         REGWR(826 + PHY_BASE_ADDR,DENALI_PHY_826_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_827_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(827 + PHY_BASE_ADDR,DENALI_PHY_827_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(827 + PHY_BASE_ADDR,DENALI_PHY_827_DATA,0);
  #else
         REGWR(827 + PHY_BASE_ADDR,DENALI_PHY_827_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_828_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(828 + PHY_BASE_ADDR,DENALI_PHY_828_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(828 + PHY_BASE_ADDR,DENALI_PHY_828_DATA,0);
  #else
         REGWR(828 + PHY_BASE_ADDR,DENALI_PHY_828_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_829_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(829 + PHY_BASE_ADDR,DENALI_PHY_829_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(829 + PHY_BASE_ADDR,DENALI_PHY_829_DATA,0);
  #else
         REGWR(829 + PHY_BASE_ADDR,DENALI_PHY_829_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_830_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(830 + PHY_BASE_ADDR,DENALI_PHY_830_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(830 + PHY_BASE_ADDR,DENALI_PHY_830_DATA,0);
  #else
         REGWR(830 + PHY_BASE_ADDR,DENALI_PHY_830_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_831_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(831 + PHY_BASE_ADDR,DENALI_PHY_831_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(831 + PHY_BASE_ADDR,DENALI_PHY_831_DATA,0);
  #else
         REGWR(831 + PHY_BASE_ADDR,DENALI_PHY_831_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_832_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(832 + PHY_BASE_ADDR,DENALI_PHY_832_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(832 + PHY_BASE_ADDR,DENALI_PHY_832_DATA,0);
  #else
         REGWR(832 + PHY_BASE_ADDR,DENALI_PHY_832_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_833_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(833 + PHY_BASE_ADDR,DENALI_PHY_833_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(833 + PHY_BASE_ADDR,DENALI_PHY_833_DATA,0);
  #else
         REGWR(833 + PHY_BASE_ADDR,DENALI_PHY_833_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_834_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(834 + PHY_BASE_ADDR,DENALI_PHY_834_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(834 + PHY_BASE_ADDR,DENALI_PHY_834_DATA,0);
  #else
         REGWR(834 + PHY_BASE_ADDR,DENALI_PHY_834_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_835_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(835 + PHY_BASE_ADDR,DENALI_PHY_835_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(835 + PHY_BASE_ADDR,DENALI_PHY_835_DATA,0);
  #else
         REGWR(835 + PHY_BASE_ADDR,DENALI_PHY_835_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_836_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(836 + PHY_BASE_ADDR,DENALI_PHY_836_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(836 + PHY_BASE_ADDR,DENALI_PHY_836_DATA,0);
  #else
         REGWR(836 + PHY_BASE_ADDR,DENALI_PHY_836_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_837_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(837 + PHY_BASE_ADDR,DENALI_PHY_837_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(837 + PHY_BASE_ADDR,DENALI_PHY_837_DATA,0);
  #else
         REGWR(837 + PHY_BASE_ADDR,DENALI_PHY_837_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_838_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(838 + PHY_BASE_ADDR,DENALI_PHY_838_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(838 + PHY_BASE_ADDR,DENALI_PHY_838_DATA,0);
  #else
         REGWR(838 + PHY_BASE_ADDR,DENALI_PHY_838_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_839_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(839 + PHY_BASE_ADDR,DENALI_PHY_839_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(839 + PHY_BASE_ADDR,DENALI_PHY_839_DATA,0);
  #else
         REGWR(839 + PHY_BASE_ADDR,DENALI_PHY_839_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_840_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(840 + PHY_BASE_ADDR,DENALI_PHY_840_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(840 + PHY_BASE_ADDR,DENALI_PHY_840_DATA,0);
  #else
         REGWR(840 + PHY_BASE_ADDR,DENALI_PHY_840_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_841_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(841 + PHY_BASE_ADDR,DENALI_PHY_841_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(841 + PHY_BASE_ADDR,DENALI_PHY_841_DATA,0);
  #else
         REGWR(841 + PHY_BASE_ADDR,DENALI_PHY_841_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_842_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(842 + PHY_BASE_ADDR,DENALI_PHY_842_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(842 + PHY_BASE_ADDR,DENALI_PHY_842_DATA,0);
  #else
         REGWR(842 + PHY_BASE_ADDR,DENALI_PHY_842_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_843_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(843 + PHY_BASE_ADDR,DENALI_PHY_843_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(843 + PHY_BASE_ADDR,DENALI_PHY_843_DATA,0);
  #else
         REGWR(843 + PHY_BASE_ADDR,DENALI_PHY_843_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_844_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(844 + PHY_BASE_ADDR,DENALI_PHY_844_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(844 + PHY_BASE_ADDR,DENALI_PHY_844_DATA,0);
  #else
         REGWR(844 + PHY_BASE_ADDR,DENALI_PHY_844_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_845_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(845 + PHY_BASE_ADDR,DENALI_PHY_845_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(845 + PHY_BASE_ADDR,DENALI_PHY_845_DATA,0);
  #else
         REGWR(845 + PHY_BASE_ADDR,DENALI_PHY_845_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_846_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(846 + PHY_BASE_ADDR,DENALI_PHY_846_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(846 + PHY_BASE_ADDR,DENALI_PHY_846_DATA,0);
  #else
         REGWR(846 + PHY_BASE_ADDR,DENALI_PHY_846_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_847_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(847 + PHY_BASE_ADDR,DENALI_PHY_847_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(847 + PHY_BASE_ADDR,DENALI_PHY_847_DATA,0);
  #else
         REGWR(847 + PHY_BASE_ADDR,DENALI_PHY_847_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_848_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(848 + PHY_BASE_ADDR,DENALI_PHY_848_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(848 + PHY_BASE_ADDR,DENALI_PHY_848_DATA,0);
  #else
         REGWR(848 + PHY_BASE_ADDR,DENALI_PHY_848_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_849_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(849 + PHY_BASE_ADDR,DENALI_PHY_849_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(849 + PHY_BASE_ADDR,DENALI_PHY_849_DATA,0);
  #else
         REGWR(849 + PHY_BASE_ADDR,DENALI_PHY_849_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_850_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(850 + PHY_BASE_ADDR,DENALI_PHY_850_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(850 + PHY_BASE_ADDR,DENALI_PHY_850_DATA,0);
  #else
         REGWR(850 + PHY_BASE_ADDR,DENALI_PHY_850_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_851_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(851 + PHY_BASE_ADDR,DENALI_PHY_851_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(851 + PHY_BASE_ADDR,DENALI_PHY_851_DATA,0);
  #else
         REGWR(851 + PHY_BASE_ADDR,DENALI_PHY_851_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_852_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(852 + PHY_BASE_ADDR,DENALI_PHY_852_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(852 + PHY_BASE_ADDR,DENALI_PHY_852_DATA,0);
  #else
         REGWR(852 + PHY_BASE_ADDR,DENALI_PHY_852_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_853_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(853 + PHY_BASE_ADDR,DENALI_PHY_853_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(853 + PHY_BASE_ADDR,DENALI_PHY_853_DATA,0);
  #else
         REGWR(853 + PHY_BASE_ADDR,DENALI_PHY_853_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_854_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(854 + PHY_BASE_ADDR,DENALI_PHY_854_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(854 + PHY_BASE_ADDR,DENALI_PHY_854_DATA,0);
  #else
         REGWR(854 + PHY_BASE_ADDR,DENALI_PHY_854_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_855_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(855 + PHY_BASE_ADDR,DENALI_PHY_855_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(855 + PHY_BASE_ADDR,DENALI_PHY_855_DATA,0);
  #else
         REGWR(855 + PHY_BASE_ADDR,DENALI_PHY_855_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_856_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(856 + PHY_BASE_ADDR,DENALI_PHY_856_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(856 + PHY_BASE_ADDR,DENALI_PHY_856_DATA,0);
  #else
         REGWR(856 + PHY_BASE_ADDR,DENALI_PHY_856_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_857_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(857 + PHY_BASE_ADDR,DENALI_PHY_857_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(857 + PHY_BASE_ADDR,DENALI_PHY_857_DATA,0);
  #else
         REGWR(857 + PHY_BASE_ADDR,DENALI_PHY_857_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_858_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(858 + PHY_BASE_ADDR,DENALI_PHY_858_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(858 + PHY_BASE_ADDR,DENALI_PHY_858_DATA,0);
  #else
         REGWR(858 + PHY_BASE_ADDR,DENALI_PHY_858_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_859_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(859 + PHY_BASE_ADDR,DENALI_PHY_859_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(859 + PHY_BASE_ADDR,DENALI_PHY_859_DATA,0);
  #else
         REGWR(859 + PHY_BASE_ADDR,DENALI_PHY_859_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_860_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(860 + PHY_BASE_ADDR,DENALI_PHY_860_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(860 + PHY_BASE_ADDR,DENALI_PHY_860_DATA,0);
  #else
         REGWR(860 + PHY_BASE_ADDR,DENALI_PHY_860_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_861_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(861 + PHY_BASE_ADDR,DENALI_PHY_861_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(861 + PHY_BASE_ADDR,DENALI_PHY_861_DATA,0);
  #else
         REGWR(861 + PHY_BASE_ADDR,DENALI_PHY_861_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_862_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(862 + PHY_BASE_ADDR,DENALI_PHY_862_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(862 + PHY_BASE_ADDR,DENALI_PHY_862_DATA,0);
  #else
         REGWR(862 + PHY_BASE_ADDR,DENALI_PHY_862_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_863_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(863 + PHY_BASE_ADDR,DENALI_PHY_863_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(863 + PHY_BASE_ADDR,DENALI_PHY_863_DATA,0);
  #else
         REGWR(863 + PHY_BASE_ADDR,DENALI_PHY_863_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_864_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(864 + PHY_BASE_ADDR,DENALI_PHY_864_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(864 + PHY_BASE_ADDR,DENALI_PHY_864_DATA,0);
  #else
         REGWR(864 + PHY_BASE_ADDR,DENALI_PHY_864_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_865_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(865 + PHY_BASE_ADDR,DENALI_PHY_865_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(865 + PHY_BASE_ADDR,DENALI_PHY_865_DATA,0);
  #else
         REGWR(865 + PHY_BASE_ADDR,DENALI_PHY_865_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_866_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(866 + PHY_BASE_ADDR,DENALI_PHY_866_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(866 + PHY_BASE_ADDR,DENALI_PHY_866_DATA,0);
  #else
         REGWR(866 + PHY_BASE_ADDR,DENALI_PHY_866_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_867_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(867 + PHY_BASE_ADDR,DENALI_PHY_867_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(867 + PHY_BASE_ADDR,DENALI_PHY_867_DATA,0);
  #else
         REGWR(867 + PHY_BASE_ADDR,DENALI_PHY_867_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_868_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(868 + PHY_BASE_ADDR,DENALI_PHY_868_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(868 + PHY_BASE_ADDR,DENALI_PHY_868_DATA,0);
  #else
         REGWR(868 + PHY_BASE_ADDR,DENALI_PHY_868_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_869_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(869 + PHY_BASE_ADDR,DENALI_PHY_869_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(869 + PHY_BASE_ADDR,DENALI_PHY_869_DATA,0);
  #else
         REGWR(869 + PHY_BASE_ADDR,DENALI_PHY_869_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_870_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(870 + PHY_BASE_ADDR,DENALI_PHY_870_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(870 + PHY_BASE_ADDR,DENALI_PHY_870_DATA,0);
  #else
         REGWR(870 + PHY_BASE_ADDR,DENALI_PHY_870_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_871_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(871 + PHY_BASE_ADDR,DENALI_PHY_871_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(871 + PHY_BASE_ADDR,DENALI_PHY_871_DATA,0);
  #else
         REGWR(871 + PHY_BASE_ADDR,DENALI_PHY_871_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_872_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(872 + PHY_BASE_ADDR,DENALI_PHY_872_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(872 + PHY_BASE_ADDR,DENALI_PHY_872_DATA,0);
  #else
         REGWR(872 + PHY_BASE_ADDR,DENALI_PHY_872_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_873_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(873 + PHY_BASE_ADDR,DENALI_PHY_873_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(873 + PHY_BASE_ADDR,DENALI_PHY_873_DATA,0);
  #else
         REGWR(873 + PHY_BASE_ADDR,DENALI_PHY_873_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_874_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(874 + PHY_BASE_ADDR,DENALI_PHY_874_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(874 + PHY_BASE_ADDR,DENALI_PHY_874_DATA,0);
  #else
         REGWR(874 + PHY_BASE_ADDR,DENALI_PHY_874_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_875_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(875 + PHY_BASE_ADDR,DENALI_PHY_875_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(875 + PHY_BASE_ADDR,DENALI_PHY_875_DATA,0);
  #else
         REGWR(875 + PHY_BASE_ADDR,DENALI_PHY_875_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_876_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(876 + PHY_BASE_ADDR,DENALI_PHY_876_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(876 + PHY_BASE_ADDR,DENALI_PHY_876_DATA,0);
  #else
         REGWR(876 + PHY_BASE_ADDR,DENALI_PHY_876_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_877_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(877 + PHY_BASE_ADDR,DENALI_PHY_877_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(877 + PHY_BASE_ADDR,DENALI_PHY_877_DATA,0);
  #else
         REGWR(877 + PHY_BASE_ADDR,DENALI_PHY_877_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_878_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(878 + PHY_BASE_ADDR,DENALI_PHY_878_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(878 + PHY_BASE_ADDR,DENALI_PHY_878_DATA,0);
  #else
         REGWR(878 + PHY_BASE_ADDR,DENALI_PHY_878_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_879_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(879 + PHY_BASE_ADDR,DENALI_PHY_879_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(879 + PHY_BASE_ADDR,DENALI_PHY_879_DATA,0);
  #else
         REGWR(879 + PHY_BASE_ADDR,DENALI_PHY_879_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_880_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(880 + PHY_BASE_ADDR,DENALI_PHY_880_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(880 + PHY_BASE_ADDR,DENALI_PHY_880_DATA,0);
  #else
         REGWR(880 + PHY_BASE_ADDR,DENALI_PHY_880_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_881_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(881 + PHY_BASE_ADDR,DENALI_PHY_881_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(881 + PHY_BASE_ADDR,DENALI_PHY_881_DATA,0);
  #else
         REGWR(881 + PHY_BASE_ADDR,DENALI_PHY_881_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_882_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(882 + PHY_BASE_ADDR,DENALI_PHY_882_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(882 + PHY_BASE_ADDR,DENALI_PHY_882_DATA,0);
  #else
         REGWR(882 + PHY_BASE_ADDR,DENALI_PHY_882_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_883_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(883 + PHY_BASE_ADDR,DENALI_PHY_883_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(883 + PHY_BASE_ADDR,DENALI_PHY_883_DATA,0);
  #else
         REGWR(883 + PHY_BASE_ADDR,DENALI_PHY_883_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_884_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(884 + PHY_BASE_ADDR,DENALI_PHY_884_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(884 + PHY_BASE_ADDR,DENALI_PHY_884_DATA,0);
  #else
         REGWR(884 + PHY_BASE_ADDR,DENALI_PHY_884_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_885_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(885 + PHY_BASE_ADDR,DENALI_PHY_885_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(885 + PHY_BASE_ADDR,DENALI_PHY_885_DATA,0);
  #else
         REGWR(885 + PHY_BASE_ADDR,DENALI_PHY_885_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_886_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(886 + PHY_BASE_ADDR,DENALI_PHY_886_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(886 + PHY_BASE_ADDR,DENALI_PHY_886_DATA,0);
  #else
         REGWR(886 + PHY_BASE_ADDR,DENALI_PHY_886_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_887_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(887 + PHY_BASE_ADDR,DENALI_PHY_887_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(887 + PHY_BASE_ADDR,DENALI_PHY_887_DATA,0);
  #else
         REGWR(887 + PHY_BASE_ADDR,DENALI_PHY_887_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_888_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(888 + PHY_BASE_ADDR,DENALI_PHY_888_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(888 + PHY_BASE_ADDR,DENALI_PHY_888_DATA,0);
  #else
         REGWR(888 + PHY_BASE_ADDR,DENALI_PHY_888_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_889_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(889 + PHY_BASE_ADDR,DENALI_PHY_889_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(889 + PHY_BASE_ADDR,DENALI_PHY_889_DATA,0);
  #else
         REGWR(889 + PHY_BASE_ADDR,DENALI_PHY_889_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_890_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(890 + PHY_BASE_ADDR,DENALI_PHY_890_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(890 + PHY_BASE_ADDR,DENALI_PHY_890_DATA,0);
  #else
         REGWR(890 + PHY_BASE_ADDR,DENALI_PHY_890_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_891_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(891 + PHY_BASE_ADDR,DENALI_PHY_891_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(891 + PHY_BASE_ADDR,DENALI_PHY_891_DATA,0);
  #else
         REGWR(891 + PHY_BASE_ADDR,DENALI_PHY_891_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_892_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(892 + PHY_BASE_ADDR,DENALI_PHY_892_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(892 + PHY_BASE_ADDR,DENALI_PHY_892_DATA,0);
  #else
         REGWR(892 + PHY_BASE_ADDR,DENALI_PHY_892_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_893_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(893 + PHY_BASE_ADDR,DENALI_PHY_893_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(893 + PHY_BASE_ADDR,DENALI_PHY_893_DATA,0);
  #else
         REGWR(893 + PHY_BASE_ADDR,DENALI_PHY_893_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_894_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(894 + PHY_BASE_ADDR,DENALI_PHY_894_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(894 + PHY_BASE_ADDR,DENALI_PHY_894_DATA,0);
  #else
         REGWR(894 + PHY_BASE_ADDR,DENALI_PHY_894_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_895_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(895 + PHY_BASE_ADDR,DENALI_PHY_895_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(895 + PHY_BASE_ADDR,DENALI_PHY_895_DATA,0);
  #else
         REGWR(895 + PHY_BASE_ADDR,DENALI_PHY_895_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_896_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(896 + PHY_BASE_ADDR,DENALI_PHY_896_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(896 + PHY_BASE_ADDR,DENALI_PHY_896_DATA,0);
  #else
         REGWR(896 + PHY_BASE_ADDR,DENALI_PHY_896_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_897_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(897 + PHY_BASE_ADDR,DENALI_PHY_897_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(897 + PHY_BASE_ADDR,DENALI_PHY_897_DATA,0);
  #else
         REGWR(897 + PHY_BASE_ADDR,DENALI_PHY_897_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_898_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(898 + PHY_BASE_ADDR,DENALI_PHY_898_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(898 + PHY_BASE_ADDR,DENALI_PHY_898_DATA,0);
  #else
         REGWR(898 + PHY_BASE_ADDR,DENALI_PHY_898_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_899_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(899 + PHY_BASE_ADDR,DENALI_PHY_899_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(899 + PHY_BASE_ADDR,DENALI_PHY_899_DATA,0);
  #else
         REGWR(899 + PHY_BASE_ADDR,DENALI_PHY_899_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_900_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(900 + PHY_BASE_ADDR,DENALI_PHY_900_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(900 + PHY_BASE_ADDR,DENALI_PHY_900_DATA,0);
  #else
         REGWR(900 + PHY_BASE_ADDR,DENALI_PHY_900_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_901_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(901 + PHY_BASE_ADDR,DENALI_PHY_901_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(901 + PHY_BASE_ADDR,DENALI_PHY_901_DATA,0);
  #else
         REGWR(901 + PHY_BASE_ADDR,DENALI_PHY_901_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_902_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(902 + PHY_BASE_ADDR,DENALI_PHY_902_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(902 + PHY_BASE_ADDR,DENALI_PHY_902_DATA,0);
  #else
         REGWR(902 + PHY_BASE_ADDR,DENALI_PHY_902_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_903_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(903 + PHY_BASE_ADDR,DENALI_PHY_903_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(903 + PHY_BASE_ADDR,DENALI_PHY_903_DATA,0);
  #else
         REGWR(903 + PHY_BASE_ADDR,DENALI_PHY_903_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_904_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(904 + PHY_BASE_ADDR,DENALI_PHY_904_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(904 + PHY_BASE_ADDR,DENALI_PHY_904_DATA,0);
  #else
         REGWR(904 + PHY_BASE_ADDR,DENALI_PHY_904_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_905_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(905 + PHY_BASE_ADDR,DENALI_PHY_905_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(905 + PHY_BASE_ADDR,DENALI_PHY_905_DATA,0);
  #else
         REGWR(905 + PHY_BASE_ADDR,DENALI_PHY_905_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_906_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(906 + PHY_BASE_ADDR,DENALI_PHY_906_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(906 + PHY_BASE_ADDR,DENALI_PHY_906_DATA,0);
  #else
         REGWR(906 + PHY_BASE_ADDR,DENALI_PHY_906_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_907_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(907 + PHY_BASE_ADDR,DENALI_PHY_907_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(907 + PHY_BASE_ADDR,DENALI_PHY_907_DATA,0);
  #else
         REGWR(907 + PHY_BASE_ADDR,DENALI_PHY_907_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_908_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(908 + PHY_BASE_ADDR,DENALI_PHY_908_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(908 + PHY_BASE_ADDR,DENALI_PHY_908_DATA,0);
  #else
         REGWR(908 + PHY_BASE_ADDR,DENALI_PHY_908_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_909_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(909 + PHY_BASE_ADDR,DENALI_PHY_909_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(909 + PHY_BASE_ADDR,DENALI_PHY_909_DATA,0);
  #else
         REGWR(909 + PHY_BASE_ADDR,DENALI_PHY_909_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_910_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(910 + PHY_BASE_ADDR,DENALI_PHY_910_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(910 + PHY_BASE_ADDR,DENALI_PHY_910_DATA,0);
  #else
         REGWR(910 + PHY_BASE_ADDR,DENALI_PHY_910_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_911_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(911 + PHY_BASE_ADDR,DENALI_PHY_911_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(911 + PHY_BASE_ADDR,DENALI_PHY_911_DATA,0);
  #else
         REGWR(911 + PHY_BASE_ADDR,DENALI_PHY_911_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_912_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(912 + PHY_BASE_ADDR,DENALI_PHY_912_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(912 + PHY_BASE_ADDR,DENALI_PHY_912_DATA,0);
  #else
         REGWR(912 + PHY_BASE_ADDR,DENALI_PHY_912_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_913_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(913 + PHY_BASE_ADDR,DENALI_PHY_913_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(913 + PHY_BASE_ADDR,DENALI_PHY_913_DATA,0);
  #else
         REGWR(913 + PHY_BASE_ADDR,DENALI_PHY_913_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_914_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(914 + PHY_BASE_ADDR,DENALI_PHY_914_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(914 + PHY_BASE_ADDR,DENALI_PHY_914_DATA,0);
  #else
         REGWR(914 + PHY_BASE_ADDR,DENALI_PHY_914_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_915_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(915 + PHY_BASE_ADDR,DENALI_PHY_915_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(915 + PHY_BASE_ADDR,DENALI_PHY_915_DATA,0);
  #else
         REGWR(915 + PHY_BASE_ADDR,DENALI_PHY_915_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_916_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(916 + PHY_BASE_ADDR,DENALI_PHY_916_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(916 + PHY_BASE_ADDR,DENALI_PHY_916_DATA,0);
  #else
         REGWR(916 + PHY_BASE_ADDR,DENALI_PHY_916_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_917_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(917 + PHY_BASE_ADDR,DENALI_PHY_917_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(917 + PHY_BASE_ADDR,DENALI_PHY_917_DATA,0);
  #else
         REGWR(917 + PHY_BASE_ADDR,DENALI_PHY_917_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_918_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(918 + PHY_BASE_ADDR,DENALI_PHY_918_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(918 + PHY_BASE_ADDR,DENALI_PHY_918_DATA,0);
  #else
         REGWR(918 + PHY_BASE_ADDR,DENALI_PHY_918_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_919_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(919 + PHY_BASE_ADDR,DENALI_PHY_919_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(919 + PHY_BASE_ADDR,DENALI_PHY_919_DATA,0);
  #else
         REGWR(919 + PHY_BASE_ADDR,DENALI_PHY_919_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_920_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(920 + PHY_BASE_ADDR,DENALI_PHY_920_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(920 + PHY_BASE_ADDR,DENALI_PHY_920_DATA,0);
  #else
         REGWR(920 + PHY_BASE_ADDR,DENALI_PHY_920_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_921_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(921 + PHY_BASE_ADDR,DENALI_PHY_921_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(921 + PHY_BASE_ADDR,DENALI_PHY_921_DATA,0);
  #else
         REGWR(921 + PHY_BASE_ADDR,DENALI_PHY_921_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_922_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(922 + PHY_BASE_ADDR,DENALI_PHY_922_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(922 + PHY_BASE_ADDR,DENALI_PHY_922_DATA,0);
  #else
         REGWR(922 + PHY_BASE_ADDR,DENALI_PHY_922_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_923_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(923 + PHY_BASE_ADDR,DENALI_PHY_923_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(923 + PHY_BASE_ADDR,DENALI_PHY_923_DATA,0);
  #else
         REGWR(923 + PHY_BASE_ADDR,DENALI_PHY_923_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_924_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(924 + PHY_BASE_ADDR,DENALI_PHY_924_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(924 + PHY_BASE_ADDR,DENALI_PHY_924_DATA,0);
  #else
         REGWR(924 + PHY_BASE_ADDR,DENALI_PHY_924_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_925_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(925 + PHY_BASE_ADDR,DENALI_PHY_925_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(925 + PHY_BASE_ADDR,DENALI_PHY_925_DATA,0);
  #else
         REGWR(925 + PHY_BASE_ADDR,DENALI_PHY_925_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_926_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(926 + PHY_BASE_ADDR,DENALI_PHY_926_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(926 + PHY_BASE_ADDR,DENALI_PHY_926_DATA,0);
  #else
         REGWR(926 + PHY_BASE_ADDR,DENALI_PHY_926_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_927_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(927 + PHY_BASE_ADDR,DENALI_PHY_927_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(927 + PHY_BASE_ADDR,DENALI_PHY_927_DATA,0);
  #else
         REGWR(927 + PHY_BASE_ADDR,DENALI_PHY_927_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_928_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(928 + PHY_BASE_ADDR,DENALI_PHY_928_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(928 + PHY_BASE_ADDR,DENALI_PHY_928_DATA,0);
  #else
         REGWR(928 + PHY_BASE_ADDR,DENALI_PHY_928_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_929_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(929 + PHY_BASE_ADDR,DENALI_PHY_929_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(929 + PHY_BASE_ADDR,DENALI_PHY_929_DATA,0);
  #else
         REGWR(929 + PHY_BASE_ADDR,DENALI_PHY_929_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_930_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(930 + PHY_BASE_ADDR,DENALI_PHY_930_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(930 + PHY_BASE_ADDR,DENALI_PHY_930_DATA,0);
  #else
         REGWR(930 + PHY_BASE_ADDR,DENALI_PHY_930_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_931_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(931 + PHY_BASE_ADDR,DENALI_PHY_931_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(931 + PHY_BASE_ADDR,DENALI_PHY_931_DATA,0);
  #else
         REGWR(931 + PHY_BASE_ADDR,DENALI_PHY_931_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_932_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(932 + PHY_BASE_ADDR,DENALI_PHY_932_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(932 + PHY_BASE_ADDR,DENALI_PHY_932_DATA,0);
  #else
         REGWR(932 + PHY_BASE_ADDR,DENALI_PHY_932_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_933_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(933 + PHY_BASE_ADDR,DENALI_PHY_933_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(933 + PHY_BASE_ADDR,DENALI_PHY_933_DATA,0);
  #else
         REGWR(933 + PHY_BASE_ADDR,DENALI_PHY_933_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_934_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(934 + PHY_BASE_ADDR,DENALI_PHY_934_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(934 + PHY_BASE_ADDR,DENALI_PHY_934_DATA,0);
  #else
         REGWR(934 + PHY_BASE_ADDR,DENALI_PHY_934_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_935_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(935 + PHY_BASE_ADDR,DENALI_PHY_935_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(935 + PHY_BASE_ADDR,DENALI_PHY_935_DATA,0);
  #else
         REGWR(935 + PHY_BASE_ADDR,DENALI_PHY_935_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_936_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(936 + PHY_BASE_ADDR,DENALI_PHY_936_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(936 + PHY_BASE_ADDR,DENALI_PHY_936_DATA,0);
  #else
         REGWR(936 + PHY_BASE_ADDR,DENALI_PHY_936_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_937_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(937 + PHY_BASE_ADDR,DENALI_PHY_937_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(937 + PHY_BASE_ADDR,DENALI_PHY_937_DATA,0);
  #else
         REGWR(937 + PHY_BASE_ADDR,DENALI_PHY_937_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_938_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(938 + PHY_BASE_ADDR,DENALI_PHY_938_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(938 + PHY_BASE_ADDR,DENALI_PHY_938_DATA,0);
  #else
         REGWR(938 + PHY_BASE_ADDR,DENALI_PHY_938_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_939_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(939 + PHY_BASE_ADDR,DENALI_PHY_939_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(939 + PHY_BASE_ADDR,DENALI_PHY_939_DATA,0);
  #else
         REGWR(939 + PHY_BASE_ADDR,DENALI_PHY_939_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_940_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(940 + PHY_BASE_ADDR,DENALI_PHY_940_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(940 + PHY_BASE_ADDR,DENALI_PHY_940_DATA,0);
  #else
         REGWR(940 + PHY_BASE_ADDR,DENALI_PHY_940_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_941_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(941 + PHY_BASE_ADDR,DENALI_PHY_941_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(941 + PHY_BASE_ADDR,DENALI_PHY_941_DATA,0);
  #else
         REGWR(941 + PHY_BASE_ADDR,DENALI_PHY_941_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_942_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(942 + PHY_BASE_ADDR,DENALI_PHY_942_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(942 + PHY_BASE_ADDR,DENALI_PHY_942_DATA,0);
  #else
         REGWR(942 + PHY_BASE_ADDR,DENALI_PHY_942_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_943_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(943 + PHY_BASE_ADDR,DENALI_PHY_943_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(943 + PHY_BASE_ADDR,DENALI_PHY_943_DATA,0);
  #else
         REGWR(943 + PHY_BASE_ADDR,DENALI_PHY_943_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_944_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(944 + PHY_BASE_ADDR,DENALI_PHY_944_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(944 + PHY_BASE_ADDR,DENALI_PHY_944_DATA,0);
  #else
         REGWR(944 + PHY_BASE_ADDR,DENALI_PHY_944_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_945_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(945 + PHY_BASE_ADDR,DENALI_PHY_945_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(945 + PHY_BASE_ADDR,DENALI_PHY_945_DATA,0);
  #else
         REGWR(945 + PHY_BASE_ADDR,DENALI_PHY_945_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_946_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(946 + PHY_BASE_ADDR,DENALI_PHY_946_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(946 + PHY_BASE_ADDR,DENALI_PHY_946_DATA,0);
  #else
         REGWR(946 + PHY_BASE_ADDR,DENALI_PHY_946_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_947_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(947 + PHY_BASE_ADDR,DENALI_PHY_947_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(947 + PHY_BASE_ADDR,DENALI_PHY_947_DATA,0);
  #else
         REGWR(947 + PHY_BASE_ADDR,DENALI_PHY_947_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_948_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(948 + PHY_BASE_ADDR,DENALI_PHY_948_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(948 + PHY_BASE_ADDR,DENALI_PHY_948_DATA,0);
  #else
         REGWR(948 + PHY_BASE_ADDR,DENALI_PHY_948_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_949_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(949 + PHY_BASE_ADDR,DENALI_PHY_949_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(949 + PHY_BASE_ADDR,DENALI_PHY_949_DATA,0);
  #else
         REGWR(949 + PHY_BASE_ADDR,DENALI_PHY_949_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_950_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(950 + PHY_BASE_ADDR,DENALI_PHY_950_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(950 + PHY_BASE_ADDR,DENALI_PHY_950_DATA,0);
  #else
         REGWR(950 + PHY_BASE_ADDR,DENALI_PHY_950_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_951_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(951 + PHY_BASE_ADDR,DENALI_PHY_951_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(951 + PHY_BASE_ADDR,DENALI_PHY_951_DATA,0);
  #else
         REGWR(951 + PHY_BASE_ADDR,DENALI_PHY_951_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_952_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(952 + PHY_BASE_ADDR,DENALI_PHY_952_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(952 + PHY_BASE_ADDR,DENALI_PHY_952_DATA,0);
  #else
         REGWR(952 + PHY_BASE_ADDR,DENALI_PHY_952_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_953_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(953 + PHY_BASE_ADDR,DENALI_PHY_953_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(953 + PHY_BASE_ADDR,DENALI_PHY_953_DATA,0);
  #else
         REGWR(953 + PHY_BASE_ADDR,DENALI_PHY_953_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_954_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(954 + PHY_BASE_ADDR,DENALI_PHY_954_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(954 + PHY_BASE_ADDR,DENALI_PHY_954_DATA,0);
  #else
         REGWR(954 + PHY_BASE_ADDR,DENALI_PHY_954_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_955_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(955 + PHY_BASE_ADDR,DENALI_PHY_955_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(955 + PHY_BASE_ADDR,DENALI_PHY_955_DATA,0);
  #else
         REGWR(955 + PHY_BASE_ADDR,DENALI_PHY_955_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_956_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(956 + PHY_BASE_ADDR,DENALI_PHY_956_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(956 + PHY_BASE_ADDR,DENALI_PHY_956_DATA,0);
  #else
         REGWR(956 + PHY_BASE_ADDR,DENALI_PHY_956_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_957_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(957 + PHY_BASE_ADDR,DENALI_PHY_957_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(957 + PHY_BASE_ADDR,DENALI_PHY_957_DATA,0);
  #else
         REGWR(957 + PHY_BASE_ADDR,DENALI_PHY_957_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_958_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(958 + PHY_BASE_ADDR,DENALI_PHY_958_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(958 + PHY_BASE_ADDR,DENALI_PHY_958_DATA,0);
  #else
         REGWR(958 + PHY_BASE_ADDR,DENALI_PHY_958_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_959_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(959 + PHY_BASE_ADDR,DENALI_PHY_959_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(959 + PHY_BASE_ADDR,DENALI_PHY_959_DATA,0);
  #else
         REGWR(959 + PHY_BASE_ADDR,DENALI_PHY_959_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_960_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(960 + PHY_BASE_ADDR,DENALI_PHY_960_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(960 + PHY_BASE_ADDR,DENALI_PHY_960_DATA,0);
  #else
         REGWR(960 + PHY_BASE_ADDR,DENALI_PHY_960_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_961_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(961 + PHY_BASE_ADDR,DENALI_PHY_961_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(961 + PHY_BASE_ADDR,DENALI_PHY_961_DATA,0);
  #else
         REGWR(961 + PHY_BASE_ADDR,DENALI_PHY_961_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_962_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(962 + PHY_BASE_ADDR,DENALI_PHY_962_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(962 + PHY_BASE_ADDR,DENALI_PHY_962_DATA,0);
  #else
         REGWR(962 + PHY_BASE_ADDR,DENALI_PHY_962_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_963_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(963 + PHY_BASE_ADDR,DENALI_PHY_963_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(963 + PHY_BASE_ADDR,DENALI_PHY_963_DATA,0);
  #else
         REGWR(963 + PHY_BASE_ADDR,DENALI_PHY_963_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_964_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(964 + PHY_BASE_ADDR,DENALI_PHY_964_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(964 + PHY_BASE_ADDR,DENALI_PHY_964_DATA,0);
  #else
         REGWR(964 + PHY_BASE_ADDR,DENALI_PHY_964_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_965_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(965 + PHY_BASE_ADDR,DENALI_PHY_965_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(965 + PHY_BASE_ADDR,DENALI_PHY_965_DATA,0);
  #else
         REGWR(965 + PHY_BASE_ADDR,DENALI_PHY_965_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_966_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(966 + PHY_BASE_ADDR,DENALI_PHY_966_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(966 + PHY_BASE_ADDR,DENALI_PHY_966_DATA,0);
  #else
         REGWR(966 + PHY_BASE_ADDR,DENALI_PHY_966_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_967_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(967 + PHY_BASE_ADDR,DENALI_PHY_967_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(967 + PHY_BASE_ADDR,DENALI_PHY_967_DATA,0);
  #else
         REGWR(967 + PHY_BASE_ADDR,DENALI_PHY_967_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_968_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(968 + PHY_BASE_ADDR,DENALI_PHY_968_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(968 + PHY_BASE_ADDR,DENALI_PHY_968_DATA,0);
  #else
         REGWR(968 + PHY_BASE_ADDR,DENALI_PHY_968_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_969_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(969 + PHY_BASE_ADDR,DENALI_PHY_969_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(969 + PHY_BASE_ADDR,DENALI_PHY_969_DATA,0);
  #else
         REGWR(969 + PHY_BASE_ADDR,DENALI_PHY_969_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_970_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(970 + PHY_BASE_ADDR,DENALI_PHY_970_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(970 + PHY_BASE_ADDR,DENALI_PHY_970_DATA,0);
  #else
         REGWR(970 + PHY_BASE_ADDR,DENALI_PHY_970_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_971_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(971 + PHY_BASE_ADDR,DENALI_PHY_971_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(971 + PHY_BASE_ADDR,DENALI_PHY_971_DATA,0);
  #else
         REGWR(971 + PHY_BASE_ADDR,DENALI_PHY_971_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_972_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(972 + PHY_BASE_ADDR,DENALI_PHY_972_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(972 + PHY_BASE_ADDR,DENALI_PHY_972_DATA,0);
  #else
         REGWR(972 + PHY_BASE_ADDR,DENALI_PHY_972_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_973_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(973 + PHY_BASE_ADDR,DENALI_PHY_973_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(973 + PHY_BASE_ADDR,DENALI_PHY_973_DATA,0);
  #else
         REGWR(973 + PHY_BASE_ADDR,DENALI_PHY_973_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_974_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(974 + PHY_BASE_ADDR,DENALI_PHY_974_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(974 + PHY_BASE_ADDR,DENALI_PHY_974_DATA,0);
  #else
         REGWR(974 + PHY_BASE_ADDR,DENALI_PHY_974_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_975_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(975 + PHY_BASE_ADDR,DENALI_PHY_975_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(975 + PHY_BASE_ADDR,DENALI_PHY_975_DATA,0);
  #else
         REGWR(975 + PHY_BASE_ADDR,DENALI_PHY_975_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_976_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(976 + PHY_BASE_ADDR,DENALI_PHY_976_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(976 + PHY_BASE_ADDR,DENALI_PHY_976_DATA,0);
  #else
         REGWR(976 + PHY_BASE_ADDR,DENALI_PHY_976_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_977_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(977 + PHY_BASE_ADDR,DENALI_PHY_977_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(977 + PHY_BASE_ADDR,DENALI_PHY_977_DATA,0);
  #else
         REGWR(977 + PHY_BASE_ADDR,DENALI_PHY_977_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_978_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(978 + PHY_BASE_ADDR,DENALI_PHY_978_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(978 + PHY_BASE_ADDR,DENALI_PHY_978_DATA,0);
  #else
         REGWR(978 + PHY_BASE_ADDR,DENALI_PHY_978_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_979_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(979 + PHY_BASE_ADDR,DENALI_PHY_979_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(979 + PHY_BASE_ADDR,DENALI_PHY_979_DATA,0);
  #else
         REGWR(979 + PHY_BASE_ADDR,DENALI_PHY_979_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_980_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(980 + PHY_BASE_ADDR,DENALI_PHY_980_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(980 + PHY_BASE_ADDR,DENALI_PHY_980_DATA,0);
  #else
         REGWR(980 + PHY_BASE_ADDR,DENALI_PHY_980_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_981_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(981 + PHY_BASE_ADDR,DENALI_PHY_981_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(981 + PHY_BASE_ADDR,DENALI_PHY_981_DATA,0);
  #else
         REGWR(981 + PHY_BASE_ADDR,DENALI_PHY_981_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_982_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(982 + PHY_BASE_ADDR,DENALI_PHY_982_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(982 + PHY_BASE_ADDR,DENALI_PHY_982_DATA,0);
  #else
         REGWR(982 + PHY_BASE_ADDR,DENALI_PHY_982_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_983_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(983 + PHY_BASE_ADDR,DENALI_PHY_983_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(983 + PHY_BASE_ADDR,DENALI_PHY_983_DATA,0);
  #else
         REGWR(983 + PHY_BASE_ADDR,DENALI_PHY_983_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_984_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(984 + PHY_BASE_ADDR,DENALI_PHY_984_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(984 + PHY_BASE_ADDR,DENALI_PHY_984_DATA,0);
  #else
         REGWR(984 + PHY_BASE_ADDR,DENALI_PHY_984_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_985_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(985 + PHY_BASE_ADDR,DENALI_PHY_985_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(985 + PHY_BASE_ADDR,DENALI_PHY_985_DATA,0);
  #else
         REGWR(985 + PHY_BASE_ADDR,DENALI_PHY_985_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_986_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(986 + PHY_BASE_ADDR,DENALI_PHY_986_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(986 + PHY_BASE_ADDR,DENALI_PHY_986_DATA,0);
  #else
         REGWR(986 + PHY_BASE_ADDR,DENALI_PHY_986_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_987_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(987 + PHY_BASE_ADDR,DENALI_PHY_987_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(987 + PHY_BASE_ADDR,DENALI_PHY_987_DATA,0);
  #else
         REGWR(987 + PHY_BASE_ADDR,DENALI_PHY_987_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_988_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(988 + PHY_BASE_ADDR,DENALI_PHY_988_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(988 + PHY_BASE_ADDR,DENALI_PHY_988_DATA,0);
  #else
         REGWR(988 + PHY_BASE_ADDR,DENALI_PHY_988_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_989_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(989 + PHY_BASE_ADDR,DENALI_PHY_989_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(989 + PHY_BASE_ADDR,DENALI_PHY_989_DATA,0);
  #else
         REGWR(989 + PHY_BASE_ADDR,DENALI_PHY_989_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_990_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(990 + PHY_BASE_ADDR,DENALI_PHY_990_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(990 + PHY_BASE_ADDR,DENALI_PHY_990_DATA,0);
  #else
         REGWR(990 + PHY_BASE_ADDR,DENALI_PHY_990_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_991_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(991 + PHY_BASE_ADDR,DENALI_PHY_991_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(991 + PHY_BASE_ADDR,DENALI_PHY_991_DATA,0);
  #else
         REGWR(991 + PHY_BASE_ADDR,DENALI_PHY_991_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_992_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(992 + PHY_BASE_ADDR,DENALI_PHY_992_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(992 + PHY_BASE_ADDR,DENALI_PHY_992_DATA,0);
  #else
         REGWR(992 + PHY_BASE_ADDR,DENALI_PHY_992_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_993_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(993 + PHY_BASE_ADDR,DENALI_PHY_993_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(993 + PHY_BASE_ADDR,DENALI_PHY_993_DATA,0);
  #else
         REGWR(993 + PHY_BASE_ADDR,DENALI_PHY_993_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_994_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(994 + PHY_BASE_ADDR,DENALI_PHY_994_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(994 + PHY_BASE_ADDR,DENALI_PHY_994_DATA,0);
  #else
         REGWR(994 + PHY_BASE_ADDR,DENALI_PHY_994_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_995_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(995 + PHY_BASE_ADDR,DENALI_PHY_995_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(995 + PHY_BASE_ADDR,DENALI_PHY_995_DATA,0);
  #else
         REGWR(995 + PHY_BASE_ADDR,DENALI_PHY_995_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_996_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(996 + PHY_BASE_ADDR,DENALI_PHY_996_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(996 + PHY_BASE_ADDR,DENALI_PHY_996_DATA,0);
  #else
         REGWR(996 + PHY_BASE_ADDR,DENALI_PHY_996_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_997_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(997 + PHY_BASE_ADDR,DENALI_PHY_997_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(997 + PHY_BASE_ADDR,DENALI_PHY_997_DATA,0);
  #else
         REGWR(997 + PHY_BASE_ADDR,DENALI_PHY_997_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_998_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(998 + PHY_BASE_ADDR,DENALI_PHY_998_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(998 + PHY_BASE_ADDR,DENALI_PHY_998_DATA,0);
  #else
         REGWR(998 + PHY_BASE_ADDR,DENALI_PHY_998_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_999_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(999 + PHY_BASE_ADDR,DENALI_PHY_999_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(999 + PHY_BASE_ADDR,DENALI_PHY_999_DATA,0);
  #else
         REGWR(999 + PHY_BASE_ADDR,DENALI_PHY_999_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1000_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1000 + PHY_BASE_ADDR,DENALI_PHY_1000_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1000 + PHY_BASE_ADDR,DENALI_PHY_1000_DATA,0);
  #else
         REGWR(1000 + PHY_BASE_ADDR,DENALI_PHY_1000_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1001_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1001 + PHY_BASE_ADDR,DENALI_PHY_1001_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1001 + PHY_BASE_ADDR,DENALI_PHY_1001_DATA,0);
  #else
         REGWR(1001 + PHY_BASE_ADDR,DENALI_PHY_1001_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1002_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1002 + PHY_BASE_ADDR,DENALI_PHY_1002_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1002 + PHY_BASE_ADDR,DENALI_PHY_1002_DATA,0);
  #else
         REGWR(1002 + PHY_BASE_ADDR,DENALI_PHY_1002_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1003_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1003 + PHY_BASE_ADDR,DENALI_PHY_1003_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1003 + PHY_BASE_ADDR,DENALI_PHY_1003_DATA,0);
  #else
         REGWR(1003 + PHY_BASE_ADDR,DENALI_PHY_1003_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1004_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1004 + PHY_BASE_ADDR,DENALI_PHY_1004_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1004 + PHY_BASE_ADDR,DENALI_PHY_1004_DATA,0);
  #else
         REGWR(1004 + PHY_BASE_ADDR,DENALI_PHY_1004_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1005_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1005 + PHY_BASE_ADDR,DENALI_PHY_1005_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1005 + PHY_BASE_ADDR,DENALI_PHY_1005_DATA,0);
  #else
         REGWR(1005 + PHY_BASE_ADDR,DENALI_PHY_1005_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1006_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1006 + PHY_BASE_ADDR,DENALI_PHY_1006_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1006 + PHY_BASE_ADDR,DENALI_PHY_1006_DATA,0);
  #else
         REGWR(1006 + PHY_BASE_ADDR,DENALI_PHY_1006_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1007_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1007 + PHY_BASE_ADDR,DENALI_PHY_1007_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1007 + PHY_BASE_ADDR,DENALI_PHY_1007_DATA,0);
  #else
         REGWR(1007 + PHY_BASE_ADDR,DENALI_PHY_1007_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1008_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1008 + PHY_BASE_ADDR,DENALI_PHY_1008_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1008 + PHY_BASE_ADDR,DENALI_PHY_1008_DATA,0);
  #else
         REGWR(1008 + PHY_BASE_ADDR,DENALI_PHY_1008_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1009_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1009 + PHY_BASE_ADDR,DENALI_PHY_1009_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1009 + PHY_BASE_ADDR,DENALI_PHY_1009_DATA,0);
  #else
         REGWR(1009 + PHY_BASE_ADDR,DENALI_PHY_1009_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1010_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1010 + PHY_BASE_ADDR,DENALI_PHY_1010_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1010 + PHY_BASE_ADDR,DENALI_PHY_1010_DATA,0);
  #else
         REGWR(1010 + PHY_BASE_ADDR,DENALI_PHY_1010_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1011_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1011 + PHY_BASE_ADDR,DENALI_PHY_1011_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1011 + PHY_BASE_ADDR,DENALI_PHY_1011_DATA,0);
  #else
         REGWR(1011 + PHY_BASE_ADDR,DENALI_PHY_1011_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1012_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1012 + PHY_BASE_ADDR,DENALI_PHY_1012_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1012 + PHY_BASE_ADDR,DENALI_PHY_1012_DATA,0);
  #else
         REGWR(1012 + PHY_BASE_ADDR,DENALI_PHY_1012_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1013_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1013 + PHY_BASE_ADDR,DENALI_PHY_1013_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1013 + PHY_BASE_ADDR,DENALI_PHY_1013_DATA,0);
  #else
         REGWR(1013 + PHY_BASE_ADDR,DENALI_PHY_1013_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1014_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1014 + PHY_BASE_ADDR,DENALI_PHY_1014_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1014 + PHY_BASE_ADDR,DENALI_PHY_1014_DATA,0);
  #else
         REGWR(1014 + PHY_BASE_ADDR,DENALI_PHY_1014_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1015_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1015 + PHY_BASE_ADDR,DENALI_PHY_1015_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1015 + PHY_BASE_ADDR,DENALI_PHY_1015_DATA,0);
  #else
         REGWR(1015 + PHY_BASE_ADDR,DENALI_PHY_1015_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1016_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1016 + PHY_BASE_ADDR,DENALI_PHY_1016_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1016 + PHY_BASE_ADDR,DENALI_PHY_1016_DATA,0);
  #else
         REGWR(1016 + PHY_BASE_ADDR,DENALI_PHY_1016_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1017_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1017 + PHY_BASE_ADDR,DENALI_PHY_1017_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1017 + PHY_BASE_ADDR,DENALI_PHY_1017_DATA,0);
  #else
         REGWR(1017 + PHY_BASE_ADDR,DENALI_PHY_1017_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1018_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1018 + PHY_BASE_ADDR,DENALI_PHY_1018_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1018 + PHY_BASE_ADDR,DENALI_PHY_1018_DATA,0);
  #else
         REGWR(1018 + PHY_BASE_ADDR,DENALI_PHY_1018_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1019_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1019 + PHY_BASE_ADDR,DENALI_PHY_1019_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1019 + PHY_BASE_ADDR,DENALI_PHY_1019_DATA,0);
  #else
         REGWR(1019 + PHY_BASE_ADDR,DENALI_PHY_1019_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1020_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1020 + PHY_BASE_ADDR,DENALI_PHY_1020_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1020 + PHY_BASE_ADDR,DENALI_PHY_1020_DATA,0);
  #else
         REGWR(1020 + PHY_BASE_ADDR,DENALI_PHY_1020_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1021_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1021 + PHY_BASE_ADDR,DENALI_PHY_1021_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1021 + PHY_BASE_ADDR,DENALI_PHY_1021_DATA,0);
  #else
         REGWR(1021 + PHY_BASE_ADDR,DENALI_PHY_1021_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1022_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1022 + PHY_BASE_ADDR,DENALI_PHY_1022_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1022 + PHY_BASE_ADDR,DENALI_PHY_1022_DATA,0);
  #else
         REGWR(1022 + PHY_BASE_ADDR,DENALI_PHY_1022_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1023_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1023 + PHY_BASE_ADDR,DENALI_PHY_1023_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1023 + PHY_BASE_ADDR,DENALI_PHY_1023_DATA,0);
  #else
         REGWR(1023 + PHY_BASE_ADDR,DENALI_PHY_1023_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1024_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1024 + PHY_BASE_ADDR,DENALI_PHY_1024_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1024 + PHY_BASE_ADDR,DENALI_PHY_1024_DATA,0);
  #else
         REGWR(1024 + PHY_BASE_ADDR,DENALI_PHY_1024_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1025_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1025 + PHY_BASE_ADDR,DENALI_PHY_1025_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1025 + PHY_BASE_ADDR,DENALI_PHY_1025_DATA,0);
  #else
         REGWR(1025 + PHY_BASE_ADDR,DENALI_PHY_1025_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1026_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1026 + PHY_BASE_ADDR,DENALI_PHY_1026_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1026 + PHY_BASE_ADDR,DENALI_PHY_1026_DATA,0);
  #else
         REGWR(1026 + PHY_BASE_ADDR,DENALI_PHY_1026_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1027_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1027 + PHY_BASE_ADDR,DENALI_PHY_1027_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1027 + PHY_BASE_ADDR,DENALI_PHY_1027_DATA,0);
  #else
         REGWR(1027 + PHY_BASE_ADDR,DENALI_PHY_1027_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1028_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1028 + PHY_BASE_ADDR,DENALI_PHY_1028_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1028 + PHY_BASE_ADDR,DENALI_PHY_1028_DATA,0);
  #else
         REGWR(1028 + PHY_BASE_ADDR,DENALI_PHY_1028_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1029_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1029 + PHY_BASE_ADDR,DENALI_PHY_1029_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1029 + PHY_BASE_ADDR,DENALI_PHY_1029_DATA,0);
  #else
         REGWR(1029 + PHY_BASE_ADDR,DENALI_PHY_1029_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1030_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1030 + PHY_BASE_ADDR,DENALI_PHY_1030_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1030 + PHY_BASE_ADDR,DENALI_PHY_1030_DATA,0);
  #else
         REGWR(1030 + PHY_BASE_ADDR,DENALI_PHY_1030_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1031_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1031 + PHY_BASE_ADDR,DENALI_PHY_1031_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1031 + PHY_BASE_ADDR,DENALI_PHY_1031_DATA,0);
  #else
         REGWR(1031 + PHY_BASE_ADDR,DENALI_PHY_1031_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1032_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1032 + PHY_BASE_ADDR,DENALI_PHY_1032_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1032 + PHY_BASE_ADDR,DENALI_PHY_1032_DATA,0);
  #else
         REGWR(1032 + PHY_BASE_ADDR,DENALI_PHY_1032_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1033_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1033 + PHY_BASE_ADDR,DENALI_PHY_1033_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1033 + PHY_BASE_ADDR,DENALI_PHY_1033_DATA,0);
  #else
         REGWR(1033 + PHY_BASE_ADDR,DENALI_PHY_1033_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1034_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1034 + PHY_BASE_ADDR,DENALI_PHY_1034_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1034 + PHY_BASE_ADDR,DENALI_PHY_1034_DATA,0);
  #else
         REGWR(1034 + PHY_BASE_ADDR,DENALI_PHY_1034_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1035_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1035 + PHY_BASE_ADDR,DENALI_PHY_1035_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1035 + PHY_BASE_ADDR,DENALI_PHY_1035_DATA,0);
  #else
         REGWR(1035 + PHY_BASE_ADDR,DENALI_PHY_1035_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1036_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1036 + PHY_BASE_ADDR,DENALI_PHY_1036_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1036 + PHY_BASE_ADDR,DENALI_PHY_1036_DATA,0);
  #else
         REGWR(1036 + PHY_BASE_ADDR,DENALI_PHY_1036_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1037_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1037 + PHY_BASE_ADDR,DENALI_PHY_1037_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1037 + PHY_BASE_ADDR,DENALI_PHY_1037_DATA,0);
  #else
         REGWR(1037 + PHY_BASE_ADDR,DENALI_PHY_1037_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1038_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1038 + PHY_BASE_ADDR,DENALI_PHY_1038_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1038 + PHY_BASE_ADDR,DENALI_PHY_1038_DATA,0);
  #else
         REGWR(1038 + PHY_BASE_ADDR,DENALI_PHY_1038_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1039_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1039 + PHY_BASE_ADDR,DENALI_PHY_1039_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1039 + PHY_BASE_ADDR,DENALI_PHY_1039_DATA,0);
  #else
         REGWR(1039 + PHY_BASE_ADDR,DENALI_PHY_1039_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1040_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1040 + PHY_BASE_ADDR,DENALI_PHY_1040_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1040 + PHY_BASE_ADDR,DENALI_PHY_1040_DATA,0);
  #else
         REGWR(1040 + PHY_BASE_ADDR,DENALI_PHY_1040_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1041_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1041 + PHY_BASE_ADDR,DENALI_PHY_1041_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1041 + PHY_BASE_ADDR,DENALI_PHY_1041_DATA,0);
  #else
         REGWR(1041 + PHY_BASE_ADDR,DENALI_PHY_1041_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1042_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1042 + PHY_BASE_ADDR,DENALI_PHY_1042_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1042 + PHY_BASE_ADDR,DENALI_PHY_1042_DATA,0);
  #else
         REGWR(1042 + PHY_BASE_ADDR,DENALI_PHY_1042_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1043_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1043 + PHY_BASE_ADDR,DENALI_PHY_1043_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1043 + PHY_BASE_ADDR,DENALI_PHY_1043_DATA,0);
  #else
         REGWR(1043 + PHY_BASE_ADDR,DENALI_PHY_1043_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1044_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1044 + PHY_BASE_ADDR,DENALI_PHY_1044_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1044 + PHY_BASE_ADDR,DENALI_PHY_1044_DATA,0);
  #else
         REGWR(1044 + PHY_BASE_ADDR,DENALI_PHY_1044_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1045_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1045 + PHY_BASE_ADDR,DENALI_PHY_1045_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1045 + PHY_BASE_ADDR,DENALI_PHY_1045_DATA,0);
  #else
         REGWR(1045 + PHY_BASE_ADDR,DENALI_PHY_1045_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1046_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1046 + PHY_BASE_ADDR,DENALI_PHY_1046_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1046 + PHY_BASE_ADDR,DENALI_PHY_1046_DATA,0);
  #else
         REGWR(1046 + PHY_BASE_ADDR,DENALI_PHY_1046_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1047_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1047 + PHY_BASE_ADDR,DENALI_PHY_1047_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1047 + PHY_BASE_ADDR,DENALI_PHY_1047_DATA,0);
  #else
         REGWR(1047 + PHY_BASE_ADDR,DENALI_PHY_1047_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1048_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1048 + PHY_BASE_ADDR,DENALI_PHY_1048_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1048 + PHY_BASE_ADDR,DENALI_PHY_1048_DATA,0);
  #else
         REGWR(1048 + PHY_BASE_ADDR,DENALI_PHY_1048_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1049_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1049 + PHY_BASE_ADDR,DENALI_PHY_1049_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1049 + PHY_BASE_ADDR,DENALI_PHY_1049_DATA,0);
  #else
         REGWR(1049 + PHY_BASE_ADDR,DENALI_PHY_1049_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1050_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1050 + PHY_BASE_ADDR,DENALI_PHY_1050_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1050 + PHY_BASE_ADDR,DENALI_PHY_1050_DATA,0);
  #else
         REGWR(1050 + PHY_BASE_ADDR,DENALI_PHY_1050_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1051_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1051 + PHY_BASE_ADDR,DENALI_PHY_1051_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1051 + PHY_BASE_ADDR,DENALI_PHY_1051_DATA,0);
  #else
         REGWR(1051 + PHY_BASE_ADDR,DENALI_PHY_1051_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1052_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1052 + PHY_BASE_ADDR,DENALI_PHY_1052_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1052 + PHY_BASE_ADDR,DENALI_PHY_1052_DATA,0);
  #else
         REGWR(1052 + PHY_BASE_ADDR,DENALI_PHY_1052_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1053_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1053 + PHY_BASE_ADDR,DENALI_PHY_1053_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1053 + PHY_BASE_ADDR,DENALI_PHY_1053_DATA,0);
  #else
         REGWR(1053 + PHY_BASE_ADDR,DENALI_PHY_1053_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1054_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1054 + PHY_BASE_ADDR,DENALI_PHY_1054_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1054 + PHY_BASE_ADDR,DENALI_PHY_1054_DATA,0);
  #else
         REGWR(1054 + PHY_BASE_ADDR,DENALI_PHY_1054_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1055_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1055 + PHY_BASE_ADDR,DENALI_PHY_1055_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1055 + PHY_BASE_ADDR,DENALI_PHY_1055_DATA,0);
  #else
         REGWR(1055 + PHY_BASE_ADDR,DENALI_PHY_1055_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1056_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1056 + PHY_BASE_ADDR,DENALI_PHY_1056_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1056 + PHY_BASE_ADDR,DENALI_PHY_1056_DATA,0);
  #else
         REGWR(1056 + PHY_BASE_ADDR,DENALI_PHY_1056_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1057_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1057 + PHY_BASE_ADDR,DENALI_PHY_1057_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1057 + PHY_BASE_ADDR,DENALI_PHY_1057_DATA,0);
  #else
         REGWR(1057 + PHY_BASE_ADDR,DENALI_PHY_1057_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1058_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1058 + PHY_BASE_ADDR,DENALI_PHY_1058_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1058 + PHY_BASE_ADDR,DENALI_PHY_1058_DATA,0);
  #else
         REGWR(1058 + PHY_BASE_ADDR,DENALI_PHY_1058_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1059_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1059 + PHY_BASE_ADDR,DENALI_PHY_1059_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1059 + PHY_BASE_ADDR,DENALI_PHY_1059_DATA,0);
  #else
         REGWR(1059 + PHY_BASE_ADDR,DENALI_PHY_1059_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1060_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1060 + PHY_BASE_ADDR,DENALI_PHY_1060_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1060 + PHY_BASE_ADDR,DENALI_PHY_1060_DATA,0);
  #else
         REGWR(1060 + PHY_BASE_ADDR,DENALI_PHY_1060_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1061_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1061 + PHY_BASE_ADDR,DENALI_PHY_1061_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1061 + PHY_BASE_ADDR,DENALI_PHY_1061_DATA,0);
  #else
         REGWR(1061 + PHY_BASE_ADDR,DENALI_PHY_1061_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1062_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1062 + PHY_BASE_ADDR,DENALI_PHY_1062_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1062 + PHY_BASE_ADDR,DENALI_PHY_1062_DATA,0);
  #else
         REGWR(1062 + PHY_BASE_ADDR,DENALI_PHY_1062_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1063_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1063 + PHY_BASE_ADDR,DENALI_PHY_1063_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1063 + PHY_BASE_ADDR,DENALI_PHY_1063_DATA,0);
  #else
         REGWR(1063 + PHY_BASE_ADDR,DENALI_PHY_1063_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1064_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1064 + PHY_BASE_ADDR,DENALI_PHY_1064_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1064 + PHY_BASE_ADDR,DENALI_PHY_1064_DATA,0);
  #else
         REGWR(1064 + PHY_BASE_ADDR,DENALI_PHY_1064_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1065_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1065 + PHY_BASE_ADDR,DENALI_PHY_1065_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1065 + PHY_BASE_ADDR,DENALI_PHY_1065_DATA,0);
  #else
         REGWR(1065 + PHY_BASE_ADDR,DENALI_PHY_1065_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1066_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1066 + PHY_BASE_ADDR,DENALI_PHY_1066_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1066 + PHY_BASE_ADDR,DENALI_PHY_1066_DATA,0);
  #else
         REGWR(1066 + PHY_BASE_ADDR,DENALI_PHY_1066_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1067_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1067 + PHY_BASE_ADDR,DENALI_PHY_1067_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1067 + PHY_BASE_ADDR,DENALI_PHY_1067_DATA,0);
  #else
         REGWR(1067 + PHY_BASE_ADDR,DENALI_PHY_1067_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1068_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1068 + PHY_BASE_ADDR,DENALI_PHY_1068_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1068 + PHY_BASE_ADDR,DENALI_PHY_1068_DATA,0);
  #else
         REGWR(1068 + PHY_BASE_ADDR,DENALI_PHY_1068_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1069_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1069 + PHY_BASE_ADDR,DENALI_PHY_1069_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1069 + PHY_BASE_ADDR,DENALI_PHY_1069_DATA,0);
  #else
         REGWR(1069 + PHY_BASE_ADDR,DENALI_PHY_1069_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1070_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1070 + PHY_BASE_ADDR,DENALI_PHY_1070_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1070 + PHY_BASE_ADDR,DENALI_PHY_1070_DATA,0);
  #else
         REGWR(1070 + PHY_BASE_ADDR,DENALI_PHY_1070_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1071_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1071 + PHY_BASE_ADDR,DENALI_PHY_1071_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1071 + PHY_BASE_ADDR,DENALI_PHY_1071_DATA,0);
  #else
         REGWR(1071 + PHY_BASE_ADDR,DENALI_PHY_1071_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1072_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1072 + PHY_BASE_ADDR,DENALI_PHY_1072_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1072 + PHY_BASE_ADDR,DENALI_PHY_1072_DATA,0);
  #else
         REGWR(1072 + PHY_BASE_ADDR,DENALI_PHY_1072_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1073_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1073 + PHY_BASE_ADDR,DENALI_PHY_1073_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1073 + PHY_BASE_ADDR,DENALI_PHY_1073_DATA,0);
  #else
         REGWR(1073 + PHY_BASE_ADDR,DENALI_PHY_1073_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1074_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1074 + PHY_BASE_ADDR,DENALI_PHY_1074_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1074 + PHY_BASE_ADDR,DENALI_PHY_1074_DATA,0);
  #else
         REGWR(1074 + PHY_BASE_ADDR,DENALI_PHY_1074_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1075_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1075 + PHY_BASE_ADDR,DENALI_PHY_1075_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1075 + PHY_BASE_ADDR,DENALI_PHY_1075_DATA,0);
  #else
         REGWR(1075 + PHY_BASE_ADDR,DENALI_PHY_1075_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1076_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1076 + PHY_BASE_ADDR,DENALI_PHY_1076_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1076 + PHY_BASE_ADDR,DENALI_PHY_1076_DATA,0);
  #else
         REGWR(1076 + PHY_BASE_ADDR,DENALI_PHY_1076_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1077_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1077 + PHY_BASE_ADDR,DENALI_PHY_1077_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1077 + PHY_BASE_ADDR,DENALI_PHY_1077_DATA,0);
  #else
         REGWR(1077 + PHY_BASE_ADDR,DENALI_PHY_1077_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1078_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1078 + PHY_BASE_ADDR,DENALI_PHY_1078_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1078 + PHY_BASE_ADDR,DENALI_PHY_1078_DATA,0);
  #else
         REGWR(1078 + PHY_BASE_ADDR,DENALI_PHY_1078_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1079_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1079 + PHY_BASE_ADDR,DENALI_PHY_1079_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1079 + PHY_BASE_ADDR,DENALI_PHY_1079_DATA,0);
  #else
         REGWR(1079 + PHY_BASE_ADDR,DENALI_PHY_1079_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1080_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1080 + PHY_BASE_ADDR,DENALI_PHY_1080_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1080 + PHY_BASE_ADDR,DENALI_PHY_1080_DATA,0);
  #else
         REGWR(1080 + PHY_BASE_ADDR,DENALI_PHY_1080_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1081_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1081 + PHY_BASE_ADDR,DENALI_PHY_1081_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1081 + PHY_BASE_ADDR,DENALI_PHY_1081_DATA,0);
  #else
         REGWR(1081 + PHY_BASE_ADDR,DENALI_PHY_1081_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1082_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1082 + PHY_BASE_ADDR,DENALI_PHY_1082_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1082 + PHY_BASE_ADDR,DENALI_PHY_1082_DATA,0);
  #else
         REGWR(1082 + PHY_BASE_ADDR,DENALI_PHY_1082_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1083_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1083 + PHY_BASE_ADDR,DENALI_PHY_1083_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1083 + PHY_BASE_ADDR,DENALI_PHY_1083_DATA,0);
  #else
         REGWR(1083 + PHY_BASE_ADDR,DENALI_PHY_1083_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1084_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1084 + PHY_BASE_ADDR,DENALI_PHY_1084_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1084 + PHY_BASE_ADDR,DENALI_PHY_1084_DATA,0);
  #else
         REGWR(1084 + PHY_BASE_ADDR,DENALI_PHY_1084_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1085_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1085 + PHY_BASE_ADDR,DENALI_PHY_1085_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1085 + PHY_BASE_ADDR,DENALI_PHY_1085_DATA,0);
  #else
         REGWR(1085 + PHY_BASE_ADDR,DENALI_PHY_1085_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1086_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1086 + PHY_BASE_ADDR,DENALI_PHY_1086_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1086 + PHY_BASE_ADDR,DENALI_PHY_1086_DATA,0);
  #else
         REGWR(1086 + PHY_BASE_ADDR,DENALI_PHY_1086_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1087_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1087 + PHY_BASE_ADDR,DENALI_PHY_1087_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1087 + PHY_BASE_ADDR,DENALI_PHY_1087_DATA,0);
  #else
         REGWR(1087 + PHY_BASE_ADDR,DENALI_PHY_1087_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1088_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1088 + PHY_BASE_ADDR,DENALI_PHY_1088_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1088 + PHY_BASE_ADDR,DENALI_PHY_1088_DATA,0);
  #else
         REGWR(1088 + PHY_BASE_ADDR,DENALI_PHY_1088_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1089_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1089 + PHY_BASE_ADDR,DENALI_PHY_1089_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1089 + PHY_BASE_ADDR,DENALI_PHY_1089_DATA,0);
  #else
         REGWR(1089 + PHY_BASE_ADDR,DENALI_PHY_1089_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1090_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1090 + PHY_BASE_ADDR,DENALI_PHY_1090_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1090 + PHY_BASE_ADDR,DENALI_PHY_1090_DATA,0);
  #else
         REGWR(1090 + PHY_BASE_ADDR,DENALI_PHY_1090_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1091_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1091 + PHY_BASE_ADDR,DENALI_PHY_1091_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1091 + PHY_BASE_ADDR,DENALI_PHY_1091_DATA,0);
  #else
         REGWR(1091 + PHY_BASE_ADDR,DENALI_PHY_1091_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1092_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1092 + PHY_BASE_ADDR,DENALI_PHY_1092_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1092 + PHY_BASE_ADDR,DENALI_PHY_1092_DATA,0);
  #else
         REGWR(1092 + PHY_BASE_ADDR,DENALI_PHY_1092_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1093_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1093 + PHY_BASE_ADDR,DENALI_PHY_1093_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1093 + PHY_BASE_ADDR,DENALI_PHY_1093_DATA,0);
  #else
         REGWR(1093 + PHY_BASE_ADDR,DENALI_PHY_1093_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1094_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1094 + PHY_BASE_ADDR,DENALI_PHY_1094_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1094 + PHY_BASE_ADDR,DENALI_PHY_1094_DATA,0);
  #else
         REGWR(1094 + PHY_BASE_ADDR,DENALI_PHY_1094_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1095_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1095 + PHY_BASE_ADDR,DENALI_PHY_1095_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1095 + PHY_BASE_ADDR,DENALI_PHY_1095_DATA,0);
  #else
         REGWR(1095 + PHY_BASE_ADDR,DENALI_PHY_1095_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1096_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1096 + PHY_BASE_ADDR,DENALI_PHY_1096_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1096 + PHY_BASE_ADDR,DENALI_PHY_1096_DATA,0);
  #else
         REGWR(1096 + PHY_BASE_ADDR,DENALI_PHY_1096_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1097_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1097 + PHY_BASE_ADDR,DENALI_PHY_1097_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1097 + PHY_BASE_ADDR,DENALI_PHY_1097_DATA,0);
  #else
         REGWR(1097 + PHY_BASE_ADDR,DENALI_PHY_1097_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1098_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1098 + PHY_BASE_ADDR,DENALI_PHY_1098_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1098 + PHY_BASE_ADDR,DENALI_PHY_1098_DATA,0);
  #else
         REGWR(1098 + PHY_BASE_ADDR,DENALI_PHY_1098_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1099_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1099 + PHY_BASE_ADDR,DENALI_PHY_1099_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1099 + PHY_BASE_ADDR,DENALI_PHY_1099_DATA,0);
  #else
         REGWR(1099 + PHY_BASE_ADDR,DENALI_PHY_1099_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1100_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1100 + PHY_BASE_ADDR,DENALI_PHY_1100_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1100 + PHY_BASE_ADDR,DENALI_PHY_1100_DATA,0);
  #else
         REGWR(1100 + PHY_BASE_ADDR,DENALI_PHY_1100_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1101_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1101 + PHY_BASE_ADDR,DENALI_PHY_1101_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1101 + PHY_BASE_ADDR,DENALI_PHY_1101_DATA,0);
  #else
         REGWR(1101 + PHY_BASE_ADDR,DENALI_PHY_1101_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1102_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1102 + PHY_BASE_ADDR,DENALI_PHY_1102_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1102 + PHY_BASE_ADDR,DENALI_PHY_1102_DATA,0);
  #else
         REGWR(1102 + PHY_BASE_ADDR,DENALI_PHY_1102_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1103_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1103 + PHY_BASE_ADDR,DENALI_PHY_1103_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1103 + PHY_BASE_ADDR,DENALI_PHY_1103_DATA,0);
  #else
         REGWR(1103 + PHY_BASE_ADDR,DENALI_PHY_1103_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1104_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1104 + PHY_BASE_ADDR,DENALI_PHY_1104_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1104 + PHY_BASE_ADDR,DENALI_PHY_1104_DATA,0);
  #else
         REGWR(1104 + PHY_BASE_ADDR,DENALI_PHY_1104_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1105_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1105 + PHY_BASE_ADDR,DENALI_PHY_1105_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1105 + PHY_BASE_ADDR,DENALI_PHY_1105_DATA,0);
  #else
         REGWR(1105 + PHY_BASE_ADDR,DENALI_PHY_1105_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1106_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1106 + PHY_BASE_ADDR,DENALI_PHY_1106_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1106 + PHY_BASE_ADDR,DENALI_PHY_1106_DATA,0);
  #else
         REGWR(1106 + PHY_BASE_ADDR,DENALI_PHY_1106_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1107_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1107 + PHY_BASE_ADDR,DENALI_PHY_1107_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1107 + PHY_BASE_ADDR,DENALI_PHY_1107_DATA,0);
  #else
         REGWR(1107 + PHY_BASE_ADDR,DENALI_PHY_1107_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1108_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1108 + PHY_BASE_ADDR,DENALI_PHY_1108_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1108 + PHY_BASE_ADDR,DENALI_PHY_1108_DATA,0);
  #else
         REGWR(1108 + PHY_BASE_ADDR,DENALI_PHY_1108_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1109_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1109 + PHY_BASE_ADDR,DENALI_PHY_1109_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1109 + PHY_BASE_ADDR,DENALI_PHY_1109_DATA,0);
  #else
         REGWR(1109 + PHY_BASE_ADDR,DENALI_PHY_1109_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1110_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1110 + PHY_BASE_ADDR,DENALI_PHY_1110_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1110 + PHY_BASE_ADDR,DENALI_PHY_1110_DATA,0);
  #else
         REGWR(1110 + PHY_BASE_ADDR,DENALI_PHY_1110_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1111_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1111 + PHY_BASE_ADDR,DENALI_PHY_1111_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1111 + PHY_BASE_ADDR,DENALI_PHY_1111_DATA,0);
  #else
         REGWR(1111 + PHY_BASE_ADDR,DENALI_PHY_1111_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1112_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1112 + PHY_BASE_ADDR,DENALI_PHY_1112_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1112 + PHY_BASE_ADDR,DENALI_PHY_1112_DATA,0);
  #else
         REGWR(1112 + PHY_BASE_ADDR,DENALI_PHY_1112_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1113_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1113 + PHY_BASE_ADDR,DENALI_PHY_1113_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1113 + PHY_BASE_ADDR,DENALI_PHY_1113_DATA,0);
  #else
         REGWR(1113 + PHY_BASE_ADDR,DENALI_PHY_1113_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1114_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1114 + PHY_BASE_ADDR,DENALI_PHY_1114_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1114 + PHY_BASE_ADDR,DENALI_PHY_1114_DATA,0);
  #else
         REGWR(1114 + PHY_BASE_ADDR,DENALI_PHY_1114_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1115_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1115 + PHY_BASE_ADDR,DENALI_PHY_1115_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1115 + PHY_BASE_ADDR,DENALI_PHY_1115_DATA,0);
  #else
         REGWR(1115 + PHY_BASE_ADDR,DENALI_PHY_1115_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1116_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1116 + PHY_BASE_ADDR,DENALI_PHY_1116_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1116 + PHY_BASE_ADDR,DENALI_PHY_1116_DATA,0);
  #else
         REGWR(1116 + PHY_BASE_ADDR,DENALI_PHY_1116_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1117_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1117 + PHY_BASE_ADDR,DENALI_PHY_1117_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1117 + PHY_BASE_ADDR,DENALI_PHY_1117_DATA,0);
  #else
         REGWR(1117 + PHY_BASE_ADDR,DENALI_PHY_1117_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1118_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1118 + PHY_BASE_ADDR,DENALI_PHY_1118_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1118 + PHY_BASE_ADDR,DENALI_PHY_1118_DATA,0);
  #else
         REGWR(1118 + PHY_BASE_ADDR,DENALI_PHY_1118_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1119_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1119 + PHY_BASE_ADDR,DENALI_PHY_1119_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1119 + PHY_BASE_ADDR,DENALI_PHY_1119_DATA,0);
  #else
         REGWR(1119 + PHY_BASE_ADDR,DENALI_PHY_1119_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1120_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1120 + PHY_BASE_ADDR,DENALI_PHY_1120_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1120 + PHY_BASE_ADDR,DENALI_PHY_1120_DATA,0);
  #else
         REGWR(1120 + PHY_BASE_ADDR,DENALI_PHY_1120_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1121_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1121 + PHY_BASE_ADDR,DENALI_PHY_1121_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1121 + PHY_BASE_ADDR,DENALI_PHY_1121_DATA,0);
  #else
         REGWR(1121 + PHY_BASE_ADDR,DENALI_PHY_1121_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1122_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1122 + PHY_BASE_ADDR,DENALI_PHY_1122_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1122 + PHY_BASE_ADDR,DENALI_PHY_1122_DATA,0);
  #else
         REGWR(1122 + PHY_BASE_ADDR,DENALI_PHY_1122_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1123_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1123 + PHY_BASE_ADDR,DENALI_PHY_1123_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1123 + PHY_BASE_ADDR,DENALI_PHY_1123_DATA,0);
  #else
         REGWR(1123 + PHY_BASE_ADDR,DENALI_PHY_1123_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1124_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1124 + PHY_BASE_ADDR,DENALI_PHY_1124_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1124 + PHY_BASE_ADDR,DENALI_PHY_1124_DATA,0);
  #else
         REGWR(1124 + PHY_BASE_ADDR,DENALI_PHY_1124_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1125_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1125 + PHY_BASE_ADDR,DENALI_PHY_1125_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1125 + PHY_BASE_ADDR,DENALI_PHY_1125_DATA,0);
  #else
         REGWR(1125 + PHY_BASE_ADDR,DENALI_PHY_1125_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1126_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1126 + PHY_BASE_ADDR,DENALI_PHY_1126_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1126 + PHY_BASE_ADDR,DENALI_PHY_1126_DATA,0);
  #else
         REGWR(1126 + PHY_BASE_ADDR,DENALI_PHY_1126_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1127_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1127 + PHY_BASE_ADDR,DENALI_PHY_1127_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1127 + PHY_BASE_ADDR,DENALI_PHY_1127_DATA,0);
  #else
         REGWR(1127 + PHY_BASE_ADDR,DENALI_PHY_1127_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1128_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1128 + PHY_BASE_ADDR,DENALI_PHY_1128_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1128 + PHY_BASE_ADDR,DENALI_PHY_1128_DATA,0);
  #else
         REGWR(1128 + PHY_BASE_ADDR,DENALI_PHY_1128_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1129_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1129 + PHY_BASE_ADDR,DENALI_PHY_1129_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1129 + PHY_BASE_ADDR,DENALI_PHY_1129_DATA,0);
  #else
         REGWR(1129 + PHY_BASE_ADDR,DENALI_PHY_1129_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1130_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1130 + PHY_BASE_ADDR,DENALI_PHY_1130_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1130 + PHY_BASE_ADDR,DENALI_PHY_1130_DATA,0);
  #else
         REGWR(1130 + PHY_BASE_ADDR,DENALI_PHY_1130_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1131_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1131 + PHY_BASE_ADDR,DENALI_PHY_1131_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1131 + PHY_BASE_ADDR,DENALI_PHY_1131_DATA,0);
  #else
         REGWR(1131 + PHY_BASE_ADDR,DENALI_PHY_1131_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1132_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1132 + PHY_BASE_ADDR,DENALI_PHY_1132_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1132 + PHY_BASE_ADDR,DENALI_PHY_1132_DATA,0);
  #else
         REGWR(1132 + PHY_BASE_ADDR,DENALI_PHY_1132_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1133_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1133 + PHY_BASE_ADDR,DENALI_PHY_1133_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1133 + PHY_BASE_ADDR,DENALI_PHY_1133_DATA,0);
  #else
         REGWR(1133 + PHY_BASE_ADDR,DENALI_PHY_1133_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1134_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1134 + PHY_BASE_ADDR,DENALI_PHY_1134_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1134 + PHY_BASE_ADDR,DENALI_PHY_1134_DATA,0);
  #else
         REGWR(1134 + PHY_BASE_ADDR,DENALI_PHY_1134_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1135_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1135 + PHY_BASE_ADDR,DENALI_PHY_1135_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1135 + PHY_BASE_ADDR,DENALI_PHY_1135_DATA,0);
  #else
         REGWR(1135 + PHY_BASE_ADDR,DENALI_PHY_1135_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1136_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1136 + PHY_BASE_ADDR,DENALI_PHY_1136_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1136 + PHY_BASE_ADDR,DENALI_PHY_1136_DATA,0);
  #else
         REGWR(1136 + PHY_BASE_ADDR,DENALI_PHY_1136_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1137_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1137 + PHY_BASE_ADDR,DENALI_PHY_1137_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1137 + PHY_BASE_ADDR,DENALI_PHY_1137_DATA,0);
  #else
         REGWR(1137 + PHY_BASE_ADDR,DENALI_PHY_1137_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1138_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1138 + PHY_BASE_ADDR,DENALI_PHY_1138_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1138 + PHY_BASE_ADDR,DENALI_PHY_1138_DATA,0);
  #else
         REGWR(1138 + PHY_BASE_ADDR,DENALI_PHY_1138_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1139_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1139 + PHY_BASE_ADDR,DENALI_PHY_1139_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1139 + PHY_BASE_ADDR,DENALI_PHY_1139_DATA,0);
  #else
         REGWR(1139 + PHY_BASE_ADDR,DENALI_PHY_1139_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1140_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1140 + PHY_BASE_ADDR,DENALI_PHY_1140_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1140 + PHY_BASE_ADDR,DENALI_PHY_1140_DATA,0);
  #else
         REGWR(1140 + PHY_BASE_ADDR,DENALI_PHY_1140_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1141_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1141 + PHY_BASE_ADDR,DENALI_PHY_1141_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1141 + PHY_BASE_ADDR,DENALI_PHY_1141_DATA,0);
  #else
         REGWR(1141 + PHY_BASE_ADDR,DENALI_PHY_1141_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1142_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1142 + PHY_BASE_ADDR,DENALI_PHY_1142_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1142 + PHY_BASE_ADDR,DENALI_PHY_1142_DATA,0);
  #else
         REGWR(1142 + PHY_BASE_ADDR,DENALI_PHY_1142_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1143_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1143 + PHY_BASE_ADDR,DENALI_PHY_1143_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1143 + PHY_BASE_ADDR,DENALI_PHY_1143_DATA,0);
  #else
         REGWR(1143 + PHY_BASE_ADDR,DENALI_PHY_1143_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1144_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1144 + PHY_BASE_ADDR,DENALI_PHY_1144_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1144 + PHY_BASE_ADDR,DENALI_PHY_1144_DATA,0);
  #else
         REGWR(1144 + PHY_BASE_ADDR,DENALI_PHY_1144_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1145_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1145 + PHY_BASE_ADDR,DENALI_PHY_1145_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1145 + PHY_BASE_ADDR,DENALI_PHY_1145_DATA,0);
  #else
         REGWR(1145 + PHY_BASE_ADDR,DENALI_PHY_1145_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1146_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1146 + PHY_BASE_ADDR,DENALI_PHY_1146_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1146 + PHY_BASE_ADDR,DENALI_PHY_1146_DATA,0);
  #else
         REGWR(1146 + PHY_BASE_ADDR,DENALI_PHY_1146_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1147_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1147 + PHY_BASE_ADDR,DENALI_PHY_1147_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1147 + PHY_BASE_ADDR,DENALI_PHY_1147_DATA,0);
  #else
         REGWR(1147 + PHY_BASE_ADDR,DENALI_PHY_1147_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1148_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1148 + PHY_BASE_ADDR,DENALI_PHY_1148_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1148 + PHY_BASE_ADDR,DENALI_PHY_1148_DATA,0);
  #else
         REGWR(1148 + PHY_BASE_ADDR,DENALI_PHY_1148_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1149_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1149 + PHY_BASE_ADDR,DENALI_PHY_1149_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1149 + PHY_BASE_ADDR,DENALI_PHY_1149_DATA,0);
  #else
         REGWR(1149 + PHY_BASE_ADDR,DENALI_PHY_1149_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1150_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1150 + PHY_BASE_ADDR,DENALI_PHY_1150_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1150 + PHY_BASE_ADDR,DENALI_PHY_1150_DATA,0);
  #else
         REGWR(1150 + PHY_BASE_ADDR,DENALI_PHY_1150_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1151_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1151 + PHY_BASE_ADDR,DENALI_PHY_1151_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1151 + PHY_BASE_ADDR,DENALI_PHY_1151_DATA,0);
  #else
         REGWR(1151 + PHY_BASE_ADDR,DENALI_PHY_1151_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1152_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1152 + PHY_BASE_ADDR,DENALI_PHY_1152_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1152 + PHY_BASE_ADDR,DENALI_PHY_1152_DATA,0);
  #else
         REGWR(1152 + PHY_BASE_ADDR,DENALI_PHY_1152_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1153_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1153 + PHY_BASE_ADDR,DENALI_PHY_1153_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1153 + PHY_BASE_ADDR,DENALI_PHY_1153_DATA,0);
  #else
         REGWR(1153 + PHY_BASE_ADDR,DENALI_PHY_1153_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1154_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1154 + PHY_BASE_ADDR,DENALI_PHY_1154_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1154 + PHY_BASE_ADDR,DENALI_PHY_1154_DATA,0);
  #else
         REGWR(1154 + PHY_BASE_ADDR,DENALI_PHY_1154_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1155_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1155 + PHY_BASE_ADDR,DENALI_PHY_1155_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1155 + PHY_BASE_ADDR,DENALI_PHY_1155_DATA,0);
  #else
         REGWR(1155 + PHY_BASE_ADDR,DENALI_PHY_1155_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1156_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1156 + PHY_BASE_ADDR,DENALI_PHY_1156_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1156 + PHY_BASE_ADDR,DENALI_PHY_1156_DATA,0);
  #else
         REGWR(1156 + PHY_BASE_ADDR,DENALI_PHY_1156_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1157_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1157 + PHY_BASE_ADDR,DENALI_PHY_1157_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1157 + PHY_BASE_ADDR,DENALI_PHY_1157_DATA,0);
  #else
         REGWR(1157 + PHY_BASE_ADDR,DENALI_PHY_1157_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1158_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1158 + PHY_BASE_ADDR,DENALI_PHY_1158_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1158 + PHY_BASE_ADDR,DENALI_PHY_1158_DATA,0);
  #else
         REGWR(1158 + PHY_BASE_ADDR,DENALI_PHY_1158_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1159_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1159 + PHY_BASE_ADDR,DENALI_PHY_1159_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1159 + PHY_BASE_ADDR,DENALI_PHY_1159_DATA,0);
  #else
         REGWR(1159 + PHY_BASE_ADDR,DENALI_PHY_1159_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1160_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1160 + PHY_BASE_ADDR,DENALI_PHY_1160_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1160 + PHY_BASE_ADDR,DENALI_PHY_1160_DATA,0);
  #else
         REGWR(1160 + PHY_BASE_ADDR,DENALI_PHY_1160_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1161_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1161 + PHY_BASE_ADDR,DENALI_PHY_1161_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1161 + PHY_BASE_ADDR,DENALI_PHY_1161_DATA,0);
  #else
         REGWR(1161 + PHY_BASE_ADDR,DENALI_PHY_1161_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1162_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1162 + PHY_BASE_ADDR,DENALI_PHY_1162_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1162 + PHY_BASE_ADDR,DENALI_PHY_1162_DATA,0);
  #else
         REGWR(1162 + PHY_BASE_ADDR,DENALI_PHY_1162_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1163_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1163 + PHY_BASE_ADDR,DENALI_PHY_1163_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1163 + PHY_BASE_ADDR,DENALI_PHY_1163_DATA,0);
  #else
         REGWR(1163 + PHY_BASE_ADDR,DENALI_PHY_1163_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1164_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1164 + PHY_BASE_ADDR,DENALI_PHY_1164_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1164 + PHY_BASE_ADDR,DENALI_PHY_1164_DATA,0);
  #else
         REGWR(1164 + PHY_BASE_ADDR,DENALI_PHY_1164_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1165_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1165 + PHY_BASE_ADDR,DENALI_PHY_1165_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1165 + PHY_BASE_ADDR,DENALI_PHY_1165_DATA,0);
  #else
         REGWR(1165 + PHY_BASE_ADDR,DENALI_PHY_1165_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1166_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1166 + PHY_BASE_ADDR,DENALI_PHY_1166_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1166 + PHY_BASE_ADDR,DENALI_PHY_1166_DATA,0);
  #else
         REGWR(1166 + PHY_BASE_ADDR,DENALI_PHY_1166_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1167_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1167 + PHY_BASE_ADDR,DENALI_PHY_1167_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1167 + PHY_BASE_ADDR,DENALI_PHY_1167_DATA,0);
  #else
         REGWR(1167 + PHY_BASE_ADDR,DENALI_PHY_1167_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1168_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1168 + PHY_BASE_ADDR,DENALI_PHY_1168_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1168 + PHY_BASE_ADDR,DENALI_PHY_1168_DATA,0);
  #else
         REGWR(1168 + PHY_BASE_ADDR,DENALI_PHY_1168_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1169_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1169 + PHY_BASE_ADDR,DENALI_PHY_1169_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1169 + PHY_BASE_ADDR,DENALI_PHY_1169_DATA,0);
  #else
         REGWR(1169 + PHY_BASE_ADDR,DENALI_PHY_1169_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1170_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1170 + PHY_BASE_ADDR,DENALI_PHY_1170_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1170 + PHY_BASE_ADDR,DENALI_PHY_1170_DATA,0);
  #else
         REGWR(1170 + PHY_BASE_ADDR,DENALI_PHY_1170_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1171_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1171 + PHY_BASE_ADDR,DENALI_PHY_1171_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1171 + PHY_BASE_ADDR,DENALI_PHY_1171_DATA,0);
  #else
         REGWR(1171 + PHY_BASE_ADDR,DENALI_PHY_1171_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1172_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1172 + PHY_BASE_ADDR,DENALI_PHY_1172_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1172 + PHY_BASE_ADDR,DENALI_PHY_1172_DATA,0);
  #else
         REGWR(1172 + PHY_BASE_ADDR,DENALI_PHY_1172_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1173_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1173 + PHY_BASE_ADDR,DENALI_PHY_1173_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1173 + PHY_BASE_ADDR,DENALI_PHY_1173_DATA,0);
  #else
         REGWR(1173 + PHY_BASE_ADDR,DENALI_PHY_1173_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1174_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1174 + PHY_BASE_ADDR,DENALI_PHY_1174_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1174 + PHY_BASE_ADDR,DENALI_PHY_1174_DATA,0);
  #else
         REGWR(1174 + PHY_BASE_ADDR,DENALI_PHY_1174_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1175_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1175 + PHY_BASE_ADDR,DENALI_PHY_1175_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1175 + PHY_BASE_ADDR,DENALI_PHY_1175_DATA,0);
  #else
         REGWR(1175 + PHY_BASE_ADDR,DENALI_PHY_1175_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1176_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1176 + PHY_BASE_ADDR,DENALI_PHY_1176_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1176 + PHY_BASE_ADDR,DENALI_PHY_1176_DATA,0);
  #else
         REGWR(1176 + PHY_BASE_ADDR,DENALI_PHY_1176_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1177_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1177 + PHY_BASE_ADDR,DENALI_PHY_1177_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1177 + PHY_BASE_ADDR,DENALI_PHY_1177_DATA,0);
  #else
         REGWR(1177 + PHY_BASE_ADDR,DENALI_PHY_1177_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1178_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1178 + PHY_BASE_ADDR,DENALI_PHY_1178_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1178 + PHY_BASE_ADDR,DENALI_PHY_1178_DATA,0);
  #else
         REGWR(1178 + PHY_BASE_ADDR,DENALI_PHY_1178_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1179_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1179 + PHY_BASE_ADDR,DENALI_PHY_1179_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1179 + PHY_BASE_ADDR,DENALI_PHY_1179_DATA,0);
  #else
         REGWR(1179 + PHY_BASE_ADDR,DENALI_PHY_1179_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1180_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1180 + PHY_BASE_ADDR,DENALI_PHY_1180_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1180 + PHY_BASE_ADDR,DENALI_PHY_1180_DATA,0);
  #else
         REGWR(1180 + PHY_BASE_ADDR,DENALI_PHY_1180_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1181_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1181 + PHY_BASE_ADDR,DENALI_PHY_1181_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1181 + PHY_BASE_ADDR,DENALI_PHY_1181_DATA,0);
  #else
         REGWR(1181 + PHY_BASE_ADDR,DENALI_PHY_1181_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1182_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1182 + PHY_BASE_ADDR,DENALI_PHY_1182_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1182 + PHY_BASE_ADDR,DENALI_PHY_1182_DATA,0);
  #else
         REGWR(1182 + PHY_BASE_ADDR,DENALI_PHY_1182_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1183_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1183 + PHY_BASE_ADDR,DENALI_PHY_1183_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1183 + PHY_BASE_ADDR,DENALI_PHY_1183_DATA,0);
  #else
         REGWR(1183 + PHY_BASE_ADDR,DENALI_PHY_1183_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1184_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1184 + PHY_BASE_ADDR,DENALI_PHY_1184_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1184 + PHY_BASE_ADDR,DENALI_PHY_1184_DATA,0);
  #else
         REGWR(1184 + PHY_BASE_ADDR,DENALI_PHY_1184_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1185_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1185 + PHY_BASE_ADDR,DENALI_PHY_1185_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1185 + PHY_BASE_ADDR,DENALI_PHY_1185_DATA,0);
  #else
         REGWR(1185 + PHY_BASE_ADDR,DENALI_PHY_1185_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1186_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1186 + PHY_BASE_ADDR,DENALI_PHY_1186_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1186 + PHY_BASE_ADDR,DENALI_PHY_1186_DATA,0);
  #else
         REGWR(1186 + PHY_BASE_ADDR,DENALI_PHY_1186_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1187_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1187 + PHY_BASE_ADDR,DENALI_PHY_1187_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1187 + PHY_BASE_ADDR,DENALI_PHY_1187_DATA,0);
  #else
         REGWR(1187 + PHY_BASE_ADDR,DENALI_PHY_1187_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1188_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1188 + PHY_BASE_ADDR,DENALI_PHY_1188_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1188 + PHY_BASE_ADDR,DENALI_PHY_1188_DATA,0);
  #else
         REGWR(1188 + PHY_BASE_ADDR,DENALI_PHY_1188_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1189_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1189 + PHY_BASE_ADDR,DENALI_PHY_1189_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1189 + PHY_BASE_ADDR,DENALI_PHY_1189_DATA,0);
  #else
         REGWR(1189 + PHY_BASE_ADDR,DENALI_PHY_1189_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1190_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1190 + PHY_BASE_ADDR,DENALI_PHY_1190_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1190 + PHY_BASE_ADDR,DENALI_PHY_1190_DATA,0);
  #else
         REGWR(1190 + PHY_BASE_ADDR,DENALI_PHY_1190_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1191_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1191 + PHY_BASE_ADDR,DENALI_PHY_1191_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1191 + PHY_BASE_ADDR,DENALI_PHY_1191_DATA,0);
  #else
         REGWR(1191 + PHY_BASE_ADDR,DENALI_PHY_1191_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1192_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1192 + PHY_BASE_ADDR,DENALI_PHY_1192_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1192 + PHY_BASE_ADDR,DENALI_PHY_1192_DATA,0);
  #else
         REGWR(1192 + PHY_BASE_ADDR,DENALI_PHY_1192_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1193_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1193 + PHY_BASE_ADDR,DENALI_PHY_1193_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1193 + PHY_BASE_ADDR,DENALI_PHY_1193_DATA,0);
  #else
         REGWR(1193 + PHY_BASE_ADDR,DENALI_PHY_1193_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1194_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1194 + PHY_BASE_ADDR,DENALI_PHY_1194_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1194 + PHY_BASE_ADDR,DENALI_PHY_1194_DATA,0);
  #else
         REGWR(1194 + PHY_BASE_ADDR,DENALI_PHY_1194_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1195_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1195 + PHY_BASE_ADDR,DENALI_PHY_1195_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1195 + PHY_BASE_ADDR,DENALI_PHY_1195_DATA,0);
  #else
         REGWR(1195 + PHY_BASE_ADDR,DENALI_PHY_1195_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1196_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1196 + PHY_BASE_ADDR,DENALI_PHY_1196_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1196 + PHY_BASE_ADDR,DENALI_PHY_1196_DATA,0);
  #else
         REGWR(1196 + PHY_BASE_ADDR,DENALI_PHY_1196_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1197_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1197 + PHY_BASE_ADDR,DENALI_PHY_1197_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1197 + PHY_BASE_ADDR,DENALI_PHY_1197_DATA,0);
  #else
         REGWR(1197 + PHY_BASE_ADDR,DENALI_PHY_1197_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1198_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1198 + PHY_BASE_ADDR,DENALI_PHY_1198_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1198 + PHY_BASE_ADDR,DENALI_PHY_1198_DATA,0);
  #else
         REGWR(1198 + PHY_BASE_ADDR,DENALI_PHY_1198_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1199_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1199 + PHY_BASE_ADDR,DENALI_PHY_1199_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1199 + PHY_BASE_ADDR,DENALI_PHY_1199_DATA,0);
  #else
         REGWR(1199 + PHY_BASE_ADDR,DENALI_PHY_1199_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1200_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1200 + PHY_BASE_ADDR,DENALI_PHY_1200_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1200 + PHY_BASE_ADDR,DENALI_PHY_1200_DATA,0);
  #else
         REGWR(1200 + PHY_BASE_ADDR,DENALI_PHY_1200_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1201_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1201 + PHY_BASE_ADDR,DENALI_PHY_1201_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1201 + PHY_BASE_ADDR,DENALI_PHY_1201_DATA,0);
  #else
         REGWR(1201 + PHY_BASE_ADDR,DENALI_PHY_1201_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1202_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1202 + PHY_BASE_ADDR,DENALI_PHY_1202_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1202 + PHY_BASE_ADDR,DENALI_PHY_1202_DATA,0);
  #else
         REGWR(1202 + PHY_BASE_ADDR,DENALI_PHY_1202_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1203_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1203 + PHY_BASE_ADDR,DENALI_PHY_1203_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1203 + PHY_BASE_ADDR,DENALI_PHY_1203_DATA,0);
  #else
         REGWR(1203 + PHY_BASE_ADDR,DENALI_PHY_1203_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1204_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1204 + PHY_BASE_ADDR,DENALI_PHY_1204_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1204 + PHY_BASE_ADDR,DENALI_PHY_1204_DATA,0);
  #else
         REGWR(1204 + PHY_BASE_ADDR,DENALI_PHY_1204_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1205_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1205 + PHY_BASE_ADDR,DENALI_PHY_1205_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1205 + PHY_BASE_ADDR,DENALI_PHY_1205_DATA,0);
  #else
         REGWR(1205 + PHY_BASE_ADDR,DENALI_PHY_1205_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1206_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1206 + PHY_BASE_ADDR,DENALI_PHY_1206_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1206 + PHY_BASE_ADDR,DENALI_PHY_1206_DATA,0);
  #else
         REGWR(1206 + PHY_BASE_ADDR,DENALI_PHY_1206_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1207_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1207 + PHY_BASE_ADDR,DENALI_PHY_1207_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1207 + PHY_BASE_ADDR,DENALI_PHY_1207_DATA,0);
  #else
         REGWR(1207 + PHY_BASE_ADDR,DENALI_PHY_1207_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1208_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1208 + PHY_BASE_ADDR,DENALI_PHY_1208_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1208 + PHY_BASE_ADDR,DENALI_PHY_1208_DATA,0);
  #else
         REGWR(1208 + PHY_BASE_ADDR,DENALI_PHY_1208_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1209_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1209 + PHY_BASE_ADDR,DENALI_PHY_1209_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1209 + PHY_BASE_ADDR,DENALI_PHY_1209_DATA,0);
  #else
         REGWR(1209 + PHY_BASE_ADDR,DENALI_PHY_1209_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1210_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1210 + PHY_BASE_ADDR,DENALI_PHY_1210_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1210 + PHY_BASE_ADDR,DENALI_PHY_1210_DATA,0);
  #else
         REGWR(1210 + PHY_BASE_ADDR,DENALI_PHY_1210_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1211_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1211 + PHY_BASE_ADDR,DENALI_PHY_1211_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1211 + PHY_BASE_ADDR,DENALI_PHY_1211_DATA,0);
  #else
         REGWR(1211 + PHY_BASE_ADDR,DENALI_PHY_1211_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1212_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1212 + PHY_BASE_ADDR,DENALI_PHY_1212_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1212 + PHY_BASE_ADDR,DENALI_PHY_1212_DATA,0);
  #else
         REGWR(1212 + PHY_BASE_ADDR,DENALI_PHY_1212_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1213_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1213 + PHY_BASE_ADDR,DENALI_PHY_1213_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1213 + PHY_BASE_ADDR,DENALI_PHY_1213_DATA,0);
  #else
         REGWR(1213 + PHY_BASE_ADDR,DENALI_PHY_1213_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1214_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1214 + PHY_BASE_ADDR,DENALI_PHY_1214_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1214 + PHY_BASE_ADDR,DENALI_PHY_1214_DATA,0);
  #else
         REGWR(1214 + PHY_BASE_ADDR,DENALI_PHY_1214_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1215_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1215 + PHY_BASE_ADDR,DENALI_PHY_1215_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1215 + PHY_BASE_ADDR,DENALI_PHY_1215_DATA,0);
  #else
         REGWR(1215 + PHY_BASE_ADDR,DENALI_PHY_1215_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1216_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1216 + PHY_BASE_ADDR,DENALI_PHY_1216_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1216 + PHY_BASE_ADDR,DENALI_PHY_1216_DATA,0);
  #else
         REGWR(1216 + PHY_BASE_ADDR,DENALI_PHY_1216_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1217_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1217 + PHY_BASE_ADDR,DENALI_PHY_1217_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1217 + PHY_BASE_ADDR,DENALI_PHY_1217_DATA,0);
  #else
         REGWR(1217 + PHY_BASE_ADDR,DENALI_PHY_1217_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1218_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1218 + PHY_BASE_ADDR,DENALI_PHY_1218_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1218 + PHY_BASE_ADDR,DENALI_PHY_1218_DATA,0);
  #else
         REGWR(1218 + PHY_BASE_ADDR,DENALI_PHY_1218_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1219_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1219 + PHY_BASE_ADDR,DENALI_PHY_1219_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1219 + PHY_BASE_ADDR,DENALI_PHY_1219_DATA,0);
  #else
         REGWR(1219 + PHY_BASE_ADDR,DENALI_PHY_1219_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1220_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1220 + PHY_BASE_ADDR,DENALI_PHY_1220_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1220 + PHY_BASE_ADDR,DENALI_PHY_1220_DATA,0);
  #else
         REGWR(1220 + PHY_BASE_ADDR,DENALI_PHY_1220_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1221_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1221 + PHY_BASE_ADDR,DENALI_PHY_1221_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1221 + PHY_BASE_ADDR,DENALI_PHY_1221_DATA,0);
  #else
         REGWR(1221 + PHY_BASE_ADDR,DENALI_PHY_1221_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1222_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1222 + PHY_BASE_ADDR,DENALI_PHY_1222_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1222 + PHY_BASE_ADDR,DENALI_PHY_1222_DATA,0);
  #else
         REGWR(1222 + PHY_BASE_ADDR,DENALI_PHY_1222_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1223_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1223 + PHY_BASE_ADDR,DENALI_PHY_1223_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1223 + PHY_BASE_ADDR,DENALI_PHY_1223_DATA,0);
  #else
         REGWR(1223 + PHY_BASE_ADDR,DENALI_PHY_1223_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1224_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1224 + PHY_BASE_ADDR,DENALI_PHY_1224_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1224 + PHY_BASE_ADDR,DENALI_PHY_1224_DATA,0);
  #else
         REGWR(1224 + PHY_BASE_ADDR,DENALI_PHY_1224_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1225_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1225 + PHY_BASE_ADDR,DENALI_PHY_1225_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1225 + PHY_BASE_ADDR,DENALI_PHY_1225_DATA,0);
  #else
         REGWR(1225 + PHY_BASE_ADDR,DENALI_PHY_1225_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1226_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1226 + PHY_BASE_ADDR,DENALI_PHY_1226_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1226 + PHY_BASE_ADDR,DENALI_PHY_1226_DATA,0);
  #else
         REGWR(1226 + PHY_BASE_ADDR,DENALI_PHY_1226_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1227_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1227 + PHY_BASE_ADDR,DENALI_PHY_1227_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1227 + PHY_BASE_ADDR,DENALI_PHY_1227_DATA,0);
  #else
         REGWR(1227 + PHY_BASE_ADDR,DENALI_PHY_1227_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1228_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1228 + PHY_BASE_ADDR,DENALI_PHY_1228_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1228 + PHY_BASE_ADDR,DENALI_PHY_1228_DATA,0);
  #else
         REGWR(1228 + PHY_BASE_ADDR,DENALI_PHY_1228_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1229_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1229 + PHY_BASE_ADDR,DENALI_PHY_1229_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1229 + PHY_BASE_ADDR,DENALI_PHY_1229_DATA,0);
  #else
         REGWR(1229 + PHY_BASE_ADDR,DENALI_PHY_1229_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1230_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1230 + PHY_BASE_ADDR,DENALI_PHY_1230_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1230 + PHY_BASE_ADDR,DENALI_PHY_1230_DATA,0);
  #else
         REGWR(1230 + PHY_BASE_ADDR,DENALI_PHY_1230_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1231_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1231 + PHY_BASE_ADDR,DENALI_PHY_1231_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1231 + PHY_BASE_ADDR,DENALI_PHY_1231_DATA,0);
  #else
         REGWR(1231 + PHY_BASE_ADDR,DENALI_PHY_1231_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1232_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1232 + PHY_BASE_ADDR,DENALI_PHY_1232_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1232 + PHY_BASE_ADDR,DENALI_PHY_1232_DATA,0);
  #else
         REGWR(1232 + PHY_BASE_ADDR,DENALI_PHY_1232_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1233_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1233 + PHY_BASE_ADDR,DENALI_PHY_1233_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1233 + PHY_BASE_ADDR,DENALI_PHY_1233_DATA,0);
  #else
         REGWR(1233 + PHY_BASE_ADDR,DENALI_PHY_1233_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1234_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1234 + PHY_BASE_ADDR,DENALI_PHY_1234_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1234 + PHY_BASE_ADDR,DENALI_PHY_1234_DATA,0);
  #else
         REGWR(1234 + PHY_BASE_ADDR,DENALI_PHY_1234_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1235_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1235 + PHY_BASE_ADDR,DENALI_PHY_1235_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1235 + PHY_BASE_ADDR,DENALI_PHY_1235_DATA,0);
  #else
         REGWR(1235 + PHY_BASE_ADDR,DENALI_PHY_1235_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1236_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1236 + PHY_BASE_ADDR,DENALI_PHY_1236_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1236 + PHY_BASE_ADDR,DENALI_PHY_1236_DATA,0);
  #else
         REGWR(1236 + PHY_BASE_ADDR,DENALI_PHY_1236_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1237_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1237 + PHY_BASE_ADDR,DENALI_PHY_1237_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1237 + PHY_BASE_ADDR,DENALI_PHY_1237_DATA,0);
  #else
         REGWR(1237 + PHY_BASE_ADDR,DENALI_PHY_1237_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1238_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1238 + PHY_BASE_ADDR,DENALI_PHY_1238_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1238 + PHY_BASE_ADDR,DENALI_PHY_1238_DATA,0);
  #else
         REGWR(1238 + PHY_BASE_ADDR,DENALI_PHY_1238_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1239_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1239 + PHY_BASE_ADDR,DENALI_PHY_1239_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1239 + PHY_BASE_ADDR,DENALI_PHY_1239_DATA,0);
  #else
         REGWR(1239 + PHY_BASE_ADDR,DENALI_PHY_1239_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1240_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1240 + PHY_BASE_ADDR,DENALI_PHY_1240_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1240 + PHY_BASE_ADDR,DENALI_PHY_1240_DATA,0);
  #else
         REGWR(1240 + PHY_BASE_ADDR,DENALI_PHY_1240_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1241_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1241 + PHY_BASE_ADDR,DENALI_PHY_1241_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1241 + PHY_BASE_ADDR,DENALI_PHY_1241_DATA,0);
  #else
         REGWR(1241 + PHY_BASE_ADDR,DENALI_PHY_1241_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1242_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1242 + PHY_BASE_ADDR,DENALI_PHY_1242_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1242 + PHY_BASE_ADDR,DENALI_PHY_1242_DATA,0);
  #else
         REGWR(1242 + PHY_BASE_ADDR,DENALI_PHY_1242_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1243_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1243 + PHY_BASE_ADDR,DENALI_PHY_1243_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1243 + PHY_BASE_ADDR,DENALI_PHY_1243_DATA,0);
  #else
         REGWR(1243 + PHY_BASE_ADDR,DENALI_PHY_1243_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1244_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1244 + PHY_BASE_ADDR,DENALI_PHY_1244_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1244 + PHY_BASE_ADDR,DENALI_PHY_1244_DATA,0);
  #else
         REGWR(1244 + PHY_BASE_ADDR,DENALI_PHY_1244_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1245_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1245 + PHY_BASE_ADDR,DENALI_PHY_1245_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1245 + PHY_BASE_ADDR,DENALI_PHY_1245_DATA,0);
  #else
         REGWR(1245 + PHY_BASE_ADDR,DENALI_PHY_1245_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1246_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1246 + PHY_BASE_ADDR,DENALI_PHY_1246_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1246 + PHY_BASE_ADDR,DENALI_PHY_1246_DATA,0);
  #else
         REGWR(1246 + PHY_BASE_ADDR,DENALI_PHY_1246_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1247_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1247 + PHY_BASE_ADDR,DENALI_PHY_1247_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1247 + PHY_BASE_ADDR,DENALI_PHY_1247_DATA,0);
  #else
         REGWR(1247 + PHY_BASE_ADDR,DENALI_PHY_1247_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1248_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1248 + PHY_BASE_ADDR,DENALI_PHY_1248_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1248 + PHY_BASE_ADDR,DENALI_PHY_1248_DATA,0);
  #else
         REGWR(1248 + PHY_BASE_ADDR,DENALI_PHY_1248_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1249_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1249 + PHY_BASE_ADDR,DENALI_PHY_1249_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1249 + PHY_BASE_ADDR,DENALI_PHY_1249_DATA,0);
  #else
         REGWR(1249 + PHY_BASE_ADDR,DENALI_PHY_1249_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1250_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1250 + PHY_BASE_ADDR,DENALI_PHY_1250_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1250 + PHY_BASE_ADDR,DENALI_PHY_1250_DATA,0);
  #else
         REGWR(1250 + PHY_BASE_ADDR,DENALI_PHY_1250_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1251_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1251 + PHY_BASE_ADDR,DENALI_PHY_1251_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1251 + PHY_BASE_ADDR,DENALI_PHY_1251_DATA,0);
  #else
         REGWR(1251 + PHY_BASE_ADDR,DENALI_PHY_1251_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1252_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1252 + PHY_BASE_ADDR,DENALI_PHY_1252_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1252 + PHY_BASE_ADDR,DENALI_PHY_1252_DATA,0);
  #else
         REGWR(1252 + PHY_BASE_ADDR,DENALI_PHY_1252_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1253_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1253 + PHY_BASE_ADDR,DENALI_PHY_1253_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1253 + PHY_BASE_ADDR,DENALI_PHY_1253_DATA,0);
  #else
         REGWR(1253 + PHY_BASE_ADDR,DENALI_PHY_1253_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1254_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1254 + PHY_BASE_ADDR,DENALI_PHY_1254_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1254 + PHY_BASE_ADDR,DENALI_PHY_1254_DATA,0);
  #else
         REGWR(1254 + PHY_BASE_ADDR,DENALI_PHY_1254_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1255_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1255 + PHY_BASE_ADDR,DENALI_PHY_1255_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1255 + PHY_BASE_ADDR,DENALI_PHY_1255_DATA,0);
  #else
         REGWR(1255 + PHY_BASE_ADDR,DENALI_PHY_1255_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1256_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1256 + PHY_BASE_ADDR,DENALI_PHY_1256_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1256 + PHY_BASE_ADDR,DENALI_PHY_1256_DATA,0);
  #else
         REGWR(1256 + PHY_BASE_ADDR,DENALI_PHY_1256_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1257_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1257 + PHY_BASE_ADDR,DENALI_PHY_1257_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1257 + PHY_BASE_ADDR,DENALI_PHY_1257_DATA,0);
  #else
         REGWR(1257 + PHY_BASE_ADDR,DENALI_PHY_1257_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1258_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1258 + PHY_BASE_ADDR,DENALI_PHY_1258_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1258 + PHY_BASE_ADDR,DENALI_PHY_1258_DATA,0);
  #else
         REGWR(1258 + PHY_BASE_ADDR,DENALI_PHY_1258_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1259_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1259 + PHY_BASE_ADDR,DENALI_PHY_1259_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1259 + PHY_BASE_ADDR,DENALI_PHY_1259_DATA,0);
  #else
         REGWR(1259 + PHY_BASE_ADDR,DENALI_PHY_1259_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1260_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1260 + PHY_BASE_ADDR,DENALI_PHY_1260_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1260 + PHY_BASE_ADDR,DENALI_PHY_1260_DATA,0);
  #else
         REGWR(1260 + PHY_BASE_ADDR,DENALI_PHY_1260_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1261_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1261 + PHY_BASE_ADDR,DENALI_PHY_1261_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1261 + PHY_BASE_ADDR,DENALI_PHY_1261_DATA,0);
  #else
         REGWR(1261 + PHY_BASE_ADDR,DENALI_PHY_1261_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1262_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1262 + PHY_BASE_ADDR,DENALI_PHY_1262_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1262 + PHY_BASE_ADDR,DENALI_PHY_1262_DATA,0);
  #else
         REGWR(1262 + PHY_BASE_ADDR,DENALI_PHY_1262_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1263_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1263 + PHY_BASE_ADDR,DENALI_PHY_1263_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1263 + PHY_BASE_ADDR,DENALI_PHY_1263_DATA,0);
  #else
         REGWR(1263 + PHY_BASE_ADDR,DENALI_PHY_1263_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1264_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1264 + PHY_BASE_ADDR,DENALI_PHY_1264_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1264 + PHY_BASE_ADDR,DENALI_PHY_1264_DATA,0);
  #else
         REGWR(1264 + PHY_BASE_ADDR,DENALI_PHY_1264_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1265_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1265 + PHY_BASE_ADDR,DENALI_PHY_1265_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1265 + PHY_BASE_ADDR,DENALI_PHY_1265_DATA,0);
  #else
         REGWR(1265 + PHY_BASE_ADDR,DENALI_PHY_1265_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1266_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1266 + PHY_BASE_ADDR,DENALI_PHY_1266_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1266 + PHY_BASE_ADDR,DENALI_PHY_1266_DATA,0);
  #else
         REGWR(1266 + PHY_BASE_ADDR,DENALI_PHY_1266_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1267_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1267 + PHY_BASE_ADDR,DENALI_PHY_1267_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1267 + PHY_BASE_ADDR,DENALI_PHY_1267_DATA,0);
  #else
         REGWR(1267 + PHY_BASE_ADDR,DENALI_PHY_1267_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1268_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1268 + PHY_BASE_ADDR,DENALI_PHY_1268_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1268 + PHY_BASE_ADDR,DENALI_PHY_1268_DATA,0);
  #else
         REGWR(1268 + PHY_BASE_ADDR,DENALI_PHY_1268_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1269_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1269 + PHY_BASE_ADDR,DENALI_PHY_1269_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1269 + PHY_BASE_ADDR,DENALI_PHY_1269_DATA,0);
  #else
         REGWR(1269 + PHY_BASE_ADDR,DENALI_PHY_1269_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1270_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1270 + PHY_BASE_ADDR,DENALI_PHY_1270_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1270 + PHY_BASE_ADDR,DENALI_PHY_1270_DATA,0);
  #else
         REGWR(1270 + PHY_BASE_ADDR,DENALI_PHY_1270_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1271_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1271 + PHY_BASE_ADDR,DENALI_PHY_1271_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1271 + PHY_BASE_ADDR,DENALI_PHY_1271_DATA,0);
  #else
         REGWR(1271 + PHY_BASE_ADDR,DENALI_PHY_1271_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1272_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1272 + PHY_BASE_ADDR,DENALI_PHY_1272_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1272 + PHY_BASE_ADDR,DENALI_PHY_1272_DATA,0);
  #else
         REGWR(1272 + PHY_BASE_ADDR,DENALI_PHY_1272_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1273_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1273 + PHY_BASE_ADDR,DENALI_PHY_1273_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1273 + PHY_BASE_ADDR,DENALI_PHY_1273_DATA,0);
  #else
         REGWR(1273 + PHY_BASE_ADDR,DENALI_PHY_1273_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1274_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1274 + PHY_BASE_ADDR,DENALI_PHY_1274_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1274 + PHY_BASE_ADDR,DENALI_PHY_1274_DATA,0);
  #else
         REGWR(1274 + PHY_BASE_ADDR,DENALI_PHY_1274_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1275_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1275 + PHY_BASE_ADDR,DENALI_PHY_1275_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1275 + PHY_BASE_ADDR,DENALI_PHY_1275_DATA,0);
  #else
         REGWR(1275 + PHY_BASE_ADDR,DENALI_PHY_1275_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1276_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1276 + PHY_BASE_ADDR,DENALI_PHY_1276_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1276 + PHY_BASE_ADDR,DENALI_PHY_1276_DATA,0);
  #else
         REGWR(1276 + PHY_BASE_ADDR,DENALI_PHY_1276_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1277_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1277 + PHY_BASE_ADDR,DENALI_PHY_1277_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1277 + PHY_BASE_ADDR,DENALI_PHY_1277_DATA,0);
  #else
         REGWR(1277 + PHY_BASE_ADDR,DENALI_PHY_1277_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1278_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1278 + PHY_BASE_ADDR,DENALI_PHY_1278_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1278 + PHY_BASE_ADDR,DENALI_PHY_1278_DATA,0);
  #else
         REGWR(1278 + PHY_BASE_ADDR,DENALI_PHY_1278_DATA,0);
  #endif
#endif
#endif
#ifdef DENALI_PHY_1279_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1279 + PHY_BASE_ADDR,DENALI_PHY_1279_DATA,0);
#else
  #ifdef REG_BACKDOOR_ACCESS
         REGWR_PHY(1279 + PHY_BASE_ADDR,DENALI_PHY_1279_DATA,0);
  #else
         REGWR(1279 + PHY_BASE_ADDR,DENALI_PHY_1279_DATA,0);
  #endif
#endif
#endif

}//}}}

//void init_ddr_bvt() {//{{{
//  uint32_t temp;
//  printf("DDR init start\n");
//  temp = reg32(DDR_CTRL0_BASE+0x00);
//  printf("read DDR_CTRL0_BASE+0 :0x%x\n\n",temp);
//
//}//}}}

void init_ddr_bvt() {//{{{
    printf("DDR init start\n");

    /* unsigned int tmp0; */

    /* printf("tmp0\n"); */
    ////reg32(DDR_CFG_BASE) = 0x12341234;
    //REGWR(0, 0x5a5aa5a5, 0);
    //tmp0 = reg32(DDR_CFG_BASE);
    //printf("tmp1\n");

    config_ctl_pi();
    //cmd(0xfe000000);
    //while(1){
    //    unsigned int tmp_cmd = SYS_DEBUG->dbg_cmd0;
    //    if(tmp_cmd == 0xfe000001){
    //        printf("back_door config done: ctrl&pi\n");
    //        break;
    //    }
    //}

    delay(1);

//    if(lpddr4_mode){
//        SET_PARAM(PI_BOOT_FROM_WORK_FREQ_ADDR, PI_BOOT_FROM_WORK_FREQ_OFFSET, PI_BOOT_FROM_WORK_FREQ_WIDTH, 1); 
//        SET_PARAM(PI_FREQ_MAP_ADDR, PI_FREQ_MAP_OFFSET, PI_FREQ_MAP_WIDTH, 4); 
//    }
//    printf("ctl&pi config done\n");

    config_phy();
    //cmd(0xfe100000);
    //while(1){
    //    unsigned int tmp_cmd = SYS_DEBUG->dbg_cmd0;
    //    if(tmp_cmd == 0xfe100001){
    //        printf("back_door config done: phy\n");
    //        break;
    //    }
    //}

    delay(1);

//    SET_PARAM(LPI_WAKEUP_EN_ADDR, LPI_WAKEUP_EN_OFFSET, LPI_WAKEUP_EN_WIDTH, 45);

    // Verilog AXI tests expect read data to be in order per port.
    // Force in-order accept to help guarantee this.
    SET_PARAM(IN_ORDER_ACCEPT_ADDR, IN_ORDER_ACCEPT_OFFSET, IN_ORDER_ACCEPT_WIDTH, 1);
    // Disable special R/W group switches so that R/W group placement is always at END of R/W group.
    SET_PARAM(DISABLE_RW_GROUP_W_BNK_CONFLICT_ADDR, DISABLE_RW_GROUP_W_BNK_CONFLICT_OFFSET, DISABLE_RW_GROUP_W_BNK_CONFLICT_WIDTH, 0);
    // Disable rank grouping within a R/W group so that R/W group placement is always at END of R/W group.
    SET_PARAM(CS_SAME_EN_ADDR, CS_SAME_EN_OFFSET, CS_SAME_EN_WIDTH, 0);

//    PI_INIT_WOLVL();
    START(0);   
    
    printf("DDR init done\n");

}//}}}

void MRW(uint32_t MR_NUM, uint32_t MR_OPS){/*{{{*/
   uint32_t mr_data = 0;
   uint32_t mr_ctrl = 0;
   mr_data = MR_OPS;
   SET_PARAM(MRSINGLE_DATA_0_ADDR,MRSINGLE_DATA_0_OFFSET, MRSINGLE_DATA_0_WIDTH, mr_data);
   SET_PARAM(MRSINGLE_DATA_1_ADDR,MRSINGLE_DATA_1_OFFSET, MRSINGLE_DATA_1_WIDTH, mr_data);
   delay(10);

   //mr_ctrl[7:0]   = MR_NUM;
   //mr_ctrl[22:8]  = 0;
   //mr_ctrl[23]    = 1;
   //mr_ctrl[24]    = 1; // When set, write to all CS
   //mr_ctrl[25]    = 1; //Triggers single MR
   //mr_ctrl[26]    = 0;
    
   mr_ctrl = (1<<25) | (1<<24) | (1<<23) | MR_NUM;

   SET_PARAM(WRITE_MODEREG_ADDR,WRITE_MODEREG_OFFSET, WRITE_MODEREG_WIDTH, mr_ctrl);
   //wait (TIER_MEMCD.mrw_write_modereg_done); //repeat (200) @(posedge clk);
   //repeat (10) @(posedge clk);  
   delay(10);
}/*}}}*/

void config_ctl_fpga(){//{{{
#ifdef DENALI_CTL_00_DATA
         REGWR(0,DENALI_CTL_00_DATA,0);
#endif
#ifdef DENALI_CTL_01_DATA
         REGWR(1,DENALI_CTL_01_DATA,0);
#endif
#ifdef DENALI_CTL_02_DATA
         REGWR(2,DENALI_CTL_02_DATA,0);
#endif
#ifdef DENALI_CTL_03_DATA
         REGWR(3,DENALI_CTL_03_DATA,0);
#endif
#ifdef DENALI_CTL_04_DATA
         REGWR(4,DENALI_CTL_04_DATA,0);
#endif
#ifdef DENALI_CTL_05_DATA
         REGWR(5,DENALI_CTL_05_DATA,0);
#endif
#ifdef DENALI_CTL_06_DATA
         REGWR(6,DENALI_CTL_06_DATA,0);
#endif
#ifdef DENALI_CTL_07_DATA
         REGWR(7,DENALI_CTL_07_DATA,0);
#endif
#ifdef DENALI_CTL_08_DATA
         REGWR(8,DENALI_CTL_08_DATA,0);
#endif
#ifdef DENALI_CTL_09_DATA
         REGWR(9,DENALI_CTL_09_DATA,0);
#endif
#ifdef DENALI_CTL_10_DATA
         REGWR(10,DENALI_CTL_10_DATA,0);
#endif
#ifdef DENALI_CTL_11_DATA
         REGWR(11,DENALI_CTL_11_DATA,0);
#endif
#ifdef DENALI_CTL_12_DATA
         REGWR(12,DENALI_CTL_12_DATA,0);
#endif
#ifdef DENALI_CTL_13_DATA
         REGWR(13,DENALI_CTL_13_DATA,0);
#endif
#ifdef DENALI_CTL_14_DATA
         REGWR(14,DENALI_CTL_14_DATA,0);
#endif
#ifdef DENALI_CTL_15_DATA
         REGWR(15,DENALI_CTL_15_DATA,0);
#endif
#ifdef DENALI_CTL_16_DATA
         REGWR(16,DENALI_CTL_16_DATA,0);
#endif
#ifdef DENALI_CTL_17_DATA
         REGWR(17,DENALI_CTL_17_DATA,0);
#endif
#ifdef DENALI_CTL_18_DATA
         REGWR(18,DENALI_CTL_18_DATA,0);
#endif
#ifdef DENALI_CTL_19_DATA
         REGWR(19,DENALI_CTL_19_DATA,0);
#endif
#ifdef DENALI_CTL_20_DATA
         REGWR(20,DENALI_CTL_20_DATA,0);
#endif
#ifdef DENALI_CTL_21_DATA
         REGWR(21,DENALI_CTL_21_DATA,0);
#endif
#ifdef DENALI_CTL_22_DATA
         REGWR(22,DENALI_CTL_22_DATA,0);
#endif
#ifdef DENALI_CTL_23_DATA
         REGWR(23,DENALI_CTL_23_DATA,0);
#endif
#ifdef DENALI_CTL_24_DATA
         REGWR(24,DENALI_CTL_24_DATA,0);
#endif
#ifdef DENALI_CTL_25_DATA
         REGWR(25,DENALI_CTL_25_DATA,0);
#endif
#ifdef DENALI_CTL_26_DATA
         REGWR(26,DENALI_CTL_26_DATA,0);
#endif
#ifdef DENALI_CTL_27_DATA
         REGWR(27,DENALI_CTL_27_DATA,0);
#endif
#ifdef DENALI_CTL_28_DATA
         REGWR(28,DENALI_CTL_28_DATA,0);
#endif
#ifdef DENALI_CTL_29_DATA
         REGWR(29,DENALI_CTL_29_DATA,0);
#endif
#ifdef DENALI_CTL_30_DATA
         REGWR(30,DENALI_CTL_30_DATA,0);
#endif
#ifdef DENALI_CTL_31_DATA
         REGWR(31,DENALI_CTL_31_DATA,0);
#endif
#ifdef DENALI_CTL_32_DATA
         REGWR(32,DENALI_CTL_32_DATA,0);
#endif
#ifdef DENALI_CTL_33_DATA
         REGWR(33,DENALI_CTL_33_DATA,0);
#endif
#ifdef DENALI_CTL_34_DATA
         REGWR(34,DENALI_CTL_34_DATA,0);
#endif
#ifdef DENALI_CTL_35_DATA
         REGWR(35,DENALI_CTL_35_DATA,0);
#endif
#ifdef DENALI_CTL_36_DATA
         REGWR(36,DENALI_CTL_36_DATA,0);
#endif
#ifdef DENALI_CTL_37_DATA
         REGWR(37,DENALI_CTL_37_DATA,0);
#endif
#ifdef DENALI_CTL_38_DATA
         REGWR(38,DENALI_CTL_38_DATA,0);
#endif
#ifdef DENALI_CTL_39_DATA
         REGWR(39,DENALI_CTL_39_DATA,0);
#endif
#ifdef DENALI_CTL_40_DATA
         REGWR(40,DENALI_CTL_40_DATA,0);
#endif
#ifdef DENALI_CTL_41_DATA
         REGWR(41,DENALI_CTL_41_DATA,0);
#endif
#ifdef DENALI_CTL_42_DATA
         REGWR(42,DENALI_CTL_42_DATA,0);
#endif
#ifdef DENALI_CTL_43_DATA
         REGWR(43,DENALI_CTL_43_DATA,0);
#endif
#ifdef DENALI_CTL_44_DATA
         REGWR(44,DENALI_CTL_44_DATA,0);
#endif
#ifdef DENALI_CTL_45_DATA
         REGWR(45,DENALI_CTL_45_DATA,0);
#endif
#ifdef DENALI_CTL_46_DATA
         REGWR(46,DENALI_CTL_46_DATA,0);
#endif
#ifdef DENALI_CTL_47_DATA
         REGWR(47,DENALI_CTL_47_DATA,0);
#endif
#ifdef DENALI_CTL_48_DATA
         REGWR(48,DENALI_CTL_48_DATA,0);
#endif
#ifdef DENALI_CTL_49_DATA
         REGWR(49,DENALI_CTL_49_DATA,0);
#endif
#ifdef DENALI_CTL_50_DATA
         REGWR(50,DENALI_CTL_50_DATA,0);
#endif
#ifdef DENALI_CTL_51_DATA
         REGWR(51,DENALI_CTL_51_DATA,0);
#endif
#ifdef DENALI_CTL_52_DATA
         REGWR(52,DENALI_CTL_52_DATA,0);
#endif
#ifdef DENALI_CTL_53_DATA
         REGWR(53,DENALI_CTL_53_DATA,0);
#endif
#ifdef DENALI_CTL_54_DATA
         REGWR(54,DENALI_CTL_54_DATA,0);
#endif
#ifdef DENALI_CTL_55_DATA
         REGWR(55,DENALI_CTL_55_DATA,0);
#endif
#ifdef DENALI_CTL_56_DATA
         REGWR(56,DENALI_CTL_56_DATA,0);
#endif
#ifdef DENALI_CTL_57_DATA
         REGWR(57,DENALI_CTL_57_DATA,0);
#endif
#ifdef DENALI_CTL_58_DATA
         REGWR(58,DENALI_CTL_58_DATA,0);
#endif
#ifdef DENALI_CTL_59_DATA
         REGWR(59,DENALI_CTL_59_DATA,0);
#endif
#ifdef DENALI_CTL_60_DATA
         REGWR(60,DENALI_CTL_60_DATA,0);
#endif
#ifdef DENALI_CTL_61_DATA
         REGWR(61,DENALI_CTL_61_DATA,0);
#endif
#ifdef DENALI_CTL_62_DATA
         REGWR(62,DENALI_CTL_62_DATA,0);
#endif
#ifdef DENALI_CTL_63_DATA
         REGWR(63,DENALI_CTL_63_DATA,0);
#endif
#ifdef DENALI_CTL_64_DATA
         REGWR(64,DENALI_CTL_64_DATA,0);
#endif
#ifdef DENALI_CTL_65_DATA
         REGWR(65,DENALI_CTL_65_DATA,0);
#endif
#ifdef DENALI_CTL_66_DATA
         REGWR(66,DENALI_CTL_66_DATA,0);
#endif
#ifdef DENALI_CTL_67_DATA
         REGWR(67,DENALI_CTL_67_DATA,0);
#endif
#ifdef DENALI_CTL_68_DATA
         REGWR(68,DENALI_CTL_68_DATA,0);
#endif
#ifdef DENALI_CTL_69_DATA
         REGWR(69,DENALI_CTL_69_DATA,0);
#endif
#ifdef DENALI_CTL_70_DATA
         REGWR(70,DENALI_CTL_70_DATA,0);
#endif
#ifdef DENALI_CTL_71_DATA
         REGWR(71,DENALI_CTL_71_DATA,0);
#endif
#ifdef DENALI_CTL_72_DATA
         REGWR(72,DENALI_CTL_72_DATA,0);
#endif
#ifdef DENALI_CTL_73_DATA
         REGWR(73,DENALI_CTL_73_DATA,0);
#endif
#ifdef DENALI_CTL_74_DATA
         REGWR(74,DENALI_CTL_74_DATA,0);
#endif
#ifdef DENALI_CTL_75_DATA
         REGWR(75,DENALI_CTL_75_DATA,0);
#endif
#ifdef DENALI_CTL_76_DATA
         REGWR(76,DENALI_CTL_76_DATA,0);
#endif
#ifdef DENALI_CTL_77_DATA
         REGWR(77,DENALI_CTL_77_DATA,0);
#endif
#ifdef DENALI_CTL_78_DATA
         REGWR(78,DENALI_CTL_78_DATA,0);
#endif
#ifdef DENALI_CTL_79_DATA
         REGWR(79,DENALI_CTL_79_DATA,0);
#endif
#ifdef DENALI_CTL_80_DATA
         REGWR(80,DENALI_CTL_80_DATA,0);
#endif
#ifdef DENALI_CTL_81_DATA
         REGWR(81,DENALI_CTL_81_DATA,0);
#endif
#ifdef DENALI_CTL_82_DATA
         REGWR(82,DENALI_CTL_82_DATA,0);
#endif
#ifdef DENALI_CTL_83_DATA
         REGWR(83,DENALI_CTL_83_DATA,0);
#endif
#ifdef DENALI_CTL_84_DATA
         REGWR(84,DENALI_CTL_84_DATA,0);
#endif
#ifdef DENALI_CTL_85_DATA
         REGWR(85,DENALI_CTL_85_DATA,0);
#endif
#ifdef DENALI_CTL_86_DATA
         REGWR(86,DENALI_CTL_86_DATA,0);
#endif
#ifdef DENALI_CTL_87_DATA
         REGWR(87,DENALI_CTL_87_DATA,0);
#endif
#ifdef DENALI_CTL_88_DATA
         REGWR(88,DENALI_CTL_88_DATA,0);
#endif
#ifdef DENALI_CTL_89_DATA
         REGWR(89,DENALI_CTL_89_DATA,0);
#endif
#ifdef DENALI_CTL_90_DATA
         REGWR(90,DENALI_CTL_90_DATA,0);
#endif
#ifdef DENALI_CTL_91_DATA
         REGWR(91,DENALI_CTL_91_DATA,0);
#endif
#ifdef DENALI_CTL_92_DATA
         REGWR(92,DENALI_CTL_92_DATA,0);
#endif
#ifdef DENALI_CTL_93_DATA
         REGWR(93,DENALI_CTL_93_DATA,0);
#endif
#ifdef DENALI_CTL_94_DATA
         REGWR(94,DENALI_CTL_94_DATA,0);
#endif
#ifdef DENALI_CTL_95_DATA
         REGWR(95,DENALI_CTL_95_DATA,0);
#endif
#ifdef DENALI_CTL_96_DATA
         REGWR(96,DENALI_CTL_96_DATA,0);
#endif
#ifdef DENALI_CTL_97_DATA
         REGWR(97,DENALI_CTL_97_DATA,0);
#endif
#ifdef DENALI_CTL_98_DATA
         REGWR(98,DENALI_CTL_98_DATA,0);
#endif
#ifdef DENALI_CTL_99_DATA
         REGWR(99,DENALI_CTL_99_DATA,0);
#endif
#ifdef DENALI_CTL_100_DATA
         REGWR(100,DENALI_CTL_100_DATA,0);
#endif
#ifdef DENALI_CTL_101_DATA
         REGWR(101,DENALI_CTL_101_DATA,0);
#endif
#ifdef DENALI_CTL_102_DATA
         REGWR(102,DENALI_CTL_102_DATA,0);
#endif
#ifdef DENALI_CTL_103_DATA
         REGWR(103,DENALI_CTL_103_DATA,0);
#endif
#ifdef DENALI_CTL_104_DATA
         REGWR(104,DENALI_CTL_104_DATA,0);
#endif
#ifdef DENALI_CTL_105_DATA
         REGWR(105,DENALI_CTL_105_DATA,0);
#endif
#ifdef DENALI_CTL_106_DATA
         REGWR(106,DENALI_CTL_106_DATA,0);
#endif
#ifdef DENALI_CTL_107_DATA
         REGWR(107,DENALI_CTL_107_DATA,0);
#endif
#ifdef DENALI_CTL_108_DATA
         REGWR(108,DENALI_CTL_108_DATA,0);
#endif
#ifdef DENALI_CTL_109_DATA
         REGWR(109,DENALI_CTL_109_DATA,0);
#endif
#ifdef DENALI_CTL_110_DATA
         REGWR(110,DENALI_CTL_110_DATA,0);
#endif
#ifdef DENALI_CTL_111_DATA
         REGWR(111,DENALI_CTL_111_DATA,0);
#endif
#ifdef DENALI_CTL_112_DATA
         REGWR(112,DENALI_CTL_112_DATA,0);
#endif
#ifdef DENALI_CTL_113_DATA
         REGWR(113,DENALI_CTL_113_DATA,0);
#endif
#ifdef DENALI_CTL_114_DATA
         REGWR(114,DENALI_CTL_114_DATA,0);
#endif
#ifdef DENALI_CTL_115_DATA
         REGWR(115,DENALI_CTL_115_DATA,0);
#endif
#ifdef DENALI_CTL_116_DATA
         REGWR(116,DENALI_CTL_116_DATA,0);
#endif
#ifdef DENALI_CTL_117_DATA
         REGWR(117,DENALI_CTL_117_DATA,0);
#endif
#ifdef DENALI_CTL_118_DATA
         REGWR(118,DENALI_CTL_118_DATA,0);
#endif
#ifdef DENALI_CTL_119_DATA
         REGWR(119,DENALI_CTL_119_DATA,0);
#endif
#ifdef DENALI_CTL_120_DATA
         REGWR(120,DENALI_CTL_120_DATA,0);
#endif
#ifdef DENALI_CTL_121_DATA
         REGWR(121,DENALI_CTL_121_DATA,0);
#endif
#ifdef DENALI_CTL_122_DATA
         REGWR(122,DENALI_CTL_122_DATA,0);
#endif
#ifdef DENALI_CTL_123_DATA
         REGWR(123,DENALI_CTL_123_DATA,0);
#endif
#ifdef DENALI_CTL_124_DATA
         REGWR(124,DENALI_CTL_124_DATA,0);
#endif
#ifdef DENALI_CTL_125_DATA
         REGWR(125,DENALI_CTL_125_DATA,0);
#endif
#ifdef DENALI_CTL_126_DATA
         REGWR(126,DENALI_CTL_126_DATA,0);
#endif
#ifdef DENALI_CTL_127_DATA
         REGWR(127,DENALI_CTL_127_DATA,0);
#endif
#ifdef DENALI_CTL_128_DATA
         REGWR(128,DENALI_CTL_128_DATA,0);
#endif
#ifdef DENALI_CTL_129_DATA
         REGWR(129,DENALI_CTL_129_DATA,0);
#endif
#ifdef DENALI_CTL_130_DATA
         REGWR(130,DENALI_CTL_130_DATA,0);
#endif
#ifdef DENALI_CTL_131_DATA
         REGWR(131,DENALI_CTL_131_DATA,0);
#endif
#ifdef DENALI_CTL_132_DATA
         REGWR(132,DENALI_CTL_132_DATA,0);
#endif
#ifdef DENALI_CTL_133_DATA
         REGWR(133,DENALI_CTL_133_DATA,0);
#endif
#ifdef DENALI_CTL_134_DATA
         REGWR(134,DENALI_CTL_134_DATA,0);
#endif
#ifdef DENALI_CTL_135_DATA
         REGWR(135,DENALI_CTL_135_DATA,0);
#endif
#ifdef DENALI_CTL_136_DATA
         REGWR(136,DENALI_CTL_136_DATA,0);
#endif
#ifdef DENALI_CTL_137_DATA
         REGWR(137,DENALI_CTL_137_DATA,0);
#endif
#ifdef DENALI_CTL_138_DATA
         REGWR(138,DENALI_CTL_138_DATA,0);
#endif
#ifdef DENALI_CTL_139_DATA
         REGWR(139,DENALI_CTL_139_DATA,0);
#endif
#ifdef DENALI_CTL_140_DATA
         REGWR(140,DENALI_CTL_140_DATA,0);
#endif
#ifdef DENALI_CTL_141_DATA
         REGWR(141,DENALI_CTL_141_DATA,0);
#endif
#ifdef DENALI_CTL_142_DATA
         REGWR(142,DENALI_CTL_142_DATA,0);
#endif
#ifdef DENALI_CTL_143_DATA
         REGWR(143,DENALI_CTL_143_DATA,0);
#endif
#ifdef DENALI_CTL_144_DATA
         REGWR(144,DENALI_CTL_144_DATA,0);
#endif
#ifdef DENALI_CTL_145_DATA
         REGWR(145,DENALI_CTL_145_DATA,0);
#endif
#ifdef DENALI_CTL_146_DATA
         REGWR(146,DENALI_CTL_146_DATA,0);
#endif
#ifdef DENALI_CTL_147_DATA
         REGWR(147,DENALI_CTL_147_DATA,0);
#endif
#ifdef DENALI_CTL_148_DATA
         REGWR(148,DENALI_CTL_148_DATA,0);
#endif
#ifdef DENALI_CTL_149_DATA
         REGWR(149,DENALI_CTL_149_DATA,0);
#endif
#ifdef DENALI_CTL_150_DATA
         REGWR(150,DENALI_CTL_150_DATA,0);
#endif
#ifdef DENALI_CTL_151_DATA
         REGWR(151,DENALI_CTL_151_DATA,0);
#endif
#ifdef DENALI_CTL_152_DATA
         REGWR(152,DENALI_CTL_152_DATA,0);
#endif
#ifdef DENALI_CTL_153_DATA
         REGWR(153,DENALI_CTL_153_DATA,0);
#endif
#ifdef DENALI_CTL_154_DATA
         REGWR(154,DENALI_CTL_154_DATA,0);
#endif
#ifdef DENALI_CTL_155_DATA
         REGWR(155,DENALI_CTL_155_DATA,0);
#endif
#ifdef DENALI_CTL_156_DATA
         REGWR(156,DENALI_CTL_156_DATA,0);
#endif
#ifdef DENALI_CTL_157_DATA
         REGWR(157,DENALI_CTL_157_DATA,0);
#endif
#ifdef DENALI_CTL_158_DATA
         REGWR(158,DENALI_CTL_158_DATA,0);
#endif
#ifdef DENALI_CTL_159_DATA
         REGWR(159,DENALI_CTL_159_DATA,0);
#endif
#ifdef DENALI_CTL_160_DATA
         REGWR(160,DENALI_CTL_160_DATA,0);
#endif
#ifdef DENALI_CTL_161_DATA
         REGWR(161,DENALI_CTL_161_DATA,0);
#endif
#ifdef DENALI_CTL_162_DATA
         REGWR(162,DENALI_CTL_162_DATA,0);
#endif
#ifdef DENALI_CTL_163_DATA
         REGWR(163,DENALI_CTL_163_DATA,0);
#endif
#ifdef DENALI_CTL_164_DATA
         REGWR(164,DENALI_CTL_164_DATA,0);
#endif
#ifdef DENALI_CTL_165_DATA
         REGWR(165,DENALI_CTL_165_DATA,0);
#endif
#ifdef DENALI_CTL_166_DATA
         REGWR(166,DENALI_CTL_166_DATA,0);
#endif
#ifdef DENALI_CTL_167_DATA
         REGWR(167,DENALI_CTL_167_DATA,0);
#endif
#ifdef DENALI_CTL_168_DATA
         REGWR(168,DENALI_CTL_168_DATA,0);
#endif
#ifdef DENALI_CTL_169_DATA
         REGWR(169,DENALI_CTL_169_DATA,0);
#endif
#ifdef DENALI_CTL_170_DATA
         REGWR(170,DENALI_CTL_170_DATA,0);
#endif
#ifdef DENALI_CTL_171_DATA
         REGWR(171,DENALI_CTL_171_DATA,0);
#endif
#ifdef DENALI_CTL_172_DATA
         REGWR(172,DENALI_CTL_172_DATA,0);
#endif
#ifdef DENALI_CTL_173_DATA
         REGWR(173,DENALI_CTL_173_DATA,0);
#endif
#ifdef DENALI_CTL_174_DATA
         REGWR(174,DENALI_CTL_174_DATA,0);
#endif
#ifdef DENALI_CTL_175_DATA
         REGWR(175,DENALI_CTL_175_DATA,0);
#endif
#ifdef DENALI_CTL_176_DATA
         REGWR(176,DENALI_CTL_176_DATA,0);
#endif
#ifdef DENALI_CTL_177_DATA
         REGWR(177,DENALI_CTL_177_DATA,0);
#endif
#ifdef DENALI_CTL_178_DATA
         REGWR(178,DENALI_CTL_178_DATA,0);
#endif
#ifdef DENALI_CTL_179_DATA
         REGWR(179,DENALI_CTL_179_DATA,0);
#endif
#ifdef DENALI_CTL_180_DATA
         REGWR(180,DENALI_CTL_180_DATA,0);
#endif
#ifdef DENALI_CTL_181_DATA
         REGWR(181,DENALI_CTL_181_DATA,0);
#endif
#ifdef DENALI_CTL_182_DATA
         REGWR(182,DENALI_CTL_182_DATA,0);
#endif
#ifdef DENALI_CTL_183_DATA
         REGWR(183,DENALI_CTL_183_DATA,0);
#endif
#ifdef DENALI_CTL_184_DATA
         REGWR(184,DENALI_CTL_184_DATA,0);
#endif
#ifdef DENALI_CTL_185_DATA
         REGWR(185,DENALI_CTL_185_DATA,0);
#endif
#ifdef DENALI_CTL_186_DATA
         REGWR(186,DENALI_CTL_186_DATA,0);
#endif
#ifdef DENALI_CTL_187_DATA
         REGWR(187,DENALI_CTL_187_DATA,0);
#endif
#ifdef DENALI_CTL_188_DATA
         REGWR(188,DENALI_CTL_188_DATA,0);
#endif
#ifdef DENALI_CTL_189_DATA
         REGWR(189,DENALI_CTL_189_DATA,0);
#endif
#ifdef DENALI_CTL_190_DATA
         REGWR(190,DENALI_CTL_190_DATA,0);
#endif
#ifdef DENALI_CTL_191_DATA
         REGWR(191,DENALI_CTL_191_DATA,0);
#endif
#ifdef DENALI_CTL_192_DATA
         REGWR(192,DENALI_CTL_192_DATA,0);
#endif
#ifdef DENALI_CTL_193_DATA
         REGWR(193,DENALI_CTL_193_DATA,0);
#endif
#ifdef DENALI_CTL_194_DATA
         REGWR(194,DENALI_CTL_194_DATA,0);
#endif
#ifdef DENALI_CTL_195_DATA
         REGWR(195,DENALI_CTL_195_DATA,0);
#endif
#ifdef DENALI_CTL_196_DATA
         REGWR(196,DENALI_CTL_196_DATA,0);
#endif
#ifdef DENALI_CTL_197_DATA
         REGWR(197,DENALI_CTL_197_DATA,0);
#endif
#ifdef DENALI_CTL_198_DATA
         REGWR(198,DENALI_CTL_198_DATA,0);
#endif
#ifdef DENALI_CTL_199_DATA
         REGWR(199,DENALI_CTL_199_DATA,0);
#endif
#ifdef DENALI_CTL_200_DATA
         REGWR(200,DENALI_CTL_200_DATA,0);
#endif
#ifdef DENALI_CTL_201_DATA
         REGWR(201,DENALI_CTL_201_DATA,0);
#endif
#ifdef DENALI_CTL_202_DATA
         REGWR(202,DENALI_CTL_202_DATA,0);
#endif
#ifdef DENALI_CTL_203_DATA
         REGWR(203,DENALI_CTL_203_DATA,0);
#endif
#ifdef DENALI_CTL_204_DATA
         REGWR(204,DENALI_CTL_204_DATA,0);
#endif
#ifdef DENALI_CTL_205_DATA
         REGWR(205,DENALI_CTL_205_DATA,0);
#endif
#ifdef DENALI_CTL_206_DATA
         REGWR(206,DENALI_CTL_206_DATA,0);
#endif
#ifdef DENALI_CTL_207_DATA
         REGWR(207,DENALI_CTL_207_DATA,0);
#endif
#ifdef DENALI_CTL_208_DATA
         REGWR(208,DENALI_CTL_208_DATA,0);
#endif
#ifdef DENALI_CTL_209_DATA
         REGWR(209,DENALI_CTL_209_DATA,0);
#endif
#ifdef DENALI_CTL_210_DATA
         REGWR(210,DENALI_CTL_210_DATA,0);
#endif
#ifdef DENALI_CTL_211_DATA
         REGWR(211,DENALI_CTL_211_DATA,0);
#endif
#ifdef DENALI_CTL_212_DATA
         REGWR(212,DENALI_CTL_212_DATA,0);
#endif
#ifdef DENALI_CTL_213_DATA
         REGWR(213,DENALI_CTL_213_DATA,0);
#endif
#ifdef DENALI_CTL_214_DATA
         REGWR(214,DENALI_CTL_214_DATA,0);
#endif
#ifdef DENALI_CTL_215_DATA
         REGWR(215,DENALI_CTL_215_DATA,0);
#endif
#ifdef DENALI_CTL_216_DATA
         REGWR(216,DENALI_CTL_216_DATA,0);
#endif
#ifdef DENALI_CTL_217_DATA
         REGWR(217,DENALI_CTL_217_DATA,0);
#endif
#ifdef DENALI_CTL_218_DATA
         REGWR(218,DENALI_CTL_218_DATA,0);
#endif
#ifdef DENALI_CTL_219_DATA
         REGWR(219,DENALI_CTL_219_DATA,0);
#endif
#ifdef DENALI_CTL_220_DATA
         REGWR(220,DENALI_CTL_220_DATA,0);
#endif
#ifdef DENALI_CTL_221_DATA
         REGWR(221,DENALI_CTL_221_DATA,0);
#endif
#ifdef DENALI_CTL_222_DATA
         REGWR(222,DENALI_CTL_222_DATA,0);
#endif
#ifdef DENALI_CTL_223_DATA
         REGWR(223,DENALI_CTL_223_DATA,0);
#endif
#ifdef DENALI_CTL_224_DATA
         REGWR(224,DENALI_CTL_224_DATA,0);
#endif
#ifdef DENALI_CTL_225_DATA
         REGWR(225,DENALI_CTL_225_DATA,0);
#endif
#ifdef DENALI_CTL_226_DATA
         REGWR(226,DENALI_CTL_226_DATA,0);
#endif
#ifdef DENALI_CTL_227_DATA
         REGWR(227,DENALI_CTL_227_DATA,0);
#endif
#ifdef DENALI_CTL_228_DATA
         REGWR(228,DENALI_CTL_228_DATA,0);
#endif
#ifdef DENALI_CTL_229_DATA
         REGWR(229,DENALI_CTL_229_DATA,0);
#endif
#ifdef DENALI_CTL_230_DATA
         REGWR(230,DENALI_CTL_230_DATA,0);
#endif
#ifdef DENALI_CTL_231_DATA
         REGWR(231,DENALI_CTL_231_DATA,0);
#endif
#ifdef DENALI_CTL_232_DATA
         REGWR(232,DENALI_CTL_232_DATA,0);
#endif
#ifdef DENALI_CTL_233_DATA
         REGWR(233,DENALI_CTL_233_DATA,0);
#endif
#ifdef DENALI_CTL_234_DATA
         REGWR(234,DENALI_CTL_234_DATA,0);
#endif
#ifdef DENALI_CTL_235_DATA
         REGWR(235,DENALI_CTL_235_DATA,0);
#endif
#ifdef DENALI_CTL_236_DATA
         REGWR(236,DENALI_CTL_236_DATA,0);
#endif
#ifdef DENALI_CTL_237_DATA
         REGWR(237,DENALI_CTL_237_DATA,0);
#endif
#ifdef DENALI_CTL_238_DATA
         REGWR(238,DENALI_CTL_238_DATA,0);
#endif
#ifdef DENALI_CTL_239_DATA
         REGWR(239,DENALI_CTL_239_DATA,0);
#endif
#ifdef DENALI_CTL_240_DATA
         REGWR(240,DENALI_CTL_240_DATA,0);
#endif
#ifdef DENALI_CTL_241_DATA
         REGWR(241,DENALI_CTL_241_DATA,0);
#endif
#ifdef DENALI_CTL_242_DATA
         REGWR(242,DENALI_CTL_242_DATA,0);
#endif
#ifdef DENALI_CTL_243_DATA
         REGWR(243,DENALI_CTL_243_DATA,0);
#endif
#ifdef DENALI_CTL_244_DATA
         REGWR(244,DENALI_CTL_244_DATA,0);
#endif
#ifdef DENALI_CTL_245_DATA
         REGWR(245,DENALI_CTL_245_DATA,0);
#endif
#ifdef DENALI_CTL_246_DATA
         REGWR(246,DENALI_CTL_246_DATA,0);
#endif
#ifdef DENALI_CTL_247_DATA
         REGWR(247,DENALI_CTL_247_DATA,0);
#endif
#ifdef DENALI_CTL_248_DATA
         REGWR(248,DENALI_CTL_248_DATA,0);
#endif
#ifdef DENALI_CTL_249_DATA
         REGWR(249,DENALI_CTL_249_DATA,0);
#endif
#ifdef DENALI_CTL_250_DATA
         REGWR(250,DENALI_CTL_250_DATA,0);
#endif
#ifdef DENALI_CTL_251_DATA
         REGWR(251,DENALI_CTL_251_DATA,0);
#endif
#ifdef DENALI_CTL_252_DATA
         REGWR(252,DENALI_CTL_252_DATA,0);
#endif
#ifdef DENALI_CTL_253_DATA
         REGWR(253,DENALI_CTL_253_DATA,0);
#endif
#ifdef DENALI_CTL_254_DATA
         REGWR(254,DENALI_CTL_254_DATA,0);
#endif
#ifdef DENALI_CTL_255_DATA
         REGWR(255,DENALI_CTL_255_DATA,0);
#endif
#ifdef DENALI_CTL_256_DATA
         REGWR(256,DENALI_CTL_256_DATA,0);
#endif
#ifdef DENALI_CTL_257_DATA
         REGWR(257,DENALI_CTL_257_DATA,0);
#endif
#ifdef DENALI_CTL_258_DATA
         REGWR(258,DENALI_CTL_258_DATA,0);
#endif
#ifdef DENALI_CTL_259_DATA
         REGWR(259,DENALI_CTL_259_DATA,0);
#endif
#ifdef DENALI_CTL_260_DATA
         REGWR(260,DENALI_CTL_260_DATA,0);
#endif
#ifdef DENALI_CTL_261_DATA
         REGWR(261,DENALI_CTL_261_DATA,0);
#endif
#ifdef DENALI_CTL_262_DATA
         REGWR(262,DENALI_CTL_262_DATA,0);
#endif
#ifdef DENALI_CTL_263_DATA
         REGWR(263,DENALI_CTL_263_DATA,0);
#endif
#ifdef DENALI_CTL_264_DATA
         REGWR(264,DENALI_CTL_264_DATA,0);
#endif
#ifdef DENALI_CTL_265_DATA
         REGWR(265,DENALI_CTL_265_DATA,0);
#endif
#ifdef DENALI_CTL_266_DATA
         REGWR(266,DENALI_CTL_266_DATA,0);
#endif
#ifdef DENALI_CTL_267_DATA
         REGWR(267,DENALI_CTL_267_DATA,0);
#endif
#ifdef DENALI_CTL_268_DATA
         REGWR(268,DENALI_CTL_268_DATA,0);
#endif
#ifdef DENALI_CTL_269_DATA
         REGWR(269,DENALI_CTL_269_DATA,0);
#endif
#ifdef DENALI_CTL_270_DATA
         REGWR(270,DENALI_CTL_270_DATA,0);
#endif
#ifdef DENALI_CTL_271_DATA
         REGWR(271,DENALI_CTL_271_DATA,0);
#endif
#ifdef DENALI_CTL_272_DATA
         REGWR(272,DENALI_CTL_272_DATA,0);
#endif
#ifdef DENALI_CTL_273_DATA
         REGWR(273,DENALI_CTL_273_DATA,0);
#endif
#ifdef DENALI_CTL_274_DATA
         REGWR(274,DENALI_CTL_274_DATA,0);
#endif
#ifdef DENALI_CTL_275_DATA
         REGWR(275,DENALI_CTL_275_DATA,0);
#endif
#ifdef DENALI_CTL_276_DATA
         REGWR(276,DENALI_CTL_276_DATA,0);
#endif
#ifdef DENALI_CTL_277_DATA
         REGWR(277,DENALI_CTL_277_DATA,0);
#endif
#ifdef DENALI_CTL_278_DATA
         REGWR(278,DENALI_CTL_278_DATA,0);
#endif
#ifdef DENALI_CTL_279_DATA
         REGWR(279,DENALI_CTL_279_DATA,0);
#endif
#ifdef DENALI_CTL_280_DATA
         REGWR(280,DENALI_CTL_280_DATA,0);
#endif
#ifdef DENALI_CTL_281_DATA
         REGWR(281,DENALI_CTL_281_DATA,0);
#endif
#ifdef DENALI_CTL_282_DATA
         REGWR(282,DENALI_CTL_282_DATA,0);
#endif
#ifdef DENALI_CTL_283_DATA
         REGWR(283,DENALI_CTL_283_DATA,0);
#endif
#ifdef DENALI_CTL_284_DATA
         REGWR(284,DENALI_CTL_284_DATA,0);
#endif
#ifdef DENALI_CTL_285_DATA
         REGWR(285,DENALI_CTL_285_DATA,0);
#endif
#ifdef DENALI_CTL_286_DATA
         REGWR(286,DENALI_CTL_286_DATA,0);
#endif
#ifdef DENALI_CTL_287_DATA
         REGWR(287,DENALI_CTL_287_DATA,0);
#endif
#ifdef DENALI_CTL_288_DATA
         REGWR(288,DENALI_CTL_288_DATA,0);
#endif
#ifdef DENALI_CTL_289_DATA
         REGWR(289,DENALI_CTL_289_DATA,0);
#endif
#ifdef DENALI_CTL_290_DATA
         REGWR(290,DENALI_CTL_290_DATA,0);
#endif
#ifdef DENALI_CTL_291_DATA
         REGWR(291,DENALI_CTL_291_DATA,0);
#endif
#ifdef DENALI_CTL_292_DATA
         REGWR(292,DENALI_CTL_292_DATA,0);
#endif
#ifdef DENALI_CTL_293_DATA
         REGWR(293,DENALI_CTL_293_DATA,0);
#endif
#ifdef DENALI_CTL_294_DATA
         REGWR(294,DENALI_CTL_294_DATA,0);
#endif
#ifdef DENALI_CTL_295_DATA
         REGWR(295,DENALI_CTL_295_DATA,0);
#endif
#ifdef DENALI_CTL_296_DATA
         REGWR(296,DENALI_CTL_296_DATA,0);
#endif
#ifdef DENALI_CTL_297_DATA
         REGWR(297,DENALI_CTL_297_DATA,0);
#endif
#ifdef DENALI_CTL_298_DATA
         REGWR(298,DENALI_CTL_298_DATA,0);
#endif
#ifdef DENALI_CTL_299_DATA
         REGWR(299,DENALI_CTL_299_DATA,0);
#endif
#ifdef DENALI_CTL_300_DATA
         REGWR(300,DENALI_CTL_300_DATA,0);
#endif
#ifdef DENALI_CTL_301_DATA
         REGWR(301,DENALI_CTL_301_DATA,0);
#endif
#ifdef DENALI_CTL_302_DATA
         REGWR(302,DENALI_CTL_302_DATA,0);
#endif
#ifdef DENALI_CTL_303_DATA
         REGWR(303,DENALI_CTL_303_DATA,0);
#endif
#ifdef DENALI_CTL_304_DATA
         REGWR(304,DENALI_CTL_304_DATA,0);
#endif
#ifdef DENALI_CTL_305_DATA
         REGWR(305,DENALI_CTL_305_DATA,0);
#endif
#ifdef DENALI_CTL_306_DATA
         REGWR(306,DENALI_CTL_306_DATA,0);
#endif
#ifdef DENALI_CTL_307_DATA
         REGWR(307,DENALI_CTL_307_DATA,0);
#endif
#ifdef DENALI_CTL_308_DATA
         REGWR(308,DENALI_CTL_308_DATA,0);
#endif
#ifdef DENALI_CTL_309_DATA
         REGWR(309,DENALI_CTL_309_DATA,0);
#endif
#ifdef DENALI_CTL_310_DATA
         REGWR(310,DENALI_CTL_310_DATA,0);
#endif
#ifdef DENALI_CTL_311_DATA
         REGWR(311,DENALI_CTL_311_DATA,0);
#endif
#ifdef DENALI_CTL_312_DATA
         REGWR(312,DENALI_CTL_312_DATA,0);
#endif
#ifdef DENALI_CTL_313_DATA
         REGWR(313,DENALI_CTL_313_DATA,0);
#endif
#ifdef DENALI_CTL_314_DATA
         REGWR(314,DENALI_CTL_314_DATA,0);
#endif
#ifdef DENALI_CTL_315_DATA
         REGWR(315,DENALI_CTL_315_DATA,0);
#endif
#ifdef DENALI_CTL_316_DATA
         REGWR(316,DENALI_CTL_316_DATA,0);
#endif
#ifdef DENALI_CTL_317_DATA
         REGWR(317,DENALI_CTL_317_DATA,0);
#endif
#ifdef DENALI_CTL_318_DATA
         REGWR(318,DENALI_CTL_318_DATA,0);
#endif
#ifdef DENALI_CTL_319_DATA
         REGWR(319,DENALI_CTL_319_DATA,0);
#endif
#ifdef DENALI_CTL_320_DATA
         REGWR(320,DENALI_CTL_320_DATA,0);
#endif
#ifdef DENALI_CTL_321_DATA
         REGWR(321,DENALI_CTL_321_DATA,0);
#endif
#ifdef DENALI_CTL_322_DATA
         REGWR(322,DENALI_CTL_322_DATA,0);
#endif
#ifdef DENALI_CTL_323_DATA
         REGWR(323,DENALI_CTL_323_DATA,0);
#endif
#ifdef DENALI_CTL_324_DATA
         REGWR(324,DENALI_CTL_324_DATA,0);
#endif
#ifdef DENALI_CTL_325_DATA
         REGWR(325,DENALI_CTL_325_DATA,0);
#endif
#ifdef DENALI_CTL_326_DATA
         REGWR(326,DENALI_CTL_326_DATA,0);
#endif
#ifdef DENALI_CTL_327_DATA
         REGWR(327,DENALI_CTL_327_DATA,0);
#endif
#ifdef DENALI_CTL_328_DATA
         REGWR(328,DENALI_CTL_328_DATA,0);
#endif
#ifdef DENALI_CTL_329_DATA
         REGWR(329,DENALI_CTL_329_DATA,0);
#endif
#ifdef DENALI_CTL_330_DATA
         REGWR(330,DENALI_CTL_330_DATA,0);
#endif
#ifdef DENALI_CTL_331_DATA
         REGWR(331,DENALI_CTL_331_DATA,0);
#endif
#ifdef DENALI_CTL_332_DATA
         REGWR(332,DENALI_CTL_332_DATA,0);
#endif
#ifdef DENALI_CTL_333_DATA
         REGWR(333,DENALI_CTL_333_DATA,0);
#endif
#ifdef DENALI_CTL_334_DATA
         REGWR(334,DENALI_CTL_334_DATA,0);
#endif
#ifdef DENALI_CTL_335_DATA
         REGWR(335,DENALI_CTL_335_DATA,0);
#endif
#ifdef DENALI_CTL_336_DATA
         REGWR(336,DENALI_CTL_336_DATA,0);
#endif
#ifdef DENALI_CTL_337_DATA
         REGWR(337,DENALI_CTL_337_DATA,0);
#endif
#ifdef DENALI_CTL_338_DATA
         REGWR(338,DENALI_CTL_338_DATA,0);
#endif
#ifdef DENALI_CTL_339_DATA
         REGWR(339,DENALI_CTL_339_DATA,0);
#endif
#ifdef DENALI_CTL_340_DATA
         REGWR(340,DENALI_CTL_340_DATA,0);
#endif
#ifdef DENALI_CTL_341_DATA
         REGWR(341,DENALI_CTL_341_DATA,0);
#endif
#ifdef DENALI_CTL_342_DATA
         REGWR(342,DENALI_CTL_342_DATA,0);
#endif
#ifdef DENALI_CTL_343_DATA
         REGWR(343,DENALI_CTL_343_DATA,0);
#endif
#ifdef DENALI_CTL_344_DATA
         REGWR(344,DENALI_CTL_344_DATA,0);
#endif
#ifdef DENALI_CTL_345_DATA
         REGWR(345,DENALI_CTL_345_DATA,0);
#endif
#ifdef DENALI_CTL_346_DATA
         REGWR(346,DENALI_CTL_346_DATA,0);
#endif
#ifdef DENALI_CTL_347_DATA
         REGWR(347,DENALI_CTL_347_DATA,0);
#endif
#ifdef DENALI_CTL_348_DATA
         REGWR(348,DENALI_CTL_348_DATA,0);
#endif
#ifdef DENALI_CTL_349_DATA
         REGWR(349,DENALI_CTL_349_DATA,0);
#endif
#ifdef DENALI_CTL_350_DATA
         REGWR(350,DENALI_CTL_350_DATA,0);
#endif
#ifdef DENALI_CTL_351_DATA
         REGWR(351,DENALI_CTL_351_DATA,0);
#endif
#ifdef DENALI_CTL_352_DATA
         REGWR(352,DENALI_CTL_352_DATA,0);
#endif
#ifdef DENALI_CTL_353_DATA
         REGWR(353,DENALI_CTL_353_DATA,0);
#endif
#ifdef DENALI_CTL_354_DATA
         REGWR(354,DENALI_CTL_354_DATA,0);
#endif
#ifdef DENALI_CTL_355_DATA
         REGWR(355,DENALI_CTL_355_DATA,0);
#endif
#ifdef DENALI_CTL_356_DATA
         REGWR(356,DENALI_CTL_356_DATA,0);
#endif
#ifdef DENALI_CTL_357_DATA
         REGWR(357,DENALI_CTL_357_DATA,0);
#endif
#ifdef DENALI_CTL_358_DATA
         REGWR(358,DENALI_CTL_358_DATA,0);
#endif
#ifdef DENALI_CTL_359_DATA
         REGWR(359,DENALI_CTL_359_DATA,0);
#endif
#ifdef DENALI_CTL_360_DATA
         REGWR(360,DENALI_CTL_360_DATA,0);
#endif
#ifdef DENALI_CTL_361_DATA
         REGWR(361,DENALI_CTL_361_DATA,0);
#endif
#ifdef DENALI_CTL_362_DATA
         REGWR(362,DENALI_CTL_362_DATA,0);
#endif
#ifdef DENALI_CTL_363_DATA
         REGWR(363,DENALI_CTL_363_DATA,0);
#endif
#ifdef DENALI_CTL_364_DATA
         REGWR(364,DENALI_CTL_364_DATA,0);
#endif
#ifdef DENALI_CTL_365_DATA
         REGWR(365,DENALI_CTL_365_DATA,0);
#endif
#ifdef DENALI_CTL_366_DATA
         REGWR(366,DENALI_CTL_366_DATA,0);
#endif
#ifdef DENALI_CTL_367_DATA
         REGWR(367,DENALI_CTL_367_DATA,0);
#endif
#ifdef DENALI_CTL_368_DATA
         REGWR(368,DENALI_CTL_368_DATA,0);
#endif
#ifdef DENALI_CTL_369_DATA
         REGWR(369,DENALI_CTL_369_DATA,0);
#endif
#ifdef DENALI_CTL_370_DATA
         REGWR(370,DENALI_CTL_370_DATA,0);
#endif
#ifdef DENALI_CTL_371_DATA
         REGWR(371,DENALI_CTL_371_DATA,0);
#endif
#ifdef DENALI_CTL_372_DATA
         REGWR(372,DENALI_CTL_372_DATA,0);
#endif
#ifdef DENALI_CTL_373_DATA
         REGWR(373,DENALI_CTL_373_DATA,0);
#endif
#ifdef DENALI_CTL_374_DATA
         REGWR(374,DENALI_CTL_374_DATA,0);
#endif
#ifdef DENALI_CTL_375_DATA
         REGWR(375,DENALI_CTL_375_DATA,0);
#endif
#ifdef DENALI_CTL_376_DATA
         REGWR(376,DENALI_CTL_376_DATA,0);
#endif
#ifdef DENALI_CTL_377_DATA
         REGWR(377,DENALI_CTL_377_DATA,0);
#endif
#ifdef DENALI_CTL_378_DATA
         REGWR(378,DENALI_CTL_378_DATA,0);
#endif
#ifdef DENALI_CTL_379_DATA
         REGWR(379,DENALI_CTL_379_DATA,0);
#endif
#ifdef DENALI_CTL_380_DATA
         REGWR(380,DENALI_CTL_380_DATA,0);
#endif
#ifdef DENALI_CTL_381_DATA
         REGWR(381,DENALI_CTL_381_DATA,0);
#endif
#ifdef DENALI_CTL_382_DATA
         REGWR(382,DENALI_CTL_382_DATA,0);
#endif
#ifdef DENALI_CTL_383_DATA
         REGWR(383,DENALI_CTL_383_DATA,0);
#endif
#ifdef DENALI_CTL_384_DATA
         REGWR(384,DENALI_CTL_384_DATA,0);
#endif
#ifdef DENALI_CTL_385_DATA
         REGWR(385,DENALI_CTL_385_DATA,0);
#endif
#ifdef DENALI_CTL_386_DATA
         REGWR(386,DENALI_CTL_386_DATA,0);
#endif
#ifdef DENALI_CTL_387_DATA
         REGWR(387,DENALI_CTL_387_DATA,0);
#endif
#ifdef DENALI_CTL_388_DATA
         REGWR(388,DENALI_CTL_388_DATA,0);
#endif
#ifdef DENALI_CTL_389_DATA
         REGWR(389,DENALI_CTL_389_DATA,0);
#endif
#ifdef DENALI_CTL_390_DATA
         REGWR(390,DENALI_CTL_390_DATA,0);
#endif
#ifdef DENALI_CTL_391_DATA
         REGWR(391,DENALI_CTL_391_DATA,0);
#endif
#ifdef DENALI_CTL_392_DATA
         REGWR(392,DENALI_CTL_392_DATA,0);
#endif
#ifdef DENALI_CTL_393_DATA
         REGWR(393,DENALI_CTL_393_DATA,0);
#endif
#ifdef DENALI_CTL_394_DATA
         REGWR(394,DENALI_CTL_394_DATA,0);
#endif
#ifdef DENALI_CTL_395_DATA
         REGWR(395,DENALI_CTL_395_DATA,0);
#endif
#ifdef DENALI_CTL_396_DATA
         REGWR(396,DENALI_CTL_396_DATA,0);
#endif
#ifdef DENALI_CTL_397_DATA
         REGWR(397,DENALI_CTL_397_DATA,0);
#endif
#ifdef DENALI_CTL_398_DATA
         REGWR(398,DENALI_CTL_398_DATA,0);
#endif
#ifdef DENALI_CTL_399_DATA
         REGWR(399,DENALI_CTL_399_DATA,0);
#endif
#ifdef DENALI_CTL_400_DATA
         REGWR(400,DENALI_CTL_400_DATA,0);
#endif
#ifdef DENALI_CTL_401_DATA
         REGWR(401,DENALI_CTL_401_DATA,0);
#endif
#ifdef DENALI_CTL_402_DATA
         REGWR(402,DENALI_CTL_402_DATA,0);
#endif
#ifdef DENALI_CTL_403_DATA
         REGWR(403,DENALI_CTL_403_DATA,0);
#endif
#ifdef DENALI_CTL_404_DATA
         REGWR(404,DENALI_CTL_404_DATA,0);
#endif
#ifdef DENALI_CTL_405_DATA
         REGWR(405,DENALI_CTL_405_DATA,0);
#endif
#ifdef DENALI_CTL_406_DATA
         REGWR(406,DENALI_CTL_406_DATA,0);
#endif
#ifdef DENALI_CTL_407_DATA
         REGWR(407,DENALI_CTL_407_DATA,0);
#endif
#ifdef DENALI_CTL_408_DATA
         REGWR(408,DENALI_CTL_408_DATA,0);
#endif
#ifdef DENALI_CTL_409_DATA
         REGWR(409,DENALI_CTL_409_DATA,0);
#endif
#ifdef DENALI_CTL_410_DATA
         REGWR(410,DENALI_CTL_410_DATA,0);
#endif
#ifdef DENALI_CTL_411_DATA
         REGWR(411,DENALI_CTL_411_DATA,0);
#endif
#ifdef DENALI_CTL_412_DATA
         REGWR(412,DENALI_CTL_412_DATA,0);
#endif
#ifdef DENALI_CTL_413_DATA
         REGWR(413,DENALI_CTL_413_DATA,0);
#endif
#ifdef DENALI_CTL_414_DATA
         REGWR(414,DENALI_CTL_414_DATA,0);
#endif
#ifdef DENALI_CTL_415_DATA
         REGWR(415,DENALI_CTL_415_DATA,0);
#endif
#ifdef DENALI_CTL_416_DATA
         REGWR(416,DENALI_CTL_416_DATA,0);
#endif
#ifdef DENALI_CTL_417_DATA
         REGWR(417,DENALI_CTL_417_DATA,0);
#endif
#ifdef DENALI_CTL_418_DATA
         REGWR(418,DENALI_CTL_418_DATA,0);
#endif
#ifdef DENALI_CTL_419_DATA
         REGWR(419,DENALI_CTL_419_DATA,0);
#endif
#ifdef DENALI_CTL_420_DATA
         REGWR(420,DENALI_CTL_420_DATA,0);
#endif
#ifdef DENALI_CTL_421_DATA
         REGWR(421,DENALI_CTL_421_DATA,0);
#endif
#ifdef DENALI_CTL_422_DATA
         REGWR(422,DENALI_CTL_422_DATA,0);
#endif
#ifdef DENALI_CTL_423_DATA
         REGWR(423,DENALI_CTL_423_DATA,0);
#endif
#ifdef DENALI_CTL_424_DATA
         REGWR(424,DENALI_CTL_424_DATA,0);
#endif
#ifdef DENALI_CTL_425_DATA
         REGWR(425,DENALI_CTL_425_DATA,0);
#endif
#ifdef DENALI_CTL_426_DATA
         REGWR(426,DENALI_CTL_426_DATA,0);
#endif
#ifdef DENALI_CTL_427_DATA
         REGWR(427,DENALI_CTL_427_DATA,0);
#endif
#ifdef DENALI_CTL_428_DATA
         REGWR(428,DENALI_CTL_428_DATA,0);
#endif
#ifdef DENALI_CTL_429_DATA
         REGWR(429,DENALI_CTL_429_DATA,0);
#endif
#ifdef DENALI_CTL_430_DATA
         REGWR(430,DENALI_CTL_430_DATA,0);
#endif
#ifdef DENALI_CTL_431_DATA
         REGWR(431,DENALI_CTL_431_DATA,0);
#endif
#ifdef DENALI_CTL_432_DATA
         REGWR(432,DENALI_CTL_432_DATA,0);
#endif
#ifdef DENALI_CTL_433_DATA
         REGWR(433,DENALI_CTL_433_DATA,0);
#endif
#ifdef DENALI_CTL_434_DATA
         REGWR(434,DENALI_CTL_434_DATA,0);
#endif
#ifdef DENALI_CTL_435_DATA
         REGWR(435,DENALI_CTL_435_DATA,0);
#endif
#ifdef DENALI_CTL_436_DATA
         REGWR(436,DENALI_CTL_436_DATA,0);
#endif
#ifdef DENALI_CTL_437_DATA
         REGWR(437,DENALI_CTL_437_DATA,0);
#endif
#ifdef DENALI_CTL_438_DATA
         REGWR(438,DENALI_CTL_438_DATA,0);
#endif
#ifdef DENALI_CTL_439_DATA
         REGWR(439,DENALI_CTL_439_DATA,0);
#endif
#ifdef DENALI_CTL_440_DATA
         REGWR(440,DENALI_CTL_440_DATA,0);
#endif
#ifdef DENALI_CTL_441_DATA
         REGWR(441,DENALI_CTL_441_DATA,0);
#endif
#ifdef DENALI_CTL_442_DATA
         REGWR(442,DENALI_CTL_442_DATA,0);
#endif
#ifdef DENALI_CTL_443_DATA
         REGWR(443,DENALI_CTL_443_DATA,0);
#endif
#ifdef DENALI_CTL_444_DATA
         REGWR(444,DENALI_CTL_444_DATA,0);
#endif
#ifdef DENALI_CTL_445_DATA
         REGWR(445,DENALI_CTL_445_DATA,0);
#endif
#ifdef DENALI_CTL_446_DATA
         REGWR(446,DENALI_CTL_446_DATA,0);
#endif
#ifdef DENALI_CTL_447_DATA
         REGWR(447,DENALI_CTL_447_DATA,0);
#endif
#ifdef DENALI_CTL_448_DATA
         REGWR(448,DENALI_CTL_448_DATA,0);
#endif
#ifdef DENALI_CTL_449_DATA
         REGWR(449,DENALI_CTL_449_DATA,0);
#endif
#ifdef DENALI_CTL_450_DATA
         REGWR(450,DENALI_CTL_450_DATA,0);
#endif
#ifdef DENALI_CTL_451_DATA
         REGWR(451,DENALI_CTL_451_DATA,0);
#endif
#ifdef DENALI_CTL_452_DATA
         REGWR(452,DENALI_CTL_452_DATA,0);
#endif
#ifdef DENALI_CTL_453_DATA
         REGWR(453,DENALI_CTL_453_DATA,0);
#endif
#ifdef DENALI_CTL_454_DATA
         REGWR(454,DENALI_CTL_454_DATA,0);
#endif
#ifdef DENALI_CTL_455_DATA
         REGWR(455,DENALI_CTL_455_DATA,0);
#endif
#ifdef DENALI_CTL_456_DATA
         REGWR(456,DENALI_CTL_456_DATA,0);
#endif
#ifdef DENALI_CTL_457_DATA
         REGWR(457,DENALI_CTL_457_DATA,0);
#endif
#ifdef DENALI_CTL_458_DATA
         REGWR(458,DENALI_CTL_458_DATA,0);
#endif
#ifdef DENALI_CTL_459_DATA
         REGWR(459,DENALI_CTL_459_DATA,0);
#endif
#ifdef DENALI_CTL_460_DATA
         REGWR(460,DENALI_CTL_460_DATA,0);
#endif
#ifdef DENALI_CTL_461_DATA
         REGWR(461,DENALI_CTL_461_DATA,0);
#endif
#ifdef DENALI_CTL_462_DATA
         REGWR(462,DENALI_CTL_462_DATA,0);
#endif
#ifdef DENALI_CTL_463_DATA
         REGWR(463,DENALI_CTL_463_DATA,0);
#endif
#ifdef DENALI_CTL_464_DATA
         REGWR(464,DENALI_CTL_464_DATA,0);
#endif
#ifdef DENALI_CTL_465_DATA
         REGWR(465,DENALI_CTL_465_DATA,0);
#endif
#ifdef DENALI_CTL_466_DATA
         REGWR(466,DENALI_CTL_466_DATA,0);
#endif
#ifdef DENALI_CTL_467_DATA
         REGWR(467,DENALI_CTL_467_DATA,0);
#endif
#ifdef DENALI_CTL_468_DATA
         REGWR(468,DENALI_CTL_468_DATA,0);
#endif
#ifdef DENALI_CTL_469_DATA
         REGWR(469,DENALI_CTL_469_DATA,0);
#endif
#ifdef DENALI_CTL_470_DATA
         REGWR(470,DENALI_CTL_470_DATA,0);
#endif
#ifdef DENALI_CTL_471_DATA
         REGWR(471,DENALI_CTL_471_DATA,0);
#endif
#ifdef DENALI_CTL_472_DATA
         REGWR(472,DENALI_CTL_472_DATA,0);
#endif
#ifdef DENALI_CTL_473_DATA
         REGWR(473,DENALI_CTL_473_DATA,0);
#endif
#ifdef DENALI_CTL_474_DATA
         REGWR(474,DENALI_CTL_474_DATA,0);
#endif
#ifdef DENALI_CTL_475_DATA
         REGWR(475,DENALI_CTL_475_DATA,0);
#endif
#ifdef DENALI_CTL_476_DATA
         REGWR(476,DENALI_CTL_476_DATA,0);
#endif
#ifdef DENALI_CTL_477_DATA
         REGWR(477,DENALI_CTL_477_DATA,0);
#endif
#ifdef DENALI_CTL_478_DATA
         REGWR(478,DENALI_CTL_478_DATA,0);
#endif
#ifdef DENALI_CTL_479_DATA
         REGWR(479,DENALI_CTL_479_DATA,0);
#endif
#ifdef DENALI_CTL_480_DATA
         REGWR(480,DENALI_CTL_480_DATA,0);
#endif
#ifdef DENALI_CTL_481_DATA
         REGWR(481,DENALI_CTL_481_DATA,0);
#endif
#ifdef DENALI_CTL_482_DATA
         REGWR(482,DENALI_CTL_482_DATA,0);
#endif
#ifdef DENALI_CTL_483_DATA
         REGWR(483,DENALI_CTL_483_DATA,0);
#endif
#ifdef DENALI_CTL_484_DATA
         REGWR(484,DENALI_CTL_484_DATA,0);
#endif
#ifdef DENALI_CTL_485_DATA
         REGWR(485,DENALI_CTL_485_DATA,0);
#endif
#ifdef DENALI_CTL_486_DATA
         REGWR(486,DENALI_CTL_486_DATA,0);
#endif
#ifdef DENALI_CTL_487_DATA
         REGWR(487,DENALI_CTL_487_DATA,0);
#endif
#ifdef DENALI_CTL_488_DATA
         REGWR(488,DENALI_CTL_488_DATA,0);
#endif
#ifdef DENALI_CTL_489_DATA
         REGWR(489,DENALI_CTL_489_DATA,0);
#endif
#ifdef DENALI_CTL_490_DATA
         REGWR(490,DENALI_CTL_490_DATA,0);
#endif
#ifdef DENALI_CTL_491_DATA
         REGWR(491,DENALI_CTL_491_DATA,0);
#endif
#ifdef DENALI_CTL_492_DATA
         REGWR(492,DENALI_CTL_492_DATA,0);
#endif
#ifdef DENALI_CTL_493_DATA
         REGWR(493,DENALI_CTL_493_DATA,0);
#endif
#ifdef DENALI_CTL_494_DATA
         REGWR(494,DENALI_CTL_494_DATA,0);
#endif
#ifdef DENALI_CTL_495_DATA
         REGWR(495,DENALI_CTL_495_DATA,0);
#endif
#ifdef DENALI_CTL_496_DATA
         REGWR(496,DENALI_CTL_496_DATA,0);
#endif
#ifdef DENALI_CTL_497_DATA
         REGWR(497,DENALI_CTL_497_DATA,0);
#endif
#ifdef DENALI_CTL_498_DATA
         REGWR(498,DENALI_CTL_498_DATA,0);
#endif
#ifdef DENALI_CTL_499_DATA
         REGWR(499,DENALI_CTL_499_DATA,0);
#endif
#ifdef DENALI_CTL_500_DATA
         REGWR(500,DENALI_CTL_500_DATA,0);
#endif
#ifdef DENALI_CTL_501_DATA
         REGWR(501,DENALI_CTL_501_DATA,0);
#endif
#ifdef DENALI_CTL_502_DATA
         REGWR(502,DENALI_CTL_502_DATA,0);
#endif
#ifdef DENALI_CTL_503_DATA
         REGWR(503,DENALI_CTL_503_DATA,0);
#endif
#ifdef DENALI_CTL_504_DATA
         REGWR(504,DENALI_CTL_504_DATA,0);
#endif
#ifdef DENALI_CTL_505_DATA
         REGWR(505,DENALI_CTL_505_DATA,0);
#endif
#ifdef DENALI_CTL_506_DATA
         REGWR(506,DENALI_CTL_506_DATA,0);
#endif
#ifdef DENALI_CTL_507_DATA
         REGWR(507,DENALI_CTL_507_DATA,0);
#endif
#ifdef DENALI_CTL_508_DATA
         REGWR(508,DENALI_CTL_508_DATA,0);
#endif
#ifdef DENALI_CTL_509_DATA
         REGWR(509,DENALI_CTL_509_DATA,0);
#endif
#ifdef DENALI_CTL_510_DATA
         REGWR(510,DENALI_CTL_510_DATA,0);
#endif
#ifdef DENALI_CTL_511_DATA
         REGWR(511,DENALI_CTL_511_DATA,0);
#endif
#ifdef DENALI_CTL_512_DATA
         REGWR(512,DENALI_CTL_512_DATA,0);
#endif
#ifdef DENALI_CTL_513_DATA
         REGWR(513,DENALI_CTL_513_DATA,0);
#endif
#ifdef DENALI_CTL_514_DATA
         REGWR(514,DENALI_CTL_514_DATA,0);
#endif
#ifdef DENALI_CTL_515_DATA
         REGWR(515,DENALI_CTL_515_DATA,0);
#endif
#ifdef DENALI_CTL_516_DATA
         REGWR(516,DENALI_CTL_516_DATA,0);
#endif
#ifdef DENALI_CTL_517_DATA
         REGWR(517,DENALI_CTL_517_DATA,0);
#endif
#ifdef DENALI_CTL_518_DATA
         REGWR(518,DENALI_CTL_518_DATA,0);
#endif
#ifdef DENALI_CTL_519_DATA
         REGWR(519,DENALI_CTL_519_DATA,0);
#endif
#ifdef DENALI_CTL_520_DATA
         REGWR(520,DENALI_CTL_520_DATA,0);
#endif
#ifdef DENALI_CTL_521_DATA
         REGWR(521,DENALI_CTL_521_DATA,0);
#endif
#ifdef DENALI_CTL_522_DATA
         REGWR(522,DENALI_CTL_522_DATA,0);
#endif
#ifdef DENALI_CTL_523_DATA
         REGWR(523,DENALI_CTL_523_DATA,0);
#endif
#ifdef DENALI_CTL_524_DATA
         REGWR(524,DENALI_CTL_524_DATA,0);
#endif
#ifdef DENALI_CTL_525_DATA
         REGWR(525,DENALI_CTL_525_DATA,0);
#endif
#ifdef DENALI_CTL_526_DATA
         REGWR(526,DENALI_CTL_526_DATA,0);
#endif
#ifdef DENALI_CTL_527_DATA
         REGWR(527,DENALI_CTL_527_DATA,0);
#endif
#ifdef DENALI_CTL_528_DATA
         REGWR(528,DENALI_CTL_528_DATA,0);
#endif
#ifdef DENALI_CTL_529_DATA
         REGWR(529,DENALI_CTL_529_DATA,0);
#endif
#ifdef DENALI_CTL_530_DATA
         REGWR(530,DENALI_CTL_530_DATA,0);
#endif
#ifdef DENALI_CTL_531_DATA
         REGWR(531,DENALI_CTL_531_DATA,0);
#endif
#ifdef DENALI_CTL_532_DATA
         REGWR(532,DENALI_CTL_532_DATA,0);
#endif
#ifdef DENALI_CTL_533_DATA
         REGWR(533,DENALI_CTL_533_DATA,0);
#endif
#ifdef DENALI_CTL_534_DATA
         REGWR(534,DENALI_CTL_534_DATA,0);
#endif
#ifdef DENALI_CTL_535_DATA
         REGWR(535,DENALI_CTL_535_DATA,0);
#endif
#ifdef DENALI_CTL_536_DATA
         REGWR(536,DENALI_CTL_536_DATA,0);
#endif
#ifdef DENALI_CTL_537_DATA
         REGWR(537,DENALI_CTL_537_DATA,0);
#endif
#ifdef DENALI_CTL_538_DATA
         REGWR(538,DENALI_CTL_538_DATA,0);
#endif
#ifdef DENALI_CTL_539_DATA
         REGWR(539,DENALI_CTL_539_DATA,0);
#endif
#ifdef DENALI_CTL_540_DATA
         REGWR(540,DENALI_CTL_540_DATA,0);
#endif
#ifdef DENALI_CTL_541_DATA
         REGWR(541,DENALI_CTL_541_DATA,0);
#endif
#ifdef DENALI_CTL_542_DATA
         REGWR(542,DENALI_CTL_542_DATA,0);
#endif
#ifdef DENALI_CTL_543_DATA
         REGWR(543,DENALI_CTL_543_DATA,0);
#endif
#ifdef DENALI_CTL_544_DATA
         REGWR(544,DENALI_CTL_544_DATA,0);
#endif
#ifdef DENALI_CTL_545_DATA
         REGWR(545,DENALI_CTL_545_DATA,0);
#endif
#ifdef DENALI_CTL_546_DATA
         REGWR(546,DENALI_CTL_546_DATA,0);
#endif
#ifdef DENALI_CTL_547_DATA
         REGWR(547,DENALI_CTL_547_DATA,0);
#endif
#ifdef DENALI_CTL_548_DATA
         REGWR(548,DENALI_CTL_548_DATA,0);
#endif
#ifdef DENALI_CTL_549_DATA
         REGWR(549,DENALI_CTL_549_DATA,0);
#endif
#ifdef DENALI_CTL_550_DATA
         REGWR(550,DENALI_CTL_550_DATA,0);
#endif
#ifdef DENALI_CTL_551_DATA
         REGWR(551,DENALI_CTL_551_DATA,0);
#endif
#ifdef DENALI_CTL_552_DATA
         REGWR(552,DENALI_CTL_552_DATA,0);
#endif
#ifdef DENALI_CTL_553_DATA
         REGWR(553,DENALI_CTL_553_DATA,0);
#endif
#ifdef DENALI_CTL_554_DATA
         REGWR(554,DENALI_CTL_554_DATA,0);
#endif
#ifdef DENALI_CTL_555_DATA
         REGWR(555,DENALI_CTL_555_DATA,0);
#endif
#ifdef DENALI_CTL_556_DATA
         REGWR(556,DENALI_CTL_556_DATA,0);
#endif
#ifdef DENALI_CTL_557_DATA
         REGWR(557,DENALI_CTL_557_DATA,0);
#endif
#ifdef DENALI_CTL_558_DATA
         REGWR(558,DENALI_CTL_558_DATA,0);
#endif
#ifdef DENALI_CTL_559_DATA
         REGWR(559,DENALI_CTL_559_DATA,0);
#endif
#ifdef DENALI_CTL_560_DATA
         REGWR(560,DENALI_CTL_560_DATA,0);
#endif
#ifdef DENALI_CTL_561_DATA
         REGWR(561,DENALI_CTL_561_DATA,0);
#endif
#ifdef DENALI_CTL_562_DATA
         REGWR(562,DENALI_CTL_562_DATA,0);
#endif
#ifdef DENALI_CTL_563_DATA
         REGWR(563,DENALI_CTL_563_DATA,0);
#endif
#ifdef DENALI_CTL_564_DATA
         REGWR(564,DENALI_CTL_564_DATA,0);
#endif
#ifdef DENALI_CTL_565_DATA
         REGWR(565,DENALI_CTL_565_DATA,0);
#endif
#ifdef DENALI_CTL_566_DATA
         REGWR(566,DENALI_CTL_566_DATA,0);
#endif
#ifdef DENALI_CTL_567_DATA
         REGWR(567,DENALI_CTL_567_DATA,0);
#endif
#ifdef DENALI_CTL_568_DATA
         REGWR(568,DENALI_CTL_568_DATA,0);
#endif
#ifdef DENALI_CTL_569_DATA
         REGWR(569,DENALI_CTL_569_DATA,0);
#endif
#ifdef DENALI_CTL_570_DATA
         REGWR(570,DENALI_CTL_570_DATA,0);
#endif
#ifdef DENALI_CTL_571_DATA
         REGWR(571,DENALI_CTL_571_DATA,0);
#endif
#ifdef DENALI_CTL_572_DATA
         REGWR(572,DENALI_CTL_572_DATA,0);
#endif
#ifdef DENALI_CTL_573_DATA
         REGWR(573,DENALI_CTL_573_DATA,0);
#endif
#ifdef DENALI_CTL_574_DATA
         REGWR(574,DENALI_CTL_574_DATA,0);
#endif
#ifdef DENALI_CTL_575_DATA
         REGWR(575,DENALI_CTL_575_DATA,0);
#endif
#ifdef DENALI_CTL_576_DATA
         REGWR(576,DENALI_CTL_576_DATA,0);
#endif
#ifdef DENALI_CTL_577_DATA
         REGWR(577,DENALI_CTL_577_DATA,0);
#endif
#ifdef DENALI_CTL_578_DATA
         REGWR(578,DENALI_CTL_578_DATA,0);
#endif
#ifdef DENALI_CTL_579_DATA
         REGWR(579,DENALI_CTL_579_DATA,0);
#endif
#ifdef DENALI_CTL_580_DATA
         REGWR(580,DENALI_CTL_580_DATA,0);
#endif
#ifdef DENALI_CTL_581_DATA
         REGWR(581,DENALI_CTL_581_DATA,0);
#endif
#ifdef DENALI_CTL_582_DATA
         REGWR(582,DENALI_CTL_582_DATA,0);
#endif
#ifdef DENALI_CTL_583_DATA
         REGWR(583,DENALI_CTL_583_DATA,0);
#endif
#ifdef DENALI_CTL_584_DATA
         REGWR(584,DENALI_CTL_584_DATA,0);
#endif
#ifdef DENALI_CTL_585_DATA
         REGWR(585,DENALI_CTL_585_DATA,0);
#endif
#ifdef DENALI_CTL_586_DATA
         REGWR(586,DENALI_CTL_586_DATA,0);
#endif
#ifdef DENALI_CTL_587_DATA
         REGWR(587,DENALI_CTL_587_DATA,0);
#endif
#ifdef DENALI_CTL_588_DATA
         REGWR(588,DENALI_CTL_588_DATA,0);
#endif
#ifdef DENALI_CTL_589_DATA
         REGWR(589,DENALI_CTL_589_DATA,0);
#endif
#ifdef DENALI_CTL_590_DATA
         REGWR(590,DENALI_CTL_590_DATA,0);
#endif
#ifdef DENALI_CTL_591_DATA
         REGWR(591,DENALI_CTL_591_DATA,0);
#endif
#ifdef DENALI_CTL_592_DATA
         REGWR(592,DENALI_CTL_592_DATA,0);
#endif
#ifdef DENALI_CTL_593_DATA
         REGWR(593,DENALI_CTL_593_DATA,0);
#endif
#ifdef DENALI_CTL_594_DATA
         REGWR(594,DENALI_CTL_594_DATA,0);
#endif
#ifdef DENALI_CTL_595_DATA
         REGWR(595,DENALI_CTL_595_DATA,0);
#endif
#ifdef DENALI_CTL_596_DATA
         REGWR(596,DENALI_CTL_596_DATA,0);
#endif
#ifdef DENALI_CTL_597_DATA
         REGWR(597,DENALI_CTL_597_DATA,0);
#endif
#ifdef DENALI_CTL_598_DATA
         REGWR(598,DENALI_CTL_598_DATA,0);
#endif
#ifdef DENALI_CTL_599_DATA
         REGWR(599,DENALI_CTL_599_DATA,0);
#endif
#ifdef DENALI_CTL_600_DATA
         REGWR(600,DENALI_CTL_600_DATA,0);
#endif
#ifdef DENALI_CTL_601_DATA
         REGWR(601,DENALI_CTL_601_DATA,0);
#endif
#ifdef DENALI_CTL_602_DATA
         REGWR(602,DENALI_CTL_602_DATA,0);
#endif
#ifdef DENALI_CTL_603_DATA
         REGWR(603,DENALI_CTL_603_DATA,0);
#endif
#ifdef DENALI_CTL_604_DATA
         REGWR(604,DENALI_CTL_604_DATA,0);
#endif
#ifdef DENALI_CTL_605_DATA
         REGWR(605,DENALI_CTL_605_DATA,0);
#endif
#ifdef DENALI_CTL_606_DATA
         REGWR(606,DENALI_CTL_606_DATA,0);
#endif
#ifdef DENALI_CTL_607_DATA
         REGWR(607,DENALI_CTL_607_DATA,0);
#endif
#ifdef DENALI_CTL_608_DATA
         REGWR(608,DENALI_CTL_608_DATA,0);
#endif
#ifdef DENALI_CTL_609_DATA
         REGWR(609,DENALI_CTL_609_DATA,0);
#endif
#ifdef DENALI_CTL_610_DATA
         REGWR(610,DENALI_CTL_610_DATA,0);
#endif
#ifdef DENALI_CTL_611_DATA
         REGWR(611,DENALI_CTL_611_DATA,0);
#endif
#ifdef DENALI_CTL_612_DATA
         REGWR(612,DENALI_CTL_612_DATA,0);
#endif
#ifdef DENALI_CTL_613_DATA
         REGWR(613,DENALI_CTL_613_DATA,0);
#endif
#ifdef DENALI_CTL_614_DATA
         REGWR(614,DENALI_CTL_614_DATA,0);
#endif
#ifdef DENALI_CTL_615_DATA
         REGWR(615,DENALI_CTL_615_DATA,0);
#endif
#ifdef DENALI_CTL_616_DATA
         REGWR(616,DENALI_CTL_616_DATA,0);
#endif
#ifdef DENALI_CTL_617_DATA
         REGWR(617,DENALI_CTL_617_DATA,0);
#endif
#ifdef DENALI_CTL_618_DATA
         REGWR(618,DENALI_CTL_618_DATA,0);
#endif
#ifdef DENALI_CTL_619_DATA
         REGWR(619,DENALI_CTL_619_DATA,0);
#endif
#ifdef DENALI_CTL_620_DATA
         REGWR(620,DENALI_CTL_620_DATA,0);
#endif
#ifdef DENALI_CTL_621_DATA
         REGWR(621,DENALI_CTL_621_DATA,0);
#endif
#ifdef DENALI_CTL_622_DATA
         REGWR(622,DENALI_CTL_622_DATA,0);
#endif
#ifdef DENALI_CTL_623_DATA
         REGWR(623,DENALI_CTL_623_DATA,0);
#endif
#ifdef DENALI_CTL_624_DATA
         REGWR(624,DENALI_CTL_624_DATA,0);
#endif
#ifdef DENALI_CTL_625_DATA
         REGWR(625,DENALI_CTL_625_DATA,0);
#endif
#ifdef DENALI_CTL_626_DATA
         REGWR(626,DENALI_CTL_626_DATA,0);
#endif
#ifdef DENALI_CTL_627_DATA
         REGWR(627,DENALI_CTL_627_DATA,0);
#endif
#ifdef DENALI_CTL_628_DATA
         REGWR(628,DENALI_CTL_628_DATA,0);
#endif
#ifdef DENALI_CTL_629_DATA
         REGWR(629,DENALI_CTL_629_DATA,0);
#endif
#ifdef DENALI_CTL_630_DATA
         REGWR(630,DENALI_CTL_630_DATA,0);
#endif
#ifdef DENALI_CTL_631_DATA
         REGWR(631,DENALI_CTL_631_DATA,0);
#endif
#ifdef DENALI_CTL_632_DATA
         REGWR(632,DENALI_CTL_632_DATA,0);
#endif
#ifdef DENALI_CTL_633_DATA
         REGWR(633,DENALI_CTL_633_DATA,0);
#endif
#ifdef DENALI_CTL_634_DATA
         REGWR(634,DENALI_CTL_634_DATA,0);
#endif
#ifdef DENALI_CTL_635_DATA
         REGWR(635,DENALI_CTL_635_DATA,0);
#endif
#ifdef DENALI_CTL_636_DATA
         REGWR(636,DENALI_CTL_636_DATA,0);
#endif
#ifdef DENALI_CTL_637_DATA
         REGWR(637,DENALI_CTL_637_DATA,0);
#endif
#ifdef DENALI_CTL_638_DATA
         REGWR(638,DENALI_CTL_638_DATA,0);
#endif
#ifdef DENALI_CTL_639_DATA
         REGWR(639,DENALI_CTL_639_DATA,0);
#endif
#ifdef DENALI_CTL_640_DATA
         REGWR(640,DENALI_CTL_640_DATA,0);
#endif
#ifdef DENALI_CTL_641_DATA
         REGWR(641,DENALI_CTL_641_DATA,0);
#endif
#ifdef DENALI_CTL_642_DATA
         REGWR(642,DENALI_CTL_642_DATA,0);
#endif
#ifdef DENALI_CTL_643_DATA
         REGWR(643,DENALI_CTL_643_DATA,0);
#endif
#ifdef DENALI_CTL_644_DATA
         REGWR(644,DENALI_CTL_644_DATA,0);
#endif
#ifdef DENALI_CTL_645_DATA
         REGWR(645,DENALI_CTL_645_DATA,0);
#endif
#ifdef DENALI_CTL_646_DATA
         REGWR(646,DENALI_CTL_646_DATA,0);
#endif
#ifdef DENALI_CTL_647_DATA
         REGWR(647,DENALI_CTL_647_DATA,0);
#endif
#ifdef DENALI_CTL_648_DATA
         REGWR(648,DENALI_CTL_648_DATA,0);
#endif
#ifdef DENALI_CTL_649_DATA
         REGWR(649,DENALI_CTL_649_DATA,0);
#endif
#ifdef DENALI_CTL_650_DATA
         REGWR(650,DENALI_CTL_650_DATA,0);
#endif
#ifdef DENALI_CTL_651_DATA
         REGWR(651,DENALI_CTL_651_DATA,0);
#endif
#ifdef DENALI_CTL_652_DATA
         REGWR(652,DENALI_CTL_652_DATA,0);
#endif
#ifdef DENALI_CTL_653_DATA
         REGWR(653,DENALI_CTL_653_DATA,0);
#endif
#ifdef DENALI_CTL_654_DATA
         REGWR(654,DENALI_CTL_654_DATA,0);
#endif
#ifdef DENALI_CTL_655_DATA
         REGWR(655,DENALI_CTL_655_DATA,0);
#endif
#ifdef DENALI_CTL_656_DATA
         REGWR(656,DENALI_CTL_656_DATA,0);
#endif
#ifdef DENALI_CTL_657_DATA
         REGWR(657,DENALI_CTL_657_DATA,0);
#endif
#ifdef DENALI_CTL_658_DATA
         REGWR(658,DENALI_CTL_658_DATA,0);
#endif
#ifdef DENALI_CTL_659_DATA
         REGWR(659,DENALI_CTL_659_DATA,0);
#endif
#ifdef DENALI_CTL_660_DATA
         REGWR(660,DENALI_CTL_660_DATA,0);
#endif
#ifdef DENALI_CTL_661_DATA
         REGWR(661,DENALI_CTL_661_DATA,0);
#endif
#ifdef DENALI_CTL_662_DATA
         REGWR(662,DENALI_CTL_662_DATA,0);
#endif
#ifdef DENALI_CTL_663_DATA
         REGWR(663,DENALI_CTL_663_DATA,0);
#endif
#ifdef DENALI_CTL_664_DATA
         REGWR(664,DENALI_CTL_664_DATA,0);
#endif
#ifdef DENALI_CTL_665_DATA
         REGWR(665,DENALI_CTL_665_DATA,0);
#endif
#ifdef DENALI_CTL_666_DATA
         REGWR(666,DENALI_CTL_666_DATA,0);
#endif
#ifdef DENALI_CTL_667_DATA
         REGWR(667,DENALI_CTL_667_DATA,0);
#endif
#ifdef DENALI_CTL_668_DATA
         REGWR(668,DENALI_CTL_668_DATA,0);
#endif
#ifdef DENALI_CTL_669_DATA
         REGWR(669,DENALI_CTL_669_DATA,0);
#endif
#ifdef DENALI_CTL_670_DATA
         REGWR(670,DENALI_CTL_670_DATA,0);
#endif
#ifdef DENALI_CTL_671_DATA
         REGWR(671,DENALI_CTL_671_DATA,0);
#endif
#ifdef DENALI_CTL_672_DATA
         REGWR(672,DENALI_CTL_672_DATA,0);
#endif
#ifdef DENALI_CTL_673_DATA
         REGWR(673,DENALI_CTL_673_DATA,0);
#endif
#ifdef DENALI_CTL_674_DATA
         REGWR(674,DENALI_CTL_674_DATA,0);
#endif
#ifdef DENALI_CTL_675_DATA
         REGWR(675,DENALI_CTL_675_DATA,0);
#endif
#ifdef DENALI_CTL_676_DATA
         REGWR(676,DENALI_CTL_676_DATA,0);
#endif
#ifdef DENALI_CTL_677_DATA
         REGWR(677,DENALI_CTL_677_DATA,0);
#endif
#ifdef DENALI_CTL_678_DATA
         REGWR(678,DENALI_CTL_678_DATA,0);
#endif
#ifdef DENALI_CTL_679_DATA
         REGWR(679,DENALI_CTL_679_DATA,0);
#endif
#ifdef DENALI_CTL_680_DATA
         REGWR(680,DENALI_CTL_680_DATA,0);
#endif
#ifdef DENALI_CTL_681_DATA
         REGWR(681,DENALI_CTL_681_DATA,0);
#endif
#ifdef DENALI_CTL_682_DATA
         REGWR(682,DENALI_CTL_682_DATA,0);
#endif
#ifdef DENALI_CTL_683_DATA
         REGWR(683,DENALI_CTL_683_DATA,0);
#endif
#ifdef DENALI_CTL_684_DATA
         REGWR(684,DENALI_CTL_684_DATA,0);
#endif
#ifdef DENALI_CTL_685_DATA
         REGWR(685,DENALI_CTL_685_DATA,0);
#endif
#ifdef DENALI_CTL_686_DATA
         REGWR(686,DENALI_CTL_686_DATA,0);
#endif
#ifdef DENALI_CTL_687_DATA
         REGWR(687,DENALI_CTL_687_DATA,0);
#endif
#ifdef DENALI_CTL_688_DATA
         REGWR(688,DENALI_CTL_688_DATA,0);
#endif
#ifdef DENALI_CTL_689_DATA
         REGWR(689,DENALI_CTL_689_DATA,0);
#endif
#ifdef DENALI_CTL_690_DATA
         REGWR(690,DENALI_CTL_690_DATA,0);
#endif
#ifdef DENALI_CTL_691_DATA
         REGWR(691,DENALI_CTL_691_DATA,0);
#endif
#ifdef DENALI_CTL_692_DATA
         REGWR(692,DENALI_CTL_692_DATA,0);
#endif
#ifdef DENALI_CTL_693_DATA
         REGWR(693,DENALI_CTL_693_DATA,0);
#endif
#ifdef DENALI_CTL_694_DATA
         REGWR(694,DENALI_CTL_694_DATA,0);
#endif
#ifdef DENALI_CTL_695_DATA
         REGWR(695,DENALI_CTL_695_DATA,0);
#endif
#ifdef DENALI_CTL_696_DATA
         REGWR(696,DENALI_CTL_696_DATA,0);
#endif
#ifdef DENALI_CTL_697_DATA
         REGWR(697,DENALI_CTL_697_DATA,0);
#endif
#ifdef DENALI_CTL_698_DATA
         REGWR(698,DENALI_CTL_698_DATA,0);
#endif
#ifdef DENALI_CTL_699_DATA
         REGWR(699,DENALI_CTL_699_DATA,0);
#endif
#ifdef DENALI_CTL_700_DATA
         REGWR(700,DENALI_CTL_700_DATA,0);
#endif
#ifdef DENALI_CTL_701_DATA
         REGWR(701,DENALI_CTL_701_DATA,0);
#endif
#ifdef DENALI_CTL_702_DATA
         REGWR(702,DENALI_CTL_702_DATA,0);
#endif
#ifdef DENALI_CTL_703_DATA
         REGWR(703,DENALI_CTL_703_DATA,0);
#endif
#ifdef DENALI_CTL_704_DATA
         REGWR(704,DENALI_CTL_704_DATA,0);
#endif
#ifdef DENALI_CTL_705_DATA
         REGWR(705,DENALI_CTL_705_DATA,0);
#endif
#ifdef DENALI_CTL_706_DATA
         REGWR(706,DENALI_CTL_706_DATA,0);
#endif
#ifdef DENALI_CTL_707_DATA
         REGWR(707,DENALI_CTL_707_DATA,0);
#endif
#ifdef DENALI_CTL_708_DATA
         REGWR(708,DENALI_CTL_708_DATA,0);
#endif
#ifdef DENALI_CTL_709_DATA
         REGWR(709,DENALI_CTL_709_DATA,0);
#endif
#ifdef DENALI_CTL_710_DATA
         REGWR(710,DENALI_CTL_710_DATA,0);
#endif
#ifdef DENALI_CTL_711_DATA
         REGWR(711,DENALI_CTL_711_DATA,0);
#endif
#ifdef DENALI_CTL_712_DATA
         REGWR(712,DENALI_CTL_712_DATA,0);
#endif
#ifdef DENALI_CTL_713_DATA
         REGWR(713,DENALI_CTL_713_DATA,0);
#endif
#ifdef DENALI_CTL_714_DATA
         REGWR(714,DENALI_CTL_714_DATA,0);
#endif
#ifdef DENALI_CTL_715_DATA
         REGWR(715,DENALI_CTL_715_DATA,0);
#endif
#ifdef DENALI_CTL_716_DATA
         REGWR(716,DENALI_CTL_716_DATA,0);
#endif
#ifdef DENALI_CTL_717_DATA
         REGWR(717,DENALI_CTL_717_DATA,0);
#endif
#ifdef DENALI_CTL_718_DATA
         REGWR(718,DENALI_CTL_718_DATA,0);
#endif
#ifdef DENALI_CTL_719_DATA
         REGWR(719,DENALI_CTL_719_DATA,0);
#endif
#ifdef DENALI_CTL_720_DATA
         REGWR(720,DENALI_CTL_720_DATA,0);
#endif
#ifdef DENALI_CTL_721_DATA
         REGWR(721,DENALI_CTL_721_DATA,0);
#endif
#ifdef DENALI_CTL_722_DATA
         REGWR(722,DENALI_CTL_722_DATA,0);
#endif
#ifdef DENALI_CTL_723_DATA
         REGWR(723,DENALI_CTL_723_DATA,0);
#endif
#ifdef DENALI_CTL_724_DATA
         REGWR(724,DENALI_CTL_724_DATA,0);
#endif
#ifdef DENALI_CTL_725_DATA
         REGWR(725,DENALI_CTL_725_DATA,0);
#endif
#ifdef DENALI_CTL_726_DATA
         REGWR(726,DENALI_CTL_726_DATA,0);
#endif
#ifdef DENALI_CTL_727_DATA
         REGWR(727,DENALI_CTL_727_DATA,0);
#endif
#ifdef DENALI_CTL_728_DATA
         REGWR(728,DENALI_CTL_728_DATA,0);
#endif
#ifdef DENALI_CTL_729_DATA
         REGWR(729,DENALI_CTL_729_DATA,0);
#endif
#ifdef DENALI_CTL_730_DATA
         REGWR(730,DENALI_CTL_730_DATA,0);
#endif
#ifdef DENALI_CTL_731_DATA
         REGWR(731,DENALI_CTL_731_DATA,0);
#endif
#ifdef DENALI_CTL_732_DATA
         REGWR(732,DENALI_CTL_732_DATA,0);
#endif
#ifdef DENALI_CTL_733_DATA
         REGWR(733,DENALI_CTL_733_DATA,0);
#endif
#ifdef DENALI_CTL_734_DATA
         REGWR(734,DENALI_CTL_734_DATA,0);
#endif
#ifdef DENALI_CTL_735_DATA
         REGWR(735,DENALI_CTL_735_DATA,0);
#endif
#ifdef DENALI_CTL_736_DATA
         REGWR(736,DENALI_CTL_736_DATA,0);
#endif
#ifdef DENALI_CTL_737_DATA
         REGWR(737,DENALI_CTL_737_DATA,0);
#endif
#ifdef DENALI_CTL_738_DATA
         REGWR(738,DENALI_CTL_738_DATA,0);
#endif
#ifdef DENALI_CTL_739_DATA
         REGWR(739,DENALI_CTL_739_DATA,0);
#endif
#ifdef DENALI_CTL_740_DATA
         REGWR(740,DENALI_CTL_740_DATA,0);
#endif
#ifdef DENALI_CTL_741_DATA
         REGWR(741,DENALI_CTL_741_DATA,0);
#endif
#ifdef DENALI_CTL_742_DATA
         REGWR(742,DENALI_CTL_742_DATA,0);
#endif
#ifdef DENALI_CTL_743_DATA
         REGWR(743,DENALI_CTL_743_DATA,0);
#endif
#ifdef DENALI_CTL_744_DATA
         REGWR(744,DENALI_CTL_744_DATA,0);
#endif
#ifdef DENALI_CTL_745_DATA
         REGWR(745,DENALI_CTL_745_DATA,0);
#endif
#ifdef DENALI_CTL_746_DATA
         REGWR(746,DENALI_CTL_746_DATA,0);
#endif
#ifdef DENALI_CTL_747_DATA
         REGWR(747,DENALI_CTL_747_DATA,0);
#endif
#ifdef DENALI_CTL_748_DATA
         REGWR(748,DENALI_CTL_748_DATA,0);
#endif
#ifdef DENALI_CTL_749_DATA
         REGWR(749,DENALI_CTL_749_DATA,0);
#endif
#ifdef DENALI_CTL_750_DATA
         REGWR(750,DENALI_CTL_750_DATA,0);
#endif
#ifdef DENALI_CTL_751_DATA
         REGWR(751,DENALI_CTL_751_DATA,0);
#endif
#ifdef DENALI_CTL_752_DATA
         REGWR(752,DENALI_CTL_752_DATA,0);
#endif
#ifdef DENALI_CTL_753_DATA
         REGWR(753,DENALI_CTL_753_DATA,0);
#endif
#ifdef DENALI_CTL_754_DATA
         REGWR(754,DENALI_CTL_754_DATA,0);
#endif
#ifdef DENALI_CTL_755_DATA
         REGWR(755,DENALI_CTL_755_DATA,0);
#endif
#ifdef DENALI_CTL_756_DATA
         REGWR(756,DENALI_CTL_756_DATA,0);
#endif
#ifdef DENALI_CTL_757_DATA
         REGWR(757,DENALI_CTL_757_DATA,0);
#endif
#ifdef DENALI_CTL_758_DATA
         REGWR(758,DENALI_CTL_758_DATA,0);
#endif
#ifdef DENALI_CTL_759_DATA
         REGWR(759,DENALI_CTL_759_DATA,0);
#endif
#ifdef DENALI_CTL_760_DATA
         REGWR(760,DENALI_CTL_760_DATA,0);
#endif
#ifdef DENALI_CTL_761_DATA
         REGWR(761,DENALI_CTL_761_DATA,0);
#endif
#ifdef DENALI_CTL_762_DATA
         REGWR(762,DENALI_CTL_762_DATA,0);
#endif
#ifdef DENALI_CTL_763_DATA
         REGWR(763,DENALI_CTL_763_DATA,0);
#endif
#ifdef DENALI_CTL_764_DATA
         REGWR(764,DENALI_CTL_764_DATA,0);
#endif
#ifdef DENALI_CTL_765_DATA
         REGWR(765,DENALI_CTL_765_DATA,0);
#endif
#ifdef DENALI_CTL_766_DATA
         REGWR(766,DENALI_CTL_766_DATA,0);
#endif
#ifdef DENALI_CTL_767_DATA
         REGWR(767,DENALI_CTL_767_DATA,0);
#endif
#ifdef DENALI_CTL_768_DATA
         REGWR(768,DENALI_CTL_768_DATA,0);
#endif
#ifdef DENALI_CTL_769_DATA
         REGWR(769,DENALI_CTL_769_DATA,0);
#endif
#ifdef DENALI_CTL_770_DATA
         REGWR(770,DENALI_CTL_770_DATA,0);
#endif
#ifdef DENALI_CTL_771_DATA
         REGWR(771,DENALI_CTL_771_DATA,0);
#endif
#ifdef DENALI_CTL_772_DATA
         REGWR(772,DENALI_CTL_772_DATA,0);
#endif
#ifdef DENALI_CTL_773_DATA
         REGWR(773,DENALI_CTL_773_DATA,0);
#endif
#ifdef DENALI_CTL_774_DATA
         REGWR(774,DENALI_CTL_774_DATA,0);
#endif
#ifdef DENALI_CTL_775_DATA
         REGWR(775,DENALI_CTL_775_DATA,0);
#endif
#ifdef DENALI_CTL_776_DATA
         REGWR(776,DENALI_CTL_776_DATA,0);
#endif
#ifdef DENALI_CTL_777_DATA
         REGWR(777,DENALI_CTL_777_DATA,0);
#endif
#ifdef DENALI_CTL_778_DATA
         REGWR(778,DENALI_CTL_778_DATA,0);
#endif
#ifdef DENALI_CTL_779_DATA
         REGWR(779,DENALI_CTL_779_DATA,0);
#endif
#ifdef DENALI_CTL_780_DATA
         REGWR(780,DENALI_CTL_780_DATA,0);
#endif
#ifdef DENALI_CTL_781_DATA
         REGWR(781,DENALI_CTL_781_DATA,0);
#endif
#ifdef DENALI_CTL_782_DATA
         REGWR(782,DENALI_CTL_782_DATA,0);
#endif
#ifdef DENALI_CTL_783_DATA
         REGWR(783,DENALI_CTL_783_DATA,0);
#endif
#ifdef DENALI_CTL_784_DATA
         REGWR(784,DENALI_CTL_784_DATA,0);
#endif
#ifdef DENALI_CTL_785_DATA
         REGWR(785,DENALI_CTL_785_DATA,0);
#endif
#ifdef DENALI_CTL_786_DATA
         REGWR(786,DENALI_CTL_786_DATA,0);
#endif
#ifdef DENALI_CTL_787_DATA
         REGWR(787,DENALI_CTL_787_DATA,0);
#endif
#ifdef DENALI_CTL_788_DATA
         REGWR(788,DENALI_CTL_788_DATA,0);
#endif
#ifdef DENALI_CTL_789_DATA
         REGWR(789,DENALI_CTL_789_DATA,0);
#endif
#ifdef DENALI_CTL_790_DATA
         REGWR(790,DENALI_CTL_790_DATA,0);
#endif
#ifdef DENALI_CTL_791_DATA
         REGWR(791,DENALI_CTL_791_DATA,0);
#endif
#ifdef DENALI_CTL_792_DATA
         REGWR(792,DENALI_CTL_792_DATA,0);
#endif
#ifdef DENALI_CTL_793_DATA
         REGWR(793,DENALI_CTL_793_DATA,0);
#endif
#ifdef DENALI_CTL_794_DATA
         REGWR(794,DENALI_CTL_794_DATA,0);
#endif
#ifdef DENALI_CTL_795_DATA
         REGWR(795,DENALI_CTL_795_DATA,0);
#endif
#ifdef DENALI_CTL_796_DATA
         REGWR(796,DENALI_CTL_796_DATA,0);
#endif
#ifdef DENALI_CTL_797_DATA
         REGWR(797,DENALI_CTL_797_DATA,0);
#endif
#ifdef DENALI_CTL_798_DATA
         REGWR(798,DENALI_CTL_798_DATA,0);
#endif
#ifdef DENALI_CTL_799_DATA
         REGWR(799,DENALI_CTL_799_DATA,0);
#endif
#ifdef DENALI_CTL_800_DATA
         REGWR(800,DENALI_CTL_800_DATA,0);
#endif
#ifdef DENALI_CTL_801_DATA
         REGWR(801,DENALI_CTL_801_DATA,0);
#endif
#ifdef DENALI_CTL_802_DATA
         REGWR(802,DENALI_CTL_802_DATA,0);
#endif
#ifdef DENALI_CTL_803_DATA
         REGWR(803,DENALI_CTL_803_DATA,0);
#endif
#ifdef DENALI_CTL_804_DATA
         REGWR(804,DENALI_CTL_804_DATA,0);
#endif
#ifdef DENALI_CTL_805_DATA
         REGWR(805,DENALI_CTL_805_DATA,0);
#endif
#ifdef DENALI_CTL_806_DATA
         REGWR(806,DENALI_CTL_806_DATA,0);
#endif
#ifdef DENALI_CTL_807_DATA
         REGWR(807,DENALI_CTL_807_DATA,0);
#endif
#ifdef DENALI_CTL_808_DATA
         REGWR(808,DENALI_CTL_808_DATA,0);
#endif
#ifdef DENALI_CTL_809_DATA
         REGWR(809,DENALI_CTL_809_DATA,0);
#endif
#ifdef DENALI_CTL_810_DATA
         REGWR(810,DENALI_CTL_810_DATA,0);
#endif
#ifdef DENALI_CTL_811_DATA
         REGWR(811,DENALI_CTL_811_DATA,0);
#endif
#ifdef DENALI_CTL_812_DATA
         REGWR(812,DENALI_CTL_812_DATA,0);
#endif
#ifdef DENALI_CTL_813_DATA
         REGWR(813,DENALI_CTL_813_DATA,0);
#endif
#ifdef DENALI_CTL_814_DATA
         REGWR(814,DENALI_CTL_814_DATA,0);
#endif
#ifdef DENALI_CTL_815_DATA
         REGWR(815,DENALI_CTL_815_DATA,0);
#endif
#ifdef DENALI_CTL_816_DATA
         REGWR(816,DENALI_CTL_816_DATA,0);
#endif
#ifdef DENALI_CTL_817_DATA
         REGWR(817,DENALI_CTL_817_DATA,0);
#endif
#ifdef DENALI_CTL_818_DATA
         REGWR(818,DENALI_CTL_818_DATA,0);
#endif
#ifdef DENALI_CTL_819_DATA
         REGWR(819,DENALI_CTL_819_DATA,0);
#endif
#ifdef DENALI_CTL_820_DATA
         REGWR(820,DENALI_CTL_820_DATA,0);
#endif
#ifdef DENALI_CTL_821_DATA
         REGWR(821,DENALI_CTL_821_DATA,0);
#endif
#ifdef DENALI_CTL_822_DATA
         REGWR(822,DENALI_CTL_822_DATA,0);
#endif
#ifdef DENALI_CTL_823_DATA
         REGWR(823,DENALI_CTL_823_DATA,0);
#endif
#ifdef DENALI_CTL_824_DATA
         REGWR(824,DENALI_CTL_824_DATA,0);
#endif
#ifdef DENALI_CTL_825_DATA
         REGWR(825,DENALI_CTL_825_DATA,0);
#endif
#ifdef DENALI_CTL_826_DATA
         REGWR(826,DENALI_CTL_826_DATA,0);
#endif
#ifdef DENALI_CTL_827_DATA
         REGWR(827,DENALI_CTL_827_DATA,0);
#endif
#ifdef DENALI_CTL_828_DATA
         REGWR(828,DENALI_CTL_828_DATA,0);
#endif
#ifdef DENALI_CTL_829_DATA
         REGWR(829,DENALI_CTL_829_DATA,0);
#endif
#ifdef DENALI_CTL_830_DATA
         REGWR(830,DENALI_CTL_830_DATA,0);
#endif
#ifdef DENALI_CTL_831_DATA
         REGWR(831,DENALI_CTL_831_DATA,0);
#endif
#ifdef DENALI_CTL_832_DATA
         REGWR(832,DENALI_CTL_832_DATA,0);
#endif
#ifdef DENALI_CTL_833_DATA
         REGWR(833,DENALI_CTL_833_DATA,0);
#endif
#ifdef DENALI_CTL_834_DATA
         REGWR(834,DENALI_CTL_834_DATA,0);
#endif
#ifdef DENALI_CTL_835_DATA
         REGWR(835,DENALI_CTL_835_DATA,0);
#endif
#ifdef DENALI_CTL_836_DATA
         REGWR(836,DENALI_CTL_836_DATA,0);
#endif
#ifdef DENALI_CTL_837_DATA
         REGWR(837,DENALI_CTL_837_DATA,0);
#endif
#ifdef DENALI_CTL_838_DATA
         REGWR(838,DENALI_CTL_838_DATA,0);
#endif
#ifdef DENALI_CTL_839_DATA
         REGWR(839,DENALI_CTL_839_DATA,0);
#endif
#ifdef DENALI_CTL_840_DATA
         REGWR(840,DENALI_CTL_840_DATA,0);
#endif
#ifdef DENALI_CTL_841_DATA
         REGWR(841,DENALI_CTL_841_DATA,0);
#endif
#ifdef DENALI_CTL_842_DATA
         REGWR(842,DENALI_CTL_842_DATA,0);
#endif
#ifdef DENALI_CTL_843_DATA
         REGWR(843,DENALI_CTL_843_DATA,0);
#endif
#ifdef DENALI_CTL_844_DATA
         REGWR(844,DENALI_CTL_844_DATA,0);
#endif
#ifdef DENALI_CTL_845_DATA
         REGWR(845,DENALI_CTL_845_DATA,0);
#endif
#ifdef DENALI_CTL_846_DATA
         REGWR(846,DENALI_CTL_846_DATA,0);
#endif
#ifdef DENALI_CTL_847_DATA
         REGWR(847,DENALI_CTL_847_DATA,0);
#endif
#ifdef DENALI_CTL_848_DATA
         REGWR(848,DENALI_CTL_848_DATA,0);
#endif
#ifdef DENALI_CTL_849_DATA
         REGWR(849,DENALI_CTL_849_DATA,0);
#endif
#ifdef DENALI_CTL_850_DATA
         REGWR(850,DENALI_CTL_850_DATA,0);
#endif
#ifdef DENALI_CTL_851_DATA
         REGWR(851,DENALI_CTL_851_DATA,0);
#endif
#ifdef DENALI_CTL_852_DATA
         REGWR(852,DENALI_CTL_852_DATA,0);
#endif
#ifdef DENALI_CTL_853_DATA
         REGWR(853,DENALI_CTL_853_DATA,0);
#endif
#ifdef DENALI_CTL_854_DATA
         REGWR(854,DENALI_CTL_854_DATA,0);
#endif
#ifdef DENALI_CTL_855_DATA
         REGWR(855,DENALI_CTL_855_DATA,0);
#endif
#ifdef DENALI_CTL_856_DATA
         REGWR(856,DENALI_CTL_856_DATA,0);
#endif
#ifdef DENALI_CTL_857_DATA
         REGWR(857,DENALI_CTL_857_DATA,0);
#endif
#ifdef DENALI_CTL_858_DATA
         REGWR(858,DENALI_CTL_858_DATA,0);
#endif
#ifdef DENALI_CTL_859_DATA
         REGWR(859,DENALI_CTL_859_DATA,0);
#endif
#ifdef DENALI_CTL_860_DATA
         REGWR(860,DENALI_CTL_860_DATA,0);
#endif
#ifdef DENALI_CTL_861_DATA
         REGWR(861,DENALI_CTL_861_DATA,0);
#endif
#ifdef DENALI_CTL_862_DATA
         REGWR(862,DENALI_CTL_862_DATA,0);
#endif
#ifdef DENALI_CTL_863_DATA
         REGWR(863,DENALI_CTL_863_DATA,0);
#endif
#ifdef DENALI_CTL_864_DATA
         REGWR(864,DENALI_CTL_864_DATA,0);
#endif
#ifdef DENALI_CTL_865_DATA
         REGWR(865,DENALI_CTL_865_DATA,0);
#endif
#ifdef DENALI_CTL_866_DATA
         REGWR(866,DENALI_CTL_866_DATA,0);
#endif
#ifdef DENALI_CTL_867_DATA
         REGWR(867,DENALI_CTL_867_DATA,0);
#endif
#ifdef DENALI_CTL_868_DATA
         REGWR(868,DENALI_CTL_868_DATA,0);
#endif
#ifdef DENALI_CTL_869_DATA
         REGWR(869,DENALI_CTL_869_DATA,0);
#endif
#ifdef DENALI_CTL_870_DATA
         REGWR(870,DENALI_CTL_870_DATA,0);
#endif
#ifdef DENALI_CTL_871_DATA
         REGWR(871,DENALI_CTL_871_DATA,0);
#endif
#ifdef DENALI_CTL_872_DATA
         REGWR(872,DENALI_CTL_872_DATA,0);
#endif
#ifdef DENALI_CTL_873_DATA
         REGWR(873,DENALI_CTL_873_DATA,0);
#endif
#ifdef DENALI_CTL_874_DATA
         REGWR(874,DENALI_CTL_874_DATA,0);
#endif
#ifdef DENALI_CTL_875_DATA
         REGWR(875,DENALI_CTL_875_DATA,0);
#endif
#ifdef DENALI_CTL_876_DATA
         REGWR(876,DENALI_CTL_876_DATA,0);
#endif
#ifdef DENALI_CTL_877_DATA
         REGWR(877,DENALI_CTL_877_DATA,0);
#endif
#ifdef DENALI_CTL_878_DATA
         REGWR(878,DENALI_CTL_878_DATA,0);
#endif
#ifdef DENALI_CTL_879_DATA
         REGWR(879,DENALI_CTL_879_DATA,0);
#endif
#ifdef DENALI_CTL_880_DATA
         REGWR(880,DENALI_CTL_880_DATA,0);
#endif
#ifdef DENALI_CTL_881_DATA
         REGWR(881,DENALI_CTL_881_DATA,0);
#endif
#ifdef DENALI_CTL_882_DATA
         REGWR(882,DENALI_CTL_882_DATA,0);
#endif
#ifdef DENALI_CTL_883_DATA
         REGWR(883,DENALI_CTL_883_DATA,0);
#endif
#ifdef DENALI_CTL_884_DATA
         REGWR(884,DENALI_CTL_884_DATA,0);
#endif
#ifdef DENALI_CTL_885_DATA
         REGWR(885,DENALI_CTL_885_DATA,0);
#endif
#ifdef DENALI_CTL_886_DATA
         REGWR(886,DENALI_CTL_886_DATA,0);
#endif
#ifdef DENALI_CTL_887_DATA
         REGWR(887,DENALI_CTL_887_DATA,0);
#endif
#ifdef DENALI_CTL_888_DATA
         REGWR(888,DENALI_CTL_888_DATA,0);
#endif
#ifdef DENALI_CTL_889_DATA
         REGWR(889,DENALI_CTL_889_DATA,0);
#endif
#ifdef DENALI_CTL_890_DATA
         REGWR(890,DENALI_CTL_890_DATA,0);
#endif
#ifdef DENALI_CTL_891_DATA
         REGWR(891,DENALI_CTL_891_DATA,0);
#endif
#ifdef DENALI_CTL_892_DATA
         REGWR(892,DENALI_CTL_892_DATA,0);
#endif
#ifdef DENALI_CTL_893_DATA
         REGWR(893,DENALI_CTL_893_DATA,0);
#endif
#ifdef DENALI_CTL_894_DATA
         REGWR(894,DENALI_CTL_894_DATA,0);
#endif
#ifdef DENALI_CTL_895_DATA
         REGWR(895,DENALI_CTL_895_DATA,0);
#endif
#ifdef DENALI_CTL_896_DATA
         REGWR(896,DENALI_CTL_896_DATA,0);
#endif

}//}}}

void config_phy_fpga(int odl_val){//{{{
#ifdef DENALI_PHY_00_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(0 + PHY_BASE_ADDR,DENALI_PHY_00_DATA,0);
#else
         REGWR(0 + PHY_BASE_ADDR,DENALI_PHY_00_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_01_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(1 + PHY_BASE_ADDR,DENALI_PHY_01_DATA,0);
#else
         REGWR(1 + PHY_BASE_ADDR,DENALI_PHY_01_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_02_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(2 + PHY_BASE_ADDR,DENALI_PHY_02_DATA,0);
#else
         REGWR(2 + PHY_BASE_ADDR,DENALI_PHY_02_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_03_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(3 + PHY_BASE_ADDR,DENALI_PHY_03_DATA,0);
#else
         REGWR(3 + PHY_BASE_ADDR,DENALI_PHY_03_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_04_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(4 + PHY_BASE_ADDR,DENALI_PHY_04_DATA,0);
#else
         REGWR(4 + PHY_BASE_ADDR,DENALI_PHY_04_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_05_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(5 + PHY_BASE_ADDR,DENALI_PHY_05_DATA,0);
#else
         REGWR(5 + PHY_BASE_ADDR,DENALI_PHY_05_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_06_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(6 + PHY_BASE_ADDR,DENALI_PHY_06_DATA,0);
#else
         REGWR(6 + PHY_BASE_ADDR,DENALI_PHY_06_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_07_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(7 + PHY_BASE_ADDR,DENALI_PHY_07_DATA,0);
#else
         REGWR(7 + PHY_BASE_ADDR,DENALI_PHY_07_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_08_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(8 + PHY_BASE_ADDR,DENALI_PHY_08_DATA,0);
#else
         REGWR(8 + PHY_BASE_ADDR,DENALI_PHY_08_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_09_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(9 + PHY_BASE_ADDR,DENALI_PHY_09_DATA,0);
#else
         REGWR(9 + PHY_BASE_ADDR,DENALI_PHY_09_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_10_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(10 + PHY_BASE_ADDR,DENALI_PHY_10_DATA,0);
#else
         REGWR(10 + PHY_BASE_ADDR,DENALI_PHY_10_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_11_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(11 + PHY_BASE_ADDR,DENALI_PHY_11_DATA,0);
#else
         REGWR(11 + PHY_BASE_ADDR,DENALI_PHY_11_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_12_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(12 + PHY_BASE_ADDR,DENALI_PHY_12_DATA,0);
#else
         REGWR(12 + PHY_BASE_ADDR,DENALI_PHY_12_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_13_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(13 + PHY_BASE_ADDR,DENALI_PHY_13_DATA,0);
#else
         REGWR(13 + PHY_BASE_ADDR,DENALI_PHY_13_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_14_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(14 + PHY_BASE_ADDR,DENALI_PHY_14_DATA,0);
#else
         REGWR(14 + PHY_BASE_ADDR,DENALI_PHY_14_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_15_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(15 + PHY_BASE_ADDR,DENALI_PHY_15_DATA,0);
#else
         REGWR(15 + PHY_BASE_ADDR,DENALI_PHY_15_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_16_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(16 + PHY_BASE_ADDR,DENALI_PHY_16_DATA,0);
#else
         REGWR(16 + PHY_BASE_ADDR,DENALI_PHY_16_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_17_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(17 + PHY_BASE_ADDR,DENALI_PHY_17_DATA,0);
#else
         REGWR(17 + PHY_BASE_ADDR,DENALI_PHY_17_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_18_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(18 + PHY_BASE_ADDR,DENALI_PHY_18_DATA,0);
#else
         REGWR(18 + PHY_BASE_ADDR,DENALI_PHY_18_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_19_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(19 + PHY_BASE_ADDR,DENALI_PHY_19_DATA,0);
#else
         REGWR(19 + PHY_BASE_ADDR,DENALI_PHY_19_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_20_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(20 + PHY_BASE_ADDR,DENALI_PHY_20_DATA,0);
#else
         REGWR(20 + PHY_BASE_ADDR,DENALI_PHY_20_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_21_DATA
//#ifdef SIM_SPEEDUP
//         REGWR_TCL(21 + PHY_BASE_ADDR,DENALI_PHY_21_DATA|(odl_val<<8),0);
//#else
//         REGWR(21 + PHY_BASE_ADDR,DENALI_PHY_21_DATA|(odl_val<<8),0);
#ifdef SIM_SPEEDUP
         REGWR_TCL(21 + PHY_BASE_ADDR,DENALI_PHY_21_DATA,0);
#else
         REGWR(21 + PHY_BASE_ADDR,DENALI_PHY_21_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_22_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(22 + PHY_BASE_ADDR,DENALI_PHY_22_DATA|(odl_val<<16),0);
#else
         REGWR(22 + PHY_BASE_ADDR,DENALI_PHY_22_DATA|(odl_val<<16),0);
#endif
#endif
#ifdef DENALI_PHY_23_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(23 + PHY_BASE_ADDR,DENALI_PHY_23_DATA,0);
#else
         REGWR(23 + PHY_BASE_ADDR,DENALI_PHY_23_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_24_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(24 + PHY_BASE_ADDR,DENALI_PHY_24_DATA|(odl_val<<16),0);
#else
         REGWR(24 + PHY_BASE_ADDR,DENALI_PHY_24_DATA|(odl_val<<16),0);
#endif
#endif
#ifdef DENALI_PHY_25_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(25 + PHY_BASE_ADDR,DENALI_PHY_25_DATA,0);
#else
         REGWR(25 + PHY_BASE_ADDR,DENALI_PHY_25_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_26_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(26 + PHY_BASE_ADDR,DENALI_PHY_26_DATA|(odl_val<<16),0);
#else
         REGWR(26 + PHY_BASE_ADDR,DENALI_PHY_26_DATA|(odl_val<<16),0);
#endif
#endif
#ifdef DENALI_PHY_27_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(27 + PHY_BASE_ADDR,DENALI_PHY_27_DATA,0);
#else
         REGWR(27 + PHY_BASE_ADDR,DENALI_PHY_27_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_28_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(28 + PHY_BASE_ADDR,DENALI_PHY_28_DATA|(odl_val<<16),0);
#else
         REGWR(28 + PHY_BASE_ADDR,DENALI_PHY_28_DATA|(odl_val<<16),0);
#endif
#endif
#ifdef DENALI_PHY_29_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(29 + PHY_BASE_ADDR,DENALI_PHY_29_DATA,0);
#else
         REGWR(29 + PHY_BASE_ADDR,DENALI_PHY_29_DATA,0);
#endif
#endif
#ifdef DENALI_PHY_30_DATA
#ifdef SIM_SPEEDUP
         REGWR_TCL(30 + PHY_BASE_ADDR,DENALI_PHY_30_DATA,0);
#else
         REGWR(30 + PHY_BASE_ADDR,DENALI_PHY_30_DATA,0);
#endif
#endif
}//}}}

void init_ddr_fpga(int odl_val){/*{{{*/
    printf("DDR_fpga init start\n");
    int regdata;

    config_ctl_fpga();
    config_phy_fpga(odl_val);
    //161, 16, 6
    SET_PARAM(LPI_WAKEUP_EN_ADDR, LPI_WAKEUP_EN_OFFSET, LPI_WAKEUP_EN_WIDTH, 45);
    //325, 8, 1
    SET_PARAM(IN_ORDER_ACCEPT_ADDR, IN_ORDER_ACCEPT_OFFSET, IN_ORDER_ACCEPT_WIDTH, 0x1);
    //316, 16, 2
    SET_PARAM(DISABLE_RW_GROUP_W_BNK_CONFLICT_ADDR, DISABLE_RW_GROUP_W_BNK_CONFLICT_OFFSET, DISABLE_RW_GROUP_W_BNK_CONFLICT_WIDTH, 0);
    //316, 0, 1
    SET_PARAM(CS_SAME_EN_ADDR, CS_SAME_EN_OFFSET, CS_SAME_EN_WIDTH, 0);

    //start
    //disable swap when inline ecc enable
    SET_PARAM(SWAP_EN_ADDR, SWAP_EN_OFFSET, SWAP_EN_WIDTH, 0x0);//317, 0, 1
    SET_PARAM(IN_ORDER_ACCEPT_ADDR, IN_ORDER_ACCEPT_OFFSET, IN_ORDER_ACCEPT_WIDTH, 0x1);//325, 8, 1
    // Disable write reordering for verilog tests to avoid issues with SRAM
    SET_PARAM(WR_ORDER_REQ_ADDR, WR_ORDER_REQ_OFFSET, WR_ORDER_REQ_WIDTH, 0x2);//325, 16, 2


    // For verilog patterns, keep the init freq same as boot freq
    SET_PARAM(INIT_FREQ_ADDR, INIT_FREQ_OFFSET, INIT_FREQ_WIDTH, 0x0);//175, 0, 2
    // Disable Inline ECC for verilog tests as the testbench doesn't handle
    // the addressing requirements.
    SET_PARAM(ECC_ENABLE_ADDR, ECC_ENABLE_OFFSET, ECC_ENABLE_WIDTH, 0x00);//234, 24, 2

    // Asserting param_start
    regdata = reg32(DDR_CFG_BASE + START_ADDR*4);
    regdata = regdata | 0x1;
    printf("start regdata:%x\n", regdata);
    REGWR(START_ADDR,regdata,0);


    //wait(TIER_PHY_TOP.cdn_hs_phy.inst_pi.dfi_init_complete_phy);
    //wait(test_top.u_dut.u_ddr_sys_wrapper.u_ddr_top.u_ddr_subsystem.cdn_hs_phy_top.cdn_hs_phy.inst_pi.dfi_init_complete_phy);
    while(1) {
        unsigned int tmp;
        tmp = reg32(DDR_CFG_BASE + 0x550);
        if((tmp>>25)&0x1){
            printf("START Done\n");
            break;
        }
    }

    uint32_t tmp, mr1_data, mr2_data;
    tmp = reg32(DDR_CFG_BASE+0x328);

    mr1_data = tmp & 0xff;
    mr2_data = (tmp>>8) & 0xff;
    

    MRW(1, mr1_data);
    MRW(2, mr2_data);

}/*}}}*/

// train{{{
ulong write_memtest(vu_long *buf, ulong start_addr, ulong end_addr)/*{{{*/
{
	vu_long *addr;
	vu_long offset;
	vu_long pattern;
	vu_long anti_pattern;
	vu_long num_words;
	addr = buf;
	num_words = (end_addr - start_addr) / (4*sizeof(vu_long));
	/*
	 * Fill memory with a known pattern.
	 */
	for (pattern = 1, offset = 0; offset < num_words; pattern++, offset++) {
		addr[offset] = pattern;
	}
	addr = (vu_long*) (buf + 1*num_words);
	for (pattern = 1, offset = 0; offset < num_words; pattern++, offset++) {
		anti_pattern = ~pattern;
		addr[offset] = anti_pattern;
	}
	addr = (vu_long*) (buf + 2*num_words);
	pattern = (vu_long)0xaaaaaaaa; // toggle between every bit
	for (offset = 0; offset < num_words; offset++) {
		pattern = ~pattern;
		addr[offset] = pattern;
	}
	addr = (vu_long*) (buf + 3*num_words);
	pattern = (vu_long)0x0; // toggle between every patten,verify power supply
	for (offset = 0; offset < num_words; offset++) {
		pattern = ~pattern;
		addr[offset] = pattern;
	}
	return 0;
}/*}}}*/

ulong read_memtest(vu_long *buf, ulong start_addr, ulong end_addr)/*{{{*/
{
	vu_long *addr;
	ulong errs = 0;
	vu_long offset;
	vu_long pattern;
	vu_long temp;
	vu_long anti_pattern;
	vu_long num_words;
	addr = buf;
	num_words = (end_addr - start_addr) / (4*sizeof(vu_long));


	pattern = (vu_long)0xaaaaaaaa;
	anti_pattern = (vu_long)0x55555555;
	/*
	 * Check each location and invert it for the second pass.
	 */
	for (pattern = 1, offset = 0; offset < num_words; pattern++, offset++) {
		temp = addr[offset];
		if (temp != pattern) {
			//printf("\nFAILURE (read/write) @ 0x%.8lx: expected 0x%.8lx, actual 0x%.8lx)\n", start_addr + offset * sizeof(vu_long), pattern, temp);
			errs++;
			return errs;
		}
	}
	addr = (vu_long*) (buf + 1*num_words);
	for (pattern = 1, offset = 0; offset < num_words; pattern++, offset++) {
		anti_pattern = ~pattern;
		temp = addr[offset];
		if (temp != anti_pattern) {
			//printf("\nFAILURE (read/write): @ 0x%.8lx: expected 0x%.8lx, actual 0x%.8lx)\n", start_addr + offset * sizeof(vu_long), anti_pattern, temp);			   
			errs++;
			return errs;
		}
	}
	addr = (vu_long*) (buf + 2*num_words);
	pattern = (vu_long)0xaaaaaaaa; // toggle between every bit
	for (offset = 0; offset < num_words;offset++) {
		pattern = ~pattern;
		temp = addr[offset];
		if (temp != pattern) {
			//printf("\nFAILURE (read/write): @ 0x%.8lx: expected 0x%.8lx, actual 0x%.8lx)\n", start_addr + offset * sizeof(vu_long), anti_pattern, temp);			   
			errs++;
			return errs;
		}
	}
	addr = (vu_long*) (buf + 3*num_words);
	pattern = (vu_long)0x0; // toggle between every bit
	for (offset = 0; offset < num_words;offset++) {
		pattern = ~pattern;
		temp = addr[offset];
		if (temp != pattern) {
			//printf("\nFAILURE (read/write): @ 0x%.8lx: expected 0x%.8lx, actual 0x%.8lx)\n", start_addr + offset * sizeof(vu_long), anti_pattern, temp);			   
			errs++;
			return errs;
		}
	}
	return errs;
}/*}}}*/

//mode=0: full memtest; mode=1: write_memtest;mode=2: read_memtest
int seehi_memtest_new(int mode){
	int ret = 0;
	vu_long start[] = { 0x50000000, 0x52000000 };
	vu_long size[] = { 0x00001000, 0x00001000 };
	if(mode == 1) {
		for (int i = 0; i < sizeof(start) / sizeof(start[0]); i++) {
			ret = (int)write_memtest((vu_long *)start[i], (ulong)start[i], (ulong)(start[i] + size[i]));
		}
		return 0;
	}else if(mode == 2) {
		for (int i = 0; i < sizeof(start) / sizeof(start[0]); i++) {
			ret = (int)read_memtest((vu_long *)start[i], (ulong)start[i], (ulong)(start[i] + size[i]));
			if (ret != 0) {
				////printf("mem_test_alt failed at: 0x%lx--0x%lx\n", (ulong)start[i], (ulong)(start[i] + size[i]));
			}
		}
	}
	return ret;
}

void cfg_delayline(int val)/*{{{*/
{
//  REGWR(21 + PHY_BASE_ADDR,(REGREAD(21 + PHY_BASE_ADDR)&(~0x1ff))|val,0);
  REGWR(22 + PHY_BASE_ADDR,(REGREAD(22 + PHY_BASE_ADDR)&(~0x1ff))|val,0);
  REGWR(24 + PHY_BASE_ADDR,(REGREAD(24 + PHY_BASE_ADDR)&(~0x1ff))|val,0);
  REGWR(26 + PHY_BASE_ADDR,(REGREAD(26 + PHY_BASE_ADDR)&(~0x1ff))|val,0);
  REGWR(28 + PHY_BASE_ADDR,(REGREAD(28 + PHY_BASE_ADDR)&(~0x1ff))|val,0);
  REGWR(30 + PHY_BASE_ADDR,DENALI_PHY_30_DATA,0);
  delay(10);
//  printf("DENALI_PHY_21_DATA = %x\n",REGREAD(21 + PHY_BASE_ADDR));
  printf("DENALI_PHY_22_DATA = %x\n",REGREAD(22 + PHY_BASE_ADDR));
//  printf("DENALI_PHY_24_DATA = %x\n",REGREAD(24 + PHY_BASE_ADDR));
//  printf("DENALI_PHY_26_DATA = %x\n",REGREAD(26 + PHY_BASE_ADDR));
//  printf("DENALI_PHY_28_DATA = %x\n",REGREAD(28 + PHY_BASE_ADDR));
//  printf("DENALI_PHY_30_DATA = %x\n",REGREAD(30 + PHY_BASE_ADDR));

}/*}}}*/

void train_idelay(void) /*{{{*/
{

  int errs;
  printf("train idelay begin\n");
  
  seehi_memtest_new(1);
  printf("memtest write \n");

  for(int i=0; i<TRAIN_DDR_DELAY_MAX_RD; i=i+TRAIN_DDR_DELAY_STEP_RD)
  {
      cfg_delayline(i);
      errs=seehi_memtest_new(2);
      if(errs!=0) printf("error %x : %x\n",errs,i);
      else printf("pass %x!!!!!!!!!!!!!!!!!!!!!\n",i);
  }
  printf("train idelay end\n");
}/*}}}*/
/*}}}*/

int wl_memtest(int mode)
{
  int addr=0x40000000;
  int step=4;
  int num=10000;
  if(mode==1){
      printf("write !!!!!!!!!!!!!!\n");
      for (int i=0;i<num;i++){
          reg32(addr + i*step) = 0x12345678 + i;
      }
  }else if(mode==2){
//      printf("read !!!!!!!!!!!!!!\n");
      int rdata=0;
//      int err=0;
      for (int i=0;i<num;i++){
          rdata = reg32(addr + i*step);
          if(rdata!=0x12345678 + i){
//              err = 1;
              printf("error : ddr read %x = %x != %x\n",addr + i*step,rdata,0x12345678 + i);
              return 1;
          }else {
//              printf("pass  : ddr read %x = %x !!!!!!!\n",addr + i*step,rdata);
          }
      }
  }
  return 0;
}

void train_idelay_wl(void) /*{{{*/
{

  int errs;
  printf("train idelay begin\n");
  
  wl_memtest(1);
  printf("memtest write \n");

  for(int i=0; i<TRAIN_DDR_DELAY_MAX_RD; i=i+TRAIN_DDR_DELAY_STEP_RD)
  {
      cfg_delayline(i);
      errs=wl_memtest(2);
      if(errs!=0) printf("error %x\n",i);
      else printf("\npass %x-------------------------\n\n",i);
  }
  printf("train idelay end\n");
}/*}}}*/

typedef struct {
    unsigned int p[4];
} data16_t;
//void w16b_test(int *addr, int step, int num) {
//    data16_t buf;
//    buf.p[0] = 0;
//    int* pDst = addr;
//    for(int i=0; i<num; i+=step) {
//        *(data16_t*)pDst = *(data16_t)(buf + (data16_t)i);
//	pDst += step;
//    }
//}
//void r6b_test(int *addr, int step, int num) {
//    data16_t buf;
//    int* pSrc = addr;
//    for(int i=0; i<num; i+=step) {
//	*buf = *(data16_t*)pSrc;
//	pSrc += step;
////        printf("ddr read %x = %0161x !!!!!!!\n",i,buf);
//    }
//}

void dma_m2ddr (int num)/*{{{*/
{
/*
    for(int i=1;i<32;i++)
    {
        //reg32(SYSRAM_BASE + i*0x4) = 0x55443322+i*0x1010101;
        reg32(SYSRAM_BASE + i*0x4) = i;
    }
*/

    DMA -> CH2_SAR_0 = SYSRAM_BASE;//source address 
    printf(" DMA -> CH2_SAR_0:%x\n",DMA -> CH2_SAR_0 );

    DMA -> CH2_DAR_0 = 0x50030000;//destination address 
    printf(" DMA -> CH2_DAR_0:%x\n",DMA -> CH2_DAR_0 );

    DMA -> CH2_CTL_0 = 
        AXI_MASTER_0                         << 0|//source select master 1
        AXI_MASTER_0                         << 2|//destination select master 1
        SRC_ADDR_INCREMENT                   << 4|//source address growth
        DST_ADDR_INCREMENT                   << 6|//destination address growth 
        SRC_TRANSFER_WIDTH_128               << 8|//sourec transfer width is 64 bits
        DST_TRANSFER_WIDTH_128               <<11|//destination transfer width is 64 bits      
        //SRC_MSIZE_1                        <<14|//memory not need transaction
        DST_MSIZE_1                          <<18|//memory not need transaction
        NONPOSTED_LASTWRITE_EN               <<30;

    DMA -> CH2_CTL_32 = 
        ARLEN_EN                               <<(38-32)|//source burst length enable
        0X3                                    <<(39-32)|//source burst length is 0x4
        AWLEN_EN                               <<(47-32)|//destination burst length enable
        0x3                                    <<(48-32)|//destination burst length is 0x4
        SRC_STATUS_DISABLE                     <<(56-32)|//disable fetch status from source peripheral of channelx pointed to by the content of CHx_SSTATAR
        DST_STATUS_DISABLE                     <<(57-32)|//disable fetch status from destination peripheral of channelx pointed to by the content of CHx_DSTATAR
        INTDISABLE_COMPLETOFBLKTRANS_SHADORLLI <<(58-32)|//disables CHx_IntStatusReg.BLOCK_TFR_DONE_IntStat field
        NOTLAST_SHADORLLI                      <<(62-32)|//indicates shadow register content or the linked list item fetched from the memory not's the last one
        SHADORLLI_INVALID                      <<(63-32);//indicates the content of shadow register or the linked list item fetched from the memory is invalid

    DMA -> CH2_BLOCK_TS_0 = num;  // b:32 BLOCK_TS //write data number 

    DMA -> CH2_CFG2_0 = 
        SRC_CONTIGUOUS        << 0|//source block transfer type is single block
        DST_CONTIGUOUS        << 2;//destination block transfer type is single block

    DMA -> CH2_CFG2_32 = 
        MEM_TO_MEM_DMAC        <<(32-32)|//transfer type and Flow Control model are respectively Memory to Memory,DW_axi_dmac
        CHANNEL_PRIORITY7      <<(47-32)|//channel priority is 7
        CHANNEL_LOCK_DISABLE   <<(52-32)|//This channel not locks the host interface when other channels want to get host approval
        0x4                    <<(55-32)|//Source Outstanding Request Limit == 3
        0x4                    <<(59-32);//Destination Outstanding Request Limit == 3

    DMA -> CH2_INTSTATUS_ENABLEREG_0 = 0xffffffff;//Enable interrupt generation bit is valid
    DMA -> CH2_INTSIGNAL_ENABLEREG_0 = 0xffffffff;//Enable interrupt generation bit is valid
    DMA -> DMAC_CFGREG_0  = 0x3;//enable DMAC and its interrupt logic       
    DMA -> DMAC_CHENREG_0 = 0x202;//EN channel0  while(1)       

    printf(" m2ddr done DMA -> DMAC_CHENREG_0:%x\n",DMA -> DMAC_CHENREG_0 );

    //while generate DMA transfer completed interface  
    while(1) {
    	  if(((DMA -> CH2_INTSTATUS_0) & 0x00000002) ==  0x00000002) 
        {break;}	
    }

}/*}}}*/

void dma_ddr2m (int num)/*{{{*/
{
   // DMA -> CH2_SAR_0 = SYSRAM_BASE;//source address 
   // printf(" DMA -> CH2_SAR_0:%x\n",DMA -> CH2_SAR_0 );

   // DMA -> CH2_DAR_0 = 0x50010000;//destination address 
   // printf(" DMA -> CH2_DAR_0:%x\n",DMA -> CH2_DAR_0 );

    DMA -> CH2_SAR_0 =  0x50030000;//source address 
    printf(" DMA -> CH2_SAR_0:%x\n",DMA -> CH2_SAR_0 );

    DMA -> CH2_DAR_0 =  SYSRAM_BASE + 0x1000;//destination address 
    printf(" DMA -> CH2_DAR_0:%x\n",DMA -> CH2_DAR_0 );



    DMA -> CH2_CTL_0 = 
        AXI_MASTER_0                         << 0|//source select master 1
        AXI_MASTER_0                         << 2|//destination select master 1
        SRC_ADDR_INCREMENT                   << 4|//source address growth
        DST_ADDR_INCREMENT                   << 6|//destination address growth 
        SRC_TRANSFER_WIDTH_128               << 8|//sourec transfer width is 64 bits
        DST_TRANSFER_WIDTH_128               <<11|//destination transfer width is 64 bits      
        SRC_MSIZE_1                          <<14|//memory not need transaction
        //DST_MSIZE_1                        <<18|//memory not need transaction
        NONPOSTED_LASTWRITE_EN               <<30;

    DMA -> CH2_CTL_32 = 
        ARLEN_EN                               <<(38-32)|//source burst length enable
        0X3                                    <<(39-32)|//source burst length is 0x4
        AWLEN_EN                               <<(47-32)|//destination burst length enable
        0x3                                    <<(48-32)|//destination burst length is 0x4
        SRC_STATUS_DISABLE                     <<(56-32)|//disable fetch status from source peripheral of channelx pointed to by the content of CHx_SSTATAR
        DST_STATUS_DISABLE                     <<(57-32)|//disable fetch status from destination peripheral of channelx pointed to by the content of CHx_DSTATAR
        INTDISABLE_COMPLETOFBLKTRANS_SHADORLLI <<(58-32)|//disables CHx_IntStatusReg.BLOCK_TFR_DONE_IntStat field
        NOTLAST_SHADORLLI                      <<(62-32)|//indicates shadow register content or the linked list item fetched from the memory not's the last one
        SHADORLLI_INVALID                      <<(63-32);//indicates the content of shadow register or the linked list item fetched from the memory is invalid

    DMA -> CH2_BLOCK_TS_0 = num;  // b:32 BLOCK_TS //write data number 

    DMA -> CH2_CFG2_0 = 
        SRC_CONTIGUOUS        << 0|//source block transfer type is single block
        DST_CONTIGUOUS        << 2;//destination block transfer type is single block

    DMA -> CH2_CFG2_32 = 
        MEM_TO_MEM_DMAC        <<(32-32)|//transfer type and Flow Control model are respectively Memory to Memory,DW_axi_dmac
        CHANNEL_PRIORITY7      <<(47-32)|//channel priority is 7
        CHANNEL_LOCK_DISABLE   <<(52-32)|//This channel not locks the host interface when other channels want to get host approval
        0x4                    <<(55-32)|//Source Outstanding Request Limit == 3
        0x4                    <<(59-32);//Destination Outstanding Request Limit == 3

    DMA -> CH2_INTSTATUS_ENABLEREG_0 = 0xffffffff;//Enable interrupt generation bit is valid
    DMA -> CH2_INTSIGNAL_ENABLEREG_0 = 0xffffffff;//Enable interrupt generation bit is valid
    DMA -> DMAC_CFGREG_0  = 0x3;//enable DMAC and its interrupt logic       
    DMA -> DMAC_CHENREG_0 = 0x202;//EN channel0  while(1)       

    printf("ddr2m done DMA -> DMAC_CHENREG_0:%x\n",DMA -> DMAC_CHENREG_0 );


    //while generate DMA transfer completed interface // 0x24000000 +  0x188 + (x-1) *0x100
//    while(1) {
//    	  if(((DMA -> CH2_INTSTATUS_0) & 0x00000002) ==  0x00000002){break;}	
//    }

}/*}}}*/

void test_odelay_data(int num)
{

	 int rdata;
         REGWR(22 + PHY_BASE_ADDR,(num<<16)+0x7f,0);
         REGWR(24 + PHY_BASE_ADDR,(num<<16)+0x7f,0);
         REGWR(26 + PHY_BASE_ADDR,(num<<16)+0x7f,0);
         REGWR(29 + PHY_BASE_ADDR,(num<<16)+0x7f,0);
         REGWR(30 + PHY_BASE_ADDR,0x100,0);
	 delay(2);
	 reg32(0x50090204)=0xffffffff;
	 rdata=reg32(0x2b004058);
         printf("test_odelay_data %x,",rdata);
	 rdata=reg32(0x50090204);
         printf("test_odelay_data %x,%x\n",num,rdata);

}
void test_timing_data(int num)
{

	 int rdata;
         REGWR(0 + PHY_BASE_ADDR,(num<<8)+0x40,0);
         REGWR(3 + PHY_BASE_ADDR,(num<<8)+0x40,0);
         REGWR(6 + PHY_BASE_ADDR,(num<<8)+0x40,0);
         REGWR(9 + PHY_BASE_ADDR,(num<<8)+0x40,0);
	 delay(2);
	 reg32(0x50090200)=0xffffffff;
	 rdata=reg32(0x2b004000);
         printf("test_odelay_data %x,",rdata);
	 rdata=reg32(0x50090200);
         printf("test_odelay_data %x,%x\n",num,rdata);

}

///////////////////////////////////////////////////////////////////////////////////////////

void set_warm_rst_a55(uint8_t pid)
{
    if((pid >= CORE0) && (pid <= CORE3)) {
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | RCV ));
        while(1) {
            if((get_pmu_reg(pid,PPU_PWSR_OP_DYN_STATUS_ADDR) & 0xf) == 0xa) {
                printf("core:%d rst done\n",pid);
                set_pmu_reg(pid,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                break;
            }
        }
        // set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<pid));
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | ON ));
    } else {
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, 0xffffffff);
        for(int i=CORE0; i<=CORE3; i++) {
            set_pmu_reg(i,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | RCV ));
        } 
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                               OP4 << PPU_PWPR_OP_POLICY_LSB |
                                                 1  << PPU_PWPR_PWR_DYN_EN_LSB | RCV));
        while(1) {
            if((get_pmu_reg(pid,PPU_PWSR_OP_DYN_STATUS_ADDR) & 0xf) == 0xa) {
                printf("cluster rst done\n");
                set_pmu_reg(CORE0,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                set_pmu_reg(CORE1,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                set_pmu_reg(CORE2,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                set_pmu_reg(CORE3,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                set_pmu_reg(pid,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                break;
            }
        }
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<CORE3));
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                               OP4 << PPU_PWPR_OP_POLICY_LSB |
                                                1 << PPU_PWPR_PWR_DYN_EN_LSB | ON));
        for(int i=CORE0; i<=CORE3; i++) {
            set_pmu_reg(i,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | ON ));
        }
    }
}

void isp_fw_rst(void)
{
      set_pmu_power_on(IMG);
      set_pmu_power_on(ISP);
      // REG32(ISP_BASE+ISP_IMSC) = 0X5;
      // printf("ISP.ISP_IMSC is set to 0x%x",REG32(ISP_BASE+ISP_IMSC));
      set_pmu_fw_warm_rst(ISP);
      // if(REG32(ISP_BASE+ISP_IMSC) != 0) {
          // printf("ERROR, ISP.ISP_IMSC is 0x%x, not rst",REG32(ISP_BASE+ISP_IMSC));
          // err_cnt++;
      // } else {
          // printf("ISP.ISP_IMSC is 0x%x, rsted",REG32(ISP_BASE+ISP_IMSC));
      // }
}

void pmu_irqhandler (void) {
    uint32_t exp;
    printf("PMU interrupt occurred! gic_cnt:%d\n",gic_cnt);
    //IRQ_Disable(INT_PMU_INTR);
    uint32_t tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
    printf("read PMU_ISR:%x\n",tmp);
    if(gic_cnt == 0) {
        //default power on interrupt
        exp = get_pmu_isr(PMU);
        if(tmp == exp) {
            printf("all default power on PD power up\n");
            //check each pd ISR and clear PPU_ISR
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
                    err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_default_pwsr(i));
                }
            }
        }
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,0xffffffff);
    } else if(gic_cnt == 1) {
        exp = 0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_IMG_IRQ_LSB |
              0x1 << PMU_ISR_PPU_VPU_IRQ_LSB;
        if((tmp & exp) == exp) {
            printf("all off PD power up\n");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
                }
            }
        }
    } else if(gic_cnt == 2) {
        exp = //0x1 << PMU_ISR_PPU_CORE0_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE1_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE2_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE3_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CLUSTER_IRQ_LSB |
              0x1 << PMU_ISR_PPU_VPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_IMG_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_DDR0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DDR1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_PERI1_IRQ_LSB;

        if((tmp & exp) == exp) {
            printf("PD rst done\n");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
                }
            }
        }
    } else if(gic_cnt == 3) {
        exp = //0x1 << PMU_ISR_PPU_CORE0_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CORE1_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CORE2_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CORE3_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CLUSTER_IRQ_LSB |
              0x1 << PMU_ISR_PPU_VPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_IMG_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_DDR0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DDR1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_PERI1_IRQ_LSB;

        if((tmp & exp) == exp) {
            printf("PD power off done\n");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,OFF));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,OFF));
                }
            }
        }
    } else if(gic_cnt == 4) {
        exp = //0x1 << PMU_ISR_PPU_CORE0_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CORE1_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CORE2_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CORE3_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CLUSTER_IRQ_LSB |
              0x1 << PMU_ISR_PPU_VPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_IMG_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_DDR0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DDR1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_PERI1_IRQ_LSB;

        if((tmp & exp) == exp) {
            printf("PD power on done\n");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON));
                }
            }
        }
    }
    gic_cnt++;
    set_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR,0xffffffff);
}

void test_ddr_rst()
{
	int cnt = 0;

	REG32(0x60000000) = 0x12345678;
	printf("1 REG32(0x60000000) = 0x%x\n", REG32(0x60000000));

	set_pmu_warm_rst(DDR0);
	set_pmu_warm_rst(DDR1);

	printf("2 REG32(0x60000000) = 0x%x\n", REG32(0x60000000));
	  while(1) {
		systimer_delay(1, IN_S);
		cnt++;
		if(cnt % 10 == 0)
		printf("cnt 0x%x\n", cnt);
	  }
}

int main (void)
{   
    printf("test %s ...\n", __FILE__);
	systimer_init();

	printf("ddr start \n");
	init_ddr_fpga(0);
	printf("DDR init DONE\n");

	test_ddr_rst();

}

#else
#include "main_qemu.c"
#endif

// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
