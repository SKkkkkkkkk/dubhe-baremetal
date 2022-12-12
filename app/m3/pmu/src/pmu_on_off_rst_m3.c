#ifndef QEMU
#include "m3.h"
#include <stdio.h>
#include "systimer.h"
#include "pmu.h"

int gic_cnt = 0;
int err_cnt = 0;

void set_warm_rst_a55(uint8_t pid)
{
    if((pid >= CORE0) && (pid <= CORE3)) {
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | RCV ));
        while(1) {
            if((get_pmu_reg(pid,PPU_PWSR_OP_DYN_STATUS_ADDR) & 0xf) == 0xa) {
                printf("core:%d rst done\n",pid);
                set_pmu_reg(pid,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                break;
            }
        }
        // set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<pid));
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | ON ));
    } else {
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, 0xffffffff);
        for(int i=CORE0; i<=CORE3; i++) {
            set_pmu_reg(i,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | RCV ));
			while(1) {
				if((get_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR) & 0xf) == 0xa) {
					printf("core:%d rst done\n",i);
					break;
				}
			}
        } 
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                               OP4 << PPU_PWPR_OP_POLICY_LSB |
                                                 1  << PPU_PWPR_PWR_DYN_EN_LSB | RCV));
        while(1) {
            if((get_pmu_reg(pid,PPU_PWSR_OP_DYN_STATUS_ADDR) & 0xf) == 0xa) {
                printf("cluster rst done\n");
                set_pmu_reg(CORE0,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                set_pmu_reg(CORE1,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                set_pmu_reg(CORE2,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                set_pmu_reg(CORE3,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                set_pmu_reg(pid,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
                break;
            }
        }
        // set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<CORE3));
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                               OP4 << PPU_PWPR_OP_POLICY_LSB |
                                                1 << PPU_PWPR_PWR_DYN_EN_LSB | ON));
        for(int i=CORE0; i<=CORE3; i++) {
            set_pmu_reg(i,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | ON ));
        }
    }
}

void pmu_irqhandler (void) {
    unsigned int exp;
    printf("PMU interrupt occurred! gic_cnt:%d\n",gic_cnt);
    //IRQ_Disable(INT_PMU_INTR);
    unsigned int tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
    printf("read PMU_ISR:%x\n",tmp);
    if(gic_cnt == 0) {
        //default power on interrupt
        exp = get_pmu_isr(PMU);
        if(tmp == exp) {
            printf("all default power on PD power up\n");
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
            printf("all off PD power up\n");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
                }
            }
        }
    } else if(gic_cnt == 2) {
        exp = //0x1 << PMU_ISR_PPU_CORE0_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE1_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE2_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE3_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CLUSTER_IRQ_LSB |
              0x1 << PMU_ISR_PPU_VPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_IMG_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_DDR0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DDR1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_PERI1_IRQ_LSB;

        if((tmp & exp) == exp) {
            printf("PD rst done\n");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
                }
            }
        }
    } else if(gic_cnt == 3) {
        exp = //0x1 << PMU_ISR_PPU_CORE0_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE1_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE2_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE3_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CLUSTER_IRQ_LSB |
              0x1 << PMU_ISR_PPU_VPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_IMG_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_DDR0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DDR1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_PERI1_IRQ_LSB;

        if((tmp & exp) == exp) {
            printf("PD power off done\n");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,OFF));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,OFF));
                }
            }
        }
    } else if(gic_cnt == 4) {
        exp = //0x1 << PMU_ISR_PPU_CORE0_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE1_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE2_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CORE3_IRQ_LSB |
			  // 0x1 << PMU_ISR_PPU_CLUSTER_IRQ_LSB |
              0x1 << PMU_ISR_PPU_VPU_IRQ_LSB |
              0x1 << PMU_ISR_PPU_IMG_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DEWARP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_ISP_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_DDR0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_DDR1_IRQ_LSB |
              0x1 << PMU_ISR_PPU_GPU_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
			  0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_PERI1_IRQ_LSB;

        if((tmp & exp) == exp) {
            printf("PD power on done\n");
            for(int i=0; i<PDNUM; i++) {
                if(((exp >> i) & 1) == 1) {
                    err_cnt += check_pmu_irq(i,get_pmu_isr(i),0xffffffff,1);
                    err_cnt += check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON));
                    err_cnt += check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON));
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

	void pmu_irqhandler(void);
	NVIC_SetPriority(PMU_IRQn, 0);
	NVIC_SetVector(PMU_IRQn, (unsigned int)(uintptr_t)pmu_irqhandler);
	NVIC_EnableIRQ(PMU_IRQn);

    printf("gic_cnt is %d, err_cnt is %d\n", gic_cnt, err_cnt);
    while (gic_cnt <= 4) {
      if(gic_cnt == 0) {
      } else if(gic_cnt == 1) {
		printf("///////////////open other pd\n");
        set_pmu_off_pd2on();
      } else if(gic_cnt == 2) {
		printf("///////////////reset core1 - core3 pd\n");
		set_warm_rst_a55(AP);
		printf("////////////reset other pd\n");
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_ISR_PPU_PERI1_IRQ_LSB));
        for(int i=0; i<PDNUM; i++) {
			if(i == LP || i == CORE0 || i == CORE1 || i == CORE2 || i == CORE3 || i == AP)
				continue;
			set_pmu_reg(i,PPU_PWPR_OP_DYN_EN_ADDR,RST);
		}
      } else if(gic_cnt == 3) {
		printf("///////////////close other pd\n");
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_ISR_PPU_PERI1_IRQ_LSB));
        for(int i=0; i<PDNUM; i++) {
			if(i == LP || i == CORE0 || i == CORE1 || i == CORE2 || i == CORE3 || i == AP)
				continue;
			set_pmu_reg(i,PPU_PWPR_OP_DYN_EN_ADDR,OFF);
		}
      } else if(gic_cnt == 4) {
		printf("///////////////open other pd\n");
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_ISR_PPU_PERI1_IRQ_LSB));
        for(int i=0; i<PDNUM; i++) {
			if(i == LP || i == CORE0 || i == CORE1 || i == CORE2 || i == CORE3 || i == AP)
				continue;
			set_pmu_reg(i,PPU_PWPR_OP_DYN_EN_ADDR,ON);
		}
      }
      while(1) {
        asm volatile("nop");
        if(local_gic_cnt != gic_cnt) {
            break;
        }
      }
	  systimer_delay(1, IN_S);
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
