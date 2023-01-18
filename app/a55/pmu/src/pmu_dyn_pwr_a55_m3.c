#include <stdio.h>
#include "systimer.h"
#include "wakeup_core.h"
#include "irq_ctrl.h"
#include "gic.h"
#include "pmu.h"

#define ALL_SECOND_CORE_OFF				1
#define ALL_SECOND_CORE_ON_OFF			1
#define CORE0_SELF_OFF					0
#define AP_OP_MODE_TEST					1

void set_opmode_change_a55(uint8_t stat)
{   
    set_pmu_reg(AP,PPU_IMR_STA_DENY_IRQ_MASK_ADDR,0xffffffff);
    set_pmu_reg(AP,PPU_AIMR_LPIP_TIMEOUT_IRQ_MASK_ADDR,0xffffffff);
    set_pmu_reg(AP,PPU_PWPR_OP_DYN_EN_ADDR,(1 << PPU_PWPR_OP_DYN_EN_LSB |
                                           stat << PPU_PWPR_OP_POLICY_LSB |
                                             1 << PPU_PWPR_PWR_DYN_EN_LSB | ON));
    while(1) {
        uint32_t val = get_pmu_reg(AP,PPU_ISR_OTHER_IRQ_ADDR);
        if((val & 0x1) == 0x1) {
            printf("AP change to opmode 0x%x done\n",stat);
            break;
        }
    }
}

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
	// printf("core1 0x2e000ff4 = 0x%x\n", REG32(0x2e000ff4));
	if( REG32(0x2e000ff4) == 0){
		REG32(0x2e000ff4) = 0x5a5a5a01;
		set_power_off_a55(CORE1);
		uint32_t pwsr = 1;
		while(1) {
			pwsr = get_pmu_reg(CORE1,PPU_PWPR_OP_DYN_EN_ADDR);
			if(((pwsr >> PPU_PWPR_PWR_POLICY_LSB) & 0xf) == OFF) {
				printf("start enter wfi core1\n");
				break;
			}
		}
		POWER_OFF_SEQ();
	}else{
		printf("core1 is wakeup\n");
	}
    while(1);
}

void core2_c_entry(void)
{
    printf("hello_world core2\n");
	// printf("core2 0x2e000ff8 = 0x%x\n", REG32(0x2e000ff8));
	if( REG32(0x2e000ff8) == 0){
		REG32(0x2e000ff8) = 0x5a5a5a02;
		set_power_off_a55(CORE2);
		uint32_t pwsr = 1;
		while(1) {
			pwsr = get_pmu_reg(CORE2,PPU_PWPR_OP_DYN_EN_ADDR);
			if(((pwsr >> PPU_PWPR_PWR_POLICY_LSB) & 0xf) == OFF) {
				printf("start enter wfi core2\n");
				break;
			}
		}
		POWER_OFF_SEQ();
	}else{
		printf("core2 is wakeup\n");
	}
    while(1);
}

void core3_c_entry(void)
{
    printf("hello_world core3\n");
	// printf("core3 0x2e000ffc = 0x%x\n", REG32(0x2e000ffc));
	if( REG32(0x2e000ffc) == 0){
		REG32(0x2e000ffc) = 0x5a5a5a03;
		set_power_off_a55(CORE3);
		uint32_t pwsr = 1;
		while(1) {
			pwsr = get_pmu_reg(CORE3,PPU_PWPR_OP_DYN_EN_ADDR);
			if(((pwsr >> PPU_PWPR_PWR_POLICY_LSB) & 0xf) == OFF) {
				printf("start enter wfi core3\n");
				break;
			}
		}
		POWER_OFF_SEQ();
	}else{
		printf("core3 is wakeup\n");
	}
    while(1);
}

int main(void)
{
  uint32_t pwsr = 1;
  printf("test %s ...\n", __FILE__); 
  systimer_init();

  if( REG32(0x2e000ff0) == 0){
	  REG32(0x2e000ff0) = 0x5a5a5a00;
	  wakeup_core(3, core3_c_entry); //core3 enter to WFI
	  systimer_delay(500, IN_MS);
	  wakeup_core(2, core2_c_entry); //core2 enter to WFI
	  systimer_delay(500, IN_MS);
	  wakeup_core(1, core1_c_entry); //core1 enter to WFI
	  systimer_delay(500, IN_MS);

	  printf("auto power off\n");
	  systimer_delay(500, IN_MS);

	  set_power_off_a55(AP);
	  set_power_off_a55(CORE0);

	  while(1) {
		  pwsr = get_pmu_reg(CORE0,PPU_PWPR_OP_DYN_EN_ADDR);
		  if(((pwsr >> PPU_PWPR_PWR_POLICY_LSB) & 0xf) == OFF) {
			  printf("start enter wfi core0\n");
			  break;
		  }
	  }
	  printf("poweroff core0\n");
	  POWER_OFF_SEQ(); //core0 enter WFI
	  for(int i=0; i<4; i++) {
		  asm volatile("nop");
	  }
	  asm volatile("wfi");
  }else{
	  printf("core0 is wakeup!!!\n");

	  systimer_delay(500, IN_MS);
	  wakeup_core(3, core3_c_entry); //core3 enter to WFI
	  systimer_delay(500, IN_MS);
	  wakeup_core(2, core2_c_entry); //core2 enter to WFI
	  systimer_delay(500, IN_MS);
	  wakeup_core(1, core1_c_entry); //core1 enter to WFI
	  systimer_delay(500, IN_MS);
	  printf("PASS!!!\n");
  }
}
