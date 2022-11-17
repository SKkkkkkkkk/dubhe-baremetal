#line 100000
#include "pmu.h"
#include "regs_type.h"
//return string for pid name
char *get_pmu_name(uint8_t pid)
{
  switch(pid) {
    case CORE0  : return "CORE0";
    case CORE1  : return "CORE1";
    case CORE2  : return "CORE2";
    case CORE3  : return "CORE3";
    case AP     : return "AP";
    case VPU    : return "VPU";
    case IMG    : return "IMG";
    case DEWARP : return "DEWARP";
    case ISP    : return "ISP";
    case DDR0   : return "DDR0";
    case DDR1   : return "DDR1";
    case GPU    : return "GPU";
    case LP     : return "LP";
    case NPU0   : return "NPU0";
    case NPU1   : return "NPU1";
    case PERI0  : return "PERI0";
    case PERI1  : return "PERI1";
    default     : return "PMU";
  }
}

//return ppu base regaddr
int get_pmu_base(uint8_t pid)
{
  switch(pid) {
    case CORE0  : return PPU_CORE0_BASE;
    case CORE1  : return PPU_CORE1_BASE;
    case CORE2  : return PPU_CORE2_BASE;
    case CORE3  : return PPU_CORE3_BASE;
    case AP     : return PPU_AP_BASE;
    case VPU    : return PPU_VPU_BASE;
    case IMG    : return PPU_IMG_BASE;
    case DEWARP : return PPU_DEWARP_BASE;
    case ISP    : return PPU_ISP_BASE;
    case DDR0   : return PPU_DDR0_BASE;
    case DDR1   : return PPU_DDR1_BASE;
    case GPU    : return PPU_GPU_BASE;
    case LP     : return PPU_LP_BASE;
    case NPU0   : return PPU_NPU0_BASE;
    case NPU1   : return PPU_NPU1_BASE;
    case PERI0  : return PPU_PERI0_BASE;
    case PERI1  : return PPU_PERI1_BASE;
    default     : return PMU_REG_BASE;
  }
}

//return ppu isr states after trans done
int get_pmu_isr(uint8_t pid)
{
  int output;
  switch(pid) {
    case CORE0  : output = 0x83; break;
    case CORE1  : output = 0x83; break;
    case CORE2  : output = 0x83; break;
    case CORE3  : output = 0x83; break;
    case AP     : output = 0x83; break;
    case VPU    : output = 0x81; break;
    case IMG    : output = 0x81; break;
    case DEWARP : output = 0x81; break;
    case ISP    : output = 0x81; break;
    case DDR0   : output = 0x81; break;
    case DDR1   : output = 0x81; break;
    case GPU    : output = 0x81; break;
    case LP     : output = 0x81; break;
    case NPU0   : output = 0x81; break;
    case NPU1   : output = 0x81; break;
    case PERI0  : output = 0x81; break;
    case PERI1  : output = 0x81; break;
    default     : output = 0x7d961f; //PMU
  }
  return output;
}


int get_pmu_pwsr(uint8_t pid, uint8_t stat)
{
  int output;
  switch(pid) {
    case CORE0  : if(stat >= ON) {output = 0x108; } else {output = 0x100;} break;
    case CORE1  : if(stat >= ON) {output = 0x108; } else {output = 0x100;} break;
    case CORE2  : if(stat >= ON) {output = 0x108; } else {output = 0x100;} break;
    case CORE3  : if(stat >= ON) {output = 0x108; } else {output = 0x100;} break;
    case AP     : if(stat >= ON) {
                    if(stat == ON+OP0) {
                      output = 0x1000108;
                    } else if(stat == ON+OP1) {
                      output = 0x1010108;
                    } else if(stat == ON+OP2) {
                      output = 0x1020108;
                    } else if(stat == ON+OP3) {
                      output = 0x1030108;
                    } else if(stat == ON+OP4) {
                      output = 0x1040108;
                    }
                  } else {
                    output = 0x1000100;
                  } break;
    case VPU    : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case IMG    : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case DEWARP : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case ISP    : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case DDR0   : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case DDR1   : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case GPU    : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case LP     : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case NPU0   : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case NPU1   : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case PERI0  : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    case PERI1  : if(stat >= ON) {output = 0x8; } else {output = 0x0;} break;
    default     : output = 0xffffffff;
  }
  return output;
}


