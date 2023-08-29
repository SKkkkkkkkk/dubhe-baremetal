#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "dw_apb_ssi.h"
#include "nor_flash_port.h"
// #include "ape1210_gpio.h"
#include "nor_flash.h"
#include "systimer.h" /* for flash_reset */

#define SPI_TX_FIFO_DEPTH 32U
#define SPI_RX_FIFO_DEPTH 32U
#define DMA_FIFO_DEPTH 2048U

#define CPU_READ_BLOCK_SIZE SPI_RX_FIFO_DEPTH
#define BLOCK_SIZE_DMA DMA_FIFO_DEPTH

#define SPI_NUMS 4

static flash_model_t flash_model[SPI_NUMS] = {UNKNOWN_FLASH};

bool flash_init(spi_id_t spi_id, uint16_t clk_div, uint8_t spi_mode, flash_model_t _flash_model)
{
	spi_init_config_t spi_init_config = {
		.as_master = true,
		.clock_div = clk_div,
		.spi_id = spi_id,
		.spi_mode = spi_mode
	};
	dw_spi_init(&spi_init_config);

	if(_flash_model == UNKNOWN_FLASH)
	{
		flash_model[spi_id] = UNKNOWN_FLASH;
		return true;
	}
		

	int16_t dw_spi_tuning(spi_id_t spi_id, void *t_buf, uint8_t t_size, void *r_buf, uint16_t r_size);
	uint8_t tuning_buf_t[1] = {0x9f};
	uint8_t tuning_buf_r[3] = {((_flash_model>>16)&0xff), ((_flash_model>>8)&0xff), ((_flash_model)&0xff)};
	int16_t tuning = dw_spi_tuning(spi_id, tuning_buf_t, 1, tuning_buf_r, 3);
	if(tuning==-1)
		return false;
	flash_model[spi_id] = _flash_model;
	return true;
}

void flash_deinit(spi_id_t spi_id)
{
	dw_spi_deinit(spi_id, true);
	flash_model[spi_id] = UNKNOWN_FLASH;
	return;
}

void flash_read_id(spi_id_t spi_id, uint8_t * const flash_id, uint8_t id_size)
{
	assert(flash_id);
	uint8_t cmd_read_id = Flash_JedecDeviceID;
	dw_spi_eeprom_read(spi_id, &cmd_read_id, 1, flash_id, id_size);
	return;
}

static inline uint8_t _flash_read_sr(spi_id_t spi_id)
{
	uint8_t cmd = Flash_ReadStatusReg;
	uint8_t sr;
	dw_spi_eeprom_read(spi_id, &cmd, 1, &sr, 1);
	return sr;
}

static inline uint8_t _flash_read_extended_address_reg(spi_id_t spi_id)
{
	assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
	uint8_t cmd = Flash_Read_Extended_Address_Register;
	uint8_t sr;
	dw_spi_eeprom_read(spi_id, &cmd, 1, &sr, 1);
	return sr;
}

static inline bool _flash_is_busy(spi_id_t spi_id)
{
	return (_flash_read_sr(spi_id) & (1 << WRITE_IN_PROGRESS))? true:false;
}

static inline bool _flash_is_enable_write(spi_id_t spi_id)
{
	return (_flash_read_sr(spi_id) & (1 << WRITE_ENABLE_LATCH))? true:false;
}

static inline void _flash_write_enable(spi_id_t spi_id)
{
	uint8_t cmd = Flash_WriteEnable;
	dw_spi_transmit_only(spi_id, &cmd, 1, 1);

	while(!_flash_is_enable_write(spi_id));

	return;
}

static inline void _flash_write_disable(spi_id_t spi_id)
{
	uint8_t cmd = Flash_WriteDisable;
	dw_spi_transmit_only(spi_id, &cmd, 1, 1);

	while(_flash_is_enable_write(spi_id));

	return;
}

static inline void _flash_write_extended_address_reg(spi_id_t spi_id, uint8_t ext_ar)
{
	assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
	_flash_write_enable(spi_id);
	ext_ar &= 1;
	uint8_t cmd[2] = {
		Flash_Write_Extended_Address_Register,
		ext_ar
	};
	dw_spi_transmit_only(spi_id, &cmd, 2, 1);

	while(_flash_read_extended_address_reg(spi_id)!=ext_ar);
	return;
}

