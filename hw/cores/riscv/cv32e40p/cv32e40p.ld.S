/* Script for -z combreloc */
/* Copyright (C) 2014-2020 Free Software Foundation, Inc.
   Copyright (C) 2019 ETH Zürich and University of Bologna
   Copyright (C) 2020 OpenHW Group
   Copying and distribution of this script, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  */

/* This linker script is adapted from the default linker script for upstream
   RISC-V GCC.  It has been modified for use in verification of CORE-V cores.
*/

OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv",
	      "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

/* CORE-V */
MEMORY
{
	/* Our testbench is a bit weird in that we initialize the RAM (thus
	   allowing initialized sections to be placed there). Infact we dump all
	   sections to external_ram. */
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

  . = ORIGIN(dram);
  .rodata         : 
  {
    *(.rodata .rodata.* .gnu.linkonce.r.*) 
  } >dram

  .data           :
  {
    __DATA_BEGIN__ = .;
    *(.data .data.* .gnu.linkonce.d.*)
    SORT(CONSTRUCTORS)
  } >dram
  .sdata          :
  {
    __SDATA_BEGIN__ = .;
    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  } >dram

  _edata = .; PROVIDE (edata = .);
  . = .;
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

