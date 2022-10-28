#include <stdint.h>
#include <stdbool.h>

#ifdef A55
	#include <ca55_chip_define.h>
#else
	#include <cm3_chip_define.h>
#endif


int pinmux(uint8_t pin_num, uint8_t iomux)
{
	if((pin_num>=0) && (pinmux<=30))
	{
		uint32_t pin_ctrl_base = SYSCTRL_BASE + 0x800UL + ((pin_num-0)*4);
		uint32_t tmp = REG32(pin_ctrl_base);
		tmp &= ~(7 << 4);
		tmp |= iomux << 4;
		tmp |= 1 << 0;
		REG32(pin_ctrl_base) = tmp;
	}
	else if((pin_num>=90) && (pinmux<=93))
	{
		uint32_t pin_ctrl_base = SYSCTRL_BASE + 0x87cUL + ((pin_num-90)*4);
		uint32_t tmp = REG32(pin_ctrl_base);
		tmp &= ~(7 << 4);
		tmp |= iomux << 4;
		tmp |= 1 << 0;
		REG32(pin_ctrl_base) = tmp;
	}
	else if((pin_num>=82) && (pinmux<=89))
	{
		uint32_t pin_ctrl_base = SYSCTRL_BASE + 0x88cUL + ((pin_num-82)*4);
		uint32_t tmp = REG32(pin_ctrl_base);
		tmp &= ~(7 << 4);
		tmp |= iomux << 4;
		tmp |= 1 << 0;
		REG32(pin_ctrl_base) = tmp;
	}
	else if((pin_num>=32) && (pinmux<=63))
	{
		uint32_t pin_ctrl_base = PERI0_REGS_BASE + 0x800UL + ((pin_num-32)*4);
		uint32_t tmp = REG32(pin_ctrl_base);
		tmp &= ~(7 << 4);
		tmp |= iomux << 4;
		tmp |= 1 << 0;
		REG32(pin_ctrl_base) = tmp;
	}
	else if((pin_num>=64) && (pinmux<=81))
	{
		uint32_t pin_ctrl_base = PERI1_REGS_BASE + 0x800UL + ((pin_num-64)*4);
		uint32_t tmp = REG32(pin_ctrl_base);
		tmp &= ~(7 << 4);
		tmp |= iomux << 4;
		tmp |= 1 << 0;
		REG32(pin_ctrl_base) = tmp;
	}
	else
	{

	}

	return 0;
}