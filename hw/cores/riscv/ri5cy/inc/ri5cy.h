#ifndef __RI5CY_H__
#define __RI5CY_H__

#include <stdint.h>

static inline uint32_t _int_disable(void)
{
  uint32_t mstatus;
  asm volatile("csrrci %0, mstatus,0x01\n"
        : "=r"(mstatus));
  return mstatus;
}

static inline uint32_t _int_enable(void)
{
  uint32_t mstatus;
  asm volatile("csrrsi %0, mstatus,0x1\n"
        : "=r"(mstatus));
  return mstatus;
}

static inline void set_vector(uint32_t addr)
{
    // *((volatile uint32_t *)0x10000D00) = addr;
    // *((volatile uint32_t *)0x10000CFC) = 0x7E;
}

void system_init(void);


#endif /*__RI5CY_H__*/