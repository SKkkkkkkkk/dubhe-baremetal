#if defined A55
#ifndef __DMA_H__      /* ToDo: replace '<Device>' with your device name */
#define __DMA_H__

// #include "ca55_chip_define.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{
//common register offset 
 __IOM uint32_t DMAC_IDREG_0                             ; //0x0
 __IOM uint32_t DMAC_IDREG_32                            ; //0x4
 __IOM uint32_t DMAC_COMPVERREG_0                        ; //0x8
 __IOM uint32_t DMAC_COMPVERREG_32                       ; //0xc
 __IOM uint32_t DMAC_CFGREG_0                            ; //0x10
 __IOM uint32_t DMAC_CFGREG_32                           ; //0x14
 __IOM uint32_t DMAC_CHENREG_0                           ; //0x18
 __IOM uint32_t DMAC_CHENREG_32                          ; //0x1c
 __IOM uint32_t DMAC_CHSUSPREG_0                         ; //0x20
 __IOM uint32_t DMAC_CHSUSPREG_32                        ; //0x24
 __IOM uint32_t DMAC_CHABORTREG_0                        ; //0x28
 __IOM uint32_t DMAC_CHABORTREG_32                       ; //0x2c
 __IOM uint32_t DMAC_INTSTATUSREG_0                      ; //0x30
 __IOM uint32_t DMAC_INTSTATUSREG_32                     ; //0x34
 __IOM uint32_t DMAC_COMMONREG_INTCLEARREG_0             ; //0x38
 __IOM uint32_t DMAC_COMMONREG_INTCLEARREG_32            ; //0x3c
 __IOM uint32_t DMAC_COMMONREG_INTSTATUS_ENABLEREG_0     ; //0x40
 __IOM uint32_t DMAC_COMMONREG_INTSTATUS_ENABLEREG_32    ; //0x44
 __IOM uint32_t DMAC_COMMONREG_INTSIGNAL_ENABLEREG_0     ; //0x48
 __IOM uint32_t DMAC_COMMONREG_INTSIGNAL_ENABLEREG_32    ; //0x4c
 __IOM uint32_t DMAC_COMMONREG_INTSTATUSREG_0            ; //0x50
 __IOM uint32_t DMAC_COMMONREG_INTSTATUSREG_32           ; //0x54
 __IOM uint32_t DMAC_RESETREG_0                          ; //0x58
 __IOM uint32_t DMAC_RESETREG_32                         ; //0x5c
 __IOM uint32_t DMAC_LOWPOWER_CFGREG_0                   ; //0x60
 __IOM uint32_t DMAC_LOWPOWER_CFGREG_32                  ; //0x64
          RESERVED(0[38], uint32_t)    
//channel 1 register
 __IOM uint32_t CH1_SAR_0                                ; //0x100
 __IOM uint32_t CH1_SAR_32                               ; //0x104
 __IOM uint32_t CH1_DAR_0                                ; //0x108
 __IOM uint32_t CH1_DAR_32                               ; //0x10c
 __IOM uint32_t CH1_BLOCK_TS_0                           ; //0x110
 __IOM uint32_t CH1_BLOCK_TS_32                          ; //0x114
 __IOM uint32_t CH1_CTL_0                                ; //0x118
 __IOM uint32_t CH1_CTL_32                               ; //0x11c
 __IOM uint32_t CH1_CFG2_0                               ; //0x120
 __IOM uint32_t CH1_CFG2_32                              ; //0x124
 __IOM uint32_t CH1_LLP_0                                ; //0x128
 __IOM uint32_t CH1_LLP_32                               ; //0x12c
 __IOM uint32_t CH1_STATUSREG_0                          ; //0x130
 __IOM uint32_t CH1_STATUSREG_32                         ; //0x134
 __IOM uint32_t CH1_SWHSSRCREG_0                         ; //0x138
 __IOM uint32_t CH1_SWHSSRCREG_32                        ; //0x13c
 __IOM uint32_t CH1_SWHSDSTREG_0                         ; //0x140
 __IOM uint32_t CH1_SWHSDSTREG_32                        ; //0x144
 __IOM uint32_t CH1_BLK_TFR_RESUMEREQREG_0               ; //0x148
 __IOM uint32_t CH1_BLK_TFR_RESUMEREQREG_32              ; //0x14c
 __IOM uint32_t CH1_AXI_IDREG_0                          ; //0x150
 __IOM uint32_t CH1_AXI_IDREG_32                         ; //0x154
 __IOM uint32_t CH1_AXI_QOSREG_0                         ; //0x158
 __IOM uint32_t CH1_AXI_QOSREG_32                        ; //0x15c
 __IOM uint32_t CH1_SSTAT_0                              ; //0x160
 __IOM uint32_t CH1_SSTAT_32                             ; //0x164
 __IOM uint32_t CH1_DSTAT_0                              ; //0x168
 __IOM uint32_t CH1_DSTAT_32                             ; //0x16c
 __IOM uint32_t CH1_SSTATAR_0                            ; //0x170
 __IOM uint32_t CH1_SSTATAR_32                           ; //0x174
 __IOM uint32_t CH1_DSTATAR_0                            ; //0x178
 __IOM uint32_t CH1_DSTATAR_32                           ; //0x17c
 __IOM uint32_t CH1_INTSTATUS_ENABLEREG_0                ; //0x180
 __IOM uint32_t CH1_INTSTATUS_ENABLEREG_32               ; //0x184
 __IOM uint32_t CH1_INTSTATUS_0                          ; //0x188
 __IOM uint32_t CH1_INTSTATUS_32                         ; //0x18c
 __IOM uint32_t CH1_INTSIGNAL_ENABLEREG_0                ; //0x190
 __IOM uint32_t CH1_INTSIGNAL_ENABLEREG_32               ; //0x194
 __IOM uint32_t CH1_INTCLEARREG_0                        ; //0x198
 __IOM uint32_t CH1_INTCLEARREG_32                       ; //0x19c
          RESERVED(1[24], uint32_t)    

