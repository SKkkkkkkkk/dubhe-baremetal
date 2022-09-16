#include <stdio.h>
#define OFF_RW_ADDR (2*1024*1024)

// #define DMA_READ
// #define DMA_WRITE
// #define QUAD_READ

#if !defined(DMA_WRITE) // flash_write
	#define FLASH_WRITE(spi_id, addr, buf, size) flash_write(spi_id, addr, buf, size)
#else // flash_write_dma
	#define FLASH_WRITE(spi_id, addr, buf, size) flash_write_dma(spi_id, addr, buf, size)
#endif

#if !defined(DMA_READ)
	#if !defined(QUAD_READ) //cpu + std read
		#define FLASH_READ(spi_id, addr, buf, size) flash_read(spi_id, addr, buf, size)
	#else //cpu + quad read
		#define FLASH_READ(spi_id, addr, buf, size) flash_read_quad(spi_id, addr, buf, size)
	#endif
#else
	#if !defined(QUAD_READ) //dma + std read
		#define FLASH_READ(spi_id, addr, buf, size) flash_read_dma(spi_id, addr, buf, size)
	#else //dma + quad read
		#define FLASH_READ(spi_id, addr, buf, size) flash_read_quad_dma(spi_id, addr, buf, size)
	#endif
#endif

//case 1: 地址对齐，大小<=page size
#define CASE1_ADDR (0+OFF_RW_ADDR)
#define CASE1_SIZE (2048*2+1)
static uint8_t case1_r_buf[CASE1_SIZE] /*__attribute__((section(".noncache_mem.case1_r_buf")))*/ = {0};
static uint8_t case1_w_buf[CASE1_SIZE] /*__attribute__((section(".noncache_mem.case1_w_buf")))*/ = {0};

//case 2: 地址对齐，大小>page size且不对齐
#define CASE2_ADDR (0+OFF_RW_ADDR)
#define CASE2_SIZE (256*2+1)
static uint8_t case2_r_buf[CASE2_SIZE] /*__attribute__((section(".noncache_mem.case2_r_buf")))*/ = {0};
static uint8_t case2_w_buf[CASE2_SIZE] /*__attribute__((section(".noncache_mem.case2_w_buf")))*/ = {0};

//case 3: 地址不对齐，大小不对齐
#define CASE3_ADDR (1+OFF_RW_ADDR)
#define CASE3_SIZE (256*2+1)
static uint8_t case3_r_buf[CASE2_SIZE] /*__attribute__((section(".noncache_mem.case3_r_buf")))*/ = {0};
static uint8_t case3_w_buf[CASE2_SIZE] /*__attribute__((section(".noncache_mem.case3_w_buf")))*/ = {0};

//case 4: 地址不对齐，大小对齐
#define CASE4_ADDR (1+OFF_RW_ADDR)
#define CASE4_SIZE (2048*2)
static uint8_t case4_r_buf[CASE4_SIZE] /*__attribute__((section(".noncache_mem.case4_r_buf")))*/ = {0};
static uint8_t case4_w_buf[CASE4_SIZE] /*__attribute__((section(".noncache_mem.case4_w_buf")))*/ = {0};

#define CASE6_ADDR (0x1000000)
#define CASE6_SIZE (2048*2)
static uint8_t case6_r_buf[CASE6_SIZE] /*__attribute__((section(".noncache_mem.case6_r_buf")))*/ = {0};
static uint8_t case6_w_buf[CASE6_SIZE] /*__attribute__((section(".noncache_mem.case6_w_buf")))*/ = {0};

#define CASE7_ADDR (0x1000000-1)
#define CASE7_SIZE (2048*2)
static uint8_t case7_r_buf[CASE6_SIZE] /*__attribute__((section(".noncache_mem.case7_r_buf")))*/ = {0};
static uint8_t case7_w_buf[CASE6_SIZE] /*__attribute__((section(".noncache_mem.case7_r_buf")))*/ = {0};

#define P25Q40UJ_ADDR (0x00)
#define P25Q40UJ_SIZE (512*1024)
static uint8_t p25q40uj_r_buf[P25Q40UJ_SIZE] = {0};
static uint8_t p25q40uj_w_buf[P25Q40UJ_SIZE] = {0};

