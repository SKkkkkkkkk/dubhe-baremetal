#include <stdio.h>
#include "systimer.h"
#include "pmu.h"


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

int main(void)
{
  unsigned int pwsr = 1;
  printf("test %s ...\n", __FILE__); 
  systimer_init();

  // wakeup_core(3, core3_c_entry); //core3 enter to WFI
  // wakeup_core(2, core2_c_entry); //core2 enter to WFI
  // wakeup_core(1, core1_c_entry); //core1 enter to WFI
  // set_power_off_a55(AP);
  // systimer_delay(1, IN_S);
  // systimer_delay(1, IN_S);
  // systimer_delay(1, IN_S);
  // systimer_delay(1, IN_S);

  set_power_off_a55(CORE0);
  while(1) {
	  pwsr = get_pmu_reg(CORE0,PPU_PWPR_OP_DYN_EN_ADDR);
	  if(((pwsr >> PPU_PWPR_PWR_POLICY_LSB) & 0xf) == OFF) {
		printf("start enter wfi core0\n");
		break;
	  }
  }
  printf("poweroff core0\n");
  for(int i=0; i<4; i++) {
    asm volatile("nop");
  }
  asm volatile("wfi");
}
