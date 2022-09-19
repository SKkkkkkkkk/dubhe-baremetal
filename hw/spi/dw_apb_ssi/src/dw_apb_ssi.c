#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "dw_apb_ssi.h"
#include "dw_apb_ssi_regs.h"
#include "dw_apb_gpio.h"

#define BOOTSPI_BASE (0x250D0000) /*!< (BootSPI   ) Base Address */
#define SPI0_BASE (0xF8030000UL) /*!< (SPI0      ) Base Address */
#define SPI1_BASE (0xF8031000UL) /*!< (SPI1      ) Base Address */
#define SPI2_BASE (0xF8032000UL) /*!< (SPI2      ) Base Address */

#define BOOTSPI ((BOOTSPI_TypeDef *)BOOTSPI_BASE)
#define SPI0 ((DW_APB_SSI_TypeDef *)SPI0_BASE)
#define SPI1 ((DW_APB_SSI_TypeDef *)SPI1_BASE)
#define SPI2 ((DW_APB_SSI_TypeDef *)SPI2_BASE)

#ifdef A55
	#include "arch_features.h"
	#define __DMB	dmbsy
#endif

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-label"
	#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif


#define SR_BUSY 0UL
#define SR_TFNF 1UL
#define SR_TFE  2UL
#define SR_RFNE 3UL

static bool const SPI_CS_USE_GPIO = false; ///< SPI CS 是否使用GPIO作为片选

/**
 * @brief SPI 状态位.
 * 0 - 未初始化
 * 1 - 已初始化
 */
static uint8_t spi_state[4] = {0,0,0,0};

static inline uint8_t get_spi_state(spi_id_t spi_id)
{
	assert(spi_id <= 3);
	return spi_state[spi_id];
}

static inline void set_spi_state(spi_id_t spi_id, uint8_t state)
{
	assert(spi_id <= 3);
	spi_state[spi_id] = state;
	return;
}

/***
 * Todo:
 * 现在std，quad都把4个信号引脚设置为了spi功能，std模式不需要D2，D3.
 */
// static inline void spix_pinmux(spi_init_config_t const * const spi_init_config)
// {

// }
static inline void spix_pinmux(spi_init_config_t const * const spi_init_config)
{
	gpio_init_config_t gpio_init_config = {
		.gpio_control_mode = Software_Mode,
		.gpio_mode = GPIO_Output_Mode
	};
	switch (spi_init_config->spi_id) {
	case BOOTSPI_ID:
		pin_set_iomux(GROUP_GPIO0, 16, 0);
		if ( (!SPI_CS_USE_GPIO) )
		{
			pin_set_iomux(GROUP_GPIO1, 17, 0);
		}
		else
		{
			gpio_init_config.group = GROUP_GPIO0;
			gpio_init_config.pin = 17;
			gpio_init(&gpio_init_config);
			pin_set_iomux(GROUP_GPIO0, 17, 3); //as gpio
		}
		pin_set_iomux(GROUP_GPIO0, 18, 0);
		pin_set_iomux(GROUP_GPIO0, 19, 0);
		pin_set_iomux(GROUP_GPIO0, 20, 0);
		pin_set_iomux(GROUP_GPIO0, 21, 0);
		return;
	case SPI0_ID:
		pin_set_iomux(GROUP_GPIO1, 21, 2);
		if ( (!SPI_CS_USE_GPIO) /* 不使用GPIO作为CS */ || (!spi_init_config->as_master) /* slave模式 */ )
			pin_set_iomux(GROUP_GPIO1, 22, 2);
		else
		{
			gpio_init_config.group = GROUP_GPIO1;
			gpio_init_config.pin = 22;
			gpio_init(&gpio_init_config);
			pin_set_iomux(GROUP_GPIO1, 22, 3); //as gpio
		}
		pin_set_iomux(GROUP_GPIO1, 23, 2);
		pin_set_iomux(GROUP_GPIO1, 24, 2);
		pin_set_iomux(GROUP_GPIO1, 25, 2);
		pin_set_iomux(GROUP_GPIO1, 26, 2);
		return;
	case SPI1_ID:
		pin_set_iomux(GROUP_GPIO1, 27, 1);
		if ( (!SPI_CS_USE_GPIO) /* 不使用GPIO作为CS */ || (!spi_init_config->as_master) /* slave模式 */ )
			pin_set_iomux(GROUP_GPIO1, 28, 1);
		else
		{
			gpio_init_config.group = GROUP_GPIO1;
			gpio_init_config.pin = 28;
			gpio_init(&gpio_init_config);
			pin_set_iomux(GROUP_GPIO1, 28, 3); //as gpio
		}
		pin_set_iomux(GROUP_GPIO1, 29, 1);
		pin_set_iomux(GROUP_GPIO1, 30, 1);
		pin_set_iomux(GROUP_GPIO1, 31, 1);
		pin_set_iomux(GROUP_GPIO2,  0, 1);
		return;
	case SPI2_ID:
		pin_set_iomux(GROUP_GPIO2, 1, 1);
		if ( (!SPI_CS_USE_GPIO) /* 不使用GPIO作为CS */ || (!spi_init_config->as_master) /* slave模式 */ )
			pin_set_iomux(GROUP_GPIO2, 2, 1);
		else
		{
			gpio_init_config.group = GROUP_GPIO2;
			gpio_init_config.pin = 2;
			gpio_init(&gpio_init_config);
			pin_set_iomux(GROUP_GPIO2, 2, 3); //as gpio
		}
		pin_set_iomux(GROUP_GPIO2, 3, 1);
		pin_set_iomux(GROUP_GPIO2, 4, 1);
		pin_set_iomux(GROUP_GPIO2, 5, 1);
		pin_set_iomux(GROUP_GPIO2, 6, 1);
		return;
	default:
		return;
	}
}

static inline void* get_spi_base(spi_id_t spi_id)
{
	void* spi_base = NULL;
	switch (spi_id) {
	case BOOTSPI_ID:
		spi_base = (void*)BOOTSPI_BASE;
		break;

	case SPI0_ID:
		spi_base = (void*)SPI0_BASE;
		break;

	case SPI1_ID:
		spi_base = (void*)SPI1_BASE;
		break;

	case SPI2_ID:
		spi_base = (void*)SPI2_BASE;
		break;

	default:
		break;
	}

	return spi_base;
}

static inline void bootspi_disable(void)
{
	BOOTSPI->SSIENR = 0;
	BOOTSPI->SER = 0;
	BOOTSPI->DMACR = 0;
	if (SPI_CS_USE_GPIO)
		gpio_write_pin(GROUP_GPIO1, 11, GPIO_PIN_SET);
}

static inline void bootspi_enable(void)
{
	BOOTSPI->SSIENR = 1;
}

static inline void spi_disable(spi_id_t spi_id, bool is_master)
{
	if(spi_id==BOOTSPI_ID)
		return bootspi_disable();
	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
	assert(spix!=(DW_APB_SSI_TypeDef*)BOOTSPI_BASE);
	if(is_master)
	{
		spix->SSIENR = 0;
		spix->SER = 0;
		spix->DMACR = 0;
		if (SPI_CS_USE_GPIO)
		{
			switch (spi_id)
			{
			case SPI0_ID:
				gpio_write_pin(GROUP_GPIO0, 7, GPIO_PIN_SET);
				break;
			case SPI1_ID:
				gpio_write_pin(GROUP_GPIO2, 19, GPIO_PIN_SET);
				break;
			case SPI2_ID:
				gpio_write_pin(GROUP_GPIO1, 17, GPIO_PIN_SET);
				break;
			default:
				break;
			}
		}
	}
	else
	{
		// spix->S_SSIENR = 0;
		// spix->S_DMACR = 0;
	}
}

static inline void spi_enable(spi_id_t spi_id, bool is_master)
{
	if(spi_id==BOOTSPI_ID)
		return bootspi_enable();
	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
	assert(spix!=(DW_APB_SSI_TypeDef*)BOOTSPI_BASE);
	if(is_master)
	{
		spix->SSIENR = 1;
	}
	else
	{
		// spix->S_SSIENR = 1;	
	}
}

static inline bool bootspi_rxfifo_is_empty(void)
{
	return ((BOOTSPI->SR & SPI_RFNE_Msk) == 0)? true:false;
}

static inline void bootspi_select_slave(uint8_t slave)
{
	if (SPI_CS_USE_GPIO)
	{
		if(slave==0)
			gpio_write_pin(GROUP_GPIO1, 11, GPIO_PIN_SET);
		else
			gpio_write_pin(GROUP_GPIO1, 11, GPIO_PIN_RESET);
	}
	BOOTSPI->SER = slave;
}

static inline void spi_select_slave(spi_id_t spi_id, uint8_t slave)
{
	if(spi_id==BOOTSPI_ID)
		return bootspi_select_slave(slave);
	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
	assert(spix!=(DW_APB_SSI_TypeDef*)BOOTSPI_BASE);
	if (SPI_CS_USE_GPIO)
	{
		switch (spi_id)
		{
		case SPI0_ID:
			gpio_write_pin(GROUP_GPIO0, 7, GPIO_PIN_RESET);
			break;
		case SPI1_ID:
			gpio_write_pin(GROUP_GPIO2, 19, GPIO_PIN_RESET);
			break;
		case SPI2_ID:
			gpio_write_pin(GROUP_GPIO1, 17, GPIO_PIN_RESET);
			break;
		default:
			break;
		}
	}
	spix->SER = slave;
}

static inline void _bootspi_init(spi_init_config_t const * const spi_init_config)
{
	assert(spi_init_config);
	assert(spi_init_config->spi_id == BOOTSPI_ID);
	assert(spi_init_config->as_master); //bootspi只支持master
	assert(spi_init_config->spi_mode <= 3);

	spix_pinmux(spi_init_config);
	BOOTSPI->MAP = 1U;
	//1.disable spi
	bootspi_disable();

	//2.配置CTRLR0
	uint32_t ctrl0 = BOOTSPI->CTRLR0;

	//使用SPI协议
	ctrl0 &= (~SPI_FRF_Msk);

	//SPI Mode
	ctrl0 &= ~(3UL << 6UL);
	ctrl0 |= (spi_init_config->spi_mode) << 6UL;

	//SRL testint_mode 需要清0
	ctrl0 &= ~SPI_SRL_Msk;

	//DFS_32 目前只支持8bit
	ctrl0 &= ~SPI_DFS_32_Msk;
	ctrl0 |= (8 - 1) << SPI_DFS_32_Pos;

	//SSTE Slave Select Toggle Enable
	ctrl0 &= ~SPI_SSTE_Msk;

	BOOTSPI->CTRLR0 = ctrl0;

	BOOTSPI->BAUDR = spi_init_config->clock_div;

	BOOTSPI->DMACR = 0; //清除dma reqs

	set_spi_state(BOOTSPI_ID, 1);
}

