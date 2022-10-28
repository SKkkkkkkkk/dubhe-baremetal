/*
 * The .data section gets copied from ROM to RAM at runtime.
 * Its LMA should be 16-byte aligned to allow efficient copying of 16-bytes
 * aligned regions in it.
 * Its VMA must be page-aligned as it marks the first read/write page.
 */
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(entrypoint)

 #define ROM_AND_RAM 0
 #define ALL_IN_RAM	 1
 #define ALL_IN_DDR	 2

#ifndef WHERE_TO_LOAD
	#define WHERE_TO_LOAD ROM_AND_RAM
#endif

#ifndef QEMU
	#define __ROM_BASE 0x00000000
	#define __ROM_SIZE 256K

	#define __RAM_BASE 0x20000000
	#define __RAM_SIZE 196K

	#define __DDR_BASE 0x40000000
	#define __DDR_SIZE 0xC0000000
#else
	#define __ROM_BASE 0x00000000
	#define __ROM_SIZE 64M

	#define __RAM_BASE 0x0e000000
	#define __RAM_SIZE 16M

    #define __DDR_BASE 0x40000000
	#define __DDR_SIZE 0xC0000000 //3G
#endif

#define __STACK_SIZE 0x4000 //16KB
#define __HEAP_SIZE  0x4000 //16KB
#define PAGE_SIZE	4096

MEMORY {
    ROM (rx): ORIGIN = __ROM_BASE, LENGTH = __ROM_SIZE
    RAM (rwx): ORIGIN = __RAM_BASE, LENGTH = __RAM_SIZE
    DDR (rwx): ORIGIN = __DDR_BASE, LENGTH = __DDR_SIZE
}

SECTIONS
{
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    . = ORIGIN(ROM);
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	. = ORIGIN(RAM);
#else // DDR
    . = ORIGIN(DDR);
#endif
    ASSERT(. == ALIGN(PAGE_SIZE), "RO_BASE address is not aligned on a page boundary.")
    .text . :
    {
        __TEXT_START__ = .;
        *entrypoint*(.text*)
        *(SORT_BY_ALIGNMENT(.text*))
        *(.vectors)
        . = ALIGN(((1) << (12)));
        __TEXT_END__ = .;
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    } >ROM
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	} >RAM
#else // DDR
	} >DDR
#endif

    .ARM.extab . : 
    {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    } >ROM
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	} >RAM
#else // DDR
	} >DDR
#endif

    .ARM.exidx . : 
    {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    } >ROM
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	} >RAM
#else // DDR
	} >DDR
#endif

    .rodata . : 
    {
        __RODATA_START__ = .;
        *(SORT_BY_ALIGNMENT(.rodata*))
        __RODATA_END__ = .;
        . = ALIGN(16); //.data LMA should be 16-byte aligned.
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    } >ROM
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	} >RAM
#else // DDR
	} >DDR
#endif



#if (WHERE_TO_LOAD == ROM_AND_RAM)
    . = ORIGIN(RAM);
	. = ALIGN(PAGE_SIZE);
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	. = ALIGN(PAGE_SIZE);
#else // DDR
	. = ALIGN(PAGE_SIZE);
#endif
    ASSERT(. == ALIGN(PAGE_SIZE), "RW_BASE address is not aligned on a page boundary.")
    .data . :
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    AT(ADDR(.rodata) + SIZEOF(.rodata))
#endif
    ALIGN(16)
    {
        __DATA_START__ = .; 
        *(SORT_BY_ALIGNMENT(.data*))
        . = ALIGN(16);
        __DATA_END__ = .; 
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    } >RAM
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	} >RAM
#else // DDR
	} >DDR
#endif

    __DATA_RAM_START__ = __DATA_START__;
    __DATA_RAM_END__ = __DATA_END__;
    ASSERT(__DATA_RAM_END__ == ALIGN(16), "__DATA_RAM_END__ address is not aligned on 16B.")

    .bss (NOLOAD) : ALIGN(16) 
    {
        __BSS_START__ = .;
        *(SORT_BY_ALIGNMENT(.bss*)) 
        *(COMMON)
        . = ALIGN(16);
        *(base_xlat_table)
        . = ALIGN(16);
        __BSS_END__ = .;
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    } >RAM
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	} >RAM
#else // DDR
	} >DDR
#endif
    ASSERT(__BSS_END__ == ALIGN(16), "__BSS_END__ address is not aligned on 16B.")

    .xlat_table (NOLOAD) : 
    {   __XLAT_TABLE_START__ = .; 
        *(xlat_table)   
        __XLAT_TABLE_END__ = .; 
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    } >RAM
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	} >RAM
#else // DDR
	} >DDR
#endif

    .heap (NOLOAD): ALIGN(16)
    {
        __HEAP_START__ = .;
        end = __HEAP_START__;
        . = . + __HEAP_SIZE;
        __HEAP_END__ = .;
        __HEAPLIMIT = __HEAP_END__;
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    } >RAM
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	} >RAM
#else // DDR
	} >DDR
#endif

    .stack (NOLOAD): ALIGN(16)
    {
        __STACKS_START__ = .;
        . = . + __STACK_SIZE;
        . = ALIGN(16);
        __STACKS_END__ = .;
        __STACK_TOP__ = __STACKS_END__;
#if (WHERE_TO_LOAD == ROM_AND_RAM)
    } >RAM
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
	} >RAM
#else // DDR
	} >DDR
#endif
    ASSERT(__STACK_TOP__ == ALIGN(16), "__STACK_TOP__ address is not aligned on 16B.")

    __RAM_START__ = ADDR(.data);
    __RAM_END__ = .;
    __DATA_ROM_START__ = LOADADDR(.data);
    __DATA_SIZE__ = SIZEOF(.data);
    __BSS_SIZE__ = SIZEOF(.bss);


#if (WHERE_TO_LOAD == ROM_AND_RAM)
    __ROM_END__ = __DATA_ROM_START__ + __DATA_SIZE__;
    ASSERT(__ROM_END__ <= (__ROM_BASE + __ROM_SIZE), "ROM content has exceeded its limit.")
#elif (WHERE_TO_LOAD == ALL_IN_RAM)
    ASSERT(__RAM_END__ <= (__RAM_BASE + __RAM_SIZE), "RW section has exceeded its limit.")
#else
    ASSERT(__RAM_END__ <= (__DDR_BASE + __DDR_SIZE), "RW section has exceeded its limit.")
#endif
    
}
