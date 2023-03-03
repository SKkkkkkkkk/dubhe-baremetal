
/* ****************************************************************
 *        CADENCE                    Copyright (c) 2001-2022      *
 *                                   Cadence Design Systems, Inc. *
 *                                   All rights reserved.         *
 ******************************************************************
 *  The values calculated from this script are meant to be        *
 *  representative programmings.   The values may not reflect the *
 *  actual required programming for production use.   Please      *
 *  closely review all programmed values for technical accuracy   *
 *  before use in production parts.                               *
 *  The register programmings provided in this script are         * 
 *  accurate to the version of Jedec specs and memory             *
 *  models available at the time it was created.  This script     *
 *  will not be maintained for newer releases of Jedec specs      *
 *  or memory models.                                             *

 ******************************************************************
 *                                                                 
 *   Module:         regconfig.h
 *   Documentation:  Register programming header file
 *
 ******************************************************************
 ******************************************************************
 * WARNING:  This file was automatically generated.  Manual
 * editing may result in undetermined behavior.
 ******************************************************************
 ******************************************************************/
// REL: seehi.chengdu.sh100-LPDDR4_FPGA__20220913 Commit ID: 8ebded999b4cf0e8014eb6996a2f27cda5de6c88



// ********************************************************************
// Option: IP               : IP Mode                                            = PHY
// Option: BL               : Burst Length                                       = 16
// Option: CL               : CAS Latency                                        = 6
// Option: MHZ              : Simulation MHz                                     = 25
// Option: AP               : Auto Precharge Mode                          (0/1) = 0
// Option: DLLBP            : DLL Bypass Mode                              (0/1) = 0
// Option: HALF             : Half-Memory Support                          (0/1) = 0
// Option: RDIMM            : Registered Dimm Support                      (0/1) = 0
// Option: RSV2             : Reserved                                       (0) = 0
// Option: RDPREAM          : Read Preamble Length                               = 0
// Option: WRPREAM          : Write Preamble Length                              = 0
// Option: BOF              : Burst On the Fly                             (0/1) = 0
// Option: WLS              : Write Latency Set                            (0/1) = 0
// Option: OUT              : Output file                                        = regconfig_sim_PHY_25.h
// Option: WRDBIEN          : Write DBI Enable                             (0/1) = 0
// Option: RDDBIEN          : Read DBI Enable                              (0/1) = 0
// Option: RDPSTMBLE        : RD Postamble En                              (0/1) = 0
// Option: WRPSTMBLE        : WR Postamble En                              (0/1) = 0
// Option: CRC              : CRC En                                       (0/1) = 0
// Option: CRC_RD_EN        : CRC En for Reads                             (0/1) = 0
// Option: DDR5_RDIMM_MODE  : SDR1 SDR2 DDR                              (0/1/2) = 1
// Option: DATA2CMD_Ratio   : Data to CK Frequency Ratio                         = 0
// Option: DVFSC            : Dynamic Voltage Freq Scaling Core            (0/1) = 0
// Option: BANK_Mode        : Bank Architecture for LPDDR5               (0/1/2) = 2
// Option: RD_Link_ECC      : READ Link ECC Support                        (0/1) = 0
// Option: WR_Link_ECC      : WRITE Link ECC Support                       (0/1) = 0
// Option: X8_Device        : Byte Mode Support for LPDDR4 or LPDDR5       (0/1/2) = 0
// Option: PHY_LOW_LAT_MODE : PHY low latency mode                     (0/1/2/3) = 0
// Option: SOMA             : Memory-SOMA file(s)                                = jedec_lpddr4_32gb_4266,./memory/jedec_lpddr4_32gb_4266/memory.xml.sim,./memory/jedec_lpddr4_32gb_4266/memory.xml.sim
// Option: PL               : CA Parity Latency                                  = -1
// Option: LRDIMM           : Load Reduced Dimm Support                    (0/1) = 0
// Option: CLA          : RCD Command Latency Adder              (0/1/2/3/4) = 0
// Option: PBR              : PBR mode support                             (0/1) = 0
// Option: FGR        : FGR mode support                             (0/1) = 0
// Option: DDR5_1N_MODE     : 1N/2N Mode                                   (0/1) = 0
// Command-line arguments: jedec_lpddr4_32gb_4266,./memory/jedec_lpddr4_32gb_4266/memory.xml.sim,./memory/jedec_lpddr4_32gb_4266/memory.xml.sim IP=PHY BL=16 CL=6 MHZ=25 OUT=regconfig_sim_PHY_25.h
// ********************************************************************
// Memory: jedec_lpddr4_32gb_4266
// Memory: jedec_lpddr4_32gb_4266
// ********************************************************************