static inline void _flash_enter_4b_addr_mode(spi_id_t spi_id)
{
	assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
	uint8_t cmd = Flash_Enter_4Byte_Address_Mode;
	dw_spi_transmit_only(spi_id, &cmd, 1, 1); 
	/*需要读flash状态寄存器确定是否进入4B Mode吗？*/
	return;
}

static inline void _flash_exit_4b_addr_mode(spi_id_t spi_id)
{
	assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
	uint8_t cmd = Flash_Exit_4Byte_Address_Mode;
	dw_spi_transmit_only(spi_id, &cmd, 1, 1);
	/*需要读flash状态寄存器确定是否退出4B Mode吗？*/
	return;
}


static inline void _flash_read(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size)
{
	assert(buf);
	assert(size <= CPU_READ_BLOCK_SIZE);
	if(size==0) return;

	uint8_t cmd[4];
	uint8_t ext_ar;
	cmd[0] = Flash_ReadData;
	cmd[1] = (addr & 0x00FF0000UL) >> 16UL;
	cmd[2] = (addr & 0x0000FF00UL) >> 8UL;
	cmd[3] = (addr & 0x000000FFUL) >> 0UL;

	if((addr+size-1)<=0xffffffU) //不需要翻页
	{
		if((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW))
		{
			ext_ar = _flash_read_extended_address_reg(spi_id);
			if(ext_ar!=0)
				_flash_write_extended_address_reg(spi_id, 0);
		}
		dw_spi_eeprom_read(spi_id, cmd, 4, buf, (uint16_t)size);
	}
	else if(addr>=0x1000000) //直接翻页
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		ext_ar = _flash_read_extended_address_reg(spi_id);
		if(ext_ar!=1)
			_flash_write_extended_address_reg(spi_id, 1);
		dw_spi_eeprom_read(spi_id, cmd, 4, buf, (uint16_t)size);
	}
	else //两次传输
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		_flash_write_extended_address_reg(spi_id, 0);
		dw_spi_eeprom_read(spi_id, cmd, 4, buf, (uint16_t)(0x1000000-addr));

		_flash_write_extended_address_reg(spi_id, 1);
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		dw_spi_eeprom_read(spi_id, cmd, 4, buf+(0x1000000-addr), (uint16_t)(size-(0x1000000-addr)));
	}
}

void flash_read(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size)
{
	assert(buf);
	if(size==0)
		return;

	if (size <= CPU_READ_BLOCK_SIZE)
		_flash_read(spi_id, addr, buf, size);
	else
	{
		uint32_t count = size / CPU_READ_BLOCK_SIZE;
		uint32_t remainder = size % CPU_READ_BLOCK_SIZE;
		uint32_t i = 0;
		for (; i < count; i++)
			_flash_read(spi_id, addr + i * CPU_READ_BLOCK_SIZE, buf + i * CPU_READ_BLOCK_SIZE, CPU_READ_BLOCK_SIZE);
		if (remainder != 0)
			_flash_read(spi_id, addr + i * CPU_READ_BLOCK_SIZE, buf + i * CPU_READ_BLOCK_SIZE,  remainder);
	}
}

void flash_sector_erase(spi_id_t spi_id, uint32_t addr)
{
	assert((addr&4095)==0);

	uint8_t ext_ar;
	uint8_t cmd[4] = {
		Flash_SectorErase, 
		(addr & 0x00FF0000UL) >> 16UL,
		(addr & 0x0000FF00UL) >> 8UL,
		(addr & 0x000000FFUL) >> 0UL,
	};

	//1. write enable
	//2. send erase cmd
	if(addr<=0xffffff)
	{
		if((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW))
		{
			ext_ar = _flash_read_extended_address_reg(spi_id);
			if(ext_ar!=0)
				_flash_write_extended_address_reg(spi_id, 0);
		}
		_flash_write_enable(spi_id);
		dw_spi_transmit_only(spi_id, &cmd, 4, 1);
	}
	else
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		ext_ar = _flash_read_extended_address_reg(spi_id);
		if(ext_ar!=1)
			_flash_write_extended_address_reg(spi_id, 1);
		_flash_write_enable(spi_id);
		dw_spi_transmit_only(spi_id, &cmd, 4, 1);
	}
	
	//3. wait for erase over
	while(_flash_is_busy(spi_id));
}