//channel 2 register
 __IOM uint32_t CH2_SAR_0                                ; //(0x100+(1*0x100))
 __IOM uint32_t CH2_SAR_32                               ; //(0x104+(1*0x100))
 __IOM uint32_t CH2_DAR_0                                ; //(0x108+(1*0x100))
 __IOM uint32_t CH2_DAR_32                               ; //(0x10c+(1*0x100))
 __IOM uint32_t CH2_BLOCK_TS_0                           ; //(0x110+(1*0x100))
 __IOM uint32_t CH2_BLOCK_TS_32                          ; //(0x114+(1*0x100))
 __IOM uint32_t CH2_CTL_0                                ; //(0x118+(1*0x100))
 __IOM uint32_t CH2_CTL_32                               ; //(0x11c+(1*0x100))
 __IOM uint32_t CH2_CFG2_0                               ; //(0x120+(1*0x100))
 __IOM uint32_t CH2_CFG2_32                              ; //(0x124+(1*0x100))
 __IOM uint32_t CH2_LLP_0                                ; //(0x128+(1*0x100))
 __IOM uint32_t CH2_LLP_32                               ; //(0x12c+(1*0x100))
 __IOM uint32_t CH2_STATUSREG_0                          ; //(0x130+(1*0x100))
 __IOM uint32_t CH2_STATUSREG_32                         ; //(0x134+(1*0x100))
 __IOM uint32_t CH2_SWHSSRCREG_0                         ; //(0x138+(1*0x100))
 __IOM uint32_t CH2_SWHSSRCREG_32                        ; //(0x13c+(1*0x100))
 __IOM uint32_t CH2_SWHSDSTREG_0                         ; //(0x140+(1*0x100))
 __IOM uint32_t CH2_SWHSDSTREG_32                        ; //(0x144+(1*0x100))
 __IOM uint32_t CH2_BLK_TFR_RESUMEREQREG_0               ; //(0x148+(1*0x100))
 __IOM uint32_t CH2_BLK_TFR_RESUMEREQREG_32              ; //(0x14c+(1*0x100))
 __IOM uint32_t CH2_AXI_IDREG_0                          ; //(0x150+(1*0x100))
 __IOM uint32_t CH2_AXI_IDREG_32                         ; //(0x154+(1*0x100))
 __IOM uint32_t CH2_AXI_QOSREG_0                         ; //(0x158+(1*0x100))
 __IOM uint32_t CH2_AXI_QOSREG_32                        ; //(0x15c+(1*0x100))
 __IOM uint32_t CH2_SSTAT_0                              ; //(0x160+(1*0x100))
 __IOM uint32_t CH2_SSTAT_32                             ; //(0x164+(1*0x100))
 __IOM uint32_t CH2_DSTAT_0                              ; //(0x168+(1*0x100))
 __IOM uint32_t CH2_DSTAT_32                             ; //(0x16c+(1*0x100))
 __IOM uint32_t CH2_SSTATAR_0                            ; //(0x170+(1*0x100))
 __IOM uint32_t CH2_SSTATAR_32                           ; //(0x174+(1*0x100))
 __IOM uint32_t CH2_DSTATAR_0                            ; //(0x178+(1*0x100))
 __IOM uint32_t CH2_DSTATAR_32                           ; //(0x17c+(1*0x100))
 __IOM uint32_t CH2_INTSTATUS_ENABLEREG_0                ; //(0x180+(1*0x100))
 __IOM uint32_t CH2_INTSTATUS_ENABLEREG_32               ; //(0x184+(1*0x100))
 __IOM uint32_t CH2_INTSTATUS_0                          ; //(0x188+(1*0x100))
 __IOM uint32_t CH2_INTSTATUS_32                         ; //(0x18c+(1*0x100))
 __IOM uint32_t CH2_INTSIGNAL_ENABLEREG_0                ; //(0x190+(1*0x100))
 __IOM uint32_t CH2_INTSIGNAL_ENABLEREG_32               ; //(0x194+(1*0x100))
 __IOM uint32_t CH2_INTCLEARREG_0                        ; //(0x198+(1*0x100))
 __IOM uint32_t CH2_INTCLEARREG_32                       ; //(0x19c+(1*0x100))
          RESERVED(2[24], uint32_t)    

