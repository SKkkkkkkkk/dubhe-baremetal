OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv",
	      "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

/* CORE-V */
MEMORY
{
  external_ram (rwx) : ORIGIN = 0x00000000, LENGTH = 512M
  dram (rw) :          ORIGIN = 0xf0000000, LENGTH = 64K
}

SECTIONS
{
  . = ORIGIN(external_ram);
  /* interrupt vectors */
  .vectors (ORIGIN(external_ram)):
  {
    PROVIDE(__vector_start = .);
    KEEP(*(.vectors));
  } >external_ram

  /* crt0 init code */
  PROVIDE(__boot_address = ORIGIN(external_ram)+0x80);
  .init (__boot_address):
  {
    KEEP (*(SORT_NONE(.init)))
    KEEP (*(.text.start))
  } >external_ram

  .text           :
  {
    *(.text.unlikely .text.*_unlikely .text.unlikely.*)
    *(.text.exit .text.exit.*)
    *(.text.startup .text.startup.*)
    *(.text.hot .text.hot.*)
    *(SORT(.text.sorted.*))
    *(.text .stub .text.* .gnu.linkonce.t.*)
    /* .gnu.warning sections are handled specially by elf.em.  */
    *(.gnu.warning)
  } >external_ram

  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);

  .rodata         : 
  {
    *(.rodata .rodata.* .gnu.linkonce.r.*) 
  } >external_ram

  .data           :
  {
    __DATA_BEGIN__ = .;
    *(.data .data.* .gnu.linkonce.d.*)
    SORT(CONSTRUCTORS)
  } >external_ram
  .sdata          :
  {
    __SDATA_BEGIN__ = .;
    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  } >external_ram

  _edata = .; PROVIDE (edata = .);

  . = ORIGIN(dram);
  __bss_start = .;
  .sbss           :
  {
    *(.dynsbss)
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
  } >dram
  .bss            :
  {
   *(.dynbss)
   *(.bss .bss.* .gnu.linkonce.b.*)
   *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections.
      FIXME: Why do we need it? When there is no .bss section, we do not
      pad the .data section.  */
   . = ALIGN(. != 0 ? 32 / 8 : 1);
  } >dram
  . = ALIGN(32 / 8);
  . = SEGMENT_START("ldata-segment", .);
  . = ALIGN(32 / 8);
  __bss_end = .;
  _end = .; PROVIDE (end = .);

  /* Heap grows upward towards end of dram */
  .heap                    : ALIGN(16)
  {
   PROVIDE(__heap_start = .);
   /* If end of dram is not 16-byte aligned, align to previous 16-byte
      boundary */
   PROVIDE(__heap_end = ALIGN(ORIGIN(dram) + LENGTH(dram) - __heap_start - 15, 16));
   . = __heap_end;
  } >dram

  /* Stack grows downward from end of dram */
  .stack (__heap_start)    : ALIGN(16) /* this is a requirement of the ABI(?) */
  {
   PROVIDE(__stack_start = __heap_start);
   . = __heap_end;
   PROVIDE(__stack_end = .);
  } >dram
}

