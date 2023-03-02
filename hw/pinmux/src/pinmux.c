#include <stdint.h>
#include <stdbool.h>

#include "chip_mem_layout.h"

void pinmux(uint8_t pin_num, uint8_t iomux)
{
	if(/*(pin_num>=0) always true &&*/ (pin_num<=30))
	{
		uint32_t pin_ctrl_base = SYSCTRL_BASE + 0x800UL + ((pin_num-0)*4);
		uint32_t tmp = REG32(pin_ctrl_base);
		tmp &= ~(7 << 4);
		tmp |= iomux << 4;
		tmp |= 1 << 0;
		REG32(pin_ctrl_base) = tmp;
	}
	else if((pin_num>=90) && (pin_num<=93))
	{
		uint32_t pin_ctrl_base = SYSCTRL_BASE + 0x87cUL + ((pin_num-90)*4);
		uint32_t tmp = REG32(pin_ctrl_base);
		tmp &= ~(7 << 4);
		tmp |= iomux << 4;
		tmp |= 1 << 0;
		REG32(pin_ctrl_base) = tmp;
	}
	else if((pin_num>=64) && (pin_num<=89))  // From M2V5R1, PER1 Pinmux regs were transfered to SYSCTRL.
	{
		uint32_t pin_ctrl_base = SYSCTRL_BASE + 0x88cUL + ((pin_num-82)*4);
		uint32_t tmp = REG32(pin_ctrl_base);
		tmp &= ~(7 << 4);
		tmp |= iomux << 4;
		tmp |= 1 << 0;
		REG32(pin_ctrl_base) = tmp;
	}
	else if((pin_num>=32) && (pin_num<=63))
	{
		uint32_t pin_ctrl_base = PERI0_REGS_BASE + 0x800UL + ((pin_num-32)*4);
		uint32_t tmp = REG32(pin_ctrl_base);
		tmp &= ~(7 << 4);
		tmp |= iomux << 4;
		tmp |= 1 << 0;
		REG32(pin_ctrl_base) = tmp;
	}
	// else if((pin_num>=64) && (pin_num<=81)) // From M2V5R1, PER1 Pinmux regs were transfered to SYSCTRL.
	// {
	// 	uint32_t pin_ctrl_base = PERI1_REGS_BASE + 0x800UL + ((pin_num-64)*4);
	// 	uint32_t tmp = REG32(pin_ctrl_base);
	// 	tmp &= ~(7 << 4);
	// 	tmp |= iomux << 4;
	// 	tmp |= 1 << 0;
	// 	REG32(pin_ctrl_base) = tmp;
	// }
	else
	{

	}

	return;
}