void nor_flash_test(spi_id_t spi_id, flash_model_t flash_model)
{
	uint8_t _flash_id[3];
	if(flash_init(spi_id, 20, 3, flash_model) == false)
	{
		printf("flash_init error.\n\r");
		return;
	}
#if !defined(DMA_WRITE) // flash_write
	printf("flash_write\n\r");
#else // flash_write_dma
	printf("flash_write_dma\n\r");
#endif

#if !defined(DMA_READ)
	#if !defined(QUAD_READ) //cpu + std read
		printf("flash_read\n\r");
	#else //cpu + quad read
		printf("flash_read_quad\n\r");
	#endif
#else
	#if !defined(QUAD_READ) //dma + std read
		printf("flash_read_dma\n\r");
	#else //dma + quad read
		printf("flash_read_quad_dma\n\r");
	#endif
#endif
	
	// flash_reset(spi_id);
	flash_read_id(spi_id, _flash_id, 3);
	printf("flash_id: 0x%x 0x%x 0x%x\n\r", _flash_id[0], _flash_id[1], _flash_id[2]);

	if(flash_model == P25Q40UJ)
	{
		for(uint32_t i = 0;i<P25Q40UJ_SIZE;i++)
			p25q40uj_w_buf[i] = i%256;
		for(uint32_t i = 0;i<(P25Q40UJ_SIZE/4096);i++)
			flash_sector_erase(spi_id, (P25Q40UJ_ADDR&0xfffff000)+4096*i);
		// flash_read(spi_id, P25Q40UJ_ADDR, p25q40uj_r_buf, P25Q40UJ_SIZE);
		memset(p25q40uj_r_buf, 0xa5, P25Q40UJ_SIZE);
		flash_write(spi_id, P25Q40UJ_ADDR, p25q40uj_w_buf, P25Q40UJ_SIZE);
		flash_read(spi_id, P25Q40UJ_ADDR, p25q40uj_r_buf, P25Q40UJ_SIZE);
		if(memcmp(p25q40uj_w_buf,p25q40uj_r_buf,P25Q40UJ_SIZE)==0) //比对
		{
			printf("p25q40uj Test OK!\n\r");
		}
		else
		{
			printf("p25q40uj Test Error!\n\r");
		}
		while(1);
	}

	#define TEST_SECTOR_NUMS 2
	for(uint32_t i = 0;i<CASE1_SIZE;i++)
		case1_w_buf[i] = i%256;
	for(uint32_t i = 0;i<TEST_SECTOR_NUMS;i++)
		flash_sector_erase(spi_id, (CASE1_ADDR&0xfffff000)+4096*i);
	FLASH_WRITE(spi_id, CASE1_ADDR, case1_w_buf, CASE1_SIZE);
	memset(case1_r_buf, 0xa5, CASE1_SIZE);
	FLASH_READ(spi_id, CASE1_ADDR, case1_r_buf, CASE1_SIZE);
	if(memcmp(case1_w_buf,case1_r_buf,CASE1_SIZE)==0) //比对
	{
		printf("case1 Test OK!\n\r");
	}
	else
	{
		printf("case1 Test Error!\n\r");
	}


	for(uint32_t i = 0;i<CASE2_SIZE;i++)
		case2_w_buf[i] = i%256;
	for(uint32_t i = 0;i<TEST_SECTOR_NUMS;i++)
		flash_sector_erase(spi_id, (CASE2_ADDR&0xfffff000)+4096*i);
	FLASH_WRITE(spi_id, CASE2_ADDR, case2_w_buf, CASE2_SIZE);
	memset(case2_r_buf, 0xa5, CASE2_SIZE);
	FLASH_READ(spi_id, CASE2_ADDR, case2_r_buf, CASE2_SIZE);
	if(memcmp(case2_w_buf,case2_r_buf,CASE2_SIZE)==0) //比对
	{
		printf("case2 Test OK!\n\r");
	}
	else
	{
		printf("case2 Test Error!\n\r");
	}

	for(uint32_t i = 0;i<CASE3_SIZE;i++)
		case3_w_buf[i] = i%256;
	for(uint32_t i = 0;i<TEST_SECTOR_NUMS;i++)
		flash_sector_erase(spi_id, (CASE3_ADDR&0xfffff000)+4096*i);
	FLASH_WRITE(spi_id, CASE3_ADDR, case3_w_buf, CASE3_SIZE);
	memset(case3_r_buf, 0xa5, CASE3_SIZE);
	FLASH_READ(spi_id, CASE3_ADDR, case3_r_buf, CASE3_SIZE);
	if(memcmp(case3_w_buf,case3_r_buf,CASE3_SIZE)==0) //比对
	{
		printf("case3 Test OK!\n\r");
	}
	else
	{
		printf("case3 Test Error!\n\r");
	}
	

	for(uint32_t i = 0;i<CASE4_SIZE;i++)
		case4_w_buf[i] = i%256;
	for(uint32_t i = 0;i<TEST_SECTOR_NUMS;i++)
		flash_sector_erase(spi_id, (CASE4_ADDR&0xfffff000)+4096*i);
	FLASH_WRITE(spi_id, CASE4_ADDR, case4_w_buf, CASE4_SIZE);
	memset(case4_r_buf, 0xa5, CASE4_SIZE);
	FLASH_READ(spi_id, CASE4_ADDR, case4_r_buf, CASE4_SIZE);
	if(memcmp(case4_w_buf,case4_r_buf,CASE4_SIZE)==0) //比对
	{
		printf("case4 Test OK!\n\r");
	}
	else
	{
		printf("case4 Test Error!\n\r");
	}

if((flash_model==GD25LQ255)||(flash_model==W25Q256JW))
{
	for(uint32_t i = 0;i<CASE6_SIZE;i++)
		case6_w_buf[i] = i%256;
	for(uint32_t i = 0;i<TEST_SECTOR_NUMS;i++)
		flash_sector_erase(spi_id, (CASE6_ADDR&0xfffff000)+4096*i);
	FLASH_WRITE(spi_id, CASE6_ADDR, case6_w_buf, CASE6_SIZE);
	memset(case6_r_buf, 0xa5, CASE6_SIZE);
	FLASH_READ(spi_id, CASE6_ADDR, case6_r_buf, CASE6_SIZE);
	if(memcmp(case6_w_buf,case6_r_buf,CASE6_SIZE)==0) //比对
	{
		printf("case6 Test OK!\n\r");
	}
	else
	{
		printf("case6 Test Error!\n\r");
	}

	for(uint32_t i = 0;i<CASE7_SIZE;i++)
		case7_w_buf[i] = i%256;
	for(uint32_t i = 0;i<TEST_SECTOR_NUMS;i++)
		flash_sector_erase(spi_id, (CASE7_ADDR&0xfffff000)+4096*i);
	FLASH_WRITE(spi_id, CASE7_ADDR, case7_w_buf, CASE7_SIZE);
	memset(case7_r_buf, 0xa5, CASE7_SIZE);
	FLASH_READ(spi_id, CASE7_ADDR, case7_r_buf, CASE7_SIZE);
	if(memcmp(case7_w_buf,case7_r_buf,CASE7_SIZE)==0) //比对
	{
		printf("case7 Test OK!\n\r");
	}
	else
	{
		printf("case7 Test Error!\n\r");
	}
}


	flash_deinit(spi_id);
}