//channel 3 register
 __IOM uint32_t CH3_SAR_0                                ; //(0x100+(2*0x100))
 __IOM uint32_t CH3_SAR_32                               ; //(0x104+(2*0x100))
 __IOM uint32_t CH3_DAR_0                                ; //(0x108+(2*0x100))
 __IOM uint32_t CH3_DAR_32                               ; //(0x10c+(2*0x100))
 __IOM uint32_t CH3_BLOCK_TS_0                           ; //(0x110+(2*0x100))
 __IOM uint32_t CH3_BLOCK_TS_32                          ; //(0x114+(2*0x100))
 __IOM uint32_t CH3_CTL_0                                ; //(0x118+(2*0x100))
 __IOM uint32_t CH3_CTL_32                               ; //(0x11c+(2*0x100))
 __IOM uint32_t CH3_CFG2_0                               ; //(0x120+(2*0x100))
 __IOM uint32_t CH3_CFG2_32                              ; //(0x124+(2*0x100))
 __IOM uint32_t CH3_LLP_0                                ; //(0x128+(2*0x100))
 __IOM uint32_t CH3_LLP_32                               ; //(0x12c+(2*0x100))
 __IOM uint32_t CH3_STATUSREG_0                          ; //(0x130+(2*0x100))
 __IOM uint32_t CH3_STATUSREG_32                         ; //(0x134+(2*0x100))
 __IOM uint32_t CH3_SWHSSRCREG_0                         ; //(0x138+(2*0x100))
 __IOM uint32_t CH3_SWHSSRCREG_32                        ; //(0x13c+(2*0x100))
 __IOM uint32_t CH3_SWHSDSTREG_0                         ; //(0x140+(2*0x100))
 __IOM uint32_t CH3_SWHSDSTREG_32                        ; //(0x144+(2*0x100))
 __IOM uint32_t CH3_BLK_TFR_RESUMEREQREG_0               ; //(0x148+(2*0x100))
 __IOM uint32_t CH3_BLK_TFR_RESUMEREQREG_32              ; //(0x14c+(2*0x100))
 __IOM uint32_t CH3_AXI_IDREG_0                          ; //(0x150+(2*0x100))
 __IOM uint32_t CH3_AXI_IDREG_32                         ; //(0x154+(2*0x100))
 __IOM uint32_t CH3_AXI_QOSREG_0                         ; //(0x158+(2*0x100))
 __IOM uint32_t CH3_AXI_QOSREG_32                        ; //(0x15c+(2*0x100))
 __IOM uint32_t CH3_SSTAT_0                              ; //(0x160+(2*0x100))
 __IOM uint32_t CH3_SSTAT_32                             ; //(0x164+(2*0x100))
 __IOM uint32_t CH3_DSTAT_0                              ; //(0x168+(2*0x100))
 __IOM uint32_t CH3_DSTAT_32                             ; //(0x16c+(2*0x100))
 __IOM uint32_t CH3_SSTATAR_0                            ; //(0x170+(2*0x100))
 __IOM uint32_t CH3_SSTATAR_32                           ; //(0x174+(2*0x100))
 __IOM uint32_t CH3_DSTATAR_0                            ; //(0x178+(2*0x100))
 __IOM uint32_t CH3_DSTATAR_32                           ; //(0x17c+(2*0x100))
 __IOM uint32_t CH3_INTSTATUS_ENABLEREG_0                ; //(0x180+(2*0x100))
 __IOM uint32_t CH3_INTSTATUS_ENABLEREG_32               ; //(0x184+(2*0x100))
 __IOM uint32_t CH3_INTSTATUS_0                          ; //(0x188+(2*0x100))
 __IOM uint32_t CH3_INTSTATUS_32                         ; //(0x18c+(2*0x100))
 __IOM uint32_t CH3_INTSIGNAL_ENABLEREG_0                ; //(0x190+(2*0x100))
 __IOM uint32_t CH3_INTSIGNAL_ENABLEREG_32               ; //(0x194+(2*0x100))
 __IOM uint32_t CH3_INTCLEARREG_0                        ; //(0x198+(2*0x100))
 __IOM uint32_t CH3_INTCLEARREG_32                       ; //(0x19c+(2*0x100))
          RESERVED(3[24], uint32_t)    

