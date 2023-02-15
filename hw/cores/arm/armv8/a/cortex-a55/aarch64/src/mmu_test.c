#include "arch_features.h"
#include <stdint.h>

#define TT_S1_TABLE          0x00000000000000003    // NSTable=0 PXNTable=0 UXNTable=0 APTable=0

// TT block entries templates   (L1 and L2 NOT L3)
// Assuming table contents:
// 0 = b01000100 = Normal Inner/Outer Non-Cacheable
// 1 = b11111111 = Normal Inner/Outer WB/WA/RA
// 2 = b00000000 = Device-nGnRnE
#define TT_S1_FAULT           0x0
#define TT_S1_NORMAL_NO_CACHE 0x00000000000000401    // Index = 0 AF=1
#define TT_S1_NORMAL_WBWA     0x00000000000000405    // Index = 1 AF=1
#define TT_S1_DEVICE_nGnRnE   0x00600000000000409    // Index = 2 AF=1 PXN=1 UXN=1

#define TT_S1_UXN             (1 << 54)
#define TT_S1_PXN             (1 << 53)
#define TT_S1_nG              (1 << 11)
#define TT_S1_NS              (1 << 5)

#define TT_S1_NON_SHARED      (0 << 8)               // Non-shareable
#define TT_S1_INNER_SHARED    (3 << 8)               // Inner-shareable
#define TT_S1_OUTER_SHARED    (2 << 8)               // Outer-shareable

#define TT_S1_PRIV_RW         (0x0)
#define TT_S1_PRIV_RO         (0x2 << 6)
#define TT_S1_USER_RW         (0x1 << 6)
#define TT_S1_USER_RO         (0x3 << 6)


// 4KB细粒度 PageTable都需要4KB对齐
uint64_t tt_l1[512] __attribute__((__aligned__(4096)));
uint64_t tt_l2[512] __attribute__((__aligned__(4096)));
uint64_t tt_l3_1[512] __attribute__((__aligned__(4096)));
uint64_t tt_l3_2[512] __attribute__((__aligned__(4096)));


void config_mmu(void)
{
	// 设置translation table基地址
	write_ttbr0_el3((uint64_t)&tt_l1);

	// 通过mair_el3寄存器设置了3种内存属性
	write_mair_el3(0x000000000000FF44);

	// 设置4KB内存细粒度控, VA/PA space size，Page table cacheability&shareability.
	write_tcr_el3(0x19 | (1<<8) | (1<<10) | (3<<12));
	dsbsy();
	isb();

	// 清空Translation Lookaside Buffer.
	tlbialle3();
	dsbsy();
	isb();

	// L1 Table, 1G细粒度
	tt_l1[0] = TT_S1_TABLE | (uint64_t)&tt_l2; // [0, 0x40000000) table entry => tt_l2
	tt_l1[1] = TT_S1_NORMAL_WBWA | TT_S1_OUTER_SHARED | 0x40000000; // [0x40000000, 0x80000000) // NORMAL_WBWA, INNER_SHARED, RW
	tt_l1[2] = TT_S1_NORMAL_WBWA | TT_S1_OUTER_SHARED | 0x80000000; // [0x80000000, 0xC0000000) // NORMAL_WBWA, INNER_SHARED, RW
	tt_l1[3] = TT_S1_NORMAL_WBWA | TT_S1_OUTER_SHARED | 0xC0000000; // [0xC0000000, 0x100000000) // NORMAL_WBWA, INNER_SHARED, RW

	// L2 Table, 2M细粒度
	tt_l2[0] = TT_S1_TABLE | (uint64_t)&tt_l3_1; // [0, 0x20_0000)  table entry => tt_l3_1
	tt_l2[256] = TT_S1_TABLE | (uint64_t)&tt_l3_2; // [0x2000_0000, 0x2020_0000) table entry => tt_l3_2

	for(uint16_t i = 264; i<376; i++) // [0x2100_0000, 0x2F000000) DEVICE_nGnRnE
		tt_l2[i] =  TT_S1_DEVICE_nGnRnE | (i*2*1024*1024);

	// L3 Table, 4KB细粒度
	for(uint8_t i = 0; i<64 ; i++) // 256KB ROM, NORMAL_WBWA INNER_SHARED RO
		tt_l3_1[i] = 3 | TT_S1_NORMAL_WBWA | TT_S1_OUTER_SHARED | TT_S1_PRIV_RO | (i*4096);

	for(uint8_t i = 0; i<64 ; i++) // 256KB SYSRAM, NORMAL_WBWA INNER_SHARED RW
		tt_l3_2[i] = 3 | TT_S1_NORMAL_WBWA | TT_S1_OUTER_SHARED | (i*4096 + 0x20000000);


	dsbsy();

	// enable i/d cache & mmu.
	uint64_t sctlr_el3 = read_sctlr_el3();
	sctlr_el3 |= (1<<0) | (1<<2) | (1<<12); 
	write_sctlr_el3(sctlr_el3);
	isb();
}