#include <stdio.h>
#include "systimer.h"
#include "pmu.h"

int gic_cnt = 0;
int err_cnt = 0;

void set_power_off_a55(uint8_t pid)
{
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<pid));
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
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<pid));
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

void pmu_irqhandler (void) {
    unsigned int exp;
    printf("NOTE PMU interrupt occurred! gic_cnt:%d",gic_cnt);
    unsigned int cnt = 0;
    if(gic_cnt == 0) {
        //default power on interrupt
        exp = get_pmu_isr(PMU); //0x7d961f;
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,0xffffffff);
    } else if(gic_cnt == 1) {
        exp = 1 << PMU_ISR_PPU_CORE3_IRQ_LSB;
    } else if(gic_cnt == 2) {
        exp = 1 << PMU_ISR_PPU_CORE2_IRQ_LSB;
    } else if(gic_cnt == 3) {
        exp = 1 << PMU_ISR_PPU_CORE1_IRQ_LSB;
    } else if(gic_cnt == 4) {
        exp = 1 << PMU_ISR_PPU_CORE0_IRQ_LSB;
    } else if(gic_cnt == 5) {
        exp = 1 << PMU_ISR_PPU_CLUSTER_IRQ_LSB;
    } else if(gic_cnt == 6) {
        exp = 1 << PMU_ISR_PPU_CLUSTER_IRQ_LSB;
    } else if(gic_cnt == 7) {
        exp = 1 << PMU_ISR_PPU_CORE0_IRQ_LSB;
    } else if(gic_cnt == 8) {
        exp = 1 << PMU_ISR_PPU_CORE1_IRQ_LSB;
    } else if(gic_cnt == 9) {
        exp = 1 << PMU_ISR_PPU_CORE2_IRQ_LSB;
    } else if(gic_cnt == 10) {
        exp = 1 << PMU_ISR_PPU_CORE3_IRQ_LSB;
    }
    while(get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR) != exp) {
        if(cnt >= 50) {
            printf("ERROR!! PMU_ISR not match exp:0x%x",exp);
            err_cnt++;
            break;
        }
        cnt++;
    }
    //check each pd ISR and clear PPU_ISR
    for(int i=0; i<PDNUM; i++) {
        if(((exp >> i) & 1) == 1) {
            //err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
            unsigned int isr = 0;
            unsigned int aisr = 0;
            cnt = 0;
            while((isr != 1) && (aisr != 1)) {
                isr = get_pmu_reg(i,PPU_ISR_OTHER_IRQ_ADDR);
                aisr = get_pmu_reg(i,PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR);
                if(cnt >= 100) {
                    printf("ERROR!! PPU %d_ISR 0x%x AISR 0x%x still not done",i,isr,aisr);
                    err_cnt++;
                    break;
                }
                cnt++;
                isr &= 0x1;
                aisr = (aisr >> 1) & 0x1;
            }
            set_pmu_reg(i,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
            if((gic_cnt > 0) && (gic_cnt < 6)) {
                unsigned int pwsr;
                for(int i=0; i<20; i++) {
                    pwsr = get_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR);
                    if((pwsr & 0xf) == OFF) break;
                }
                if((pwsr & 0xf) != OFF) err_cnt++;
            } else if(gic_cnt >= 6) {
                err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
            }
        }
    }
    gic_cnt++;
    set_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR,0xffffffff);
}


int main (void)
{
    int local_gic_cnt = gic_cnt;

    //open each PPU ISR
	void pmu_irqhandler(void);
	NVIC_SetPriority(PMU_IRQn, 0);
	NVIC_SetVector(PMU_IRQn, (unsigned int)(uintptr_t)pmu_irqhandler);
	NVIC_EnableIRQ(PMU_IRQn);

    printf("NOTE gic_cnt is %d, err_cnt is %d", gic_cnt, err_cnt);

    while (gic_cnt <= 10) {
      if(gic_cnt == 0) {
      } else if(gic_cnt == 1) {
        set_power_off_a55(CORE3);
      } else if(gic_cnt == 2) {
        set_power_off_a55(CORE2);
      } else if(gic_cnt == 3) {
        set_power_off_a55(CORE1);
      } else if(gic_cnt == 4) {
        set_power_off_a55(CORE0);
      } else if(gic_cnt == 5) {
        set_power_off_a55(AP);
      } else if(gic_cnt == 6) {
        set_power_on_a55(AP);
      } else if(gic_cnt == 7) {
        set_power_on_a55(CORE0);
      } else if(gic_cnt == 8) {
        set_power_on_a55(CORE1);
      } else if(gic_cnt == 9) {
        set_power_on_a55(CORE2);
      } else if(gic_cnt == 10) {
        set_power_on_a55(CORE3);
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