int get_pmu_default_pwsr(uint8_t pid)
{
  int output;
  switch(pid) {
    case CORE0  : output = get_pmu_pwsr(pid,ON); break;
    case CORE1  : output = get_pmu_pwsr(pid,ON); break;
    case CORE2  : output = get_pmu_pwsr(pid,ON); break;
    case CORE3  : output = get_pmu_pwsr(pid,ON); break;
    case AP     : output = get_pmu_pwsr(pid,ON+OP4); break;
    case VPU    : output = get_pmu_pwsr(pid,OFF); break;
    case IMG    : output = get_pmu_pwsr(pid,OFF); break;
    case DEWARP : output = get_pmu_pwsr(pid,OFF); break;
    case ISP    : output = get_pmu_pwsr(pid,OFF); break;
    case DDR0   : output = get_pmu_pwsr(pid,ON); break;
    case DDR1   : output = get_pmu_pwsr(pid,ON); break;
    case GPU    : output = get_pmu_pwsr(pid,OFF); break;
    case LP     : output = get_pmu_pwsr(pid,ON); break;
    case NPU0   : output = get_pmu_pwsr(pid,OFF); break;
    case NPU1   : output = get_pmu_pwsr(pid,OFF); break;
    case PERI0  : output = get_pmu_pwsr(pid,ON); break;
    case PERI1  : output = get_pmu_pwsr(pid,ON); break;
    default     : output = 0xffffffff;
  }
  return output;
}


int get_pmu_cr(uint8_t pid, uint8_t stat)
{
  int output;
  switch(pid) {
    case CORE0  : if(stat >= ON) {output = 0x20038;} else {output = 0x2c0;} break;
    case CORE1  : if(stat >= ON) {output = 0x20038;} else {output = 0x2c0;} break;
    case CORE2  : if(stat >= ON) {output = 0x20038;} else {output = 0x2c0;} break;
    case CORE3  : if(stat >= ON) {output = 0x20038;} else {output = 0x2c0;} break;
    case AP     : if(stat >= ON)
                  {
                    if(stat == ON+OP0) {
                      output = 0x2303a;
                    } else if (stat == ON+OP1) {
                      output = 0x6303a;
                    } else if (stat == ON+OP2) {
                      output = 0xa303a;
                    } else if (stat == ON+OP3) {
                      output = 0xe203a;
                    } else if (stat == ON+OP4) {
                      output = 0x12003a;
                    }
                  } else {
                    output = 0x123ec0;
                  }
                  break;
    case VPU    : if(stat >= ON) {output = 0x3a;} else {output = 0x2c0;} break;
    case IMG    : if(stat >= ON) {output = 0x3a;} else {output = 0x2c0;} break;
    case DEWARP : if(stat >= ON) {output = 0x3a;} else {output = 0x2c0;} break;
    case ISP    : if(stat >= ON) {output = 0x3a;} else {output = 0x2c0;} break;
    case DDR0   : if(stat >= ON) {output = 0x3a;} else {output = 0x3c0;} break;
    case DDR1   : if(stat >= ON) {output = 0x3a;} else {output = 0x3c0;} break;
    case GPU    : if(stat >= ON) {output = 0x3a;} else {output = 0x2c0;} break;
    case LP     : if(stat >= ON) {output = 0x3a;} else {output = 0x2c0;} break;
    case NPU0   : if(stat >= ON) {output = 0x3a;} else {output = 0x2c0;} break;
    case NPU1   : if(stat >= ON) {output = 0x3a;} else {output = 0x2c0;} break;
    case PERI0  : if(stat >= ON) {output = 0x3a;} else {output = 0x2c0;} break;
    case PERI1  : if(stat >= ON) {output = 0x3a;} else {output = 0x2c0;} break;
    default     : output = 0xffffffff;
  }
  return output;
}

