#include <stdint.h>

#define REG64(addr) (*(volatile uint64_t *)(uint32_t)(addr))
#define REG32(addr) (*(volatile uint32_t *)(uint32_t)(addr))
#define REG16(addr) (*(volatile uint16_t *)(uint32_t)(addr))
#define REG8(addr) (*(volatile uint8_t *)(uint32_t)(addr))

#define CORE0_ADDR (0x00a40000 + 0)
#define CORE1_ADDR (0x00a40000 + 8)
#define CORE2_ADDR (0x00a40000 + 16)
#define CORE3_ADDR (0x00a40000 + 24)

int main()
{
	REG64(CORE0_ADDR);
	
	while(1);
}