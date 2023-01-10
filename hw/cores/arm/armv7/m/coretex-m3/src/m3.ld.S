
#include "mem_and_clock.h"

#define ROM_AND_RAM 0
#define ALL_IN_SYSRAM	1
#define ALL_IN_DDR	2
#define MEMMAP_SCHEME_1 3 

#ifndef QEMU
  #ifndef WHERE_TO_LOAD
    #define WHERE_TO_LOAD ROM_AND_RAM
  #else
      #if (WHERE_TO_LOAD == ROM_AND_RAM)
      #elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
      #elif (WHERE_TO_LOAD == ALL_IN_DDR)
      #elif (WHERE_TO_LOAD == MEMMAP_SCHEME_1)
      #else
        #error "unspport WHERE_TO_LOAD."
      #endif
  #endif
#else
  #ifndef WHERE_TO_LOAD
    #define WHERE_TO_LOAD ROM_AND_RAM
  #else
      #if (WHERE_TO_LOAD == ROM_AND_RAM)
      #elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
      #elif (WHERE_TO_LOAD == ALL_IN_DDR)
        #error "unspport ALL_IN_DDR in qemu."
      #elif (WHERE_TO_LOAD == MEMMAP_SCHEME_1)
        #error "unspport MEMMAP_SCHEME_1 in qemu."
      #else
        #error "unspport WHERE_TO_LOAD."
      #endif
  #endif
#endif

MEMORY
{
  CODE (rx)  : ORIGIN = __CODE_BASE,         LENGTH = __CODE_SIZE
  SRAM (rwx) : ORIGIN = __SRAM_BASE,         LENGTH = __SRAM_SIZE
#ifndef QEMU
  DDR  (rwx) : ORIGIN = __EXTERNAL_RAM_BASE, LENGTH = __EXTERNAL_RAM_SIZE
#endif
}

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions CODE and SRAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
ENTRY(Reset_Handler)

SECTIONS
{
  .text :
  {
    KEEP(*(.vectors))
    *(.text*)

    KEEP(*(.init))
    KEEP(*(.fini))

    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    *(.rodata*)
    __tinyCLI_CMD_START__ = .;
    KEEP(*(.tinyCLI_CMD*))
    __tinyCLI_CMD_END__ = .;

    KEEP(*(.eh_frame*))
#if (WHERE_TO_LOAD == ROM_AND_RAM)
  } >CODE
#elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
	} >SRAM
#else // DDR
	} >DDR
#endif

  .ram :
  {
    . = ALIGN(4);
    __ram_start = .;
    *(.ram_vector*)
    *(.ram_func*)
    *(.ram_data*)
    __ram_end = .;
  } > SRAM

  /*
   * SG veneers:
   * All SG veneers are placed in the special output section .gnu.sgstubs. Its start address
   * must be set, either with the command line option �--section-start� or in a linker script,
   * to indicate where to place these veneers in memory.
   */
/*
  .gnu.sgstubs :
  {
    . = ALIGN(32);
  } > CODE
*/
  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
#if (WHERE_TO_LOAD == ROM_AND_RAM)
  } >CODE
#elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
	} >SRAM
#else // DDR
	} >DDR
#endif

  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
#if (WHERE_TO_LOAD == ROM_AND_RAM)
  } >CODE
#elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
	} >SRAM
#else // DDR
	} >DDR
#endif
  __exidx_end = .;

  .copy.table :
  {
    . = ALIGN(4);
    __copy_table_start__ = .;

    LONG (__etext)
    LONG (__data_start__)
    LONG ((__data_end__ - __data_start__) / 4)

    /* Add each additional data section here */
/*
    LONG (__etext2)
    LONG (__data2_start__)
    LONG ((__data2_end__ - __data2_start__) / 4)
*/
    __copy_table_end__ = .;
#if (WHERE_TO_LOAD == ROM_AND_RAM)
  } >CODE
#elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
	} >SRAM
#else // DDR
	} >DDR
#endif

  .zero.table :
  {
    . = ALIGN(4);
    __zero_table_start__ = .;
    LONG (__bss_start__)
    LONG ((__bss_end__ - __bss_start__) / 4)
    /* Add each additional bss section here */
/*
    LONG (__bss2_start__)
    LONG ((__bss2_end__ - __bss2_start__) / 4)
*/
    __zero_table_end__ = .;
#if (WHERE_TO_LOAD == ROM_AND_RAM)
  } >CODE
#elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
	} >SRAM
#else // DDR
	} >DDR
#endif

  /**
   * Location counter can end up 2byte aligned with narrow Thumb code but
   * __etext is assumed by startup code to be the LMA of a section in SRAM
   * which must be 4byte aligned 
   */
  __etext = ALIGN (4);

  .data : AT (__etext)
  {
    __data_start__ = .;
    *(vtable)
    *(.data)
    *(.data.*)

    . = ALIGN(4);
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    KEEP(*(.jcr*))
    . = ALIGN(4);
    /* All data end */
    __data_end__ = .;
#if (WHERE_TO_LOAD == ROM_AND_RAM)
  } >SRAM
#elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
	} >SRAM
#else // DDR
	} >DDR
#endif

  /*
   * Secondary data section, optional
   *
   * Remember to add each additional data section
   * to the .copy.table above to asure proper
   * initialization during startup.
   */
/*
  __etext2 = ALIGN (4);

  .data2 : AT (__etext2)
  {
    . = ALIGN(4);
    __data2_start__ = .;
    *(.data2)
    *(.data2.*)
    . = ALIGN(4);
    __data2_end__ = .;

  } > RAM2
*/

  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
#if (WHERE_TO_LOAD == ROM_AND_RAM)
  } >SRAM
#elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
	} >SRAM
#else // DDR
	} >DDR
#endif

  /*
   * Secondary bss section, optional
   *
   * Remember to add each additional bss section
   * to the .zero.table above to asure proper
   * initialization during startup.
   */
/*
  .bss2 :
  {
    . = ALIGN(4);
    __bss2_start__ = .;
    *(.bss2)
    *(.bss2.*)
    . = ALIGN(4);
    __bss2_end__ = .;
  } > RAM2 AT > RAM2
*/

  .heap (NOLOAD) :
  {
    . = ALIGN(8);
    __end__ = .;
    __HEAP_START__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
    __HEAP_END__ = .;
#if (WHERE_TO_LOAD == ROM_AND_RAM)
  } >SRAM
#elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
	} >SRAM
#elif (WHERE_TO_LOAD == ALL_IN_DDR)
	} >DDR
#else // MEMMAP_SCHEME_1
  } >SRAM
#endif


  .stack (NOLOAD) :
  {
    . = ALIGN(8);
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(8);
    __StackTop = .;
#if (WHERE_TO_LOAD == ROM_AND_RAM)
  } >SRAM
#elif (WHERE_TO_LOAD == ALL_IN_SYSRAM)
	} >SRAM
#elif (WHERE_TO_LOAD == ALL_IN_DDR)
	} >DDR
#else // MEMMAP_SCHEME_1
  } >SRAM
#endif

  PROVIDE(__stack = __StackTop);

  /* Check if data + heap + stack exceeds SRAM limit */
  ASSERT(__StackLimit >= __HeapLimit, "region SRAM overflowed with stack")
}
