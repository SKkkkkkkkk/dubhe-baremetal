#include <ymodem_update.h>
// #include <platform.h>
#include <xyzModem.h>
#include <nor_flash.h>
// #include <mmio.h>
#include <regs_type.h>
#include <dw_apb_uart.h>
#include <arch_helpers.h>


#define ERROR printf

#define PLAT_FIP_BASE 		(256*1024)

static inline bool tstc()
{
	return uart_tstc(UART1_BASE);
}

static int getcxmodem(void)
{
	if (tstc())
		return (getchar());
	return -1;
}

static inline void setpc(uint64_t pc)
{
	assert((pc&3)==0);
	dmbsy();
	dsbsy();
	isb();
	asm volatile ("br %0"::"r"(pc):"memory");
}


int ymodem_update(enum output_device o_device)
{
	int addr;
	int err;
	int ret;
	connection_info_t info;
	char r_buf[256];
	fip_toc_header_t* toc_header;
	// fip_toc_entry_t* toc_entry;
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
			// #if (FIP_BACKEND_DRIVER == FIP_BACKEND_MEMMAP)
				// memcpy((void*)(uintptr_t)PLAT_FIP_BASE, r_buf, ret);
			// #elif (FIP_BACKEND_DRIVER == FIP_BACKEND_FLASH)
				assert((PLAT_FIP_BASE&0xfff)==0); // FIP_BASE必须4KB对齐
				if((addr&0xfff)==0) // 4KB对齐擦除
				{
					flash_sector_erase(BOOTSPI_ID, addr+PLAT_FIP_BASE);
				}
				flash_write(BOOTSPI_ID, addr+PLAT_FIP_BASE, (uint8_t*)r_buf, ret);
			// #else
				// #error "Unsupport FIP_BACKEND_DRIVER."
			// #endif
			addr += ret;
		}
	} 
	else 
	{
		ERROR("%s\n\r", xyzModem_error(err));
	}

	xyzModem_stream_close(&err);
	xyzModem_stream_terminate(false, &getcxmodem);

	// asm volatile("b ."); //???
	ret = putchar('B');
	printf("\n\rymodem update finished.\n\r");
	return addr;
}