#ifdef A55
#ifndef __WAKEUP_CORE_H__
#define __WAKEUP_CORE_H__
#include <stdint.h>

uint32_t get_core_id();
void wakeup_core(uint8_t core_id, void* func);

#endif
#endif