void flash_block_erase(spi_id_t spi_id, uint32_t addr)
{
	assert((addr&65535)==0);
	uint8_t ext_ar;
	uint8_t cmd[4] = {
		Flash_BlockErase, 
		(addr & 0x00FF0000UL) >> 16UL,
		(addr & 0x0000FF00UL) >> 8UL,
		(addr & 0x000000FFUL) >> 0UL,
	};

	//1. write enable
	//2. send erase cmd
	if(addr<=0xffffff)
	{
		if((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW))
		{
			ext_ar = _flash_read_extended_address_reg(spi_id);
			if(ext_ar!=0)
				_flash_write_extended_address_reg(spi_id, 0);
		}
		_flash_write_enable(spi_id);
		dw_spi_transmit_only(spi_id, &cmd, 4, 1);
	}
	else
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		ext_ar = _flash_read_extended_address_reg(spi_id);
		if(ext_ar!=1)
			_flash_write_extended_address_reg(spi_id, 1);
		_flash_write_enable(spi_id);
		dw_spi_transmit_only(spi_id, &cmd, 4, 1);
	}
	
	//3. wait for erase over
	while(_flash_is_busy(spi_id));
}

void flash_chip_erase(spi_id_t spi_id)
{
	//1. write enable
	_flash_write_enable(spi_id);

	//2. send chip erase cmd
	uint8_t cmd = Flash_ChipErase;
	dw_spi_transmit_only(spi_id, &cmd, 1, 1);
	
	//3. wait for erase over
	while(_flash_is_busy(spi_id));
}

static inline void _flash_page_write(spi_id_t spi_id, uint32_t addr, uint8_t const * const buf, uint32_t size)
{
	assert(buf);
	// assert((addr&255)==0);
	assert((256 - (addr%256)) >= size);
	if(size==0)
		return;
	uint8_t ext_ar;
	uint8_t cmd[4+size]; 
	cmd[0] = Flash_PageProgram;
	cmd[1] = (addr & 0x00FF0000UL) >> 16UL;
	cmd[2] = (addr & 0x0000FF00UL) >> 8UL;
	cmd[3] = (addr & 0x000000FFUL) >> 0UL;
	memcpy(cmd+4, buf, size);

	if((addr+size-1)<=0xffffffU) //不需要翻页
	{
		if((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW))
		{
			ext_ar = _flash_read_extended_address_reg(spi_id);
			if(ext_ar!=0)
				_flash_write_extended_address_reg(spi_id, 0);
		}
		_flash_write_enable(spi_id);
		dw_spi_transmit_only(spi_id, cmd, 4+size, 0);
		while(_flash_is_busy(spi_id));
	}
	else if(addr>=0x1000000) //直接翻页
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		ext_ar = _flash_read_extended_address_reg(spi_id);
		if(ext_ar!=1)
			_flash_write_extended_address_reg(spi_id, 1);
		_flash_write_enable(spi_id);
		dw_spi_transmit_only(spi_id, cmd, 4+size, 0);
		while(_flash_is_busy(spi_id));
	}
	else //两次传输
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		_flash_write_extended_address_reg(spi_id, 0);
		_flash_write_enable(spi_id);
		dw_spi_transmit_only(spi_id, cmd, (0x1000000-addr), 0);
		while(_flash_is_busy(spi_id));

		_flash_write_extended_address_reg(spi_id, 1);
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		_flash_write_enable(spi_id);
		dw_spi_transmit_only(spi_id, cmd+(0x1000000-addr), (size-(0x1000000-addr)), 0);
		while(_flash_is_busy(spi_id));
	}
}

