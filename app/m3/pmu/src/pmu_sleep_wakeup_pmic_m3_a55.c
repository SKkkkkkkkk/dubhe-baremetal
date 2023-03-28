#include "m3.h"
#include <stdio.h>
#include "systimer.h"
#include "pmu.h"
#include "pinmux.h"
#include "dw_apb_gpio.h"
#include "axp2101.h"
#include "i2c_wo.h"
#include "rtc.h"

int gic_cnt = 0;
int err_cnt = 0;
int count = 0;
#define GPIO0                GPIO0_BASE
#define GPIO_WAKEUP      1
#define PMIC_WAKEUP      1
#define STOP             1

#define WAKEUP_PIN			16
#define WAKEUP_PIN_GROUP	(WAKEUP_PIN / 32)
#define WAKEUP_PIN_NUM		(WAKEUP_PIN % 32)
#define DEBUG_REGS			(0x4e000ee0)
#define DEBUG_REGS_RST		(0x4e000ee4)
#define DUMP_LOG(x)			(REG32(DEBUG_REGS) = (0x1UL << (x)) | REG32(DEBUG_REGS))

void set_power_off_m3(uint8_t pid)
{
	set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<pid));
	set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR, OFF);
}

void set_power_on_m3(uint8_t pid)
{
	set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<pid));
	set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,ON);
}

void set_power_off_a55(uint8_t pid)
{
    // set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<pid));
    if(pid == AP) {
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                               OP0 << PPU_PWPR_OP_POLICY_LSB |
                                                 1 << PPU_PWPR_PWR_DYN_EN_LSB | OFF));
    } else if (pid < AP && pid >= CORE0){
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | OFF));
    }else{
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR, OFF);
	}
}

void set_power_on_a55(uint8_t pid)
{
    // set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<pid));
    if(pid == AP) {
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                               OP4 << PPU_PWPR_OP_POLICY_LSB |
                                                 1 << PPU_PWPR_PWR_DYN_EN_LSB | ON));
    } else if (pid < AP && pid >= CORE0){
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | ON));
    } else {
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,ON);
    }
}

void irq_handler_gpio(void)
{
    unsigned int intstatus = *((unsigned int *) (GPIO0 + 0x40));

    count++;

    if (intstatus & (1 << WAKEUP_PIN_NUM)) {
        gpio_clear_interrput(WAKEUP_PIN_GROUP, WAKEUP_PIN_NUM);
        printf("gpio in irq 0. %d\n", count);
    } else {
        printf("irq is not 0\n");
    }

    return;
}

void set_gpio_wakeup(void)
{
	pinmux(0, 7); //gpio0_0

    gpio_init_config_t gpio_init_config = {
        .group             = WAKEUP_PIN_GROUP,
        .pin               = WAKEUP_PIN_NUM,
        .gpio_control_mode = Software_Mode,
        .gpio_mode         = GPIO_Falling_Int_Mode,
        // .gpio_mode = GPIO_Input_Mode,
    };
    gpio_init(&gpio_init_config);
}

void set_power_off_seq(void)
{
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_IMR_PMU_WAKEUP_1_MASK_LSB | 1 << PMU_IMR_PPU_LP_IRQ_MASK_LSB));
    // set_pmu_reg(PMU,PMU_PDSEQ_0_LOGICID_ADDR,(PERI0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_0_LOGICID_ADDR,(DDR1 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_1_LOGICID_ADDR,(DDR0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_2_LOGICID_ADDR,(LP << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_0_LOGICID_ADDR,(PERI1 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_4_LOGICID_ADDR,(CORE3 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_5_LOGICID_ADDR,(CORE2 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_6_LOGICID_ADDR,(CORE1 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_7_LOGICID_ADDR,(CORE0 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_8_LOGICID_ADDR,(AP << 8 | OFF));
    set_pmu_reg(PMU,PMU_PD_CR_NUM_PD_ADDR, (3 << 4 | 1));
}

