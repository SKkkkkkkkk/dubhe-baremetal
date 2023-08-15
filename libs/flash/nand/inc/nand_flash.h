#ifndef __NAND_FLASH_H__
#define __NAND_FLASH_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "dw_apb_ssi.h"
#include "nand_flash_cmd.h"

// #define NAND_FLASH_READ_ONLY


typedef enum {
	UNKNOWN_FLASH = 0,
	GD5F1GQ4R	  = 0xC8C1,
	GD5F1GQ5RExxG = 0xC841,
} nand_flash_model_t;

bool nand_flash_init(spi_id_t spi_id, uint16_t clk_div, uint8_t spi_mode, nand_flash_model_t nand_flash_model);

void nand_flash_reset(spi_id_t spi_id);

void nand_flash_read_id(spi_id_t spi_id, uint8_t * const flash_id, uint8_t id_size);

uint8_t nand_get_feature(spi_id_t spi_id, uint8_t feature_addr);

void nand_set_feature(spi_id_t spi_id, uint8_t feature_addr, uint8_t feature);

int nand_flash_read(spi_id_t spi_id, uint32_t addr, uint8_t * buf, uint32_t buf_size);

#ifndef NAND_FLASH_READ_ONLY
int nand_flash_erase(spi_id_t spi_id, uint32_t addr);

int nand_flash_page_program(spi_id_t spi_id, uint32_t addr, uint8_t * const buf);
#endif

#ifdef __cplusplus
	}
#endif

#endif