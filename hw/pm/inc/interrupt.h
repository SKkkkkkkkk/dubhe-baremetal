/*
 */

#ifndef _SYS_INTERRUPT_H_
#define _SYS_INTERRUPT_H_

#include "compiler.h"

/*
 * CPU interrupt mask handling.
 */

#define IRQMASK_REG_NAME_R "primask"
#define IRQMASK_REG_NAME_W "primask"

/* Save the current interrupt enable state and disable IRQ */
static __always_inline unsigned long arch_irq_save(void)
{
    unsigned long flags;

    __asm volatile("mrs	%0, " IRQMASK_REG_NAME_R "\n"
                   "cpsid	i"
                   : "=r"(flags)
                   :
                   : "memory", "cc");
    return flags;
}

/* Restore saved IRQ state */
static __always_inline void arch_irq_restore(unsigned long flags)
{
    __asm volatile("msr	" IRQMASK_REG_NAME_W ", %0"
                   :
                   : "r"(flags)
                   : "memory", "cc");
}

/* Get the current interrupt enable state */
static __always_inline unsigned long arch_irq_get_flags(void)
{
    unsigned long flags;

    __asm volatile("mrs	%0, " IRQMASK_REG_NAME_R "\n"
                   : "=r"(flags)
                   :
                   : "memory", "cc");
    return flags;
}

/* Disable IRQ, non-nested version */
#define arch_irq_disable() __asm volatile("cpsid i" : : : "memory", "cc")

/* Enable IRQ, non-nested version */
#define arch_irq_enable()  __asm volatile("cpsie i" : : : "memory", "cc")

/* Disable FIQ, non-nested version */
#define arch_fiq_disable() __asm volatile("cpsid f" : : : "memory", "cc")

/* Enable FIQ, non-nested version */
#define arch_fiq_enable()  __asm volatile("cpsie f" : : : "memory", "cc")

#endif /* _SYS_INTERRUPT_H_ */