//channel 4 register
 __IOM uint32_t CH4_SAR_0                                ; //(0x100+(3*0x100))
 __IOM uint32_t CH4_SAR_32                               ; //(0x104+(3*0x100))
 __IOM uint32_t CH4_DAR_0                                ; //(0x108+(3*0x100))
 __IOM uint32_t CH4_DAR_32                               ; //(0x10c+(3*0x100))
 __IOM uint32_t CH4_BLOCK_TS_0                           ; //(0x110+(3*0x100))
 __IOM uint32_t CH4_BLOCK_TS_32                          ; //(0x114+(3*0x100))
 __IOM uint32_t CH4_CTL_0                                ; //(0x118+(3*0x100))
 __IOM uint32_t CH4_CTL_32                               ; //(0x11c+(3*0x100))
 __IOM uint32_t CH4_CFG2_0                               ; //(0x120+(3*0x100))
 __IOM uint32_t CH4_CFG2_32                              ; //(0x124+(3*0x100))
 __IOM uint32_t CH4_LLP_0                                ; //(0x128+(3*0x100))
 __IOM uint32_t CH4_LLP_32                               ; //(0x12c+(3*0x100))
 __IOM uint32_t CH4_STATUSREG_0                          ; //(0x130+(3*0x100))
 __IOM uint32_t CH4_STATUSREG_32                         ; //(0x134+(3*0x100))
 __IOM uint32_t CH4_SWHSSRCREG_0                         ; //(0x138+(3*0x100))
 __IOM uint32_t CH4_SWHSSRCREG_32                        ; //(0x13c+(3*0x100))
 __IOM uint32_t CH4_SWHSDSTREG_0                         ; //(0x140+(3*0x100))
 __IOM uint32_t CH4_SWHSDSTREG_32                        ; //(0x144+(3*0x100))
 __IOM uint32_t CH4_BLK_TFR_RESUMEREQREG_0               ; //(0x148+(3*0x100))
 __IOM uint32_t CH4_BLK_TFR_RESUMEREQREG_32              ; //(0x14c+(3*0x100))
 __IOM uint32_t CH4_AXI_IDREG_0                          ; //(0x150+(3*0x100))
 __IOM uint32_t CH4_AXI_IDREG_32                         ; //(0x154+(3*0x100))
 __IOM uint32_t CH4_AXI_QOSREG_0                         ; //(0x158+(3*0x100))
 __IOM uint32_t CH4_AXI_QOSREG_32                        ; //(0x15c+(3*0x100))
 __IOM uint32_t CH4_SSTAT_0                              ; //(0x160+(3*0x100))
 __IOM uint32_t CH4_SSTAT_32                             ; //(0x164+(3*0x100))
 __IOM uint32_t CH4_DSTAT_0                              ; //(0x168+(3*0x100))
 __IOM uint32_t CH4_DSTAT_32                             ; //(0x16c+(3*0x100))
 __IOM uint32_t CH4_SSTATAR_0                            ; //(0x170+(3*0x100))
 __IOM uint32_t CH4_SSTATAR_32                           ; //(0x174+(3*0x100))
 __IOM uint32_t CH4_DSTATAR_0                            ; //(0x178+(3*0x100))
 __IOM uint32_t CH4_DSTATAR_32                           ; //(0x17c+(3*0x100))
 __IOM uint32_t CH4_INTSTATUS_ENABLEREG_0                ; //(0x180+(3*0x100))
 __IOM uint32_t CH4_INTSTATUS_ENABLEREG_32               ; //(0x184+(3*0x100))
 __IOM uint32_t CH4_INTSTATUS_0                          ; //(0x188+(3*0x100))
 __IOM uint32_t CH4_INTSTATUS_32                         ; //(0x18c+(3*0x100))
 __IOM uint32_t CH4_INTSIGNAL_ENABLEREG_0                ; //(0x190+(3*0x100))
 __IOM uint32_t CH4_INTSIGNAL_ENABLEREG_32               ; //(0x194+(3*0x100))
 __IOM uint32_t CH4_INTCLEARREG_0                        ; //(0x198+(3*0x100))
 __IOM uint32_t CH4_INTCLEARREG_32                       ; //(0x19c+(3*0x100))
          RESERVED(4[24], uint32_t)    

