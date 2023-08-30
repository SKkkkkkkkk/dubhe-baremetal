#ifndef QEMU
#include <stdio.h>
#include "chip_mem_layout.h"

#define GET_LOW_2BYTE(x) ((x) & 0x0000ffff)
#define GET_HIGH_2BYTE(x) (((x) >> 16) & 0x0000ffff)
#define GET_BIT(x, n) (((x) >> (n)) & 0x00000001)

void print_sysinfo()
{
	uint32_t value;

	value = REG32(SYS_BASE + 0xD00);
	printf("chip name               = 0x%x\n", GET_LOW_2BYTE(value));
	printf("is asic                 = 0x%x\n", GET_BIT(value, 16));
	printf("tape out num            = 0x%x\n", (value >> 24) & 0xff);

	value = REG32(SYS_BASE + 0xD04);
	printf("milestone               = 0x%x\n", (value >> 0) & 0x000000ff);
	printf("version                 = 0x%x\n", (value >> 8) & 0x000000ff);
	printf("reversion               = 0x%x\n", (value >> 16) & 0x000000ff);

	value = REG32(SYS_BASE + 0xD10);
	printf("ap sys version          = 0x%x\n", GET_LOW_2BYTE(value));
	printf("lp sys version          = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD14);
	printf("peri0 sys version       = 0x%x\n", GET_LOW_2BYTE(value));
	printf("peri1 sys version       = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD18);
	printf("ddr sys version         = 0x%x\n", GET_LOW_2BYTE(value));
	printf("npu sys version         = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD1c);
	printf("vpu sys version         = 0x%x\n", GET_LOW_2BYTE(value));
#if defined(HW_VERSION_MPW)
	printf("gpu sys version         = 0x%x\n", GET_HIGH_2BYTE(value));
#endif

	value = REG32(SYS_BASE + 0xD20);
#if defined(HW_VERSION_MPW)
	printf("image sys version       = 0x%x\n", GET_LOW_2BYTE(value));
#else
	printf("image in sys version    = 0x%x\n", GET_LOW_2BYTE(value));
	printf("image out sys version   = 0x%x\n", GET_HIGH_2BYTE(value));
#endif

	value = REG32(SYS_BASE + 0xD30);
	printf("adc version             = 0x%x\n", GET_LOW_2BYTE(value));
	printf("apb sync version        = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD34);
	printf("boot rom version        = 0x%x\n", GET_LOW_2BYTE(value));
	printf("boot spi version        = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD38);
#if defined(HW_VERSION_MPW)
	printf("clci version            = 0x%x\n", GET_LOW_2BYTE(value));
	printf("cm3 icache version      = 0x%x\n", GET_HIGH_2BYTE(value));
#else
	printf("axi sram version        = 0x%x\n", GET_LOW_2BYTE(value));
#endif

	value = REG32(SYS_BASE + 0xD3C);
	printf("cortex m3 version       = 0x%x\n", GET_LOW_2BYTE(value));
	printf("cpu version             = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD40);
	printf("daplite version         = 0x%x\n", GET_LOW_2BYTE(value));
	printf("dc version              = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD44);
	printf("ddrctrl version         = 0x%x\n", GET_LOW_2BYTE(value));
	printf("dmac version            = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD48);
	printf("dwp version             = 0x%x\n", GET_LOW_2BYTE(value));
	printf("efuse version           = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD4C);
	printf("efuse ctrl version      = 0x%x\n", GET_LOW_2BYTE(value));
	printf("emmc version            = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD50);
	printf("emmc51 version          = 0x%x\n", GET_LOW_2BYTE(value));
	printf("generic timer version   = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD54);
	printf("gic version             = 0x%x\n", GET_LOW_2BYTE(value));
	printf("gmac version            = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD58);
	printf("gpio version            = 0x%x\n", GET_LOW_2BYTE(value));
	printf("gpu version             = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD5C);
	printf("i2c mst version         = 0x%x\n", GET_LOW_2BYTE(value));
	printf("i2c wo version          = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD60);
	printf("i2s version             = 0x%x\n", GET_LOW_2BYTE(value));
	printf("idma version            = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD64);
	printf("isp version             = 0x%x\n", GET_LOW_2BYTE(value));
	printf("lp dmac version         = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD68);
	printf("m3 boot rom version     = 0x%x\n", GET_LOW_2BYTE(value));
	printf("mailbox version         = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD6C);
	printf("mipi rx version         = 0x%x\n", GET_LOW_2BYTE(value));
	printf("mipi tx version         = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD70);
	printf("mst spi version         = 0x%x\n", GET_LOW_2BYTE(value));
	printf("npu version             = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD74);
	printf("pdm version             = 0x%x\n", GET_LOW_2BYTE(value));
	printf("pka version             = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD78);
	printf("pmon version            = 0x%x\n", GET_LOW_2BYTE(value));
	printf("pmu version             = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD7C);
	printf("ppu version             = 0x%x\n", GET_LOW_2BYTE(value));
	printf("process monitor version = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD80);
	printf("pwm version             = 0x%x\n", GET_LOW_2BYTE(value));
#if defined(HW_VERSION_MPW)
	printf("rot raw version         = 0x%x\n", GET_HIGH_2BYTE(value));
#else
	printf("cve rotate version      = 0x%x\n", GET_HIGH_2BYTE(value));
#endif

	value = REG32(SYS_BASE + 0xD84);
#if defined(HW_VERSION_MPW)
	printf("rot yuv version         = 0x%x\n", GET_LOW_2BYTE(value));
#else
	printf("cve scaler version      = 0x%x\n", GET_LOW_2BYTE(value));
#endif
	printf("rtc version             = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD88);
	printf("slv spi version         = 0x%x\n", GET_LOW_2BYTE(value));
	printf("spacc version           = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD8C);
	printf("sys ram version         = 0x%x\n", GET_LOW_2BYTE(value));
	printf("sysctrl version         = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD90);
	printf("timerx2 version         = 0x%x\n", GET_LOW_2BYTE(value));
	printf("timerx6 version         = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD94);
	printf("uart version            = 0x%x\n", GET_LOW_2BYTE(value));
	printf("usb version             = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD98);
	printf("vad version             = 0x%x\n", GET_LOW_2BYTE(value));
	printf("vpu version             = 0x%x\n", GET_HIGH_2BYTE(value));

	value = REG32(SYS_BASE + 0xD9C);
	printf("wdt version             = 0x%x\n", GET_LOW_2BYTE(value));

	value = REG32(SYS_BASE + 0xE00);
	printf("ap sys exist            = 0x%x\n", GET_BIT(value, 0));
	printf("cpu exist               = 0x%x\n", GET_BIT(value, 1));
	printf("daplite exist           = 0x%x\n", GET_BIT(value, 2));
	printf("dmac exist              = 0x%x\n", GET_BIT(value, 3));
	printf("gic exist               = 0x%x\n", GET_BIT(value, 4));
	printf("timerx6 exist           = 0x%x\n", GET_BIT(value, 5));
	printf("wdt1 exist              = 0x%x\n", GET_BIT(value, 6));
	printf("boot rom1 exist         = 0x%x\n", GET_BIT(value, 7));

	value = REG32(SYS_BASE + 0xE04);
	printf("lp sys exist            = 0x%x\n", GET_BIT(value, 0));
	printf("cortex m3 exist         = 0x%x\n", GET_BIT(value, 1));
	printf("lp dmac exist           = 0x%x\n", GET_BIT(value, 2));
	printf("m3 boot rom exist       = 0x%x\n", GET_BIT(value, 3));
	printf("mailbox exist           = 0x%x\n", GET_BIT(value, 4));
	printf("vad exist               = 0x%x\n", GET_BIT(value, 5));
	printf("pdm0 exist              = 0x%x\n", GET_BIT(value, 6));
	printf("pdm1 exist              = 0x%x\n", GET_BIT(value, 7));
	printf("pdm2 exist              = 0x%x\n", GET_BIT(value, 8));
	printf("pdm3 exist              = 0x%x\n", GET_BIT(value, 9));
	printf("i2s0 exist              = 0x%x\n", GET_BIT(value, 10));
	printf("timerx2 exist           = 0x%x\n", GET_BIT(value, 11));
	printf("i2c0 exist              = 0x%x\n", GET_BIT(value, 12));
	printf("uart0 exist             = 0x%x\n", GET_BIT(value, 13));
	printf("wdt0 exist              = 0x%x\n", GET_BIT(value, 14));

	value = REG32(SYS_BASE + 0xE08);
	printf("peri0 sys exist         = 0x%x\n", GET_BIT(value, 0));
	printf("emm0 exist              = 0x%x\n", GET_BIT(value, 1));
	printf("emmc1 exist             = 0x%x\n", GET_BIT(value, 2));
	printf("sdio2 exist             = 0x%x\n", GET_BIT(value, 3));
	printf("gpio1 exist             = 0x%x\n", GET_BIT(value, 4));
	printf("gpio2 exist             = 0x%x\n", GET_BIT(value, 5));
	printf("i2s1 exist              = 0x%x\n", GET_BIT(value, 6));
	printf("i2s2 exist              = 0x%x\n", GET_BIT(value, 7));
	printf("bootspi exist           = 0x%x\n", GET_BIT(value, 8));
	printf("uart1 exist             = 0x%x\n", GET_BIT(value, 9));
	printf("uart2 exist             = 0x%x\n", GET_BIT(value, 10));
	printf("i2c1 exist              = 0x%x\n", GET_BIT(value, 11));
	printf("i2c2 exist              = 0x%x\n", GET_BIT(value, 12));
	printf("i2c3 exist              = 0x%x\n", GET_BIT(value, 13));
	printf("i2c4 exist              = 0x%x\n", GET_BIT(value, 14));
	printf("pwm0 exist              = 0x%x\n", GET_BIT(value, 15));
	printf("pdm4 exist              = 0x%x\n", GET_BIT(value, 16));
	printf("pdm5 exist              = 0x%x\n", GET_BIT(value, 17));

	value = REG32(SYS_BASE + 0xE0C);
	printf("peri1 sys exist         = 0x%x\n", GET_BIT(value, 0));
	printf("gmac exist              = 0x%x\n", GET_BIT(value, 1));
	printf("usb exist               = 0x%x\n", GET_BIT(value, 2));
	printf("spacc exist             = 0x%x\n", GET_BIT(value, 3));
	printf("pka exist               = 0x%x\n", GET_BIT(value, 4));
	printf("mst spi0 exist          = 0x%x\n", GET_BIT(value, 5));
	printf("mst spi1 exist          = 0x%x\n", GET_BIT(value, 6));
	printf("mst spi2 exist          = 0x%x\n", GET_BIT(value, 7));
	printf("slv spi exist           = 0x%x\n", GET_BIT(value, 8));
	printf("pwm1 exist              = 0x%x\n", GET_BIT(value, 9));
	printf("pwm2 exist              = 0x%x\n", GET_BIT(value, 10));
	printf("pwm3 exist              = 0x%x\n", GET_BIT(value, 11));
	printf("pwm4 exist              = 0x%x\n", GET_BIT(value, 12));
	printf("pwm5 exist              = 0x%x\n", GET_BIT(value, 13));
	printf("pwm6 exist              = 0x%x\n", GET_BIT(value, 14));
	printf("pwm7 exist              = 0x%x\n", GET_BIT(value, 15));
	printf("efuse ctrl exist        = 0x%x\n", GET_BIT(value, 16));
	printf("uart3 exist             = 0x%x\n", GET_BIT(value, 17));
	printf("uart4 exist             = 0x%x\n", GET_BIT(value, 18));
	printf("uart5 exist             = 0x%x\n", GET_BIT(value, 19));
	printf("uart6 exist             = 0x%x\n", GET_BIT(value, 20));
	printf("uart7 exist             = 0x%x\n", GET_BIT(value, 21));
	printf("i2c5 exist              = 0x%x\n", GET_BIT(value, 22));
	printf("i2c6 exist              = 0x%x\n", GET_BIT(value, 23));
	printf("i2s3 exist              = 0x%x\n", GET_BIT(value, 24));

	value = REG32(SYS_BASE + 0xE10);
	printf("image sys exist         = 0x%x\n", GET_BIT(value, 0));
	printf("isp exist               = 0x%x\n", GET_BIT(value, 1));
	printf("dwp exist               = 0x%x\n", GET_BIT(value, 2));
	printf("lcdc exist              = 0x%x\n", GET_BIT(value, 3));
	printf("idma exist              = 0x%x\n", GET_BIT(value, 4));
#if defined(HW_VERSION_MPW)
	printf("rot raw exist           = 0x%x\n", GET_BIT(value, 5));
	printf("rot yuv exist           = 0x%x\n", GET_BIT(value, 6));
#else
	printf("cve rotate exist        = 0x%x\n", GET_BIT(value, 5));
	printf("cve scaler exist        = 0x%x\n", GET_BIT(value, 6));
#endif
	printf("mipi rx0 exist          = 0x%x\n", GET_BIT(value, 7));
	printf("mipi rx1 exist          = 0x%x\n", GET_BIT(value, 8));
	printf("mipi tx exist           = 0x%x\n", GET_BIT(value, 9));

	value = REG32(SYS_BASE + 0xE14);
	printf("npu0 exist              = 0x%x\n", GET_BIT(value, 0));
	printf("npu1 exist              = 0x%x\n", GET_BIT(value, 1));

	value = REG32(SYS_BASE + 0xE18);
	printf("ddr0 sys exist          = 0x%x\n", GET_BIT(value, 0));
#if defined(HW_VERSION_MPW)
	printf("ddr1 sys exist          = 0x%x\n", GET_BIT(value, 1));
#endif

	value = REG32(SYS_BASE + 0xE1C);
	printf("vpu sys exist           = 0x%x\n", GET_BIT(value, 0));

	value = REG32(SYS_BASE + 0xE20);
	printf("gpu sys exist           = 0x%x\n", GET_BIT(value, 0));
}

int main()
{
	printf("\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
	printf("Build Info: "BUILD_TIMESTAMP"\n\r");
	printf("Git Hash: "GIT_HASH"\n\r");
	printf(" *************SOC_INFO***************** \n");
	print_sysinfo();
	printf(" *************SOC_INFO***************** \n");
	return 0;
}

#else
#include "main_qemu.c"
#endif