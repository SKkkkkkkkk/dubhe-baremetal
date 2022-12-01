#include <stdio.h>
#include <string.h>
#include "xyzModem.h"
#include "regs_type.h"
#include "chip_mem_layout.h"

static inline bool tstc()
{
  return REG32(UART1_BASE+0x14)&1;
}

static inline int getcxmodem(void)
{
	if (tstc())
		return (getchar());
	return -1;
}

int uart_load(/*uintptr_t sram_addr*/)
{
	int err;
	int ret;
	int total_size = 0;
	connection_info_t info;
	char r_buf[256];
	const int block_size = sizeof(r_buf);

	info.mode = xyzModem_ymodem;
	ret = xyzModem_stream_open(&info, &err);
	if (ret == 0)
	{
		while ((ret = xyzModem_stream_read(r_buf, block_size, &err)) > 0) 
		{
			memcpy((void*)(DDR_BASE + total_size), r_buf, (size_t)ret);
			total_size += ret;
		}
	} 
	else 
	{
		printf("%s\n\r", xyzModem_error(err));
		total_size = 0;
	}

	xyzModem_stream_close(&err);
	xyzModem_stream_terminate(false, &getcxmodem);

	printf("\n\ruart_load finished.\n\r");
	void setpc(uintptr_t pc);
	setpc(DDR_BASE);
	return total_size;
}