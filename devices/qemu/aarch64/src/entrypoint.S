#include <arch.h>
#include <asm_macros.S>

	.globl entrypoint
func entrypoint

	// mrs x0, tcr_el3
	// mrs x1, tcr_el2
	// mrs x2, tcr_el1

	// mrs x0, sctlr_el3 //0x30c50830
	ldr x0, =0x30c50830 
	msr sctlr_el3, x0
	isb

	ldr x0, =vector_table
	msr vbar_el3, x0
	isb

	mrs x1, mpidr_el1 //0x80000000
	and x1, x1, 0xff //x1 => core_id
	cmp x1, #0
	bne secondary_core

	// bl init_general_regs
	mov x0, XZR
	mov x1, XZR
	mov x2, XZR
	mov x3, XZR
	mov x4, XZR
	mov x5, XZR
	mov x6, XZR
	mov x7, XZR
	mov x8, XZR
	mov x9, XZR
	mov x10, XZR
	mov x11, XZR
	mov x12, XZR
	mov x13, XZR
	mov x14, XZR
	mov x15, XZR
	mov x16, XZR
	mov x17, XZR
	mov x18, XZR
	mov x19, XZR
	mov x20, XZR
	mov x21, XZR
	mov x22, XZR
	mov x23, XZR
	mov x24, XZR
	mov x25, XZR
	mov x26, XZR
	mov x27, XZR
	mov x28, XZR
	mov x29, XZR
	mov x30, XZR

	mov	x1, #0x100a //(SCTLR_I_BIT | SCTLR_A_BIT | SCTLR_SA_BIT)
	mrs	x0, sctlr_el3
	orr	x0, x0, x1
	bfi x0, xzr, #1, #1 // clear bit1
	msr	sctlr_el3, x0
	isb

	mrs x0, scr_el3
	mov x0, #0x238                 	// #568
	msr scr_el3, x0

	mrs x0, daif
	mrs x1, currentel
	mrs x2, spsel

	msr daifclr, #0x7	//enable syserror,irq,fiq

	bl setup_stack

	ldr	x0, =__BSS_START__
	ldr	x1, =__BSS_END__
clear_loop:
	str	xzr, [x0], #8
	cmp	x0, x1
	b.lo	clear_loop

	ldr x0, =__DATA_RAM_START__
	ldr x1, =__DATA_ROM_START__
	ldr x2, =__DATA_SIZE__
data_init_start:
	cbz x2, data_init_end
	ldr x3, [x1], #8
	str x3, [x0], #8
	sub x2, x2, #8
	cbnz x2, data_init_start
data_init_end:

#define el2_main main
	// bl config_mmu

	// Initialize SCTLR_EL2 and HCR_EL2 to save values before entering EL2.
	//MSR SCTLR_EL2, XZR
	//MSR HCR_EL2, XZR
	
	// Determine the EL2 Execution state.
	//MRS X0, SCR_EL3
	//ORR X0, X0, #(1<<10) // RW EL2 Execution state is AArch64.
	//ORR X0, X0, #(1<<0) // NS EL1 is Non-secure world.
	//MSR SCR_EL3, x0
	//MOV X0, #0b01001 // DAIF=0000 
	//MSR SPSR_EL3, X0 // M[4:0]=01001 EL2h must match SCR_EL3.RW
	//// Determine EL2 entry.
	//ADR X0, el2_main // el2_entry points to the first instruction of 
	//MSR ELR_EL3, X0 // EL2 code.

	bl core0_c_entry
	b .
endfunc entrypoint


#define CORE_STACK_SIZE (128*1024/4)
func setup_stack
	.extern __STACK_TOP__
	ldr x0, =__STACK_TOP__
	bfi x0, xzr, #0, #4 // clear bit0-3

	mrs x1, mpidr_el1 //0x80000000
	and x1, x1, 0xff //x1 => core_id

	/* 
	** x0 => stack_top
	** x1 => core_id
	** x2 => offset_size 
	*/
	ldr x2, =CORE_STACK_SIZE
	mul x2, x2, x1                  //stack offset for each cpuid
	sub x0, x0, x2                  //stack top for each cpuid

	mov sp, x0
	ret
endfunc setup_stack

func secondary_core
	wfi
	b secondary_core
endfunc secondary_core


	.globl get_core_id
func get_core_id
	mrs x0, mpidr_el1 //0x80000000
	and x0, x0, 0xff //x0 => core_id
	ret
endfunc get_core_id
