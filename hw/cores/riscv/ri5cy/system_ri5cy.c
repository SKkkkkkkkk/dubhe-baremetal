#include "ri5cy.h"


void system_init(void)
{
    _int_disable();
    // set_vector(0x00UL);
    int32_t IRQ_Initialize(void);
    IRQ_Initialize();
    _int_enable();
}