void set_default_power_on_seq(void)
{
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_IMR_PPU_GPU_IRQ_MASK_LSB));
    // set_pmu_reg(PMU,PMU_PUSEQ_0_LOGICID_ADDR,(PERI0 << 8 | ON));
    //set_pmu_reg(PMU,PMU_PUSEQ_1_LOGICID_ADDR,(PERI1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_0_LOGICID_ADDR,(DDR0 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_1_LOGICID_ADDR,(DDR1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_2_LOGICID_ADDR,(VPU << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_3_LOGICID_ADDR,(NPU0 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_4_LOGICID_ADDR,(NPU1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_5_LOGICID_ADDR,(IMG << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_6_LOGICID_ADDR,(ISP << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_7_LOGICID_ADDR,(DEWARP << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_8_LOGICID_ADDR,(GPU << 8 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_4_LOGICID_ADDR,(AP << 8 | OP4 << 4 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_5_LOGICID_ADDR,(CORE0 << 8 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_6_LOGICID_ADDR,(CORE1 << 8 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_7_LOGICID_ADDR,(CORE2 << 8 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_8_LOGICID_ADDR,(CORE3 << 8 | ON));
    set_pmu_reg(PMU,PMU_PU_CR_NUM_PD_ADDR, (9 << 4 | 1));
}

void pmu_irqhandler (void) {
    unsigned int exp;
    printf("NOTE PMU interrupt occurred! gic_cnt:%d\n",gic_cnt);
    //IRQ_Disable(INT_PMU_INTR);
    unsigned int tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
    printf("read PMU_ISR:%x\n",tmp);
    if(gic_cnt == 0) {
        //default power on interrupt
        exp = get_pmu_isr(PMU); //0x7d961f;
        if(((tmp >> PMU_IMR_PMU_WAKEUP_1_MASK_LSB) & 0x1) == 1) {
            gic_cnt = 1; //jump gic_cnt=2 power off operation
            //clear_all_ppu_isr();
            // exp = 0x203961f;
            exp = 0x1 << PMU_IMR_PMU_WAKEUP_1_MASK_LSB |
			      0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
			      0x1 << PMU_ISR_PPU_DDR0_IRQ_LSB |
                  0x1 << PMU_ISR_PPU_DDR1_IRQ_LSB |
			      0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB;
        }
		for(int i=0; i<20; i++) {
			if(get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR) == exp) {
				if(gic_cnt == 0) {
					printf("NOTE cold power on, all default power on PD power up\n");
				} else {
					printf("NOTE wakeup, enter power on LP\n");
				}
				break;
			} else if(i==19) {
                printf("ERROR isr not match exp:0x%x",exp);
                err_cnt++;
            }
		}
		//check each pd ISR and clear PPU_ISR
		for(int i=0; i<PDNUM; i++) {
			if(((tmp >> i) & 1) == 1) {
				err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
			}
		}
		set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,0xffffffff);
    } else if(gic_cnt == 2) {
        // exp = 0x1efff;
        exp = //0x1 << PMU_ISR_PPU_CORE0_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CORE1_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CORE2_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CORE3_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_CLUSTER_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_PERI1_IRQ_LSB;
			  0x1 << PMU_ISR_PPU_VPU_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_IMG_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_DDR0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DDR1_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB;

        if(tmp == exp) {
            printf("NOTE all PD power up again\n");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
                }
            }
        } else {
            printf("ERROR power up seq result exp:0x%xn",exp);
            err_cnt++;
        }
    }
    gic_cnt++;
    set_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR,0xffffffff);
}

int pmic_to_sleep_delay(int ms)
{
    uint32_t tmp;
    tmp = REG32(SYS_BASE + 0x8dc);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8dc) = tmp;

    tmp = REG32(SYS_BASE + 0x8e0);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8e0) = tmp;
    i2c_wo_init(0x34);

    i2c_wo_delay(ms); // 1s
    // i2c_wo_fifo(0x10);  //poweroff
    // i2c_wo_fifo(0x01 | STOP<<8);
    i2c_wo_fifo(AXP2101_INTEN1);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(AXP2101_INTEN2);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(AXP2101_INTEN3);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x26);
    i2c_wo_fifo(0x09 | STOP << 8);
    i2c_wo_fifo(0x80);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x90);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x91);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(0x26);
    i2c_wo_fifo(0x19 | STOP << 8);
    i2c_wo_fifo(0x41);
    i2c_wo_fifo(0x0c | STOP << 8);

    printf("axp2101_powerkey_suspend !!! \n");
	return 0;
}