void dw_spi_init(spi_init_config_t const * const spi_init_config)
{
	assert(spi_init_config);
	// assert(spi_init_config->as_master);
	assert(spi_init_config->spi_id <= 3);
	assert(spi_init_config->spi_mode <= 3);

	//如果是bootspi
	if(spi_init_config->spi_id == BOOTSPI_ID)
		return _bootspi_init(spi_init_config);

	//如果不是bootspi
	DW_APB_SSI_TypeDef *spix = get_spi_base(spi_init_config->spi_id);
	assert(spix);

	spix_pinmux(spi_init_config);
	// if (spi_init_config->as_master) //as master
	// {
		//1.disable spi
		spi_disable(spi_init_config->spi_id, true);

		// as master
		// spix->CTRLR = 3 | (1 << 4);

		//2.配置CTRLR0
		uint32_t ctrl0 = spix->CTRLR0;

		//使用SPI协议
		ctrl0 &= (~SPI_FRF_Msk);

		//SPI Mode
		ctrl0 &= ~(3UL << 6UL);
		ctrl0 |= (spi_init_config->spi_mode) << 6UL;

		//SRL
		ctrl0 &= ~SPI_SRL_Msk;

		//DFS_32
		ctrl0 &= ~SPI_DFS_32_Msk;
		// ctrl0 |= (spi_init_config->data_frame_size - 1) << SPI_DFS_32_Pos;
		ctrl0 |= (8 - 1) << SPI_DFS_32_Pos;

		//SSTE
		ctrl0 &= ~SPI_SSTE_Msk;

		spix->CTRLR0 = ctrl0;

		spix->BAUDR = spi_init_config->clock_div;

		spix->DMACR = 0;
	// } 
	// else //as slave
	// {
	// 	//1.disable spi
	// 	spi_disable(spi_init_config->spi_id, false);

	// 	//as slave
	// 	spix->CTRLR = 2 | (1 << 4);

	// 	//2.配置CTRLR0
	// 	uint32_t ctrl0 = spix->S_CTRLR0;

	// 	//使用SPI协议
	// 	ctrl0 &= (~SPI_FRF_Msk);

	// 	//SPI Mode
	// 	ctrl0 &= ~(3UL << 6UL);
	// 	ctrl0 |= (spi_init_config->spi_mode) << 6UL;

	// 	//SLV_OE
	// 	ctrl0 &= ~SPI_SLV_OE_Msk;

	// 	//SRL
	// 	ctrl0 &= ~SPI_SRL_Msk;

	// 	//DFS_32
	// 	ctrl0 &= ~SPI_DFS_32_Msk;
	// 	// ctrl0 |= (spi_init_config->data_frame_size - 1) << SPI_DFS_32_Pos;
	// 	ctrl0 |= (8 - 1) << SPI_DFS_32_Pos;

	// 	//SSTE
	// 	ctrl0 &= ~SPI_SSTE_Msk;

	// 	spix->S_CTRLR0 = ctrl0;

	// 	//3.设置总线频率
	// 	spix->S_BAUDR = spi_init_config->clock_div;

	// 	spix->S_DMACR = 0;

	// 	spi_enable(spi_init_config->spi_id, false);
	// }

	set_spi_state(spi_init_config->spi_id, 1);
}

void dw_spi_deinit(spi_id_t spi_id, bool as_master)
{
	assert(spi_id <= 3);
	
	spi_disable(spi_id, as_master);
	set_spi_state(spi_id, 0);
}

//安全模式下的发送可以被打断，发送的数据也是连续的，但是最多一次发送fifo_depth的数据
//非安全模式下的发送不可被打断，否则发送数据可能不是连续的，但是发送的数据长度不限
static inline void bootspi_transmit_and_receive(void *t_buf, void *r_buf, uint32_t tr_size, bool safe_mode)
{
	assert(t_buf);
	assert(r_buf);
	if (safe_mode)
		assert(tr_size<=32);

	bootspi_disable();

	uint8_t *t_buf_p = t_buf;
	uint8_t *r_buf_p = r_buf;

	uint32_t ctrl0 = BOOTSPI->CTRLR0;

	//TMOD = transmit_and_receive
	ctrl0 &= ~SPI_TMOD_Msk;
	// ctrl0 |= 0 << SPI_TMOD_Pos;

	ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF

	BOOTSPI->CTRLR0 = ctrl0;

	bootspi_enable();

	if (!safe_mode) //不限制传输数据为FIFO DEPTH大小
	{
		bootspi_select_slave(1U);
	}

	//填充发送数据
	uint32_t cnt = tr_size;
	while (cnt--) {
		while ((BOOTSPI->SR & SPI_TFNF_Msk) == 0)
			; //先片选+不使用gpio片选的情况下，可能导致数据不连续
		BOOTSPI->DR = *t_buf_p++;
		if ((!safe_mode) && (r_buf_p!=NULL)) //非安全模式的发送下，必须发一个接收一个
		{
			while (bootspi_rxfifo_is_empty())
				;
			*r_buf_p++ = BOOTSPI->DR;
		}
	}

	if (safe_mode) //安全模式下，可以发送完再进行接收(txfifo_depth==rxfifo_depth)
	{
		bootspi_select_slave(1U);
		for (uint32_t i = 0; i < tr_size; i++) {
			while (bootspi_rxfifo_is_empty())
				;
			r_buf_p[i] = BOOTSPI->DR;
		}
	}

	bootspi_disable();
}

void dw_spi_transmit_and_receive(spi_id_t spi_id, void *t_buf, void *r_buf, uint32_t tr_size, bool safe_mode)
{
	assert(get_spi_state(spi_id)!=0);
	assert(t_buf);
	assert(r_buf);
	if (safe_mode)
		assert(tr_size<=32);

	if(spi_id == BOOTSPI_ID)
		return bootspi_transmit_and_receive(t_buf, r_buf, tr_size, safe_mode);
	
	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
	// if (spix->CTRLR & 1) 
	// {
		uint8_t *t_buf_p = t_buf;
		uint8_t *r_buf_p = r_buf;

		spi_disable(spi_id, true);

		uint32_t ctrl0 = spix->CTRLR0;

		//TMOD = transmit_and_receive
		ctrl0 &= ~SPI_TMOD_Msk;
		// ctrl0 |= 0 << SPI_TMOD_Pos;

		ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF

		spix->CTRLR0 = ctrl0;

		spix->SSIENR = 1;

		if (!safe_mode) //不限制传输数据为FIFO DEPTH大小
		{
			spi_select_slave(spi_id, 1);
		}

		//填充发送数据
		uint32_t cnt = tr_size;
		while (cnt--) {
			if (!safe_mode)
				while ((spix->SR & SPI_TFNF_Msk) == 0)
					;
			spix->DR = *t_buf_p++;
			if (!safe_mode) //非安全模式的发送下，必须发一个接收一个
			{
				while ((spix->SR & SPI_RFNE_Msk) == 0)
					;
				*r_buf_p++ = spix->DR;
			}
		}

		if (safe_mode)
		{
			spi_select_slave(spi_id, 1);
			//等待数据接收完
			for (uint8_t i = 0; i < tr_size; i++) {
				while ((spix->SR & SPI_RFNE_Msk) == 0)
					;
				r_buf_p[i] = spix->DR;
			}
		}
		
		spi_disable(spi_id, true);
	// } 
	// else 
	// {
	// 	while(1);
	// }
}

static inline void boot_spi_eeprom_read(void *t_buf, uint8_t t_size, void *r_buf, uint16_t r_size)
{
	assert(t_buf);
	assert(r_buf);
	assert((t_size<=32) && (t_size!=0));
	// assert(r_size<=65535);

	uint8_t *t_buf_p = t_buf;
	uint8_t *r_buf_p = r_buf;

	bootspi_disable();

	uint32_t ctrl0 = BOOTSPI->CTRLR0;
	//TMOD = EEPROM Read
	ctrl0 &= ~SPI_TMOD_Msk;
	ctrl0 |= 3 << SPI_TMOD_Pos;
	ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
	BOOTSPI->CTRLR0 = ctrl0;

	BOOTSPI->CTRLR1 = r_size - 1;

	bootspi_enable();

	//填充发送数据
	while (t_size--) {
		BOOTSPI->DR = *t_buf_p++; //t_size <= tx_fifo_depth
	}

	bootspi_select_slave(1U);

	//等待数据接收完
	for (uint16_t i = 0; i < r_size; i++) {
		while (bootspi_rxfifo_is_empty())
			;

		r_buf_p[i] = BOOTSPI->DR;
	}

	bootspi_disable();
}

void dw_spi_eeprom_read(spi_id_t spi_id, void *t_buf, uint8_t t_size, void *r_buf, uint16_t r_size)
{
	//0. 判断输入合法性
	assert(get_spi_state(spi_id)!=0);
	assert(spi_id <= 3);
	assert(t_buf);
	assert(r_buf);
	assert((t_size<=32) && (t_size!=0));
	// assert(r_size <= 65535);

	//1. bootspi or spix
	if(spi_id == BOOTSPI_ID)
		return boot_spi_eeprom_read(t_buf, t_size, r_buf, r_size);

	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
	assert(spix);
	// if (spix->CTRLR & 1) { //as master

		uint8_t *t_buf_p = t_buf;
		uint8_t *r_buf_p = r_buf;

		spi_disable(SPI0_ID, true);

		uint32_t ctrl0 = spix->CTRLR0;

		//TMOD = EEPROM Read
		ctrl0 &= ~SPI_TMOD_Msk;
		ctrl0 |= 3 << SPI_TMOD_Pos;

		ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF

		spix->CTRLR0 = ctrl0;

		spix->CTRLR1 = r_size - 1;

		spix->SSIENR = 1;

		//填充发送数据
		while (t_size--) {
			spix->DR = *t_buf_p++;
		}

		spi_select_slave(spi_id, 1);

		//等待数据接收完 应该很难出现r_buf溢出的风险(cpu read speed < spi receive speed)
		for (uint16_t i = 0; i < r_size; i++) {
			while ((spix->SR & SPI_RFNE_Msk) == 0)
				;
			r_buf_p[i] = spix->DR;
		}

		spi_disable(spi_id, true);
	// } else {
	// }
}

static inline void bootspi_receive_only(void* r_buf, uint16_t r_size)
{
	assert(r_buf);
	// assert(r_size <= 65535);

	bootspi_disable();

	uint8_t *r_buf_p = r_buf;

	uint32_t ctrl0 = BOOTSPI->CTRLR0;
	//TMOD = receive_only
	ctrl0 &= ~SPI_TMOD_Msk;
	ctrl0 |= 2 << SPI_TMOD_Pos;
	ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
	BOOTSPI->CTRLR0 = ctrl0;

	BOOTSPI->CTRLR1 = r_size - 1;

	bootspi_enable();
	bootspi_select_slave(1U);

	//开始接受
	BOOTSPI->DR = 0xff;

	//等待数据接收完
	for (uint16_t i = 0; i < r_size; i++) {
		while (bootspi_rxfifo_is_empty())
			;
		r_buf_p[i] = BOOTSPI->DR;
	}

	bootspi_disable();
}