/*
	#define CPU_READ_BLOCK_SIZE 32U
	#define BLOCK_SIZE_DMA (2048U)
*/
// #define CASE5_ADDR (OFF_RW_ADDR+10)
// #define CASE5_SIZE (1024*1024-1)
// static uint8_t case5_r_buf[CASE5_SIZE] /* 使用DMA时，接收首地址和大小需要对齐DU_CACHE_LINE_SIZE */ __attribute__((aligned(64))) /*__attribute__((section(".noncache_mem.case5_r_buf")))*/ = {0};
// static uint8_t case5_w_buf[CASE5_SIZE] /*__attribute__((section(".noncache_mem.case5_w_buf")))*/ = {0};
// void flash_fastest_read_test(spi_id_t spi_id, flash_model_t flash_model)
// {
// 	printf("flash_fastest_read_test:\n\r");

// 	#ifndef NO_DCACHE
// 		printf("\tUSE_DCACHE\n\r");
// 	#endif
// 	#ifndef NO_ICACHE
// 		printf("\tUSE_ICACHE\n\r");
// 	#endif
	
// 	uint8_t flash_id[3];
// 	uint16_t spi_div = 20;
// 	if(!flash_init(spi_id, spi_div, 3, flash_model))
// 	{
// 		printf("\tflash_init error.\n\r");
// 		return;
// 	}
// 	printf("\tSPI_DIV: %u\n\r", spi_div);
// 	printf("\tTest Addr: 0x%x\n\r", CASE5_ADDR);

