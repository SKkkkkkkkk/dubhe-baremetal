#ifndef _CA55_CHIP_DEFINE_H_
#define _CA55_CHIP_DEFINE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <utils_def.h>
//############################################

#ifndef QEMU

#define A55_BOOTROM1_BASE              UL(0x000000000)
#define AXI_SRAM					   UL(0x000100000)
#define A55_BOOTFLASH_BASE             UL(0x001000000)
#define M3_BOOTLASH_BASE               UL(0x001020000)
#define SPI_Flash_BASE                 UL(0x001040000)
#define SYSRAM_BASE                    UL(0x020000000)
#define LP_CFG_BASE                    UL(0x021000000)
#define VAD_BASE                       UL(0x021010000)
#define PDM0_BASE                      UL(0x021020000)
#define PDM1_BASE                      UL(0x021030000)
#define Mailbox_BASE                   UL(0x021040000)
#define I2S0_BASE                      UL(0x021050000)
#define PDM2_BASE                      UL(0x021060000)
#define TIMERX2_BASE                   UL(0x021070000)
#define I2C0_BASE                      UL(0x021080000)
#define UART0_BASE                     UL(0x021090000)
#define WDT0_BASE                      UL(0x0210A0000)
#define PDM3_BASE                      UL(0x0210B0000)
#define DMAC1_BASE                     UL(0x024000000)
#define GIC_BASE                       UL(0x024010000)
#define DAP_LATE_BASE                  UL(0x024030000)
#define TIMERX6_BASE                   UL(0x024040000)
#define WDT1_BASE                      UL(0x024050000)
#define AP_CFG_BASE                    UL(0x024060000)
#define EMMC0_BASE                     UL(0x025000000)
#define EMMC1_BASE                     UL(0x025010000)
#define SDIO2_BASE                     UL(0x025020000)
#define GPIO1_BASE                     UL(0x025030000)
#define GPIO2_BASE                     UL(0x025040000)
#define I2S2_BASE                      UL(0x025050000)
#define BOOTSPI_BASE                   UL(0x025060000)
#define UART1_BASE                     UL(0x025070000)
#define UART2_BASE                     UL(0x025080000)
#define I2C1_BASE                      UL(0x025090000)
#define I2C2_BASE                      UL(0x0250A0000)
#define I2C3_BASE                      UL(0x0250B0000)
#define PWM0_BASE                      UL(0x0250C0000)
#define I2S1_BASE                      UL(0x0250D0000)
#define PDM4_BASE                      UL(0x0250E0000)
#define PDM5_BASE                      UL(0x0250F0000)
#define PERI0_REGS_BASE                UL(0x025100000)
#define I2C4_BASE                      UL(0x025110000)
#define GMAC_BASE                      UL(0x026000000)
#define SPACC_BASE                     UL(0x026010000)
#define PKA_BASE                       UL(0x026020000)
#define SPI0_BASE                      UL(0x026030000)
#define SPI1_BASE                      UL(0x026040000)
#define SPI2_BASE                      UL(0x026050000)
#define SPIS_BASE                      UL(0x026060000)
#define PWM1_BASE                      UL(0x026070000)
#define PWM2_BASE                      UL(0x026080000)
#define PWM3_BASE                      UL(0x026090000)
#define UART3_BASE                     UL(0x0260A0000)
#define EFUSE_BASE                     UL(0x0260B0000)
#define PERI1_REGS_BASE                UL(0x0260C0000)
#define USB_BASE                       UL(0x026100000)
#define PWM4_BASE                      UL(0x026200000)
#define PWM5_BASE                      UL(0x026210000)
#define PWM6_BASE                      UL(0x026220000)
#define PWM7_BASE                      UL(0x026230000)
#define I2C5_BASE                      UL(0x026240000)
#define I2C6_BASE                      UL(0x026250000)
#define UART4_BASE                     UL(0x026260000)
#define UART5_BASE                     UL(0x026270000)
#define UART6_BASE                     UL(0x026280000)
#define UART7_BASE                     UL(0x026290000)
#define I2S3_BASE                      UL(0x0262A0000)
#define NPU0_BASE                      UL(0x028000000)
#define NPU1_BASE                      UL(0x028800000)
#define VPU_BASE                       UL(0x029000000)
#define VPU_REGS_BASE                  UL(0x029100000)
#define GPU_BASE                       UL(0x02A000000)
#define GPU_REGS_BASE                  UL(0x02A100000)
#define DDR_CTRL0_BASE                 UL(0x02B000000)
#define PMON0_BASE                     UL(0x02B010000)
#define DDR_CTRL1_BASE                 UL(0x02B100000)
#define PMON1_BASE                     UL(0x02B110000)
#define ISP_BASE                       UL(0x02C000000)
#define DEWARP_BASE                    UL(0x02C020000)
#define IMAGE_CFG_BASE                 UL(0x02C050000)
#define MIPI_RX_CSI2_0_BASE            UL(0x02C060000)
#define MIPI_RX_CSI2_1_BASE            UL(0x02C070000)
#define MIPI_RX_CSI2_2_BASE            UL(0x02C080000)
#define MIPI_RX_CSI2_3_BASE            UL(0x02C090000)
#define MIPI_TX_DSI_BASE               UL(0x02C0A0000)
#define MIPI_TX_CSI_BASE               UL(0x02C0B0000)
#define IDI_BRIDGE_BASE                UL(0x02C0C0000)
#define IDMA_BASE                      UL(0x02C0D0000)
#define LCDC_BASE                      UL(0x02C100000)
#define ROT_RAW_BASE                   UL(0x02C140000)
#define ROT_YUV_BASE                   UL(0x02C150000)
#define SYSCTRL_BASE                   UL(0x02E000000)
#define GPIO0_BASE                     UL(0x02E010000)
#define SYSTEMCOUNTER_BASE             UL(0x02E020000)
#define RTC_BASE                       UL(0x02E030000)
#define PMU_BASE                       UL(0x02E040000)
#define ADC_BASE                       UL(0x02E060000)
#define CLCI_BASE                      UL(0x02E070000)
#define I2C_WO_BASE                    UL(0x02E080000)
#define DDR_BASE                       UL(0x040000000)
#define SOC_BASE                       UL(0x000000000)
#define SYS_BASE                       SYSCTRL_BASE

#else

#define VIRT_FLASH          UL(0x00000000)
#define VIRT_GIC_DIST       UL(0x08000000)
#define VIRT_GIC_CPU        UL(0x08010000)
#define VIRT_UART           UL(0x09000000)
#define VIRT_SECURE_UART    UL(0x09040000)
#define VIRT_SECURE_MEM     UL(0x0e000000)
#define VIRT_MEM            UL(0x40000000)

#endif

//############################################

#ifdef __cplusplus
    }
#endif

#include "regs_type.h"
#endif
