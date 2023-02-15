
/******************************************************************
 *        CADENCE                    Copyright (c) 2001-2022      *
 *                                   Cadence Design Systems, Inc. *
 *                                   All rights reserved.         *
 *                                                                *
 *  This work may not be copied, modified, re-published,          *
 *  uploaded, executed, or distributed in any way, in any medium, *
 *  whether in whole or in part, without prior written permission *
 *  from Cadence Design Systems, Inc.                             *
 ******************************************************************
 ******************************************************************
 *                                                                 
 *   Module:         define.h
 *   Date:           Tue Aug  9 19:05:17 PDT 2022
 *   Documentation:  Configuration header
 *   Info:           
 ******************************************************************
 ******************************************************************/

/*****************************************************************
 * WARNING:  This file was automatically generated.  Manual
 * editing may result in undetermined behavior, and may be in
 * violation of the Denali Support Agreement.
 *****************************************************************/
// REL: seehi.chengdu.sh100-LPDDR4__20220809 Commit ID: 2ddadd2cea5793adea25ae6124a590291c6640e5

// timescale 10ps/1ps

#define ADDER_NUM_DELAYS               24                   // number of delay elements in the adder digital delay line
#define ADDER_NUM_DELAYS_WIDTH         5                    // 
#define ADDR_CTRL_BLOCK_CNT            4                    // 
#define ADRCTL_ADDER_NUM_DELAYS        24                   // number of delay elements in the address/control adder digital delay line
#define ADRCTL_ADDER_NUM_DELAYS_WIDTH  5                    // 
#define ADRCTL_BASE_NUM_DELAYS         104                  // number of delay elements in the address/control base digital delay line
#define ADRCTL_BASE_NUM_DELAYS_WIDTH   7                    // 
#define ADRCTL_SLICE_SCAN_COUNT        20                   // 
#define BASE_NUM_DELAYS                104                  // number of delay elements in the base digital delay line
#define BASE_NUM_DELAYS_WIDTH          7                    // 
#define BUF_DLY                        4.0                  // buffer delay        
#define CAL_BLOCK_COUNT                1                    // 
#define CAL_NUM_DELAYS                 4                    // number of delay elements in the calibration clk digital delay line
#define CDN_CAL_ABT_WIDTH              7                    // 
#define CDN_CAL_VREF_COUNT             6                    // 
#define CLK_Q_DLY                      7.0                  // flop clock to output delay        
#define CLK_Q_DRV_DLY                  8.0                  // addr/ctrl driving flop clock to output delay        
#define DLY_NAND_DLY                   1.0                  // 2 input nand delay (delay element)        
#define DQS_INV_DLY                    1.5                  // Read DQS inverter delay        
#define FLIGHT_DELAY                   50                   // memory to controller flight time delay        
#define FULL_NUM_DELAYS                128                  // number of delay elements in the master digital delay line
#define FULL_NUM_DELAYS_WIDTH          8                    // 
#define GT_OUT_DLY                     5.0                  // clock gate output delay        
#define GT_SETUP_DLY                   12.0                 // clock gate setup delay        
#define INV_DLY                        1.5                  // inverter delay        
#define IO_PAD_DELAY_OUT               30                   // Output delay through IO Pad from ASIC to memory       
#define LVL_DATA_TOG_WIDTH             9                    // 
#define LVL_DATA_WIDTH                 18                   // 
#define LVL_MODE_WIDTH                 4                    // 
#define LVL_RESP_WIDTH                 2                    // 
#define MAX_ADRCTL_REG_ADDR_WIDTH      8                    // 
#define MAX_CTL_REG_ADDR_WIDTH         10                   // 
#define MAX_PHY_REG_ADDR_WIDTH         12                   // 
#define MAX_SLICE_REG_ADDR_WIDTH       8                    // 
#define MCB_SCAN_COUNT                 3                    // 
#define MUX2_DLY                       6.5                  // 2 input mux delay        
#define MUX_NAND_DLY                   2.0                  // 2 input nand delay (mux)        
#define NAND_DLY                       2.0                  // 2 input nand delay        
#define NOR2_DLY                       3.5                  // 2 input nor delay        
#define NUM_OF_DFI_DLY_STAGES          1                    // 
#define ON_FLY_GATE_ADJUST_EN_ADDR     17                   // 
#define ON_FLY_GATE_ADJUST_EN_OFFSET   16                   // 
#define ON_FLY_GATE_ADJUST_EN_WIDTH    2                    // 
#define PD                             #4                   // synchronous logic delay in the phy
#define PHY_AC_BASE_ADDR               5376                 // 
#define PHY_AC_BYPASS_OVERRIDE_ADDR    1067 + PHY_BASE_ADDR // 
#define PHY_AC_BYPASS_OVERRIDE_OFFSET  24                   // 
#define PHY_AC_BYPASS_OVERRIDE_WIDTH   1                    // 
#define PHY_AC_BYPASS_SLAVE_DELAY_ADDR 1066 + PHY_BASE_ADDR // 
#define PHY_AC_BYPASS_SLAVE_DELAY_OFFSET 8                    // 
#define PHY_AC_BYPASS_SLAVE_DELAY_WIDTH 10                   // 
#define PHY_AC_CLK_LPBK_CONTROL_ADDR   1054 + PHY_BASE_ADDR // 
#define PHY_AC_CLK_LPBK_CONTROL_OFFSET 16                   // 
#define PHY_AC_CLK_LPBK_CONTROL_WIDTH  6                    // 
#define PHY_AC_CLK_LPBK_ENABLE_ADDR    1054 + PHY_BASE_ADDR // 
#define PHY_AC_CLK_LPBK_ENABLE_OFFSET  8                    // 
#define PHY_AC_CLK_LPBK_ENABLE_WIDTH   1                    // 
#define PHY_AC_CLK_LPBK_OBS_SELECT_ADDR 1054 + PHY_BASE_ADDR // 
#define PHY_AC_CLK_LPBK_OBS_SELECT_OFFSET 0                    // 
#define PHY_AC_CLK_LPBK_OBS_SELECT_WIDTH 1                    // 
#define PHY_AC_CLK_LPBK_RESULT_OBS_ADDR 1055 + PHY_BASE_ADDR // 
#define PHY_AC_CLK_LPBK_RESULT_OBS_OFFSET 0                    // 
#define PHY_AC_CLK_LPBK_RESULT_OBS_WIDTH 16                   // 
#define PHY_AC_DCC_RXCAL_CTRL_GATE_DISABLE_ADDR 1071 + PHY_BASE_ADDR // 
#define PHY_AC_DCC_RXCAL_CTRL_GATE_DISABLE_OFFSET 24                   // 
#define PHY_AC_DCC_RXCAL_CTRL_GATE_DISABLE_WIDTH 1                    // 
#define PHY_AC_INIT_COMPLETE_OBS_ADDR  62 + PHY_AC_BASE_ADDR // 
#define PHY_AC_INIT_COMPLETE_OBS_OFFSET 8                    // 
#define PHY_AC_INIT_COMPLETE_OBS_WIDTH 12                   // 
#define PHY_AC_LPBK_CONTROL_ADDR       1072 + PHY_BASE_ADDR // 
#define PHY_AC_LPBK_CONTROL_OFFSET     0                    // 
#define PHY_AC_LPBK_CONTROL_WIDTH      9                    // 
#define PHY_AC_LPBK_ENABLE_ADDR        1051 + PHY_BASE_ADDR // 
#define PHY_AC_LPBK_ENABLE_OFFSET      16                   // 
#define PHY_AC_LPBK_ENABLE_WIDTH       4                    // 
#define PHY_AC_LPBK_ERR_CLEAR_ADDR     1051 + PHY_BASE_ADDR // 
#define PHY_AC_LPBK_ERR_CLEAR_OFFSET   0                    // 
#define PHY_AC_LPBK_ERR_CLEAR_WIDTH    1                    // 
#define PHY_AC_LPBK_OBS_SELECT_ADDR    1051 + PHY_BASE_ADDR // 
#define PHY_AC_LPBK_OBS_SELECT_OFFSET  8                    // 
#define PHY_AC_LPBK_OBS_SELECT_WIDTH   2                    // 
#define PHY_AC_LPBK_RESULT_OBS_ADDR    1053 + PHY_BASE_ADDR // 
#define PHY_AC_LPBK_RESULT_OBS_OFFSET  0                    // 
#define PHY_AC_LPBK_RESULT_OBS_WIDTH   32                   // 
#define PHY_AC_PRBS_PATTERN_MASK_ADDR  1052 + PHY_BASE_ADDR // 
#define PHY_AC_PRBS_PATTERN_MASK_OFFSET 0                    // 
#define PHY_AC_PRBS_PATTERN_MASK_WIDTH 4                    // 
#define PHY_AC_PRBS_PATTERN_START_ADDR 1051 + PHY_BASE_ADDR // 
#define PHY_AC_PRBS_PATTERN_START_OFFSET 24                   // 
#define PHY_AC_PRBS_PATTERN_START_WIDTH 7                    // 
#define PHY_AC_PWR_RDC_DISABLE_ADDR    1055 + PHY_BASE_ADDR // 
#define PHY_AC_PWR_RDC_DISABLE_OFFSET  16                   // 
#define PHY_AC_PWR_RDC_DISABLE_WIDTH   1                    // 
#define PHY_AC_SLV_DLY_CTRL_GATE_DISABLE_ADDR 1055 + PHY_BASE_ADDR // 
#define PHY_AC_SLV_DLY_CTRL_GATE_DISABLE_OFFSET 24                   // 
#define PHY_AC_SLV_DLY_CTRL_GATE_DISABLE_WIDTH 1                    // 
#define PHY_ACBIT0_IS_TRAINED_2_ADDR   1085 + PHY_BASE_ADDR // 
#define PHY_ACBIT0_IS_TRAINED_2_OFFSET 0                    // 
#define PHY_ACBIT0_IS_TRAINED_2_WIDTH  1                    // 
#define PHY_ACBIT0_IS_TRAINED_3_ADDR   1086 + PHY_BASE_ADDR // 
#define PHY_ACBIT0_IS_TRAINED_3_OFFSET 0                    // 
#define PHY_ACBIT0_IS_TRAINED_3_WIDTH  1                    // 
#define PHY_ACBIT0_SLAVE_DELAY_0_ADDR  1075 + PHY_BASE_ADDR // 
#define PHY_ACBIT0_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_ACBIT0_SLAVE_DELAY_0_WIDTH 10                   // 
#define PHY_ACBIT0_SLAVE_DELAY_1_ADDR  1077 + PHY_BASE_ADDR // 
#define PHY_ACBIT0_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_ACBIT0_SLAVE_DELAY_1_WIDTH 10                   // 
#define PHY_ACBIT0_SLAVE_DELAY_2_ADDR  1079 + PHY_BASE_ADDR // 
#define PHY_ACBIT0_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_ACBIT0_SLAVE_DELAY_2_WIDTH 10                   // 
#define PHY_ACBIT0_SLAVE_DELAY_3_ADDR  1081 + PHY_BASE_ADDR // 
#define PHY_ACBIT0_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_ACBIT0_SLAVE_DELAY_3_WIDTH 10                   // 
#define PHY_ACBIT1_IS_TRAINED_2_ADDR   1085 + PHY_BASE_ADDR // 
#define PHY_ACBIT1_IS_TRAINED_2_OFFSET 8                    // 
#define PHY_ACBIT1_IS_TRAINED_2_WIDTH  1                    // 
#define PHY_ACBIT1_IS_TRAINED_3_ADDR   1087 + PHY_BASE_ADDR // 
#define PHY_ACBIT1_IS_TRAINED_3_OFFSET 0                    // 
#define PHY_ACBIT1_IS_TRAINED_3_WIDTH  1                    // 
#define PHY_ACBIT1_SLAVE_DELAY_0_ADDR  1075 + PHY_BASE_ADDR // 
#define PHY_ACBIT1_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_ACBIT1_SLAVE_DELAY_0_WIDTH 10                   // 
#define PHY_ACBIT1_SLAVE_DELAY_1_ADDR  1077 + PHY_BASE_ADDR // 
#define PHY_ACBIT1_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_ACBIT1_SLAVE_DELAY_1_WIDTH 10                   // 
#define PHY_ACBIT1_SLAVE_DELAY_2_ADDR  1079 + PHY_BASE_ADDR // 
#define PHY_ACBIT1_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_ACBIT1_SLAVE_DELAY_2_WIDTH 10                   // 
#define PHY_ACBIT1_SLAVE_DELAY_3_ADDR  1082 + PHY_BASE_ADDR // 
#define PHY_ACBIT1_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_ACBIT1_SLAVE_DELAY_3_WIDTH 10                   // 
#define PHY_ACBIT2_IS_TRAINED_2_ADDR   1085 + PHY_BASE_ADDR // 
#define PHY_ACBIT2_IS_TRAINED_2_OFFSET 16                   // 
#define PHY_ACBIT2_IS_TRAINED_2_WIDTH  1                    // 
#define PHY_ACBIT2_IS_TRAINED_3_ADDR   1088 + PHY_BASE_ADDR // 
#define PHY_ACBIT2_IS_TRAINED_3_OFFSET 0                    // 
#define PHY_ACBIT2_IS_TRAINED_3_WIDTH  1                    // 
#define PHY_ACBIT2_SLAVE_DELAY_0_ADDR  1076 + PHY_BASE_ADDR // 
#define PHY_ACBIT2_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_ACBIT2_SLAVE_DELAY_0_WIDTH 10                   // 
#define PHY_ACBIT2_SLAVE_DELAY_1_ADDR  1078 + PHY_BASE_ADDR // 
#define PHY_ACBIT2_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_ACBIT2_SLAVE_DELAY_1_WIDTH 10                   // 
#define PHY_ACBIT2_SLAVE_DELAY_2_ADDR  1080 + PHY_BASE_ADDR // 
#define PHY_ACBIT2_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_ACBIT2_SLAVE_DELAY_2_WIDTH 10                   // 
#define PHY_ACBIT2_SLAVE_DELAY_3_ADDR  1083 + PHY_BASE_ADDR // 
#define PHY_ACBIT2_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_ACBIT2_SLAVE_DELAY_3_WIDTH 10                   // 
#define PHY_ACBIT3_IS_TRAINED_2_ADDR   1085 + PHY_BASE_ADDR // 
#define PHY_ACBIT3_IS_TRAINED_2_OFFSET 24                   // 
#define PHY_ACBIT3_IS_TRAINED_2_WIDTH  1                    // 
#define PHY_ACBIT3_IS_TRAINED_3_ADDR   1089 + PHY_BASE_ADDR // 
#define PHY_ACBIT3_IS_TRAINED_3_OFFSET 0                    // 
#define PHY_ACBIT3_IS_TRAINED_3_WIDTH  1                    // 
#define PHY_ACBIT3_SLAVE_DELAY_0_ADDR  1076 + PHY_BASE_ADDR // 
#define PHY_ACBIT3_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_ACBIT3_SLAVE_DELAY_0_WIDTH 10                   // 
#define PHY_ACBIT3_SLAVE_DELAY_1_ADDR  1078 + PHY_BASE_ADDR // 
#define PHY_ACBIT3_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_ACBIT3_SLAVE_DELAY_1_WIDTH 10                   // 
#define PHY_ACBIT3_SLAVE_DELAY_2_ADDR  1080 + PHY_BASE_ADDR // 
#define PHY_ACBIT3_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_ACBIT3_SLAVE_DELAY_2_WIDTH 10                   // 
#define PHY_ACBIT3_SLAVE_DELAY_3_ADDR  1084 + PHY_BASE_ADDR // 
#define PHY_ACBIT3_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_ACBIT3_SLAVE_DELAY_3_WIDTH 10                   // 
#define PHY_ACBIT_SHIFT_OBS_ADDR       64 + PHY_AC_BASE_ADDR // 
#define PHY_ACBIT_SHIFT_OBS_OFFSET     0                    // 
#define PHY_ACBIT_SHIFT_OBS_SELECT_ADDR 63 + PHY_AC_BASE_ADDR // 
#define PHY_ACBIT_SHIFT_OBS_SELECT_OFFSET 16                   // 
#define PHY_ACBIT_SHIFT_OBS_SELECT_WIDTH 4                    // 
#define PHY_ACBIT_SHIFT_OBS_WIDTH      4                    // 
#define PHY_ACBIT_SLV_DLY_ENC_OBS_ADDR 63 + PHY_AC_BASE_ADDR // 
#define PHY_ACBIT_SLV_DLY_ENC_OBS_OFFSET 24                   // 
#define PHY_ACBIT_SLV_DLY_ENC_OBS_SELECT_ADDR 63 + PHY_AC_BASE_ADDR // 
#define PHY_ACBIT_SLV_DLY_ENC_OBS_SELECT_OFFSET 8                    // 
#define PHY_ACBIT_SLV_DLY_ENC_OBS_SELECT_WIDTH 5                    // 
#define PHY_ACBIT_SLV_DLY_ENC_OBS_WIDTH 8                    // 
#define PHY_ACS_PCLK_CNTR_CFG_ADDR     23 + PHY_AC_BASE_ADDR // 
#define PHY_ACS_PCLK_CNTR_CFG_OFFSET   24                   // 
#define PHY_ACS_PCLK_CNTR_CFG_WIDTH    7                    // 
#define PHY_ADDRESS_WIDTH              12                   // 
#define PHY_ADR0_CLK_WR_SLAVE_DELAY_WIDTH 10                   // 
#define PHY_ADR1_CLK_WR_SLAVE_DELAY_WIDTH 10                   // 
#define PHY_ADR2_CLK_WR_SLAVE_DELAY_WIDTH 10                   // 
#define PHY_ADR3_CLK_WR_SLAVE_DELAY_WIDTH 10                   // 
#define PHY_ADR4_CLK_WR_SLAVE_DELAY_WIDTH 10                   // 
#define PHY_ADR5_CLK_WR_SLAVE_DELAY_WIDTH 10                   // 
#define PHY_ADR_ADDR_SEL_ADDR          1028 + PHY_BASE_ADDR // 
#define PHY_ADR_ADDR_SEL_OFFSET        0                    // 
#define PHY_ADR_ADDR_SEL_WIDTH         24                   // 
#define PHY_ADR_BYTE_MODE_ADDR         1028 + PHY_BASE_ADDR // 
#define PHY_ADR_BYTE_MODE_OFFSET       24                   // 
#define PHY_ADR_BYTE_MODE_WIDTH        1                    // 
#define PHY_ADR_CA_TRAIN_MODE_1_ADDR   1029 + PHY_BASE_ADDR // 
#define PHY_ADR_CA_TRAIN_MODE_1_OFFSET 0                    // 
#define PHY_ADR_CA_TRAIN_MODE_1_WIDTH  1                    // 
#define PHY_ADR_CALVL_CAPTURE_CNT_ADDR 1044 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_CAPTURE_CNT_OFFSET 0                    // 
#define PHY_ADR_CALVL_CAPTURE_CNT_WIDTH 4                    // 
#define PHY_ADR_CALVL_CH0_OBS0_ADDR    1032 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_CH0_OBS0_OFFSET  0                    // 
#define PHY_ADR_CALVL_CH0_OBS0_WIDTH   32                   // 
#define PHY_ADR_CALVL_CH1_OBS0_ADDR    1033 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_CH1_OBS0_OFFSET  0                    // 
#define PHY_ADR_CALVL_CH1_OBS0_WIDTH   32                   // 
#define PHY_ADR_CALVL_CH2_OBS0_ADDR    1034 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_CH2_OBS0_OFFSET  0                    // 
#define PHY_ADR_CALVL_CH2_OBS0_WIDTH   32                   // 
#define PHY_ADR_CALVL_CH3_OBS0_ADDR    1035 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_CH3_OBS0_OFFSET  0                    // 
#define PHY_ADR_CALVL_CH3_OBS0_WIDTH   32                   // 
#define PHY_ADR_CALVL_COARSE_DLY_ADDR  1024 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_COARSE_DLY_OFFSET 16                   // 
#define PHY_ADR_CALVL_COARSE_DLY_WIDTH 10                   // 
#define PHY_ADR_CALVL_DEBUG_MODE_ADDR  1030 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_DEBUG_MODE_OFFSET 16                   // 
#define PHY_ADR_CALVL_DEBUG_MODE_WIDTH 1                    // 
#define PHY_ADR_CALVL_DLY_STEP_ADDR    1043 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_DLY_STEP_OFFSET  0                    // 
#define PHY_ADR_CALVL_DLY_STEP_WIDTH   4                    // 
#define PHY_ADR_CALVL_OBS1_ADDR        1036 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_OBS1_OFFSET      0                    // 
#define PHY_ADR_CALVL_OBS1_WIDTH       32                   // 
#define PHY_ADR_CALVL_OBS2_ADDR        1037 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_OBS2_OFFSET      0                    // 
#define PHY_ADR_CALVL_OBS2_WIDTH       32                   // 
#define PHY_ADR_CALVL_OBS_SELECT_ADDR  1031 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_OBS_SELECT_OFFSET 8                    // 
#define PHY_ADR_CALVL_OBS_SELECT_WIDTH 3                    // 
#define PHY_ADR_CALVL_PERIODIC_START_OFFSET_ADDR 1030 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_PERIODIC_START_OFFSET_OFFSET 0                    // 
#define PHY_ADR_CALVL_PERIODIC_START_OFFSET_WIDTH 9                    // 
#define PHY_ADR_CALVL_QTR_ADDR         1025 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_QTR_OFFSET       16                   // 
#define PHY_ADR_CALVL_QTR_WIDTH        10                   // 
#define PHY_ADR_CALVL_RANK_CTRL_ADDR   1029 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_RANK_CTRL_OFFSET 16                   // 
#define PHY_ADR_CALVL_RANK_CTRL_WIDTH  2                    // 
#define PHY_ADR_CALVL_RESP_WAIT_CNT_ADDR 1029 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_RESP_WAIT_CNT_OFFSET 24                   // 
#define PHY_ADR_CALVL_RESP_WAIT_CNT_WIDTH 4                    // 
#define PHY_ADR_CALVL_START_ADDR       1024 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_START_OFFSET     0                    // 
#define PHY_ADR_CALVL_START_WIDTH      10                   // 
#define PHY_ADR_CALVL_SWIZZLE0_ADDR    1026 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_SWIZZLE0_OFFSET  0                    // 
#define PHY_ADR_CALVL_SWIZZLE0_WIDTH   32                   // 
#define PHY_ADR_CALVL_TRAIN_MASK_ADDR  1042 + PHY_BASE_ADDR // 
#define PHY_ADR_CALVL_TRAIN_MASK_OFFSET 0                    // 
#define PHY_ADR_CALVL_TRAIN_MASK_WIDTH 6                    // 
#define PHY_ADR_CLK_WR_BYPASS_SLAVE_DELAY_ADDR 1067 + PHY_BASE_ADDR // 
#define PHY_ADR_CLK_WR_BYPASS_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_ADR_CLK_WR_BYPASS_SLAVE_DELAY_WIDTH 10                   // 
#define PHY_ADR_CSLVL_TRAIN_MASK_ADDR  1027 + PHY_BASE_ADDR // 
#define PHY_ADR_CSLVL_TRAIN_MASK_OFFSET 0                    // 
#define PHY_ADR_CSLVL_TRAIN_MASK_WIDTH 6                    // 
#define PHY_ADR_MEAS_DLY_STEP_ENABLE_ADDR 1027 + PHY_BASE_ADDR // 
#define PHY_ADR_MEAS_DLY_STEP_ENABLE_OFFSET 16                   // 
#define PHY_ADR_MEAS_DLY_STEP_ENABLE_WIDTH 1                    // 
#define PHY_ADR_MEAS_DLY_STEP_VALUE_ADDR 1027 + PHY_BASE_ADDR // 
#define PHY_ADR_MEAS_DLY_STEP_VALUE_OFFSET 8                    // 
#define PHY_ADR_MEAS_DLY_STEP_VALUE_WIDTH 8                    // 
#define PHY_ADR_MEM_CLASS_ADDR         1029 + PHY_BASE_ADDR // 
#define PHY_ADR_MEM_CLASS_OFFSET       8                    // 
#define PHY_ADR_MEM_CLASS_WIDTH        3                    // 
#define PHY_ADR_SW_CALVL_DVW_MIN_ADDR  1042 + PHY_BASE_ADDR // 
#define PHY_ADR_SW_CALVL_DVW_MIN_EN_ADDR 1042 + PHY_BASE_ADDR // 
#define PHY_ADR_SW_CALVL_DVW_MIN_EN_OFFSET 24                   // 
#define PHY_ADR_SW_CALVL_DVW_MIN_EN_WIDTH 1                    // 
#define PHY_ADR_SW_CALVL_DVW_MIN_OFFSET 8                    // 
#define PHY_ADR_SW_CALVL_DVW_MIN_WIDTH 9                    // 
#define PHY_ADRCTL_LP3_RX_CAL_ADDR     33 + PHY_AC_BASE_ADDR // 
#define PHY_ADRCTL_LP3_RX_CAL_OFFSET   16                   // 
#define PHY_ADRCTL_LP3_RX_CAL_WIDTH    13                   // 
#define PHY_ADRCTL_LPDDR_ADDR          1046 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_LPDDR_OFFSET        8                    // 
#define PHY_ADRCTL_LPDDR_WIDTH         1                    // 
#define PHY_ADRCTL_MANUAL_CLEAR_ADDR   1044 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MANUAL_CLEAR_OFFSET 16                   // 
#define PHY_ADRCTL_MANUAL_CLEAR_WIDTH  2                    // 
#define PHY_ADRCTL_MANUAL_UPDATE_ADDR  1050 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MANUAL_UPDATE_OFFSET 24                   // 
#define PHY_ADRCTL_MANUAL_UPDATE_WIDTH 1                    // 
#define PHY_ADRCTL_MASTER_DELAY_HALF_MEASURE_ADDR 1074 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MASTER_DELAY_HALF_MEASURE_OFFSET 16                   // 
#define PHY_ADRCTL_MASTER_DELAY_HALF_MEASURE_WIDTH 8                    // 
#define PHY_ADRCTL_MASTER_DELAY_LESS_STEP_ADDR 1073 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MASTER_DELAY_LESS_STEP_OFFSET 0                    // 
#define PHY_ADRCTL_MASTER_DELAY_LESS_STEP_WIDTH 3                    // 
#define PHY_ADRCTL_MASTER_DELAY_LOCK_ERROR_CNT_OBS_ADDR 1044 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MASTER_DELAY_LOCK_ERROR_CNT_OBS_OFFSET 24                   // 
#define PHY_ADRCTL_MASTER_DELAY_LOCK_ERROR_CNT_OBS_WIDTH 8                    // 
#define PHY_ADRCTL_MASTER_DELAY_LOCK_MODE_ADDR 1072 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MASTER_DELAY_LOCK_MODE_OFFSET 24                   // 
#define PHY_ADRCTL_MASTER_DELAY_LOCK_MODE_WIDTH 3                    // 
#define PHY_ADRCTL_MASTER_DELAY_START_ADDR 1073 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MASTER_DELAY_START_OFFSET 16                   // 
#define PHY_ADRCTL_MASTER_DELAY_START_WIDTH 11                   // 
#define PHY_ADRCTL_MASTER_DELAY_STEP_ADDR 1074 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MASTER_DELAY_STEP_OFFSET 0                    // 
#define PHY_ADRCTL_MASTER_DELAY_STEP_WIDTH 6                    // 
#define PHY_ADRCTL_MASTER_DELAY_THRESHOLD_FACTOR_ADDR 1073 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MASTER_DELAY_THRESHOLD_FACTOR_OFFSET 8                    // 
#define PHY_ADRCTL_MASTER_DELAY_THRESHOLD_FACTOR_WIDTH 3                    // 
#define PHY_ADRCTL_MASTER_DELAY_WAIT_ADDR 1074 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MASTER_DELAY_WAIT_OFFSET 8                    // 
#define PHY_ADRCTL_MASTER_DELAY_WAIT_WIDTH 8                    // 
#define PHY_ADRCTL_MASTER_DLY_LOCK_OBS_ADDR 1045 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MASTER_DLY_LOCK_OBS_OFFSET 8                    // 
#define PHY_ADRCTL_MASTER_DLY_LOCK_OBS_SELECT_ADDR 1045 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_MASTER_DLY_LOCK_OBS_SELECT_OFFSET 0                    // 
#define PHY_ADRCTL_MASTER_DLY_LOCK_OBS_SELECT_WIDTH 4                    // 
#define PHY_ADRCTL_MASTER_DLY_LOCK_OBS_WIDTH 11                   // 
#define PHY_ADRCTL_MSTR_DLY_ENC_SEL_ADDR 55 + PHY_AC_BASE_ADDR // 
#define PHY_ADRCTL_MSTR_DLY_ENC_SEL_OFFSET 8                    // 
#define PHY_ADRCTL_MSTR_DLY_ENC_SEL_WIDTH 2                    // 
#define PHY_ADRCTL_PVT_MAP_0_ADDR      46 + PHY_AC_BASE_ADDR // 
#define PHY_ADRCTL_PVT_MAP_0_OFFSET    0                    // 
#define PHY_ADRCTL_PVT_MAP_0_WIDTH     7                    // 
#define PHY_ADRCTL_PVT_MAP_WIDTH       7                    // 
#define PHY_ADRCTL_RX_CAL_ADDR         33 + PHY_AC_BASE_ADDR // 
#define PHY_ADRCTL_RX_CAL_OFFSET       0                    // 
#define PHY_ADRCTL_RX_CAL_WIDTH        10                   // 
#define PHY_ADRCTL_SLAVE_LOOP_CNT_UPDATE_ADDR 1045 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_SLAVE_LOOP_CNT_UPDATE_OFFSET 24                   // 
#define PHY_ADRCTL_SLAVE_LOOP_CNT_UPDATE_WIDTH 3                    // 
#define PHY_ADRCTL_SNAP_OBS_REGS_ADDR  1046 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_SNAP_OBS_REGS_OFFSET 0                    // 
#define PHY_ADRCTL_SNAP_OBS_REGS_WIDTH 1                    // 
#define PHY_ADRCTL_STATIC_TOG_DISABLE_ADDR 1050 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_STATIC_TOG_DISABLE_OFFSET 16                   // 
#define PHY_ADRCTL_STATIC_TOG_DISABLE_WIDTH 4                    // 
#define PHY_ADRCTL_SW_MASTER_MODE_ADDR 1072 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_SW_MASTER_MODE_OFFSET 16                   // 
#define PHY_ADRCTL_SW_MASTER_MODE_WIDTH 4                    // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_0_ADDR 1046 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_0_OFFSET 16                   // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_0_WIDTH 4                    // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_1_ADDR 1047 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_1_OFFSET 0                    // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_1_WIDTH 4                    // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_2_ADDR 1047 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_2_OFFSET 16                   // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_2_WIDTH 4                    // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_3_ADDR 1049 + PHY_BASE_ADDR // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_3_OFFSET 0                    // 
#define PHY_ADRCTL_SW_TXPWR_CTRL_3_WIDTH 4                    // 
#define PHY_AUTO_TIMING_MARGIN_CONTROL_ADDR 10                   // 
#define PHY_AUTO_TIMING_MARGIN_CONTROL_OFFSET 0                    // 
#define PHY_AUTO_TIMING_MARGIN_CONTROL_WIDTH 32                   // 
#define PHY_AUTO_TIMING_MARGIN_OBS_ADDR 11                   // 
#define PHY_AUTO_TIMING_MARGIN_OBS_OFFSET 0                    // 
#define PHY_AUTO_TIMING_MARGIN_OBS_WIDTH 28                   // 
#define PHY_BASE_ADDR                  4096                 // 
#define PHY_BYPASS_TWO_CYC_PREAMBLE_ADDR 2                    // 
#define PHY_BYPASS_TWO_CYC_PREAMBLE_OFFSET 24                   // 
#define PHY_BYPASS_TWO_CYC_PREAMBLE_WIDTH 2                    // 
#define PHY_BYTE_DISABLE_STATIC_TOG_DISABLE_ADDR 6 + PHY_AC_BASE_ADDR // 
#define PHY_BYTE_DISABLE_STATIC_TOG_DISABLE_OFFSET 8                    // 
#define PHY_BYTE_DISABLE_STATIC_TOG_DISABLE_WIDTH 1                    // 
#define PHY_BYTE_MODE_ADDR             37                   // 
#define PHY_BYTE_MODE_OFFSET           24                   // 
#define PHY_BYTE_MODE_WIDTH            1                    // 
#define PHY_CAL_CLEAR_0_ADDR           34 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_CLEAR_0_OFFSET         16                   // 
#define PHY_CAL_CLEAR_0_WIDTH          1                    // 
#define PHY_CAL_CLK_SELECT_0_ADDR      86 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_CLK_SELECT_0_OFFSET    0                    // 
#define PHY_CAL_CLK_SELECT_0_WIDTH     3                    // 
#define PHY_CAL_CPTR_CNT_0_ADDR        42 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_CPTR_CNT_0_OFFSET      24                   // 
#define PHY_CAL_CPTR_CNT_0_WIDTH       7                    // 
#define PHY_CAL_CPTR_CNT_WIDTH         7                    // 
#define PHY_CAL_DBG_CFG_0_ADDR         43 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_DBG_CFG_0_OFFSET       24                   // 
#define PHY_CAL_DBG_CFG_0_WIDTH        1                    // 
#define PHY_CAL_DBG_CFG_WIDTH          1                    // 
#define PHY_CAL_INTERVAL_COUNT_0_ADDR  35 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_INTERVAL_COUNT_0_OFFSET 0                    // 
#define PHY_CAL_INTERVAL_COUNT_0_WIDTH 32                   // 
#define PHY_CAL_MODE_0_ADDR            34 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_MODE_0_OFFSET          0                    // 
#define PHY_CAL_MODE_0_WIDTH           13                   // 
#define PHY_CAL_PD_FINE_ADJ_0_ADDR     43 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_PD_FINE_ADJ_0_OFFSET   8                    // 
#define PHY_CAL_PD_FINE_ADJ_0_WIDTH    8                    // 
#define PHY_CAL_PD_FINE_ADJ_WIDTH      8                    // 
#define PHY_CAL_PU_FINE_ADJ_0_ADDR     43 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_PU_FINE_ADJ_0_OFFSET   0                    // 
#define PHY_CAL_PU_FINE_ADJ_0_WIDTH    8                    // 
#define PHY_CAL_PU_FINE_ADJ_WIDTH      8                    // 
#define PHY_CAL_RANGE_PASS1_PD_MAX_DELTA_0_ADDR 50 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS1_PD_MAX_DELTA_0_OFFSET 0                    // 
#define PHY_CAL_RANGE_PASS1_PD_MAX_DELTA_0_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS1_PD_MAX_DELTA_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS1_PD_MIN_DELTA_0_ADDR 51 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS1_PD_MIN_DELTA_0_OFFSET 16                   // 
#define PHY_CAL_RANGE_PASS1_PD_MIN_DELTA_0_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS1_PD_MIN_DELTA_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS1_PU_MAX_DELTA_0_ADDR 49 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS1_PU_MAX_DELTA_0_OFFSET 24                   // 
#define PHY_CAL_RANGE_PASS1_PU_MAX_DELTA_0_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS1_PU_MAX_DELTA_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS1_PU_MIN_DELTA_0_ADDR 51 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS1_PU_MIN_DELTA_0_OFFSET 8                    // 
#define PHY_CAL_RANGE_PASS1_PU_MIN_DELTA_0_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS1_PU_MIN_DELTA_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS1_RX_MAX_DELTA_0_ADDR 50 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS1_RX_MAX_DELTA_0_OFFSET 8                    // 
#define PHY_CAL_RANGE_PASS1_RX_MAX_DELTA_0_WIDTH 5                    // 
#define PHY_CAL_RANGE_PASS1_RX_MAX_DELTA_WIDTH 5                    // 
#define PHY_CAL_RANGE_PASS1_RX_MIN_DELTA_0_ADDR 51 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS1_RX_MIN_DELTA_0_OFFSET 24                   // 
#define PHY_CAL_RANGE_PASS1_RX_MIN_DELTA_0_WIDTH 5                    // 
#define PHY_CAL_RANGE_PASS1_RX_MIN_DELTA_WIDTH 5                    // 
#define PHY_CAL_RANGE_PASS2_PD_MAX_DELTA_0_ADDR 50 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS2_PD_MAX_DELTA_0_OFFSET 24                   // 
#define PHY_CAL_RANGE_PASS2_PD_MAX_DELTA_0_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS2_PD_MAX_DELTA_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS2_PD_MIN_DELTA_0_ADDR 52 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS2_PD_MIN_DELTA_0_OFFSET 8                    // 
#define PHY_CAL_RANGE_PASS2_PD_MIN_DELTA_0_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS2_PD_MIN_DELTA_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS2_PU_MAX_DELTA_0_ADDR 50 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS2_PU_MAX_DELTA_0_OFFSET 16                   // 
#define PHY_CAL_RANGE_PASS2_PU_MAX_DELTA_0_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS2_PU_MAX_DELTA_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS2_PU_MIN_DELTA_0_ADDR 52 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS2_PU_MIN_DELTA_0_OFFSET 0                    // 
#define PHY_CAL_RANGE_PASS2_PU_MIN_DELTA_0_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS2_PU_MIN_DELTA_WIDTH 6                    // 
#define PHY_CAL_RANGE_PASS2_RX_MAX_DELTA_0_ADDR 51 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS2_RX_MAX_DELTA_0_OFFSET 0                    // 
#define PHY_CAL_RANGE_PASS2_RX_MAX_DELTA_0_WIDTH 5                    // 
#define PHY_CAL_RANGE_PASS2_RX_MAX_DELTA_WIDTH 5                    // 
#define PHY_CAL_RANGE_PASS2_RX_MIN_DELTA_0_ADDR 52 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RANGE_PASS2_RX_MIN_DELTA_0_OFFSET 16                   // 
#define PHY_CAL_RANGE_PASS2_RX_MIN_DELTA_0_WIDTH 5                    // 
#define PHY_CAL_RANGE_PASS2_RX_MIN_DELTA_WIDTH 5                    // 
#define PHY_CAL_RCV_FINE_ADJ_0_ADDR    43 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RCV_FINE_ADJ_0_OFFSET  16                   // 
#define PHY_CAL_RCV_FINE_ADJ_0_WIDTH   8                    // 
#define PHY_CAL_RCV_FINE_ADJ_WIDTH     8                    // 
#define PHY_CAL_RESULT2_OBS_0_ADDR     38 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RESULT2_OBS_0_OFFSET   0                    // 
#define PHY_CAL_RESULT2_OBS_0_WIDTH    24                   // 
#define PHY_CAL_RESULT3_OBS_0_ADDR     45 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RESULT3_OBS_0_OFFSET   0                    // 
#define PHY_CAL_RESULT3_OBS_0_WIDTH    32                   // 
#define PHY_CAL_RESULT4_OBS_0_ADDR     39 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RESULT4_OBS_0_OFFSET   0                    // 
#define PHY_CAL_RESULT4_OBS_0_WIDTH    24                   // 
#define PHY_CAL_RESULT5_OBS_0_ADDR     40 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RESULT5_OBS_0_OFFSET   0                    // 
#define PHY_CAL_RESULT5_OBS_0_WIDTH    24                   // 
#define PHY_CAL_RESULT6_OBS_0_ADDR     41 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RESULT6_OBS_0_OFFSET   0                    // 
#define PHY_CAL_RESULT6_OBS_0_WIDTH    24                   // 
#define PHY_CAL_RESULT7_OBS_0_ADDR     42 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RESULT7_OBS_0_OFFSET   0                    // 
#define PHY_CAL_RESULT7_OBS_0_WIDTH    24                   // 
#define PHY_CAL_RESULT_OBS_0_ADDR      37 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_RESULT_OBS_0_OFFSET    0                    // 
#define PHY_CAL_RESULT_OBS_0_WIDTH     24                   // 
#define PHY_CAL_SAMPLE_WAIT_0_ADDR     36 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_SAMPLE_WAIT_0_OFFSET   0                    // 
#define PHY_CAL_SAMPLE_WAIT_0_WIDTH    8                    // 
#define PHY_CAL_SETTLING_PRD_0_ADDR    86 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_SETTLING_PRD_0_OFFSET  24                   // 
#define PHY_CAL_SETTLING_PRD_0_WIDTH   7                    // 
#define PHY_CAL_SETTLING_PRD_WIDTH     7                    // 
#define PHY_CAL_SLOPE_ADJ_0_ADDR       46 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_SLOPE_ADJ_0_OFFSET     8                    // 
#define PHY_CAL_SLOPE_ADJ_0_WIDTH      20                   // 
#define PHY_CAL_SLOPE_ADJ_PASS2_0_ADDR 47 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_SLOPE_ADJ_PASS2_0_OFFSET 0                    // 
#define PHY_CAL_SLOPE_ADJ_PASS2_0_WIDTH 20                   // 
#define PHY_CAL_SLOPE_ADJ_PASS2_WIDTH  20                   // 
#define PHY_CAL_SLOPE_ADJ_WIDTH        20                   // 
#define PHY_CAL_START_0_ADDR           34 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_START_0_OFFSET         24                   // 
#define PHY_CAL_START_0_WIDTH          1                    // 
#define PHY_CAL_SW_CAL_CFG_0_ADDR      49 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_SW_CAL_CFG_0_OFFSET    0                    // 
#define PHY_CAL_SW_CAL_CFG_0_WIDTH     23                   // 
#define PHY_CAL_SW_CAL_CFG_WIDTH       23                   // 
#define PHY_CAL_TWO_PASS_CFG_0_ADDR    48 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_TWO_PASS_CFG_0_OFFSET  0                    // 
#define PHY_CAL_TWO_PASS_CFG_0_WIDTH   25                   // 
#define PHY_CAL_TWO_PASS_CFG_WIDTH     25                   // 
#define PHY_CAL_VREF_SWITCH_TIMER_0_ADDR 86 + PHY_AC_BASE_ADDR // 
#define PHY_CAL_VREF_SWITCH_TIMER_0_OFFSET 8                    // 
#define PHY_CAL_VREF_SWITCH_TIMER_0_WIDTH 16                   // 
#define PHY_CAL_VREF_SWITCH_TIMER_WIDTH 16                   // 
#define PHY_CALVL_BEST_THRSHLD_ADDR    1025 + PHY_BASE_ADDR // 
#define PHY_CALVL_BEST_THRSHLD_OFFSET  0                    // 
#define PHY_CALVL_BEST_THRSHLD_WIDTH   3                    // 
#define PHY_CALVL_PER_VREF_THRSHLD_ADDR 1025 + PHY_BASE_ADDR // 
#define PHY_CALVL_PER_VREF_THRSHLD_OFFSET 8                    // 
#define PHY_CALVL_PER_VREF_THRSHLD_WIDTH 6                    // 
#define PHY_CALVL_VREF_DRIVING_SLICE_ADDR 37                   // 
#define PHY_CALVL_VREF_DRIVING_SLICE_OFFSET 16                   // 
#define PHY_CALVL_VREF_DRIVING_SLICE_WIDTH 1                    // 
#define PHY_CLK_BYPASS_OVERRIDE_ADDR   3                    // 
#define PHY_CLK_BYPASS_OVERRIDE_OFFSET 0                    // 
#define PHY_CLK_BYPASS_OVERRIDE_WIDTH  1                    // 
#define PHY_CLK_DC_ADJUST_0_ADDR       1070 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_ADJUST_0_OFFSET     16                   // 
#define PHY_CLK_DC_ADJUST_0_WIDTH      8                    // 
#define PHY_CLK_DC_ADJUST_DIRECT_ADDR  1069 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_ADJUST_DIRECT_OFFSET 24                   // 
#define PHY_CLK_DC_ADJUST_DIRECT_WIDTH 1                    // 
#define PHY_CLK_DC_ADJUST_SAMPLE_CNT_ADDR 1069 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_ADJUST_SAMPLE_CNT_OFFSET 8                    // 
#define PHY_CLK_DC_ADJUST_SAMPLE_CNT_WIDTH 8                    // 
#define PHY_CLK_DC_ADJUST_START_ADDR   1069 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_ADJUST_START_OFFSET 0                    // 
#define PHY_CLK_DC_ADJUST_START_WIDTH  6                    // 
#define PHY_CLK_DC_ADJUST_THRSHLD_ADDR 1069 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_ADJUST_THRSHLD_OFFSET 16                   // 
#define PHY_CLK_DC_ADJUST_THRSHLD_WIDTH 8                    // 
#define PHY_CLK_DC_CAL_CLK_SEL_ADDR    1090 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_CAL_CLK_SEL_OFFSET  8                    // 
#define PHY_CLK_DC_CAL_CLK_SEL_WIDTH   3                    // 
#define PHY_CLK_DC_CAL_POLARITY_ADDR   1070 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_CAL_POLARITY_OFFSET 0                    // 
#define PHY_CLK_DC_CAL_POLARITY_WIDTH  1                    // 
#define PHY_CLK_DC_CAL_SAMPLE_WAIT_ADDR 1068 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_CAL_SAMPLE_WAIT_OFFSET 0                    // 
#define PHY_CLK_DC_CAL_SAMPLE_WAIT_WIDTH 8                    // 
#define PHY_CLK_DC_CAL_START_ADDR      1070 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_CAL_START_OFFSET    8                    // 
#define PHY_CLK_DC_CAL_START_WIDTH     1                    // 
#define PHY_CLK_DC_CAL_TIMEOUT_ADDR    1068 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_CAL_TIMEOUT_OFFSET  8                    // 
#define PHY_CLK_DC_CAL_TIMEOUT_WIDTH   8                    // 
#define PHY_CLK_DC_DM_THRSHLD_ADDR     1090 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_DM_THRSHLD_OFFSET   0                    // 
#define PHY_CLK_DC_DM_THRSHLD_WIDTH    8                    // 
#define PHY_CLK_DC_FREQ_CHG_ADJ_ADDR   1068 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_FREQ_CHG_ADJ_OFFSET 24                   // 
#define PHY_CLK_DC_FREQ_CHG_ADJ_WIDTH  1                    // 
#define PHY_CLK_DC_INIT_DISABLE_ADDR   1070 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_INIT_DISABLE_OFFSET 24                   // 
#define PHY_CLK_DC_INIT_DISABLE_WIDTH  1                    // 
#define PHY_CLK_DC_WEIGHT_ADDR         1068 + PHY_BASE_ADDR // 
#define PHY_CLK_DC_WEIGHT_OFFSET       16                   // 
#define PHY_CLK_DC_WEIGHT_WIDTH        2                    // 
#define PHY_CLK_SWITCH_OBS_ADDR        8 + PHY_AC_BASE_ADDR // 
#define PHY_CLK_SWITCH_OBS_OFFSET      0                    // 
#define PHY_CLK_SWITCH_OBS_WIDTH       32                   // 
#define PHY_CLK_WR_BYPASS_SLAVE_DELAY_ADDR 0                    // 
#define PHY_CLK_WR_BYPASS_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_CLK_WR_BYPASS_SLAVE_DELAY_WIDTH 10                   // 
#define PHY_CLK_WRDM_SLAVE_DELAY_0_ADDR 113 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDM_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_CLK_WRDM_SLAVE_DELAY_0_WIDTH 11                   // 
#define PHY_CLK_WRDM_SLAVE_DELAY_1_ADDR 369 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDM_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_CLK_WRDM_SLAVE_DELAY_1_WIDTH 11                   // 
#define PHY_CLK_WRDM_SLAVE_DELAY_2_ADDR 625 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDM_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_CLK_WRDM_SLAVE_DELAY_2_WIDTH 11                   // 
#define PHY_CLK_WRDM_SLAVE_DELAY_3_ADDR 881 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDM_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_CLK_WRDM_SLAVE_DELAY_3_WIDTH 11                   // 
#define PHY_CLK_WRDM_SLAVE_DELAY_ADDR  113                  // 
#define PHY_CLK_WRDM_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_CLK_WRDM_SLAVE_DELAY_WIDTH 11                   // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_0_ADDR 109 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_0_WIDTH 11                   // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_1_ADDR 365 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_1_WIDTH 11                   // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_2_ADDR 621 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_2_WIDTH 11                   // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_3_ADDR 877 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_3_WIDTH 11                   // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_ADDR 109                  // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_CLK_WRDQ0_SLAVE_DELAY_WIDTH 11                   // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_0_ADDR 109 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_0_WIDTH 11                   // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_1_ADDR 365 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_1_WIDTH 11                   // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_2_ADDR 621 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_2_WIDTH 11                   // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_3_ADDR 877 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_3_WIDTH 11                   // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_ADDR 109                  // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_CLK_WRDQ1_SLAVE_DELAY_WIDTH 11                   // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_0_ADDR 110 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_0_WIDTH 11                   // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_1_ADDR 366 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_1_WIDTH 11                   // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_2_ADDR 622 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_2_WIDTH 11                   // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_3_ADDR 878 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_3_WIDTH 11                   // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_ADDR 110                  // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_CLK_WRDQ2_SLAVE_DELAY_WIDTH 11                   // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_0_ADDR 110 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_0_WIDTH 11                   // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_1_ADDR 366 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_1_WIDTH 11                   // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_2_ADDR 622 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_2_WIDTH 11                   // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_3_ADDR 878 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_3_WIDTH 11                   // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_ADDR 110                  // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_CLK_WRDQ3_SLAVE_DELAY_WIDTH 11                   // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_0_ADDR 111 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_0_WIDTH 11                   // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_1_ADDR 367 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_1_WIDTH 11                   // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_2_ADDR 623 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_2_WIDTH 11                   // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_3_ADDR 879 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_3_WIDTH 11                   // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_ADDR 111                  // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_CLK_WRDQ4_SLAVE_DELAY_WIDTH 11                   // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_0_ADDR 111 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_0_WIDTH 11                   // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_1_ADDR 367 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_1_WIDTH 11                   // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_2_ADDR 623 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_2_WIDTH 11                   // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_3_ADDR 879 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_3_WIDTH 11                   // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_ADDR 111                  // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_CLK_WRDQ5_SLAVE_DELAY_WIDTH 11                   // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_0_ADDR 112 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_0_WIDTH 11                   // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_1_ADDR 368 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_1_WIDTH 11                   // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_2_ADDR 624 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_2_WIDTH 11                   // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_3_ADDR 880 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_3_WIDTH 11                   // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_ADDR 112                  // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_CLK_WRDQ6_SLAVE_DELAY_WIDTH 11                   // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_0_ADDR 112 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_0_WIDTH 11                   // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_1_ADDR 368 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_1_WIDTH 11                   // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_2_ADDR 624 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_2_WIDTH 11                   // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_3_ADDR 880 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_3_WIDTH 11                   // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_ADDR 112                  // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_CLK_WRDQ7_SLAVE_DELAY_WIDTH 11                   // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_0_ADDR 113 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_1_ADDR 369 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_2_ADDR 625 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_3_ADDR 881 + PHY_BASE_ADDR // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_ADDR 113                  // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_BYPASS_ADDR 1                    // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_BYPASS_OFFSET 8                    // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_BYPASS_WIDTH 9                    // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_CLK_WRDQS_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_CONTINUOUS_CLK_CAL_UPDATE_ADDR 5 + PHY_AC_BASE_ADDR // 
#define PHY_CONTINUOUS_CLK_CAL_UPDATE_OFFSET 16                   // 
#define PHY_CONTINUOUS_CLK_CAL_UPDATE_WIDTH 1                    // 
#define PHY_CS_ACS_ALLOCATION_BIT0_2_ADDR 1047 + PHY_BASE_ADDR // 
#define PHY_CS_ACS_ALLOCATION_BIT0_2_OFFSET 24                   // 
#define PHY_CS_ACS_ALLOCATION_BIT0_2_WIDTH 4                    // 
#define PHY_CS_ACS_ALLOCATION_BIT0_3_ADDR 1049 + PHY_BASE_ADDR // 
#define PHY_CS_ACS_ALLOCATION_BIT0_3_OFFSET 8                    // 
#define PHY_CS_ACS_ALLOCATION_BIT0_3_WIDTH 4                    // 
#define PHY_CS_ACS_ALLOCATION_BIT1_2_ADDR 1048 + PHY_BASE_ADDR // 
#define PHY_CS_ACS_ALLOCATION_BIT1_2_OFFSET 0                    // 
#define PHY_CS_ACS_ALLOCATION_BIT1_2_WIDTH 4                    // 
#define PHY_CS_ACS_ALLOCATION_BIT1_3_ADDR 1049 + PHY_BASE_ADDR // 
#define PHY_CS_ACS_ALLOCATION_BIT1_3_OFFSET 16                   // 
#define PHY_CS_ACS_ALLOCATION_BIT1_3_WIDTH 4                    // 
#define PHY_CS_ACS_ALLOCATION_BIT2_2_ADDR 1048 + PHY_BASE_ADDR // 
#define PHY_CS_ACS_ALLOCATION_BIT2_2_OFFSET 8                    // 
#define PHY_CS_ACS_ALLOCATION_BIT2_2_WIDTH 4                    // 
#define PHY_CS_ACS_ALLOCATION_BIT2_3_ADDR 1049 + PHY_BASE_ADDR // 
#define PHY_CS_ACS_ALLOCATION_BIT2_3_OFFSET 24                   // 
#define PHY_CS_ACS_ALLOCATION_BIT2_3_WIDTH 4                    // 
#define PHY_CS_ACS_ALLOCATION_BIT3_2_ADDR 1048 + PHY_BASE_ADDR // 
#define PHY_CS_ACS_ALLOCATION_BIT3_2_OFFSET 16                   // 
#define PHY_CS_ACS_ALLOCATION_BIT3_2_WIDTH 4                    // 
#define PHY_CS_ACS_ALLOCATION_BIT3_3_ADDR 1050 + PHY_BASE_ADDR // 
#define PHY_CS_ACS_ALLOCATION_BIT3_3_OFFSET 0                    // 
#define PHY_CS_ACS_ALLOCATION_BIT3_3_WIDTH 4                    // 
#define PHY_CSLVL_CAPTURE_CNT_ADDR     1091 + PHY_BASE_ADDR // 
#define PHY_CSLVL_CAPTURE_CNT_OFFSET   24                   // 
#define PHY_CSLVL_CAPTURE_CNT_WIDTH    4                    // 
#define PHY_CSLVL_COARSE_CAPTURE_CNT_ADDR 1065 + PHY_BASE_ADDR // 
#define PHY_CSLVL_COARSE_CAPTURE_CNT_OFFSET 0                    // 
#define PHY_CSLVL_COARSE_CAPTURE_CNT_WIDTH 4                    // 
#define PHY_CSLVL_COARSE_CHK_ADDR      1065 + PHY_BASE_ADDR // 
#define PHY_CSLVL_COARSE_CHK_OFFSET    8                    // 
#define PHY_CSLVL_COARSE_CHK_WIDTH     10                   // 
#define PHY_CSLVL_COARSE_DLY_ADDR      1059 + PHY_BASE_ADDR // 
#define PHY_CSLVL_COARSE_DLY_OFFSET    0                    // 
#define PHY_CSLVL_COARSE_DLY_WIDTH     10                   // 
#define PHY_CSLVL_CS_MAP_ADDR          1066 + PHY_BASE_ADDR // 
#define PHY_CSLVL_CS_MAP_OFFSET        0                    // 
#define PHY_CSLVL_CS_MAP_WIDTH         4                    // 
#define PHY_CSLVL_DEBUG_MODE_ADDR      1059 + PHY_BASE_ADDR // 
#define PHY_CSLVL_DEBUG_MODE_OFFSET    16                   // 
#define PHY_CSLVL_DEBUG_MODE_WIDTH     1                    // 
#define PHY_CSLVL_DLY_STEP_ADDR        1090 + PHY_BASE_ADDR // 
#define PHY_CSLVL_DLY_STEP_OFFSET      16                   // 
#define PHY_CSLVL_DLY_STEP_WIDTH       4                    // 
#define PHY_CSLVL_ENABLE_ADDR          1064 + PHY_BASE_ADDR // 
#define PHY_CSLVL_ENABLE_OFFSET        0                    // 
#define PHY_CSLVL_ENABLE_WIDTH         1                    // 
#define PHY_CSLVL_FAST_EN_ADDR         1064 + PHY_BASE_ADDR // 
#define PHY_CSLVL_FAST_EN_OFFSET       8                    // 
#define PHY_CSLVL_FAST_EN_WIDTH        1                    // 
#define PHY_CSLVL_OBS0_ADDR            1061 + PHY_BASE_ADDR // 
#define PHY_CSLVL_OBS0_OFFSET          0                    // 
#define PHY_CSLVL_OBS0_WIDTH           32                   // 
#define PHY_CSLVL_OBS1_ADDR            1062 + PHY_BASE_ADDR // 
#define PHY_CSLVL_OBS1_OFFSET          0                    // 
#define PHY_CSLVL_OBS1_WIDTH           32                   // 
#define PHY_CSLVL_OBS2_ADDR            1063 + PHY_BASE_ADDR // 
#define PHY_CSLVL_OBS2_OFFSET          0                    // 
#define PHY_CSLVL_OBS2_WIDTH           32                   // 
#define PHY_CSLVL_PERIODIC_START_OFFSET_ADDR 1065 + PHY_BASE_ADDR // 
#define PHY_CSLVL_PERIODIC_START_OFFSET_OFFSET 24                   // 
#define PHY_CSLVL_PERIODIC_START_OFFSET_WIDTH 8                    // 
#define PHY_CSLVL_QTR_ADDR             1064 + PHY_BASE_ADDR // 
#define PHY_CSLVL_QTR_OFFSET           16                   // 
#define PHY_CSLVL_QTR_WIDTH            10                   // 
#define PHY_CSLVL_START_ADDR           1058 + PHY_BASE_ADDR // 
#define PHY_CSLVL_START_OFFSET         16                   // 
#define PHY_CSLVL_START_WIDTH          10                   // 
#define PHY_CTRL_LPBK_EN_ADDR          8                    // 
#define PHY_CTRL_LPBK_EN_OFFSET        16                   // 
#define PHY_CTRL_LPBK_EN_WIDTH         2                    // 
#define PHY_DATA_BYTE_ORDER_SEL_ADDR   54 + PHY_AC_BASE_ADDR // 
#define PHY_DATA_BYTE_ORDER_SEL_HIGH_ADDR 55 + PHY_AC_BASE_ADDR // 
#define PHY_DATA_BYTE_ORDER_SEL_HIGH_OFFSET 0                    // 
#define PHY_DATA_BYTE_ORDER_SEL_HIGH_WIDTH 8                    // 
#define PHY_DATA_BYTE_ORDER_SEL_OFFSET 0                    // 
#define PHY_DATA_BYTE_ORDER_SEL_WIDTH  32                   // 
#define PHY_DATA_DC_ADJUST_DIRECT_ADDR 70                   // 
#define PHY_DATA_DC_ADJUST_DIRECT_OFFSET 8                    // 
#define PHY_DATA_DC_ADJUST_DIRECT_WIDTH 1                    // 
#define PHY_DATA_DC_ADJUST_SAMPLE_CNT_ADDR 69                   // 
#define PHY_DATA_DC_ADJUST_SAMPLE_CNT_OFFSET 24                   // 
#define PHY_DATA_DC_ADJUST_SAMPLE_CNT_WIDTH 8                    // 
#define PHY_DATA_DC_ADJUST_START_ADDR  69                   // 
#define PHY_DATA_DC_ADJUST_START_OFFSET 16                   // 
#define PHY_DATA_DC_ADJUST_START_WIDTH 6                    // 
#define PHY_DATA_DC_ADJUST_THRSHLD_ADDR 70                   // 
#define PHY_DATA_DC_ADJUST_THRSHLD_OFFSET 0                    // 
#define PHY_DATA_DC_ADJUST_THRSHLD_WIDTH 8                    // 
#define PHY_DATA_DC_CAL_CLK_SEL_ADDR   87                   // 
#define PHY_DATA_DC_CAL_CLK_SEL_OFFSET 24                   // 
#define PHY_DATA_DC_CAL_CLK_SEL_WIDTH  3                    // 
#define PHY_DATA_DC_CAL_POLARITY_ADDR  70                   // 
#define PHY_DATA_DC_CAL_POLARITY_OFFSET 16                   // 
#define PHY_DATA_DC_CAL_POLARITY_WIDTH 1                    // 
#define PHY_DATA_DC_CAL_SAMPLE_WAIT_ADDR 68                   // 
#define PHY_DATA_DC_CAL_SAMPLE_WAIT_OFFSET 24                   // 
#define PHY_DATA_DC_CAL_SAMPLE_WAIT_WIDTH 8                    // 
#define PHY_DATA_DC_CAL_START_ADDR     70                   // 
#define PHY_DATA_DC_CAL_START_OFFSET   24                   // 
#define PHY_DATA_DC_CAL_START_WIDTH    1                    // 
#define PHY_DATA_DC_CAL_TIMEOUT_ADDR   69                   // 
#define PHY_DATA_DC_CAL_TIMEOUT_OFFSET 0                    // 
#define PHY_DATA_DC_CAL_TIMEOUT_WIDTH  8                    // 
#define PHY_DATA_DC_DM_CLK_ADJUST_ADDR 130                  // 
#define PHY_DATA_DC_DM_CLK_ADJUST_OFFSET 8                    // 
#define PHY_DATA_DC_DM_CLK_ADJUST_WIDTH 8                    // 
#define PHY_DATA_DC_DM_CLK_DIFF_THRSHLD_ADDR 105                  // 
#define PHY_DATA_DC_DM_CLK_DIFF_THRSHLD_OFFSET 24                   // 
#define PHY_DATA_DC_DM_CLK_DIFF_THRSHLD_WIDTH 8                    // 
#define PHY_DATA_DC_DM_CLK_SE_THRSHLD_ADDR 105                  // 
#define PHY_DATA_DC_DM_CLK_SE_THRSHLD_OFFSET 16                   // 
#define PHY_DATA_DC_DM_CLK_SE_THRSHLD_WIDTH 8                    // 
#define PHY_DATA_DC_DQ0_CLK_ADJUST_ADDR 128                  // 
#define PHY_DATA_DC_DQ0_CLK_ADJUST_OFFSET 8                    // 
#define PHY_DATA_DC_DQ0_CLK_ADJUST_WIDTH 8                    // 
#define PHY_DATA_DC_DQ1_CLK_ADJUST_ADDR 128                  // 
#define PHY_DATA_DC_DQ1_CLK_ADJUST_OFFSET 16                   // 
#define PHY_DATA_DC_DQ1_CLK_ADJUST_WIDTH 8                    // 
#define PHY_DATA_DC_DQ2_CLK_ADJUST_ADDR 128                  // 
#define PHY_DATA_DC_DQ2_CLK_ADJUST_OFFSET 24                   // 
#define PHY_DATA_DC_DQ2_CLK_ADJUST_WIDTH 8                    // 
#define PHY_DATA_DC_DQ3_CLK_ADJUST_ADDR 129                  // 
#define PHY_DATA_DC_DQ3_CLK_ADJUST_OFFSET 0                    // 
#define PHY_DATA_DC_DQ3_CLK_ADJUST_WIDTH 8                    // 
#define PHY_DATA_DC_DQ4_CLK_ADJUST_ADDR 129                  // 
#define PHY_DATA_DC_DQ4_CLK_ADJUST_OFFSET 8                    // 
#define PHY_DATA_DC_DQ4_CLK_ADJUST_WIDTH 8                    // 
#define PHY_DATA_DC_DQ5_CLK_ADJUST_ADDR 129                  // 
#define PHY_DATA_DC_DQ5_CLK_ADJUST_OFFSET 16                   // 
#define PHY_DATA_DC_DQ5_CLK_ADJUST_WIDTH 8                    // 
#define PHY_DATA_DC_DQ6_CLK_ADJUST_ADDR 129                  // 
#define PHY_DATA_DC_DQ6_CLK_ADJUST_OFFSET 24                   // 
#define PHY_DATA_DC_DQ6_CLK_ADJUST_WIDTH 8                    // 
#define PHY_DATA_DC_DQ7_CLK_ADJUST_ADDR 130                  // 
#define PHY_DATA_DC_DQ7_CLK_ADJUST_OFFSET 0                    // 
#define PHY_DATA_DC_DQ7_CLK_ADJUST_WIDTH 8                    // 
#define PHY_DATA_DC_DQ_INIT_SLV_DELAY_ADDR 104                  // 
#define PHY_DATA_DC_DQ_INIT_SLV_DELAY_OFFSET 16                   // 
#define PHY_DATA_DC_DQ_INIT_SLV_DELAY_WIDTH 10                   // 
#define PHY_DATA_DC_DQS_CLK_ADJUST_ADDR 128                  // 
#define PHY_DATA_DC_DQS_CLK_ADJUST_OFFSET 0                    // 
#define PHY_DATA_DC_DQS_CLK_ADJUST_WIDTH 8                    // 
#define PHY_DATA_DC_DQS_INIT_SLV_DELAY_ADDR 104                  // 
#define PHY_DATA_DC_DQS_INIT_SLV_DELAY_OFFSET 0                    // 
#define PHY_DATA_DC_DQS_INIT_SLV_DELAY_WIDTH 9                    // 
#define PHY_DATA_DC_INIT_DISABLE_ADDR  103                  // 
#define PHY_DATA_DC_INIT_DISABLE_OFFSET 16                   // 
#define PHY_DATA_DC_INIT_DISABLE_WIDTH 2                    // 
#define PHY_DATA_DC_SW_RANK_ADDR       71                   // 
#define PHY_DATA_DC_SW_RANK_OFFSET     0                    // 
#define PHY_DATA_DC_SW_RANK_WIDTH      2                    // 
#define PHY_DATA_DC_WDQLVL_ENABLE_0_ADDR 105 + PHY_BASE_ADDR // 
#define PHY_DATA_DC_WDQLVL_ENABLE_0_OFFSET 8                    // 
#define PHY_DATA_DC_WDQLVL_ENABLE_0_WIDTH 1                    // 
#define PHY_DATA_DC_WDQLVL_ENABLE_1_ADDR 361 + PHY_BASE_ADDR // 
#define PHY_DATA_DC_WDQLVL_ENABLE_1_OFFSET 8                    // 
#define PHY_DATA_DC_WDQLVL_ENABLE_1_WIDTH 1                    // 
#define PHY_DATA_DC_WDQLVL_ENABLE_ADDR 105                  // 
#define PHY_DATA_DC_WDQLVL_ENABLE_OFFSET 8                    // 
#define PHY_DATA_DC_WDQLVL_ENABLE_WIDTH 1                    // 
#define PHY_DATA_DC_WEIGHT_ADDR        69                   // 
#define PHY_DATA_DC_WEIGHT_OFFSET      8                    // 
#define PHY_DATA_DC_WEIGHT_WIDTH       2                    // 
#define PHY_DATA_DC_WRLVL_ENABLE_ADDR  105                  // 
#define PHY_DATA_DC_WRLVL_ENABLE_OFFSET 0                    // 
#define PHY_DATA_DC_WRLVL_ENABLE_WIDTH 1                    // 
#define PHY_DBI_MODE_ADDR              93                   // 
#define PHY_DBI_MODE_OFFSET            8                    // 
#define PHY_DBI_MODE_WIDTH             1                    // 
#define PHY_DCC_ADJUST_WIDTH           6                    // 
#define PHY_DCC_RXCAL_CTRL_GATE_DISABLE_ADDR 72                   // 
#define PHY_DCC_RXCAL_CTRL_GATE_DISABLE_OFFSET 0                    // 
#define PHY_DCC_RXCAL_CTRL_GATE_DISABLE_WIDTH 1                    // 
#define PHY_DDL_AC_ENABLE_ADDR         1056 + PHY_BASE_ADDR // 
#define PHY_DDL_AC_ENABLE_OFFSET       0                    // 
#define PHY_DDL_AC_ENABLE_WIDTH        4                    // 
#define PHY_DDL_AC_MASK_ADDR           1058 + PHY_BASE_ADDR // 
#define PHY_DDL_AC_MASK_OFFSET         0                    // 
#define PHY_DDL_AC_MASK_WIDTH          6                    // 
#define PHY_DDL_AC_MODE_ADDR           1057 + PHY_BASE_ADDR // 
#define PHY_DDL_AC_MODE_OFFSET         0                    // 
#define PHY_DDL_AC_MODE_WIDTH          27                   // 
#define PHY_DDL_AC_TEST_OBS_WIDTH      32                   // 
#define PHY_DDL_AC_TRACK_UPD_THRESHOLD_ADDR 1058 + PHY_BASE_ADDR // 
#define PHY_DDL_AC_TRACK_UPD_THRESHOLD_OFFSET 8                    // 
#define PHY_DDL_AC_TRACK_UPD_THRESHOLD_WIDTH 8                    // 
#define PHY_DDL_MASK_ADDR              58                   // 
#define PHY_DDL_MASK_OFFSET            0                    // 
#define PHY_DDL_MASK_WIDTH             6                    // 
#define PHY_DDL_MODE_0_ADDR            57 + PHY_BASE_ADDR  // 
#define PHY_DDL_MODE_0_OFFSET          0                    // 
#define PHY_DDL_MODE_1_ADDR            313 + PHY_BASE_ADDR // 
#define PHY_DDL_MODE_1_OFFSET          0                    // 
#define PHY_DDL_MODE_2_ADDR            569 + PHY_BASE_ADDR // 
#define PHY_DDL_MODE_2_OFFSET          0                    // 
#define PHY_DDL_MODE_3_ADDR            825 + PHY_BASE_ADDR // 
#define PHY_DDL_MODE_3_OFFSET          0                    // 
#define PHY_DDL_MODE_ADDR              57                   // 
#define PHY_DDL_MODE_OFFSET            0                    // 
#define PHY_DDL_MODE_WIDTH             31                   // 
#define PHY_DDL_TEST_MSTR_DLY_OBS_0_ADDR 60 + PHY_BASE_ADDR  // 
#define PHY_DDL_TEST_MSTR_DLY_OBS_1_ADDR 316 + PHY_BASE_ADDR // 
#define PHY_DDL_TEST_MSTR_DLY_OBS_2_ADDR 572 + PHY_BASE_ADDR // 
#define PHY_DDL_TEST_MSTR_DLY_OBS_3_ADDR 828 + PHY_BASE_ADDR // 
#define PHY_DDL_TEST_MSTR_DLY_OBS_ADDR 60                   // 
#define PHY_DDL_TEST_MSTR_DLY_OBS_OFFSET 0                    // 
#define PHY_DDL_TEST_MSTR_DLY_OBS_WIDTH 32                   // 
#define PHY_DDL_TEST_OBS_0_ADDR        59 + PHY_BASE_ADDR  // 
#define PHY_DDL_TEST_OBS_1_ADDR        315 + PHY_BASE_ADDR // 
#define PHY_DDL_TEST_OBS_2_ADDR        571 + PHY_BASE_ADDR // 
#define PHY_DDL_TEST_OBS_3_ADDR        827 + PHY_BASE_ADDR // 
#define PHY_DDL_TEST_OBS_ADDR          59                   // 
#define PHY_DDL_TEST_OBS_OFFSET        0                    // 
#define PHY_DDL_TEST_OBS_WIDTH         32                   // 
#define PHY_DDL_TRACK_UPD_THRESHOLD_ADDR 61                   // 
#define PHY_DDL_TRACK_UPD_THRESHOLD_OFFSET 0                    // 
#define PHY_DDL_TRACK_UPD_THRESHOLD_WIDTH 8                    // 
#define PHY_DESKEW_PLL_COUNT           2                    // 
#define PHY_DFI40_POLARITY_ADDR        19                   // 
#define PHY_DFI40_POLARITY_OFFSET      16                   // 
#define PHY_DFI40_POLARITY_WIDTH       1                    // 
#define PHY_DFI_CTRLMSG_DATA_WIDTH     16                   // 
#define PHY_DFI_CTRLMSG_WIDTH          8                    // 
#define PHY_DFI_DATA_BYTE_WIDTH        4                    // 
#define PHY_DFI_DATA_SLICE_WIDTH       4                    // 
#define PHY_DFI_DATA_WIDTH             64                   // 
#define PHY_DFI_DBI_WIDTH              8                    // 
#define PHY_DFI_PHYUPD_TYPE_ADDR       4 + PHY_AC_BASE_ADDR // 
#define PHY_DFI_PHYUPD_TYPE_OFFSET     24                   // 
#define PHY_DFI_PHYUPD_TYPE_WIDTH      2                    // 
#define PHY_DFI_RDLVL_SLICE_WIDTH      4                    // 
#define PHY_DFI_WDQ_OSC_CODE_WIDTH     4                    // 
#define PHY_DFI_WDQLVL_RESULT_WIDTH    8                    // 
#define PHY_DFI_WDQLVL_SLICE_WIDTH     4                    // 
#define PHY_DFI_WRDATA_MASK_WIDTH      8                    // 
#define PHY_DFI_WRLVL_RESP_WIDTH       4                    // 
#define PHY_DFI_WRLVL_SLICE_WIDTH      4                    // 
#define PHY_DLL_RST_EN_ADDR            62 + PHY_AC_BASE_ADDR // 
#define PHY_DLL_RST_EN_OFFSET          0                    // 
#define PHY_DLL_RST_EN_WIDTH           2                    // 
#define PHY_DQ_DM_SWIZZLE0_0_ADDR      107 + PHY_BASE_ADDR // 
#define PHY_DQ_DM_SWIZZLE0_0_OFFSET    0                    // 
#define PHY_DQ_DM_SWIZZLE0_0_WIDTH     32                   // 
#define PHY_DQ_DM_SWIZZLE0_1_ADDR      363 + PHY_BASE_ADDR // 
#define PHY_DQ_DM_SWIZZLE0_1_OFFSET    0                    // 
#define PHY_DQ_DM_SWIZZLE0_1_WIDTH     32                   // 
#define PHY_DQ_DM_SWIZZLE0_2_ADDR      619 + PHY_BASE_ADDR // 
#define PHY_DQ_DM_SWIZZLE0_2_OFFSET    0                    // 
#define PHY_DQ_DM_SWIZZLE0_2_WIDTH     32                   // 
#define PHY_DQ_DM_SWIZZLE0_3_ADDR      875 + PHY_BASE_ADDR // 
#define PHY_DQ_DM_SWIZZLE0_3_OFFSET    0                    // 
#define PHY_DQ_DM_SWIZZLE0_3_WIDTH     32                   // 
#define PHY_DQ_DM_SWIZZLE0_ADDR        107                  // 
#define PHY_DQ_DM_SWIZZLE0_OFFSET      0                    // 
#define PHY_DQ_DM_SWIZZLE0_WIDTH       32                   // 
#define PHY_DQ_DM_SWIZZLE1_0_ADDR      108 + PHY_BASE_ADDR // 
#define PHY_DQ_DM_SWIZZLE1_0_OFFSET    0                    // 
#define PHY_DQ_DM_SWIZZLE1_0_WIDTH     4                    // 
#define PHY_DQ_DM_SWIZZLE1_1_ADDR      364 + PHY_BASE_ADDR // 
#define PHY_DQ_DM_SWIZZLE1_1_OFFSET    0                    // 
#define PHY_DQ_DM_SWIZZLE1_1_WIDTH     4                    // 
#define PHY_DQ_DM_SWIZZLE1_2_ADDR      620 + PHY_BASE_ADDR // 
#define PHY_DQ_DM_SWIZZLE1_2_OFFSET    0                    // 
#define PHY_DQ_DM_SWIZZLE1_2_WIDTH     4                    // 
#define PHY_DQ_DM_SWIZZLE1_3_ADDR      876 + PHY_BASE_ADDR // 
#define PHY_DQ_DM_SWIZZLE1_3_OFFSET    0                    // 
#define PHY_DQ_DM_SWIZZLE1_3_WIDTH     4                    // 
#define PHY_DQ_DM_SWIZZLE1_ADDR        108                  // 
#define PHY_DQ_DM_SWIZZLE1_OFFSET      0                    // 
#define PHY_DQ_DM_SWIZZLE1_WIDTH       4                    // 
#define PHY_DQ_FFE_ADDR                131                  // 
#define PHY_DQ_FFE_OFFSET              8                    // 
#define PHY_DQ_FFE_WIDTH               2                    // 
#define PHY_DQ_IDLE_ADDR               12                   // 
#define PHY_DQ_IDLE_OFFSET             0                    // 
#define PHY_DQ_IDLE_WIDTH              9                    // 
#define PHY_DQ_IE_TIMING_ADDR          92                   // 
#define PHY_DQ_IE_TIMING_OFFSET        8                    // 
#define PHY_DQ_IE_TIMING_WIDTH         8                    // 
#define PHY_DQ_IO_UPDATE_TIMING_ADDR   89                   // 
#define PHY_DQ_IO_UPDATE_TIMING_OFFSET 24                   // 
#define PHY_DQ_IO_UPDATE_TIMING_WIDTH  4                    // 
#define PHY_DQ_MASK_ADDR               31                   // 
#define PHY_DQ_MASK_OFFSET             24                   // 
#define PHY_DQ_MASK_WIDTH              8                    // 
#define PHY_DQ_OE_TIMING_0_ADDR        88 + PHY_BASE_ADDR  // 
#define PHY_DQ_OE_TIMING_0_OFFSET      0                    // 
#define PHY_DQ_OE_TIMING_0_WIDTH       8                    // 
#define PHY_DQ_OE_TIMING_1_ADDR        344 + PHY_BASE_ADDR // 
#define PHY_DQ_OE_TIMING_1_OFFSET      0                    // 
#define PHY_DQ_OE_TIMING_1_WIDTH       8                    // 
#define PHY_DQ_OE_TIMING_2_ADDR        600 + PHY_BASE_ADDR // 
#define PHY_DQ_OE_TIMING_2_OFFSET      0                    // 
#define PHY_DQ_OE_TIMING_2_WIDTH       8                    // 
#define PHY_DQ_OE_TIMING_3_ADDR        856 + PHY_BASE_ADDR // 
#define PHY_DQ_OE_TIMING_3_OFFSET      0                    // 
#define PHY_DQ_OE_TIMING_3_WIDTH       8                    // 
#define PHY_DQ_OE_TIMING_ADDR          88                   // 
#define PHY_DQ_OE_TIMING_OFFSET        0                    // 
#define PHY_DQ_OE_TIMING_WIDTH         8                    // 
#define PHY_DQ_TSEL_ENABLE_ADDR        73                   // 
#define PHY_DQ_TSEL_ENABLE_OFFSET      0                    // 
#define PHY_DQ_TSEL_ENABLE_WIDTH       3                    // 
#define PHY_DQ_TSEL_RD_TIMING_ADDR     88                   // 
#define PHY_DQ_TSEL_RD_TIMING_OFFSET   8                    // 
#define PHY_DQ_TSEL_RD_TIMING_WIDTH    8                    // 
#define PHY_DQ_TSEL_SELECT_0_ADDR      73 + PHY_BASE_ADDR  // 
#define PHY_DQ_TSEL_SELECT_0_OFFSET    8                    // 
#define PHY_DQ_TSEL_SELECT_0_WIDTH     16                   // 
#define PHY_DQ_TSEL_SELECT_1_ADDR      329 + PHY_BASE_ADDR // 
#define PHY_DQ_TSEL_SELECT_1_OFFSET    8                    // 
#define PHY_DQ_TSEL_SELECT_1_WIDTH     16                   // 
#define PHY_DQ_TSEL_SELECT_2_ADDR      585 + PHY_BASE_ADDR // 
#define PHY_DQ_TSEL_SELECT_2_OFFSET    8                    // 
#define PHY_DQ_TSEL_SELECT_2_WIDTH     16                   // 
#define PHY_DQ_TSEL_SELECT_3_ADDR      841 + PHY_BASE_ADDR // 
#define PHY_DQ_TSEL_SELECT_3_OFFSET    8                    // 
#define PHY_DQ_TSEL_SELECT_3_WIDTH     16                   // 
#define PHY_DQ_TSEL_SELECT_ADDR        73                   // 
#define PHY_DQ_TSEL_SELECT_OFFSET      8                    // 
#define PHY_DQ_TSEL_SELECT_WIDTH       16                   // 
#define PHY_DQ_TSEL_WR_TIMING_ADDR     88                   // 
#define PHY_DQ_TSEL_WR_TIMING_OFFSET   16                   // 
#define PHY_DQ_TSEL_WR_TIMING_WIDTH    8                    // 
#define PHY_DQS_FFE_ADDR               131                  // 
#define PHY_DQS_FFE_OFFSET             16                   // 
#define PHY_DQS_FFE_WIDTH              2                    // 
#define PHY_DQS_IE_TIMING_ADDR         92                   // 
#define PHY_DQS_IE_TIMING_OFFSET       16                   // 
#define PHY_DQS_IE_TIMING_WIDTH        8                    // 
#define PHY_DQS_IO_UPDATE_TIMING_ADDR  89                   // 
#define PHY_DQS_IO_UPDATE_TIMING_OFFSET 16                   // 
#define PHY_DQS_IO_UPDATE_TIMING_WIDTH 7                    // 
#define PHY_DQS_OE_RD_TIMING_ADDR      90                   // 
#define PHY_DQS_OE_RD_TIMING_OFFSET    8                    // 
#define PHY_DQS_OE_RD_TIMING_WIDTH     8                    // 
#define PHY_DQS_OE_TIMING_ADDR         88                   // 
#define PHY_DQS_OE_TIMING_OFFSET       24                   // 
#define PHY_DQS_OE_TIMING_WIDTH        8                    // 
#define PHY_DQS_TSEL_ENABLE_ADDR       73                   // 
#define PHY_DQS_TSEL_ENABLE_OFFSET     24                   // 
#define PHY_DQS_TSEL_ENABLE_WIDTH      3                    // 
#define PHY_DQS_TSEL_RD_TIMING_ADDR    89                   // 
#define PHY_DQS_TSEL_RD_TIMING_OFFSET  8                    // 
#define PHY_DQS_TSEL_RD_TIMING_WIDTH   8                    // 
#define PHY_DQS_TSEL_SELECT_0_ADDR     74 + PHY_BASE_ADDR  // 
#define PHY_DQS_TSEL_SELECT_0_OFFSET   0                    // 
#define PHY_DQS_TSEL_SELECT_0_WIDTH    16                   // 
#define PHY_DQS_TSEL_SELECT_1_ADDR     330 + PHY_BASE_ADDR // 
#define PHY_DQS_TSEL_SELECT_1_OFFSET   0                    // 
#define PHY_DQS_TSEL_SELECT_1_WIDTH    16                   // 
#define PHY_DQS_TSEL_SELECT_2_ADDR     586 + PHY_BASE_ADDR // 
#define PHY_DQS_TSEL_SELECT_2_OFFSET   0                    // 
#define PHY_DQS_TSEL_SELECT_2_WIDTH    16                   // 
#define PHY_DQS_TSEL_SELECT_3_ADDR     842 + PHY_BASE_ADDR // 
#define PHY_DQS_TSEL_SELECT_3_OFFSET   0                    // 
#define PHY_DQS_TSEL_SELECT_3_WIDTH    16                   // 
#define PHY_DQS_TSEL_SELECT_ADDR       74                   // 
#define PHY_DQS_TSEL_SELECT_OFFSET     0                    // 
#define PHY_DQS_TSEL_SELECT_WIDTH      16                   // 
#define PHY_DQS_TSEL_WR_TIMING_ADDR    90                   // 
#define PHY_DQS_TSEL_WR_TIMING_OFFSET  16                   // 
#define PHY_DQS_TSEL_WR_TIMING_WIDTH   8                    // 
#define PHY_DS0_DQS_ERR_COUNTER_ADDR   58 + PHY_AC_BASE_ADDR // 
#define PHY_DS0_DQS_ERR_COUNTER_OFFSET 0                    // 
#define PHY_DS0_DQS_ERR_COUNTER_WIDTH  32                   // 
#define PHY_DS1_DQS_ERR_COUNTER_ADDR   59 + PHY_AC_BASE_ADDR // 
#define PHY_DS1_DQS_ERR_COUNTER_OFFSET 0                    // 
#define PHY_DS1_DQS_ERR_COUNTER_WIDTH  32                   // 
#define PHY_DS2_DQS_ERR_COUNTER_ADDR   60 + PHY_AC_BASE_ADDR // 
#define PHY_DS2_DQS_ERR_COUNTER_OFFSET 0                    // 
#define PHY_DS2_DQS_ERR_COUNTER_WIDTH  32                   // 
#define PHY_DS3_DQS_ERR_COUNTER_ADDR   61 + PHY_AC_BASE_ADDR // 
#define PHY_DS3_DQS_ERR_COUNTER_OFFSET 0                    // 
#define PHY_DS3_DQS_ERR_COUNTER_WIDTH  32                   // 
#define PHY_DS_EXIT_CTRL_ADDR          23 + PHY_AC_BASE_ADDR // 
#define PHY_DS_EXIT_CTRL_OFFSET        0                    // 
#define PHY_DS_EXIT_CTRL_WIDTH         17                   // 
#define PHY_DS_INIT_COMPLETE_OBS_ADDR  62 + PHY_AC_BASE_ADDR // 
#define PHY_DS_INIT_COMPLETE_OBS_OFFSET 24                   // 
#define PHY_DS_INIT_COMPLETE_OBS_WIDTH 4                    // 
#define PHY_DS_PCLK_CNTR_CFG_ADDR      24 + PHY_AC_BASE_ADDR // 
#define PHY_DS_PCLK_CNTR_CFG_OFFSET    0                    // 
#define PHY_DS_PCLK_CNTR_CFG_WIDTH     7                    // 
#define PHY_DSLICE_MAX_CS              2                    // 
#define PHY_DSLICE_PAD_BOOSTPN_SETTING_ADDR 130                  // 
#define PHY_DSLICE_PAD_BOOSTPN_SETTING_OFFSET 16                   // 
#define PHY_DSLICE_PAD_BOOSTPN_SETTING_WIDTH 16                   // 
#define PHY_DSLICE_PAD_RX_CTLE_SETTING_ADDR 131                  // 
#define PHY_DSLICE_PAD_RX_CTLE_SETTING_OFFSET 0                    // 
#define PHY_DSLICE_PAD_RX_CTLE_SETTING_WIDTH 6                    // 
#define PHY_ERR_MASK_EN_ADDR           55 + PHY_AC_BASE_ADDR // 
#define PHY_ERR_MASK_EN_OFFSET         24                   // 
#define PHY_ERR_MASK_EN_WIDTH          3                    // 
#define PHY_ERR_STATUS_ADDR            56 + PHY_AC_BASE_ADDR // 
#define PHY_ERR_STATUS_OFFSET          0                    // 
#define PHY_ERR_STATUS_WIDTH           3                    // 
#define PHY_ERROR_INFO_DS0_ADDR        56 + PHY_AC_BASE_ADDR // 
#define PHY_ERROR_INFO_DS0_OFFSET      16                   // 
#define PHY_ERROR_INFO_DS0_WIDTH       1                    // 
#define PHY_ERROR_INFO_DS1_ADDR        56 + PHY_AC_BASE_ADDR // 
#define PHY_ERROR_INFO_DS1_OFFSET      24                   // 
#define PHY_ERROR_INFO_DS1_WIDTH       1                    // 
#define PHY_ERROR_INFO_DS2_ADDR        57 + PHY_AC_BASE_ADDR // 
#define PHY_ERROR_INFO_DS2_OFFSET      0                    // 
#define PHY_ERROR_INFO_DS2_WIDTH       1                    // 
#define PHY_ERROR_INFO_DS3_ADDR        57 + PHY_AC_BASE_ADDR // 
#define PHY_ERROR_INFO_DS3_OFFSET      8                    // 
#define PHY_ERROR_INFO_DS3_WIDTH       1                    // 
#define PHY_ERROR_INFO_TOP_ADDR        56 + PHY_AC_BASE_ADDR // 
#define PHY_ERROR_INFO_TOP_OFFSET      8                    // 
#define PHY_ERROR_INFO_TOP_WIDTH       4                    // 
#define PHY_EXTEND_CLK_STOP_ADDR       90                   // 
#define PHY_EXTEND_CLK_STOP_OFFSET     0                    // 
#define PHY_EXTEND_CLK_STOP_WIDTH      1                    // 
#define PHY_FAST_LVL_EN_ADDR           79                   // 
#define PHY_FAST_LVL_EN_OFFSET         0                    // 
#define PHY_FAST_LVL_EN_WIDTH          4                    // 
#define PHY_FDBK_PWR_CTRL_ADDR         71                   // 
#define PHY_FDBK_PWR_CTRL_OFFSET       8                    // 
#define PHY_FDBK_PWR_CTRL_WIDTH        3                    // 
#define PHY_FIFO_PTR_OBS_ADDR          38                   // 
#define PHY_FIFO_PTR_OBS_OFFSET        16                   // 
#define PHY_FIFO_PTR_OBS_SELECT_ADDR   29                   // 
#define PHY_FIFO_PTR_OBS_SELECT_OFFSET 24                   // 
#define PHY_FIFO_PTR_OBS_SELECT_WIDTH  4                    // 
#define PHY_FIFO_PTR_OBS_WIDTH         8                    // 
#define PHY_FRAC_DELAY_NUM             3                    // 
#define PHY_FREQ_FSP_WIDTH             2                    // 
#define PHY_FREQ_SEL_ADDR              0 + PHY_AC_BASE_ADDR // 
#define PHY_FREQ_SEL_FROM_REGIF_ADDR   1 + PHY_AC_BASE_ADDR // 
#define PHY_FREQ_SEL_FROM_REGIF_OFFSET 0                    // 
#define PHY_FREQ_SEL_FROM_REGIF_WIDTH  1                    // 
#define PHY_FREQ_SEL_INDEX_ADDR        1 + PHY_AC_BASE_ADDR // 
#define PHY_FREQ_SEL_INDEX_OFFSET      16                   // 
#define PHY_FREQ_SEL_INDEX_WIDTH       2                    // 
#define PHY_FREQ_SEL_MULTICAST_EN_ADDR 1 + PHY_AC_BASE_ADDR // 
#define PHY_FREQ_SEL_MULTICAST_EN_OFFSET 8                    // 
#define PHY_FREQ_SEL_MULTICAST_EN_WIDTH 1                    // 
#define PHY_FREQ_SEL_OFFSET            0                    // 
#define PHY_FREQ_SEL_WIDTH             2                    // 
#define PHY_GATE_DELAY_COMP_DISABLE_ADDR 8                    // 
#define PHY_GATE_DELAY_COMP_DISABLE_OFFSET 8                    // 
#define PHY_GATE_DELAY_COMP_DISABLE_WIDTH 1                    // 
#define PHY_GATE_ERROR_DELAY_SELECT_ADDR 16                   // 
#define PHY_GATE_ERROR_DELAY_SELECT_OFFSET 16                   // 
#define PHY_GATE_ERROR_DELAY_SELECT_WIDTH 4                    // 
#define PHY_GATE_SMPL1_SLAVE_DELAY_ADDR 17                   // 
#define PHY_GATE_SMPL1_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_GATE_SMPL1_SLAVE_DELAY_WIDTH 8                    // 
#define PHY_GATE_SMPL1_SLV_DLY_ENC_OBS_ADDR 46                   // 
#define PHY_GATE_SMPL1_SLV_DLY_ENC_OBS_OFFSET 0                    // 
#define PHY_GATE_SMPL1_SLV_DLY_ENC_OBS_WIDTH 10                   // 
#define PHY_GATE_SMPL2_SLAVE_DELAY_ADDR 17                   // 
#define PHY_GATE_SMPL2_SLAVE_DELAY_OFFSET 8                    // 
#define PHY_GATE_SMPL2_SLAVE_DELAY_WIDTH 8                    // 
#define PHY_GATE_SMPL2_SLV_DLY_ENC_OBS_ADDR 46                   // 
#define PHY_GATE_SMPL2_SLV_DLY_ENC_OBS_OFFSET 16                   // 
#define PHY_GATE_SMPL2_SLV_DLY_ENC_OBS_WIDTH 10                   // 
#define PHY_GATE_TRACKING_OBS_ADDR     18                   // 
#define PHY_GATE_TRACKING_OBS_OFFSET   0                    // 
#define PHY_GATE_TRACKING_OBS_WIDTH    32                   // 
#define PHY_GT_RDDQLVL_EN_ADDR         87                   // 
#define PHY_GT_RDDQLVL_EN_OFFSET       0                    // 
#define PHY_GT_RDDQLVL_EN_WIDTH        1                    // 
#define PHY_GTLVL_0_WIDTH_CHK_EN_ADDR  99                   // 
#define PHY_GTLVL_0_WIDTH_CHK_EN_OFFSET 8                    // 
#define PHY_GTLVL_0_WIDTH_CHK_EN_WIDTH 1                    // 
#define PHY_GTLVL_0_WIDTH_THRSHLD_ADDR 99                   // 
#define PHY_GTLVL_0_WIDTH_THRSHLD_OFFSET 0                    // 
#define PHY_GTLVL_0_WIDTH_THRSHLD_WIDTH 7                    // 
#define PHY_GTLVL_BACK_STEP_ADDR       98                   // 
#define PHY_GTLVL_BACK_STEP_OFFSET     0                    // 
#define PHY_GTLVL_BACK_STEP_WIDTH      10                   // 
#define PHY_GTLVL_CAPTURE_CNT_0_ADDR   32 + PHY_BASE_ADDR  // 
#define PHY_GTLVL_CAPTURE_CNT_0_OFFSET 8                    // 
#define PHY_GTLVL_CAPTURE_CNT_0_WIDTH  6                    // 
#define PHY_GTLVL_CAPTURE_CNT_1_ADDR   288 + PHY_BASE_ADDR // 
#define PHY_GTLVL_CAPTURE_CNT_1_OFFSET 8                    // 
#define PHY_GTLVL_CAPTURE_CNT_1_WIDTH  6                    // 
#define PHY_GTLVL_CAPTURE_CNT_2_ADDR   544 + PHY_BASE_ADDR // 
#define PHY_GTLVL_CAPTURE_CNT_2_OFFSET 8                    // 
#define PHY_GTLVL_CAPTURE_CNT_2_WIDTH  6                    // 
#define PHY_GTLVL_CAPTURE_CNT_3_ADDR   800 + PHY_BASE_ADDR // 
#define PHY_GTLVL_CAPTURE_CNT_3_OFFSET 8                    // 
#define PHY_GTLVL_CAPTURE_CNT_3_WIDTH  6                    // 
#define PHY_GTLVL_CAPTURE_CNT_ADDR     32                   // 
#define PHY_GTLVL_CAPTURE_CNT_OFFSET   8                    // 
#define PHY_GTLVL_CAPTURE_CNT_WIDTH    6                    // 
#define PHY_GTLVL_DLY_STEP_ADDR        97                   // 
#define PHY_GTLVL_DLY_STEP_OFFSET      16                   // 
#define PHY_GTLVL_DLY_STEP_WIDTH       4                    // 
#define PHY_GTLVL_FINAL_STEP_ADDR      98                   // 
#define PHY_GTLVL_FINAL_STEP_OFFSET    16                   // 
#define PHY_GTLVL_FINAL_STEP_WIDTH     10                   // 
#define PHY_GTLVL_HARD0_DELAY_OBS_ADDR 47                   // 
#define PHY_GTLVL_HARD0_DELAY_OBS_OFFSET 16                   // 
#define PHY_GTLVL_HARD0_DELAY_OBS_WIDTH 13                   // 
#define PHY_GTLVL_HARD1_DELAY_OBS_ADDR 48                   // 
#define PHY_GTLVL_HARD1_DELAY_OBS_OFFSET 0                    // 
#define PHY_GTLVL_HARD1_DELAY_OBS_WIDTH 13                   // 
#define PHY_GTLVL_LAT_ADJ_START_0_ADDR 125 + PHY_BASE_ADDR // 
#define PHY_GTLVL_LAT_ADJ_START_0_OFFSET 24                   // 
#define PHY_GTLVL_LAT_ADJ_START_0_WIDTH 5                    // 
#define PHY_GTLVL_LAT_ADJ_START_1_ADDR 381 + PHY_BASE_ADDR // 
#define PHY_GTLVL_LAT_ADJ_START_1_OFFSET 24                   // 
#define PHY_GTLVL_LAT_ADJ_START_1_WIDTH 5                    // 
#define PHY_GTLVL_LAT_ADJ_START_2_ADDR 637 + PHY_BASE_ADDR // 
#define PHY_GTLVL_LAT_ADJ_START_2_OFFSET 24                   // 
#define PHY_GTLVL_LAT_ADJ_START_2_WIDTH 5                    // 
#define PHY_GTLVL_LAT_ADJ_START_3_ADDR 893 + PHY_BASE_ADDR // 
#define PHY_GTLVL_LAT_ADJ_START_3_OFFSET 24                   // 
#define PHY_GTLVL_LAT_ADJ_START_3_WIDTH 5                    // 
#define PHY_GTLVL_LAT_ADJ_START_ADDR   125                  // 
#define PHY_GTLVL_LAT_ADJ_START_OFFSET 24                   // 
#define PHY_GTLVL_LAT_ADJ_START_WIDTH  5                    // 
#define PHY_GTLVL_PER_START_ADDR       32                   // 
#define PHY_GTLVL_PER_START_OFFSET     0                    // 
#define PHY_GTLVL_PER_START_WIDTH      8                    // 
#define PHY_GTLVL_RDDQS_SLV_DLY_START_ADDR 125                  // 
#define PHY_GTLVL_RDDQS_SLV_DLY_START_OFFSET 0                    // 
#define PHY_GTLVL_RDDQS_SLV_DLY_START_WIDTH 9                    // 
#define PHY_GTLVL_RESP_WAIT_CNT_ADDR   97                   // 
#define PHY_GTLVL_RESP_WAIT_CNT_OFFSET 24                   // 
#define PHY_GTLVL_RESP_WAIT_CNT_WIDTH  5                    // 
#define PHY_GTLVL_STATUS_OBS_ADDR      49                   // 
#define PHY_GTLVL_STATUS_OBS_OFFSET    0                    // 
#define PHY_GTLVL_STATUS_OBS_WIDTH     18                   // 
#define PHY_GTLVL_UPDT_WAIT_CNT_ADDR   32                   // 
#define PHY_GTLVL_UPDT_WAIT_CNT_OFFSET 16                   // 
#define PHY_GTLVL_UPDT_WAIT_CNT_WIDTH  4                    // 
#define PHY_IE_MODE_0_ADDR             93 + PHY_BASE_ADDR  // 
#define PHY_IE_MODE_0_OFFSET           0                    // 
#define PHY_IE_MODE_0_WIDTH            2                    // 
#define PHY_IE_MODE_1_ADDR             349 + PHY_BASE_ADDR // 
#define PHY_IE_MODE_1_OFFSET           0                    // 
#define PHY_IE_MODE_1_WIDTH            2                    // 
#define PHY_IE_MODE_2_ADDR             605 + PHY_BASE_ADDR // 
#define PHY_IE_MODE_2_OFFSET           0                    // 
#define PHY_IE_MODE_2_WIDTH            2                    // 
#define PHY_IE_MODE_3_ADDR             861 + PHY_BASE_ADDR // 
#define PHY_IE_MODE_3_OFFSET           0                    // 
#define PHY_IE_MODE_3_WIDTH            2                    // 
#define PHY_IE_MODE_ADDR               93                   // 
#define PHY_IE_MODE_OFFSET             0                    // 
#define PHY_IE_MODE_WIDTH              2                    // 
#define PHY_IF_CLK_INVERT_FOR_LOW_FREQ_ADDR 4 + PHY_AC_BASE_ADDR // 
#define PHY_IF_CLK_INVERT_FOR_LOW_FREQ_OFFSET 8                    // 
#define PHY_IF_CLK_INVERT_FOR_LOW_FREQ_WIDTH 1                    // 
#define PHY_INIT_UPDATE_CONFIG_ADDR    55 + PHY_AC_BASE_ADDR // 
#define PHY_INIT_UPDATE_CONFIG_OFFSET  16                   // 
#define PHY_INIT_UPDATE_CONFIG_WIDTH   3                    // 
#define PHY_IO_PAD_DELAY_TIMING_ADDR   89                   // 
#define PHY_IO_PAD_DELAY_TIMING_BYPASS_ADDR 1                    // 
#define PHY_IO_PAD_DELAY_TIMING_BYPASS_OFFSET 0                    // 
#define PHY_IO_PAD_DELAY_TIMING_BYPASS_WIDTH 3                    // 
#define PHY_IO_PAD_DELAY_TIMING_OFFSET 0                    // 
#define PHY_IO_PAD_DELAY_TIMING_WIDTH  3                    // 
#define PHY_LOW_FREQ_SEL_ADDR          68 + PHY_AC_BASE_ADDR // 
#define PHY_LOW_FREQ_SEL_OFFSET        16                   // 
#define PHY_LOW_FREQ_SEL_WIDTH         1                    // 
#define PHY_LP4_ACTIVE_ADDR            5 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_ACTIVE_OFFSET          0                    // 
#define PHY_LP4_ACTIVE_WIDTH           1                    // 
#define PHY_LP4_BOOT_CAL_CLK_SELECT_0_ADDR 36 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_BOOT_CAL_CLK_SELECT_0_OFFSET 8                    // 
#define PHY_LP4_BOOT_CAL_CLK_SELECT_0_WIDTH 3                    // 
#define PHY_LP4_BOOT_DISABLE_ADDR      4 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_BOOT_DISABLE_OFFSET    16                   // 
#define PHY_LP4_BOOT_DISABLE_WIDTH     1                    // 
#define PHY_LP4_BOOT_LOW_FREQ_SEL_ADDR 21 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_BOOT_LOW_FREQ_SEL_OFFSET 0                    // 
#define PHY_LP4_BOOT_LOW_FREQ_SEL_WIDTH 1                    // 
#define PHY_LP4_BOOT_PAD_ACS_IO_CFG_ADDR 2 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_BOOT_PAD_ACS_IO_CFG_OFFSET 8                    // 
#define PHY_LP4_BOOT_PAD_ACS_IO_CFG_WIDTH 15                   // 
#define PHY_LP4_BOOT_PAD_CAL_IO_CFG_0_ADDR 3 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_BOOT_PAD_CAL_IO_CFG_0_OFFSET 0                    // 
#define PHY_LP4_BOOT_PAD_CAL_IO_CFG_0_WIDTH 18                   // 
#define PHY_LP4_BOOT_PAD_DSLICE_IO_CFG_ADDR 0                    // 
#define PHY_LP4_BOOT_PAD_DSLICE_IO_CFG_OFFSET 24                   // 
#define PHY_LP4_BOOT_PAD_DSLICE_IO_CFG_WIDTH 7                    // 
#define PHY_LP4_BOOT_PLL_BYPASS_ADDR   7 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_BOOT_PLL_BYPASS_OFFSET 0                    // 
#define PHY_LP4_BOOT_PLL_BYPASS_WIDTH  1                    // 
#define PHY_LP4_BOOT_PLL_CTRL_ADDR     12 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_BOOT_PLL_CTRL_OFFSET   8                    // 
#define PHY_LP4_BOOT_PLL_CTRL_WIDTH    13                   // 
#define PHY_LP4_BOOT_PLL_DESKEWCALIN_0_ADDR 17 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_BOOT_PLL_DESKEWCALIN_0_OFFSET 16                   // 
#define PHY_LP4_BOOT_PLL_DESKEWCALIN_0_WIDTH 12                   // 
#define PHY_LP4_BOOT_PLL_DESKEWCALIN_1_ADDR 20 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_BOOT_PLL_DESKEWCALIN_1_OFFSET 16                   // 
#define PHY_LP4_BOOT_PLL_DESKEWCALIN_1_WIDTH 12                   // 
#define PHY_LP4_BOOT_RDDATA_EN_DLY_ADDR 6                    // 
#define PHY_LP4_BOOT_RDDATA_EN_DLY_OFFSET 24                   // 
#define PHY_LP4_BOOT_RDDATA_EN_DLY_WIDTH 5                    // 
#define PHY_LP4_BOOT_RDDATA_EN_IE_DLY_ADDR 6                    // 
#define PHY_LP4_BOOT_RDDATA_EN_IE_DLY_OFFSET 16                   // 
#define PHY_LP4_BOOT_RDDATA_EN_IE_DLY_WIDTH 2                    // 
#define PHY_LP4_BOOT_RDDATA_EN_OE_DLY_ADDR 8                    // 
#define PHY_LP4_BOOT_RDDATA_EN_OE_DLY_OFFSET 0                    // 
#define PHY_LP4_BOOT_RDDATA_EN_OE_DLY_WIDTH 5                    // 
#define PHY_LP4_BOOT_RDDATA_EN_TSEL_DLY_ADDR 7                    // 
#define PHY_LP4_BOOT_RDDATA_EN_TSEL_DLY_OFFSET 0                    // 
#define PHY_LP4_BOOT_RDDATA_EN_TSEL_DLY_WIDTH 5                    // 
#define PHY_LP4_BOOT_RDDQS_LATENCY_ADJUST_ADDR 7                    // 
#define PHY_LP4_BOOT_RDDQS_LATENCY_ADJUST_OFFSET 16                   // 
#define PHY_LP4_BOOT_RDDQS_LATENCY_ADJUST_WIDTH 5                    // 
#define PHY_LP4_BOOT_RPTR_UPDATE_ADDR  7                    // 
#define PHY_LP4_BOOT_RPTR_UPDATE_OFFSET 8                    // 
#define PHY_LP4_BOOT_RPTR_UPDATE_WIDTH 4                    // 
#define PHY_LP4_BOOT_RX_PCLK_ACS_SELECT_ADDR 2 + PHY_AC_BASE_ADDR // 
#define PHY_LP4_BOOT_RX_PCLK_ACS_SELECT_OFFSET 0                    // 
#define PHY_LP4_BOOT_RX_PCLK_ACS_SELECT_WIDTH 3                    // 
#define PHY_LP4_BOOT_RX_PCLK_CLK_SEL_ADDR 0                    // 
#define PHY_LP4_BOOT_RX_PCLK_CLK_SEL_OFFSET 16                   // 
#define PHY_LP4_BOOT_RX_PCLK_CLK_SEL_WIDTH 3                    // 
#define PHY_LP4_BOOT_WRPATH_GATE_DISABLE_ADDR 7                    // 
#define PHY_LP4_BOOT_WRPATH_GATE_DISABLE_OFFSET 24                   // 
#define PHY_LP4_BOOT_WRPATH_GATE_DISABLE_WIDTH 2                    // 
#define PHY_LP4_WDQS_OE_EXTEND_ADDR    61                   // 
#define PHY_LP4_WDQS_OE_EXTEND_OFFSET  8                    // 
#define PHY_LP4_WDQS_OE_EXTEND_WIDTH   1                    // 
#define PHY_LP_CTRLUPD_CNTR_CFG_ADDR   22 + PHY_AC_BASE_ADDR // 
#define PHY_LP_CTRLUPD_CNTR_CFG_OFFSET 0                    // 
#define PHY_LP_CTRLUPD_CNTR_CFG_WIDTH  10                   // 
#define PHY_LP_DBI_TRAIN_ADDR          93                   // 
#define PHY_LP_DBI_TRAIN_OFFSET        16                   // 
#define PHY_LP_DBI_TRAIN_WIDTH         1                    // 
#define PHY_LP_WAKEUP_ADDR             21 + PHY_AC_BASE_ADDR // 
#define PHY_LP_WAKEUP_OFFSET           16                   // 
#define PHY_LP_WAKEUP_WIDTH            8                    // 
#define PHY_LPBK_CONTROL_0_ADDR        9 + PHY_BASE_ADDR   // 
#define PHY_LPBK_CONTROL_0_OFFSET      0                    // 
#define PHY_LPBK_CONTROL_0_WIDTH       9                    // 
#define PHY_LPBK_CONTROL_1_ADDR        265 + PHY_BASE_ADDR // 
#define PHY_LPBK_CONTROL_1_OFFSET      0                    // 
#define PHY_LPBK_CONTROL_1_WIDTH       9                    // 
#define PHY_LPBK_CONTROL_2_ADDR        521 + PHY_BASE_ADDR // 
#define PHY_LPBK_CONTROL_2_OFFSET      0                    // 
#define PHY_LPBK_CONTROL_2_WIDTH       9                    // 
#define PHY_LPBK_CONTROL_3_ADDR        777 + PHY_BASE_ADDR // 
#define PHY_LPBK_CONTROL_3_OFFSET      0                    // 
#define PHY_LPBK_CONTROL_3_WIDTH       9                    // 
#define PHY_LPBK_CONTROL_ADDR          9                    // 
#define PHY_LPBK_CONTROL_OFFSET        0                    // 
#define PHY_LPBK_CONTROL_WIDTH         9                    // 
#define PHY_LPBK_DFX_TIMEOUT_EN_ADDR   9                    // 
#define PHY_LPBK_DFX_TIMEOUT_EN_OFFSET 16                   // 
#define PHY_LPBK_DFX_TIMEOUT_EN_WIDTH  1                    // 
#define PHY_LPBK_ERROR_COUNT_OBS_0_ADDR 40 + PHY_BASE_ADDR  // 
#define PHY_LPBK_ERROR_COUNT_OBS_ADDR  40                   // 
#define PHY_LPBK_ERROR_COUNT_OBS_OFFSET 0                    // 
#define PHY_LPBK_ERROR_COUNT_OBS_WIDTH 16                   // 
#define PHY_LPBK_RESULT_OBS_0_ADDR     39 + PHY_BASE_ADDR  // 
#define PHY_LPBK_RESULT_OBS_1_ADDR     295 + PHY_BASE_ADDR // 
#define PHY_LPBK_RESULT_OBS_2_ADDR     551 + PHY_BASE_ADDR // 
#define PHY_LPBK_RESULT_OBS_3_ADDR     807 + PHY_BASE_ADDR // 
#define PHY_LPBK_RESULT_OBS_ADDR       39                   // 
#define PHY_LPBK_RESULT_OBS_OFFSET     0                    // 
#define PHY_LPBK_RESULT_OBS_WIDTH      32                   // 
#define PHY_LPDDR_ADDR                 19                   // 
#define PHY_LPDDR_OFFSET               0                    // 
#define PHY_LPDDR_WIDTH                1                    // 
#define PHY_LS_IDLE_EN_ADDR            21 + PHY_AC_BASE_ADDR // 
#define PHY_LS_IDLE_EN_OFFSET          24                   // 
#define PHY_LS_IDLE_EN_WIDTH           1                    // 
#define PHY_LVL_DEBUG_MODE_ADDR        30                   // 
#define PHY_LVL_DEBUG_MODE_OFFSET      0                    // 
#define PHY_LVL_DEBUG_MODE_WIDTH       1                    // 
#define PHY_LVL_MEAS_DLY_STEP_ENABLE_ADDR 1092 + PHY_BASE_ADDR // 
#define PHY_LVL_MEAS_DLY_STEP_ENABLE_OFFSET 0                    // 
#define PHY_LVL_MEAS_DLY_STEP_ENABLE_WIDTH 1                    // 
#define PHY_LVL_PATTERN_WIDTH          4                    // 
#define PHY_MANUAL_UPDATE_PHYUPD_ENABLE_ADDR 4 + PHY_AC_BASE_ADDR // 
#define PHY_MANUAL_UPDATE_PHYUPD_ENABLE_OFFSET 0                    // 
#define PHY_MANUAL_UPDATE_PHYUPD_ENABLE_WIDTH 1                    // 
#define PHY_MASTER_DELAY_HALF_MEASURE_ADDR 96                   // 
#define PHY_MASTER_DELAY_HALF_MEASURE_OFFSET 8                    // 
#define PHY_MASTER_DELAY_HALF_MEASURE_WIDTH 8                    // 
#define PHY_MASTER_DELAY_LESS_STEP_ADDR 94                   // 
#define PHY_MASTER_DELAY_LESS_STEP_OFFSET 24                   // 
#define PHY_MASTER_DELAY_LESS_STEP_WIDTH 3                    // 
#define PHY_MASTER_DELAY_LOCK_ERROR_CNT_OBS_ADDR 38                   // 
#define PHY_MASTER_DELAY_LOCK_ERROR_CNT_OBS_OFFSET 8                    // 
#define PHY_MASTER_DELAY_LOCK_ERROR_CNT_OBS_WIDTH 8                    // 
#define PHY_MASTER_DELAY_LOCK_MODE_ADDR 94                   // 
#define PHY_MASTER_DELAY_LOCK_MODE_OFFSET 16                   // 
#define PHY_MASTER_DELAY_LOCK_MODE_WIDTH 3                    // 
#define PHY_MASTER_DELAY_START_ADDR    95                   // 
#define PHY_MASTER_DELAY_START_OFFSET  8                    // 
#define PHY_MASTER_DELAY_START_WIDTH   11                   // 
#define PHY_MASTER_DELAY_STEP_0_ADDR   95 + PHY_BASE_ADDR  // 
#define PHY_MASTER_DELAY_STEP_0_OFFSET 24                   // 
#define PHY_MASTER_DELAY_STEP_0_WIDTH  6                    // 
#define PHY_MASTER_DELAY_STEP_ADDR     95                   // 
#define PHY_MASTER_DELAY_STEP_OFFSET   24                   // 
#define PHY_MASTER_DELAY_STEP_WIDTH    6                    // 
#define PHY_MASTER_DELAY_THRESHOLD_FACTOR_ADDR 95                   // 
#define PHY_MASTER_DELAY_THRESHOLD_FACTOR_OFFSET 0                    // 
#define PHY_MASTER_DELAY_THRESHOLD_FACTOR_WIDTH 3                    // 
#define PHY_MASTER_DELAY_WAIT_ADDR     96                   // 
#define PHY_MASTER_DELAY_WAIT_OFFSET   0                    // 
#define PHY_MASTER_DELAY_WAIT_WIDTH    8                    // 
#define PHY_MASTER_DLY_LOCK_OBS_ADDR   40                   // 
#define PHY_MASTER_DLY_LOCK_OBS_OFFSET 16                   // 
#define PHY_MASTER_DLY_LOCK_OBS_SELECT_ADDR 28                   // 
#define PHY_MASTER_DLY_LOCK_OBS_SELECT_OFFSET 16                   // 
#define PHY_MASTER_DLY_LOCK_OBS_SELECT_WIDTH 4                    // 
#define PHY_MASTER_DLY_LOCK_OBS_WIDTH  11                   // 
#define PHY_MAX_CKE                    2                    // 
#define PHY_MAX_CLK                    1                    // 
#define PHY_MAX_CLK_PER_CA             1                    // 
#define PHY_MAX_CS                     4                    // 
#define PHY_MAX_CS_AFTER_REDUC         4                    // 
#define PHY_MAX_CS_PER_CA              4                    // 
#define PHY_MAX_CS_SINGLE_CHNL         2                    // 
#define PHY_MAX_ODT                    0                    // 
#define PHY_MAX_RST                    1                    // 
#define PHY_MC_DFI_ADDRESS_WIDTH       12                   // 
#define PHY_MEAS_DLY_STEP_ENABLE_ADDR  106                  // 
#define PHY_MEAS_DLY_STEP_ENABLE_OFFSET 8                    // 
#define PHY_MEAS_DLY_STEP_ENABLE_WIDTH 6                    // 
#define PHY_MEAS_DLY_STEP_VALUE_ADDR   41                   // 
#define PHY_MEAS_DLY_STEP_VALUE_OFFSET 16                   // 
#define PHY_MEAS_DLY_STEP_VALUE_WIDTH  8                    // 
#define PHY_MEM_ADDRESS_PER_CA_WIDTH   6                    // 
#define PHY_MEM_ADDRESS_WIDTH          6                    // 
#define PHY_MEM_CLASS_ADDR             19                   // 
#define PHY_MEM_CLASS_OFFSET           8                    // 
#define PHY_MEM_CLASS_WIDTH            3                    // 
#define PHY_MEM_DATA_WIDTH             32                   // 
#define PHY_MEM_DM_WIDTH               4                    // 
#define PHY_MEM_DQS_WIDTH              4                    // 
#define PHY_MEM_ECC_CHECK_WIDTH        0                    // 
#define PHY_MEM_ECC_SLICE_COUNT        0                    // 
#define PHY_MEM_SLICE_COUNT            4                    // 
#define PHY_MEMCLK_STATIC_TOG_DISABLE_ADDR 1071 + PHY_BASE_ADDR // 
#define PHY_MEMCLK_STATIC_TOG_DISABLE_OFFSET 16                   // 
#define PHY_MEMCLK_STATIC_TOG_DISABLE_WIDTH 1                    // 
#define PHY_MEMCLK_SW_TXIO_CTRL_ADDR   1071 + PHY_BASE_ADDR // 
#define PHY_MEMCLK_SW_TXIO_CTRL_OFFSET 8                    // 
#define PHY_MEMCLK_SW_TXIO_CTRL_WIDTH  1                    // 
#define PHY_MEMCLK_SW_TXPWR_CTRL_ADDR  1071 + PHY_BASE_ADDR // 
#define PHY_MEMCLK_SW_TXPWR_CTRL_OFFSET 0                    // 
#define PHY_MEMCLK_SW_TXPWR_CTRL_WIDTH 1                    // 
#define PHY_NTP_PASS_ADDR              126                  // 
#define PHY_NTP_PASS_OFFSET            24                   // 
#define PHY_NTP_PASS_WIDTH             1                    // 
#define PHY_NTP_WDQ_BIT_EN_ADDR        77                   // 
#define PHY_NTP_WDQ_BIT_EN_OFFSET      16                   // 
#define PHY_NTP_WDQ_BIT_EN_WIDTH       8                    // 
#define PHY_NTP_WDQ_START_ADDR         76                   // 
#define PHY_NTP_WDQ_START_OFFSET       8                    // 
#define PHY_NTP_WDQ_START_WIDTH        10                   // 
#define PHY_NTP_WDQ_STEP_SIZE_ADDR     76                   // 
#define PHY_NTP_WDQ_STEP_SIZE_OFFSET   0                    // 
#define PHY_NTP_WDQ_STEP_SIZE_WIDTH    8                    // 
#define PHY_NTP_WDQ_STOP_ADDR          77                   // 
#define PHY_NTP_WDQ_STOP_OFFSET        0                    // 
#define PHY_NTP_WDQ_STOP_WIDTH         10                   // 
#define PHY_NTP_WRLAT_START_ADDR       126                  // 
#define PHY_NTP_WRLAT_START_OFFSET     16                   // 
#define PHY_NTP_WRLAT_START_WIDTH      4                    // 
#define PHY_PAD_ACS_IO_CFG_ADDR        66 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_ACS_IO_CFG_OFFSET      0                    // 
#define PHY_PAD_ACS_IO_CFG_WIDTH       15                   // 
#define PHY_PAD_ACS_RX_PCLK_CLK_SEL_ADDR 66 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_ACS_RX_PCLK_CLK_SEL_OFFSET 16                   // 
#define PHY_PAD_ACS_RX_PCLK_CLK_SEL_WIDTH 3                    // 
#define PHY_PAD_ADDR_DRIVE2_ADDR       77 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_ADDR_DRIVE2_OFFSET     0                    // 
#define PHY_PAD_ADDR_DRIVE2_WIDTH      29                   // 
#define PHY_PAD_ADDR_DRIVE_ADDR        76 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_ADDR_DRIVE_OFFSET      0                    // 
#define PHY_PAD_ADDR_DRIVE_WIDTH       30                   // 
#define PHY_PAD_ADDR_TERM_ADDR         28 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_ADDR_TERM_OFFSET       0                    // 
#define PHY_PAD_ADDR_TERM_WIDTH        18                   // 
#define PHY_PAD_ADR_IO_CFG_ADDR        1044 + PHY_BASE_ADDR // 
#define PHY_PAD_ADR_IO_CFG_OFFSET      8                    // 
#define PHY_PAD_ADR_IO_CFG_WIDTH       7                    // 
#define PHY_PAD_ATB_CTRL_ADDR          53 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_ATB_CTRL_OFFSET        0                    // 
#define PHY_PAD_ATB_CTRL_WIDTH         16                   // 
#define PHY_PAD_CAL_IO_CFG_0_ADDR      65 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_CAL_IO_CFG_0_OFFSET    0                    // 
#define PHY_PAD_CAL_IO_CFG_0_WIDTH     18                   // 
#define PHY_PAD_CAL_IO_CFG_WIDTH       18                   // 
#define PHY_PAD_CKE_DRIVE2_ADDR        81 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_CKE_DRIVE2_OFFSET      0                    // 
#define PHY_PAD_CKE_DRIVE2_WIDTH       29                   // 
#define PHY_PAD_CKE_DRIVE_ADDR         80 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_CKE_DRIVE_OFFSET       0                    // 
#define PHY_PAD_CKE_DRIVE_WIDTH        30                   // 
#define PHY_PAD_CKE_TERM_ADDR          30 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_CKE_TERM_OFFSET        0                    // 
#define PHY_PAD_CKE_TERM_WIDTH         18                   // 
#define PHY_PAD_CLK_DRIVE2_ADDR        79 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_CLK_DRIVE2_OFFSET      0                    // 
#define PHY_PAD_CLK_DRIVE2_WIDTH       20                   // 
#define PHY_PAD_CLK_DRIVE_ADDR         78 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_CLK_DRIVE_OFFSET       0                    // 
#define PHY_PAD_CLK_DRIVE_WIDTH        32                   // 
#define PHY_PAD_CLK_TERM_ADDR          29 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_CLK_TERM_OFFSET        0                    // 
#define PHY_PAD_CLK_TERM_WIDTH         18                   // 
#define PHY_PAD_CS_DRIVE2_ADDR         85 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_CS_DRIVE2_OFFSET       0                    // 
#define PHY_PAD_CS_DRIVE2_WIDTH        29                   // 
#define PHY_PAD_CS_DRIVE_ADDR          84 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_CS_DRIVE_OFFSET        0                    // 
#define PHY_PAD_CS_DRIVE_WIDTH         30                   // 
#define PHY_PAD_CS_TERM_ADDR           32 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_CS_TERM_OFFSET         0                    // 
#define PHY_PAD_CS_TERM_WIDTH          18                   // 
#define PHY_PAD_DATA_DRIVE2_ADDR       73 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_DATA_DRIVE2_OFFSET     0                    // 
#define PHY_PAD_DATA_DRIVE2_WIDTH      2                    // 
#define PHY_PAD_DATA_DRIVE_ADDR        72 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_DATA_DRIVE_OFFSET      0                    // 
#define PHY_PAD_DATA_DRIVE_WIDTH       31                   // 
#define PHY_PAD_DATA_TERM_ADDR         26 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_DATA_TERM_OFFSET       0                    // 
#define PHY_PAD_DATA_TERM_WIDTH        17                   // 
#define PHY_PAD_DM_RX_DCD_ADDR         81                   // 
#define PHY_PAD_DM_RX_DCD_OFFSET       16                   // 
#define PHY_PAD_DM_RX_DCD_WIDTH        5                    // 
#define PHY_PAD_DQS_DRIVE2_ADDR        75 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_DQS_DRIVE2_OFFSET      0                    // 
#define PHY_PAD_DQS_DRIVE2_WIDTH       2                    // 
#define PHY_PAD_DQS_DRIVE_ADDR         74 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_DQS_DRIVE_OFFSET       0                    // 
#define PHY_PAD_DQS_DRIVE_WIDTH        32                   // 
#define PHY_PAD_DQS_RX_DCD_ADDR        81                   // 
#define PHY_PAD_DQS_RX_DCD_OFFSET      24                   // 
#define PHY_PAD_DQS_RX_DCD_WIDTH       5                    // 
#define PHY_PAD_DQS_TERM_ADDR          27 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_DQS_TERM_OFFSET        0                    // 
#define PHY_PAD_DQS_TERM_WIDTH         17                   // 
#define PHY_PAD_DS_PCLK_DIV_ADDR       24 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_DS_PCLK_DIV_OFFSET     8                    // 
#define PHY_PAD_DS_PCLK_DIV_WIDTH      3                    // 
#define PHY_PAD_DSLICE_IO_CFG_ADDR     82                   // 
#define PHY_PAD_DSLICE_IO_CFG_OFFSET   8                    // 
#define PHY_PAD_DSLICE_IO_CFG_WIDTH    7                    // 
#define PHY_PAD_FDBK_DRIVE2_ADDR       71 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_FDBK_DRIVE2_OFFSET     0                    // 
#define PHY_PAD_FDBK_DRIVE2_WIDTH      20                   // 
#define PHY_PAD_FDBK_DRIVE_ADDR        70 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_FDBK_DRIVE_OFFSET      0                    // 
#define PHY_PAD_FDBK_DRIVE_WIDTH       30                   // 
#define PHY_PAD_FDBK_RX_DCD_ADDR       82                   // 
#define PHY_PAD_FDBK_RX_DCD_OFFSET     0                    // 
#define PHY_PAD_FDBK_RX_DCD_WIDTH      5                    // 
#define PHY_PAD_FDBK_TERM_ADDR         25 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_FDBK_TERM_OFFSET       0                    // 
#define PHY_PAD_FDBK_TERM_WIDTH        18                   // 
#define PHY_PAD_RST_DRIVE2_ADDR        83 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_RST_DRIVE2_OFFSET      0                    // 
#define PHY_PAD_RST_DRIVE2_WIDTH       29                   // 
#define PHY_PAD_RST_DRIVE_ADDR         82 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_RST_DRIVE_OFFSET       0                    // 
#define PHY_PAD_RST_DRIVE_WIDTH        30                   // 
#define PHY_PAD_RST_TERM_ADDR          31 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_RST_TERM_OFFSET        0                    // 
#define PHY_PAD_RST_TERM_WIDTH         18                   // 
#define PHY_PAD_RX_BIAS_EN_ADDR        68                   // 
#define PHY_PAD_RX_BIAS_EN_OFFSET      0                    // 
#define PHY_PAD_RX_BIAS_EN_WIDTH       11                   // 
#define PHY_PAD_RX_DCD_0_ADDR          79                   // 
#define PHY_PAD_RX_DCD_0_OFFSET        16                   // 
#define PHY_PAD_RX_DCD_0_WIDTH         5                    // 
#define PHY_PAD_RX_DCD_1_ADDR          79                   // 
#define PHY_PAD_RX_DCD_1_OFFSET        24                   // 
#define PHY_PAD_RX_DCD_1_WIDTH         5                    // 
#define PHY_PAD_RX_DCD_2_ADDR          80                   // 
#define PHY_PAD_RX_DCD_2_OFFSET        0                    // 
#define PHY_PAD_RX_DCD_2_WIDTH         5                    // 
#define PHY_PAD_RX_DCD_3_ADDR          80                   // 
#define PHY_PAD_RX_DCD_3_OFFSET        8                    // 
#define PHY_PAD_RX_DCD_3_WIDTH         5                    // 
#define PHY_PAD_RX_DCD_4_ADDR          80                   // 
#define PHY_PAD_RX_DCD_4_OFFSET        16                   // 
#define PHY_PAD_RX_DCD_4_WIDTH         5                    // 
#define PHY_PAD_RX_DCD_5_ADDR          80                   // 
#define PHY_PAD_RX_DCD_5_OFFSET        24                   // 
#define PHY_PAD_RX_DCD_5_WIDTH         5                    // 
#define PHY_PAD_RX_DCD_6_ADDR          81                   // 
#define PHY_PAD_RX_DCD_6_OFFSET        0                    // 
#define PHY_PAD_RX_DCD_6_WIDTH         5                    // 
#define PHY_PAD_RX_DCD_7_ADDR          81                   // 
#define PHY_PAD_RX_DCD_7_OFFSET        8                    // 
#define PHY_PAD_RX_DCD_7_WIDTH         5                    // 
#define PHY_PAD_RX_DCD_WIDTH           5                    // 
#define PHY_PAD_TX_DCD_ADDR            79                   // 
#define PHY_PAD_TX_DCD_OFFSET          8                    // 
#define PHY_PAD_TX_DCD_WIDTH           5                    // 
#define PHY_PAD_VREF_CTRL_AC_ADDR      69 + PHY_AC_BASE_ADDR // 
#define PHY_PAD_VREF_CTRL_AC_OFFSET    0                    // 
#define PHY_PAD_VREF_CTRL_AC_WIDTH     13                   // 
#define PHY_PAD_VREF_CTRL_DQ_ADDR      91                   // 
#define PHY_PAD_VREF_CTRL_DQ_OFFSET    16                   // 
#define PHY_PAD_VREF_CTRL_DQ_WIDTH     13                   // 
#define PHY_PER_CS_TRAINING_EN_0_ADDR  92 + PHY_BASE_ADDR  // 
#define PHY_PER_CS_TRAINING_EN_0_OFFSET 0                    // 
#define PHY_PER_CS_TRAINING_EN_0_WIDTH 1                    // 
#define PHY_PER_CS_TRAINING_EN_1_ADDR  348 + PHY_BASE_ADDR // 
#define PHY_PER_CS_TRAINING_EN_1_OFFSET 0                    // 
#define PHY_PER_CS_TRAINING_EN_1_WIDTH 1                    // 
#define PHY_PER_CS_TRAINING_EN_2_ADDR  604 + PHY_BASE_ADDR // 
#define PHY_PER_CS_TRAINING_EN_2_OFFSET 0                    // 
#define PHY_PER_CS_TRAINING_EN_2_WIDTH 1                    // 
#define PHY_PER_CS_TRAINING_EN_3_ADDR  860 + PHY_BASE_ADDR // 
#define PHY_PER_CS_TRAINING_EN_3_OFFSET 0                    // 
#define PHY_PER_CS_TRAINING_EN_3_WIDTH 1                    // 
#define PHY_PER_CS_TRAINING_EN_ADDR    92                   // 
#define PHY_PER_CS_TRAINING_EN_OFFSET  0                    // 
#define PHY_PER_CS_TRAINING_EN_WIDTH   1                    // 
#define PHY_PER_CS_TRAINING_INDEX_ADDR 6                    // 
#define PHY_PER_CS_TRAINING_INDEX_OFFSET 8                    // 
#define PHY_PER_CS_TRAINING_INDEX_WIDTH 1                    // 
#define PHY_PER_CS_TRAINING_MULTICAST_EN_ADDR 6                    // 
#define PHY_PER_CS_TRAINING_MULTICAST_EN_OFFSET 0                    // 
#define PHY_PER_CS_TRAINING_MULTICAST_EN_WIDTH 1                    // 
#define PHY_PER_RANK_CS_MAP_ADDR       5                    // 
#define PHY_PER_RANK_CS_MAP_OFFSET     24                   // 
#define PHY_PER_RANK_CS_MAP_WIDTH      2                    // 
#define PHY_PLL_BYPASS_ADDR            67 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_BYPASS_OFFSET          0                    // 
#define PHY_PLL_BYPASS_WIDTH           1                    // 
#define PHY_PLL_CTRL_ADDR              68 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_CTRL_OFFSET            0                    // 
#define PHY_PLL_CTRL_OVERRIDE_ADDR     13 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_CTRL_OVERRIDE_OFFSET   0                    // 
#define PHY_PLL_CTRL_OVERRIDE_WIDTH    16                   // 
#define PHY_PLL_CTRL_WIDTH             13                   // 
#define PHY_PLL_DESKEWCALIN_0_ADDR     17 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_DESKEWCALIN_0_OFFSET   0                    // 
#define PHY_PLL_DESKEWCALIN_0_WIDTH    12                   // 
#define PHY_PLL_DESKEWCALIN_1_ADDR     20 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_DESKEWCALIN_1_OFFSET   0                    // 
#define PHY_PLL_DESKEWCALIN_1_WIDTH    12                   // 
#define PHY_PLL_OBS_0_ADDR             15 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_OBS_0_OFFSET           8                    // 
#define PHY_PLL_OBS_0_WIDTH            16                   // 
#define PHY_PLL_OBS_1_ADDR             18 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_OBS_1_OFFSET           0                    // 
#define PHY_PLL_OBS_1_WIDTH            16                   // 
#define PHY_PLL_OBS_WIDTH              16                   // 
#define PHY_PLL_SPO_CAL_CTRL_0_ADDR    14 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_SPO_CAL_CTRL_0_OFFSET  0                    // 
#define PHY_PLL_SPO_CAL_CTRL_0_WIDTH   10                   // 
#define PHY_PLL_SPO_CAL_CTRL_1_ADDR    14 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_SPO_CAL_CTRL_1_OFFSET  16                   // 
#define PHY_PLL_SPO_CAL_CTRL_1_WIDTH   10                   // 
#define PHY_PLL_SPO_CAL_OBS_0_ADDR     16 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_SPO_CAL_OBS_0_OFFSET   0                    // 
#define PHY_PLL_SPO_CAL_OBS_0_WIDTH    17                   // 
#define PHY_PLL_SPO_CAL_OBS_1_ADDR     19 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_SPO_CAL_OBS_1_OFFSET   0                    // 
#define PHY_PLL_SPO_CAL_OBS_1_WIDTH    17                   // 
#define PHY_PLL_SPO_CAL_OBS_WIDTH      17                   // 
#define PHY_PLL_WAIT_ADDR              9 + PHY_AC_BASE_ADDR // 
#define PHY_PLL_WAIT_OFFSET            0                    // 
#define PHY_PLL_WAIT_WIDTH             8                    // 
#define PHY_PRBS_PATTERN_MASK_ADDR     13                   // 
#define PHY_PRBS_PATTERN_MASK_OFFSET   0                    // 
#define PHY_PRBS_PATTERN_MASK_WIDTH    9                    // 
#define PHY_PRBS_PATTERN_START_0_ADDR  12 + PHY_BASE_ADDR  // 
#define PHY_PRBS_PATTERN_START_0_OFFSET 16                   // 
#define PHY_PRBS_PATTERN_START_0_WIDTH 7                    // 
#define PHY_PRBS_PATTERN_START_1_ADDR  268 + PHY_BASE_ADDR // 
#define PHY_PRBS_PATTERN_START_1_OFFSET 16                   // 
#define PHY_PRBS_PATTERN_START_1_WIDTH 7                    // 
#define PHY_PRBS_PATTERN_START_2_ADDR  524 + PHY_BASE_ADDR // 
#define PHY_PRBS_PATTERN_START_2_OFFSET 16                   // 
#define PHY_PRBS_PATTERN_START_2_WIDTH 7                    // 
#define PHY_PRBS_PATTERN_START_3_ADDR  780 + PHY_BASE_ADDR // 
#define PHY_PRBS_PATTERN_START_3_OFFSET 16                   // 
#define PHY_PRBS_PATTERN_START_3_WIDTH 7                    // 
#define PHY_PRBS_PATTERN_START_ADDR    12                   // 
#define PHY_PRBS_PATTERN_START_OFFSET  16                   // 
#define PHY_PRBS_PATTERN_START_WIDTH   7                    // 
#define PHY_PST_AMBLE_ADDR             74                   // 
#define PHY_PST_AMBLE_OFFSET           24                   // 
#define PHY_PST_AMBLE_WIDTH            4                    // 
#define PHY_RDDATA_EN_DLY_0_ADDR       106 + PHY_BASE_ADDR // 
#define PHY_RDDATA_EN_DLY_0_OFFSET     16                   // 
#define PHY_RDDATA_EN_DLY_0_WIDTH      5                    // 
#define PHY_RDDATA_EN_DLY_1_ADDR       362 + PHY_BASE_ADDR // 
#define PHY_RDDATA_EN_DLY_1_OFFSET     16                   // 
#define PHY_RDDATA_EN_DLY_1_WIDTH      5                    // 
#define PHY_RDDATA_EN_DLY_2_ADDR       618 + PHY_BASE_ADDR // 
#define PHY_RDDATA_EN_DLY_2_OFFSET     16                   // 
#define PHY_RDDATA_EN_DLY_2_WIDTH      5                    // 
#define PHY_RDDATA_EN_DLY_3_ADDR       874 + PHY_BASE_ADDR // 
#define PHY_RDDATA_EN_DLY_3_OFFSET     16                   // 
#define PHY_RDDATA_EN_DLY_3_WIDTH      5                    // 
#define PHY_RDDATA_EN_DLY_ADDR         106                  // 
#define PHY_RDDATA_EN_DLY_OFFSET       16                   // 
#define PHY_RDDATA_EN_DLY_WIDTH        5                    // 
#define PHY_RDDATA_EN_IE_DLY_ADDR      92                   // 
#define PHY_RDDATA_EN_IE_DLY_OFFSET    24                   // 
#define PHY_RDDATA_EN_IE_DLY_WIDTH     2                    // 
#define PHY_RDDATA_EN_OE_DLY_ADDR      94                   // 
#define PHY_RDDATA_EN_OE_DLY_OFFSET    0                    // 
#define PHY_RDDATA_EN_OE_DLY_WIDTH     5                    // 
#define PHY_RDDATA_EN_TSEL_DLY_0_ADDR  93 + PHY_BASE_ADDR  // 
#define PHY_RDDATA_EN_TSEL_DLY_0_OFFSET 24                   // 
#define PHY_RDDATA_EN_TSEL_DLY_0_WIDTH 5                    // 
#define PHY_RDDATA_EN_TSEL_DLY_1_ADDR  349 + PHY_BASE_ADDR // 
#define PHY_RDDATA_EN_TSEL_DLY_1_OFFSET 24                   // 
#define PHY_RDDATA_EN_TSEL_DLY_1_WIDTH 5                    // 
#define PHY_RDDATA_EN_TSEL_DLY_2_ADDR  605 + PHY_BASE_ADDR // 
#define PHY_RDDATA_EN_TSEL_DLY_2_OFFSET 24                   // 
#define PHY_RDDATA_EN_TSEL_DLY_2_WIDTH 5                    // 
#define PHY_RDDATA_EN_TSEL_DLY_3_ADDR  861 + PHY_BASE_ADDR // 
#define PHY_RDDATA_EN_TSEL_DLY_3_OFFSET 24                   // 
#define PHY_RDDATA_EN_TSEL_DLY_3_WIDTH 5                    // 
#define PHY_RDDATA_EN_TSEL_DLY_ADDR    93                   // 
#define PHY_RDDATA_EN_TSEL_DLY_OFFSET  24                   // 
#define PHY_RDDATA_EN_TSEL_DLY_WIDTH   5                    // 
#define PHY_RDDM_SLAVE_DELAY_0_ADDR    86 + PHY_BASE_ADDR  // 
#define PHY_RDDM_SLAVE_DELAY_0_OFFSET  16                   // 
#define PHY_RDDM_SLAVE_DELAY_0_WIDTH   9                    // 
#define PHY_RDDM_SLAVE_DELAY_1_ADDR    342 + PHY_BASE_ADDR // 
#define PHY_RDDM_SLAVE_DELAY_1_OFFSET  16                   // 
#define PHY_RDDM_SLAVE_DELAY_1_WIDTH   9                    // 
#define PHY_RDDM_SLAVE_DELAY_2_ADDR    598 + PHY_BASE_ADDR // 
#define PHY_RDDM_SLAVE_DELAY_2_OFFSET  16                   // 
#define PHY_RDDM_SLAVE_DELAY_2_WIDTH   9                    // 
#define PHY_RDDM_SLAVE_DELAY_3_ADDR    854 + PHY_BASE_ADDR // 
#define PHY_RDDM_SLAVE_DELAY_3_OFFSET  16                   // 
#define PHY_RDDM_SLAVE_DELAY_3_WIDTH   9                    // 
#define PHY_RDDM_SLAVE_DELAY_ADDR      86                   // 
#define PHY_RDDM_SLAVE_DELAY_OFFSET    16                   // 
#define PHY_RDDM_SLAVE_DELAY_WIDTH     9                    // 
#define PHY_RDDQ0_SLAVE_DELAY_0_ADDR   82 + PHY_BASE_ADDR  // 
#define PHY_RDDQ0_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQ0_SLAVE_DELAY_0_WIDTH  9                    // 
#define PHY_RDDQ0_SLAVE_DELAY_1_ADDR   338 + PHY_BASE_ADDR // 
#define PHY_RDDQ0_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQ0_SLAVE_DELAY_1_WIDTH  9                    // 
#define PHY_RDDQ0_SLAVE_DELAY_2_ADDR   594 + PHY_BASE_ADDR // 
#define PHY_RDDQ0_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQ0_SLAVE_DELAY_2_WIDTH  9                    // 
#define PHY_RDDQ0_SLAVE_DELAY_3_ADDR   850 + PHY_BASE_ADDR // 
#define PHY_RDDQ0_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQ0_SLAVE_DELAY_3_WIDTH  9                    // 
#define PHY_RDDQ0_SLAVE_DELAY_ADDR     82                   // 
#define PHY_RDDQ0_SLAVE_DELAY_OFFSET   16                   // 
#define PHY_RDDQ0_SLAVE_DELAY_WIDTH    9                    // 
#define PHY_RDDQ1_SLAVE_DELAY_0_ADDR   83 + PHY_BASE_ADDR  // 
#define PHY_RDDQ1_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQ1_SLAVE_DELAY_0_WIDTH  9                    // 
#define PHY_RDDQ1_SLAVE_DELAY_1_ADDR   339 + PHY_BASE_ADDR // 
#define PHY_RDDQ1_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQ1_SLAVE_DELAY_1_WIDTH  9                    // 
#define PHY_RDDQ1_SLAVE_DELAY_2_ADDR   595 + PHY_BASE_ADDR // 
#define PHY_RDDQ1_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQ1_SLAVE_DELAY_2_WIDTH  9                    // 
#define PHY_RDDQ1_SLAVE_DELAY_3_ADDR   851 + PHY_BASE_ADDR // 
#define PHY_RDDQ1_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQ1_SLAVE_DELAY_3_WIDTH  9                    // 
#define PHY_RDDQ1_SLAVE_DELAY_ADDR     83                   // 
#define PHY_RDDQ1_SLAVE_DELAY_OFFSET   0                    // 
#define PHY_RDDQ1_SLAVE_DELAY_WIDTH    9                    // 
#define PHY_RDDQ2_SLAVE_DELAY_0_ADDR   83 + PHY_BASE_ADDR  // 
#define PHY_RDDQ2_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQ2_SLAVE_DELAY_0_WIDTH  9                    // 
#define PHY_RDDQ2_SLAVE_DELAY_1_ADDR   339 + PHY_BASE_ADDR // 
#define PHY_RDDQ2_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQ2_SLAVE_DELAY_1_WIDTH  9                    // 
#define PHY_RDDQ2_SLAVE_DELAY_2_ADDR   595 + PHY_BASE_ADDR // 
#define PHY_RDDQ2_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQ2_SLAVE_DELAY_2_WIDTH  9                    // 
#define PHY_RDDQ2_SLAVE_DELAY_3_ADDR   851 + PHY_BASE_ADDR // 
#define PHY_RDDQ2_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQ2_SLAVE_DELAY_3_WIDTH  9                    // 
#define PHY_RDDQ2_SLAVE_DELAY_ADDR     83                   // 
#define PHY_RDDQ2_SLAVE_DELAY_OFFSET   16                   // 
#define PHY_RDDQ2_SLAVE_DELAY_WIDTH    9                    // 
#define PHY_RDDQ3_SLAVE_DELAY_0_ADDR   84 + PHY_BASE_ADDR  // 
#define PHY_RDDQ3_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQ3_SLAVE_DELAY_0_WIDTH  9                    // 
#define PHY_RDDQ3_SLAVE_DELAY_1_ADDR   340 + PHY_BASE_ADDR // 
#define PHY_RDDQ3_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQ3_SLAVE_DELAY_1_WIDTH  9                    // 
#define PHY_RDDQ3_SLAVE_DELAY_2_ADDR   596 + PHY_BASE_ADDR // 
#define PHY_RDDQ3_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQ3_SLAVE_DELAY_2_WIDTH  9                    // 
#define PHY_RDDQ3_SLAVE_DELAY_3_ADDR   852 + PHY_BASE_ADDR // 
#define PHY_RDDQ3_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQ3_SLAVE_DELAY_3_WIDTH  9                    // 
#define PHY_RDDQ3_SLAVE_DELAY_ADDR     84                   // 
#define PHY_RDDQ3_SLAVE_DELAY_OFFSET   0                    // 
#define PHY_RDDQ3_SLAVE_DELAY_WIDTH    9                    // 
#define PHY_RDDQ4_SLAVE_DELAY_0_ADDR   84 + PHY_BASE_ADDR  // 
#define PHY_RDDQ4_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQ4_SLAVE_DELAY_0_WIDTH  9                    // 
#define PHY_RDDQ4_SLAVE_DELAY_1_ADDR   340 + PHY_BASE_ADDR // 
#define PHY_RDDQ4_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQ4_SLAVE_DELAY_1_WIDTH  9                    // 
#define PHY_RDDQ4_SLAVE_DELAY_2_ADDR   596 + PHY_BASE_ADDR // 
#define PHY_RDDQ4_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQ4_SLAVE_DELAY_2_WIDTH  9                    // 
#define PHY_RDDQ4_SLAVE_DELAY_3_ADDR   852 + PHY_BASE_ADDR // 
#define PHY_RDDQ4_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQ4_SLAVE_DELAY_3_WIDTH  9                    // 
#define PHY_RDDQ4_SLAVE_DELAY_ADDR     84                   // 
#define PHY_RDDQ4_SLAVE_DELAY_OFFSET   16                   // 
#define PHY_RDDQ4_SLAVE_DELAY_WIDTH    9                    // 
#define PHY_RDDQ5_SLAVE_DELAY_0_ADDR   85 + PHY_BASE_ADDR  // 
#define PHY_RDDQ5_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQ5_SLAVE_DELAY_0_WIDTH  9                    // 
#define PHY_RDDQ5_SLAVE_DELAY_1_ADDR   341 + PHY_BASE_ADDR // 
#define PHY_RDDQ5_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQ5_SLAVE_DELAY_1_WIDTH  9                    // 
#define PHY_RDDQ5_SLAVE_DELAY_2_ADDR   597 + PHY_BASE_ADDR // 
#define PHY_RDDQ5_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQ5_SLAVE_DELAY_2_WIDTH  9                    // 
#define PHY_RDDQ5_SLAVE_DELAY_3_ADDR   853 + PHY_BASE_ADDR // 
#define PHY_RDDQ5_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQ5_SLAVE_DELAY_3_WIDTH  9                    // 
#define PHY_RDDQ5_SLAVE_DELAY_ADDR     85                   // 
#define PHY_RDDQ5_SLAVE_DELAY_OFFSET   0                    // 
#define PHY_RDDQ5_SLAVE_DELAY_WIDTH    9                    // 
#define PHY_RDDQ6_SLAVE_DELAY_0_ADDR   85 + PHY_BASE_ADDR  // 
#define PHY_RDDQ6_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQ6_SLAVE_DELAY_0_WIDTH  9                    // 
#define PHY_RDDQ6_SLAVE_DELAY_1_ADDR   341 + PHY_BASE_ADDR // 
#define PHY_RDDQ6_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQ6_SLAVE_DELAY_1_WIDTH  9                    // 
#define PHY_RDDQ6_SLAVE_DELAY_2_ADDR   597 + PHY_BASE_ADDR // 
#define PHY_RDDQ6_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQ6_SLAVE_DELAY_2_WIDTH  9                    // 
#define PHY_RDDQ6_SLAVE_DELAY_3_ADDR   853 + PHY_BASE_ADDR // 
#define PHY_RDDQ6_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQ6_SLAVE_DELAY_3_WIDTH  9                    // 
#define PHY_RDDQ6_SLAVE_DELAY_ADDR     85                   // 
#define PHY_RDDQ6_SLAVE_DELAY_OFFSET   16                   // 
#define PHY_RDDQ6_SLAVE_DELAY_WIDTH    9                    // 
#define PHY_RDDQ7_SLAVE_DELAY_0_ADDR   86 + PHY_BASE_ADDR  // 
#define PHY_RDDQ7_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQ7_SLAVE_DELAY_0_WIDTH  9                    // 
#define PHY_RDDQ7_SLAVE_DELAY_1_ADDR   342 + PHY_BASE_ADDR // 
#define PHY_RDDQ7_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQ7_SLAVE_DELAY_1_WIDTH  9                    // 
#define PHY_RDDQ7_SLAVE_DELAY_2_ADDR   598 + PHY_BASE_ADDR // 
#define PHY_RDDQ7_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQ7_SLAVE_DELAY_2_WIDTH  9                    // 
#define PHY_RDDQ7_SLAVE_DELAY_3_ADDR   854 + PHY_BASE_ADDR // 
#define PHY_RDDQ7_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQ7_SLAVE_DELAY_3_WIDTH  9                    // 
#define PHY_RDDQ7_SLAVE_DELAY_ADDR     86                   // 
#define PHY_RDDQ7_SLAVE_DELAY_OFFSET   0                    // 
#define PHY_RDDQ7_SLAVE_DELAY_WIDTH    9                    // 
#define PHY_RDDQ_ENC_OBS_SELECT_ADDR   28                   // 
#define PHY_RDDQ_ENC_OBS_SELECT_OFFSET 24                   // 
#define PHY_RDDQ_ENC_OBS_SELECT_WIDTH  4                    // 
#define PHY_RDDQ_SLV_DLY_ENC_OBS_ADDR  41                   // 
#define PHY_RDDQ_SLV_DLY_ENC_OBS_OFFSET 0                    // 
#define PHY_RDDQ_SLV_DLY_ENC_OBS_WIDTH 7                    // 
#define PHY_RDDQS_BASE_SLV_DLY_ENC_OBS_ADDR 41                   // 
#define PHY_RDDQS_BASE_SLV_DLY_ENC_OBS_OFFSET 8                    // 
#define PHY_RDDQS_BASE_SLV_DLY_ENC_OBS_WIDTH 7                    // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_0_ADDR 123 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_1_ADDR 379 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_2_ADDR 635 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_3_ADDR 891 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_ADDR 123                  // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_RDDQS_DM_FALL_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_0_ADDR 122 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_1_ADDR 378 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_2_ADDR 634 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_3_ADDR 890 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_ADDR 122                  // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_RDDQS_DM_RISE_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_0_ADDR 115 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_1_ADDR 371 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_2_ADDR 627 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_3_ADDR 883 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_ADDR 115                  // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_RDDQS_DQ0_FALL_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_0_ADDR 114 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_1_ADDR 370 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_2_ADDR 626 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_3_ADDR 882 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_ADDR 114                  // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_RDDQS_DQ0_RISE_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_0_ADDR 116 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_1_ADDR 372 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_2_ADDR 628 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_3_ADDR 884 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_ADDR 116                  // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_RDDQS_DQ1_FALL_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_0_ADDR 115 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_1_ADDR 371 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_2_ADDR 627 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_3_ADDR 883 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_ADDR 115                  // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_RDDQS_DQ1_RISE_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_0_ADDR 117 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_1_ADDR 373 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_2_ADDR 629 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_3_ADDR 885 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_ADDR 117                  // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_RDDQS_DQ2_FALL_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_0_ADDR 116 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_1_ADDR 372 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_2_ADDR 628 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_3_ADDR 884 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_ADDR 116                  // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_RDDQS_DQ2_RISE_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_0_ADDR 118 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_1_ADDR 374 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_2_ADDR 630 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_3_ADDR 886 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_ADDR 118                  // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_RDDQS_DQ3_FALL_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_0_ADDR 117 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_1_ADDR 373 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_2_ADDR 629 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_3_ADDR 885 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_ADDR 117                  // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_RDDQS_DQ3_RISE_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_0_ADDR 119 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_1_ADDR 375 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_2_ADDR 631 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_3_ADDR 887 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_ADDR 119                  // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_RDDQS_DQ4_FALL_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_0_ADDR 118 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_1_ADDR 374 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_2_ADDR 630 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_3_ADDR 886 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_ADDR 118                  // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_RDDQS_DQ4_RISE_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_0_ADDR 120 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_1_ADDR 376 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_2_ADDR 632 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_3_ADDR 888 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_ADDR 120                  // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_RDDQS_DQ5_FALL_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_0_ADDR 119 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_1_ADDR 375 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_2_ADDR 631 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_3_ADDR 887 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_ADDR 119                  // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_RDDQS_DQ5_RISE_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_0_ADDR 121 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_1_ADDR 377 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_2_ADDR 633 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_3_ADDR 889 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_ADDR 121                  // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_RDDQS_DQ6_FALL_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_0_ADDR 120 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_1_ADDR 376 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_2_ADDR 632 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_3_ADDR 888 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_ADDR 120                  // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_RDDQS_DQ6_RISE_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_0_ADDR 122 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_0_OFFSET 0                    // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_1_ADDR 378 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_1_OFFSET 0                    // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_2_ADDR 634 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_2_OFFSET 0                    // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_3_ADDR 890 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_3_OFFSET 0                    // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_ADDR 122                  // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_RDDQS_DQ7_FALL_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_0_ADDR 121 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_1_ADDR 377 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_2_ADDR 633 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_3_ADDR 889 + PHY_BASE_ADDR // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_ADDR 121                  // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_RDDQS_DQ7_RISE_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ_BYPASS_SLAVE_DELAY_ADDR 16                   // 
#define PHY_RDDQS_DQ_BYPASS_SLAVE_DELAY_OFFSET 0                    // 
#define PHY_RDDQS_DQ_BYPASS_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_DQ_ENC_OBS_SELECT_ADDR 29                   // 
#define PHY_RDDQS_DQ_ENC_OBS_SELECT_OFFSET 0                    // 
#define PHY_RDDQS_DQ_ENC_OBS_SELECT_WIDTH 4                    // 
#define PHY_RDDQS_DQ_FALL_ADDER_SLV_DLY_ENC_OBS_ADDR 42                   // 
#define PHY_RDDQS_DQ_FALL_ADDER_SLV_DLY_ENC_OBS_OFFSET 0                    // 
#define PHY_RDDQS_DQ_FALL_ADDER_SLV_DLY_ENC_OBS_WIDTH 8                    // 
#define PHY_RDDQS_DQ_RISE_ADDER_SLV_DLY_ENC_OBS_ADDR 41                   // 
#define PHY_RDDQS_DQ_RISE_ADDER_SLV_DLY_ENC_OBS_OFFSET 24                   // 
#define PHY_RDDQS_DQ_RISE_ADDER_SLV_DLY_ENC_OBS_WIDTH 8                    // 
#define PHY_RDDQS_GATE_BYPASS_SLAVE_DELAY_ADDR 2                    // 
#define PHY_RDDQS_GATE_BYPASS_SLAVE_DELAY_OFFSET 8                    // 
#define PHY_RDDQS_GATE_BYPASS_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_0_ADDR 123 + PHY_BASE_ADDR // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_0_OFFSET 16                   // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_0_WIDTH 9                    // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_1_ADDR 379 + PHY_BASE_ADDR // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_1_OFFSET 16                   // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_1_WIDTH 9                    // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_2_ADDR 635 + PHY_BASE_ADDR // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_2_OFFSET 16                   // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_2_WIDTH 9                    // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_3_ADDR 891 + PHY_BASE_ADDR // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_3_OFFSET 16                   // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_3_WIDTH 9                    // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_ADDR 123                  // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_OFFSET 16                   // 
#define PHY_RDDQS_GATE_SLAVE_DELAY_WIDTH 9                    // 
#define PHY_RDDQS_GATE_SLV_DLY_ENC_OBS_ADDR 42                   // 
#define PHY_RDDQS_GATE_SLV_DLY_ENC_OBS_OFFSET 8                    // 
#define PHY_RDDQS_GATE_SLV_DLY_ENC_OBS_WIDTH 11                   // 
#define PHY_RDDQS_LATENCY_ADJUST_0_ADDR 124 + PHY_BASE_ADDR // 
#define PHY_RDDQS_LATENCY_ADJUST_0_OFFSET 0                    // 
#define PHY_RDDQS_LATENCY_ADJUST_0_WIDTH 5                    // 
#define PHY_RDDQS_LATENCY_ADJUST_1_ADDR 380 + PHY_BASE_ADDR // 
#define PHY_RDDQS_LATENCY_ADJUST_1_OFFSET 0                    // 
#define PHY_RDDQS_LATENCY_ADJUST_1_WIDTH 5                    // 
#define PHY_RDDQS_LATENCY_ADJUST_2_ADDR 636 + PHY_BASE_ADDR // 
#define PHY_RDDQS_LATENCY_ADJUST_2_OFFSET 0                    // 
#define PHY_RDDQS_LATENCY_ADJUST_2_WIDTH 5                    // 
#define PHY_RDDQS_LATENCY_ADJUST_3_ADDR 892 + PHY_BASE_ADDR // 
#define PHY_RDDQS_LATENCY_ADJUST_3_OFFSET 0                    // 
#define PHY_RDDQS_LATENCY_ADJUST_3_WIDTH 5                    // 
#define PHY_RDDQS_LATENCY_ADJUST_ADDR  124                  // 
#define PHY_RDDQS_LATENCY_ADJUST_OFFSET 0                    // 
#define PHY_RDDQS_LATENCY_ADJUST_WIDTH 5                    // 
#define PHY_RDLVL_BEST_THRSHLD_ADDR    13                   // 
#define PHY_RDLVL_BEST_THRSHLD_OFFSET  24                   // 
#define PHY_RDLVL_BEST_THRSHLD_WIDTH   3                    // 
#define PHY_RDLVL_BEST_VREF_LOWER_OBS_ADDR 14                   // 
#define PHY_RDLVL_BEST_VREF_LOWER_OBS_OFFSET 24                   // 
#define PHY_RDLVL_BEST_VREF_LOWER_OBS_WIDTH 7                    // 
#define PHY_RDLVL_BEST_VREF_UPPER_OBS_ADDR 15                   // 
#define PHY_RDLVL_BEST_VREF_UPPER_OBS_OFFSET 0                    // 
#define PHY_RDLVL_BEST_VREF_UPPER_OBS_WIDTH 12                   // 
#define PHY_RDLVL_CAPTURE_CNT_0_ADDR   32 + PHY_BASE_ADDR  // 
#define PHY_RDLVL_CAPTURE_CNT_0_OFFSET 24                   // 
#define PHY_RDLVL_CAPTURE_CNT_0_WIDTH  6                    // 
#define PHY_RDLVL_CAPTURE_CNT_1_ADDR   288 + PHY_BASE_ADDR // 
#define PHY_RDLVL_CAPTURE_CNT_1_OFFSET 24                   // 
#define PHY_RDLVL_CAPTURE_CNT_1_WIDTH  6                    // 
#define PHY_RDLVL_CAPTURE_CNT_2_ADDR   544 + PHY_BASE_ADDR // 
#define PHY_RDLVL_CAPTURE_CNT_2_OFFSET 24                   // 
#define PHY_RDLVL_CAPTURE_CNT_2_WIDTH  6                    // 
#define PHY_RDLVL_CAPTURE_CNT_3_ADDR   800 + PHY_BASE_ADDR // 
#define PHY_RDLVL_CAPTURE_CNT_3_OFFSET 24                   // 
#define PHY_RDLVL_CAPTURE_CNT_3_WIDTH  6                    // 
#define PHY_RDLVL_CAPTURE_CNT_ADDR     32                   // 
#define PHY_RDLVL_CAPTURE_CNT_OFFSET   24                   // 
#define PHY_RDLVL_CAPTURE_CNT_WIDTH    6                    // 
#define PHY_RDLVL_CLK_JITTER_TOLERANCE_ADDR 50                   // 
#define PHY_RDLVL_CLK_JITTER_TOLERANCE_OFFSET 0                    // 
#define PHY_RDLVL_CLK_JITTER_TOLERANCE_WIDTH 9                    // 
#define PHY_RDLVL_DATA_MASK_ADDR       34                   // 
#define PHY_RDLVL_DATA_MASK_OFFSET     0                    // 
#define PHY_RDLVL_DATA_MASK_WIDTH      8                    // 
#define PHY_RDLVL_DLY_STEP_ADDR        100                  // 
#define PHY_RDLVL_DLY_STEP_OFFSET      8                    // 
#define PHY_RDLVL_DLY_STEP_WIDTH       4                    // 
#define PHY_RDLVL_DVW_MIN_ADDR         102                  // 
#define PHY_RDLVL_DVW_MIN_OFFSET       0                    // 
#define PHY_RDLVL_DVW_MIN_WIDTH        9                    // 
#define PHY_RDLVL_FINAL_VREF_OFFSET_ADDR 75                   // 
#define PHY_RDLVL_FINAL_VREF_OFFSET_OFFSET 24                   // 
#define PHY_RDLVL_FINAL_VREF_OFFSET_WIDTH 4                    // 
#define PHY_RDLVL_MAX_EDGE_ADDR        101                  // 
#define PHY_RDLVL_MAX_EDGE_OFFSET      0                    // 
#define PHY_RDLVL_MAX_EDGE_WIDTH       9                    // 
#define PHY_RDLVL_OP_MODE_ADDR         33                   // 
#define PHY_RDLVL_OP_MODE_OFFSET       8                    // 
#define PHY_RDLVL_OP_MODE_WIDTH        2                    // 
#define PHY_RDLVL_PATT8_ADDR           20                   // 
#define PHY_RDLVL_PATT8_OFFSET         0                    // 
#define PHY_RDLVL_PATT8_WIDTH          32                   // 
#define PHY_RDLVL_PATT9_ADDR           21                   // 
#define PHY_RDLVL_PATT9_OFFSET         0                    // 
#define PHY_RDLVL_PATT9_WIDTH          32                   // 
#define PHY_RDLVL_PATT10_ADDR          22                   // 
#define PHY_RDLVL_PATT10_OFFSET        0                    // 
#define PHY_RDLVL_PATT10_WIDTH         32                   // 
#define PHY_RDLVL_PATT11_ADDR          23                   // 
#define PHY_RDLVL_PATT11_OFFSET        0                    // 
#define PHY_RDLVL_PATT11_WIDTH         32                   // 
#define PHY_RDLVL_PATT12_ADDR          24                   // 
#define PHY_RDLVL_PATT12_OFFSET        0                    // 
#define PHY_RDLVL_PATT12_WIDTH         32                   // 
#define PHY_RDLVL_PATT13_ADDR          25                   // 
#define PHY_RDLVL_PATT13_OFFSET        0                    // 
#define PHY_RDLVL_PATT13_WIDTH         32                   // 
#define PHY_RDLVL_PATT14_ADDR          26                   // 
#define PHY_RDLVL_PATT14_OFFSET        0                    // 
#define PHY_RDLVL_PATT14_WIDTH         32                   // 
#define PHY_RDLVL_PATT15_ADDR          27                   // 
#define PHY_RDLVL_PATT15_OFFSET        0                    // 
#define PHY_RDLVL_PATT15_WIDTH         32                   // 
#define PHY_RDLVL_PER_START_OFFSET_ADDR 102                  // 
#define PHY_RDLVL_PER_START_OFFSET_OFFSET 24                   // 
#define PHY_RDLVL_PER_START_OFFSET_WIDTH 5                    // 
#define PHY_RDLVL_PER_VREF_THRSHLD_ADDR 14                   // 
#define PHY_RDLVL_PER_VREF_THRSHLD_OFFSET 0                    // 
#define PHY_RDLVL_PER_VREF_THRSHLD_WIDTH 6                    // 
#define PHY_RDLVL_PERIODIC_OBS_ADDR    53                   // 
#define PHY_RDLVL_PERIODIC_OBS_OFFSET  0                    // 
#define PHY_RDLVL_PERIODIC_OBS_SELECT_ADDR 33                   // 
#define PHY_RDLVL_PERIODIC_OBS_SELECT_OFFSET 24                   // 
#define PHY_RDLVL_PERIODIC_OBS_SELECT_WIDTH 8                    // 
#define PHY_RDLVL_PERIODIC_OBS_WIDTH   32                   // 
#define PHY_RDLVL_RDDQS_DQ_LE_DLY_OBS_ADDR 50                   // 
#define PHY_RDLVL_RDDQS_DQ_LE_DLY_OBS_OFFSET 16                   // 
#define PHY_RDLVL_RDDQS_DQ_LE_DLY_OBS_WIDTH 9                    // 
#define PHY_RDLVL_RDDQS_DQ_OBS_SELECT_ADDR 33                   // 
#define PHY_RDLVL_RDDQS_DQ_OBS_SELECT_OFFSET 16                   // 
#define PHY_RDLVL_RDDQS_DQ_OBS_SELECT_WIDTH 5                    // 
#define PHY_RDLVL_RDDQS_DQ_SLV_DLY_START_ADDR 127                  // 
#define PHY_RDLVL_RDDQS_DQ_SLV_DLY_START_OFFSET 0                    // 
#define PHY_RDLVL_RDDQS_DQ_SLV_DLY_START_WIDTH 9                    // 
#define PHY_RDLVL_RDDQS_DQ_TE_DLY_OBS_ADDR 51                   // 
#define PHY_RDLVL_RDDQS_DQ_TE_DLY_OBS_OFFSET 0                    // 
#define PHY_RDLVL_RDDQS_DQ_TE_DLY_OBS_WIDTH 9                    // 
#define PHY_RDLVL_STATUS_OBS_ADDR      52                   // 
#define PHY_RDLVL_STATUS_OBS_OFFSET    0                    // 
#define PHY_RDLVL_STATUS_OBS_WIDTH     32                   // 
#define PHY_RDLVL_UPDT_WAIT_CNT_ADDR   33                   // 
#define PHY_RDLVL_UPDT_WAIT_CNT_OFFSET 0                    // 
#define PHY_RDLVL_UPDT_WAIT_CNT_WIDTH  4                    // 
#define PHY_RDLVL_VREF_DELTA_ADDR      14                   // 
#define PHY_RDLVL_VREF_DELTA_OFFSET    16                   // 
#define PHY_RDLVL_VREF_DELTA_WIDTH     4                    // 
#define PHY_RDLVL_VREF_OUTLIER_ADDR    14                   // 
#define PHY_RDLVL_VREF_OUTLIER_OFFSET  8                    // 
#define PHY_RDLVL_VREF_OUTLIER_WIDTH   3                    // 
#define PHY_RDPATH_GATE_DISABLE_ADDR   71                   // 
#define PHY_RDPATH_GATE_DISABLE_OFFSET 24                   // 
#define PHY_RDPATH_GATE_DISABLE_WIDTH  1                    // 
#define PHY_REG_DATA_WIDTH             32                   // 
#define PHY_REG_MASK_WIDTH             4                    // 
#define PHY_RPTR_RESET_ENABLE_ADDR     1 + PHY_AC_BASE_ADDR // 
#define PHY_RPTR_RESET_ENABLE_OFFSET   24                   // 
#define PHY_RPTR_RESET_ENABLE_WIDTH    1                    // 
#define PHY_RPTR_UPDATE_0_ADDR         96 + PHY_BASE_ADDR  // 
#define PHY_RPTR_UPDATE_0_OFFSET       16                   // 
#define PHY_RPTR_UPDATE_0_WIDTH        4                    // 
#define PHY_RPTR_UPDATE_1_ADDR         352 + PHY_BASE_ADDR // 
#define PHY_RPTR_UPDATE_1_OFFSET       16                   // 
#define PHY_RPTR_UPDATE_1_WIDTH        4                    // 
#define PHY_RPTR_UPDATE_2_ADDR         608 + PHY_BASE_ADDR // 
#define PHY_RPTR_UPDATE_2_OFFSET       16                   // 
#define PHY_RPTR_UPDATE_2_WIDTH        4                    // 
#define PHY_RPTR_UPDATE_3_ADDR         864 + PHY_BASE_ADDR // 
#define PHY_RPTR_UPDATE_3_OFFSET       16                   // 
#define PHY_RPTR_UPDATE_3_WIDTH        4                    // 
#define PHY_RPTR_UPDATE_ADDR           96                   // 
#define PHY_RPTR_UPDATE_OFFSET         16                   // 
#define PHY_RPTR_UPDATE_WIDTH          4                    // 
#define PHY_RX_CAL_ALL_DLY_ADDR        87                   // 
#define PHY_RX_CAL_ALL_DLY_BYPASS_ADDR 1                    // 
#define PHY_RX_CAL_ALL_DLY_BYPASS_OFFSET 24                   // 
#define PHY_RX_CAL_ALL_DLY_BYPASS_WIDTH 6                    // 
#define PHY_RX_CAL_ALL_DLY_OFFSET      8                    // 
#define PHY_RX_CAL_ALL_DLY_WIDTH       6                    // 
#define PHY_RX_CAL_DM_ADDR             66                   // 
#define PHY_RX_CAL_DM_OFFSET           0                    // 
#define PHY_RX_CAL_DM_WIDTH            18                   // 
#define PHY_RX_CAL_DQ0_ADDR            61                   // 
#define PHY_RX_CAL_DQ0_OFFSET          16                   // 
#define PHY_RX_CAL_DQ0_WIDTH           9                    // 
#define PHY_RX_CAL_DQ1_ADDR            62                   // 
#define PHY_RX_CAL_DQ1_OFFSET          0                    // 
#define PHY_RX_CAL_DQ1_WIDTH           9                    // 
#define PHY_RX_CAL_DQ2_ADDR            62                   // 
#define PHY_RX_CAL_DQ2_OFFSET          16                   // 
#define PHY_RX_CAL_DQ2_WIDTH           9                    // 
#define PHY_RX_CAL_DQ3_ADDR            63                   // 
#define PHY_RX_CAL_DQ3_OFFSET          0                    // 
#define PHY_RX_CAL_DQ3_WIDTH           9                    // 
#define PHY_RX_CAL_DQ4_ADDR            63                   // 
#define PHY_RX_CAL_DQ4_OFFSET          16                   // 
#define PHY_RX_CAL_DQ4_WIDTH           9                    // 
#define PHY_RX_CAL_DQ5_ADDR            64                   // 
#define PHY_RX_CAL_DQ5_OFFSET          0                    // 
#define PHY_RX_CAL_DQ5_WIDTH           9                    // 
#define PHY_RX_CAL_DQ6_ADDR            64                   // 
#define PHY_RX_CAL_DQ6_OFFSET          16                   // 
#define PHY_RX_CAL_DQ6_WIDTH           9                    // 
#define PHY_RX_CAL_DQ7_ADDR            65                   // 
#define PHY_RX_CAL_DQ7_OFFSET          0                    // 
#define PHY_RX_CAL_DQ7_WIDTH           9                    // 
#define PHY_RX_CAL_DQS_ADDR            67                   // 
#define PHY_RX_CAL_DQS_OFFSET          0                    // 
#define PHY_RX_CAL_DQS_WIDTH           9                    // 
#define PHY_RX_CAL_FDBK_ADDR           67                   // 
#define PHY_RX_CAL_FDBK_OFFSET         16                   // 
#define PHY_RX_CAL_FDBK_WIDTH          9                    // 
#define PHY_RX_PCLK_CLK_SEL_ADDR       87                   // 
#define PHY_RX_PCLK_CLK_SEL_OFFSET     16                   // 
#define PHY_RX_PCLK_CLK_SEL_WIDTH      3                    // 
#define PHY_SET_DFI_INPUT_0_ADDR       11 + PHY_AC_BASE_ADDR // 
#define PHY_SET_DFI_INPUT_0_OFFSET     0                    // 
#define PHY_SET_DFI_INPUT_0_WIDTH      4                    // 
#define PHY_SET_DFI_INPUT_1_ADDR       11 + PHY_AC_BASE_ADDR // 
#define PHY_SET_DFI_INPUT_1_OFFSET     8                    // 
#define PHY_SET_DFI_INPUT_1_WIDTH      4                    // 
#define PHY_SET_DFI_INPUT_2_ADDR       11 + PHY_AC_BASE_ADDR // 
#define PHY_SET_DFI_INPUT_2_OFFSET     16                   // 
#define PHY_SET_DFI_INPUT_2_WIDTH      4                    // 
#define PHY_SET_DFI_INPUT_3_ADDR       11 + PHY_AC_BASE_ADDR // 
#define PHY_SET_DFI_INPUT_3_OFFSET     24                   // 
#define PHY_SET_DFI_INPUT_3_WIDTH      4                    // 
#define PHY_SET_DFI_INPUT_RST_PAD_ADDR 12 + PHY_AC_BASE_ADDR // 
#define PHY_SET_DFI_INPUT_RST_PAD_OFFSET 0                    // 
#define PHY_SET_DFI_INPUT_RST_PAD_WIDTH 1                    // 
#define PHY_SLAVE_LOOP_CNT_UPDATE_ADDR 28                   // 
#define PHY_SLAVE_LOOP_CNT_UPDATE_OFFSET 0                    // 
#define PHY_SLAVE_LOOP_CNT_UPDATE_WIDTH 3                    // 
#define PHY_SLICE_MULTICAST_EN_ADDR    87 + PHY_AC_BASE_ADDR // 
#define PHY_SLICE_MULTICAST_EN_OFFSET  0                    // 
#define PHY_SLICE_MULTICAST_EN_WIDTH   1                    // 
#define PHY_SLICE_PWR_RDC_DISABLE_ADDR 72                   // 
#define PHY_SLICE_PWR_RDC_DISABLE_OFFSET 8                    // 
#define PHY_SLICE_PWR_RDC_DISABLE_WIDTH 1                    // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_0_ADDR 71 + PHY_BASE_ADDR  // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_0_OFFSET 16                   // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_0_WIDTH 1                    // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_1_ADDR 327 + PHY_BASE_ADDR // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_1_OFFSET 16                   // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_1_WIDTH 1                    // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_2_ADDR 583 + PHY_BASE_ADDR // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_2_OFFSET 16                   // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_2_WIDTH 1                    // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_3_ADDR 839 + PHY_BASE_ADDR // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_3_OFFSET 16                   // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_3_WIDTH 1                    // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_ADDR 71                   // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_OFFSET 16                   // 
#define PHY_SLV_DLY_CTRL_GATE_DISABLE_WIDTH 1                    // 
#define PHY_STATIC_TOG_CONTROL_ADDR    6 + PHY_AC_BASE_ADDR // 
#define PHY_STATIC_TOG_CONTROL_OFFSET  16                   // 
#define PHY_STATIC_TOG_CONTROL_WIDTH   16                   // 
#define PHY_STATIC_TOG_DISABLE_ADDR    68                   // 
#define PHY_STATIC_TOG_DISABLE_OFFSET  16                   // 
#define PHY_STATIC_TOG_DISABLE_WIDTH   5                    // 
#define PHY_SW_AC_BYPASS_SHIFT_ADDR    1067 + PHY_BASE_ADDR // 
#define PHY_SW_AC_BYPASS_SHIFT_OFFSET  16                   // 
#define PHY_SW_AC_BYPASS_SHIFT_WIDTH   6                    // 
#define PHY_SW_ACBIT0_SHIFT_0_ADDR     1038 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT0_SHIFT_0_OFFSET   0                    // 
#define PHY_SW_ACBIT0_SHIFT_0_WIDTH    6                    // 
#define PHY_SW_ACBIT0_SHIFT_1_ADDR     1039 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT0_SHIFT_1_OFFSET   0                    // 
#define PHY_SW_ACBIT0_SHIFT_1_WIDTH    6                    // 
#define PHY_SW_ACBIT0_SHIFT_2_ADDR     1040 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT0_SHIFT_2_OFFSET   0                    // 
#define PHY_SW_ACBIT0_SHIFT_2_WIDTH    6                    // 
#define PHY_SW_ACBIT0_SHIFT_3_ADDR     1041 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT0_SHIFT_3_OFFSET   0                    // 
#define PHY_SW_ACBIT0_SHIFT_3_WIDTH    6                    // 
#define PHY_SW_ACBIT1_SHIFT_0_ADDR     1038 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT1_SHIFT_0_OFFSET   8                    // 
#define PHY_SW_ACBIT1_SHIFT_0_WIDTH    6                    // 
#define PHY_SW_ACBIT1_SHIFT_1_ADDR     1039 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT1_SHIFT_1_OFFSET   8                    // 
#define PHY_SW_ACBIT1_SHIFT_1_WIDTH    6                    // 
#define PHY_SW_ACBIT1_SHIFT_2_ADDR     1040 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT1_SHIFT_2_OFFSET   8                    // 
#define PHY_SW_ACBIT1_SHIFT_2_WIDTH    6                    // 
#define PHY_SW_ACBIT1_SHIFT_3_ADDR     1041 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT1_SHIFT_3_OFFSET   8                    // 
#define PHY_SW_ACBIT1_SHIFT_3_WIDTH    6                    // 
#define PHY_SW_ACBIT2_SHIFT_0_ADDR     1038 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT2_SHIFT_0_OFFSET   16                   // 
#define PHY_SW_ACBIT2_SHIFT_0_WIDTH    6                    // 
#define PHY_SW_ACBIT2_SHIFT_1_ADDR     1039 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT2_SHIFT_1_OFFSET   16                   // 
#define PHY_SW_ACBIT2_SHIFT_1_WIDTH    6                    // 
#define PHY_SW_ACBIT2_SHIFT_2_ADDR     1040 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT2_SHIFT_2_OFFSET   16                   // 
#define PHY_SW_ACBIT2_SHIFT_2_WIDTH    6                    // 
#define PHY_SW_ACBIT2_SHIFT_3_ADDR     1041 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT2_SHIFT_3_OFFSET   16                   // 
#define PHY_SW_ACBIT2_SHIFT_3_WIDTH    6                    // 
#define PHY_SW_ACBIT3_SHIFT_0_ADDR     1038 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT3_SHIFT_0_OFFSET   24                   // 
#define PHY_SW_ACBIT3_SHIFT_0_WIDTH    6                    // 
#define PHY_SW_ACBIT3_SHIFT_1_ADDR     1039 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT3_SHIFT_1_OFFSET   24                   // 
#define PHY_SW_ACBIT3_SHIFT_1_WIDTH    6                    // 
#define PHY_SW_ACBIT3_SHIFT_2_ADDR     1040 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT3_SHIFT_2_OFFSET   24                   // 
#define PHY_SW_ACBIT3_SHIFT_2_WIDTH    6                    // 
#define PHY_SW_ACBIT3_SHIFT_3_ADDR     1041 + PHY_BASE_ADDR // 
#define PHY_SW_ACBIT3_SHIFT_3_OFFSET   24                   // 
#define PHY_SW_ACBIT3_SHIFT_3_WIDTH    6                    // 
#define PHY_SW_CSLVL_DVW_MIN_ADDR      1091 + PHY_BASE_ADDR // 
#define PHY_SW_CSLVL_DVW_MIN_EN_ADDR   1091 + PHY_BASE_ADDR // 
#define PHY_SW_CSLVL_DVW_MIN_EN_OFFSET 16                   // 
#define PHY_SW_CSLVL_DVW_MIN_EN_WIDTH  1                    // 
#define PHY_SW_CSLVL_DVW_MIN_OFFSET    0                    // 
#define PHY_SW_CSLVL_DVW_MIN_WIDTH     9                    // 
#define PHY_SW_FIFO_PTR_RST_DISABLE_ADDR 28                   // 
#define PHY_SW_FIFO_PTR_RST_DISABLE_OFFSET 8                    // 
#define PHY_SW_FIFO_PTR_RST_DISABLE_WIDTH 1                    // 
#define PHY_SW_MASTER_MODE_ADDR        94                   // 
#define PHY_SW_MASTER_MODE_OFFSET      8                    // 
#define PHY_SW_MASTER_MODE_WIDTH       4                    // 
#define PHY_SW_PLL_BYPASS_ADDR         10 + PHY_AC_BASE_ADDR // 
#define PHY_SW_PLL_BYPASS_OFFSET       0                    // 
#define PHY_SW_PLL_BYPASS_WIDTH        1                    // 
#define PHY_SW_RDLVL_DVW_MIN_EN_ADDR   102                  // 
#define PHY_SW_RDLVL_DVW_MIN_EN_OFFSET 16                   // 
#define PHY_SW_RDLVL_DVW_MIN_EN_WIDTH  1                    // 
#define PHY_SW_TXIO_CTRL_0_ADDR        1046 + PHY_BASE_ADDR // 
#define PHY_SW_TXIO_CTRL_0_OFFSET      24                   // 
#define PHY_SW_TXIO_CTRL_0_WIDTH       4                    // 
#define PHY_SW_TXIO_CTRL_1_ADDR        1047 + PHY_BASE_ADDR // 
#define PHY_SW_TXIO_CTRL_1_OFFSET      8                    // 
#define PHY_SW_TXIO_CTRL_1_WIDTH       4                    // 
#define PHY_SW_TXIO_CTRL_2_ADDR        1048 + PHY_BASE_ADDR // 
#define PHY_SW_TXIO_CTRL_2_OFFSET      24                   // 
#define PHY_SW_TXIO_CTRL_2_WIDTH       4                    // 
#define PHY_SW_TXIO_CTRL_3_ADDR        1050 + PHY_BASE_ADDR // 
#define PHY_SW_TXIO_CTRL_3_OFFSET      8                    // 
#define PHY_SW_TXIO_CTRL_3_WIDTH       4                    // 
#define PHY_SW_TXIO_CTRL_WIDTH         4                    // 
#define PHY_SW_WDQLVL_DVW_MIN_EN_ADDR  78                   // 
#define PHY_SW_WDQLVL_DVW_MIN_EN_OFFSET 16                   // 
#define PHY_SW_WDQLVL_DVW_MIN_EN_WIDTH 1                    // 
#define PHY_SW_WRDM_SHIFT_ADDR         5                    // 
#define PHY_SW_WRDM_SHIFT_OFFSET       8                    // 
#define PHY_SW_WRDM_SHIFT_WIDTH        6                    // 
#define PHY_SW_WRDQ0_SHIFT_ADDR        3                    // 
#define PHY_SW_WRDQ0_SHIFT_OFFSET      8                    // 
#define PHY_SW_WRDQ0_SHIFT_WIDTH       6                    // 
#define PHY_SW_WRDQ1_SHIFT_ADDR        3                    // 
#define PHY_SW_WRDQ1_SHIFT_OFFSET      16                   // 
#define PHY_SW_WRDQ1_SHIFT_WIDTH       6                    // 
#define PHY_SW_WRDQ2_SHIFT_ADDR        3                    // 
#define PHY_SW_WRDQ2_SHIFT_OFFSET      24                   // 
#define PHY_SW_WRDQ2_SHIFT_WIDTH       6                    // 
#define PHY_SW_WRDQ3_SHIFT_ADDR        4                    // 
#define PHY_SW_WRDQ3_SHIFT_OFFSET      0                    // 
#define PHY_SW_WRDQ3_SHIFT_WIDTH       6                    // 
#define PHY_SW_WRDQ4_SHIFT_ADDR        4                    // 
#define PHY_SW_WRDQ4_SHIFT_OFFSET      8                    // 
#define PHY_SW_WRDQ4_SHIFT_WIDTH       6                    // 
#define PHY_SW_WRDQ5_SHIFT_ADDR        4                    // 
#define PHY_SW_WRDQ5_SHIFT_OFFSET      16                   // 
#define PHY_SW_WRDQ5_SHIFT_WIDTH       6                    // 
#define PHY_SW_WRDQ6_SHIFT_ADDR        4                    // 
#define PHY_SW_WRDQ6_SHIFT_OFFSET      24                   // 
#define PHY_SW_WRDQ6_SHIFT_WIDTH       6                    // 
#define PHY_SW_WRDQ7_SHIFT_ADDR        5                    // 
#define PHY_SW_WRDQ7_SHIFT_OFFSET      0                    // 
#define PHY_SW_WRDQ7_SHIFT_WIDTH       6                    // 
#define PHY_SW_WRDQS_SHIFT_ADDR        5                    // 
#define PHY_SW_WRDQS_SHIFT_OFFSET      16                   // 
#define PHY_SW_WRDQS_SHIFT_WIDTH       4                    // 
#define PHY_TCKSRE_WAIT_ADDR           21 + PHY_AC_BASE_ADDR // 
#define PHY_TCKSRE_WAIT_OFFSET         8                    // 
#define PHY_TCKSRE_WAIT_WIDTH          4                    // 
#define PHY_TOGGLE_PRE_SUPPORT_ADDR    100                  // 
#define PHY_TOGGLE_PRE_SUPPORT_OFFSET  0                    // 
#define PHY_TOGGLE_PRE_SUPPORT_WIDTH   1                    // 
#define PHY_TOP_PWR_RDC_DISABLE_ADDR   5 + PHY_AC_BASE_ADDR // 
#define PHY_TOP_PWR_RDC_DISABLE_OFFSET 24                   // 
#define PHY_TOP_PWR_RDC_DISABLE_WIDTH  1                    // 
#define PHY_TOP_STATIC_TOG_DISABLE_ADDR 6 + PHY_AC_BASE_ADDR // 
#define PHY_TOP_STATIC_TOG_DISABLE_OFFSET 0                    // 
#define PHY_TOP_STATIC_TOG_DISABLE_WIDTH 1                    // 
#define PHY_TWO_CYC_PREAMBLE_ADDR      74                   // 
#define PHY_TWO_CYC_PREAMBLE_OFFSET    16                   // 
#define PHY_TWO_CYC_PREAMBLE_WIDTH     2                    // 
#define PHY_UPDATE_MASK_ADDR           63 + PHY_AC_BASE_ADDR // 
#define PHY_UPDATE_MASK_OFFSET         0                    // 
#define PHY_UPDATE_MASK_WIDTH          1                    // 
#define PHY_USE_PLL_DSKEWCALLOCK_ADDR  13 + PHY_AC_BASE_ADDR // 
#define PHY_USE_PLL_DSKEWCALLOCK_OFFSET 16                   // 
#define PHY_USE_PLL_DSKEWCALLOCK_WIDTH 1                    // 
#define PHY_VREF_INITIAL_START_POINT_ADDR 75                   // 
#define PHY_VREF_INITIAL_START_POINT_OFFSET 0                    // 
#define PHY_VREF_INITIAL_START_POINT_WIDTH 7                    // 
#define PHY_VREF_INITIAL_STEPSIZE_ADDR 13                   // 
#define PHY_VREF_INITIAL_STEPSIZE_OFFSET 16                   // 
#define PHY_VREF_INITIAL_STEPSIZE_WIDTH 6                    // 
#define PHY_VREF_INITIAL_STOP_POINT_ADDR 75                   // 
#define PHY_VREF_INITIAL_STOP_POINT_OFFSET 8                    // 
#define PHY_VREF_INITIAL_STOP_POINT_WIDTH 7                    // 
#define PHY_VREF_SETTING_TIME_ADDR     91                   // 
#define PHY_VREF_SETTING_TIME_OFFSET   0                    // 
#define PHY_VREF_SETTING_TIME_WIDTH    16                   // 
#define PHY_VREF_TRAIN_OBS_ADDR        15                   // 
#define PHY_VREF_TRAIN_OBS_OFFSET      16                   // 
#define PHY_VREF_TRAIN_OBS_WIDTH       7                    // 
#define PHY_VREF_TRAINING_CTRL_0_ADDR  75 + PHY_BASE_ADDR  // 
#define PHY_VREF_TRAINING_CTRL_0_OFFSET 16                   // 
#define PHY_VREF_TRAINING_CTRL_0_WIDTH 2                    // 
#define PHY_VREF_TRAINING_CTRL_1_ADDR  331 + PHY_BASE_ADDR // 
#define PHY_VREF_TRAINING_CTRL_1_OFFSET 16                   // 
#define PHY_VREF_TRAINING_CTRL_1_WIDTH 2                    // 
#define PHY_VREF_TRAINING_CTRL_2_ADDR  587 + PHY_BASE_ADDR // 
#define PHY_VREF_TRAINING_CTRL_2_OFFSET 16                   // 
#define PHY_VREF_TRAINING_CTRL_2_WIDTH 2                    // 
#define PHY_VREF_TRAINING_CTRL_3_ADDR  843 + PHY_BASE_ADDR // 
#define PHY_VREF_TRAINING_CTRL_3_OFFSET 16                   // 
#define PHY_VREF_TRAINING_CTRL_3_WIDTH 2                    // 
#define PHY_VREF_TRAINING_CTRL_ADDR    75                   // 
#define PHY_VREF_TRAINING_CTRL_OFFSET  16                   // 
#define PHY_VREF_TRAINING_CTRL_WIDTH   2                    // 
#define PHY_WDQ_OSC_DELTA_ADDR         106                  // 
#define PHY_WDQ_OSC_DELTA_OFFSET       0                    // 
#define PHY_WDQ_OSC_DELTA_WIDTH        7                    // 
#define PHY_WDQLVL_BEST_THRSHLD_ADDR   36                   // 
#define PHY_WDQLVL_BEST_THRSHLD_OFFSET 8                    // 
#define PHY_WDQLVL_BEST_THRSHLD_WIDTH  3                    // 
#define PHY_WDQLVL_BURST_CNT_ADDR      34                   // 
#define PHY_WDQLVL_BURST_CNT_OFFSET    16                   // 
#define PHY_WDQLVL_BURST_CNT_WIDTH     6                    // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_0_ADDR 34 + PHY_BASE_ADDR  // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_0_OFFSET 8                    // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_0_WIDTH 8                    // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_1_ADDR 290 + PHY_BASE_ADDR // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_1_OFFSET 8                    // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_1_WIDTH 8                    // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_2_ADDR 546 + PHY_BASE_ADDR // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_2_OFFSET 8                    // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_2_WIDTH 8                    // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_3_ADDR 802 + PHY_BASE_ADDR // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_3_OFFSET 8                    // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_3_WIDTH 8                    // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_ADDR 34                   // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_OFFSET 8                    // 
#define PHY_WDQLVL_CLK_JITTER_TOLERANCE_WIDTH 8                    // 
#define PHY_WDQLVL_DATADM_MASK_ADDR    37                   // 
#define PHY_WDQLVL_DATADM_MASK_OFFSET  0                    // 
#define PHY_WDQLVL_DATADM_MASK_WIDTH   9                    // 
#define PHY_WDQLVL_DLY_STEP_ADDR       99                   // 
#define PHY_WDQLVL_DLY_STEP_OFFSET     16                   // 
#define PHY_WDQLVL_DLY_STEP_WIDTH      8                    // 
#define PHY_WDQLVL_DQDM_LE_DLY_OBS_ADDR 54                   // 
#define PHY_WDQLVL_DQDM_LE_DLY_OBS_OFFSET 0                    // 
#define PHY_WDQLVL_DQDM_LE_DLY_OBS_WIDTH 11                   // 
#define PHY_WDQLVL_DQDM_OBS_SELECT_ADDR 35                   // 
#define PHY_WDQLVL_DQDM_OBS_SELECT_OFFSET 24                   // 
#define PHY_WDQLVL_DQDM_OBS_SELECT_WIDTH 4                    // 
#define PHY_WDQLVL_DQDM_SLV_DLY_JUMP_OFFSET_ADDR 35                   // 
#define PHY_WDQLVL_DQDM_SLV_DLY_JUMP_OFFSET_OFFSET 0                    // 
#define PHY_WDQLVL_DQDM_SLV_DLY_JUMP_OFFSET_WIDTH 10                   // 
#define PHY_WDQLVL_DQDM_SLV_DLY_START_ADDR 126                  // 
#define PHY_WDQLVL_DQDM_SLV_DLY_START_OFFSET 0                    // 
#define PHY_WDQLVL_DQDM_SLV_DLY_START_WIDTH 10                   // 
#define PHY_WDQLVL_DQDM_TE_DLY_OBS_ADDR 54                   // 
#define PHY_WDQLVL_DQDM_TE_DLY_OBS_OFFSET 16                   // 
#define PHY_WDQLVL_DQDM_TE_DLY_OBS_WIDTH 11                   // 
#define PHY_WDQLVL_DVW_MIN_ADDR        78                   // 
#define PHY_WDQLVL_DVW_MIN_OFFSET      0                    // 
#define PHY_WDQLVL_DVW_MIN_WIDTH       9                    // 
#define PHY_WDQLVL_PER_START_OFFSET_ADDR 78                   // 
#define PHY_WDQLVL_PER_START_OFFSET_OFFSET 24                   // 
#define PHY_WDQLVL_PER_START_OFFSET_WIDTH 5                    // 
#define PHY_WDQLVL_PER_VREF_THRSHLD_ADDR 36                   // 
#define PHY_WDQLVL_PER_VREF_THRSHLD_OFFSET 16                   // 
#define PHY_WDQLVL_PER_VREF_THRSHLD_WIDTH 6                    // 
#define PHY_WDQLVL_PERIODIC_OBS_ADDR   56                   // 
#define PHY_WDQLVL_PERIODIC_OBS_OFFSET 0                    // 
#define PHY_WDQLVL_PERIODIC_OBS_SELECT_ADDR 36                   // 
#define PHY_WDQLVL_PERIODIC_OBS_SELECT_OFFSET 0                    // 
#define PHY_WDQLVL_PERIODIC_OBS_SELECT_WIDTH 8                    // 
#define PHY_WDQLVL_PERIODIC_OBS_WIDTH  32                   // 
#define PHY_WDQLVL_QTR_DLY_STEP_ADDR   99                   // 
#define PHY_WDQLVL_QTR_DLY_STEP_OFFSET 24                   // 
#define PHY_WDQLVL_QTR_DLY_STEP_WIDTH  4                    // 
#define PHY_WDQLVL_STATUS_OBS_ADDR     55                   // 
#define PHY_WDQLVL_STATUS_OBS_OFFSET   0                    // 
#define PHY_WDQLVL_STATUS_OBS_WIDTH    32                   // 
#define PHY_WDQLVL_UPDT_WAIT_CNT_ADDR  35                   // 
#define PHY_WDQLVL_UPDT_WAIT_CNT_OFFSET 16                   // 
#define PHY_WDQLVL_UPDT_WAIT_CNT_WIDTH 4                    // 
#define PHY_WR_ADDER_SLV_DLY_ENC_OBS_ADDR 43                   // 
#define PHY_WR_ADDER_SLV_DLY_ENC_OBS_OFFSET 8                    // 
#define PHY_WR_ADDER_SLV_DLY_ENC_OBS_WIDTH 8                    // 
#define PHY_WR_ENC_OBS_SELECT_ADDR     29                   // 
#define PHY_WR_ENC_OBS_SELECT_OFFSET   8                    // 
#define PHY_WR_ENC_OBS_SELECT_WIDTH    4                    // 
#define PHY_WR_SHIFT_OBS_ADDR          43                   // 
#define PHY_WR_SHIFT_OBS_OFFSET        16                   // 
#define PHY_WR_SHIFT_OBS_SELECT_ADDR   29                   // 
#define PHY_WR_SHIFT_OBS_SELECT_OFFSET 16                   // 
#define PHY_WR_SHIFT_OBS_SELECT_WIDTH  4                    // 
#define PHY_WR_SHIFT_OBS_WIDTH         4                    // 
#define PHY_WRDQ_BASE_SLV_DLY_ENC_OBS_ADDR 43                   // 
#define PHY_WRDQ_BASE_SLV_DLY_ENC_OBS_OFFSET 0                    // 
#define PHY_WRDQ_BASE_SLV_DLY_ENC_OBS_WIDTH 8                    // 
#define PHY_WRDQS_BASE_SLV_DLY_ENC_OBS_ADDR 42                   // 
#define PHY_WRDQS_BASE_SLV_DLY_ENC_OBS_OFFSET 24                   // 
#define PHY_WRDQS_BASE_SLV_DLY_ENC_OBS_WIDTH 7                    // 
#define PHY_WRITE_PATH_LAT_ADD_ADDR    124                  // 
#define PHY_WRITE_PATH_LAT_ADD_BYPASS_ADDR 2                    // 
#define PHY_WRITE_PATH_LAT_ADD_BYPASS_OFFSET 0                    // 
#define PHY_WRITE_PATH_LAT_ADD_BYPASS_WIDTH 3                    // 
#define PHY_WRITE_PATH_LAT_ADD_OFFSET  8                    // 
#define PHY_WRITE_PATH_LAT_ADD_WIDTH   3                    // 
#define PHY_WRITE_PATH_LAT_ADJ_ADDR    114                  // 
#define PHY_WRITE_PATH_LAT_ADJ_OFFSET  8                    // 
#define PHY_WRITE_PATH_LAT_ADJ_WIDTH   2                    // 
#define PHY_WRITE_PATH_LAT_DEC_0_ADDR  114 + PHY_BASE_ADDR // 
#define PHY_WRITE_PATH_LAT_DEC_0_OFFSET 0                    // 
#define PHY_WRITE_PATH_LAT_DEC_0_WIDTH 1                    // 
#define PHY_WRITE_PATH_LAT_DEC_1_ADDR  370 + PHY_BASE_ADDR // 
#define PHY_WRITE_PATH_LAT_DEC_1_OFFSET 0                    // 
#define PHY_WRITE_PATH_LAT_DEC_1_WIDTH 1                    // 
#define PHY_WRITE_PATH_LAT_DEC_2_ADDR  626 + PHY_BASE_ADDR // 
#define PHY_WRITE_PATH_LAT_DEC_2_OFFSET 0                    // 
#define PHY_WRITE_PATH_LAT_DEC_2_WIDTH 1                    // 
#define PHY_WRITE_PATH_LAT_DEC_3_ADDR  882 + PHY_BASE_ADDR // 
#define PHY_WRITE_PATH_LAT_DEC_3_OFFSET 0                    // 
#define PHY_WRITE_PATH_LAT_DEC_3_WIDTH 1                    // 
#define PHY_WRITE_PATH_LAT_DEC_ADDR    114                  // 
#define PHY_WRITE_PATH_LAT_DEC_OFFSET  0                    // 
#define PHY_WRITE_PATH_LAT_DEC_WIDTH   1                    // 
#define PHY_WRITE_PATH_LAT_FRAC_ADDR   124                  // 
#define PHY_WRITE_PATH_LAT_FRAC_OFFSET 16                   // 
#define PHY_WRITE_PATH_LAT_FRAC_WIDTH  8                    // 
#define PHY_WRLVL_ALGO_ADDR            30                   // 
#define PHY_WRLVL_ALGO_OFFSET          16                   // 
#define PHY_WRLVL_ALGO_WIDTH           2                    // 
#define PHY_WRLVL_CAPTURE_CNT_0_ADDR   31 + PHY_BASE_ADDR  // 
#define PHY_WRLVL_CAPTURE_CNT_0_OFFSET 8                    // 
#define PHY_WRLVL_CAPTURE_CNT_0_WIDTH  6                    // 
#define PHY_WRLVL_CAPTURE_CNT_1_ADDR   287 + PHY_BASE_ADDR // 
#define PHY_WRLVL_CAPTURE_CNT_1_OFFSET 8                    // 
#define PHY_WRLVL_CAPTURE_CNT_1_WIDTH  6                    // 
#define PHY_WRLVL_CAPTURE_CNT_2_ADDR   543 + PHY_BASE_ADDR // 
#define PHY_WRLVL_CAPTURE_CNT_2_OFFSET 8                    // 
#define PHY_WRLVL_CAPTURE_CNT_2_WIDTH  6                    // 
#define PHY_WRLVL_CAPTURE_CNT_3_ADDR   799 + PHY_BASE_ADDR // 
#define PHY_WRLVL_CAPTURE_CNT_3_OFFSET 8                    // 
#define PHY_WRLVL_CAPTURE_CNT_3_WIDTH  6                    // 
#define PHY_WRLVL_CAPTURE_CNT_ADDR     31                   // 
#define PHY_WRLVL_CAPTURE_CNT_OFFSET   8                    // 
#define PHY_WRLVL_CAPTURE_CNT_WIDTH    6                    // 
#define PHY_WRLVL_DLY_FINE_STEP_ADDR   97                   // 
#define PHY_WRLVL_DLY_FINE_STEP_OFFSET 0                    // 
#define PHY_WRLVL_DLY_FINE_STEP_WIDTH  4                    // 
#define PHY_WRLVL_DLY_STEP_ADDR        96                   // 
#define PHY_WRLVL_DLY_STEP_OFFSET      24                   // 
#define PHY_WRLVL_DLY_STEP_WIDTH       8                    // 
#define PHY_WRLVL_EARLY_FORCE_ZERO_ADDR 125                  // 
#define PHY_WRLVL_EARLY_FORCE_ZERO_OFFSET 16                   // 
#define PHY_WRLVL_EARLY_FORCE_ZERO_WIDTH 1                    // 
#define PHY_WRLVL_ERROR_OBS_ADDR       47                   // 
#define PHY_WRLVL_ERROR_OBS_OFFSET     0                    // 
#define PHY_WRLVL_ERROR_OBS_WIDTH      2                    // 
#define PHY_WRLVL_HARD0_DELAY_OBS_ADDR 44                   // 
#define PHY_WRLVL_HARD0_DELAY_OBS_OFFSET 0                    // 
#define PHY_WRLVL_HARD0_DELAY_OBS_WIDTH 9                    // 
#define PHY_WRLVL_HARD1_DELAY_OBS_ADDR 44                   // 
#define PHY_WRLVL_HARD1_DELAY_OBS_OFFSET 16                   // 
#define PHY_WRLVL_HARD1_DELAY_OBS_WIDTH 9                    // 
#define PHY_WRLVL_LAT_UPDATE_DISABLE_ADDR 30                   // 
#define PHY_WRLVL_LAT_UPDATE_DISABLE_OFFSET 24                   // 
#define PHY_WRLVL_LAT_UPDATE_DISABLE_WIDTH 1                    // 
#define PHY_WRLVL_PER_START_ADDR       31                   // 
#define PHY_WRLVL_PER_START_OFFSET     0                    // 
#define PHY_WRLVL_PER_START_WIDTH      7                    // 
#define PHY_WRLVL_RESP_WAIT_CNT_ADDR   97                   // 
#define PHY_WRLVL_RESP_WAIT_CNT_OFFSET 8                    // 
#define PHY_WRLVL_RESP_WAIT_CNT_WIDTH  6                    // 
#define PHY_WRLVL_STATUS_OBS_ADDR      45                   // 
#define PHY_WRLVL_STATUS_OBS_OFFSET    0                    // 
#define PHY_WRLVL_STATUS_OBS_WIDTH     28                   // 
#define PHY_WRLVL_UPDT_WAIT_CNT_ADDR   31                   // 
#define PHY_WRLVL_UPDT_WAIT_CNT_OFFSET 16                   // 
#define PHY_WRLVL_UPDT_WAIT_CNT_WIDTH  4                    // 
#define PHY_WRPATH_GATE_DISABLE_0_ADDR 103 + PHY_BASE_ADDR // 
#define PHY_WRPATH_GATE_DISABLE_0_OFFSET 0                    // 
#define PHY_WRPATH_GATE_DISABLE_0_WIDTH 2                    // 
#define PHY_WRPATH_GATE_DISABLE_1_ADDR 359 + PHY_BASE_ADDR // 
#define PHY_WRPATH_GATE_DISABLE_1_OFFSET 0                    // 
#define PHY_WRPATH_GATE_DISABLE_1_WIDTH 2                    // 
#define PHY_WRPATH_GATE_DISABLE_2_ADDR 615 + PHY_BASE_ADDR // 
#define PHY_WRPATH_GATE_DISABLE_2_OFFSET 0                    // 
#define PHY_WRPATH_GATE_DISABLE_2_WIDTH 2                    // 
#define PHY_WRPATH_GATE_DISABLE_3_ADDR 871 + PHY_BASE_ADDR // 
#define PHY_WRPATH_GATE_DISABLE_3_OFFSET 0                    // 
#define PHY_WRPATH_GATE_DISABLE_3_WIDTH 2                    // 
#define PHY_WRPATH_GATE_DISABLE_ADDR   103                  // 
#define PHY_WRPATH_GATE_DISABLE_OFFSET 0                    // 
#define PHY_WRPATH_GATE_DISABLE_WIDTH  2                    // 
#define PHY_WRPATH_GATE_TIMING_ADDR    103                  // 
#define PHY_WRPATH_GATE_TIMING_OFFSET  8                    // 
#define PHY_WRPATH_GATE_TIMING_WIDTH   3                    // 
#define PID                            #8                   // synchronous logic delay in the phy interface
#define PID_ADR_REG                    #8                   // synchronous reg delay for the adr slice
#define PID_DS_REG                     #8                   // synchronous reg delay for the data slice
#define PTSCALE                        100                  // ticks / ns (determined by timescale)
#define PXP_EMULATION                  0                    // 
#define RDDQ_NUM_DELAYS                8                    // number of delay elements in the read DQ digital delay line
#define RDDQ_NUM_DELAYS_WIDTH          4                    // 
#define READ_FIFO_ENTRIES              8                    // 
#define SC_PHY_ADR_CALVL_DEBUG_CONT_ADDR 1030 + PHY_BASE_ADDR // 
#define SC_PHY_ADR_CALVL_DEBUG_CONT_OFFSET 24                   // 
#define SC_PHY_ADR_CALVL_DEBUG_CONT_WIDTH 1                    // 
#define SC_PHY_ADR_CALVL_ERROR_CLR_ADDR 1031 + PHY_BASE_ADDR // 
#define SC_PHY_ADR_CALVL_ERROR_CLR_OFFSET 0                    // 
#define SC_PHY_ADR_CALVL_ERROR_CLR_WIDTH 1                    // 
#define SC_PHY_CSLVL_DEBUG_CONT_ADDR   1059 + PHY_BASE_ADDR // 
#define SC_PHY_CSLVL_DEBUG_CONT_OFFSET 24                   // 
#define SC_PHY_CSLVL_DEBUG_CONT_WIDTH  1                    // 
#define SC_PHY_CSLVL_ERROR_CLR_ADDR    1060 + PHY_BASE_ADDR // 
#define SC_PHY_CSLVL_ERROR_CLR_OFFSET  0                    // 
#define SC_PHY_CSLVL_ERROR_CLR_WIDTH   1                    // 
#define SC_PHY_LVL_DEBUG_CONT_ADDR     30                   // 
#define SC_PHY_LVL_DEBUG_CONT_OFFSET   8                    // 
#define SC_PHY_LVL_DEBUG_CONT_WIDTH    1                    // 
#define SC_PHY_MANUAL_CLEAR_0_ADDR     38 + PHY_BASE_ADDR  // 
#define SC_PHY_MANUAL_CLEAR_0_OFFSET   0                    // 
#define SC_PHY_MANUAL_CLEAR_0_WIDTH    6                    // 
#define SC_PHY_MANUAL_CLEAR_1_ADDR     294 + PHY_BASE_ADDR // 
#define SC_PHY_MANUAL_CLEAR_1_OFFSET   0                    // 
#define SC_PHY_MANUAL_CLEAR_1_WIDTH    6                    // 
#define SC_PHY_MANUAL_CLEAR_2_ADDR     550 + PHY_BASE_ADDR // 
#define SC_PHY_MANUAL_CLEAR_2_OFFSET   0                    // 
#define SC_PHY_MANUAL_CLEAR_2_WIDTH    6                    // 
#define SC_PHY_MANUAL_CLEAR_3_ADDR     806 + PHY_BASE_ADDR // 
#define SC_PHY_MANUAL_CLEAR_3_OFFSET   0                    // 
#define SC_PHY_MANUAL_CLEAR_3_WIDTH    6                    // 
#define SC_PHY_MANUAL_CLEAR_ADDR       38                   // 
#define SC_PHY_MANUAL_CLEAR_OFFSET     0                    // 
#define SC_PHY_MANUAL_CLEAR_WIDTH      6                    // 
#define SC_PHY_MANUAL_UPDATE_ADDR      3 + PHY_AC_BASE_ADDR // 
#define SC_PHY_MANUAL_UPDATE_OFFSET    24                   // 
#define SC_PHY_MANUAL_UPDATE_WIDTH     1                    // 
#define SC_PHY_PAD_DBG_CONT_0_ADDR     44 + PHY_AC_BASE_ADDR // 
#define SC_PHY_PAD_DBG_CONT_0_OFFSET   0                    // 
#define SC_PHY_PAD_DBG_CONT_0_WIDTH    1                    // 
#define SC_PHY_PAD_DBG_CONT_WIDTH      1                    // 
#define SC_PHY_PLL_SPO_CAL_SNAP_OBS_ADDR 15 + PHY_AC_BASE_ADDR // 
#define SC_PHY_PLL_SPO_CAL_SNAP_OBS_OFFSET 0                    // 
#define SC_PHY_PLL_SPO_CAL_SNAP_OBS_WIDTH 2                    // 
#define SC_PHY_SNAP_OBS_REGS_ADDR      16                   // 
#define SC_PHY_SNAP_OBS_REGS_OFFSET    24                   // 
#define SC_PHY_SNAP_OBS_REGS_WIDTH     1                    // 
#define SC_PHY_UPDATE_CLK_CAL_VALUES_ADDR 5 + PHY_AC_BASE_ADDR // 
#define SC_PHY_UPDATE_CLK_CAL_VALUES_OFFSET 8                    // 
#define SC_PHY_UPDATE_CLK_CAL_VALUES_WIDTH 1                    // 
#define SC_PHY_WDQLVL_CLR_PREV_RESULTS_ADDR 36                   // 
#define SC_PHY_WDQLVL_CLR_PREV_RESULTS_OFFSET 24                   // 
#define SC_PHY_WDQLVL_CLR_PREV_RESULTS_WIDTH 1                    // 
#define SETUP_DLY                      3.0                  // flop setup delay        
#define SLICE_CUSTOM_8                 1                    // 
#define SLICE_DFT_ENHANCEMENT          1                    // 
#define SLICE_FFE                      1                    // 
#define SLICE_HW_FREQ_CHANGE           1                    // 
#define SLICE_MIMIC_DELAY_IN_DL        0                    // 
#define SLICE_PER_RANK_TRAINING_ENABLE 1                    // 
#define SLICE_PHY_DDL_BIST_NUM_DLYS    3                    // 
#define SLICE_PHY_DSLICE_MAX_CS        2                    // 
#define SLICE_PHY_FREQ_SEL_INDEX_WIDTH 2                    // 
#define SLICE_PHY_FREQ_SEL_MULTICAST_EN_WIDTH 1                    // 
#define SLICE_PHY_FREQ_SEL_WIDTH       2                    // 
#define SLICE_PHY_LVL_PATTERN_WIDTH    4                    // 
#define SLICE_PHY_NUM_FREQ_CHANGE_REG_COPIES 2                    // 
#define SLICE_PHY_REG_DATA_WIDTH       32                   // 
#define SLICE_PHY_REG_MASK_WIDTH       4                    // 
#define SLICE_REMOVE_WRPATH_BSCAN_MUX  0                    // 
#define SYS_MAX_CS                     2                    // 
#define TOP_SCAN_COUNT                 446                  // 
#define WRCLK_1UI                      0                    // 
#define WRDQ_BASE_NUM_DELAYS           128                  // number of delay elements in the write dq base digital delay line
#define WRDQ_BASE_NUM_DELAYS_WIDTH     8                    // 
#define WRDQS_ADDER_NUM_DELAYS         24                   // number of delay elements in the write dqs adder digital delay line
#define WRDQS_ADDER_NUM_DELAYS_WIDTH   5                    // 
#define WRDQS_BASE_NUM_DELAYS          120                  // number of delay elements in the write dqs base digital delay line
#define WRDQS_BASE_NUM_DELAYS_WIDTH    7                    // 


// HIERARCHY VARIABLES -- edit to suit your environment

//#define TIER_PHY_TOP  memcd_test.asic.ddr_subsystem.cdn_hs_phy_top              // 
