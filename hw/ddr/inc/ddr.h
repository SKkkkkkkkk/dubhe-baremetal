#include <stdint.h>
#include "define_ddr_CTL.h"
#include "define_ddr_PHY.h"
#include "define_ddr_PI.h"

void init_ddr0_faster(void);
void init_ddr(void);
void init_ddr_training();
void SET_PARAM(uint32_t param_addr, int param_ofs, uint32_t param_sz, uint64_t param_val);