int get_pmu_default_cr(uint8_t pid)
{
  int output;
  switch(pid) {
    case CORE0  : output = get_pmu_cr(CORE0, ON); break;
    case CORE1  : output = get_pmu_cr(CORE1, ON); break;
    case CORE2  : output = get_pmu_cr(CORE2, ON); break;
    case CORE3  : output = get_pmu_cr(CORE3, ON); break;
    case AP     : output = get_pmu_cr(AP, ON+OP4); break;
    case VPU    : output = get_pmu_cr(VPU, OFF); break;
    case IMG    : output = get_pmu_cr(IMG, OFF); break;
    case DEWARP : output = get_pmu_cr(DEWARP, OFF); break;
    case ISP    : output = get_pmu_cr(ISP, OFF); break;
    case DDR0   : output = get_pmu_cr(DDR0, ON); break;
    case DDR1   : output = get_pmu_cr(DDR1, ON); break;
    case GPU    : output = get_pmu_cr(GPU, OFF); break;
    case LP     : output = get_pmu_cr(LP, ON); break;
    case NPU0   : output = get_pmu_cr(NPU0, OFF); break;
    case NPU1   : output = get_pmu_cr(NPU1, OFF); break;
    case PERI0  : output = get_pmu_cr(PERI0, ON); break;
    case PERI1  : output = get_pmu_cr(PERI1, ON); break;
    default     : output = 0xffffffff;
  }
  return output;
}

void set_pmu_reg(uint8_t pid, uint32_t addr, uint32_t value)
{
  int reg_addr = addr + get_pmu_base(pid);
#if DEBUG_PMU
  printf("start set pid%0d addr 0x%x value 0x%x",pid,reg_addr,value);
#endif
  REG32(reg_addr) = value;
}

int get_pmu_reg(uint8_t pid, uint32_t addr)
{
  int reg_addr = addr + get_pmu_base(pid);
#if DEBUG_PMU
  printf("start get pid%0d addr 0x%x",pid,reg_addr);
#endif
  int val = REG32(reg_addr);
  return(val);
}