int pmic_clear_irq_wo(int ms)
{
    uint32_t tmp;
    tmp = REG32(SYS_BASE + 0x8dc);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8dc) = tmp;

    tmp = REG32(SYS_BASE + 0x8e0);
    tmp &= ~(7 << 4);
    tmp |= 3 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8e0) = tmp;
    i2c_wo_init(0x34);

    i2c_wo_delay(ms); // 1s
    // i2c_wo_fifo(0x10);  //poweroff
    // i2c_wo_fifo(0x01 | STOP<<8);
    i2c_wo_fifo(AXP2101_INTEN1);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(AXP2101_INTEN2);
    i2c_wo_fifo(0x00 | STOP << 8);
    i2c_wo_fifo(AXP2101_INTEN3);
    i2c_wo_fifo(0x00 | STOP << 8);
	i2c_wo_start();

	return 0;
}


int pmic_clear_irq(void)
{
	uint32_t tmp;
    tmp = REG32(SYS_BASE + 0x8dc);
    tmp &= ~(7 << 4);
    tmp |= 0 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8dc) = tmp;

    tmp = REG32(SYS_BASE + 0x8e0);
    tmp &= ~(7 << 4);
    tmp |= 0 << 4;
    tmp |= 1 << 0;
    REG32(SYS_BASE + 0x8e0) = tmp;

    int err = 0;
    // unsigned int val;
    struct pmic_cfg cfg;

    strcpy(cfg.name, "axp2101");
    cfg.reg_addr   = 0x34;
    cfg.i2c_bus    = 4;
    cfg.check_addr = 0x00;
    cfg.check_len  = 1;
    err            = axp2101_i2c_init(&cfg);

    if (!err) {
        axp20x_i2c_write(AXP2101_INTEN1, 0);
        axp20x_i2c_write(AXP2101_INTEN2, 0);
        axp20x_i2c_write(AXP2101_INTEN3, 0);
    }

	return 0;
}

void delay(int count)
{
	int i;
	for(i=1;i<=count;i++)
		;
}

void rtc_init(void)
{
	printf("rtc init start.\n");
	PMIC->PMIC_WPR  = 0x5a5a5a5aUL;
	//INT->INT_CLR = 0x0;
	// PMIC->IRQ_IN_MODE   = 0xff3UL;   //16s  low
	PMIC->IRQ_IN_MODE   = 0x103UL;    //1s low
	//PMIC->IRQ_OUT_MODE    = 0x3UL;
	//PMIC->IRQ_OUT_POLAR = 0x5UL;
	//PMIC->IRQ_OUT_TIME    = 0x3fffffffUL;
	// PMIC->IO_CFG        = 0x2UL;
	PMIC->PMIC_WPR      = 0x87878787UL;
	delay(10000);
	// PMIC->IRQ_OUT_POLAR = 0x2UL;
	// printf("rtc_irq_out_mode:%x\n\r", PMIC->IRQ_OUT_MODE);
	// printf("rtc_irq_out_polar:%x\n\r", PMIC->IRQ_OUT_POLAR);
	// printf("rtc_irq_out_time:%x\n\r", PMIC->IRQ_OUT_TIME);
	// printf("rtc_irq_in_mode:%x\n\r", PMIC->IRQ_IN_MODE);
	// printf("rtc_irq_io_cfg:%x\n\r", PMIC->IO_CFG);
	// RTC->RTC_WPR    = 0x5a5a5a5aUL;
	// PMIC->SOFT_REG0         = 0x5a5a5a5aUL;
	// printf("rtc_write_protect:%x\n\r", RTC->RTC_WPR);
	//RTC->INIT_TIME        = 0x80188UL;
	// RTC->ALARM_TIME         = 0x00000099UL;
	// RTC->ALARM_EN   = 0x2UL;
	// RTC->RTCSD          = 0xff0UL;
	// RTC->RTC_WPR    = 0x5a5a5a5aUL;
	// GIC_Enable();
	INT->INT_MASK   = 0x1UL;

}