// 	flash_reset(spi_id);
// 	flash_read_id(spi_id, flash_id, 3);
// 	printf("\tflash_id: 0x%x 0x%x 0x%x\n\r", flash_id[0], flash_id[1], flash_id[2]);
	
// 	// 写SR的QE bit
// 	uint8_t sr[2] = {0x00,0x00};
// 	sr[0] = _flash_read_sr(spi_id);
// 	sr[1] = _flash_read_sr_high(spi_id);
// 	sr[1] |= Quad_Enable_Bit;
// 	_flash_write_enable(spi_id);
// 	_flash_write_sr(spi_id, sr);

// 	systimer_id_t timer;
// 	uint64_t delta;

// #if !defined(DMA_WRITE) // flash_write
// 	printf("\tflash_write\n\r");
// #else // flash_write_dma
// 	printf("\tflash_write_dma\n\r");
// #endif

// 	for(uint32_t i = 0;i<CASE5_SIZE;i++)
// 		case5_w_buf[i] = i%256;
// 	for(uint32_t i = 0;i<257;i++)
// 		flash_sector_erase(spi_id, (CASE5_ADDR&0xfffff000)+4096*i);
// 	FLASH_WRITE(spi_id, CASE5_ADDR, case5_w_buf, CASE5_SIZE);
	
// 	//1. cpu方式 + 标准spi模式
// 	memset(case5_r_buf, 0xa5, CASE5_SIZE);
//  	timer = systimer_acquire_timer();
// 	flash_read(spi_id, CASE5_ADDR, case5_r_buf, CASE5_SIZE);
// 	delta = systimer_get_elapsed_time(timer, IN_MS);
// 	systimer_release_timer(timer);
// 	if(memcmp(case5_w_buf,case5_r_buf,CASE5_SIZE)==0) //比对
// 	{
// 		printf("\tcpu + std: %d\n\r", (int)delta);
// 	}
// 	else
// 	{
// 		printf("\tcpu + std read error!\n\r");
// 	}

// 	//2. dma方式 + 标准spi模式
// 	memset(case5_r_buf, 0xa5, CASE5_SIZE);
// 	timer = systimer_acquire_timer();
// 	flash_read_dma(spi_id, CASE5_ADDR, case5_r_buf, CASE5_SIZE);
// 	delta = systimer_get_elapsed_time(timer, IN_MS);
// 	systimer_release_timer(timer);
// 	if(memcmp(case5_w_buf,case5_r_buf,CASE5_SIZE)==0) //比对
// 	{
// 		printf("\tdma + std: %d\n\r", (int)delta);
// 	}
// 	else
// 	{
// 		printf("\tdma + std read error!\n\r");
// 	}

// 	//3. cpu方式 + quad
// 	memset(case5_r_buf, 0xa5, CASE5_SIZE);
// 	timer = systimer_acquire_timer();
// 	flash_read_quad(spi_id, CASE5_ADDR, case5_r_buf, CASE5_SIZE);
// 	delta = systimer_get_elapsed_time(timer, IN_MS);
// 	systimer_release_timer(timer);
// 	if(memcmp(case5_w_buf,case5_r_buf,CASE5_SIZE)==0) //比对
// 	{
// 		printf("\tcpu + quad: %d\n\r", (int)delta);
// 	}
// 	else
// 	{
// 		printf("\tcpu + quad read error!\n\r");
// 	}

// 	//4. dma方式 + quad
// 	memset(case5_r_buf, 0xa5, CASE5_SIZE);
// 	timer = systimer_acquire_timer();
// 	flash_read_quad_dma(spi_id, CASE5_ADDR, case5_r_buf, CASE5_SIZE);
// 	delta = systimer_get_elapsed_time(timer, IN_MS);
// 	systimer_release_timer(timer);
// 	if(memcmp(case5_w_buf,case5_r_buf,CASE5_SIZE)==0) //比对
// 	{
// 		printf("\tdma + quad: %d\n\r", (int)delta);
// 	}
// 	else
// 	{
// 		printf("\tdma + quad read error!\n\r");
// 	}
// }

