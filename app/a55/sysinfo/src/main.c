#ifndef QEMU
#include <stdio.h>
#include "chip_mem_layout.h"

#define writel(d, a) *(volatile uint32_t *)(a) = (d)
#define readl(a) (*(volatile uint32_t *)(a))

#define GET_LOW_2BYTE(x)                                    ({ \
    uint32_t val = x; \
    val = val & 0x0000ffff; \
    (val);\
})
#define GET_HIGH_2BYTE(x)                                    ({ \
    uint32_t val = x; \
    val = (val >> 16) & 0x0000ffff; \
    (val);\
})

void print_sysinfo()
{
	uint32_t value;
	uint32_t temp;

	value = readl(SYS_BASE + 0xD00);
	temp = GET_LOW_2BYTE(value);
	printf("chip name               = 0x%x\n", temp);
	temp = (value >> 16) & 0x00000001;
	printf("is asic                 = 0x%x\n", temp);
	temp = (value >> 24) & 0x000000ff;
	printf("tape out num            = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD04);
	temp = (value >> 0) & 0x000000ff;
	printf("milestone               = 0x%x\n", temp);
	temp = (value >> 8) & 0x000000ff;
	printf("version                 = 0x%x\n", temp);
	temp = (value >> 16) & 0x000000ff;
    printf("reversion               = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD10);
	temp = GET_LOW_2BYTE(value);
	printf("ap sys version          = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("lp sys version          = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD14);
	temp = GET_LOW_2BYTE(value);
	printf("peri0 sys version       = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("peri1 sys version       = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD18);
	temp = GET_LOW_2BYTE(value);
	printf("ddr sys version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("npu sys version         = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD1c);
	temp = GET_LOW_2BYTE(value);
	printf("vpu sys version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("gpu sys version         = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD20);
	temp = GET_LOW_2BYTE(value);
	printf("image sys version       = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD30);
	temp = GET_LOW_2BYTE(value);
	printf("adc version             = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("apb sync version        = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD34);
	temp = GET_LOW_2BYTE(value);
	printf("boot rom version        = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("boot spi version        = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD38);
	temp = GET_LOW_2BYTE(value);
	printf("clci version            = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("cm3 icache version      = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD3C);
	temp = GET_LOW_2BYTE(value);
	printf("cortex m3 version       = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("cpu version             = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD40);
	temp = GET_LOW_2BYTE(value);
	printf("daplite version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("dc version              = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD44);
	temp = GET_LOW_2BYTE(value);
	printf("ddrctrl version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("dmac version            = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD48);
	temp = GET_LOW_2BYTE(value);
	printf("dwp version             = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("efuse version           = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD4C);
	temp = GET_LOW_2BYTE(value);
	printf("efuse ctrl version      = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("emmc version            = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD50);
	temp = GET_LOW_2BYTE(value);
	printf("emmc51 version          = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("generic timer version   = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD54);
	temp = GET_LOW_2BYTE(value);
	printf("gic version             = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("gmac version            = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD58);
	temp = GET_LOW_2BYTE(value);
	printf("gpio version            = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("gpu version             = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD5C);
	temp = GET_LOW_2BYTE(value);
	printf("i2c mst version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("i2c wo version          = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD60);
	temp = GET_LOW_2BYTE(value);
	printf("i2s version             = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("idma version            = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD64);
	temp = GET_LOW_2BYTE(value);
	printf("isp version             = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("lp dmac version         = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD68);
	temp = GET_LOW_2BYTE(value);
	printf("m3 boot rom version     = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("mailbox version         = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD6C);
	temp = GET_LOW_2BYTE(value);
	printf("mipi rx version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("mipi tx version         = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD70);
	temp = GET_LOW_2BYTE(value);
	printf("mst spi version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("npu version             = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD74);
	temp = GET_LOW_2BYTE(value);
	printf("pdm version             = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("pka version             = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD78);
	temp = GET_LOW_2BYTE(value);
	printf("pmon version            = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("pmu version             = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD7C);
	temp = GET_LOW_2BYTE(value);
	printf("ppu version             = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("process monitor version = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD80);
	temp = GET_LOW_2BYTE(value);
	printf("pwm version             = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("rot raw version         = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD84);
	temp = GET_LOW_2BYTE(value);
	printf("rot yuv version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("rtc version             = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD88);
	temp = GET_LOW_2BYTE(value);
	printf("slv spi version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("spacc version           = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD8C);
	temp = GET_LOW_2BYTE(value);
	printf("sys ram version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("sysctrl version         = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD90);
	temp = GET_LOW_2BYTE(value);
	printf("timerx2 version         = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("timerx6 version         = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD94);
	temp = GET_LOW_2BYTE(value);
	printf("uart version            = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("usb version             = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD98);
	temp = GET_LOW_2BYTE(value);
	printf("vad version             = 0x%x\n", temp);
	temp = GET_HIGH_2BYTE(value);
	printf("vpu version             = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xD9C);
	temp = GET_LOW_2BYTE(value);
	printf("wdt version             = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xE00);
	temp = (value >> 0) & 0x00000001;
	printf("ap sys exist            = 0x%x\n", temp);
	temp = (value >> 1) & 0x00000001;
	printf("cpu exist               = 0x%x\n", temp);
	temp = (value >> 2) & 0x00000001;
	printf("daplite exist           = 0x%x\n", temp);
	temp = (value >> 3) & 0x00000001;
	printf("dmac exist              = 0x%x\n", temp);
	temp = (value >> 4) & 0x00000001;
	printf("gic exist               = 0x%x\n", temp);
	temp = (value >> 5) & 0x00000001;
	printf("timerx6 exist           = 0x%x\n", temp);
	temp = (value >> 6) & 0x00000001;
	printf("wdt1 exist              = 0x%x\n", temp);
	temp = (value >> 7) & 0x00000001;
	printf("boot rom1 exist         = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xE04);
	temp = (value >> 0) & 0x00000001;
	printf("lp sys exist            = 0x%x\n", temp);
	temp = (value >> 1) & 0x00000001;
	printf("cortex m3 exist         = 0x%x\n", temp);
	temp = (value >> 2) & 0x00000001;
	printf("lp dmac exist           = 0x%x\n", temp);
	temp = (value >> 3) & 0x00000001;
	printf("m3 boot rom exist       = 0x%x\n", temp);
	temp = (value >> 4) & 0x00000001;
	printf("mailbox exist           = 0x%x\n", temp);
	temp = (value >> 5) & 0x00000001;
	printf("vad exist               = 0x%x\n", temp);
	temp = (value >> 6) & 0x00000001;
	printf("pdm0 exist              = 0x%x\n", temp);
	temp = (value >> 7) & 0x00000001;
	printf("pdm1 exist              = 0x%x\n", temp);
	temp = (value >> 8) & 0x00000001;
	printf("pdm2 exist              = 0x%x\n", temp);
	temp = (value >> 9) & 0x00000001;
	printf("pdm3 exist              = 0x%x\n", temp);
	temp = (value >> 10) & 0x00000001;
	printf("i2s0 exist              = 0x%x\n", temp);
	temp = (value >> 11) & 0x00000001;
	printf("timerx2 exist           = 0x%x\n", temp);
	temp = (value >> 12) & 0x00000001;
	printf("i2c0 exist              = 0x%x\n", temp);
	temp = (value >> 13) & 0x00000001;
	printf("uart0 exist             = 0x%x\n", temp);
	temp = (value >> 14) & 0x00000001;
	printf("wdt0 exist              = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xE08);
	temp = (value >> 0) & 0x00000001;
	printf("peri0 sys exist         = 0x%x\n", temp);
	temp = (value >> 1) & 0x00000001;
	printf("emm0 exist              = 0x%x\n", temp);
	temp = (value >> 2) & 0x00000001;
	printf("emmc1 exist             = 0x%x\n", temp);
	temp = (value >> 3) & 0x00000001;
	printf("sdio2 exist             = 0x%x\n", temp);
	temp = (value >> 4) & 0x00000001;
	printf("gpio1 exist             = 0x%x\n", temp);
	temp = (value >> 5) & 0x00000001;
	printf("gpio2 exist             = 0x%x\n", temp);
	temp = (value >> 6) & 0x00000001;
	printf("i2s1 exist              = 0x%x\n", temp);
	temp = (value >> 7) & 0x00000001;
	printf("i2s2 exist              = 0x%x\n", temp);
	temp = (value >> 8) & 0x00000001;
	printf("bootspi exist           = 0x%x\n", temp);
	temp = (value >> 9) & 0x00000001;
	printf("uart1 exist             = 0x%x\n", temp);
	temp = (value >> 10) & 0x00000001;
	printf("uart2 exist             = 0x%x\n", temp);
	temp = (value >> 11) & 0x00000001;
	printf("i2c1 exist              = 0x%x\n", temp);
	temp = (value >> 12) & 0x00000001;
	printf("i2c2 exist              = 0x%x\n", temp);
	temp = (value >> 13) & 0x00000001;
	printf("i2c3 exist              = 0x%x\n", temp);
	temp = (value >> 14) & 0x00000001;
	printf("i2c4 exist              = 0x%x\n", temp);
	temp = (value >> 15) & 0x00000001;
	printf("pwm0 exist              = 0x%x\n", temp);
	temp = (value >> 16) & 0x00000001;
	printf("pdm4 exist              = 0x%x\n", temp);
	temp = (value >> 17) & 0x00000001;
	printf("pdm5 exist              = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xE0C);
	temp = (value >> 0) & 0x00000001;
	printf("peri1 sys exist         = 0x%x\n", temp);
	temp = (value >> 1) & 0x00000001;
	printf("gmac exist              = 0x%x\n", temp);
	temp = (value >> 2) & 0x00000001;
	printf("usb exist               = 0x%x\n", temp);
	temp = (value >> 3) & 0x00000001;
	printf("spacc exist             = 0x%x\n", temp);
	temp = (value >> 4) & 0x00000001;
	printf("pka exist               = 0x%x\n", temp);
	temp = (value >> 5) & 0x00000001;
	printf("mst spi0 exist          = 0x%x\n", temp);
	temp = (value >> 6) & 0x00000001;
	printf("mst spi1 exist          = 0x%x\n", temp);
	temp = (value >> 7) & 0x00000001;
	printf("mst spi2 exist          = 0x%x\n", temp);
	temp = (value >> 8) & 0x00000001;
	printf("slv spi exist           = 0x%x\n", temp);
	temp = (value >> 9) & 0x00000001;
	printf("pwm1 exist              = 0x%x\n", temp);
	temp = (value >> 10) & 0x00000001;
	printf("pwm2 exist              = 0x%x\n", temp);
	temp = (value >> 11) & 0x00000001;
	printf("pwm3 exist              = 0x%x\n", temp);
	temp = (value >> 12) & 0x00000001;
	printf("pwm4 exist              = 0x%x\n", temp);
	temp = (value >> 13) & 0x00000001;
	printf("pwm5 exist              = 0x%x\n", temp);
	temp = (value >> 14) & 0x00000001;
	printf("pwm6 exist              = 0x%x\n", temp);
	temp = (value >> 15) & 0x00000001;
	printf("pwm7 exist              = 0x%x\n", temp);
	temp = (value >> 16) & 0x00000001;
	printf("efuse ctrl exist        = 0x%x\n", temp);
	temp = (value >> 17) & 0x00000001;
	printf("uart3 exist             = 0x%x\n", temp);
	temp = (value >> 18) & 0x00000001;
	printf("uart4 exist             = 0x%x\n", temp);
	temp = (value >> 19) & 0x00000001;
	printf("uart5 exist             = 0x%x\n", temp);
	temp = (value >> 20) & 0x00000001;
	printf("uart6 exist             = 0x%x\n", temp);
	temp = (value >> 21) & 0x00000001;
	printf("uart7 exist             = 0x%x\n", temp);
	temp = (value >> 22) & 0x00000001;
	printf("i2c5 exist              = 0x%x\n", temp);
	temp = (value >> 23) & 0x00000001;
	printf("i2c6 exist              = 0x%x\n", temp);
	temp = (value >> 24) & 0x00000001;
	printf("i2s3 exist              = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xE10);
	temp = (value >> 0) & 0x00000001;
	printf("image sys exist         = 0x%x\n", temp);
	temp = (value >> 1) & 0x00000001;
	printf("isp exist               = 0x%x\n", temp);
	temp = (value >> 2) & 0x00000001;
	printf("dwp exist               = 0x%x\n", temp);
	temp = (value >> 3) & 0x00000001;
	printf("lcdc exist              = 0x%x\n", temp);
	temp = (value >> 4) & 0x00000001;
	printf("idma exist              = 0x%x\n", temp);
	temp = (value >> 5) & 0x00000001;
	printf("rot raw exist           = 0x%x\n", temp);
	temp = (value >> 6) & 0x00000001;
	printf("rot yuv exist           = 0x%x\n", temp);
	temp = (value >> 7) & 0x00000001;
	printf("mipi rx0 exist          = 0x%x\n", temp);
	temp = (value >> 8) & 0x00000001;
	printf("mipi rx1 exist          = 0x%x\n", temp);
	temp = (value >> 9) & 0x00000001;
	printf("mipi tx exist           = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xE14);
	temp = (value >> 0) & 0x00000001;
	printf("npu0 exist              = 0x%x\n", temp);
	temp = (value >> 1) & 0x00000001;
	printf("npu1 exist              = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xE18);
	temp = (value >> 0) & 0x00000001;
	printf("ddr0 sys exist          = 0x%x\n", temp);
	temp = (value >> 1) & 0x00000001;
	printf("ddr1 sys exist          = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xE1C);
	temp = (value >> 0) & 0x00000001;
	printf("vpu sys exist           = 0x%x\n", temp);

	value = readl(SYS_BASE + 0xE20);
	temp = (value >> 0) & 0x00000001;
	printf("gpu sys exist           = 0x%x\n", temp);
}

int main()
{
	printf(" ****************************** \n");
	print_sysinfo();
	printf(" ****************************** \n");

	while (1) {
		/* code */
	};
	return 0;
}

#else
#include "main_qemu.c"
#endif