//channel 5 register
 __IOM uint32_t CH5_SAR_0                                ; //(0x100+(4*0x100))
 __IOM uint32_t CH5_SAR_32                               ; //(0x104+(4*0x100))
 __IOM uint32_t CH5_DAR_0                                ; //(0x108+(4*0x100))
 __IOM uint32_t CH5_DAR_32                               ; //(0x10c+(4*0x100))
 __IOM uint32_t CH5_BLOCK_TS_0                           ; //(0x110+(4*0x100))
 __IOM uint32_t CH5_BLOCK_TS_32                          ; //(0x114+(4*0x100))
 __IOM uint32_t CH5_CTL_0                                ; //(0x118+(4*0x100))
 __IOM uint32_t CH5_CTL_32                               ; //(0x11c+(4*0x100))
 __IOM uint32_t CH5_CFG2_0                               ; //(0x120+(4*0x100))
 __IOM uint32_t CH5_CFG2_32                              ; //(0x124+(4*0x100))
 __IOM uint32_t CH5_LLP_0                                ; //(0x128+(4*0x100))
 __IOM uint32_t CH5_LLP_32                               ; //(0x12c+(4*0x100))
 __IOM uint32_t CH5_STATUSREG_0                          ; //(0x130+(4*0x100))
 __IOM uint32_t CH5_STATUSREG_32                         ; //(0x134+(4*0x100))
 __IOM uint32_t CH5_SWHSSRCREG_0                         ; //(0x138+(4*0x100))
 __IOM uint32_t CH5_SWHSSRCREG_32                        ; //(0x13c+(4*0x100))
 __IOM uint32_t CH5_SWHSDSTREG_0                         ; //(0x140+(4*0x100))
 __IOM uint32_t CH5_SWHSDSTREG_32                        ; //(0x144+(4*0x100))
 __IOM uint32_t CH5_BLK_TFR_RESUMEREQREG_0               ; //(0x148+(4*0x100))
 __IOM uint32_t CH5_BLK_TFR_RESUMEREQREG_32              ; //(0x14c+(4*0x100))
 __IOM uint32_t CH5_AXI_IDREG_0                          ; //(0x150+(4*0x100))
 __IOM uint32_t CH5_AXI_IDREG_32                         ; //(0x154+(4*0x100))
 __IOM uint32_t CH5_AXI_QOSREG_0                         ; //(0x158+(4*0x100))
 __IOM uint32_t CH5_AXI_QOSREG_32                        ; //(0x15c+(4*0x100))
 __IOM uint32_t CH5_SSTAT_0                              ; //(0x160+(4*0x100))
 __IOM uint32_t CH5_SSTAT_32                             ; //(0x164+(4*0x100))
 __IOM uint32_t CH5_DSTAT_0                              ; //(0x168+(4*0x100))
 __IOM uint32_t CH5_DSTAT_32                             ; //(0x16c+(4*0x100))
 __IOM uint32_t CH5_SSTATAR_0                            ; //(0x170+(4*0x100))
 __IOM uint32_t CH5_SSTATAR_32                           ; //(0x174+(4*0x100))
 __IOM uint32_t CH5_DSTATAR_0                            ; //(0x178+(4*0x100))
 __IOM uint32_t CH5_DSTATAR_32                           ; //(0x17c+(4*0x100))
 __IOM uint32_t CH5_INTSTATUS_ENABLEREG_0                ; //(0x180+(4*0x100))
 __IOM uint32_t CH5_INTSTATUS_ENABLEREG_32               ; //(0x184+(4*0x100))
 __IOM uint32_t CH5_INTSTATUS_0                          ; //(0x188+(4*0x100))
 __IOM uint32_t CH5_INTSTATUS_32                         ; //(0x18c+(4*0x100))
 __IOM uint32_t CH5_INTSIGNAL_ENABLEREG_0                ; //(0x190+(4*0x100))
 __IOM uint32_t CH5_INTSIGNAL_ENABLEREG_32               ; //(0x194+(4*0x100))
 __IOM uint32_t CH5_INTCLEARREG_0                        ; //(0x198+(4*0x100))
 __IOM uint32_t CH5_INTCLEARREG_32                       ; //(0x19c+(4*0x100))
          RESERVED(5[24], uint32_t)    

//channel 6 register
 __IOM uint32_t CH6_SAR_0                                ; //(0x100+(5*0x100))
 __IOM uint32_t CH6_SAR_32                               ; //(0x104+(5*0x100))
 __IOM uint32_t CH6_DAR_0                                ; //(0x108+(5*0x100))
 __IOM uint32_t CH6_DAR_32                               ; //(0x10c+(5*0x100))
 __IOM uint32_t CH6_BLOCK_TS_0                           ; //(0x110+(5*0x100))
 __IOM uint32_t CH6_BLOCK_TS_32                          ; //(0x114+(5*0x100))
 __IOM uint32_t CH6_CTL_0                                ; //(0x118+(5*0x100))
 __IOM uint32_t CH6_CTL_32                               ; //(0x11c+(5*0x100))
 __IOM uint32_t CH6_CFG2_0                               ; //(0x120+(5*0x100))
 __IOM uint32_t CH6_CFG2_32                              ; //(0x124+(5*0x100))
 __IOM uint32_t CH6_LLP_0                                ; //(0x128+(5*0x100))
 __IOM uint32_t CH6_LLP_32                               ; //(0x12c+(5*0x100))
 __IOM uint32_t CH6_STATUSREG_0                          ; //(0x130+(5*0x100))
 __IOM uint32_t CH6_STATUSREG_32                         ; //(0x134+(5*0x100))
 __IOM uint32_t CH6_SWHSSRCREG_0                         ; //(0x138+(5*0x100))
 __IOM uint32_t CH6_SWHSSRCREG_32                        ; //(0x13c+(5*0x100))
 __IOM uint32_t CH6_SWHSDSTREG_0                         ; //(0x140+(5*0x100))
 __IOM uint32_t CH6_SWHSDSTREG_32                        ; //(0x144+(5*0x100))
 __IOM uint32_t CH6_BLK_TFR_RESUMEREQREG_0               ; //(0x148+(5*0x100))
 __IOM uint32_t CH6_BLK_TFR_RESUMEREQREG_32              ; //(0x14c+(5*0x100))
 __IOM uint32_t CH6_AXI_IDREG_0                          ; //(0x150+(5*0x100))
 __IOM uint32_t CH6_AXI_IDREG_32                         ; //(0x154+(5*0x100))
 __IOM uint32_t CH6_AXI_QOSREG_0                         ; //(0x158+(5*0x100))
 __IOM uint32_t CH6_AXI_QOSREG_32                        ; //(0x15c+(5*0x100))
 __IOM uint32_t CH6_SSTAT_0                              ; //(0x160+(5*0x100))
 __IOM uint32_t CH6_SSTAT_32                             ; //(0x164+(5*0x100))
 __IOM uint32_t CH6_DSTAT_0                              ; //(0x168+(5*0x100))
 __IOM uint32_t CH6_DSTAT_32                             ; //(0x16c+(5*0x100))
 __IOM uint32_t CH6_SSTATAR_0                            ; //(0x170+(5*0x100))
 __IOM uint32_t CH6_SSTATAR_32                           ; //(0x174+(5*0x100))
 __IOM uint32_t CH6_DSTATAR_0                            ; //(0x178+(5*0x100))
 __IOM uint32_t CH6_DSTATAR_32                           ; //(0x17c+(5*0x100))
 __IOM uint32_t CH6_INTSTATUS_ENABLEREG_0                ; //(0x180+(5*0x100))
 __IOM uint32_t CH6_INTSTATUS_ENABLEREG_32               ; //(0x184+(5*0x100))
 __IOM uint32_t CH6_INTSTATUS_0                          ; //(0x188+(5*0x100))
 __IOM uint32_t CH6_INTSTATUS_32                         ; //(0x18c+(5*0x100))
 __IOM uint32_t CH6_INTSIGNAL_ENABLEREG_0                ; //(0x190+(5*0x100))
 __IOM uint32_t CH6_INTSIGNAL_ENABLEREG_32               ; //(0x194+(5*0x100))
 __IOM uint32_t CH6_INTCLEARREG_0                        ; //(0x198+(5*0x100))
 __IOM uint32_t CH6_INTCLEARREG_32                       ; //(0x19c+(5*0x100))
          RESERVED(6[24], uint32_t)    