void dw_spi_receive_only(spi_id_t spi_id, void* r_buf, uint16_t r_size)
{
	assert(get_spi_state(spi_id)!=0);
	assert(spi_id <= 3);
	assert(r_buf);
	// assert(r_size <= 65535);

	if(spi_id == BOOTSPI_ID)
		return bootspi_receive_only(r_buf, r_size);

	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
	assert(spix);
	// if (spix->CTRLR & 1) {
		uint8_t *r_buf_p = r_buf;

		spi_disable(spi_id, true);

		uint32_t ctrl0 = spix->CTRLR0;
		//TMOD = receive_only
		ctrl0 &= ~SPI_TMOD_Msk;
		ctrl0 |= 2 << SPI_TMOD_Pos;
		ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
		spix->CTRLR0 = ctrl0;

		spix->CTRLR1 = r_size - 1;

		spi_enable(spi_id, true);
		spi_select_slave(spi_id, 1);

		//开始接收
		spix->DR = 0xff;

		//等待数据接收完
		for (uint16_t i = 0; i < r_size; i++) {
			while ((spix->SR & SPI_RFNE_Msk) == 0)
				;
			r_buf_p[i] = spix->DR;
		}

		spi_disable(spi_id, true);
	// } else {
	// }
}

static inline void bootspi_transmit_only(void* t_buf, uint32_t t_size, bool safe_mode)
{
	assert(t_buf);
	if(safe_mode)
		assert(t_size <= 32);
	bootspi_disable();

	uint8_t *t_buf_p = t_buf;

	uint32_t ctrl0 = BOOTSPI->CTRLR0;

	//TMOD = transmit_only
	ctrl0 &= ~SPI_TMOD_Msk;
	ctrl0 |= 1 << SPI_TMOD_Pos;

	ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF

	BOOTSPI->CTRLR0 = ctrl0;

	bootspi_enable();

	if (!safe_mode) //不限制传输数据为FIFO DEPTH大小
	{
		bootspi_select_slave(1U);
	}

	//填充发送数据
	while (t_size--) {
		if (!safe_mode)
			while ((BOOTSPI->SR & SPI_TFNF_Msk) == 0);
		BOOTSPI->DR = *t_buf_p++;
	}

	if(safe_mode)
		bootspi_select_slave(1U);

	//等待数据发送完成
	while( !( BOOTSPI->SR & (1UL << SR_TFE) ) ); //waiting TFE for 1!!! dw_ssi 2.7.1.3
    while( ( BOOTSPI->SR & (1UL << SR_BUSY) ) );

	bootspi_disable();
}

void dw_spi_transmit_only(spi_id_t spi_id, void* t_buf, uint32_t t_size, bool safe_mode)
{
	assert(get_spi_state(spi_id)!=0);
	assert(spi_id <= 3);
	assert(t_buf);
	if(safe_mode)
		assert(t_size <= 32);

	if(spi_id == BOOTSPI_ID)
		return bootspi_transmit_only(t_buf, t_size, safe_mode);

	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
	assert(spix);

	// if (spix->CTRLR & 1) //as master
	// {
		uint8_t *t_buf_p = t_buf;

		spi_disable(spi_id, true);

		uint32_t ctrl0 = spix->CTRLR0;
		//TMOD = transmit_only
		ctrl0 &= ~SPI_TMOD_Msk;
		ctrl0 |= 1 << SPI_TMOD_Pos;
		ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
		spix->CTRLR0 = ctrl0;

		spi_enable(spi_id, true);

		if (!safe_mode) //不限制传输数据为FIFO DEPTH大小
			spi_select_slave(spi_id, 1);

		//填充发送数据
		uint32_t cnt = t_size;
		while (cnt--) {
			if (!safe_mode)
				while ((spix->SR & SPI_TFNF_Msk) == 0);
			spix->DR = *t_buf_p++;
		}

		if (safe_mode) //限制传输数据为FIFO DEPTH大小
			spi_select_slave(spi_id, 1);

		//等待数据接收完
		while (!(spix->SR & SPI_TFE_Msk));
		while ((spix->SR & SPI_BUSY_Msk));

		spi_disable(spi_id, true);
	// } else {
	// }
}

/* Enhanced SPI Modes */
static inline void bootspi_enhanced_read(spi_id_t spi_id, enhanced_transfer_format_t *enhanced_transfer_format)
{
	assert(get_spi_state(spi_id)!=0);
	assert(spi_id == 3);
	assert(enhanced_transfer_format);
	assert(enhanced_transfer_format->addr_lenth<=60);
	assert(enhanced_transfer_format->ins_lenth<=16);
	assert(enhanced_transfer_format->data);
	assert(enhanced_transfer_format->data_nums!=0 && enhanced_transfer_format->data_nums<=65536);
	assert(enhanced_transfer_format->wait_cycles<=31);

	uint8_t addr_lenth = enhanced_transfer_format->addr_lenth;
	uint8_t ins_lenth = enhanced_transfer_format->ins_lenth;
	//配置spi 
	spi_disable(spi_id, true);
	{
		uint32_t ctrl0 = BOOTSPI->CTRLR0;
		uint32_t spi_ctrl0 = BOOTSPI->SPI_CTRLR0;
		{
			// 1.1 配置几线传输
			ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
			ctrl0 |= enhanced_transfer_format->spi_frf << SPI_SPI_FRF_Pos;

			ctrl0 &= ~SPI_TMOD_Msk;
			ctrl0 |= 2 << SPI_TMOD_Pos; //read

			// 1.2 配置 TRANS_TYPE
			// 00 - Instruction and Address will be sent in Standard SPI Mode.
			// 01 - Instruction will be sent in Standard SPI Mode and Address will be sent in the mode specified by CTRLR0.SPI_FRF.
			// 10 - 10 - Both Instruction and Address will be sent in the mode specified by SPI_FRF.
			// 11 - Reserved.
			spi_ctrl0 &= ~SPI_TRANS_TYPE_Msk;
			spi_ctrl0 |= enhanced_transfer_format->trans_type << SPI_TRANS_TYPE_Pos;

			// 1.3 ADDR_L
			spi_ctrl0 &= ~SPI_ADDR_L_Msk;
			spi_ctrl0 |= (addr_lenth/4) << SPI_ADDR_L_Pos;

			// 1.4 INST_L
			spi_ctrl0 &= ~SPI_INST_L_Msk;
			if(ins_lenth==16)
				spi_ctrl0 |= 3 << SPI_INST_L_Pos;
			else
				spi_ctrl0 |= (ins_lenth/4) << SPI_INST_L_Pos;

			// 1.5 WAIT_CYCLES
			spi_ctrl0 &= ~SPI_WAIT_CYCLES_Msk;
			spi_ctrl0 |= enhanced_transfer_format->wait_cycles << SPI_WAIT_CYCLES_Pos;
		
			// 1.6 DDR disable
			spi_ctrl0 &= ~SPI_SPI_DDR_EN_Msk;
			spi_ctrl0 &= ~SPI_INST_DDR_EN_Msk;
			spi_ctrl0 &= ~SPI_SPI_RXDS_EN_Msk;
		} 
		BOOTSPI->CTRLR0 = ctrl0;
		BOOTSPI->SPI_CTRLR0 = spi_ctrl0;

		//配置接收数据量
		BOOTSPI->CTRLR1 = enhanced_transfer_format->data_nums - 1;
	}
	spi_enable(spi_id, true);


	//传输阶段
	uint8_t quotient = addr_lenth / 32; 
	uint8_t remainder = addr_lenth % 32;
	uint8_t cnt = (remainder) ? (quotient + 1) : (quotient);
	uint32_t* address = enhanced_transfer_format->address;
	uint8_t *data = enhanced_transfer_format->data;
	uint32_t data_nums = enhanced_transfer_format->data_nums;
Instruction_phase:
	if(ins_lenth!=0)
		BOOTSPI->DR = enhanced_transfer_format->instruction; //An instruction takes one FIFO location
Address_phase:
	//address can take more than one FIFO locations
	for (uint8_t i = 0; i < cnt; i++)
			BOOTSPI->DR = address[i];
	
	spi_select_slave(spi_id, 1);
	if(addr_lenth==0 && ins_lenth==0)
		BOOTSPI->DR = 0xff; //write a dummy data to start transmit.
Wait_cycles:
	//waiting...
Data_phase:
	for (uint8_t i = 0; i < data_nums; i++) {
		while (bootspi_rxfifo_is_empty());
		data[i] = BOOTSPI->DR;
	}

	spi_disable(spi_id, true);
	return;
}

void dw_spi_enhanced_read(spi_id_t spi_id, enhanced_transfer_format_t *enhanced_transfer_format)
{
	assert(get_spi_state(spi_id)!=0);
	assert(spi_id <= 3);
	assert(enhanced_transfer_format);
	assert(enhanced_transfer_format->addr_lenth<=60);
	assert(enhanced_transfer_format->ins_lenth<=16);
	assert(enhanced_transfer_format->data);
	assert(enhanced_transfer_format->data_nums!=0 && enhanced_transfer_format->data_nums<=65536);
	assert(enhanced_transfer_format->wait_cycles<=31);

	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
	assert(spix);
	if((uintptr_t)spix == (uintptr_t)BOOTSPI_BASE)
		return bootspi_enhanced_read(spi_id, enhanced_transfer_format);
	uint8_t addr_lenth = enhanced_transfer_format->addr_lenth;
	uint8_t ins_lenth = enhanced_transfer_format->ins_lenth;
	//配置spi 
	spi_disable(spi_id, true);
	{
		uint32_t ctrl0 = spix->CTRLR0;
		uint32_t spi_ctrl0 = spix->SPI_CTRLR0;
		{
			// 1.1 配置几线传输
			ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
			ctrl0 |= enhanced_transfer_format->spi_frf << SPI_SPI_FRF_Pos;

			ctrl0 &= ~SPI_TMOD_Msk;
			ctrl0 |= 2 << SPI_TMOD_Pos; //read

			// 1.2 配置 TRANS_TYPE
			// 00 - Instruction and Address will be sent in Standard SPI Mode.
			// 01 - Instruction will be sent in Standard SPI Mode and Address will be sent in the mode specified by CTRLR0.SPI_FRF.
			// 10 - 10 - Both Instruction and Address will be sent in the mode specified by SPI_FRF.
			// 11 - Reserved.
			spi_ctrl0 &= ~SPI_TRANS_TYPE_Msk;
			spi_ctrl0 |= enhanced_transfer_format->trans_type << SPI_TRANS_TYPE_Pos;

			// 1.3 ADDR_L
			spi_ctrl0 &= ~SPI_ADDR_L_Msk;
			spi_ctrl0 |= (addr_lenth/4) << SPI_ADDR_L_Pos;

			// 1.4 INST_L
			spi_ctrl0 &= ~SPI_INST_L_Msk;
			if(ins_lenth==16)
				spi_ctrl0 |= 3 << SPI_INST_L_Pos;
			else
				spi_ctrl0 |= (ins_lenth/4) << SPI_INST_L_Pos;

			// 1.5 WAIT_CYCLES
			spi_ctrl0 &= ~SPI_WAIT_CYCLES_Msk;
			spi_ctrl0 |= enhanced_transfer_format->wait_cycles << SPI_WAIT_CYCLES_Pos;
		
			// 1.6 DDR disable
			spi_ctrl0 &= ~SPI_SPI_DDR_EN_Msk;
			spi_ctrl0 &= ~SPI_INST_DDR_EN_Msk;
			spi_ctrl0 &= ~SPI_SPI_RXDS_EN_Msk;
		} 
		spix->CTRLR0 = ctrl0;
		spix->SPI_CTRLR0 = spi_ctrl0;

		//配置接收数据量
		spix->CTRLR1 = enhanced_transfer_format->data_nums - 1;
	}
	spi_enable(spi_id, true);


	//传输阶段
	uint8_t quotient = addr_lenth / 32; 
	uint8_t remainder = addr_lenth % 32;
	uint8_t cnt = (remainder) ? (quotient + 1) : (quotient);
	uint32_t* address = enhanced_transfer_format->address;
	uint8_t *data = enhanced_transfer_format->data;
	uint32_t data_nums = enhanced_transfer_format->data_nums;
Instruction_phase:
	if(ins_lenth!=0)
		spix->DR = enhanced_transfer_format->instruction; //An instruction takes one FIFO location
Address_phase:
	//address can take more than one FIFO locations
	for (uint8_t i = 0; i < cnt; i++)
			spix->DR = address[i];
	
	spi_select_slave(spi_id, 1);
	if(addr_lenth==0 && ins_lenth==0)
		spix->DR = 0xff; //write a dummy data to start transmit.
Wait_cycles:
	//waiting...
Data_phase:
	for (uint8_t i = 0; i < data_nums; i++) {
		while ((spix->SR & SPI_RFNE_Msk) == 0);
		data[i] = spix->DR;
	}

	spi_disable(spi_id, true);
	return;
}