static void _flash_page_write_dma(spi_id_t spi_id, uint32_t addr, uint8_t const * const buf, uint32_t size);
static inline void _flash_write(spi_id_t spi_id, uint32_t addr, uint8_t *buf, uint32_t size, bool using_dma)
{
	uint32_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	/*mod 运算求余，若 writeAddr 是 FLASH_PageSize 整数倍，
	运算结果 Addr 值为 0*/
	Addr = addr % FLASH_PageSize;
	/* 差 count 个数据值，刚好可以对齐到页地址 */
	count = FLASH_PageSize - Addr;
	/* 计算出要写多少整数页 */
	NumOfPage = size / FLASH_PageSize;
	/*mod 运算求余，计算出剩余不满一页的字节数 */
	NumOfSingle = size % FLASH_PageSize;
	/* Addr=0, 则 addr 刚好按页对齐 aligned */
	if (Addr == 0)
	{
		/* size < FLASH_PageSize */
		if (NumOfPage == 0)
		{
			if(!using_dma)
				_flash_page_write(spi_id, addr, buf, size);
			else
				_flash_page_write_dma(spi_id, addr, buf, size);
		}
		else /* size > FLASH_PageSize */
		{
			/* 先把整数页都写了 */
			while (NumOfPage--)
			{
				if(!using_dma)
					_flash_page_write(spi_id, addr, buf, FLASH_PageSize);
				else
					_flash_page_write_dma(spi_id, addr, buf, FLASH_PageSize);
				addr += FLASH_PageSize;
				buf += FLASH_PageSize;
			}
			/* 若有多余的不满一页的数据，把它写完 */
			if (NumOfSingle != 0)
			{
				if(!using_dma)
					_flash_page_write(spi_id, addr, buf, NumOfSingle);
				else
					_flash_page_write_dma(spi_id, addr, buf, NumOfSingle);
			}
		}
	}
	/* 若地址与 FLASH_PageSize 不对齐 */
	else
	{
		/* size < FLASH_PageSize */
		if (NumOfPage == 0)
		{
			/* 当前页剩余的 count 个位置比 NumOfSingle 小，一页写不完 */
			if (NumOfSingle > count)
			{
				temp = NumOfSingle - count;
				/* 先写满当前页 */
				if(!using_dma)
					_flash_page_write(spi_id, addr, buf, count);
				else
					_flash_page_write_dma(spi_id, addr, buf, count);
				addr += count;
				buf += count;
				/* 再写剩余的数据 */
				if(!using_dma)
					_flash_page_write(spi_id, addr, buf, temp);
				else
					_flash_page_write_dma(spi_id, addr, buf, temp);
			}
			else /* 当前页剩余的 count 个位置能写完 NumOfSingle 个数据 */
			{
				if(!using_dma)
					_flash_page_write(spi_id, addr, buf, size);
				else
					_flash_page_write_dma(spi_id, addr, buf, size);
			}
		}
		else /* size > FLASH_PageSize */
		{
			/* 地址不对齐多出的 count 分开处理，不加入这个运算 */
			size -= count;
			NumOfPage = size / FLASH_PageSize;
			NumOfSingle = size % FLASH_PageSize;
			/* 先写完 count 个数据，为的是让下一次要写的地址对齐 */
			if(!using_dma)
				_flash_page_write(spi_id, addr, buf, count);
			else
				_flash_page_write_dma(spi_id, addr, buf, count);
			/* 接下来就重复地址对齐的情况 */
			addr += count;
			buf += count;
			/* 把整数页都写了 */
			while (NumOfPage--)
			{
				if(!using_dma)
					_flash_page_write(spi_id, addr, buf, FLASH_PageSize);
				else
					_flash_page_write_dma(spi_id, addr, buf, FLASH_PageSize);
				addr += FLASH_PageSize;
				buf += FLASH_PageSize;
			}
			/* 若有多余的不满一页的数据，把它写完 */
			if (NumOfSingle != 0)
			{
				if(!using_dma)
					_flash_page_write(spi_id, addr, buf, NumOfSingle);
				else
					_flash_page_write_dma(spi_id, addr, buf, NumOfSingle);
			}
		}
	}
}

void flash_write(spi_id_t spi_id, uint32_t addr, uint8_t *buf, uint32_t size)
{
	return _flash_write(spi_id, addr, buf, size, false);
}

void flash_write_dma(spi_id_t spi_id, uint32_t addr, uint8_t *buf, uint32_t size)
{
	return _flash_write(spi_id, addr, buf, size, true);
}

