#ifndef QEMU
#include <stdio.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
#include "pmu.h"

#define USE_GIC 1

int gic_pass = 0;
#if USE_GIC
    void pmu_irqhandler (void) {
        uint32_t tmp;
        uint32_t exp = 0x1961f;
        uint32_t exp_isr;
        printf("PMU interrupt occurred! gic_pass:%d\n",gic_pass);
        //IRQ_Disable(INT_PMU_INTR);
        while(1) {
            tmp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
            printf("read PMU_ISR:%x\n",tmp);
            if(gic_pass == 0) {
                if(((tmp >> PMU_ISR_PMIC_PWR_GOOD_LSB) & 0x1f) == 0x1f) {
                    printf("vd power good\n");
                    gic_pass++;
                    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,(0x1f<<PMU_ISR_PMIC_PWR_GOOD_LSB));
                    set_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR, (0x1f<<PMU_ISR_PMIC_PWR_GOOD_LSB));
                }
            }
            if(gic_pass == 1) {
                if((tmp & exp) == exp) {
                    printf("all default power on PD power up\n");
                    gic_pass++;
                    //check each pd ISR and clear PPU_ISR
                    for(int i=0; i<PDNUM; i++) {
                        if(((exp >> i) & 1) == 1) {
                            exp_isr = get_pmu_isr(i);
                            gic_pass += check_pmu_irq(i, exp_isr, 0xffffffff, 1);
                        }
                    }
                    //clear PMU ISR
                    set_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR,0xffffffff);
                }
            }
            break;
        }
    }
#endif
int main()
{
    printf("test %s ...\n", __FILE__);
	systimer_init();

    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, 0);
#if USE_GIC
	GIC_DistInit();
  	GIC_CPUInterfaceInit(); //per CPU

	void pmu_irqhandler(void);
	GIC_SetTarget(PMU_IRQn, 1 << 0);
	IRQ_SetHandler(PMU_IRQn, pmu_irqhandler);
	IRQ_SetPriority(PMU_IRQn, 0 << 3);
	IRQ_Enable(PMU_IRQn);
#endif

#if USE_GIC
  printf("nop...\n");
  while (gic_pass < 2) {
    asm volatile("nop");
  }
#else
    unsigned int temp;
    printf("wait for PMU_ISR assert ...\n");
    while(1) {
       temp = get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
       if(temp == get_pmu_isr(PMU)){
          printf("PMU default power up done\n");
          break;
       }
    }
#endif
    printf("start check each ppu result\n");
    uint32_t result;
    uint32_t golden;
    for(int i=0; i<17; i++) {
        golden = get_pmu_default_pwsr(i);
        result = get_pmu_reg(i,PPU_PWSR_OP_DYN_STATUS_ADDR);
        if(result == golden) {
            #if DEBUG_PMU
            printf("OK: PWSR[0x%x]:0x%x\n", i, result);
            #endif
        } else {
            printf("ERROR!! PWSR[0x%x]:0x%x, exp:0x%x\n", i, result, golden);
            gic_pass = 0xff;
        }
        golden = get_pmu_default_cr(i);
        result = get_pmu_reg(PMU,PMU_PD_CORE0_CR_PPU_PSTATE_ADDR+i*4);
        if(result == golden) {
            #if DEBUG_PMU
            printf("OK: CR[0x%x]:0x%x\n", i, result);
            #endif
        } else {
            printf("ERROR!! CR[0x%x]:0x%x, exp:0x%x\n", i, result, golden);
            gic_pass = 0xff;
        }
    }

    printf("gic_pass is %d\n", gic_pass);
    if(gic_pass == 2) {
        TEST_PASS;
    } else {
		systimer_delay(10, IN_US);
        TEST_FAIL;
    }

	return 0;
}

#else
#include "main_qemu.c"
#endif
