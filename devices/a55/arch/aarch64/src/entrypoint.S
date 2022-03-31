#include <arch.h>
#include <asm_macros.S>

	.globl entrypoint
func entrypoint
	// mrs x0, sctlr_el3 //0x30c50830
	ldr x0, =0x30c50830 
	msr sctlr_el3, x0
	isb

	ldr x0, =vector_table
	msr vbar_el3, x0
	isb

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

	mrs x1, mpidr_el1 //0x80000000
	and x1, x1, 0xffff //x1 => core_id
	cmp x1, #0
	bne data_init_end

	ldr	x0, =__BSS_START__
	ldr	x1, =__BSS_END__
	cmp x0, x1
	b.eq clear_loop_end
clear_loop:
	str	xzr, [x0], #8
	cmp	x0, x1
	b.lo	clear_loop
clear_loop_end:

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

	mrs x1, mpidr_el1 //0x80000000
	and x1, x1, 0xffff //x1 => core_id
#ifndef QEMU
	ubfx w1, w1, #8, #8
#endif
	cmp w1, #0
	bne secondary_core

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

	bl main
	b .
endfunc entrypoint


#define CORE_STACK_SIZE (16*1024/4)
func setup_stack
	.extern __STACK_TOP__
	ldr x0, =__STACK_TOP__
	bfi x0, xzr, #0, #4 // clear bit0-3

	// mrs x1, mpidr_el1 //0x80000000
	// and x1, x1, 0xf00 //x1 => core_id
	// lsr x1, x1, #8 //ubfx x1, x1, #8, #8
	mrs x1, mpidr_el1 //0x80000000
	and x1, x1, 0xffff //x1 => core_id
#ifndef QEMU
	ubfx w1, w1, #8, #8
#endif
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


/*
	x1 - core-id
 */
func secondary_core
	cmp x1, 1
	b.eq core1_sleep
	cmp x1, 2
	b.eq core2_sleep
	cmp x1, 3
	b.eq core3_sleep
	b .
endfunc secondary_core

#define CORE_WAKEUP_KEY 0x123456788654321

/*
	x0 - secondary_cores_entry addr
	x1 - core-id(read only)
	x2 - real-key
	x3 - key
	x4 - jump addr
*/
core1_sleep:
	ldr x0, =secondary_cores_entry
	ldr x4, [x0, #0]
	cmp x4, 0
	beq _core1_sleep
	ldr x2, [x0, #8]
	ldr x3, =CORE_WAKEUP_KEY
	cmp x2, x3
	bne _core1_sleep

	blr x4
	b .

_core1_sleep:
	wfe
	b core1_sleep

/*
	x0 - secondary_cores_entry addr
	x1 - core-id(read only)
	x2 - real-key
	x3 - key
	x4 - jump addr
*/
core2_sleep:
	ldr x0, =secondary_cores_entry
	add x0, x0, 16
	ldr x4, [x0, #0]
	cmp x4, 0
	beq _core2_sleep
	ldr x2, [x0, #8]
	ldr x3, =CORE_WAKEUP_KEY
	cmp x2, x3
	bne _core2_sleep

	blr x4
	b .

_core2_sleep:
	wfe
	b core2_sleep

/*
	x0 - secondary_cores_entry addr
	x1 - core-id(read only)
	x2 - real-key
	x3 - key
	x4 - jump addr
*/
core3_sleep:
	ldr x0, =secondary_cores_entry
	add x0, x0, 32
	ldr x4, [x0, #0]
	cmp x4, 0
	beq _core3_sleep
	ldr x2, [x0, #8]
	ldr x3, =CORE_WAKEUP_KEY
	cmp x2, x3
	bne _core3_sleep

	blr x4
	b .

_core3_sleep:
	wfe
	b core3_sleep