/* dma */
// #include "ape1210.h"
#include "dma.h"
static inline void _flash_read_dma(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size)
{
	assert(buf);
	// assert(size <= 2048);
	if(size==0)
		return;
	uint8_t cmd[4];
	uint8_t ext_ar;
	cmd[0] = Flash_ReadData;
	cmd[1] = (addr & 0x00FF0000UL) >> 16UL;
	cmd[2] = (addr & 0x0000FF00UL) >> 8UL;
	cmd[3] = (addr & 0x000000FFUL) >> 0UL;

	if((addr+size-1)<=0xffffffU) //不需要翻页
	{
		if((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW))
		{
			ext_ar = _flash_read_extended_address_reg(spi_id);
			if(ext_ar!=0)
				_flash_write_extended_address_reg(spi_id, 0);
		}
		dw_spi_eeprom_read_dma(spi_id, cmd, 4, buf, size);
	}
	else if(addr>=0x1000000) //直接翻页
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		ext_ar = _flash_read_extended_address_reg(spi_id);
		if(ext_ar!=1)
			_flash_write_extended_address_reg(spi_id, 1);
		dw_spi_eeprom_read_dma(spi_id, cmd, 4, buf, size);
	}
	else //两次传输
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		_flash_write_extended_address_reg(spi_id, 0);
		dw_spi_eeprom_read_dma(spi_id, cmd, 4, buf, (0x1000000-addr));

		_flash_write_extended_address_reg(spi_id, 1);
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		dw_spi_eeprom_read_dma(spi_id, cmd, 4, buf+(0x1000000-addr), (size-(0x1000000-addr)));
	}
}

void flash_read_dma(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size)
{
	assert(buf);
	if(size==0)
		return;
	// DUCache_Maintain((uint32_t)buf, (uint32_t)((uint32_t)buf + size - 1), 2);
	if (size <= BLOCK_SIZE_DMA)
		_flash_read_dma(spi_id, addr, buf, size);
	else
	{
		uint32_t count = size / BLOCK_SIZE_DMA;
		uint32_t remainder = size % BLOCK_SIZE_DMA;
		uint32_t i = 0;
		for (; i < count; i++)
			_flash_read_dma(spi_id, addr + i * BLOCK_SIZE_DMA, buf + i * BLOCK_SIZE_DMA, BLOCK_SIZE_DMA);
		if (remainder != 0)
			_flash_read_dma(spi_id, addr + i * BLOCK_SIZE_DMA, buf + i * BLOCK_SIZE_DMA,  remainder);
	}
}


typedef struct _flash_read_state
{
	uint8_t is_busy;
	uint32_t count;
	uint32_t single;
} flash_read_state_t;

bool is_dw_spi_eeprom_read_dma_end(spi_id_t spi_id, DMA_Channel_t ch);

uint32_t flash_read_dma_start(spi_id_t spi_id, DMA_Channel_t* const ch, uint32_t addr, uint8_t* const buf, uint32_t size)
{
	assert(buf);
	// assert(size <= 2048);
	if(size==0)
		return 0;
	uint8_t cmd[4];
	uint8_t ext_ar;
	cmd[0] = Flash_ReadData;
	cmd[1] = (addr & 0x00FF0000UL) >> 16UL;
	cmd[2] = (addr & 0x0000FF00UL) >> 8UL;
	cmd[3] = (addr & 0x000000FFUL) >> 0UL;

	DMA_Channel_t dw_spi_eeprom_read_dma_start(spi_id_t spi_id, void *t_buf, uint8_t t_size, void *r_buf, uint16_t r_size);
	if((addr+size-1)<=0xffffffU) //不需要翻页
	{
		if((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW))
		{
			ext_ar = _flash_read_extended_address_reg(spi_id);
			if(ext_ar!=0)
				_flash_write_extended_address_reg(spi_id, 0);
		}
		if(size <= BLOCK_SIZE_DMA)
		{
			*ch = dw_spi_eeprom_read_dma_start(spi_id, cmd, 4, buf, (uint16_t)size);
			return size;
		}
		else
		{
			*ch = dw_spi_eeprom_read_dma_start(spi_id, cmd, 4, buf, (uint16_t)BLOCK_SIZE_DMA);
			return BLOCK_SIZE_DMA;
		}
	}
	else if(addr>=0x1000000) //直接翻页
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		ext_ar = _flash_read_extended_address_reg(spi_id);
		if(ext_ar!=1)
			_flash_write_extended_address_reg(spi_id, 1);
		if(size <= BLOCK_SIZE_DMA)
		{
			*ch = dw_spi_eeprom_read_dma_start(spi_id, cmd, 4, buf, (uint16_t)size);
			return size;
		}
		else
		{
			*ch = dw_spi_eeprom_read_dma_start(spi_id, cmd, 4, buf, (uint16_t)BLOCK_SIZE_DMA);
			return BLOCK_SIZE_DMA;
		}
	}
	else //两次传输
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		_flash_write_extended_address_reg(spi_id, 0);
		if((0x1000000-addr) <= BLOCK_SIZE_DMA)
		{
			*ch = dw_spi_eeprom_read_dma_start(spi_id, cmd, 4, buf, (uint16_t)(0x1000000-addr));
			return (0x1000000-addr);
		}
		else
		{
			*ch = dw_spi_eeprom_read_dma_start(spi_id, cmd, 4, buf, (uint16_t)(0x1000000-addr));
			return BLOCK_SIZE_DMA;
		}
	}
	return 0;
}