//channel 7 register
 __IOM uint32_t CH7_SAR_0                                ; //(0x100+(6*0x100))
 __IOM uint32_t CH7_SAR_32                               ; //(0x104+(6*0x100))
 __IOM uint32_t CH7_DAR_0                                ; //(0x108+(6*0x100))
 __IOM uint32_t CH7_DAR_32                               ; //(0x10c+(6*0x100))
 __IOM uint32_t CH7_BLOCK_TS_0                           ; //(0x110+(6*0x100))
 __IOM uint32_t CH7_BLOCK_TS_32                          ; //(0x114+(6*0x100))
 __IOM uint32_t CH7_CTL_0                                ; //(0x118+(6*0x100))
 __IOM uint32_t CH7_CTL_32                               ; //(0x11c+(6*0x100))
 __IOM uint32_t CH7_CFG2_0                               ; //(0x120+(6*0x100))
 __IOM uint32_t CH7_CFG2_32                              ; //(0x124+(6*0x100))
 __IOM uint32_t CH7_LLP_0                                ; //(0x128+(6*0x100))
 __IOM uint32_t CH7_LLP_32                               ; //(0x12c+(6*0x100))
 __IOM uint32_t CH7_STATUSREG_0                          ; //(0x130+(6*0x100))
 __IOM uint32_t CH7_STATUSREG_32                         ; //(0x134+(6*0x100))
 __IOM uint32_t CH7_SWHSSRCREG_0                         ; //(0x138+(6*0x100))
 __IOM uint32_t CH7_SWHSSRCREG_32                        ; //(0x13c+(6*0x100))
 __IOM uint32_t CH7_SWHSDSTREG_0                         ; //(0x140+(6*0x100))
 __IOM uint32_t CH7_SWHSDSTREG_32                        ; //(0x144+(6*0x100))
 __IOM uint32_t CH7_BLK_TFR_RESUMEREQREG_0               ; //(0x148+(6*0x100))
 __IOM uint32_t CH7_BLK_TFR_RESUMEREQREG_32              ; //(0x14c+(6*0x100))
 __IOM uint32_t CH7_AXI_IDREG_0                          ; //(0x150+(6*0x100))
 __IOM uint32_t CH7_AXI_IDREG_32                         ; //(0x154+(6*0x100))
 __IOM uint32_t CH7_AXI_QOSREG_0                         ; //(0x158+(6*0x100))
 __IOM uint32_t CH7_AXI_QOSREG_32                        ; //(0x15c+(6*0x100))
 __IOM uint32_t CH7_SSTAT_0                              ; //(0x160+(6*0x100))
 __IOM uint32_t CH7_SSTAT_32                             ; //(0x164+(6*0x100))
 __IOM uint32_t CH7_DSTAT_0                              ; //(0x168+(6*0x100))
 __IOM uint32_t CH7_DSTAT_32                             ; //(0x16c+(6*0x100))
 __IOM uint32_t CH7_SSTATAR_0                            ; //(0x170+(6*0x100))
 __IOM uint32_t CH7_SSTATAR_32                           ; //(0x174+(6*0x100))
 __IOM uint32_t CH7_DSTATAR_0                            ; //(0x178+(6*0x100))
 __IOM uint32_t CH7_DSTATAR_32                           ; //(0x17c+(6*0x100))
 __IOM uint32_t CH7_INTSTATUS_ENABLEREG_0                ; //(0x180+(6*0x100))
 __IOM uint32_t CH7_INTSTATUS_ENABLEREG_32               ; //(0x184+(6*0x100))
 __IOM uint32_t CH7_INTSTATUS_0                          ; //(0x188+(6*0x100))
 __IOM uint32_t CH7_INTSTATUS_32                         ; //(0x18c+(6*0x100))
 __IOM uint32_t CH7_INTSIGNAL_ENABLEREG_0                ; //(0x190+(6*0x100))
 __IOM uint32_t CH7_INTSIGNAL_ENABLEREG_32               ; //(0x194+(6*0x100))
 __IOM uint32_t CH7_INTCLEARREG_0                        ; //(0x198+(6*0x100))
 __IOM uint32_t CH7_INTCLEARREG_32                       ; //(0x19c+(6*0x100))
          RESERVED(7[24], uint32_t)    

