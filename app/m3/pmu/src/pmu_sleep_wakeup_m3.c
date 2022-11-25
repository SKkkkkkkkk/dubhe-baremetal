#include <stdio.h>
#include "systimer.h"
#include "pmu.h"
#include "pinmux.h"
#include "dw_apb_gpio.h"

int gic_cnt = 0;
int err_cnt = 0;
int count = 0;
#define GPIO0                GPIO0_BASE

#define WAKEUP_PIN         0
#define WAKEUP_PIN_GROUP   (WAKEUP_PIN / 32)
#define WAKEUP_PIN_NUM     (WAKEUP_PIN % 32)

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
void rtc_wp_dis(void)
{
    RTC->WPR = 0x05060708UL;
    printf("%s[%d] RTC->WPR:%x\n",__FUNCTION__, __LINE__,RTC->WPR);
}

void rtc_wp_en(void)
{
    RTC->WPR = 0x08070605UL;
    printf("%s[%d] RTC->WPR:%x\n",__FUNCTION__, __LINE__,RTC->WPR);
}

void rtc_int_clr(void)
{
    (void)RTC->INT_CLR;
    printf("rtc_int_clear_done");
}

void rtc_init(void)
{
    rtc_wp_dis();
    RTC->CMR         = 0x8100UL;
    RTC->CLR         = 0x8000UL;
    RTC->CTRLR       = 0x0UL;
    RTC->CPSR        = 0x30UL;
    RTC->INT_EN      = 0x1UL;
    RTC->INT_MASK    = 0x3UL;
    udelay(1);
    RTC->CTRLR       = 0x17UL;
    udelay(1);
    printf("rtc_init_done");
}

void set_rtc_wakeup(void)
{
    NVIC_EnableIRQ(INT_RTC_INTR);
    rtc_init();
    rtc_wp_en();
}
#endif

void set_power_off_seq(void)
{
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_IMR_PMU_WAKEUP_2_MASK_LSB));
    set_pmu_reg(PMU,PMU_PDSEQ_0_LOGICID_ADDR,(PERI1 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_1_LOGICID_ADDR,(PERI0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_2_LOGICID_ADDR,(DDR1 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_3_LOGICID_ADDR,(DDR0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_4_LOGICID_ADDR,(CORE3 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_5_LOGICID_ADDR,(CORE2 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_6_LOGICID_ADDR,(CORE1 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_7_LOGICID_ADDR,(CORE0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_8_LOGICID_ADDR,(AP << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_9_LOGICID_ADDR,(LP << 8 | OFF));
    set_pmu_reg(PMU,PMU_PD_CR_NUM_PD_ADDR, (10 << 4 | 1));
}

void set_default_power_on_seq(void)
{
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_IMR_PPU_GPU_IRQ_MASK_LSB));
    set_pmu_reg(PMU,PMU_PUSEQ_0_LOGICID_ADDR,(PERI0 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_1_LOGICID_ADDR,(PERI1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_2_LOGICID_ADDR,(DDR0 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_3_LOGICID_ADDR,(DDR1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_4_LOGICID_ADDR,(AP << 8 | OP4 << 4 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_5_LOGICID_ADDR,(CORE0 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_6_LOGICID_ADDR,(CORE1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_7_LOGICID_ADDR,(CORE2 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_8_LOGICID_ADDR,(CORE3 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_9_LOGICID_ADDR,(VPU << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_10_LOGICID_ADDR,(NPU0 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_11_LOGICID_ADDR,(NPU1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_12_LOGICID_ADDR,(IMG << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_13_LOGICID_ADDR,(ISP << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_14_LOGICID_ADDR,(DEWARP << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_15_LOGICID_ADDR,(GPU << 8 | ON));
    set_pmu_reg(PMU,PMU_PU_CR_NUM_PD_ADDR, (16 << 4 | 1));
}

void pmu_irqhandler (void) {
    unsigned int exp;
    printf("NOTE PMU interrupt occurred! gic_cnt:%d",gic_cnt);
    //IRQ_Disable(INT_PMU_INTR);
    unsigned int tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
    printf("read PMU_ISR:%x",tmp);
    if(gic_cnt == 0) {
        //default power on interrupt
        exp = get_pmu_isr(PMU); //0x7d961f;
        if(((tmp >> PMU_IMR_PMU_WAKEUP_2_MASK_LSB) & 0x1) == 1) {
            gic_cnt = 1; //jump gic_cnt=2 power off operation
            exp = 0x203961f;
        }
        for(int i=0; i<20; i++) {
            if(get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR) == exp) {
                if(gic_cnt == 0) {
                    printf("NOTE all default power on PD power up");
                } else {
                    printf("NOTE enter power on LP");
                }
                break;
            }
        }
        //check each pd ISR and clear PPU_ISR
        for(int i=0; i<PDNUM; i++) {
            if(((exp >> i) & 1) == 1) {
                err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
            }
        }
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,0xffffffff);
    } else if(gic_cnt == 2) {
        exp = 0x1efff;
        if(tmp == exp) {
            printf("NOTE all PD power up again");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
                }
            }
        } else {
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

    int local_gic_cnt = gic_cnt;
    //open each PPU ISR
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,(0x1f<<PMU_ISR_PMIC_PWR_GOOD_LSB));

	void pmu_irqhandler(void);
	NVIC_SetPriority(PMU_IRQn, 0);
	NVIC_SetVector(PMU_IRQn, (unsigned int)(uintptr_t)pmu_irqhandler);
	NVIC_EnableIRQ(PMU_IRQn);

	void irq_handler_gpio(void);
	NVIC_SetPriority(GPIO0_IRQn, 0);
	NVIC_SetVector(GPIO0_IRQn, (unsigned int)(uintptr_t)irq_handler_gpio);
	NVIC_EnableIRQ(GPIO0_IRQn);

    printf("NOTE gic_cnt is %d, err_cnt is %d", gic_cnt, err_cnt);

    while (gic_cnt <= 2) {
      if(gic_cnt == 0) {
      } else if(gic_cnt == 1) {
        // set_rtc_wakeup();
		set_gpio_wakeup();
        set_pmu_wakeup(4); //set wakeup target:lp
        set_power_off_seq();
      } else if(gic_cnt == 2) {
        // rtc_int_clr();
		*((unsigned int *) (GPIO0 + 0x4c)) = 0xffffffff; //clear gpio interrupt
        set_pmu_reg(PMU,PMU_PD_CR_NUM_PD_ADDR, 0); //clear PD_EN
        set_default_power_on_seq();
      }
      while(1) {
        asm volatile("wfi");
        if(local_gic_cnt != gic_cnt) {
            break;
        }
      }
      local_gic_cnt = gic_cnt;
    }

    if(err_cnt == 0) {
        TEST_PASS;
    } else {
		systimer_delay(10, IN_US);
        TEST_FAIL;
    }
    return 0;
}


// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
