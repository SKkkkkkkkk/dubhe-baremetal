#ifndef __DW_APB_SSI_REGS_H__
#define __DW_APB_SSI_REGS_H__

#include "regs_type.h"
typedef struct {
	__IOM uint32_t MAP;
	__IOM uint32_t MAP_BAUDR;
	__IOM uint32_t MAP_SER;
	RESERVED(0 [61], uint32_t)
	__IOM uint32_t CTRLR0;
	__IOM uint32_t CTRLR1;
	__IOM uint32_t SSIENR;
	__IOM uint32_t MWCR;
	__IOM uint32_t SER;
	__IOM uint32_t BAUDR;
	__IOM uint32_t TXFTLR;
	__IOM uint32_t RXFTLR;
	__IOM uint32_t TXFLR;
	__IOM uint32_t RXFLR;
	__IOM uint32_t SR;
	__IOM uint32_t IMR;
	__IOM uint32_t ISR;
	__IOM uint32_t RISR;
	__IOM uint32_t TXOICR;
	__IOM uint32_t RXOICR;
	__IOM uint32_t RXUICR;
	__IOM uint32_t MSTICR;
	__IOM uint32_t ICR;
	__IOM uint32_t DMACR;
	__IOM uint32_t DMATDLR;
	__IOM uint32_t DMARDLR;
	__IOM uint32_t IDR;
	__IOM uint32_t SSI_VERSION_ID;
	__IOM uint32_t DR;
	RESERVED(1 [35], uint32_t)
	__IOM uint32_t RX_SAMPLE_DLY;
	__IOM uint32_t SPI_CTRLR0;
	__IOM uint32_t TXD_DRIVE_EDGE;
	__IOM uint32_t RSVD;
} BOOTSPI_TypeDef;

typedef struct {
	__IOM uint32_t CTRLR0;
	__IOM uint32_t CTRLR1;
	__IOM uint32_t SSIENR;
	__IOM uint32_t MWCR;
	__IOM uint32_t SER;
	__IOM uint32_t BAUDR;
	__IOM uint32_t TXFTLR;
	__IOM uint32_t RXFTLR;
	__IOM uint32_t TXFLR;
	__IOM uint32_t RXFLR;
	__IOM uint32_t SR;
	__IOM uint32_t IMR;
	__IOM uint32_t ISR;
	__IOM uint32_t RISR;
	__IOM uint32_t TXOICR;
	__IOM uint32_t RXOICR;
	__IOM uint32_t RXUICR;
	__IOM uint32_t MSTICR;
	__IOM uint32_t ICR;
	__IOM uint32_t DMACR;
	__IOM uint32_t DMATDLR;
	__IOM uint32_t DMARDLR;
	__IOM uint32_t IDR;
	__IOM uint32_t SSI_VERSION_ID;
	__IOM uint32_t DR;
	RESERVED(1 [35], uint32_t)
	__IOM uint32_t RX_SAMPLE_DLY;
	__IOM uint32_t SPI_CTRLR0;
	__IOM uint32_t TXD_DRIVE_EDGE;
	__IOM uint32_t RSVD;
} DW_APB_SSI_TypeDef;

#define SPI_FRF_Pos 4UL
#define SPI_FRF_Msk (3UL << SPI_FRF_Pos)

#define SPI_SCPH_Pos 6UL
#define SPI_SCPH_Msk (1UL << SPI_SCPH_Pos)

#define SPI_SCPOL_Pos 7UL
#define SPI_SCPOL_Msk (1UL << SPI_SCPOL_Pos)

#define SPI_TMOD_Pos 8UL
#define SPI_TMOD_Msk (3UL << SPI_TMOD_Pos)

#define SPI_SLV_OE_Pos 10UL
#define SPI_SLV_OE_Msk (1UL << SPI_SLV_OE_Pos)

#define SPI_SRL_Pos 11UL
#define SPI_SRL_Msk (1UL << SPI_SRL_Pos)

#define SPI_CFS_Pos 12UL
#define SPI_CFS_Msk (15UL << SPI_CFS_Pos)

#define SPI_DFS_32_Pos 16UL
#define SPI_DFS_32_Msk (31UL << SPI_DFS_32_Pos)

#define SPI_SPI_FRF_Pos 21UL
#define SPI_SPI_FRF_Msk (3UL << SPI_SPI_FRF_Pos)

#define SPI_SSTE_Pos 24UL
#define SPI_SSTE_Msk (1UL << SPI_SSTE_Pos)

#define SPI_BUSY_Pos 0UL
#define SPI_BUSY_Msk (1UL << SPI_BUSY_Pos)

#define SPI_TFNF_Pos 1UL
#define SPI_TFNF_Msk (1UL << SPI_TFNF_Pos)

#define SPI_TFE_Pos 2UL
#define SPI_TFE_Msk (1UL << SPI_TFE_Pos)

#define SPI_RFNE_Pos 3UL
#define SPI_RFNE_Msk (1UL << SPI_RFNE_Pos)

#define SPI_RFF_Pos 4UL
#define SPI_RFF_Msk (1UL << SPI_RFF_Pos)

#define SPI_TXE_Pos 5UL
#define SPI_TXE_Msk (1UL << SPI_TXE_Pos)

#define SPI_DCOL_Pos 6UL
#define SPI_DCOL_Msk (1UL << SPI_DCOL_Pos)

#define SPI_TRANS_TYPE_Pos 0UL
#define SPI_TRANS_TYPE_Msk (3UL << SPI_TRANS_TYPE_Pos)

#define SPI_ADDR_L_Pos 2UL
#define SPI_ADDR_L_Msk (15UL << SPI_ADDR_L_Pos)

#define SPI_INST_L_Pos 8UL
#define SPI_INST_L_Msk (3UL << SPI_INST_L_Pos)

#define SPI_WAIT_CYCLES_Pos 11UL
#define SPI_WAIT_CYCLES_Msk (31UL << SPI_WAIT_CYCLES_Pos)

#define SPI_SPI_DDR_EN_Pos 16UL
#define SPI_SPI_DDR_EN_Msk (1UL << SPI_SPI_DDR_EN_Pos)

#define SPI_INST_DDR_EN_Pos 17UL
#define SPI_INST_DDR_EN_Msk (1UL << SPI_INST_DDR_EN_Pos)

#define SPI_SPI_RXDS_EN_Pos 18UL
#define SPI_SPI_RXDS_EN_Msk (1UL << SPI_SPI_RXDS_EN_Pos)

#endif