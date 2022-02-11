#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "main.h"

#define REG64(addr) (*(volatile uint64_t *)(uint32_t)(addr))
#define REG32(addr) (*(volatile uint32_t *)(uint32_t)(addr))
#define REG16(addr) (*(volatile uint16_t *)(uint32_t)(addr))
#define REG8(addr) (*(volatile uint8_t *)(uint32_t)(addr))

#define CORE0_ADDR (0x40000000 + 0)
#define CORE1_ADDR (0x40000000 + 8)
#define CORE2_ADDR (0x40000000 + 16)
#define CORE3_ADDR (0x40000000 + 24)

int core0_c_entry()
{
	void printf_init();
	printf_init();
	printf("hello world in core.%lu\n", get_core_id());
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