void int_clr(void)
{
	INT->INT_CLR = 0x0;
	printf("irq in intr done: %ld\n", RTC->CUR_TIME);
}

void rtc_irqhandler(void)
{
	int_clr();
	printf("rtc_irqhandler done !!!\n");
}

int main (void)
{
	printf("test %s ...\n", __FILE__); 
	systimer_init();

    //open each PPU ISR
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,(0x1f<<PMU_ISR_PMIC_PWR_GOOD_LSB));

	printf("m3 core 0x%x = 0x%x\n", (DEBUG_REGS_RST), (unsigned int)REG32(DEBUG_REGS_RST));

	void irq_handler_gpio(void);
	void pmu_irqhandler(void);
	void rtc_irqhandler(void);

	if( REG32(DEBUG_REGS_RST) == 0){

		printf("m3 is cold boot\n");
		REG32(DEBUG_REGS_RST) = 0x5b5b5b00;

		systimer_delay(5, IN_S);
		printf("m3 delay 5\n");

#if GPIO_WAKEUP
		NVIC_SetPriority(GPIO0_IRQn, 0);
		NVIC_SetVector(GPIO0_IRQn, (unsigned int)(uintptr_t)irq_handler_gpio);
		NVIC_EnableIRQ(GPIO0_IRQn);
#endif
#if PMIC_WAKEUP
		NVIC_SetPriority(RTC_IRQn, 0);
		NVIC_SetVector(RTC_IRQn, (unsigned int)(uintptr_t)rtc_irqhandler);
		NVIC_EnableIRQ(RTC_IRQn);
#endif

		gic_cnt = 1;
	}else{
		printf("m3 is wakeup\n");
		// NVIC_SetPriority(PMU_IRQn, 0);
		// NVIC_SetVector(PMU_IRQn, (unsigned int)(uintptr_t)pmu_irqhandler);
		// NVIC_EnableIRQ(PMU_IRQn);

#if GPIO_WAKEUP
		NVIC_SetPriority(GPIO0_IRQn, 0);
		NVIC_SetVector(GPIO0_IRQn, (unsigned int)(uintptr_t)irq_handler_gpio);
		NVIC_EnableIRQ(GPIO0_IRQn);
#endif
#if PMIC_WAKEUP
		NVIC_SetPriority(RTC_IRQn, 0);
		NVIC_SetVector(RTC_IRQn, (unsigned int)(uintptr_t)rtc_irqhandler);
		NVIC_EnableIRQ(RTC_IRQn);
#endif
		systimer_delay(1, IN_S);
		gic_cnt = 2;
	}
    printf("NOTE gic_cnt is %d, err_cnt is %d\n", gic_cnt, err_cnt);

      if(gic_cnt == 1) {
#if GPIO_WAKEUP
		set_gpio_wakeup();
#endif
        set_pmu_wakeup(1, 0x2); //set wakeup gpio16 target:lp
        set_pmu_wakeup(3, 0x2); //set wakeup rtc target:lp
		DUMP_LOG(0);
		// systimer_delay(1, IN_S);
#if PMIC_WAKEUP
		i2c_wo_appower_enable(0);
		pmic_clear_irq_wo(1);
		pmic_to_sleep_delay(500);
		i2c_wo_start();
		rtc_init();
#endif
		set_power_off_seq();

		DUMP_LOG(1);
		printf("m3 is wfi !!\n");
		asm volatile("wfi");
      } else if(gic_cnt == 2) {
#if PMIC_WAKEUP
		pmic_clear_irq_wo(1);
#endif
		NVIC_DisableIRQ(GPIO0_IRQn);
        set_pmu_reg(PMU,PMU_PD_CR_NUM_PD_ADDR, 0); //clear PD_EN
		set_default_power_on_seq();
		// systimer_delay(1, IN_S);
		printf("m3 power on a55 core0 !!\n");
		set_power_on_a55(AP);
		set_power_on_a55(CORE0);
		// set_power_on_a55(CORE1);
		// set_power_on_a55(CORE2);
		// set_power_on_a55(CORE3);
      }


    if(err_cnt == 0) {
        TEST_PASS;
    } else {
        TEST_FAIL;
    }
    return 0;
}


// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
