#include <assert.h>
#include <string.h>
#include "nand_flash.h"

#define SPI_NUMS 4
#define DUMMY_BYTE 0xFF

static nand_flash_model_t nand_flash_model[SPI_NUMS] = {UNKNOWN_FLASH};

bool nand_flash_init(spi_id_t spi_id, uint16_t clk_div, uint8_t spi_mode, nand_flash_model_t _nand_flash_model)
{
	spi_init_config_t spi_init_config = {
		.as_master = true,
		.clock_div = clk_div,
		.spi_id = spi_id,
		.spi_mode = spi_mode
	};
	dw_spi_init(&spi_init_config);

	if(_nand_flash_model == UNKNOWN_FLASH)
	{
		nand_flash_model[spi_id] = UNKNOWN_FLASH;
		return true;
	}

	int16_t dw_spi_tuning(spi_id_t spi_id, void *t_buf, uint8_t t_size, void *r_buf, uint16_t r_size);
	uint8_t tuning_buf_t[1] = {CMD_NAND_READ_ID};
	uint8_t tuning_buf_r[3] = {0xff, ((_nand_flash_model>>8)&0xff), ((_nand_flash_model)&0xff)};
	int16_t tuning = dw_spi_tuning(spi_id, tuning_buf_t, 1, tuning_buf_r, 3);
	if(tuning==-1)
		return false;
	nand_flash_model[spi_id] = _nand_flash_model;
	return true;
}


void nand_flash_read_id(spi_id_t spi_id, uint8_t * const flash_id, uint8_t id_size)
{
	assert(flash_id);
	uint8_t cmd_read_id = CMD_NAND_READ_ID;
	dw_spi_eeprom_read(spi_id, &cmd_read_id, 1, flash_id, id_size);
	return;
}


uint8_t nand_get_feature(spi_id_t spi_id, uint8_t feature_addr)
{
	if(feature_addr != FEATURE_REG_NAND_BLKLOCK_REG_ADDR 
		&& feature_addr != FEATURE_REG_NAND_CONFIGURATION_REG_ADDR 
		&& feature_addr != FEATURE_REG_NAND_STATUS_REG_ADDR 
		&& feature_addr != FEATURE_REG_NAND_DIE_SELECT_REC_ADDR)
		return 0xff;
	uint8_t cmd_get_feature[2] = {CMD_NAND_GET_FEATURE_INS, feature_addr};
	uint8_t feature;
	dw_spi_eeprom_read(spi_id, &cmd_get_feature, 2, &feature, 1);
	return feature;
}

void nand_set_feature(spi_id_t spi_id, uint8_t feature_addr, uint8_t feature)
{
	if(feature_addr != FEATURE_REG_NAND_BLKLOCK_REG_ADDR 
		&& feature_addr != FEATURE_REG_NAND_CONFIGURATION_REG_ADDR 
		/*&& feature_addr != FEATURE_REG_NAND_STATUS_REG_ADDR */
		&& feature_addr != FEATURE_REG_NAND_DIE_SELECT_REC_ADDR)
		return;
	uint8_t cmd_set_feature[3] = {CMD_NAND_SET_FEATURE, feature_addr, feature};
	dw_spi_transmit_only(spi_id, cmd_set_feature, 3, true);
	return;
}

void nand_flash_reset(spi_id_t spi_id)
{
	uint8_t cmd_reset = CMD_NAND_RESET;
	dw_spi_transmit_only(spi_id, &cmd_reset, 1, true);
	return;
}

static bool isFlashBusy(spi_id_t spi_id)
{
	uint8_t status = nand_get_feature(spi_id, FEATURE_REG_NAND_STATUS_REG_ADDR);
	return status&1 ? true : false;
}

static int nand_flash_page_read(spi_id_t spi_id, uint32_t addr, uint8_t * const buf, uint32_t buf_size)
{
	assert(buf);
	if(buf_size == 0)
		return 0;
	if(((addr&(NAND_PAGE_SIZE-1)) + buf_size) > NAND_PAGE_SIZE)
		return -1;
	
	// 1. page read to cache
	uint32_t page_addr = addr/NAND_PAGE_SIZE;
	uint8_t cmd_page_read[4] = {CMD_NAND_PAGE_READ_INS, (page_addr>>16)&0xff, (page_addr>>8)&0xff, (page_addr)&0xff};
	dw_spi_transmit_only(spi_id, cmd_page_read, 4, true);

	// 2. read the status
	while(isFlashBusy(spi_id));

	// 3. read cache
	uint8_t cmd_read_cache[4] = {CMD_NAND_READ_CACHE_INS, (addr>>8)&0x7, (addr)&0xff, DUMMY_BYTE};
	dw_spi_eeprom_read_dma(spi_id, cmd_read_cache, 4, buf, buf_size);
	return 0;
}

