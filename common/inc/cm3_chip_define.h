#ifndef _CM3_CHIP_DEFINE_H_
#define _CM3_CHIP_DEFINE_H_

#ifdef __cplusplus
extern C {
#endif

#include <utils_def.h>
//############################################

#define M3_BOOTROM0_BASE               UL(0x000000000)
#define A55_BOOTFLASH_BASE             UL(0x001000000)
#define M3_BOOTLASH_BASE               UL(0x001020000)
#define SPI_Flash_BASE                 UL(0x001040000)
#define SYSRAM_BASE                    UL(0x020000000)
#define LP_CFG_BASE                    UL(0x041000000)
#define VAD_BASE                       UL(0x041010000)
#define PDM0_BASE                      UL(0x041020000)
#define PDM1_BASE                      UL(0x041030000)
#define Mailbox_BASE                   UL(0x041040000)
#define I2S0_BASE                      UL(0x041050000)
#define PDM2_BASE                      UL(0x041060000)
#define TIMERX2_BASE                   UL(0x041070000)
#define I2C0_BASE                      UL(0x041080000)
#define UART0_BASE                     UL(0x041090000)
#define WDT0_BASE                      UL(0x0410A0000)
#define PDM3_BASE                      UL(0x0410B0000)
#define DMAC0_BASE                     UL(0x041100000)
#define EMMC0_BASE                     UL(0x045000000)
#define EMMC1_BASE                     UL(0x045010000)
#define SDIO2_BASE                     UL(0x045020000)
#define GPIO1_BASE                     UL(0x045030000)
#define GPIO2_BASE                     UL(0x045040000)
#define I2S2_BASE                      UL(0x045050000)
#define BOOTSPI_BASE                   UL(0x045060000)
#define UART1_BASE                     UL(0x045070000)
#define UART2_BASE                     UL(0x045080000)
#define I2C1_BASE                      UL(0x045090000)
#define I2C2_BASE                      UL(0x0450A0000)
#define I2C3_BASE                      UL(0x0450B0000)
#define PWM0_BASE                      UL(0x0450C0000)
#define I2S1_BASE                      UL(0x0450D0000)
#define PDM4_BASE                      UL(0x0450E0000)
#define PDM5_BASE                      UL(0x0450F0000)
#define PERI0_REGS_BASE                UL(0x045100000)
#define I2C4_BASE                      UL(0x045110000)
#define GMAC_BASE                      UL(0x046000000)
#define SPACC_BASE                     UL(0x046010000)
#define PKA_BASE                       UL(0x046020000)
#define SPI0_BASE                  UL(0x046030000)
#define SPI1_BASE                  UL(0x046040000)
#define SPI2_BASE                  UL(0x046050000)
#define SPIS_BASE                  UL(0x046060000)
#define PWM1_BASE                      UL(0x046070000)
#define PWM2_BASE                      UL(0x046080000)
#define PWM3_BASE                      UL(0x046090000)
#define UART3_BASE                     UL(0x0460A0000)
#define EFUSE_BASE                     UL(0x0460B0000)
#define PERI1_REGS_BASE                UL(0x0460C0000)
#define USB_BASE                       UL(0x046100000)
#define PWM4_BASE                      UL(0x046200000)
#define PWM5_BASE                      UL(0x046210000)
#define PWM6_BASE                      UL(0x046220000)
#define PWM7_BASE                      UL(0x046230000)
#define I2C5_BASE                      UL(0x046240000)
#define I2C6_BASE                      UL(0x046250000)
#define UART4_BASE                     UL(0x046260000)
#define UART5_BASE                     UL(0x046270000)
#define UART6_BASE                     UL(0x046280000)
#define UART7_BASE                     UL(0x046290000)
#define I2S3_BASE                      UL(0x0462A0000)
#define NPU0_BASE                      UL(0x048000000)
#define NPU1_BASE                      UL(0x048800000)
#define VPU_BASE                       UL(0x049000000)
#define VPU_REGS_BASE                  UL(0x049100000)
#define GPU_BASE                       UL(0x04A000000)
#define GPU_REGS_BASE                  UL(0x04A100000)
#define DDR_CTRL0_BASE                 UL(0x04B000000)
#define PMON0_BASE                     UL(0x04B010000)
#define DDR_CTRL1_BASE                 UL(0x04B100000)
#define PMON1_BASE                     UL(0x04B110000)
#define ISP_BASE                       UL(0x04C000000)
#define DEWARP_BASE                    UL(0x04C020000)
#define IMAGE_CFG_BASE                 UL(0x04C050000)
#define MIPI_RX_CSI2_0_BASE            UL(0x04C060000)
#define MIPI_RX_CSI2_1_BASE            UL(0x04C070000)
#define MIPI_RX_CSI2_2_BASE            UL(0x04C080000)
#define MIPI_RX_CSI2_3_BASE            UL(0x04C090000)
#define MIPI_TX_DSI_BASE               UL(0x04C0A0000)
#define MIPI_TX_CSI_BASE               UL(0x04C0B0000)
#define IDI_BRIDGE_BASE                UL(0x04C0C0000)
#define IDMA_BASE                      UL(0x04C0D0000)
#define LCDC_BASE                      UL(0x04C100000)
#define ROT_RAW_BASE                   UL(0x04C140000)
#define ROT_YUV_BASE                   UL(0x04C150000)
#define SYSCTRL_BASE                   UL(0x04E000000)
#define GPIO0_BASE                     UL(0x04E010000)
#define SYSTEMCOUNTER_BASE             UL(0x04E020000)
#define RTC_BASE                       UL(0x04E030000)
#define PMU_BASE                       UL(0x04E040000)
#define ADC_BASE                       UL(0x04E060000)
#define CLCI_BASE                      UL(0x04E070000)
#define DDR_BASE                       UL(0x060000000)
#define SOC_BASE                       UL(0x040000000)
#define SYS_BASE                       SYSCTRL_BASE


//############################################

#ifdef __cplusplus
    }
#endif

#endif
