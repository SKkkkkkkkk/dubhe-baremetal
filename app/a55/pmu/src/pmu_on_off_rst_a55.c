#ifndef QEMU
#include <stdio.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
#include "pmu.h"

int gic_cnt = 0;
int err_cnt = 0;
void pmu_irqhandler (void) {
    uint32_t exp;
    printf("PMU interrupt occurred! gic_cnt:%d",gic_cnt);
    //IRQ_Disable(INT_PMU_INTR);
    uint32_t tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
    printf("read PMU_ISR:%x",tmp);
    if(gic_cnt == 0) {
        //default power on interrupt
        exp = get_pmu_isr(PMU);
        if(tmp == exp) {
            printf("all default power on PD power up");
            //check each pd ISR and clear PPU_ISR
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i, get_pmu_isr(i), 0xffffffff, 1);
                    err_cnt += check_pmu_reg(i, PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_default_pwsr(i));
                }
            }
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
            printf("all off PD power up");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
                }
            }
        }
    } else if(gic_cnt == 2) {
        exp = 0x1 << PMU_ISR_PPU_DDR1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB;
        if((tmp & exp) == exp) {
            printf("PD rst done");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
                }
            }
        }
    } else if(gic_cnt == 3) {
        exp = 0x1 << PMU_ISR_PPU_PERI1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB;
        if((tmp & exp) == exp) {
            printf("PD power off done");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,OFF));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,OFF));
                }
            }
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

    printf("gic_cnt is %d, err_cnt is %d", gic_cnt, err_cnt);
    while (gic_cnt <= 3) {
      if(gic_cnt == 0) {
      } else if(gic_cnt == 1) {
        set_pmu_off_pd2on();
      } else if(gic_cnt == 2) {
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_ISR_PPU_DDR1_IRQ_LSB));
        set_pmu_reg(PERI0,PPU_PWPR_OP_DYN_EN_ADDR,RST);
        set_pmu_reg(DDR1,PPU_PWPR_OP_DYN_EN_ADDR,RST);
      } else if(gic_cnt == 3) {
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_ISR_PPU_DEWARP_IRQ_LSB));
        set_pmu_reg(PERI1,PPU_PWPR_OP_DYN_EN_ADDR,OFF);
        set_pmu_reg(PERI0,PPU_PWPR_OP_DYN_EN_ADDR,OFF);
        set_pmu_reg(LP,PPU_PWPR_OP_DYN_EN_ADDR,OFF);
        set_pmu_reg(DEWARP,PPU_PWPR_OP_DYN_EN_ADDR,OFF);
      }
      while(1) {
        asm volatile("nop");
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
}


#else
#include "main_qemu.c"
#endif

// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
