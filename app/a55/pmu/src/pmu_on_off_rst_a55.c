#ifndef QEMU
#include <stdio.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
#include "pmu.h"
#include "_ca55_chip_define.h"

int gic_cnt = 0;
int err_cnt = 0;

// #define USE_SW_RST

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
        set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<CORE3));
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                               OP4 << PPU_PWPR_OP_POLICY_LSB |
                                                1 << PPU_PWPR_PWR_DYN_EN_LSB | ON));
        for(int i=CORE0; i<=CORE3; i++) {
            set_pmu_reg(i,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | ON ));
        }
    }
}

void isp_fw_rst(void)
{
      set_pmu_power_on(IMG);
      set_pmu_power_on(ISP);
      // REG32(ISP_BASE+ISP_IMSC) = 0X5;
      // printf("ISP.ISP_IMSC is set to 0x%x",REG32(ISP_BASE+ISP_IMSC));
      set_pmu_fw_warm_rst(ISP);
      // if(REG32(ISP_BASE+ISP_IMSC) != 0) {
          // printf("ERROR, ISP.ISP_IMSC is 0x%x, not rst",REG32(ISP_BASE+ISP_IMSC));
          // err_cnt++;
      // } else {
          // printf("ISP.ISP_IMSC is 0x%x, rsted",REG32(ISP_BASE+ISP_IMSC));
      // }
}

void pmu_irqhandler (void) {
    uint32_t exp;
    printf("PMU interrupt occurred! gic_cnt:%d\n",gic_cnt);
    //IRQ_Disable(INT_PMU_INTR);
    uint32_t tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
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
              0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB |
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
              0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB |
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
              0x1 << PMU_ISR_PPU_LP_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU0_IRQ_LSB |
              0x1 << PMU_ISR_PPU_NPU1_IRQ_LSB |
              // 0x1 << PMU_ISR_PPU_PERI0_IRQ_LSB |
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

void test_ddr_rst()
{
	int cnt = 0;

	REG32(0x60000000) = 0x12345678;
	printf("1 REG32(0x60000000) = 0x%x\n", REG32(0x60000000));

	set_pmu_warm_rst(DDR0);
	set_pmu_warm_rst(DDR1);

	printf("2 REG32(0x60000000) = 0x%x\n", REG32(0x60000000));
	  while(1) {
		systimer_delay(1, IN_S);
		cnt++;
		if(cnt % 10 == 0)
		printf("cnt 0x%x\n", cnt);
	  }
}

int main (void)
{   
    printf("test %s ...\n", __FILE__);
	systimer_init();

	int local_gic_cnt = gic_cnt;
	// open each PPU ISR
	set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,(0x1f<<PMU_ISR_PMIC_PWR_GOOD_LSB));

	GIC_DistInit();
	GIC_CPUInterfaceInit(); //per CPU

	void pmu_irqhandler(void);
	GIC_SetTarget(PMU_IRQn, 1 << 0);
	IRQ_SetHandler(PMU_IRQn, pmu_irqhandler);
	IRQ_SetPriority(PMU_IRQn, 0 << 3);
	IRQ_Enable(PMU_IRQn);

	printf("gic_cnt is %d, err_cnt is %d\n", gic_cnt, err_cnt);
	while (gic_cnt <= 4) {
	  if(gic_cnt == 0) {
	  } else if(gic_cnt == 1) {
		printf("///////////////open other pd\n");
		set_pmu_off_pd2on();
	  } else if(gic_cnt == 2) {
		printf("///////////////reset core1 - core3 pd\n");
		for(int i=0; i<PDNUM; i++) {
			if(i == CORE1 || i== CORE2 || i == CORE3){
				set_warm_rst_a55(i);
			}
		}
		printf("////////////reset other pd\n");
		set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_ISR_PPU_PERI1_IRQ_LSB));
		for(int i=0; i<PDNUM; i++) {
			if(i == CORE0 || i == CORE1 || i== CORE2 || i == CORE3 || i == AP || i == PERI0)
				continue;

#ifdef USE_SW_RST
			set_pmu_fw_warm_rst(i);
			check_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR, get_pmu_pwsr(i,ON+OP4));
			check_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4, get_pmu_cr(i,ON+OP4));
			gic_cnt = 3;
#else
			set_pmu_reg(i,PPU_PWPR_OP_DYN_EN_ADDR,RST);
#endif
		}
	  } else if(gic_cnt == 3) {
		printf("///////////////close other pd\n");
		set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_ISR_PPU_PERI1_IRQ_LSB));
		for(int i=0; i<PDNUM; i++) {
			if(i == CORE0 || i == CORE1 || i == CORE2 || i == CORE3 || i == AP || i == PERI0)
				continue;
			set_pmu_reg(i,PPU_PWPR_OP_DYN_EN_ADDR,OFF);
		}
	  } else if(gic_cnt == 4) {
		printf("///////////////open other pd\n");
		set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_ISR_PPU_PERI1_IRQ_LSB));
		for(int i=0; i<PDNUM; i++) {
			if(i == CORE0 || i == CORE1 || i == CORE2 || i == CORE3 || i == AP || i == PERI0)
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