static inline void bootspi_enhanced_write(spi_id_t spi_id, enhanced_transfer_format_t *enhanced_transfer_format, bool safe_mode)
{
	assert(get_spi_state(spi_id)!=0);
	assert(spi_id == BOOTSPI_ID);
	assert(enhanced_transfer_format);
	assert(enhanced_transfer_format->addr_lenth<=60);
	assert(enhanced_transfer_format->ins_lenth<=16);

	uint8_t addr_lenth = enhanced_transfer_format->addr_lenth;
	uint8_t ins_lenth = enhanced_transfer_format->ins_lenth;
	//配置spi 
	spi_disable(spi_id, true);
	{
		uint32_t ctrl0 = BOOTSPI->CTRLR0;
		uint32_t spi_ctrl0 = BOOTSPI->SPI_CTRLR0;
		{
			// 1.1 配置几线传输
			ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
			ctrl0 |= enhanced_transfer_format->spi_frf << SPI_SPI_FRF_Pos; //!!!标准传输中得配置回去，不然会影响标准传输

			ctrl0 &= ~SPI_TMOD_Msk;
			ctrl0 |= 1 << SPI_TMOD_Pos; //write

			// 1.2 配置 TRANS_TYPE
			// 00 - Instruction and Address will be sent in Standard SPI Mode.
			// 01 - Instruction will be sent in Standard SPI Mode and Address will be sent in the mode specified by CTRLR0.SPI_FRF.
			// 10 - 10 - Both Instruction and Address will be sent in the mode specified by SPI_FRF.
			// 11 - Reserved.
			spi_ctrl0 &= ~SPI_TRANS_TYPE_Msk;
			spi_ctrl0 |= enhanced_transfer_format->trans_type << SPI_TRANS_TYPE_Pos;

			// 1.3 ADDR_L
			spi_ctrl0 &= ~SPI_ADDR_L_Msk;
			spi_ctrl0 |= (addr_lenth/4) << SPI_ADDR_L_Pos;

			// 1.4 INST_L
			spi_ctrl0 &= ~SPI_INST_L_Msk;
			if(ins_lenth==16)
				spi_ctrl0 |= 3 << SPI_INST_L_Pos;
			else
				spi_ctrl0 |= (ins_lenth/4) << SPI_INST_L_Pos;

			// 1.5 WAIT_CYCLES
		
			// 1.6 DDR disable
			spi_ctrl0 &= ~SPI_SPI_DDR_EN_Msk;
			spi_ctrl0 &= ~SPI_INST_DDR_EN_Msk;
			spi_ctrl0 &= ~SPI_SPI_RXDS_EN_Msk;

		} 
		BOOTSPI->CTRLR0 = ctrl0;
		BOOTSPI->SPI_CTRLR0 = spi_ctrl0;
	}
	spi_enable(spi_id, true);
	//传输阶段
	uint8_t quotient = addr_lenth / 32; 
	uint8_t remainder = addr_lenth % 32;
	uint8_t cnt = (remainder) ? (quotient + 1) : (quotient);
	uint32_t* address = enhanced_transfer_format->address;
	uint8_t *data = enhanced_transfer_format->data;
	uint32_t data_nums = enhanced_transfer_format->data_nums;
	if(!safe_mode)
		spi_select_slave(spi_id, 1);
Instruction_phase:
	if(ins_lenth!=0)
		BOOTSPI->DR = enhanced_transfer_format->instruction; //An instruction takes one FIFO location
Address_phase:
	//address can take more than one FIFO locations
	for (uint8_t i = 0; i < cnt; i++)
			BOOTSPI->DR = address[i];
Data_phase:
	for(uint32_t i = 0;i<data_nums;i++)
	{
		if (!safe_mode)
				while ((BOOTSPI->SR & SPI_TFNF_Msk) == 0);
		BOOTSPI->DR = data[i];
	}

	if(safe_mode)
		spi_select_slave(spi_id, 1);
	
	//等待数据接收完
	while (!(BOOTSPI->SR & SPI_TFE_Msk));
	while ((BOOTSPI->SR & SPI_BUSY_Msk));

	spi_disable(spi_id, true);
	return;
}

void dw_spi_enhanced_write(spi_id_t spi_id, enhanced_transfer_format_t *enhanced_transfer_format, bool safe_mode)
{
	assert(get_spi_state(spi_id)!=0);
	assert(spi_id <= 3);
	assert(enhanced_transfer_format);
	assert(enhanced_transfer_format->addr_lenth<=60);
	assert(enhanced_transfer_format->ins_lenth<=16);

	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
	assert(spix);
	if((uintptr_t)spix == BOOTSPI_BASE)
		return bootspi_enhanced_write(spi_id, enhanced_transfer_format, safe_mode);
	uint8_t addr_lenth = enhanced_transfer_format->addr_lenth;
	uint8_t ins_lenth = enhanced_transfer_format->ins_lenth;
	//配置spi 
	spi_disable(spi_id, true);
	{
		uint32_t ctrl0 = spix->CTRLR0;
		uint32_t spi_ctrl0 = spix->SPI_CTRLR0;
		{
			// 1.1 配置几线传输
			ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
			ctrl0 |= enhanced_transfer_format->spi_frf << SPI_SPI_FRF_Pos; //!!!标准传输中得配置回去，不然会影响标准传输

			ctrl0 &= ~SPI_TMOD_Msk;
			ctrl0 |= 1 << SPI_TMOD_Pos; //write

			// 1.2 配置 TRANS_TYPE
			// 00 - Instruction and Address will be sent in Standard SPI Mode.
			// 01 - Instruction will be sent in Standard SPI Mode and Address will be sent in the mode specified by CTRLR0.SPI_FRF.
			// 10 - 10 - Both Instruction and Address will be sent in the mode specified by SPI_FRF.
			// 11 - Reserved.
			spi_ctrl0 &= ~SPI_TRANS_TYPE_Msk;
			spi_ctrl0 |= enhanced_transfer_format->trans_type << SPI_TRANS_TYPE_Pos;

			// 1.3 ADDR_L
			spi_ctrl0 &= ~SPI_ADDR_L_Msk;
			spi_ctrl0 |= (addr_lenth/4) << SPI_ADDR_L_Pos;

			// 1.4 INST_L
			spi_ctrl0 &= ~SPI_INST_L_Msk;
			if(ins_lenth==16)
				spi_ctrl0 |= 3 << SPI_INST_L_Pos;
			else
				spi_ctrl0 |= (ins_lenth/4) << SPI_INST_L_Pos;

			// 1.5 WAIT_CYCLES
		
			// 1.6 DDR disable
			spi_ctrl0 &= ~SPI_SPI_DDR_EN_Msk;
			spi_ctrl0 &= ~SPI_INST_DDR_EN_Msk;
			spi_ctrl0 &= ~SPI_SPI_RXDS_EN_Msk;

		} 
		spix->CTRLR0 = ctrl0;
		spix->SPI_CTRLR0 = spi_ctrl0;
	}
	spi_enable(spi_id, true);

	//传输阶段
	uint8_t quotient = addr_lenth / 32; 
	uint8_t remainder = addr_lenth % 32;
	uint8_t cnt = (remainder) ? (quotient + 1) : (quotient);
	uint32_t* address = enhanced_transfer_format->address;
	uint8_t *data = enhanced_transfer_format->data;
	uint32_t data_nums = enhanced_transfer_format->data_nums;
	if(!safe_mode)
		spi_select_slave(spi_id, 1);
Instruction_phase:
	if(ins_lenth!=0)
		spix->DR = enhanced_transfer_format->instruction; //An instruction takes one FIFO location
Address_phase:
	//address can take more than one FIFO locations
	for (uint8_t i = 0; i < cnt; i++)
			spix->DR = address[i];
Data_phase:
	for(uint32_t i = 0;i<data_nums;i++)
	{
		if (!safe_mode)
				while ((spix->SR & SPI_TFNF_Msk) == 0);
		spix->DR = data[i];
	}

	if(safe_mode)
		spi_select_slave(spi_id, 1);
	
	//等待数据接收完
	while (!(spix->SR & SPI_TFE_Msk));
	while ((spix->SR & SPI_BUSY_Msk));

	spi_disable(spi_id, true);
	return;
}

static inline void spi_set_rx_sample_dly(spi_id_t spi_id, uint8_t rx_sample_dly)
{
	assert(spi_id<=3);
	spi_disable(spi_id,true);
	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
	switch (spi_id)
	{
	case BOOTSPI_ID:
		BOOTSPI->RX_SAMPLE_DLY = rx_sample_dly;
		break;
	case SPI0_ID:
	case SPI1_ID:
	case SPI2_ID:
		spix->RX_SAMPLE_DLY = rx_sample_dly;
		break;
	default:
		assert(0);
	}
}

