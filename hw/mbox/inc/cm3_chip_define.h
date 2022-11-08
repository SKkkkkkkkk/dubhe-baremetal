#ifndef _CM3_CHIP_DEFINE_H_
#define _CM3_CHIP_DEFINE_H_
#ifdef __cplusplus
extern C {
#endif
#define reg64    *(uint64_t volatile *)(uintptr_t)
#define reg32    *(uint32_t volatile *)(uintptr_t)
#define reg16    *(uint16_t volatile *)(uintptr_t)
#define reg8     *(uint8_t volatile *)(uintptr_t)

#define word64(x,y)    *(unsigned long volatile *)(x)=y
#define word32(x,y)    *(unsigned int  volatile *)(x)=y
#define short16(x,y)   *(unsigned short volatile *)(x)=y
#define char8(x,y)     *(unsigned char volatile  *)(x)=y

#define access64(x)    *(unsigned long  volatile *)(x)
#define access32(x)    *(unsigned int   volatile *)(x)
#define access16(x)    *(unsigned short volatile *)(x)
#define access8(x)     *(unsigned char  volatile *)(x)

#define poll0(x,y)  while( reg32(x) & y ) ;
#define poll1(x,y)  while( (reg32(x) & y) == y );

#define udelay(x)   { unsigned xx = x ; while( xx-- ) { asm volatile("nop"); } }


//############################################

#define M3_BOOTROM0_BASE               0x000000000
#define A55_BOOTFLASH_BASE             0x001000000
#define M3_BOOTLASH_BASE               0x001020000
#define SPI_Flash_BASE                 0x001040000
#define M3_SYSRAM0_BASE                0x020000000
#define LP_CFG_BASE                    0x041000000
#define VAD_BASE                       0x041010000
#define PDM0_BASE                      0x041020000
#define Mailbox_BASE                   0x041030000
#define I2S0_BASE                      0x041050000
#define ADC_BASE                       0x041060000
#define TIMER0_X2_BASE                 0x041070000
#define I2C0_BASE                      0x041080000
#define UART0_BASE                     0x041090000
#define GPIO0_BASE                     0x0410A0000
#define DMAC0_BASE                     0x041100000
#define GMAC_BASE                      0x045000000
#define eMMC_BASE                      0x045010000
#define SD_BASE                        0x045020000
#define SDIO_BASE                      0x045030000
#define USB_BASE                       0x045100000
#define HIPERI_REGS_BASE               0x045090000
#define PKA_BASE                       0x0450A0000
#define EFUSE_BASE                     0x0450B0000
#define SPACC_BASE                     0x0450C0000
#define BOOTSPI_BASE                   0x0450D0000
#define USB_BASE                       0x045100000
#define GPIO1_BASE                     0x047000000
#define GPIO2_BASE                     0x047010000
#define GPIO3_BASE                     0x047020000
#define MST_SPI0_BASE                  0x047030000
#define MST_SPI1_BASE                  0x047040000
#define MST_SPI2_BASE                  0x047050000
#define SLV_SPI2_BASE                  0x047060000
#define I2C1_BASE                      0x047070000
#define I2C2_BASE                      0x047080000
#define I2C3_BASE                      0x047090000
#define UART1_BASE                     0x0470A0000
#define UART2_BASE                     0x0470B0000
#define UART3_BASE                     0x0470C0000
#define PWM0_BASE                      0x0470D0000
#define PWM1_BASE                      0x0470E0000
#define PWM2_BASE                      0x0470F0000
#define PWM3_BASE                      0x047100000
#define I2S1_BASE                      0x047110000
#define I2S2_BASE                      0x047120000
#define PDM1_BASE                      0x047130000
#define WDT_BASE                       0x047140000
#define NPU0_BASE                      0x048000000
#define NPU1_BASE                      0x048800000
#define VPU_BASE                       0x049000000
#define GPU_BASE                       0x04A000000
#define DDR_CTRL0_BASE                 0x04B000000
#define PMON0_BASE                     0x04B010000
#define DDR_CTRL1_BASE                 0x04B100000
#define PMON1_BASE                     0x04B110000
#define ISP_BASE                       0x04C000000
#define DEWARP_BASE                    0x04C020000
#define DC_BASE                        0x04C040000
#define IMAGE_CFG_BASE                 0x04C050000
#define MIPI_RX_CSI2_0_BASE            0x04C060000
#define MIPI_RX_CSI2_1_BASE            0x04C070000
#define MIPI_RX_CSI2_2_BASE            0x04C080000
#define MIPI_RX_CSI2_3_BASE            0x04C090000
#define MIPI_TX_DSI_BASE               0x04C0A0000
#define MIPI_TX_CSI_BASE               0x04C0B0000
#define IDI_BRIDGE_BASE                0x04C0C0000
#define IDMA_BASE                      0x04C0D0000
#define ROT_YUV_BASE                   0x04C0E0000
#define ROT_RAW_BASE                   0x04C0F0000
#define SCALAR_BASE                    0x04C100000
#define SYSCTRL_BASE                   0x04E000000
#define PMU_BASE                       0x04E010000
#define GENERIC_TIMER_BASE             0x04E020000
#define RTC_BASE                       0x04E030000
#define DDR_BASE                       0x060000000
#define SOC_BASE                       0x040000000
#define SYS_BASE                       SYSCTRL_BASE


//############################################

#ifdef __cplusplus
    }
#endif

#endif