bool is_last_flash_read_finish(spi_id_t spi_id, DMA_Channel_t ch)
{
	return is_dw_spi_eeprom_read_dma_end(spi_id, ch);
}

static void _flash_page_write_dma(spi_id_t spi_id, uint32_t addr, uint8_t const * const buf, uint32_t size)
{
	assert(buf);
	assert((256 - (addr%256)) >= size);
	if(size==0)
		return;

	uint8_t ext_ar;
	uint8_t cmd[4+size];
	cmd[0] = Flash_PageProgram;
	cmd[1] = (addr & 0x00FF0000UL) >> 16UL;
	cmd[2] = (addr & 0x0000FF00UL) >> 8UL;
	cmd[3] = (addr & 0x000000FFUL) >> 0UL;
	memcpy(cmd+4, buf, size);
	
	// DUCache_Maintain((uint32_t)cmd, (uint32_t)cmd+4+size-1, 1);

	if((addr+size-1)<=0xffffffU) //不需要翻页
	{
		if((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW))
		{
			ext_ar = _flash_read_extended_address_reg(spi_id);
			if(ext_ar!=0)
				_flash_write_extended_address_reg(spi_id, 0);
		}
		_flash_write_enable(spi_id);
		dw_spi_transmit_only_dma(spi_id, cmd, 4+size);
		while(_flash_is_busy(spi_id));
	}
	else if(addr>=0x1000000) //直接翻页
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		ext_ar = _flash_read_extended_address_reg(spi_id);
		if(ext_ar!=1)
			_flash_write_extended_address_reg(spi_id, 1);
		_flash_write_enable(spi_id);
		dw_spi_transmit_only_dma(spi_id, cmd, 4+size);
		while(_flash_is_busy(spi_id));
	}
	else //两次传输
	{
		assert(0);
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		_flash_write_extended_address_reg(spi_id, 0);
		dw_spi_transmit_only_dma(spi_id, cmd, (0x1000000-addr));
		_flash_write_enable(spi_id);
		while(_flash_is_busy(spi_id));

		_flash_write_extended_address_reg(spi_id, 1);
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		_flash_write_enable(spi_id);
		dw_spi_transmit_only_dma(spi_id, cmd+(0x1000000-addr), (size-(0x1000000-addr)));
		while(_flash_is_busy(spi_id));
	}
}

static inline bool _is_flash_read_dma_end(spi_id_t spi_id, uint8_t dma_ch)
{
	// if((REG32(DMA_BASE + CH1_INTSTATUS_0) & 0x00000002) != 0)
	// {
	// 	REG32(DMA_BASE + CH1_INTCLEARREG_0) = 0x00000002;
	// 	void spi_disable(spi_id_t spi_id, bool is_master);
	// 	spi_disable(spi_id, true);

	// 	return true;
	// }
	return false;
}


/* 
 * enhanced mode 
 * !!!可能不通用
 */
/*
 * 从标准spi模式进入4线模式，写
*/
static inline uint8_t _flash_read_sr_high(spi_id_t spi_id)
{
	uint8_t cmd = Flash_ReadStatusReg_High;
	uint8_t sr;
	dw_spi_eeprom_read(spi_id, &cmd, 1, &sr, 1);
	return sr;
}

static inline void _flash_write_sr(spi_id_t spi_id, uint8_t* sr)
{
	uint8_t cmd[3];
	cmd[0] = Flash_WriteStatusReg;
	cmd[1] = sr[0];
	cmd[2] = sr[1];
	_flash_write_enable(spi_id);
	dw_spi_transmit_only(spi_id, cmd, 3, true);
	while(_flash_is_busy(spi_id));
	return;
}

