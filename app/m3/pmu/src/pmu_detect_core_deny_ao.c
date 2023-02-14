#include "m3.h"
#include <stdio.h>
#include "systimer.h"
#include "pmu.h"

int cpu_deny_cnt[4] = {0};
int gic_cnt = 0;
int err_cnt = 0;

void delay(int ms)
{
	for(int i=0; i< ms; i++)
		for(int j = 0; j< 1000; j++)
			for(int z = 0; z< 100; z++){}
}
void set_power_off_a55_while(uint8_t pid)
{
    int try_cnt=0;
	int pwsr,isr;
	int aisr;

	pwsr = get_pmu_reg(pid, PPU_PWSR_OP_DYN_STATUS_ADDR);
	isr = get_pmu_reg(pid, PPU_ISR_OTHER_IRQ_ADDR);
	aisr = get_pmu_reg(pid, PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR);

    while(1) {
        while((pwsr != 0x108) || (((isr >> 2) & 0x1) != 1) || (((aisr >> 2) & 0x1) != 1)) {
			pwsr = get_pmu_reg(pid, PPU_PWSR_OP_DYN_STATUS_ADDR);
            isr = get_pmu_reg(pid, PPU_ISR_OTHER_IRQ_ADDR);
			aisr = get_pmu_reg(pid, PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR);

			try_cnt++;
			if(try_cnt %100000 == 0){
				printf("try_cnt %d  cpu id %d pwsr 0x%x isr 0x%x aisr 0x%x\n", try_cnt, pid, pwsr, isr, aisr);
			}
			// systimer_delay(100, IN_MS);
			// delay(1);
			
        }
        // if(pwsr == 0x100) break;

        if(pid == AP) {
            set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                                   OP0 << PPU_PWPR_OP_POLICY_LSB |
                                                     1 << PPU_PWPR_PWR_DYN_EN_LSB | OFF));
        } else {
			set_pmu_reg(pid,PPU_ISR_OTHER_IRQ_ADDR, 0xffffffff);
			set_pmu_reg(pid,PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR,0xffffffff);
			printf("set a55 core0 OFF start\n");
			printf("try_cnt %d  cpu id %d pwsr 0x%x isr 0x%x aisr 0x%x\n", try_cnt, pid, pwsr, isr, aisr);
            set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | OFF));
			printf("set a55 core0 OFF end\n");
			pwsr = 0;
			isr = 0;
			aisr = 0;
        }
    }
}

void set_power_off_a55(uint8_t pid)
{
    int try_cnt=1;
	int pwsr,isr;
	int aisr;

	set_pmu_reg(pid,PPU_ISR_OTHER_IRQ_ADDR, 0xffffffff);
	set_pmu_reg(pid,PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR,0xffffffff);
    while(1) {
        if(pid == AP) {
            set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                                   OP0 << PPU_PWPR_OP_POLICY_LSB |
                                                     1 << PPU_PWPR_PWR_DYN_EN_LSB | OFF));
        } else {
            set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | OFF));
        }

		pwsr = get_pmu_reg(pid, PPU_PWSR_OP_DYN_STATUS_ADDR);
		isr = get_pmu_reg(pid, PPU_ISR_OTHER_IRQ_ADDR);
		aisr = get_pmu_reg(pid, PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR);
		printf("try_cnt %d  cpu id %d pwsr 0x%x isr 0x%x aisr 0x%x\n", try_cnt, pid, pwsr, isr, aisr);

        while((pwsr == 0x108) || (((isr >> 2) & 0x1) == 1) || (((aisr >> 2) & 0x1) == 1)) {
			pwsr = get_pmu_reg(pid, PPU_PWSR_OP_DYN_STATUS_ADDR);
            isr = get_pmu_reg(pid, PPU_ISR_OTHER_IRQ_ADDR);
			aisr = get_pmu_reg(pid, PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR);

			printf("try_cnt %d  cpu id %d pwsr 0x%x isr 0x%x aisr 0x%x\n", try_cnt, pid, pwsr, isr, aisr);
			set_pmu_reg(pid,PPU_ISR_OTHER_IRQ_ADDR, 0xffffffff);
			set_pmu_reg(pid,PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR,0xffffffff);
            set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | OFF));
			try_cnt++;
			pwsr = get_pmu_reg(pid, PPU_PWSR_OP_DYN_STATUS_ADDR);
            isr = get_pmu_reg(pid, PPU_ISR_OTHER_IRQ_ADDR);
			aisr = get_pmu_reg(pid, PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR);
        }
		printf("pwsr 0x%x break\n", pwsr);
		if(pwsr == 0x100) break;

    }
}

