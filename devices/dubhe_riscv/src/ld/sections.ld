SECTIONS
{
    .text.bootrom.align.start : { . = ALIGN(4); PROVIDE ( __bootrom_text_start = . ); } > bootrom
    .text.bootrom : {
        . = ALIGN(4);
        PROVIDE(__vector_start = .);
        KEEP (*(.vectors.bootrom))
        KEEP (*(.text.bootrom.start))
        KEEP(*(.text.bootrom))
        . = ALIGN(4);
    } > bootrom
    .text.bootrom.align.end : { . = ALIGN(4); PROVIDE ( __bootrom_text_end = . ); } > bootrom
	PROVIDE(__bootrom_text_size = SIZEOF(.text.bootrom));

    .text.align.start : { . = ALIGN(4); PROVIDE ( __text_start = . ); } > instrram
    .text : {
        . = ALIGN(4);
        PROVIDE(__vector_start = .);
        KEEP (*(.vectors))
        KEEP (*(SORT_NONE(.init)))
        KEEP (*(.text.start))
        _stext = .;
        *(.text*)
        _etext  =  .;

        PROVIDE (etext = .);
        
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array ))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array ))
        PROVIDE_HIDDEN (__fini_array_end = .);

        __CTOR_LIST__ = .;
        LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
        *(.ctors)
        LONG(0)
        __CTOR_END__ = .;
        __DTOR_LIST__ = .;
        LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
        *(.dtors)
        LONG(0)
        __DTOR_END__ = .;
        *(.lit)
        *(.shdata)
        _endtext = .;
    }  > instrram
    .text.align.end : { . = ALIGN(4); PROVIDE ( __text_end = . ); } > instrram
    PROVIDE(__text_size = SIZEOF(.text));

    .data.align.start : { . = ALIGN(4); PROVIDE ( __data_start = . ); } > dataram
    .data : 
    {
      . = ALIGN(4);
      *(.rodata);
      *(.rodata.*)

      . = ALIGN(4);
      *(.shbss)

      . = ALIGN(4);
      __global_pointer$ = . + 0x800;
      sdata  =  .;
      _sdata  =  .;
      *(.sdata);
      *(.sdata.*)
      *(.data);
      *(.data.*)
      edata  =  .;
      _edata  =  .;
    } > dataram
    .data.align.end : { . = ALIGN(4); PROVIDE ( __data_end = . ); } > dataram
    PROVIDE(__data_size = SIZEOF(.data));

    .bss :
    {
        . = ALIGN(4);
        _bss_start = .;
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)
        _bss_end = .;
        __bss_end = .;
    } > dataram
    /* heap: we should consider putting this to the bottom of the address space */
    .heap (NOLOAD):
    {
        PROVIDE(__heap_start = .);
        . = ORIGIN(dataram) + LENGTH(dataram);
        PROVIDE(__heap_end = .);
    } > dataram

    /* ensure there is enough room for stack */
    .stack (NOLOAD): {
        . = ALIGN(4);
        PROVIDE(__stack_start = .);
        . = . + _min_stack ;
        . = ALIGN(4);
        PROVIDE(_sp = .);
        PROVIDE(__stack_end = .);
        stack = . ;
        _stack = . ;
    } > stack

    .stab  0 (NOLOAD) :
    {
        [ .stab ]
    }

    .stabstr  0 (NOLOAD) :
    {
        [ .stabstr ]
    }

    .bss :
    {
        . = ALIGN(4);
        _end = .;
    } > dataram
}