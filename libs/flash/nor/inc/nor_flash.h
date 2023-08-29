#ifndef __NOR_FLASH_H__
#define __NOR_FLASH_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "dw_apb_ssi.h"
/**
 * @brief 目前支持的flash model。UNKNOWN_FLASH不支持32MB访问，不支持flash_init自动tuning。
 */
typedef enum {
	UNKNOWN_FLASH = 0,
	GD25LQ64  = 0xC86017,
	GD25LQ128 = 0xC86018,
	GD25LQ255 = 0xC86019,
	W25Q64JW = 0xEF6017,
	W25Q128JW = 0xEF6018,
	W25Q256JW = 0xEF6019,
	W25Q128FV = 0xEF4018, //STD - 0xEF4018, QPI - 0xEF6018
	P25Q40UJ = 0x856013
} flash_model_t;

/**
 * @brief flash初始化
 * @details UNKNOWN_FLASH不支持flash_init自动tuning，
 * 			其它flash model可以在配置完spi后自动读取flash id判断初始化是否成功，
 * 			tuning可以调节spi内部时序，但是需要知道flash id。
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @param[in] clk_div 分频系数，只能为偶数，范围为(2<=x<=65534)
 * @param[in] spi_mode SPI Mode: 0 1 2 3
 * @param[in] flash_model 选择flash型号
 * @remarks Note  this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
bool flash_init(spi_id_t spi_id, uint16_t clk_div, uint8_t spi_mode, flash_model_t flash_model);

/**
 * @brief flash deinit.
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_deinit(spi_id_t spi_id);

/**
 * @brief flash deinit.
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @param[out] flash_id flash id首地址
 * @param[in] id_size flash_id大小(in Bytes)
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_read_id(spi_id_t spi_id, uint8_t * const flash_id, uint8_t id_size);

/**
 * @brief flash sector erase.
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @param[in] addr 擦除的起始地址，4KB对齐，一次擦除4KB
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_sector_erase(spi_id_t spi_id, uint32_t addr);

/**
 * @brief flash block erase.
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @param[in] addr 擦除的起始地址，64KB对齐，一次擦除64KB
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_block_erase(spi_id_t spi_id, uint32_t addr);

/**
 * @brief flash chiop erase.
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_chip_erase(spi_id_t spi_id);

/**
 * @brief flash read(cpu搬运).
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @param[in] addr 读取地址
 * @param[out] buf 读取数据存储的地址
 * @param[in] size 读取数据大小(in Bytes)
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_read(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size);

/**
 * @brief flash write(cpu搬运).
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @param[in] addr 写的flash地址
 * @param[in] buf 待写入数据buf的首地址
 * @param[in] size 待写入数据buf的大小(in Bytes)
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_write(spi_id_t spi_id, uint32_t addr, uint8_t *buf, uint32_t size);

/**
 * @brief flash read(dma搬运).
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @param[in] addr 读取地址
 * @param[out] buf 读取数据存储的地址
 * @param[in] size 读取数据大小(in Bytes)
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_read_dma(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size);

/**
 * @brief flash write(dma搬运).
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @param[in] addr 写的flash地址
 * @param[in] buf 待写入数据buf的首地址
 * @param[in] size 待写入数据buf的大小(in Bytes)
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_write_dma(spi_id_t spi_id, uint32_t addr, uint8_t *buf, uint32_t size);


/**
 * @brief flash read 4线传输(cpu搬运，不推荐使用).
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @param[in] addr 读取地址
 * @param[out] buf 读取数据存储的地址
 * @param[in] size 读取数据大小(in Bytes)
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_read_quad(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size);

/**
 * @brief flash read 4线传输(dma搬运).
 * @param[in] spi_id 选择spi。BOOTSPI，SPI0，SPI1，SPI2
 * @param[in] addr 读取地址
 * @param[out] buf 读取数据存储的地址
 * @param[in] size 读取数据大小(in Bytes)
 * @remarks Note this function is not thread-safe!
 * 			If thread safety is required, the user must handle locking and unlockingthe region manually.
 */
void flash_read_quad_dma(spi_id_t spi_id, uint32_t addr, uint8_t* const buf, uint32_t size);

#ifdef __cplusplus
    }
#endif
#endif