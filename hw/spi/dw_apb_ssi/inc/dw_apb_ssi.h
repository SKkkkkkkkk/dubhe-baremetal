#ifndef __DW_SPI_H__
#define __DW_SPI_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum { T_R = 0, T_ONLY = 1, R_ONLY = 2, EEREAD = 3 } tmod_t;

typedef enum { STD_SPI_FRF = 0, DUAL_SPI_FRF = 1, QUAD_SPI_FRF = 2/*, OCTAL_SPI_FRF = 3*/ } spi_frf_t;

typedef enum { SPI0_ID = 0, SPI1_ID = 1, SPI2_ID = 2, BOOTSPI_ID = 3 } spi_id_t;

typedef struct _enhanced_transfer_format {
	spi_frf_t spi_frf;
	uint8_t trans_type;
	uint16_t instruction;
	uint8_t ins_lenth;
	uint32_t *address;
	uint8_t addr_lenth;
	uint8_t *data;
	uint32_t data_nums;
	uint8_t wait_cycles;
} enhanced_transfer_format_t;

struct _spi_init_config {
	spi_id_t spi_id;
	bool as_master; //目前只支持master
	uint8_t spi_mode; //0,1,2,3
	uint16_t clock_div;
};
typedef struct _spi_init_config spi_init_config_t;

/**
 * @brief spi init
 * @param spi_init_config 初始化结构体指针
 * @return void
 */
void dw_spi_init(spi_init_config_t const * const spi_init_config);

/**
 * @brief spi deinit
 * @param spi_id 选择spi
 * @param as_master true - master, false - slave.
 * @return void
 */
void dw_spi_deinit(spi_id_t spi_id, bool as_master);

/**
 * @brief spi transmit & receive
 * @param spi_id 选择spi
 * @param t_buf 发送buf
 * @param r_buf 接收buf
 * @param tr_size 传输数据的大小
 * @param safe_mode 是否开启安全传输模式。true - 开启，false - 关闭
 * @return void
 */
void dw_spi_transmit_and_receive(spi_id_t spi_id, void *t_buf, void *r_buf, uint32_t tr_size, bool safe_mode);

/**
 * @brief spi eeprom模式传输
 * @param spi_id 选择spi
 * @param t_buf 发送buf
 * @param t_size 发送数据大小，必须<=32
 * @param r_buf 接收buf
 * @param r_size 接收数据大小
 * @return void
 */
void dw_spi_eeprom_read(spi_id_t spi_id, void *t_buf, uint8_t t_size, void *r_buf, uint16_t r_size);

/**
 * @brief spi receive模式传输
 * @param spi_id 选择spi
 * @param r_buf 接收buf
 * @param r_size 接收数据大小
 * @return void
 */
void dw_spi_receive_only(spi_id_t spi_id, void* r_buf, uint16_t r_size);

/**
 * @brief spi transmit & receive
 * @param spi_id 选择spi
 * @param t_buf 发送buf
 * @param t_size 发送数据的大小
 * @param safe_mode 是否开启安全传输模式。true - 开启，false - 关闭
 * @return void
 */
void dw_spi_transmit_only(spi_id_t spi_id, void* t_buf, uint32_t t_size, bool safe_mode);


/* dma */
void dw_spi_eeprom_read_dma(spi_id_t spi_id, void *t_buf, uint8_t t_size, void *r_buf, uint16_t r_size);

void dw_spi_transmit_only_dma(spi_id_t spi_id, void* t_buf, uint32_t t_size);

/* enhanced spi mode */
void dw_spi_enhanced_read(spi_id_t spi_id, enhanced_transfer_format_t *enhanced_transfer_format);

void dw_spi_enhanced_write(spi_id_t spi_id, enhanced_transfer_format_t *enhanced_transfer_format, bool safe_mode);

void dw_spi_enhanced_read_dma(spi_id_t spi_id, enhanced_transfer_format_t *enhanced_transfer_format);

/*
	关于tmod_t的4种传输模式，详见dw_apb_ssi_databok.pdf
*/

/*
	关于safe_mode。
	safe_mode设置为true时，保证了传输数据的连续性(size个数据连续传输，且cs持续为低)，但是每次传输的数据量最大为spi fifo深度(32B)。
	safe_mode设置为false时，不保证传输数据的连续性(size个数据不一定连续传输，如果不采用gpio作为cs，发生不连续传输时cs会拉高)，但是每次传输的大小不定。
*/

/*
	关于是否使用gpio作为cs，可以通过 SPI_CS_USE_GPIO 宏选择。
	如果不使用gpio作为cs(cs由spi内部驱动):
		dw_spi特性会导致，txfifo为空时，cs会拉高，发生不连续数据传输时(即发送过程中txfifo为空)不会导致cs拉高。
	使用gpio作为cs:
		cs不由spi内部驱动，完全由软件写gpio控制，发生不连续数据传输时不会导致cs拉高。
*/

#ifdef __cplusplus
    }
#endif

#endif