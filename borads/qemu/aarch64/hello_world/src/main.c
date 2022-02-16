#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "main.h"
#include "seehi_print.h"

#define REG64(addr) (*(volatile uint64_t *)(uint32_t)(addr))
#define REG32(addr) (*(volatile uint32_t *)(uint32_t)(addr))
#define REG16(addr) (*(volatile uint16_t *)(uint32_t)(addr))
#define REG8(addr) (*(volatile uint8_t *)(uint32_t)(addr))

#define CORE0_ADDR (0x40000000 + 0)
#define CORE1_ADDR (0x40000000 + 8)
#define CORE2_ADDR (0x40000000 + 16)
#define CORE3_ADDR (0x40000000 + 24)

typedef enum core_statue
{
	sleep = 0,
	running = 1,
	unknow = 2
} core_statue_t;
typedef struct core_entry_list
{
	uint32_t entry_addr;
	uint32_t keys;
	core_statue_t state;
} core_entry_list_t;

#define CORE_NUMS 4
core_entry_list_t core_entry_list[CORE_NUMS] = {
	[0]={.entry_addr=0,.keys=0,.state=unknow},
	[1]={.entry_addr=0,.keys=0,.state=unknow},
	[2]={.entry_addr=0,.keys=0,.state=unknow},
	[3]={.entry_addr=0,.keys=0,.state=unknow},	
};




// volatile uint8_t bss_buf[32*1024] = {1,1};
volatile uint8_t bss_buf[32*1024];
int core0_c_entry()
{
	bss_buf[0] = 1;
	void printf_init();
	printf_init();
	core_entry_list[get_core_id()].state = running;
	printf("hello world in core.%lu\n", get_core_id());
	seehi_printf("hello world in core.%d\n", get_core_id());
	while(1);
}

int core1_c_entry()
{
	printf("hello world in core.%lu\n", get_core_id());
	while(1);
}

int core2_c_entry()
{
	printf("hello world in core.%lu\n", get_core_id());
	while(1);
}

int core3_c_entry()
{
	printf("hello world in core.%lu\n", get_core_id());
	while(1);
}




void printf_init()
{
	uart_config config = {
		.data_bits = 8,
		.stop_bits = 1,
		.parity = false,
		.baudrate = 9600
	};
	uart_configure(&config);
}