#include "uart_update.h"
#include "xyzModem.h"
#include "nor_flash.h"
#include "regs_type.h"
#include "dw_apb_uart.h"
#include "arch_helpers.h"


#define ERROR printf

#define FIP_FLASH_OFFSITE 		(512*1024)

static inline bool tstc()
{
	return uart_tstc(UART1_BASE);
}

static inline int getcxmodem(void)
{
	if (tstc())
		return (getchar());
	return -1;
}

int uart_update(enum output_device o_device)
{
	int addr;
	int err;
	int ret;
	connection_info_t info;
	char r_buf[256];
	fip_toc_header_t* toc_header;
	(void)o_device;
	int block_size = sizeof(r_buf);

	addr = 0;
	info.mode = xyzModem_ymodem;
	ret = xyzModem_stream_open(&info, &err);
	if (ret == 0)
	{
		while ((ret = xyzModem_stream_read(r_buf, block_size, &err)) > 0) 
		{
			if(addr == 0) /* 第一包数据，先检查头 */
			{
				toc_header = (fip_toc_header_t*)&r_buf;
				if (!is_valid_header(toc_header))
				{
					ERROR("Firmware Image Package header check failed.\n");
					addr = -1;
					break;
				}
				else
				{
					if(!flash_init(BOOTSPI_ID, 2, 3, UNKNOWN_FLASH))
					{
						addr = -1;
						break;
					}
				}
			}
			assert((FIP_FLASH_OFFSITE&0xfff)==0); // FIP_FLASH_OFFSITEE必须4KB对齐
			if((addr&0xfff)==0) // 4KB对齐擦除
			{
				flash_sector_erase(BOOTSPI_ID, addr+FIP_FLASH_OFFSITE);
			}
			flash_write(BOOTSPI_ID, addr+FIP_FLASH_OFFSITE, (uint8_t*)r_buf, ret);
			addr += ret;
		}
	} 
	else 
	{
		ERROR("%s\n\r", xyzModem_error(err));
	}

	xyzModem_stream_close(&err);
	xyzModem_stream_terminate(false, &getcxmodem);
	printf("\n\ruart update finished.\n\r");
	return addr;
}