#include "m3.h"
#include <stdio.h>
#include "systimer.h"
#include "pmu.h"
#include "pinmux.h"
#include "dw_apb_gpio.h"

int gic_cnt = 0;
int err_cnt = 0;
int count = 0;
#define GPIO0                GPIO0_BASE

#define WAKEUP_PIN			16
#define WAKEUP_PIN_GROUP	(WAKEUP_PIN / 32)
#define WAKEUP_PIN_NUM		(WAKEUP_PIN % 32)
#define DEBUG_REGS			(0x4e000ee0)
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

#if 0
void rtc_wp_en(void)
{
    RTC->PMIC_WRITE_PROTECT = 0x5a5a5a5aUL;
    seehi_printf("%s[%d] RTC->PMIC_WRITE_PROTECT:%x\n",__FUNCTION__, __LINE__,RTC->PMIC_WRITE_PROTECT);
}

void rtc_int_clr(void)
{
    RTC->INT_RTCIRQ = 0x0UL;
    printf("rtc_int_clear_done");
}

void set_rtc_wakeup(void)
{
    NVIC_EnableIRQ(INT_RTC_INTR);
    RTC->RTC_WRITE_PROTECT = 0x5a5a5a5aUL;
    RTC->ALARM_CLOCK_TIME = 0x5UL; //5s
    RTC->ALARM_CLOCK_TO_SOC_EN = 0x1UL; //interrupt to soc
    RTC->INT_MASK              = 0x2UL; //alarm interrupt
    seehi_printf("rtc_init_done");
}
#endif

void set_power_off_seq(void)
{
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_IMR_PMU_WAKEUP_1_MASK_LSB | 1 << PMU_IMR_PPU_LP_IRQ_MASK_LSB));
    set_pmu_reg(PMU,PMU_PDSEQ_0_LOGICID_ADDR,(PERI0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_1_LOGICID_ADDR,(DDR1 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_2_LOGICID_ADDR,(DDR0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_3_LOGICID_ADDR,(LP << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_0_LOGICID_ADDR,(PERI1 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_4_LOGICID_ADDR,(CORE3 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_5_LOGICID_ADDR,(CORE2 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_6_LOGICID_ADDR,(CORE1 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_7_LOGICID_ADDR,(CORE0 << 8 | OFF));
    // set_pmu_reg(PMU,PMU_PDSEQ_8_LOGICID_ADDR,(AP << 8 | OFF));
    set_pmu_reg(PMU,PMU_PD_CR_NUM_PD_ADDR, (4 << 4 | 1));
}

void set_default_power_on_seq(void)
{
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_IMR_PPU_GPU_IRQ_MASK_LSB));
    set_pmu_reg(PMU,PMU_PUSEQ_0_LOGICID_ADDR,(PERI0 << 8 | ON));
    //set_pmu_reg(PMU,PMU_PUSEQ_1_LOGICID_ADDR,(PERI1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_1_LOGICID_ADDR,(DDR0 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_2_LOGICID_ADDR,(DDR1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_3_LOGICID_ADDR,(VPU << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_4_LOGICID_ADDR,(NPU0 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_5_LOGICID_ADDR,(NPU1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_6_LOGICID_ADDR,(IMG << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_7_LOGICID_ADDR,(ISP << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_8_LOGICID_ADDR,(DEWARP << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_9_LOGICID_ADDR,(GPU << 8 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_4_LOGICID_ADDR,(AP << 8 | OP4 << 4 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_5_LOGICID_ADDR,(CORE0 << 8 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_6_LOGICID_ADDR,(CORE1 << 8 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_7_LOGICID_ADDR,(CORE2 << 8 | ON));
    // set_pmu_reg(PMU,PMU_PUSEQ_8_LOGICID_ADDR,(CORE3 << 8 | ON));
    set_pmu_reg(PMU,PMU_PU_CR_NUM_PD_ADDR, (10 << 4 | 1));
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



int main (void)
{
	printf("test %s ...\n", __FILE__); 
	systimer_init();

    //open each PPU ISR
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,(0x1f<<PMU_ISR_PMIC_PWR_GOOD_LSB));

	printf("m3 core 0x%x = 0x%x\n", (DEBUG_REGS + 0x1c), (unsigned int)REG32(DEBUG_REGS + 0x1c));

	void irq_handler_gpio(void);
	void pmu_irqhandler(void);

	if( REG32(DEBUG_REGS + 0x1c) == 0){

		printf("m3 is cold boot\n");
		REG32(DEBUG_REGS + 0x1c) = 0x5b5b5b00;
		REG32(DEBUG_REGS) = 0;

		systimer_delay(5, IN_S);
		printf("m3 delay 5\n");

		// uint32_t tmp;
		// uint32_t exp;

		// exp = get_pmu_isr(PMU);
		// tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
		// while(tmp != exp){
			// tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
		// }

		// for(int i=0; i<PDNUM; i++) {
			// if(((exp >> i) & 1) == 1) {
				// err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
				// err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_default_pwsr(i));
			// }
		// }
        // set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,0xffffffff);

		// NVIC_SetPriority(PMU_IRQn, 0);
		// NVIC_SetVector(PMU_IRQn, (unsigned int)(uintptr_t)pmu_irqhandler);
		// NVIC_EnableIRQ(PMU_IRQn);

		NVIC_SetPriority(GPIO0_IRQn, 0);
		NVIC_SetVector(GPIO0_IRQn, (unsigned int)(uintptr_t)irq_handler_gpio);
		NVIC_EnableIRQ(GPIO0_IRQn);

		gic_cnt = 1;
	}else{
		printf("m3 is wakeup\n");
		// NVIC_SetPriority(PMU_IRQn, 0);
		// NVIC_SetVector(PMU_IRQn, (unsigned int)(uintptr_t)pmu_irqhandler);
		// NVIC_EnableIRQ(PMU_IRQn);

		NVIC_SetPriority(GPIO0_IRQn, 0);
		NVIC_SetVector(GPIO0_IRQn, (unsigned int)(uintptr_t)irq_handler_gpio);
		NVIC_EnableIRQ(GPIO0_IRQn);
		systimer_delay(1, IN_S);
		gic_cnt = 2;
	}
    printf("NOTE gic_cnt is %d, err_cnt is %d\n", gic_cnt, err_cnt);

      if(gic_cnt == 1) {
        // set_rtc_wakeup();
		set_gpio_wakeup();
        set_pmu_wakeup(1, 0x2); //set wakeup target:lp
		DUMP_LOG(0);
		systimer_delay(1, IN_S);
		set_power_off_seq();
		DUMP_LOG(1);
		printf("m3 is wfi !!\n");
        asm volatile("wfi");
      } else if(gic_cnt == 2) {
        // rtc_int_clr();
		NVIC_DisableIRQ(GPIO0_IRQn);
        set_pmu_reg(PMU,PMU_PD_CR_NUM_PD_ADDR, 0); //clear PD_EN
		set_default_power_on_seq();
		systimer_delay(1, IN_S);
		printf("m3 power on a55 core0 !!\n");
		set_power_on_a55(AP);
		set_power_on_a55(CORE0);
      }


    if(err_cnt == 0) {
        TEST_PASS;
    } else {
		systimer_delay(1, IN_S);
        TEST_FAIL;
    }
    return 0;
}


// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
