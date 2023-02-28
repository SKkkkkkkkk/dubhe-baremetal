#define PHY_BASE_ADDR                               4096
#define PI_BASE_ADDR                                2048

#define PI_BOOT_FROM_WORK_FREQ_ADDR                 13+2048
#define PI_BOOT_FROM_WORK_FREQ_OFFSET               0
#define PI_BOOT_FROM_WORK_FREQ_WIDTH                1

#define PI_FREQ_MAP_ADDR                            10+2048
#define PI_FREQ_MAP_OFFSET                          0
#define PI_FREQ_MAP_WIDTH                           32

#define LPI_WAKEUP_EN_ADDR                          161
#define LPI_WAKEUP_EN_OFFSET                        16
#define LPI_WAKEUP_EN_WIDTH                         6

#define IN_ORDER_ACCEPT_ADDR                        325
#define IN_ORDER_ACCEPT_OFFSET                      8
#define IN_ORDER_ACCEPT_WIDTH                       1

#define DISABLE_RW_GROUP_W_BNK_CONFLICT_ADDR        316
#define DISABLE_RW_GROUP_W_BNK_CONFLICT_OFFSET      16
#define DISABLE_RW_GROUP_W_BNK_CONFLICT_WIDTH       2

#define WRITE_MODEREG_ADDR                          185                  // 
#define WRITE_MODEREG_OFFSET                        0                    // 
#define WRITE_MODEREG_WIDTH                         27                   // 

#define MRSINGLE_DATA_0_ADDR                        205                  // 
#define MRSINGLE_DATA_0_OFFSET                      0                    // 
#define MRSINGLE_DATA_0_WIDTH                       8                    // 

#define MRSINGLE_DATA_1_ADDR                        205                  // 
#define MRSINGLE_DATA_1_OFFSET                      8                    // 
#define MRSINGLE_DATA_1_WIDTH                       8                    // 

#define INIT_FREQ_ADDR                              175                  // 
#define INIT_FREQ_OFFSET                            0                    // 
#define INIT_FREQ_WIDTH                             2                    // 

#define CS_SAME_EN_ADDR                             316
#define CS_SAME_EN_OFFSET                           0
#define CS_SAME_EN_WIDTH                            1

#define PI_INIT_LVL_EN_ADDR                         1+2048
#define PI_INIT_LVL_EN_OFFSET                       16
#define PI_INIT_LVL_EN_WIDTH                        1

#define SWAP_EN_ADDR                                317
#define SWAP_EN_OFFSET                              0
#define SWAP_EN_WIDTH                               1

#define WR_ORDER_REQ_ADDR                           325
#define WR_ORDER_REQ_OFFSET                         16
#define WR_ORDER_REQ_WIDTH                          2

#define PHY_CAL_CPTR_CNT_0_ADDR                     42+5376
#define PHY_CAL_CPTR_CNT_0_OFFSET                   24
#define PHY_CAL_CPTR_CNT_0_WIDTH                    7

#define PHY_CAL_CLK_SELECT_0_ADDR                   86+5376
#define PHY_CAL_CLK_SELECT_0_OFFSET                 0
#define PHY_CAL_CLK_SELECT_0_WIDTH                  3

#define PHY_LP4_BOOT_CAL_CLK_SELECT_0_ADDR          36+5376
#define PHY_LP4_BOOT_CAL_CLK_SELECT_0_OFFSET        8
#define PHY_LP4_BOOT_CAL_CLK_SELECT_0_WIDTH         3

#define ECC_ENABLE_ADDR                             234
#define ECC_ENABLE_OFFSET                           24
#define ECC_ENABLE_WIDTH                            2

#define PI_START_ADDR                               0+2048
#define PI_START_OFFSET                             0 
#define PI_START_WIDTH                              1

#define DFIBUS_BOOT_FREQ_ADDR                       176
#define DFIBUS_BOOT_FREQ_OFFSET                     8
#define DFIBUS_BOOT_FREQ_WIDTH                      2

#define PHY_LP4_BOOT_DISABLE_ADDR                   4+5376
#define PHY_LP4_BOOT_DISABLE_OFFSET                 16
#define PHY_LP4_BOOT_DISABLE_WIDTH                  1

#define PHY_FREQ_SEL_MULTICAST_EN_ADDR              1+5376
#define PHY_FREQ_SEL_MULTICAST_EN_OFFSET            8
#define PHY_FREQ_SEL_MULTICAST_EN_WIDTH             1

#define PHY_PLL_CTRL_ADDR                           68+5376
#define PHY_PLL_CTRL_OFFSET                         0
#define PHY_PLL_CTRL_WIDTH                          13

#define START_ADDR                                  0
#define START_OFFSET                                0
#define START_WIDTH                                 1

//void init_ddr0_faster(void);
//void init_ddr(void);