int check_pmu_reg(uint8_t pid, uint32_t addr, uint32_t value)
{
  int act = get_pmu_reg(pid,addr);
  if(act == value) {
#if DEBUG_PMU
    printf("OK! pid%0d check addr 0x%x value 0x%x pass",pid,addr,value);
#endif
    return 0;
  } else {
    printf("ERROR!! pid%0d check addr 0x%x value 0x%x, exp 0x%x",pid,addr,act,value);
    return 1;
  }
}
int check_pmu_irq(uint8_t pid, uint32_t isr, uint32_t aisr, uint8_t clr)
{
  uint32_t  fail = 0;
  uint32_t  rval;
  printf("start check ppu%d, isr:0x%x aisr:0x%x", pid, isr, aisr);
  if(isr != 0xffffffff) {
    rval = get_pmu_reg(pid,PPU_ISR_OTHER_IRQ_ADDR);
    if(isr != rval) {
      printf("ERROR!! get act isr:0x%x",rval);
      fail++;
    }
    if(clr > 0) {
      set_pmu_reg(pid,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
    }
  }
  if(aisr != 0xffffffff) {
    rval = get_pmu_reg(pid,PPU_AISR_UNSPT_POLICY_IRQ_ADDR);
    if(aisr != rval) {
      printf("ERROR!! get act aisr:0x%x",rval);
      fail++;
    }
    if(clr > 0) {
      set_pmu_reg(pid,PPU_AISR_UNSPT_POLICY_IRQ_ADDR,0xffffffff);
    }
  }
  return fail;
}

void set_pmu_off_pd2on()
{
  set_pmu_reg(PMU,PMU_IMR_PMU_WAKEUP_5_MASK_ADDR, ~(1<<PMU_ISR_PPU_NPU1_IRQ_LSB));
  set_pmu_reg(PMU,PMU_PUSEQ_0_LOGICID_ADDR,(VPU << 8 | ON));
  set_pmu_reg(PMU,PMU_PUSEQ_1_LOGICID_ADDR,(IMG << 8 | ON));
  set_pmu_reg(PMU,PMU_PUSEQ_2_LOGICID_ADDR,(DEWARP << 8 | ON));
  set_pmu_reg(PMU,PMU_PUSEQ_3_LOGICID_ADDR,(ISP << 8 | ON));
  set_pmu_reg(PMU,PMU_PUSEQ_4_LOGICID_ADDR,(GPU << 8 | ON));
  set_pmu_reg(PMU,PMU_PUSEQ_5_LOGICID_ADDR,(NPU0 << 8 | ON));
  set_pmu_reg(PMU,PMU_PUSEQ_6_LOGICID_ADDR,(NPU1 << 8 | ON));
  set_pmu_reg(PMU,PMU_PU_CR_NUM_PD_ADDR,   (7 << 4 | 1));
}

void set_pmu_wakeup(uint8_t target)
{
  set_pmu_reg(PMU,PMU_WK_SRC_0_M3_WAKEUP_ADDR,(target<<8 |0x30)); //spi0
  set_pmu_reg(PMU,PMU_WK_SRC_1_M3_WAKEUP_ADDR,(target<<8 |0x2f)); //spi1
  set_pmu_reg(PMU,PMU_WK_SRC_2_M3_WAKEUP_ADDR,(target<<8 |0x9));  //rtc
  set_pmu_reg(PMU,PMU_WK_SRC_3_M3_WAKEUP_ADDR,(target<<8 |0x36)); //uart3
  set_pmu_reg(PMU,PMU_WK_SRC_4_M3_WAKEUP_ADDR,(target<<8 |0x6e)); //gpio0_0
  set_pmu_reg(PMU,PMU_WK_SRC_5_M3_WAKEUP_ADDR,(target<<8 |0x6d)); //gpio0_1
}

void set_pmu_power_on(uint8_t pid)
{
  printf("NOTE : start power on pid:0x%x ...",pid);
  set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR, ON);
  while(1) {
    if(get_pmu_reg(pid,PPU_PWSR_OP_DYN_STATUS_ADDR) == get_pmu_pwsr(pid,ON)) {
      printf("NOTE : finish power on pid:0x%x",pid);
      break;
    }
  }
}


void set_pmu_power_off(uint8_t pid)
{
  printf("NOTE : start power off pid:0x%x ...",pid);
  set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR, OFF);
  while(1) {
    if(get_pmu_reg(pid,PPU_PWSR_OP_DYN_STATUS_ADDR) == get_pmu_pwsr(pid,OFF)) {
      printf("NOTE : finish power off pid:0x%x",pid);
      break;
    }
  }
}


void set_pmu_warm_rst(uint8_t pid)
{
  printf("NOTE : start warm rst pid:0x%x ...",pid);
  set_pmu_reg(pid,PPU_PWPR_OP_DYN_EN_ADDR, RST);
  while(1) {
    if((get_pmu_reg(pid,PPU_ISR_OTHER_IRQ_ADDR) & 0x1) == 1) {
      printf("NOTE : finish warm rst pid:0x%x",pid);
      break;
    }
  }
}

void clear_all_ppu_isr(void)
{
  for(int i=0; i<PDNUM; i++) {
    set_pmu_reg(i,PPU_ISR_OTHER_IRQ_ADDR,0xffffffff);
  }
}