#define               DENALI_PHY_00_DATA 0b00000000000000000100000001000000 // FPGA_PHY_FDBK_CTRL_0:RW:24:3:=0x00 FPGA_PHY_DQS_IE_TIMING_0:RW:16:8:=0x00 FPGA_PHY_DQS_OE_TIMING_0:RW:8:8:=0x40 FPGA_PHY_DQ_OE_TIMING_0:RW:0:8:=0x40
#define               DENALI_PHY_01_DATA 0b00000000000000000000000100000000 // FPGA_PHY_RDDQS_LATENCY_ADJUST_0:RW:24:5:=0x00 FPGA_PHY_RDDATA_EN_IE_DLY_0:RW:16:2:=0x00 FPGA_PHY_RDDATA_EN_DLY_0:RW:8:5:=0x01 FPGA_PHY_LPBK_CTRL_0:RW:0:4:=0x00
#define               DENALI_PHY_02_DATA 0b00000000000000000000000000000101 // FPGA_PHY_LPBK_RESULT_OBS_0:RD:8:21:=0x000000 FPGA_PHY_RPTR_UPDATE_0:RW:0:4:=0x05
#define               DENALI_PHY_03_DATA 0b00000000000000000100000001000000 // FPGA_PHY_FDBK_CTRL_1:RW:24:3:=0x00 FPGA_PHY_DQS_IE_TIMING_1:RW:16:8:=0x00 FPGA_PHY_DQS_OE_TIMING_1:RW:8:8:=0x40 FPGA_PHY_DQ_OE_TIMING_1:RW:0:8:=0x40
#define               DENALI_PHY_04_DATA 0b00000000000000000000000100000000 // FPGA_PHY_RDDQS_LATENCY_ADJUST_1:RW:24:5:=0x00 FPGA_PHY_RDDATA_EN_IE_DLY_1:RW:16:2:=0x00 FPGA_PHY_RDDATA_EN_DLY_1:RW:8:5:=0x01 FPGA_PHY_LPBK_CTRL_1:RW:0:4:=0x00
#define               DENALI_PHY_05_DATA 0b00000000000000000000000000000101 // FPGA_PHY_LPBK_RESULT_OBS_1:RD:8:21:=0x000000 FPGA_PHY_RPTR_UPDATE_1:RW:0:4:=0x05
#define               DENALI_PHY_06_DATA 0b00000000000000000100000001000000 // FPGA_PHY_FDBK_CTRL_2:RW:24:3:=0x00 FPGA_PHY_DQS_IE_TIMING_2:RW:16:8:=0x00 FPGA_PHY_DQS_OE_TIMING_2:RW:8:8:=0x40 FPGA_PHY_DQ_OE_TIMING_2:RW:0:8:=0x40
#define               DENALI_PHY_07_DATA 0b00000000000000000000000100000000 // FPGA_PHY_RDDQS_LATENCY_ADJUST_2:RW:24:5:=0x00 FPGA_PHY_RDDATA_EN_IE_DLY_2:RW:16:2:=0x00 FPGA_PHY_RDDATA_EN_DLY_2:RW:8:5:=0x01 FPGA_PHY_LPBK_CTRL_2:RW:0:4:=0x00
#define               DENALI_PHY_08_DATA 0b00000000000000000000000000000101 // FPGA_PHY_LPBK_RESULT_OBS_2:RD:8:21:=0x000000 FPGA_PHY_RPTR_UPDATE_2:RW:0:4:=0x05
#define               DENALI_PHY_09_DATA 0b00000000000000000100000001000000 // FPGA_PHY_FDBK_CTRL_3:RW:24:3:=0x00 FPGA_PHY_DQS_IE_TIMING_3:RW:16:8:=0x00 FPGA_PHY_DQS_OE_TIMING_3:RW:8:8:=0x40 FPGA_PHY_DQ_OE_TIMING_3:RW:0:8:=0x40
#define               DENALI_PHY_10_DATA 0b00000000000000000000000100000000 // FPGA_PHY_RDDQS_LATENCY_ADJUST_3:RW:24:5:=0x00 FPGA_PHY_RDDATA_EN_IE_DLY_3:RW:16:2:=0x00 FPGA_PHY_RDDATA_EN_DLY_3:RW:8:5:=0x01 FPGA_PHY_LPBK_CTRL_3:RW:0:4:=0x00
#define               DENALI_PHY_11_DATA 0b00000000000000000000000000000101 // FPGA_PHY_LPBK_RESULT_OBS_3:RD:8:21:=0x000000 FPGA_PHY_RPTR_UPDATE_3:RW:0:4:=0x05
#define               DENALI_PHY_12_DATA 0b00000000000000000000000000000000 // FPGA_PHY_LP4_PST_AMBLE:RW:24:2:=0x00 FPGA_PHY_DBI_MODE:RW:16:1:=0x00 SC_FPGA_PHY_LPBK_ERR_CLEAR:RW+:8:1:=0x00 FPGA_PHY_LPBK_ENABLE:RW:0:1:=0x00
#define               DENALI_PHY_13_DATA 0b00000000000000000000001100000110 // SC_FPGA_PHY_MANUAL_CLEAR:RW+:16:2:=0x00 FPGA_PHY_TWO_CYC_PREAMBLE:RW:8:3:=0x03 FPGA_PHY_MEM_CLASS:RW:0:3:=0x06
#define               DENALI_PHY_14_DATA 0b00000000000000000000111100000000 // FPGA_PHY_ADRCTL_SW_TXIO_CTRL_0:RW+:8:4:=0x0f FPGA_PHY_ADRCTL_LPBK_CTRL_0:RW:0:7:=0x00
#define               DENALI_PHY_15_DATA 0b00000000000000000000000000000000 // FPGA_PHY_ADRCTL_LPBK_RESULT_OBS_0:RD:0:32:=0x00000000
#define               DENALI_PHY_16_DATA 0b00000000000000000000111100000000 // FPGA_PHY_ADRCTL_SW_TXIO_CTRL_1:RW+:8:4:=0x0f FPGA_PHY_ADRCTL_LPBK_CTRL_1:RW:0:7:=0x00
#define               DENALI_PHY_17_DATA 0b00000000000000000000000000000000 // FPGA_PHY_ADRCTL_LPBK_RESULT_OBS_1:RD:0:32:=0x00000000
#define               DENALI_PHY_18_DATA 0b00000000000000000000111100000000 // FPGA_PHY_ADRCTL_SW_TXIO_CTRL_2:RW+:8:4:=0x0f FPGA_PHY_ADRCTL_LPBK_CTRL_2:RW:0:7:=0x00
#define               DENALI_PHY_19_DATA 0b00000000000000000000000000000000 // FPGA_PHY_ADRCTL_LPBK_RESULT_OBS_2:RD:0:32:=0x00000000
#define               DENALI_PHY_20_DATA 0b00001111000000000000000000000000 // FPGA_PHY_INIT_WAIT:RW:24:4:=0x0f FPGA_PHY_DRAM_CLK_DISABLE:RW:16:1:=0x00 SC_FPGA_PHY_ADRCTL_LPBK_ERR_CLEAR:RW+:8:1:=0x00 FPGA_PHY_ADRCTL_LPBK_ENABLE:RW:0:1:=0x00
#define               DENALI_PHY_21_DATA 0b00000000000000000000000000000000 // FPGA_PHY_CLK_ODELAY_0:RW:8:9:=0x0000 FPGA_PHY_INIT_CLEAR:RW:0:1:=0x00
#define               DENALI_PHY_22_DATA 0b00000000000000000000000001111111 // FPGA_PHY_DQS_ODELAY_0:RW:16:9:=0x0000 FPGA_PHY_DQS_IDELAY_0:RW:0:9:=0x007f
#define               DENALI_PHY_23_DATA 0b00000000000000000000000001111111 // FPGA_PHY_FEEDBACK_ODELAY_0:RW:16:9:=0x0000 FPGA_PHY_FEEDBACK_IDELAY_0:RW:0:9:=0x007f
#define               DENALI_PHY_24_DATA 0b00000000000000000000000001111111 // FPGA_PHY_DQS_ODELAY_1:RW:16:9:=0x0000 FPGA_PHY_DQS_IDELAY_1:RW:0:9:=0x007f
#define               DENALI_PHY_25_DATA 0b00000000000000000000000001111111 // FPGA_PHY_FEEDBACK_ODELAY_1:RW:16:9:=0x0000 FPGA_PHY_FEEDBACK_IDELAY_1:RW:0:9:=0x007f
#define               DENALI_PHY_26_DATA 0b00000000000000000000000001111111 // FPGA_PHY_DQS_ODELAY_2:RW:16:9:=0x0000 FPGA_PHY_DQS_IDELAY_2:RW:0:9:=0x007f
#define               DENALI_PHY_27_DATA 0b00000000000000000000000001111111 // FPGA_PHY_FEEDBACK_ODELAY_2:RW:16:9:=0x0000 FPGA_PHY_FEEDBACK_IDELAY_2:RW:0:9:=0x007f
#define               DENALI_PHY_28_DATA 0b00000000000000000000000001111111 // FPGA_PHY_DQS_ODELAY_3:RW:16:9:=0x0000 FPGA_PHY_DQS_IDELAY_3:RW:0:9:=0x007f
#define               DENALI_PHY_29_DATA 0b00000000000000000000000001111111 // FPGA_PHY_FEEDBACK_ODELAY_3:RW:16:9:=0x0000 FPGA_PHY_FEEDBACK_IDELAY_3:RW:0:9:=0x007f
#define               DENALI_PHY_30_DATA 0b00000000000000000000000100000001 // SC_FPGA_PHY_ODELAY_UPDATE:RW+:8:1:=0x01 SC_FPGA_PHY_IDELAY_UPDATE:RW+:0:1:=0x01
//* ********************************************************
//* The below are used only for simulation phy purposes.
//* Please ignore if using integrated Cadence PHY.
//* SimulationValues {{{
//* ********************************************************
#define SIM_PHY_VALS_DEFINED 1 // Global define for simulation
#define                        SMALL_TREF_ENABLE 0x0 // CTL
#define                                  MR8_VAL 0x18 // CTL
#define                                  MR0_VAL 0x0 // CTL
#define                            ODT_RD_TIMING 0x0 // CTL
#define                                  TAC_MIN 0x0 // CTL
#define                                  TAC_MAX 0x0 // CTL
#define               LPDDR1_PD_CLK_GATE_DISABLE 0x0 // CTL
#define                          LPDDR1_ATTACHED 0x0 // CTL
//* ********************************************************
//* SimulationValues }}}
//* ********************************************************
