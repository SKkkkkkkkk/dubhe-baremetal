#include <arch.h>
#include <asm_macros.S>

	.globl entrypoint
func entrypoint
// #ifdef RTL
	// Initialise the register bank
	mov x0, xzr
	mov x1, xzr
	mov x2, xzr
	mov x3, xzr
	mov x4, xzr
	mov x5, xzr
	mov x6, xzr
	mov x7, xzr
	mov x8, xzr
	mov x9, xzr
	mov x10, xzr
	mov x11, xzr
	mov x12, xzr
	mov x13, xzr
	mov x14, xzr
	mov x15, xzr
	mov x16, xzr
	mov x17, xzr
	mov x18, xzr
	mov x19, xzr
	mov x20, xzr
	mov x21, xzr
	mov x22, xzr
	mov x23, xzr
	mov x24, xzr
	mov x25, xzr
	mov x26, xzr
	mov x27, xzr
	mov x28, xzr
	mov x29, xzr
	mov x30, xzr

	// Zero the stack pointers, link registers and status registers
	mov     sp,       x0
	msr     sp_el0,   xzr
	msr     sp_el1,   xzr
	msr     sp_el2,   xzr
	msr     elr_el1,  xzr
	msr     elr_el2,  xzr
	msr     elr_el3,  xzr
	msr 	spsr_el1, xzr
	msr 	spsr_el2, xzr
	msr 	spsr_el3, xzr

	msr 	vttbr_el2, xzr

	// Enable NEON and initialize the register bank if this
	// feature has been implemented
	mrs     x0, ID_AA64PFR0_EL1
	sbfx    x5, x0, #16, #4         // Extract the floating-point field
    cmp     x5, #-1                 // x0 == -1 if FP not present
	b.eq    rtl_init_end            // Skip FP initialization if not present

	mov     x1, #(0x3 << 20)
	msr     cpacr_el1, x1
	mrs     x1, cptr_el3

	bic     x1, x1, #(0x1 << 10)      // Ensure that CPTR_EL3.TFP is clear
	msr     cptr_el3, x1
	isb

	fmov    d0,  xzr
	fmov    d1,  xzr
	fmov    d2,  xzr
	fmov    d3,  xzr
	fmov    d4,  xzr
	fmov    d5,  xzr
	fmov    d6,  xzr
	fmov    d7,  xzr
	fmov    d8,  xzr
	fmov    d9,  xzr
	fmov    d10,  xzr
	fmov    d11,  xzr
	fmov    d12,  xzr
	fmov    d13,  xzr
	fmov    d14,  xzr
	fmov    d15,  xzr
	fmov    d16,  xzr
	fmov    d17,  xzr
	fmov    d18,  xzr
	fmov    d19,  xzr
	fmov    d20,  xzr
	fmov    d21,  xzr
	fmov    d22,  xzr
	fmov    d23,  xzr
	fmov    d24,  xzr
	fmov    d25,  xzr
	fmov    d26,  xzr
	fmov    d27,  xzr
	fmov    d28,  xzr
	fmov    d29,  xzr
	fmov    d30,  xzr
	fmov    d31,  xzr
rtl_init_end:
// #endif

	mrs x0, sctlr_el3 //0x30c50830
	ldr x0, =0x30c5083A
	msr sctlr_el3, x0
	isb

	ldr x0, =vector_table
	msr vbar_el3, x0
	isb

	mrs x0, scr_el3
	mov x0, #0x53e                 	// #568
	msr scr_el3, x0

	msr spsel, 1
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
	cmp x0, x1
	b.eq data_init_end
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
	msr daifclr, #0xf	//enable syserror,irq,fiq
	bne secondary_core
	// bl config_mmu
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