int16_t dw_spi_tuning(spi_id_t spi_id, void *t_buf, uint8_t t_size, void *r_buf, uint16_t r_size)
{
	int16_t min = -1;
	int16_t max = 0;
	uint8_t _r_buf[r_size];
	for(int16_t i=0;i<=8;i++)
	{
		spi_set_rx_sample_dly(spi_id, i);
		dw_spi_eeprom_read(spi_id, t_buf, t_size, _r_buf, r_size);
		if(memcmp(r_buf, _r_buf, r_size) == 0)
		{
			if(min==-1)
				min = i;
			max = i;
		}
		else
		{
			if(min!=-1)
				break;
		}
	}
	if(min==-1)
		return -1;
	uint8_t mid = (min+max)/2;
	spi_set_rx_sample_dly(spi_id, mid);
	return mid;
}

// /****************** STD & DMA **********************/
// #ifndef SPI_DMA_DRIVER_VERSION
// 	#define SPI_DMA_DRIVER_VERSION 0
// #endif

// #if (SPI_DMA_DRIVER_VERSION > 0)
// 	#include "ape1210_dma.h"
// 	typedef enum dma0_chan DMA_Channel_t;

// 	static volatile uint8_t dma_transfer_sig[8] = {0};


// 	static inline bool is_dma_channel_transfer_done(DMA_Channel_t ch)
// 	{
// 		return (bool)dma_transfer_sig[(uint32_t)ch];
// 	}

// 	static inline void free_dma_channel(DMA_Channel_t ch)
// 	{
// 		dma_transfer_sig[(uint32_t)ch] = 0;
// 		dma_destory(ch);
// 	}

// 	int spi_dma_irq_handler(void* arg)
// 	{
// 		dma_transfer_sig[(uint32_t)arg] = 1;
// 		return 0;
// 	}
// #else
// 	uint8_t dma_channel_state[8] = {0};
// 	#include "dma.h"
// #endif

// static inline DMA_Channel_t boot_spi_eeprom_read_dma_start(void *t_buf, uint8_t t_size, void *r_buf, uint16_t r_size)
// {
// 	assert(t_buf);
// 	assert(r_buf);
// 	assert((t_size<=32) && (t_size!=0));
// 	assert(r_size<=2048);

// 	uint8_t *t_buf_p = t_buf;
// 	uint8_t *r_buf_p = r_buf;

// 	bootspi_disable();

// 	uint32_t ctrl0 = BOOTSPI->CTRLR0;
// 	//TMOD = EEPROM Read
// 	ctrl0 &= ~SPI_TMOD_Msk;
// 	ctrl0 |= 3 << SPI_TMOD_Pos;
// 	ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
// 	BOOTSPI->CTRLR0 = ctrl0;

// 	BOOTSPI->CTRLR1 = r_size - 1;

// 	//build-in dma配置
// 	BOOTSPI->DMACR = 0;
// 	BOOTSPI->DMARDLR = 15; //16触发DMA请求
// 	// BOOTSPI->DMARDLR = 0; //1触发DMA请求
// 	BOOTSPI->DMACR = 1;

// 	bootspi_enable();

// 	//填充发送数据
// 	while (t_size--) {
// 		BOOTSPI->DR = *t_buf_p++; //t_size <= tx_fifo_depth
// 	}

// 	// dma配置
// #if (SPI_DMA_DRIVER_VERSION > 0)
// 	int ret = 0;
// 	DMA_Channel_t ch = DMA0_CH0_INDEX;
// 	dma_transfer_sig[(uint32_t)ch] = 0;
// 	ret = dma_init();
// 	assert(ret==0);
// 	dma_register_callback(ch, spi_dma_irq_handler, (void*)DMA0_CH0_INDEX);

// 	struct dma_slave_config config = {
// 		.direction      = DMA_DEV_TO_MEM,
// 		.src_addr       = (uint32_t)(&(BOOTSPI->DR)),
// 		.dst_addr       = (uint32_t)r_buf_p,
// 		.src_maxburst   = 3, //MSIZE_16
// 		.dst_maxburst   = 3, //MSIZE_16
// 		.data_len       = r_size,
// 		.src_width    	= DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 		.dst_width    	= DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 		.device_fc      = false,
// 		.slave_id 		= 25,
// 		.cyclic         = false,
// 		.period_len		= 0
// 	};

// 	while((ret = dma_config(ch, &config)) != 0 );
// 	dma_enable(ch);
// #else
// 	DMA_Channel_t ch;
// 	while((ch = get_a_free_dma_channel()) == NO_FREE_DMA_CHANNEL);
// 	dma_config_t config = {
// 		.ch = ch,
// 		.sar = (uintptr_t)(&(BOOTSPI->DR)),
// 		.dar = (uintptr_t)r_buf_p,

// 		.axi_dst_burst_length = 16,
// 		.axi_src_burst_length = 16,
// 		.block_ts = r_size -1,
		
// 		.is_src_addr_increse = SRC_ADDR_NOCHANGE,
// 		.is_dst_addr_increse = DST_ADDR_INCREMENT,
// 		.src_transfer_width = SRC_TRANSFER_WIDTH_8,
// 		.dst_transfer_width = DST_TRANSFER_WIDTH_8,
// 		.dst_msize = DST_MSIZE_16,
// 		.src_msize = SRC_MSIZE_16,

// 		.handle_shake = 25,
// 		.dir = PER_TO_MEM
// 	};
// 	dma_config(&config);
// 	dma_channel_start(ch);
// #endif
// 	bootspi_select_slave(1U);
// 	return ch;
// }

// DMA_Channel_t dw_spi_eeprom_read_dma_start(spi_id_t spi_id, void *t_buf, uint8_t t_size, void *r_buf, uint32_t r_size)
// {
// 	//0. 判断输入合法性
// 	assert(get_spi_state(spi_id)!=0);
// 	assert(spi_id <= 3);
// 	assert(t_buf);
// 	assert(r_buf);
// 	assert((t_size<=32) && (t_size!=0));
// 	assert(r_size <= 2048);

// 	//1. bootspi or spix
// 	if(spi_id == BOOTSPI_ID)
// 		return boot_spi_eeprom_read_dma_start(t_buf, t_size, r_buf, r_size);

// 	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
// 	assert(spix);
// 	uint8_t handshake;
// 	switch (spi_id)
// 	{
// 	case SPI0_ID:
// 		handshake = 2;
// 		break;
// 	case SPI1_ID:
// 		handshake = 4;
// 		break;
// 	case SPI2_ID:
// 		handshake = 6;
// 		break;
// 	case BOOTSPI_ID:
// 		assert(0);
// 		break;
// 	default:
// 		assert(0);
// 		break;
// 	}
// 	if (spix->CTRLR & 1) { //as master

// 		uint8_t *t_buf_p = t_buf;
// 		uint8_t *r_buf_p = r_buf;

// 		spi_disable(SPI0_ID, true);

// 		uint32_t ctrl0 = spix->CTRLR0;

// 		//TMOD = EEPROM Read
// 		ctrl0 &= ~SPI_TMOD_Msk;
// 		ctrl0 |= 3 << SPI_TMOD_Pos;

// 		ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF

// 		spix->CTRLR0 = ctrl0;

// 		spix->CTRLR1 = r_size - 1;

// 		//build-in dma配置
// 		spix->DMACR = 0;
// 		// spix->DMARDLR = 15; //16触发DMA请求
// 		spix->DMARDLR = 0; //1触发DMA请求
// 		spix->DMACR = 1;

// 		spix->SSIENR = 1;

// 		//填充发送数据
// 		while (t_size--) {
// 			spix->DR = *t_buf_p++;
// 		}

// 		// dma配置
// #if (SPI_DMA_DRIVER_VERSION > 0)
// 		int ret = 0;
// 		DMA_Channel_t ch = DMA0_CH0_INDEX;
// 		ret = dma_init();
// 		assert(ret==0);
// 		dma_register_callback(ch, spi_dma_irq_handler, NULL);

// 		struct dma_slave_config config = {
// 			.direction      = DMA_DEV_TO_MEM,
// 			.src_addr       = (uint32_t)(&(spix->DR)),
// 			.dst_addr       = (uint32_t)r_buf_p,
// 			.src_maxburst   = 0,
// 			.dst_maxburst   = 0,
// 			.data_len       = r_size,
// 			.src_width      = DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 			.dst_width      = DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 			.device_fc      = false,
// 			.slave_id 		= handshake,
// 			.cyclic         = false,
// 			.period_len		= 0
// 		};
		
// 		while((ret = dma_config(ch, &config)) != 0 );
// 		dma_enable(ch);
// #else
// 		DMA_Channel_t ch;
// 		while((ch = get_a_free_dma_channel()) == NO_FREE_DMA_CHANNEL);
// 		dma_config_t config = {
// 			.ch = ch,
// 			.sar = (uintptr_t)(&(spix->DR)),
// 			.dar = (uintptr_t)r_buf_p,

// 			.axi_dst_burst_length = 0,
// 			.axi_src_burst_length = 0,
// 			.block_ts = r_size -1,
			
// 			.is_src_addr_increse = SRC_ADDR_NOCHANGE,
// 			.is_dst_addr_increse = DST_ADDR_INCREMENT,
// 			.src_transfer_width = SRC_TRANSFER_WIDTH_8,
// 			.dst_transfer_width = DST_TRANSFER_WIDTH_8,
// 			.dst_msize = SRC_MSIZE_1,
// 			.src_msize = DST_MSIZE_1,

// 			.handle_shake = handshake,
// 			.dir = PER_TO_MEM
// 		};
// 		dma_config(&config);
// 		dma_channel_start(ch);
// #endif
// 		spi_select_slave(spi_id, 1);
// 		return ch;
// 	} else {
// 	}
// #if (SPI_DMA_DRIVER_VERSION > 0)
// 	return DMA0_ERR;
// #else
// 	return NO_FREE_DMA_CHANNEL;
// #endif
// }

// bool is_dw_spi_eeprom_read_dma_end(spi_id_t spi_id, DMA_Channel_t ch)
// {
// 	if(is_dma_channel_transfer_done(ch))
// 	{
// 		#if (SPI_DMA_DRIVER_VERSION == 0)
// 			clear_channel_transfer_done_irq(ch);
// 		#endif
// 		free_dma_channel(ch);
// 		spi_disable(spi_id, true);
// 		return true;
// 	}
// 	return false;
// }

// void dw_spi_eeprom_read_dma(spi_id_t spi_id, void *t_buf, uint8_t t_size, void *r_buf, uint16_t r_size)
// {
// 	DMA_Channel_t ch = dw_spi_eeprom_read_dma_start(spi_id, t_buf, t_size, r_buf, r_size);
// 	while(!is_dw_spi_eeprom_read_dma_end(spi_id, ch));
// }

// static inline void bootspi_transmit_only_dma(void* t_buf, uint32_t t_size)
// {
// 	assert(t_buf);
// 	assert(t_size<=2048);
// 	if(t_size==0)
// 		return;
// 	// __DMB();
// 	bootspi_disable();

// 	uint32_t ctrl0 = BOOTSPI->CTRLR0;
// 	ctrl0 &= ~SPI_TMOD_Msk;
// 	ctrl0 |= 1 << SPI_TMOD_Pos; //TMOD = transmit_only
// 	ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
// 	BOOTSPI->CTRLR0 = ctrl0;