int nand_flash_read(spi_id_t spi_id, uint32_t addr, uint8_t * buf, uint32_t buf_size)
{
	assert(buf);
	if(buf_size == 0)
		return 0;
	if(((addr&(NAND_PAGE_SIZE-1)) + buf_size) <= NAND_PAGE_SIZE)
		return nand_flash_page_read(spi_id, addr, buf, buf_size);

	uint32_t remain = buf_size;
	uint32_t addr1 = addr;
	uint32_t size1 = NAND_PAGE_SIZE - (addr&(NAND_PAGE_SIZE-1));

	do
	{
		nand_flash_page_read(spi_id, addr1, buf, size1);
		remain -= size1;
		addr1 += size1;
		buf += size1;
		size1 = remain>NAND_PAGE_SIZE ? NAND_PAGE_SIZE : remain;
	} while (remain>0);
	
	return 0;
}

#ifndef NAND_FLASH_READ_ONLY

static bool isWriteEnable(spi_id_t spi_id)
{
	uint8_t status = nand_get_feature(spi_id, FEATURE_REG_NAND_STATUS_REG_ADDR);
	return status&2 ? true : false;
}

static bool isProgramFail(spi_id_t spi_id)
{
	uint8_t status = nand_get_feature(spi_id, FEATURE_REG_NAND_STATUS_REG_ADDR);
	return status&0x08 ? true : false;
}


int nand_flash_erase(spi_id_t spi_id, uint32_t addr)
{
	if((addr&(NAND_BLOCK_SIZE-1)) != 0)
		return -1;
	
	//1. write enable
	uint8_t cmd_write_enable = CMD_NAND_WRITE_ENABLE;
	dw_spi_transmit_only(spi_id, &cmd_write_enable, 1, true);
	while(!isWriteEnable(spi_id));

	//2. block erase
	uint32_t block_addr = addr/NAND_PAGE_SIZE; // row address
	uint8_t cmd_block_erase[4] = {CMD_NAND_BLOCK_ERASE_INS, (block_addr>>16)&0xff, (block_addr>>8)&0xff, (block_addr)&0xff};
	dw_spi_transmit_only(spi_id, cmd_block_erase, 4, true);

	//3. read the status
	while(isFlashBusy(spi_id));

	//4. check the status
	if(isProgramFail(spi_id))
		return -1;
	
	return 0;
}

int nand_flash_page_program(spi_id_t spi_id, uint32_t addr, uint8_t * const buf)
{
	assert(buf);
	
	// 1. program load
	static uint8_t cmd_program_load[3+NAND_PAGE_SIZE]; // static to avoid stack overflow
	cmd_program_load[0] = CMD_NAND_PROGRAM_LOAD_INS;
	cmd_program_load[1] = (addr>>8)&0x07;
	cmd_program_load[2] = addr&0xff;
	memcpy(&cmd_program_load[3], buf, NAND_PAGE_SIZE);
	dw_spi_transmit_only(spi_id, cmd_program_load, 3+NAND_PAGE_SIZE, false);

	//2. write enable
	uint8_t cmd_write_enable = CMD_NAND_WRITE_ENABLE;
	dw_spi_transmit_only(spi_id, &cmd_write_enable, 1, true);
	while(!isWriteEnable(spi_id));

	//3. program execute
	uint32_t page_addr = addr/NAND_PAGE_SIZE;
	uint8_t cmd_program_execute[4] = {CMD_NAND_PROGRAM_EXEC_INS, (page_addr>>16)&0xff, (page_addr>>8)&0xff, (page_addr)&0xff};
	dw_spi_transmit_only(spi_id, cmd_program_execute, 4, true);

	//4. read the status
	while(isFlashBusy(spi_id));

	//5. check the status
	if(isProgramFail(spi_id))
		return -1;

	return 0;
}

#endif