#include <stdio.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
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

void POWER_OFF_SEQ(void)
{
   __asm(
         "mrs x0, s3_0_c15_c2_7\n"
         "orr x0, x0, #0x1\n"
         "msr s3_0_c15_c2_7, x0\n"
         "isb\n"
         "wfi\n" );
}

void core1_c_entry(void)
{
    printf("hello_world core1\n");
    POWER_OFF_SEQ();
    while(1);
}

void core2_c_entry(void)
{
    printf("hello_world core2\n");
    POWER_OFF_SEQ();
    while(1);
}

void core3_c_entry(void)
{
    printf("hello_world core3\n");
    POWER_OFF_SEQ();
    while(1);
}

void irq_handler_gpio(void)
{
    uint32_t intstatus = *((uint32_t *) (GPIO0 + 0x40));

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
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_IMR_PMU_WAKEUP_4_MASK_LSB |
                                                      1<<PMU_IMR_PPU_CLUSTER_IRQ_MASK_LSB));
    set_pmu_reg(PMU,PMU_PDSEQ_0_LOGICID_ADDR,(NPU1 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_1_LOGICID_ADDR,(NPU0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_2_LOGICID_ADDR,(DEWARP << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_3_LOGICID_ADDR,(ISP << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_4_LOGICID_ADDR,(IMG << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_5_LOGICID_ADDR,(VPU << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_6_LOGICID_ADDR,(GPU << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_7_LOGICID_ADDR,(PERI1 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_8_LOGICID_ADDR,(PERI0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_9_LOGICID_ADDR,(LP << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_10_LOGICID_ADDR,(DDR1 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_11_LOGICID_ADDR,(DDR0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_12_LOGICID_ADDR,(CORE3 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_13_LOGICID_ADDR,(CORE2 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_14_LOGICID_ADDR,(CORE1 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_15_LOGICID_ADDR,(CORE0 << 8 | OFF));
    set_pmu_reg(PMU,PMU_PDSEQ_16_LOGICID_ADDR,(AP << 8 | OFF));
    set_pmu_reg(PMU,PMU_PD_CR_NUM_PD_ADDR, (17 << 4 | 1));
}

void set_default_power_on_seq(void)
{
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_IMR_PPU_DDR1_IRQ_MASK_LSB));
    set_pmu_reg(PMU,PMU_PUSEQ_0_LOGICID_ADDR,(LP << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_1_LOGICID_ADDR,(PERI0 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_2_LOGICID_ADDR,(PERI1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_3_LOGICID_ADDR,(DDR0 << 8 | ON));
    set_pmu_reg(PMU,PMU_PUSEQ_4_LOGICID_ADDR,(DDR1 << 8 | ON));
    set_pmu_reg(PMU,PMU_PU_CR_NUM_PD_ADDR, (5 << 4 | 1));
}

void pmu_irqhandler (void) {
    uint32_t exp;
    printf("NOTE PMU interrupt occurred! gic_cnt:%d\n",gic_cnt);
    //IRQ_Disable(INT_PMU_INTR);
    uint32_t tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
    printf("read PMU_ISR:%x\n",tmp);
    if(gic_cnt == 0) {
        //default power on interrupt
        exp = get_pmu_isr(PMU);
        if(((tmp >> PMU_IMR_PMU_WAKEUP_4_MASK_LSB) & 0x1) == 1) {
            gic_cnt = 2; //jump gic_cnt=2 power off operation
            clear_all_ppu_isr();
        } else if(tmp == exp) {
            printf("NOTE all default power on PD power up\n");
            //check each pd ISR and clear PPU_ISR
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
                    err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_default_pwsr(i));
                }
            }
        } else {
            err_cnt++;
        }
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,0xffffffff);
    } else if(gic_cnt == 1) {
        exp = 0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_IMG_IRQ_LSB |
              0x1 << PMU_ISR_PPU_VPU_IRQ_LSB;
        if((tmp & exp) == exp) {
            printf("NOTE all off PD power up\n");
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
    } else if(gic_cnt == 3) {
        exp = 0x19600;//get_pmu_isr(PMU);
        if(tmp == exp) {
            printf("NOTE all default power on PD power up again\n");
            for(int i=0; i<PDNUM; i++) {
                if(i == AP || i == CORE0 || i == CORE1 || i == CORE2 || i == CORE3) {
                    err_cnt += check_pmu_irq(i,0x0,0xffffffff,0);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
                } else if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
                } else {
                    err_cnt += check_pmu_irq(i,0x0,0xffffffff,0);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,OFF));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,OFF));
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

	GIC_DistInit();
  	GIC_CPUInterfaceInit(); //per CPU

	void pmu_irqhandler(void);
	GIC_SetTarget(PMU_IRQn, 1 << 0);
	IRQ_SetHandler(PMU_IRQn, pmu_irqhandler);
	IRQ_SetPriority(PMU_IRQn, 0 << 3);
	IRQ_Enable(PMU_IRQn);

	void irq_handler_gpio(void);
	GIC_SetTarget(GPIO0_IRQn, 1 << 0);
	IRQ_SetHandler(GPIO0_IRQn, irq_handler_gpio);
	IRQ_SetPriority(GPIO0_IRQn, 0 << 3);
	IRQ_Enable(GPIO0_IRQn);

    printf("NOTE gic_cnt is %d, err_cnt is %d\n", gic_cnt, err_cnt);

    while (gic_cnt <= 3) {
      if(gic_cnt == 0) {
      } else if(gic_cnt == 1) {
        set_pmu_off_pd2on();
      } else if(gic_cnt == 2) {
        set_pmu_reg(PMU,PMU_PU_CR_NUM_PD_ADDR, 0); //clear PU_EN
        set_gpio_wakeup();
        set_pmu_wakeup(3); //set wakeup target:a55+pmic
        wakeup_core(3, core3_c_entry); //core3 enter to WFI
        wakeup_core(2, core2_c_entry); //core2 enter to WFI
        wakeup_core(1, core1_c_entry); //core1 enter to WFI
        set_power_off_seq();
        // seehi_cmd(0xff000000);
        POWER_OFF_SEQ(); //core0 enter WFI
      } else if(gic_cnt == 3) {
		*((uint32_t *) (GPIO0 + 0x4c)) = 0xffffffff; //clear gpio interrupt
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
        printf("ERROR err_cnt is %d\n", err_cnt);
		systimer_delay(10, IN_US);
        TEST_FAIL;
    }
    return 0;
}


// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