//channel 8 register
 __IOM uint32_t CH8_SAR_0                                ; //(0x100+(7*0x100))
 __IOM uint32_t CH8_SAR_32                               ; //(0x104+(7*0x100))
 __IOM uint32_t CH8_DAR_0                                ; //(0x108+(7*0x100))
 __IOM uint32_t CH8_DAR_32                               ; //(0x10c+(7*0x100))
 __IOM uint32_t CH8_BLOCK_TS_0                           ; //(0x110+(7*0x100))
 __IOM uint32_t CH8_BLOCK_TS_32                          ; //(0x114+(7*0x100))
 __IOM uint32_t CH8_CTL_0                                ; //(0x118+(7*0x100))
 __IOM uint32_t CH8_CTL_32                               ; //(0x11c+(7*0x100))
 __IOM uint32_t CH8_CFG2_0                               ; //(0x120+(7*0x100))
 __IOM uint32_t CH8_CFG2_32                              ; //(0x124+(7*0x100))
 __IOM uint32_t CH8_LLP_0                                ; //(0x128+(7*0x100))
 __IOM uint32_t CH8_LLP_32                               ; //(0x12c+(7*0x100))
 __IOM uint32_t CH8_STATUSREG_0                          ; //(0x130+(7*0x100))
 __IOM uint32_t CH8_STATUSREG_32                         ; //(0x134+(7*0x100))
 __IOM uint32_t CH8_SWHSSRCREG_0                         ; //(0x138+(7*0x100))
 __IOM uint32_t CH8_SWHSSRCREG_32                        ; //(0x13c+(7*0x100))
 __IOM uint32_t CH8_SWHSDSTREG_0                         ; //(0x140+(7*0x100))
 __IOM uint32_t CH8_SWHSDSTREG_32                        ; //(0x144+(7*0x100))
 __IOM uint32_t CH8_BLK_TFR_RESUMEREQREG_0               ; //(0x148+(7*0x100))
 __IOM uint32_t CH8_BLK_TFR_RESUMEREQREG_32              ; //(0x14c+(7*0x100))
 __IOM uint32_t CH8_AXI_IDREG_0                          ; //(0x150+(7*0x100))
 __IOM uint32_t CH8_AXI_IDREG_32                         ; //(0x154+(7*0x100))
 __IOM uint32_t CH8_AXI_QOSREG_0                         ; //(0x158+(7*0x100))
 __IOM uint32_t CH8_AXI_QOSREG_32                        ; //(0x15c+(7*0x100))
 __IOM uint32_t CH8_SSTAT_0                              ; //(0x160+(7*0x100))
 __IOM uint32_t CH8_SSTAT_32                             ; //(0x164+(7*0x100))
 __IOM uint32_t CH8_DSTAT_0                              ; //(0x168+(7*0x100))
 __IOM uint32_t CH8_DSTAT_32                             ; //(0x16c+(7*0x100))
 __IOM uint32_t CH8_SSTATAR_0                            ; //(0x170+(7*0x100))
 __IOM uint32_t CH8_SSTATAR_32                           ; //(0x174+(7*0x100))
 __IOM uint32_t CH8_DSTATAR_0                            ; //(0x178+(7*0x100))
 __IOM uint32_t CH8_DSTATAR_32                           ; //(0x17c+(7*0x100))
 __IOM uint32_t CH8_INTSTATUS_ENABLEREG_0                ; //(0x180+(7*0x100))
 __IOM uint32_t CH8_INTSTATUS_ENABLEREG_32               ; //(0x184+(7*0x100))
 __IOM uint32_t CH8_INTSTATUS_0                          ; //(0x188+(7*0x100))
 __IOM uint32_t CH8_INTSTATUS_32                         ; //(0x18c+(7*0x100))
 __IOM uint32_t CH8_INTSIGNAL_ENABLEREG_0                ; //(0x190+(7*0x100))
 __IOM uint32_t CH8_INTSIGNAL_ENABLEREG_32               ; //(0x194+(7*0x100))
 __IOM uint32_t CH8_INTCLEARREG_0                        ; //(0x198+(7*0x100))
 __IOM uint32_t CH8_INTCLEARREG_32                       ; //(0x19c+(7*0x100))
} DMAC_TypeDef;

#if defined A55
#define DMA                     ((DMAC_TypeDef *)DMAC1_BASE)
#elif defined M3// M3
#define DMA                     ((DMAC_TypeDef *)DMAC0_BASE)
#endif

