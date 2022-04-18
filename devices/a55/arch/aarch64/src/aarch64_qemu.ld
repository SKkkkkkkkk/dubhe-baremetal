OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(entrypoint)
/*
    0x00400000 0x00010000
    0x00a00000 0x00020000
*/
MEMORY {
    ROM (rx): ORIGIN = 0x0, LENGTH = 0x04000000 /*16384*32bit 64KB*/
    RAM (rwx): ORIGIN = 0x0e000000, LENGTH = 0x01000000 /*8192*128bit  128KB*/
}

_stack_size = 16*1024;
_heap_size = 8*1024;
SECTIONS
{
    . = 0x0;
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


    . = 0x0e000000;
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
        __BSS_END__ = .; 
    } >RAM
    ASSERT(__BSS_END__ == ALIGN(16), "__BSS_END__ address is not aligned on 16B.")

    .heap (NOLOAD): ALIGN(16)
    {
        __HEAP_START__ = .;
        end = __HEAP_START__;
        . = . + _heap_size ;
        __HEAP_END__ = .;
        __HEAPLIMIT = __HEAP_END__;
    }

    .stack (NOLOAD): 
    {
        __STACKS_START__ = .;
        . = . + _stack_size;
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