// 	//build-in DMA配置
// 	BOOTSPI->DMACR = 0;
// 	BOOTSPI->DMATDLR = 16; //<=16触发DMA请求
// 	// BOOTSPI->DMATDLR = 31; //<=31触发DMA请求
// 	BOOTSPI->DMACR = 2;

// 	bootspi_enable();

// 	// dma配置
// #if (SPI_DMA_DRIVER_VERSION > 0)
// 	int ret = 0;
// 	DMA_Channel_t ch = DMA0_CH0_INDEX;
// 	ret = dma_init();
// 	assert(ret==0);
// 	dma_register_callback(ch, spi_dma_irq_handler, NULL);

// 	struct dma_slave_config config = {
// 		.direction      = DMA_MEM_TO_DEV,
// 		.src_addr       = (uint32_t)t_buf,
// 		.dst_addr       = (uint32_t)(&(BOOTSPI->DR)),
// 		.src_maxburst   = 3,
// 		.dst_maxburst   = 3,
// 		.data_len       = t_size,
// 		.src_width    	= DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 		.dst_width    	= DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 		.device_fc      = false,
// 		.slave_id 		= 26,
// 		.cyclic         = false,
// 		.period_len		= 0
// 	};
	
// 	while((ret = dma_config(ch, &config)) != 0 );
// 	dma_enable(ch);
// #else
// 	while((REG32(DMA_BASE + DMAC_CHENREG_0) & 1) == 1);
// 	REG32(DMA_BASE + CH1_SAR_0) = (uintptr_t)t_buf;
// 	REG32(DMA_BASE + CH1_DAR_0) = (uintptr_t)(&(BOOTSPI->DR));

// 	REG32(DMA_BASE + CH1_CTL_0) = AXI_MASTER_0 << 0 |
// 									AXI_MASTER_0 << 2 |
// 									SRC_ADDR_INCREMENT << 4 |
// 									DST_ADDR_NOCHANGE << 6 |
// 									SRC_TRANSFER_WIDTH_8 << 8 |
// 									DST_TRANSFER_WIDTH_8 << 11 |
// 									//   SRC_MSIZE_16 << 14 |
// 									// DST_MSIZE_1 << 18 |
// 									DST_MSIZE_16 << 18 |
// 									NONPOSTED_LASTWRITE_EN << 30;

// 	REG32(DMA_BASE + CH1_CTL_32) = ARLEN_EN << (38 - 32) |
// 									16 << (39 - 32) | //axi source burst length
// 									AWLEN_EN << (47 - 32) |
// 									16 << (48 - 32) | //axi destination burst length
// 									SRC_STATUS_DISABLE << (56 - 32) |
// 									DST_STATUS_DISABLE << (57 - 32) |
// 									INTDISABLE_COMPLETOFBLKTRANS_SHADORLLI << (58 - 32) |
// 									NOTLAST_SHADORLLI << (62 - 32) |
// 									SHADORLLI_INVALID << (63 - 32);

// 	REG32(DMA_BASE + CH1_BLOCK_TS_0) = t_size - 1;

// 	REG32(DMA_BASE + CH1_CFG2_0) = SRC_CONTIGUOUS << 0 |
// 									DST_CONTIGUOUS << 2 |
// 									26U            << 11;

// 	REG32(DMA_BASE + CH1_CFG2_32) = MEM_TO_PER_DMAC << (32 - 32) |
// 									DST_HARDWARE_HS << (36 - 32) |
// 									/*26 << (44 - 32) | //src handshake*/
// 									CHANNEL_PRIORITY7 << (47 - 32) |
// 									CHANNEL_LOCK_DISABLE << (52 - 32) |
// 									0x4 << (55 - 32) | //Source Outstanding Request Limit == 3
// 									0x4 << (59 - 32);  //Destination Outstanding Request Limit == 3

// 	REG32(DMA_BASE + CH1_INTSTATUS_ENABLEREG_0) = 2; //Enable interrupt generation bit is valid
// 	REG32(DMA_BASE + CH1_INTSIGNAL_ENABLEREG_0) = 2; //Enable interrupt generation bit is valid
// 	REG32(DMA_BASE + DMAC_CFGREG_0) = 0x3;	//enable DMAC and its interrupt logic
// 	REG32(DMA_BASE + DMAC_CHENREG_0) = 0x101; //EN channel1  while(1)
// #endif
// 	//选择从机，开始发送
// 	bootspi_select_slave(1U);

// #if (SPI_DMA_DRIVER_VERSION > 0)
// 	while(!is_dma_channel_transfer_done(ch));
// 	free_dma_channel(ch);
// #else
// 	while ((REG32(DMA_BASE + CH1_INTSTATUS_0) & 0x00000002) == 0)
// 		;
// 	REG32(DMA_BASE + CH1_INTCLEARREG_0) = 0x00000002;
// #endif
	

// 	//等待数据发送完成
// 	while( !( BOOTSPI->SR & (1UL << SR_TFE) ) ); //waiting TFE for 1!!! dw_ssi 2.7.1.3
// 	while( ( BOOTSPI->SR & (1UL << SR_BUSY) ) );

// 	bootspi_disable();
// }

// void dw_spi_transmit_only_dma(spi_id_t spi_id, void* t_buf, uint32_t t_size)
// {
// 	assert(spi_id <= 3);
// 	assert(get_spi_state(spi_id)!=0);
// 	assert(t_buf);
// 	assert(t_size<=2048);

// 	__DMB();
// 	if(spi_id == BOOTSPI_ID)
// 		return bootspi_transmit_only_dma(t_buf, t_size);

// 	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
// 	assert(spix);

// 	uint8_t handshake;
// 	switch (spi_id)
// 	{
// 	case SPI0_ID:
// 		handshake = 3;
// 		break;
// 	case SPI1_ID:
// 		handshake = 5;
// 		break;
// 	case SPI2_ID:
// 		handshake = 7;
// 		break;
// 	case BOOTSPI_ID:
// 		assert(0);
// 		break;
// 	default:
// 		assert(0);
// 		break;
// 	}

// 	if (spix->CTRLR & 1) //as master
// 	{
// 		spi_disable(spi_id, true);

// 		uint32_t ctrl0 = spix->CTRLR0;
// 		//TMOD = transmit_only
// 		ctrl0 &= ~SPI_TMOD_Msk;
// 		ctrl0 |= 1 << SPI_TMOD_Pos;

// 		ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
// 		spix->CTRLR0 = ctrl0;

// 		//build-in DMA配置
// 		spix->DMACR = 0;
// 		// spix->DMATDLR = 16; //<=16触发DMA请求
// 		spix->DMATDLR = 31; //<=31触发DMA请求
// 		spix->DMACR = 2;

// 		spi_enable(spi_id, true);

// 		// dma配置
// #if (SPI_DMA_DRIVER_VERSION > 0)
// 		int ret = 0;
// 		DMA_Channel_t ch = DMA0_CH0_INDEX;
// 		ret = dma_init();
// 		assert(ret==0);
// 		dma_register_callback(ch, spi_dma_irq_handler, NULL);

// 		struct dma_slave_config config = {
// 			.direction      = DMA_MEM_TO_DEV,
// 			.src_addr       = (uint32_t)t_buf,
// 			.dst_addr       = (uint32_t)(&(spix->DR)),
// 			.src_maxburst   = 0,
// 			.dst_maxburst   = 0,
// 			.data_len       = t_size,
// 			.src_width    	= DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 			.dst_width    	= DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 			.device_fc      = false,
// 			.slave_id 		= handshake,
// 			.cyclic         = false,
// 			.period_len		= 0
// 		};
		
// 		while((ret = dma_config(ch, &config)) != 0 );
// 		dma_enable(ch);
// #else
// 		while((REG32(DMA_BASE + DMAC_CHENREG_0) & 1) == 1);
// 		REG32(DMA_BASE + CH1_SAR_0) = (uintptr_t)t_buf;
// 		REG32(DMA_BASE + CH1_DAR_0) = (uintptr_t)(&(spix->DR));

// 		REG32(DMA_BASE + CH1_CTL_0) = AXI_MASTER_0 << 0 |
// 										AXI_MASTER_0 << 2 |
// 										SRC_ADDR_INCREMENT << 4 |
// 										DST_ADDR_NOCHANGE << 6 |
// 										SRC_TRANSFER_WIDTH_8 << 8 |
// 										DST_TRANSFER_WIDTH_8 << 11 |
// 										//   SRC_MSIZE_16 << 14 |
// 										DST_MSIZE_1 << 18 |
// 										// DST_MSIZE_16 << 18 |
// 										NONPOSTED_LASTWRITE_EN << 30;

// 		REG32(DMA_BASE + CH1_CTL_32) = ARLEN_EN << (38 - 32) |
// 										0 << (39 - 32) | //axi source burst length
// 										AWLEN_EN << (47 - 32) |
// 										0 << (48 - 32) | //axi destination burst length
// 										SRC_STATUS_DISABLE << (56 - 32) |
// 										DST_STATUS_DISABLE << (57 - 32) |
// 										INTDISABLE_COMPLETOFBLKTRANS_SHADORLLI << (58 - 32) |
// 										NOTLAST_SHADORLLI << (62 - 32) |
// 										SHADORLLI_INVALID << (63 - 32);

// 		REG32(DMA_BASE + CH1_BLOCK_TS_0) = t_size - 1;

// 		REG32(DMA_BASE + CH1_CFG2_0) = SRC_CONTIGUOUS << 0 |
// 										DST_CONTIGUOUS << 2 |
// 										handshake      << 11;

// 		REG32(DMA_BASE + CH1_CFG2_32) = MEM_TO_PER_DMAC << (32 - 32) |
// 										DST_HARDWARE_HS << (36 - 32) |
// 										/*26 << (44 - 32) | //src handshake*/
// 										CHANNEL_PRIORITY7 << (47 - 32) |
// 										CHANNEL_LOCK_DISABLE << (52 - 32) |
// 										0x4 << (55 - 32) | //Source Outstanding Request Limit == 3
// 										0x4 << (59 - 32);  //Destination Outstanding Request Limit == 3

// 		REG32(DMA_BASE + CH1_INTSTATUS_ENABLEREG_0) = 0xffffffff; //Enable interrupt generation bit is valid
// 		REG32(DMA_BASE + CH1_INTSIGNAL_ENABLEREG_0) = 0xffffffff; //Enable interrupt generation bit is valid
// 		REG32(DMA_BASE + DMAC_CFGREG_0) = 0x3;	//enable DMAC and its interrupt logic
// 		REG32(DMA_BASE + DMAC_CHENREG_0) = 0x101; //EN channel1  while(1)
// #endif
// 		spi_select_slave(spi_id, 1);