#define AXI_MASTER_0                                    0x0
#define AXI_MASTER_1                                    0x1
#define SRC_ADDR_INCREMENT                              0x0
#define SRC_ADDR_NOCHANGE                               0x1
#define DST_ADDR_INCREMENT                              0x0
#define DST_ADDR_NOCHANGE                               0x1

#define SRC_TRANSFER_WIDTH_8                            0x0
#define SRC_TRANSFER_WIDTH_16                           0x1
#define SRC_TRANSFER_WIDTH_32                           0x2
#define SRC_TRANSFER_WIDTH_64                           0x3
#define SRC_TRANSFER_WIDTH_128                          0x4
#define SRC_TRANSFER_WIDTH_256                          0x5
#define SRC_TRANSFER_WIDTH_512                          0x6
#define DST_TRANSFER_WIDTH_8                            0x0
#define DST_TRANSFER_WIDTH_16                           0x1
#define DST_TRANSFER_WIDTH_32                           0x2
#define DST_TRANSFER_WIDTH_64                           0x3
#define DST_TRANSFER_WIDTH_128                          0x4
#define DST_TRANSFER_WIDTH_256                          0x5
#define DST_TRANSFER_WIDTH_512                          0x6

#define SRC_MSIZE_1                                     0x0
#define SRC_MSIZE_4                                     0x1
#define SRC_MSIZE_8                                     0x2
#define SRC_MSIZE_16                                    0x3
#define SRC_MSIZE_32                                    0x4
#define SRC_MSIZE_64                                    0x5
#define SRC_MSIZE_128                                   0x6
#define SRC_MSIZE_256                                   0x7
#define SRC_MSIZE_512                                   0x8
#define SRC_MSIZE_1024                                  0x9
#define DST_MSIZE_1                                     0x0
#define DST_MSIZE_4                                     0x1
#define DST_MSIZE_8                                     0x2
#define DST_MSIZE_16                                    0x3
#define DST_MSIZE_32                                    0x4
#define DST_MSIZE_64                                    0x5
#define DST_MSIZE_128                                   0x6
#define DST_MSIZE_256                                   0x7
#define DST_MSIZE_512                                   0x8
#define DST_MSIZE_1024                                  0x9

#define NONPOSTED_LASTWRITE_EN                          0x1
#define NONPOSTED_LASTWRITE_DISABLE                     0x0

#define ARLEN_EN                                        0x1
#define ARLEN_DISABLE                                   0x0
#define AWLEN_EN                                        0x1
#define AWLEN_DISABLE                                   0x0
#define SRC_STATUS_EN                                   0x1
#define SRC_STATUS_DISABLE                              0x0
#define DST_STATUS_EN                                   0x1
#define DST_STATUS_DISABLE                              0x0

#define INTEN_COMPLETOFBLKTRANS_SHADORLLI               0x1
#define INTDISABLE_COMPLETOFBLKTRANS_SHADORLLI          0x0
#define NOTLAST_SHADORLLI                               0x0
#define LAST_SHADORLLI                                  0x1

#define SHADORLLI_VALID                                 0x1
#define SHADORLLI_INVALID                               0x0

#define SRC_CONTIGUOUS                                  0x0
#define SRC_RELOAD                                      0x1
#define SRC_SHADOW_REGISTER                             0x2
#define SRC_LINKED_LIST                                 0x3
#define DST_CONTIGUOUS                                  0x0
#define DST_RELOAD                                      0x1
#define DST_SHADOW_REGISTER                             0x2
#define DST_LINKED_LIST                                 0x3

#define MEM_TO_MEM_DMAC                                 0x0
#define MEM_TO_PER_DMAC                                 0x1
#define PER_TO_MEM_DMAC                                 0x2
#define PER_TO_PER_DMAC                                 0x3
#define PER_TO_MEM_SRC                                  0x4
#define PER_TO_PER_SRC                                  0x5
#define MEM_TO_PER_DST                                  0x6
#define PER_TO_PER_DST                                  0x7

#define SRC_HARDWARE_HS                                 0x0
#define DST_HARDWARE_HS                                 0x0
#define SRC_SOFTWARE_HS                                 0x1
#define DST_SOFTWARE_HS                                 0x1
#define SRC_HWHS_HIGHPOL                                0x0
#define SRC_HWHS_LOWPOL                                 0x1
#define DST_HWHS_HIGHPOL                                0x0
#define DST_HWHS_LOWPOL                                 0x1

#define CHANNEL_PRIORITY0                               0x0
#define CHANNEL_PRIORITY1                               0x1
#define CHANNEL_PRIORITY2                               0x2
#define CHANNEL_PRIORITY3                               0x3
#define CHANNEL_PRIORITY4                               0x4
#define CHANNEL_PRIORITY5                               0x5
#define CHANNEL_PRIORITY6                               0x6
#define CHANNEL_PRIORITY7                               0x7
#define CHANNEL_PRIORITY8                               0x8
#define CHANNEL_PRIORITY9                               0x9

#define CHANNEL_LOCK_EN                                 0x1
#define CHANNEL_LOCK_DISABLE                            0x0
#define CHANNEL_LOCK_DMATRANSFER                        0x0
#define CHANNEL_LOCK_BLKTRANSFER                        0x1


#ifdef __cplusplus
    }
#endif
#endif
#endif