void set_power_on_a55(uint8_t pid)
{
    if(pid == AP) {
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                               OP4 << PPU_PWPR_OP_POLICY_LSB |
                                                 1 << PPU_PWPR_PWR_DYN_EN_LSB | ON));
    } else {
        set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_PWR_DYN_EN_LSB | ON));
    }
}


void pmu_irqhandler (void) {
    int pmu_isr;
    int ppu_pwpr;
    int ppu_disr;
    pmu_isr =  get_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR);
	printf("pmu_isr 0x%x\n", pmu_isr);
    printf("M3-error! cpu0_deny_cnt:%d, cpu1_deny_cnt:%d, cpu2_deny_cnt:%d, cpu3_deny_cnt:%d, gic_cnt:%d\n",
            cpu_deny_cnt[0], cpu_deny_cnt[1], cpu_deny_cnt[2], cpu_deny_cnt[3], gic_cnt);
    for (int id=0; id<4; id++) {
        if(((pmu_isr >> id) & 0x1) == 0x1) {
            cpu_deny_cnt[id]++;
            ppu_pwpr = get_pmu_reg(id, PPU_PWPR_OP_DYN_EN_ADDR);
            ppu_disr = get_pmu_reg(id, PPU_DISR_OP_DEVACTIVE_STATUS_ADDR);
            if((ppu_pwpr == 0x100)
             &&(ppu_disr == 0x0)) {
                set_power_off_a55(id);
            } else {
                printf("M3-error! cpu%0d: pwpr:0x%x, disr:0x%x, isr:0x%x, aisr:0x%x\n", id, ppu_pwpr, ppu_disr,  
                        get_pmu_reg(id, PPU_ISR_OTHER_IRQ_ADDR), get_pmu_reg(id, PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR));
            }
            set_pmu_reg(id,PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR,0xffffffff);
        }
    }
    gic_cnt++;
    set_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR,0xffffffff);
}


int main (void)
{
	printf("test %s ...\n", __FILE__); 
	// systimer_init();

	NVIC_SetPriority(PMU_IRQn, 0);
	NVIC_SetVector(PMU_IRQn, (unsigned int)(uintptr_t)pmu_irqhandler);
	// NVIC_EnableIRQ(PMU_IRQn);

    //open core AIMR
    for(int id=0; id<4; id++) {
		set_pmu_reg(id,PPU_IMR_STA_DENY_IRQ_MASK_ADDR, 0xffffffff);
		set_pmu_reg(id,PPU_AIMR_LPIP_TIMEOUT_IRQ_MASK_ADDR, 0xfffffffb);
		set_pmu_reg(id,PPU_ISR_OTHER_IRQ_ADDR, 0xffffffff);
		set_pmu_reg(id,PPU_AISR_LPIP_TIMEOUT_IRQ_ADDR,0xffffffff);
    }
    //clear PMU_ISR
    set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR,0xfffffff0);
    set_pmu_reg(PMU,PMU_ISR_PMU_WAKEUP_5_ADDR,0xffffffff);

    //open each PPU ISR
	printf("start\n");
	NVIC_EnableIRQ(PMU_IRQn);
	printf("end\n");

    while (1) {
		// for (int id=0; id<4; id++) {
			// set_power_off_a55_while(0);
		// }
    }
    return 0;
}


// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