static inline void _flash_reset(spi_id_t spi_id)
{
	uint8_t cmd[2] = {Flash_ResetEnable, Flash_ResetMemory};
	dw_spi_transmit_only(spi_id, cmd, 1, true);
	dw_spi_transmit_only(spi_id, cmd+1, 1, true);
}

static inline void _flash_reset_4(spi_id_t spi_id)
{
	uint8_t cmd[2] = {0x00, 0x00};
	enhanced_transfer_format_t enhanced_transfer_format = {
		.spi_frf = QUAD_SPI_FRF,
		.trans_type = 2,
		.instruction = cmd[0],
		.ins_lenth = 8,
		.address = NULL,
		.addr_lenth = 0,
		.data = NULL,
		.data_nums = 0
	};
	dw_spi_enhanced_write(spi_id, &enhanced_transfer_format, true);
	enhanced_transfer_format.instruction = cmd[1];
	dw_spi_enhanced_write(spi_id, &enhanced_transfer_format, true);
}

static inline void flash_reset(spi_id_t spi_id)
{
	_flash_reset_4(spi_id);
	systimer_delay(30, IN_US);

	_flash_reset(spi_id);
	systimer_delay(30, IN_US);
}

void flash_set_QE_bit(spi_id_t spi_id)
{
	uint8_t sr[2] = {0x00,0x00};
	sr[0] = _flash_read_sr(spi_id);
	sr[1] = _flash_read_sr_high(spi_id);
	sr[1] |= Quad_Enable_Bit;
	_flash_write_enable(spi_id);
	_flash_write_sr(spi_id, sr);
}

static inline void _flash_enable_qpi(spi_id_t spi_id)
{
	uint8_t cmd[1] = {Flash_Enable_QPI};
	dw_spi_transmit_only(spi_id, cmd, 1, true);
}

static inline void _flash_disable_qpi(spi_id_t spi_id)
{
	enhanced_transfer_format_t enhanced_transfer_format = {
		.spi_frf = QUAD_SPI_FRF,
		.trans_type = 2,
		.instruction = Flash_Disable_QPI,
		.ins_lenth = 8,
		.address = NULL,
		.addr_lenth = 0,
		.data = NULL,
		.data_nums = 0,
		.wait_cycles = 0
	};
	dw_spi_enhanced_write(spi_id, &enhanced_transfer_format, false);
}

static inline void _flash_read_quad(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size)
{
	assert(buf);
	assert(size <= CPU_READ_BLOCK_SIZE);
	if(size==0)
		return;

	enhanced_transfer_format_t enhanced_transfer_format = {
		.spi_frf = QUAD_SPI_FRF,
		.trans_type = 0,
		.instruction = Flash_Quad_Output_Fast_Read,
		.ins_lenth = 8,
		.address = &addr,
		.addr_lenth = 24,
		.data = buf,
		.data_nums = size,
		.wait_cycles = 8
	};
	// dw_spi_enhanced_read(spi_id, &enhanced_transfer_format);

	uint8_t ext_ar;
	if((addr+size-1)<=0xffffffU) //不需要翻页
	{
		if((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW))
		{
			ext_ar = _flash_read_extended_address_reg(spi_id);
			if(ext_ar!=0)
				_flash_write_extended_address_reg(spi_id, 0);
		}
		dw_spi_enhanced_read(spi_id, &enhanced_transfer_format);
	}
	else if(addr>=0x1000000) //直接翻页
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		ext_ar = _flash_read_extended_address_reg(spi_id);
		if(ext_ar!=1)
			_flash_write_extended_address_reg(spi_id, 1);
		dw_spi_enhanced_read(spi_id, &enhanced_transfer_format);
	}
	else //两次传输
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		_flash_write_extended_address_reg(spi_id, 0);
		// dw_spi_eeprom_read_dma(spi_id, cmd, 4, buf, (uint16_t)(0x1000000-addr));
		enhanced_transfer_format.data_nums = 0x1000000-addr;
		dw_spi_enhanced_read(spi_id, &enhanced_transfer_format);

		_flash_write_extended_address_reg(spi_id, 1);
		addr = 0;
		enhanced_transfer_format.data = buf + enhanced_transfer_format.data_nums;
		enhanced_transfer_format.data_nums = size - enhanced_transfer_format.data_nums;
		dw_spi_enhanced_read(spi_id, &enhanced_transfer_format);
		// dw_spi_eeprom_read_dma(spi_id, cmd, 4, buf+(0x1000000-addr), (uint16_t)(size-(0x1000000-addr)));
	}
}