// uint32_t flash_read_dma_start(spi_id_t spi_id, DMA_Channel_t* const ch, uint32_t addr, uint8_t* const buf, uint32_t size);

// #ifdef __GNUC__
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-label"
// #pragma GCC diagnostic ignored "-Wmisleading-indentation"
// #endif
// static uint8_t case1_r_buf_flash2[CASE1_SIZE] /*__attribute__((section(".noncache_mem.case1_r_buf_flash2")))*/ = {0};
// void flash_2_test(spi_id_t spi_id1, spi_id_t spi_id2, flash_model_t flash_model1, flash_model_t flash_model2)
// {
// 	uint8_t _flash_id[3];
// 	if(flash_init(spi_id1, 10, 3, flash_model1) == false)
// 	{
// 		printf("flash1 init error.\n\r");
// 		return;
// 	}
// 	if(flash_init(spi_id2, 10, 3, flash_model2) == false)
// 	{
// 		printf("flash2 init error.\n\r");
// 		return;
// 	}

// 	flash_reset(spi_id1);
// 	flash_read_id(spi_id1, _flash_id, 3);
// 	printf("flash1_id: 0x%x 0x%x 0x%x\n\r", _flash_id[0], _flash_id[1], _flash_id[2]);

// 	flash_reset(spi_id2);
// 	flash_read_id(spi_id2, _flash_id, 3);
// 	printf("flash2_id: 0x%x 0x%x 0x%x\n\r", _flash_id[0], _flash_id[1], _flash_id[2]);

// 	for(uint32_t i = 0;i<CASE1_SIZE;i++)
// 		case1_w_buf[i] = i%256;
// 	for(uint32_t i = 0;i<TEST_SECTOR_NUMS;i++)
// 	{
// 		flash_sector_erase(spi_id1, (CASE1_ADDR&0xfffff000)+4096*i);
// 		flash_sector_erase(spi_id2, (CASE1_ADDR&0xfffff000)+4096*i);
// 	}
// 	FLASH_WRITE(spi_id1, CASE1_ADDR, case1_w_buf, CASE1_SIZE);
// 	FLASH_WRITE(spi_id2, CASE1_ADDR, case1_w_buf, CASE1_SIZE);

// 	systimer_id_t timer_id;
// 	uint32_t delta;

// 	memset(case1_r_buf, 0xa5, CASE1_SIZE);
// 	printf("flash1:\n\r");
// 	if((timer_id = systimer_acquire_timer()) == 0xff)
// 	{
// 		printf("no systimer.\n\r");
// 		return;
// 	}
// 	FLASH_READ(spi_id1, CASE1_ADDR, case1_r_buf, CASE1_SIZE);
// 	// FLASH_READ(spi_id2, CASE1_ADDR, case1_r_buf_flash2, CASE1_SIZE);
// 	delta = systimer_get_elapsed_time(timer_id, IN_US);
// 	systimer_release_timer(timer_id);
// 	if((memcmp(case1_w_buf,case1_r_buf,CASE1_SIZE)==0)) //比对
// 	{
// 		printf("\tdelta: %u\n\r", delta);
// 	}
// 	else
// 	{
// 		printf("\tserial read error!\n\r");
// 	}

// 	memset(case1_r_buf_flash2, 0xa5, CASE1_SIZE);
// 	printf("flash2:\n\r");
// 	if((timer_id = systimer_acquire_timer()) == 0xff)
// 	{
// 		printf("no systimer.\n\r");
// 		return;
// 	}
// 	// FLASH_READ(spi_id1, CASE1_ADDR, case1_r_buf, CASE1_SIZE);
// 	FLASH_READ(spi_id2, CASE1_ADDR, case1_r_buf_flash2, CASE1_SIZE);
// 	delta = systimer_get_elapsed_time(timer_id, IN_US);
// 	systimer_release_timer(timer_id);
// 	if(memcmp(case1_w_buf,case1_r_buf_flash2,CASE1_SIZE)==0) //比对
// 	{
// 		printf("\tdelta: %u\n\r", delta);
// 	}
// 	else
// 	{
// 		printf("\tserial read error!\n\r");
// 	}

