OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(entrypoint)
MEMORY {
    ROM (rx): ORIGIN = 0x00400000, LENGTH = 0x00010000 /*16384*32bit 64KB*/
    RAM (rwx): ORIGIN = 0x00a00000, LENGTH = 0x00020000 /*8192*128bit  128KB*/
}

_stack_size = 8*1024;
_heap_size = 0*1024;
SECTIONS
{
    . = 0x00400000;
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


    . = 0x00a00000;
    ASSERT(. == ALIGN(((1) << (12))), "BL1_RW_BASE address is not aligned on a page boundary.")
    .data . : ALIGN(16) 
    {
        __DATA_START__ = .; 
        *(SORT_BY_ALIGNMENT(.data*))
        . = ALIGN(8);
        __DATA_END__ = .; 
    } >RAM AT>ROM
    __DATA_RAM_START__ = __DATA_START__;
    __DATA_RAM_END__ = __DATA_END__;
    ASSERT(__DATA_RAM_END__ == ALIGN(8), "__DATA_RAM_END__ address is not aligned on 8B.")

    .bss (NOLOAD) : ALIGN(16) 
    { 
        __BSS_START__ = .;
        *(SORT_BY_ALIGNMENT(.bss*)) 
        *(COMMON)
        . = ALIGN(8);
        __BSS_END__ = .; 
    } >RAM
    ASSERT(__BSS_END__ == ALIGN(8), "__BSS_END__ address is not aligned on 8B.")

    .heap (NOLOAD):
    {
        . = ALIGN(8);
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
    /* ASSERT(__ROM_END__ <= (0x00000000 + 0x00020000), "BL1's ROM content has exceeded its limit.") */
    ASSERT(__ROM_END__ <= (0x00400000 + 0x00010000), "BL1's ROM content has exceeded its limit.")
    
    /* ASSERT(. <= (0x0e000000 + 0x01000000), "BL1's RW section has exceeded its limit.") */
    ASSERT(__RAM_END__ <= (0x00a00000 + 0x00020000), "BL1's RW section has exceeded its limit.")
}