void flash_read_quad(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size)
{
	assert(buf);
	if(size==0)
		return;

	if (size <= CPU_READ_BLOCK_SIZE)
		_flash_read_quad(spi_id, addr, buf, size);
	else
	{
		uint32_t count = size / CPU_READ_BLOCK_SIZE;
		uint32_t remainder = size % CPU_READ_BLOCK_SIZE;
		uint32_t i = 0;
		for (; i < count; i++)
			_flash_read_quad(spi_id, addr + i * CPU_READ_BLOCK_SIZE, buf + i * CPU_READ_BLOCK_SIZE, CPU_READ_BLOCK_SIZE);
		if (remainder != 0)
			_flash_read_quad(spi_id, addr + i * CPU_READ_BLOCK_SIZE, buf + i * CPU_READ_BLOCK_SIZE,  remainder);
	}
}

static inline void _flash_read_quad_dma(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size)
{
	assert(buf);
	assert(size <= BLOCK_SIZE_DMA);
	if(size==0)
		return;

	enhanced_transfer_format_t enhanced_transfer_format = {
		.spi_frf = QUAD_SPI_FRF,
		.trans_type = 0,
		.instruction = Flash_Quad_Output_Fast_Read,
		.ins_lenth = 8,
		.address = &addr,
		.addr_lenth = 24,
		.data = buf,
		.data_nums = size,
		.wait_cycles = 8
	};
	// dw_spi_enhanced_read_dma(spi_id, &enhanced_transfer_format);

	uint8_t ext_ar;
	if((addr+size-1)<=0xffffffU) //不需要翻页
	{
		if((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW))
		{
			ext_ar = _flash_read_extended_address_reg(spi_id);
			if(ext_ar!=0)
				_flash_write_extended_address_reg(spi_id, 0);
		}
		dw_spi_enhanced_read_dma(spi_id, &enhanced_transfer_format);
	}
	else if(addr>=0x1000000) //直接翻页
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		ext_ar = _flash_read_extended_address_reg(spi_id);
		if(ext_ar!=1)
			_flash_write_extended_address_reg(spi_id, 1);
		dw_spi_enhanced_read_dma(spi_id, &enhanced_transfer_format);
	}
	else //两次传输
	{
		assert((flash_model[spi_id]==GD25LQ255)||(flash_model[spi_id]==W25Q256JW));
		_flash_write_extended_address_reg(spi_id, 0);
		enhanced_transfer_format.data_nums = 0x1000000-addr;
		dw_spi_enhanced_read_dma(spi_id, &enhanced_transfer_format);
		// dw_spi_eeprom_read_dma(spi_id, cmd, 4, buf, (uint16_t)(0x1000000-addr));

		_flash_write_extended_address_reg(spi_id, 1);
		addr = 0;
		enhanced_transfer_format.data = buf + enhanced_transfer_format.data_nums;
		enhanced_transfer_format.data_nums = size - enhanced_transfer_format.data_nums;
		dw_spi_enhanced_read_dma(spi_id, &enhanced_transfer_format);
		// dw_spi_eeprom_read_dma(spi_id, cmd, 4, buf+(0x1000000-addr), (uint16_t)(size-(0x1000000-addr)));
		
	}
}

void flash_read_quad_dma(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size)
{
	assert(buf);
	if(size==0)
		return;
	// DUCache_Maintain((uint32_t)buf, (uint32_t)((uint32_t)buf + size - 1), 2);
	if (size <= BLOCK_SIZE_DMA)
		_flash_read_quad_dma(spi_id, addr, buf, size);
	else
	{
		uint32_t count = size / BLOCK_SIZE_DMA;
		uint32_t remainder = size % BLOCK_SIZE_DMA;
		uint32_t i = 0;
		for (; i < count; i++)
			_flash_read_quad_dma(spi_id, addr + i * BLOCK_SIZE_DMA, buf + i * BLOCK_SIZE_DMA, BLOCK_SIZE_DMA);
		if (remainder != 0)
			_flash_read_quad_dma(spi_id, addr + i * BLOCK_SIZE_DMA, buf + i * BLOCK_SIZE_DMA,  remainder);
	}
}


/* test */
#include "nor_flash_test.c"