// #if (SPI_DMA_DRIVER_VERSION > 0)
// 		while(!is_dma_channel_transfer_done(ch));
// 		free_dma_channel(ch);
// #else
// 		while ((REG32(DMA_BASE + CH1_INTSTATUS_0) & 0x00000002) == 0)
// 			;
// 		REG32(DMA_BASE + CH1_INTCLEARREG_0) = 0x00000002;
// #endif

// 		//等待数据接收完
// 		while (!(spix->SR & SPI_TFE_Msk));
// 		while ((spix->SR & SPI_BUSY_Msk));

// 		spi_disable(spi_id, true);
// 	} else {
// 	}
// }

// /********* Enhanced + DMA ********/
// static inline void _bootspi_enhanced_read_dma(spi_id_t spi_id, enhanced_transfer_format_t *enhanced_transfer_format)
// {
// 	assert(get_spi_state(spi_id)!=0);
// 	assert(spi_id == 3);
// 	assert(enhanced_transfer_format);
// 	assert(enhanced_transfer_format->addr_lenth<=60);
// 	assert(enhanced_transfer_format->ins_lenth<=16);
// 	assert(enhanced_transfer_format->data);
// 	assert(enhanced_transfer_format->data_nums!=0 && enhanced_transfer_format->data_nums<=2048);
// 	assert(enhanced_transfer_format->wait_cycles<=31);

// 	uint8_t addr_lenth = enhanced_transfer_format->addr_lenth;
// 	uint8_t ins_lenth = enhanced_transfer_format->ins_lenth;
// 	//配置spi 
// 	spi_disable(spi_id, true);
// 	{
// 		uint32_t ctrl0 = BOOTSPI->CTRLR0;
// 		uint32_t spi_ctrl0 = BOOTSPI->SPI_CTRLR0;
// 		{
// 			// 1.1 配置几线传输
// 			ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
// 			ctrl0 |= enhanced_transfer_format->spi_frf << SPI_SPI_FRF_Pos;

// 			ctrl0 &= ~SPI_TMOD_Msk;
// 			ctrl0 |= 2 << SPI_TMOD_Pos; //read

// 			// 1.2 配置 TRANS_TYPE
// 			// 00 - Instruction and Address will be sent in Standard SPI Mode.
// 			// 01 - Instruction will be sent in Standard SPI Mode and Address will be sent in the mode specified by CTRLR0.SPI_FRF.
// 			// 10 - 10 - Both Instruction and Address will be sent in the mode specified by SPI_FRF.
// 			// 11 - Reserved.
// 			spi_ctrl0 &= ~SPI_TRANS_TYPE_Msk;
// 			spi_ctrl0 |= enhanced_transfer_format->trans_type << SPI_TRANS_TYPE_Pos;

// 			// 1.3 ADDR_L
// 			spi_ctrl0 &= ~SPI_ADDR_L_Msk;
// 			spi_ctrl0 |= (addr_lenth/4) << SPI_ADDR_L_Pos;

// 			// 1.4 INST_L
// 			spi_ctrl0 &= ~SPI_INST_L_Msk;
// 			if(ins_lenth==16)
// 				spi_ctrl0 |= 3 << SPI_INST_L_Pos;
// 			else
// 				spi_ctrl0 |= (ins_lenth/4) << SPI_INST_L_Pos;

// 			// 1.5 WAIT_CYCLES
// 			spi_ctrl0 &= ~SPI_WAIT_CYCLES_Msk;
// 			spi_ctrl0 |= enhanced_transfer_format->wait_cycles << SPI_WAIT_CYCLES_Pos;
		
// 			// 1.6 DDR disable
// 			spi_ctrl0 &= ~SPI_SPI_DDR_EN_Msk;
// 			spi_ctrl0 &= ~SPI_INST_DDR_EN_Msk;
// 			spi_ctrl0 &= ~SPI_SPI_RXDS_EN_Msk;
// 		} 
// 		BOOTSPI->CTRLR0 = ctrl0;
// 		BOOTSPI->SPI_CTRLR0 = spi_ctrl0;

// 		//build-in dma配置
// 		BOOTSPI->DMACR = 0;
// 		BOOTSPI->DMARDLR = 15; //16触发DMA请求
// 		// BOOTSPI->DMARDLR = 0; //1触发DMA请求
// 		BOOTSPI->DMACR = 1;

// 		//配置接收数据量
// 		BOOTSPI->CTRLR1 = enhanced_transfer_format->data_nums - 1;
// 	}
// 	spi_enable(spi_id, true);

// 	// dma配置
// #if (SPI_DMA_DRIVER_VERSION > 0)
// 	int ret = 0;
// 	DMA_Channel_t ch = DMA0_CH0_INDEX;
// 	dma_transfer_sig[(uint32_t)ch] = 0;
// 	ret = dma_init();
// 	assert(ret==0);
// 	dma_register_callback(ch, spi_dma_irq_handler, (void*)DMA0_CH0_INDEX);

// 	struct dma_slave_config config = {
// 		.direction      = DMA_DEV_TO_MEM,
// 		.src_addr       = (uint32_t)(&(BOOTSPI->DR)),
// 		.dst_addr       = (uint32_t)(enhanced_transfer_format->data),
// 		.src_maxburst   = 3, //MSIZE_16
// 		.dst_maxburst   = 3, //MSIZE_16
// 		.data_len       = enhanced_transfer_format->data_nums,
// 		.src_width      = DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 		.dst_width      = DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 		.device_fc      = false,
// 		.slave_id 		= 25,
// 		.cyclic         = false,
// 		.period_len		= 0
// 	};

// 	while((ret = dma_config(ch, &config)) != 0 );
// 	dma_enable(ch);
// #else
// 	while((REG32(DMA_BASE + DMAC_CHENREG_0) & 1) == 1);
// 	REG32(DMA_BASE + CH1_SAR_0) = (uintptr_t)(&(BOOTSPI->DR));
// 	REG32(DMA_BASE + CH1_DAR_0) = (uintptr_t)(enhanced_transfer_format->data);

// 	REG32(DMA_BASE + CH1_CTL_0) = AXI_MASTER_0 << 0 |
// 								AXI_MASTER_0 << 2 |
// 								SRC_ADDR_NOCHANGE << 4 |
// 								DST_ADDR_INCREMENT << 6 |
// 								SRC_TRANSFER_WIDTH_8 << 8 |
// 								DST_TRANSFER_WIDTH_8 << 11 |
// 								SRC_MSIZE_16 << 14 |
// 								// SRC_MSIZE_1 << 14 |
// 									//   DST_MSIZE_16 <<18  |
// 								NONPOSTED_LASTWRITE_EN << 30;

// 	REG32(DMA_BASE + CH1_CTL_32) = ARLEN_EN << (38 - 32) |
// 									16 << (39 - 32) | //source burst length
// 									AWLEN_EN << (47 - 32) |
// 									16 << (48 - 32) | //destination burst length
// 									SRC_STATUS_DISABLE << (56 - 32) |
// 									DST_STATUS_DISABLE << (57 - 32) |
// 									INTDISABLE_COMPLETOFBLKTRANS_SHADORLLI << (58 - 32) |
// 									NOTLAST_SHADORLLI << (62 - 32) |
// 									SHADORLLI_INVALID << (63 - 32);

// 	REG32(DMA_BASE + CH1_BLOCK_TS_0) = enhanced_transfer_format->data_nums - 1;

// 	REG32(DMA_BASE + CH1_CFG2_0) = SRC_CONTIGUOUS << 0 |
// 								DST_CONTIGUOUS << 2 |
// 								25U            << 4;

// 	REG32(DMA_BASE + CH1_CFG2_32) = PER_TO_MEM_DMAC << (32 - 32) |
// 									SRC_HARDWARE_HS << (35 - 32) |
// 									/*25 << (39 - 32) | //src handshake*/
// 									CHANNEL_PRIORITY7 << (47 - 32) |
// 									CHANNEL_LOCK_DISABLE << (52 - 32) |
// 									0x4 << (55 - 32) | //Source Outstanding Request Limit == 3
// 									0x4 << (59 - 32);  //Destination Outstanding Request Limit == 3

// 	REG32(DMA_BASE + CH1_INTSTATUS_ENABLEREG_0) = 2; //Enable interrupt generation bit is valid
// 	REG32(DMA_BASE + CH1_INTSIGNAL_ENABLEREG_0) = 2; //Enable interrupt generation bit is valid
// 	REG32(DMA_BASE + DMAC_CFGREG_0) = 0x3;           //enable DMAC and its interrupt logic
// 	REG32(DMA_BASE + DMAC_CHENREG_0) = 0x101;        //EN channel1
// #endif

// 	//传输阶段
// 	uint8_t quotient = addr_lenth / 32; 
// 	uint8_t remainder = addr_lenth % 32;
// 	uint8_t cnt = (remainder) ? (quotient + 1) : (quotient);
// 	uint32_t* address = enhanced_transfer_format->address;
// 	// uint8_t *data = enhanced_transfer_format->data;
// 	// uint32_t data_nums = enhanced_transfer_format->data_nums;
// Instruction_phase:
// 	if(ins_lenth!=0)
// 		BOOTSPI->DR = enhanced_transfer_format->instruction; //An instruction takes one FIFO location
// Address_phase:
// 	//address can take more than one FIFO locations
// 	for (uint8_t i = 0; i < cnt; i++)
// 			BOOTSPI->DR = address[i];
	
// 	spi_select_slave(spi_id, 1);
// 	if(addr_lenth==0 && ins_lenth==0)
// 		BOOTSPI->DR = 0xff; //write a dummy data to start transmit.
// Wait_cycles:
// 	//waiting...
// Data_phase:
// 	// for (uint8_t i = 0; i < data_nums; i++) {
// 	// 	while (bootspi_rxfifo_is_empty());
// 	// 	data[i] = BOOTSPI->DR;
// 	// }
// #if (SPI_DMA_DRIVER_VERSION == 0)
// 	while ((REG32(DMA_BASE + CH1_INTSTATUS_0) & 0x00000002) == 0)
// 		;

// 	REG32(DMA_BASE + CH1_INTCLEARREG_0) = 0x00000002;
// #else
// 	while(!is_dma_channel_transfer_done(ch));
// 	free_dma_channel(ch);
// #endif
// 	spi_disable(spi_id, true);
// 	return;
// }

// void dw_spi_enhanced_read_dma(spi_id_t spi_id, enhanced_transfer_format_t *enhanced_transfer_format)
// {
// 	assert(get_spi_state(spi_id)!=0);
// 	assert(spi_id <= 3);
// 	assert(enhanced_transfer_format);
// 	assert(enhanced_transfer_format->addr_lenth<=60);
// 	assert(enhanced_transfer_format->ins_lenth<=16);
// 	assert(enhanced_transfer_format->data);
// 	assert(enhanced_transfer_format->data_nums!=0 && enhanced_transfer_format->data_nums<=2048);
// 	assert(enhanced_transfer_format->wait_cycles<=31);