// 	memset(case1_r_buf, 0xa5, CASE1_SIZE);
// 	memset(case1_r_buf_flash2, 0xa5, CASE1_SIZE);
// 	printf("serial:\n\r");
// 	if((timer_id = systimer_acquire_timer()) == 0xff)
// 	{
// 		printf("no systimer.\n\r");
// 		return;
// 	}
// 	FLASH_READ(spi_id1, CASE1_ADDR, case1_r_buf, CASE1_SIZE);
// 	FLASH_READ(spi_id2, CASE1_ADDR, case1_r_buf_flash2, CASE1_SIZE);
// 	delta = systimer_get_elapsed_time(timer_id, IN_US);
// 	systimer_release_timer(timer_id);
// 	if((memcmp(case1_w_buf,case1_r_buf,CASE1_SIZE)==0) && (memcmp(case1_w_buf,case1_r_buf_flash2,CASE1_SIZE)==0)) //比对
// 	{
// 		printf("\tdelta: %u\n\r", delta);
// 	}
// 	else
// 	{
// 		printf("\tserial read error!\n\r");
// 	}
	

// 	memset(case1_r_buf, 0xa5, CASE1_SIZE);
// 	memset(case1_r_buf_flash2, 0xa5, CASE1_SIZE);
// 	printf("parallel:\n\r");
// 	if((timer_id = systimer_acquire_timer()) == 0xff)
// 	{
// 		printf("no systimer.\n\r");
// 		return;
// 	}

// 	DMA_Channel_t ch1;
// 	uint32_t addr1 = CASE1_ADDR;
// 	uint8_t* r_buf1 = case1_r_buf;
// 	uint32_t remainder1 = CASE1_SIZE;
// 	uint32_t size1;
// 	uint32_t count1 = 0;

// 	DMA_Channel_t ch2;
// 	uint32_t addr2 = CASE1_ADDR;
// 	uint8_t* r_buf2 = case1_r_buf_flash2;
// 	uint32_t remainder2 = CASE1_SIZE;
// 	uint32_t size2;
// 	uint32_t count2 = 0;

// 	while( ((remainder1!=0)||(remainder2!=0)) || ((count1!=0)||(count2!=0))  )
// 	{
// 	job1:
// 		if( (remainder1==0) && (count1==0) ) 
// 			goto job2;
// 		if(addr1 != CASE1_ADDR)
// 		{
// 			if(!is_last_flash_read_finish(spi_id1, ch1))
// 				goto job2;
// 			else
// 				count1--;
// 		}
// 		size1 = flash_read_dma_start(spi_id1, &ch1, addr1, r_buf1, remainder1);
// 		if(size1!=0)
// 		{
// 			addr1 += size1;
// 			r_buf1 += size1;
// 			remainder1 -= size1;
// 			count1++;
// 		}
		
// 	job2:
// 		if( (remainder2==0) && (count2==0) ) 
// 			continue;
// 		if(addr2 != CASE1_ADDR)
// 		{
// 			if(!is_last_flash_read_finish(spi_id2, ch2))
// 				continue;
// 			else
// 				count2--;
// 		}
// 		size2 = flash_read_dma_start(spi_id2, &ch2, addr2, r_buf2, remainder2);
// 		if(size2!=0)
// 		{
// 			addr2 += size2;
// 			r_buf2 += size2;
// 			remainder2 -= size2;
// 			count2++;
// 		}
// 	}

// 	delta = (uint32_t)systimer_get_elapsed_time(timer_id, IN_US);
// 	systimer_release_timer(timer_id);

// 	if((memcmp(case1_w_buf,case1_r_buf,CASE1_SIZE)==0) && (memcmp(case1_w_buf,case1_r_buf_flash2,CASE1_SIZE)==0)) //比对
// 	{
// 		printf("\tdelta: %u\n\r", delta);
// 	}
// 	else
// 	{
// 		printf("\tparallel read error!\n\r");
// 	}

// 	flash_deinit(spi_id1);
// 	flash_deinit(spi_id2);
// }


// #ifdef __GNUC__
// #pragma GCC diagnostic pop
// #endif