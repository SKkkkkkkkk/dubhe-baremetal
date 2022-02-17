SEARCH_DIR(.)
__DYNAMIC  =  0;

MEMORY
{
    instrram    : ORIGIN = 0x00000000, LENGTH = 0x8000
    dataram     : ORIGIN = 0x00100000, LENGTH = 0x6000
    stack       : ORIGIN = 0x00106000, LENGTH = 0x2000
    bootrom     : ORIGIN = 0x00200000, LENGTH = 0x8000  /* bootrom, readonly */
}

/* Stack information variables */
_min_stack      = 0x1000;   /* 4K - minimum stack space to reserve */
_stack_len     = LENGTH(stack);
_stack_start   = ORIGIN(stack) + LENGTH(stack);

/* We have to align each sector to word boundaries as our current s19->slm
 * conversion scripts are not able to handle non-word aligned sections. */

INCLUDE "sections.ld"