// 	DW_APB_SSI_TypeDef* spix = get_spi_base(spi_id);
// 	assert(spix);
// 	if((uintptr_t)spix == BOOTSPI_BASE)
// 		return _bootspi_enhanced_read_dma(spi_id, enhanced_transfer_format);
// 	uint8_t handshake;
// 	switch (spi_id)
// 	{
// 	case SPI0_ID:
// 		handshake = 2;
// 		break;
// 	case SPI1_ID:
// 		handshake = 4;
// 		break;
// 	case SPI2_ID:
// 		handshake = 6;
// 		break;
// 	case BOOTSPI_ID:
// 		assert(0);
// 		return;
// 	default:
// 		assert(0);
// 		return;
// 	}
// 	uint8_t addr_lenth = enhanced_transfer_format->addr_lenth;
// 	uint8_t ins_lenth = enhanced_transfer_format->ins_lenth;
// 	//配置spi 
// 	spi_disable(spi_id, true);
// 	{
// 		uint32_t ctrl0 = spix->CTRLR0;
// 		uint32_t spi_ctrl0 = spix->SPI_CTRLR0;
// 		{
// 			// 1.1 配置几线传输
// 			ctrl0 &= ~SPI_SPI_FRF_Msk; //SPI_FRF
// 			ctrl0 |= enhanced_transfer_format->spi_frf << SPI_SPI_FRF_Pos;

// 			ctrl0 &= ~SPI_TMOD_Msk;
// 			ctrl0 |= 2 << SPI_TMOD_Pos; //read

// 			// 1.2 配置 TRANS_TYPE
// 			// 00 - Instruction and Address will be sent in Standard SPI Mode.
// 			// 01 - Instruction will be sent in Standard SPI Mode and Address will be sent in the mode specified by CTRLR0.SPI_FRF.
// 			// 10 - 10 - Both Instruction and Address will be sent in the mode specified by SPI_FRF.
// 			// 11 - Reserved.
// 			spi_ctrl0 &= ~SPI_TRANS_TYPE_Msk;
// 			spi_ctrl0 |= enhanced_transfer_format->trans_type << SPI_TRANS_TYPE_Pos;

// 			// 1.3 ADDR_L
// 			spi_ctrl0 &= ~SPI_ADDR_L_Msk;
// 			spi_ctrl0 |= (addr_lenth/4) << SPI_ADDR_L_Pos;

// 			// 1.4 INST_L
// 			spi_ctrl0 &= ~SPI_INST_L_Msk;
// 			if(ins_lenth==16)
// 				spi_ctrl0 |= 3 << SPI_INST_L_Pos;
// 			else
// 				spi_ctrl0 |= (ins_lenth/4) << SPI_INST_L_Pos;

// 			// 1.5 WAIT_CYCLES
// 			spi_ctrl0 &= ~SPI_WAIT_CYCLES_Msk;
// 			spi_ctrl0 |= enhanced_transfer_format->wait_cycles << SPI_WAIT_CYCLES_Pos;
		
// 			// 1.6 DDR disable
// 			spi_ctrl0 &= ~SPI_SPI_DDR_EN_Msk;
// 			spi_ctrl0 &= ~SPI_INST_DDR_EN_Msk;
// 			spi_ctrl0 &= ~SPI_SPI_RXDS_EN_Msk;
// 		} 
// 		spix->CTRLR0 = ctrl0;
// 		spix->SPI_CTRLR0 = spi_ctrl0;

// 		//build-in dma配置
// 		spix->DMACR = 0;
// 		// spix->DMARDLR = 15; //16触发DMA请求
// 		spix->DMARDLR = 0; //1触发DMA请求
// 		spix->DMACR = 1;

// 		//配置接收数据量
// 		spix->CTRLR1 = enhanced_transfer_format->data_nums - 1;
// 	}
// 	spi_enable(spi_id, true);

// 	// dma配置
// #if (SPI_DMA_DRIVER_VERSION > 0)
// 	int ret = 0;
// 	DMA_Channel_t ch = DMA0_CH0_INDEX;
// 	dma_transfer_sig[(uint32_t)ch] = 0;
// 	ret = dma_init();
// 	assert(ret==0);
// 	dma_register_callback(ch, spi_dma_irq_handler, (void*)DMA0_CH0_INDEX);

// 	struct dma_slave_config config = {
// 		.direction      = DMA_DEV_TO_MEM,
// 		.src_addr       = (uint32_t)(&(spix->DR)),
// 		.dst_addr       = (uint32_t)(enhanced_transfer_format->data),
// 		.src_maxburst   = 0, //MSIZE_16
// 		.dst_maxburst   = 0, //MSIZE_16
// 		.data_len       = enhanced_transfer_format->data_nums,
// 		.src_width      = DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 		.dst_width      = DWAXIDMAC_TRANS_WIDTH_8, /* size=block_ts * width/8 */
// 		.device_fc      = false,
// 		.slave_id 		= handshake,
// 		.cyclic         = false,
// 		.period_len		= 0
// 	};

// 	while((ret = dma_config(ch, &config)) != 0 );
// 	dma_enable(ch);
// #else
// 	while((REG32(DMA_BASE + DMAC_CHENREG_0) & 1) == 1);
// 	REG32(DMA_BASE + CH1_SAR_0) = (uintptr_t)(&(spix->DR));
// 	REG32(DMA_BASE + CH1_DAR_0) = (uintptr_t)(enhanced_transfer_format->data);

// 	REG32(DMA_BASE + CH1_CTL_0) = AXI_MASTER_0 << 0 |
// 								AXI_MASTER_0 << 2 |
// 								SRC_ADDR_NOCHANGE << 4 |
// 								DST_ADDR_INCREMENT << 6 |
// 								SRC_TRANSFER_WIDTH_8 << 8 |
// 								DST_TRANSFER_WIDTH_8 << 11 |
// 								// SRC_MSIZE_16 << 14 |
// 								SRC_MSIZE_1 << 14 |
// 									//   DST_MSIZE_16 <<18  |
// 								NONPOSTED_LASTWRITE_EN << 30;

// 	REG32(DMA_BASE + CH1_CTL_32) = ARLEN_EN << (38 - 32) |
// 									0 << (39 - 32) | //source burst length
// 									AWLEN_EN << (47 - 32) |
// 									0 << (48 - 32) | //destination burst length
// 									SRC_STATUS_DISABLE << (56 - 32) |
// 									DST_STATUS_DISABLE << (57 - 32) |
// 									INTDISABLE_COMPLETOFBLKTRANS_SHADORLLI << (58 - 32) |
// 									NOTLAST_SHADORLLI << (62 - 32) |
// 									SHADORLLI_INVALID << (63 - 32);

// 	REG32(DMA_BASE + CH1_BLOCK_TS_0) = enhanced_transfer_format->data_nums - 1;

// 	REG32(DMA_BASE + CH1_CFG2_0) = SRC_CONTIGUOUS << 0 |
// 								DST_CONTIGUOUS << 2 |
// 								handshake      << 4;

// 	REG32(DMA_BASE + CH1_CFG2_32) = PER_TO_MEM_DMAC << (32 - 32) |
// 									SRC_HARDWARE_HS << (35 - 32) |
// 									/*25 << (39 - 32) | //src handshake*/
// 									CHANNEL_PRIORITY7 << (47 - 32) |
// 									CHANNEL_LOCK_DISABLE << (52 - 32) |
// 									0x4 << (55 - 32) | //Source Outstanding Request Limit == 3
// 									0x4 << (59 - 32);  //Destination Outstanding Request Limit == 3

// 	REG32(DMA_BASE + CH1_INTSTATUS_ENABLEREG_0) = 2; //Enable interrupt generation bit is valid
// 	REG32(DMA_BASE + CH1_INTSIGNAL_ENABLEREG_0) = 2; //Enable interrupt generation bit is valid
// 	REG32(DMA_BASE + DMAC_CFGREG_0) = 0x3;           //enable DMAC and its interrupt logic
// 	REG32(DMA_BASE + DMAC_CHENREG_0) = 0x101;        //EN channel1
// #endif

// 	//传输阶段
// 	uint8_t quotient = addr_lenth / 32; 
// 	uint8_t remainder = addr_lenth % 32;
// 	uint8_t cnt = (remainder) ? (quotient + 1) : (quotient);
// 	uint32_t* address = enhanced_transfer_format->address;
// 	// uint8_t *data = enhanced_transfer_format->data;
// 	// uint32_t data_nums = enhanced_transfer_format->data_nums;
// Instruction_phase:
// 	if(ins_lenth!=0)
// 		spix->DR = enhanced_transfer_format->instruction; //An instruction takes one FIFO location
// Address_phase:
// 	//address can take more than one FIFO locations
// 	for (uint8_t i = 0; i < cnt; i++)
// 			spix->DR = address[i];
	
// 	spi_select_slave(spi_id, 1);
// 	if(addr_lenth==0 && ins_lenth==0)
// 		spix->DR = 0xff; //write a dummy data to start transmit.
// Wait_cycles:
// 	//waiting...
// Data_phase:
// 	// for (uint8_t i = 0; i < data_nums; i++) {
// 	// 	while ((spix->SR & SPI_RFNE_Msk) == 0);
// 	// 	data[i] = spix->DR;
// 	// }
// #if (SPI_DMA_DRIVER_VERSION == 0)
// 	while ((REG32(DMA_BASE + CH1_INTSTATUS_0) & 0x00000002) == 0)
// 		;

// 	REG32(DMA_BASE + CH1_INTCLEARREG_0) = 0x00000002;
// #else
// 	while(!is_dma_channel_transfer_done(ch));
// 	free_dma_channel(ch);
// #endif
// 	spi_disable(spi_id, true);
// 	return;
// }

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


// void dw_spi_enhanced_test(void)
// {
// 	#define TEST_SPI_ID SPI0_ID
// 	spi_init_config_t spi_init_config = {
// 		.spi_id = TEST_SPI_ID,
// 		.as_master = true,
// 		.spi_mode = 3,
// 		.clock_div = 1330
// 	};
// 	dw_spi_init(&spi_init_config);

// 	uint16_t instruction = 0x1234;
// 	uint32_t addr[2] = {0x12345678, 0xa5a5a5a5};
// 	#define DATA_SIZE 1
// 	uint8_t data[DATA_SIZE];
// 	for(uint32_t i = 0;i<DATA_SIZE;i++)
// 		data[i] = i%256;
// 	enhanced_transfer_format_t enhanced_transfer_format = {
// 		.spi_frf = QUAD_SPI_FRF,
// 		.trans_type = 0,
// 		.instruction = instruction,
// 		.ins_lenth = 0,
// 		.address = addr,
// 		.addr_lenth = 0,
// 		.data = data,
// 		.data_nums = DATA_SIZE,
// 		.wait_cycles = 1
// 	};
// 	// dw_spi_transmit_only(TEST_SPI_ID, data, 32, false);
// 	dw_spi_enhanced_write(TEST_SPI_ID, &enhanced_transfer_format, false);
// 	dw_spi_enhanced_read(TEST_SPI_ID, &enhanced_transfer_format);
// 	dw_spi_deinit(TEST_SPI_ID, true);
// }