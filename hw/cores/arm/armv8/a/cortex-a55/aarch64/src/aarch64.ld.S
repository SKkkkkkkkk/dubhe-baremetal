/*
 * The .data section gets copied from ROM to RAM at runtime.
 * Its LMA should be 16-byte aligned to allow efficient copying of 16-bytes
 * aligned regions in it.
 * Its VMA must be page-aligned as it marks the first read/write page.
 */
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(entrypoint)


#ifndef QEMU
	#define __ROM_BASE 0x00000000
	#define __ROM_SIZE 0x00010000 //64KB

	#define __RAM_BASE 0x00A00000
	#define __RAM_SIZE 0x00020000 //128KB
#else
	#define __ROM_BASE 0x00000000
	#define __ROM_SIZE 0x04000000 //64MB

	#define __RAM_BASE 0x0e000000
	#define __RAM_SIZE 0x01000000 //16MB
#endif

#define __STACK_SIZE 0x4000 //16KB
#define __HEAP_SIZE  0x4000 //16KB
#define PAGE_SIZE	4096

MEMORY {
    ROM (rx): ORIGIN = __ROM_BASE, LENGTH = __ROM_SIZE /*16384*32bit 64KB*/
    RAM (rwx): ORIGIN = __RAM_BASE, LENGTH = __RAM_SIZE /*8192*128bit  128KB*/
}

SECTIONS
{
    . = ORIGIN(ROM);
    ASSERT(. == ALIGN(((1) << (12))), "BL1_RO_BASE address is not aligned on a page boundary.")
    .text . :
    {
        __TEXT_START__ = .;
        *entrypoint*(.text*)
        *(SORT_BY_ALIGNMENT(.text*))
        *(.vectors)
        . = ALIGN(((1) << (12)));
        __TEXT_END__ = .;
    } >ROM
    .ARM.extab . : 
    {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    } >ROM
    .ARM.exidx . : 
    {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } >ROM
    .rodata . : 
    {
        __RODATA_START__ = .;
        *(SORT_BY_ALIGNMENT(.rodata*))
        __RODATA_END__ = .;
        . = ALIGN(16);
    } >ROM


    . = ORIGIN(RAM);
    ASSERT(. == ALIGN(((1) << (12))), "BL1_RW_BASE address is not aligned on a page boundary.")
    .data . : ALIGN(16) 
    {
        __DATA_START__ = .; 
        *(SORT_BY_ALIGNMENT(.data*))
        . = ALIGN(16);
        __DATA_END__ = .; 
    } >RAM AT>ROM
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
    } >RAM
    ASSERT(__BSS_END__ == ALIGN(16), "__BSS_END__ address is not aligned on 16B.")

    .xlat_table (NOLOAD) : { *(xlat_table) } >RAM

    .heap (NOLOAD): ALIGN(16)
    {
        __HEAP_START__ = .;
        end = __HEAP_START__;
        . = . + __HEAP_SIZE;
        __HEAP_END__ = .;
        __HEAPLIMIT = __HEAP_END__;
    } >RAM

    .stack (NOLOAD): ALIGN(16)
    {
        __STACKS_START__ = .;
        . = . + __STACK_SIZE;
        . = ALIGN(16);
        __STACKS_END__ = .;
        __STACK_TOP__ = __STACKS_END__;
    } >RAM
    ASSERT(__STACK_TOP__ == ALIGN(16), "__STACK_TOP__ address is not aligned on 16B.")

    __RAM_START__ = ADDR(.data);
    __RAM_END__ = .;
    __DATA_ROM_START__ = LOADADDR(.data);
    __DATA_SIZE__ = SIZEOF(.data);
    __ROM_END__ = __DATA_ROM_START__ + __DATA_SIZE__;
    __BSS_SIZE__ = SIZEOF(.bss);
    
    ASSERT(__ROM_END__ <= (0x00000000 + 0x04000000), "ROM content has exceeded its limit.")
    ASSERT(. <= (0x0e000000 + 0x01000000), "RW section has exceeded its limit.")
}
