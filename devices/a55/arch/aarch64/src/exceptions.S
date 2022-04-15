#include <asm_macros.S>


/* -----------------------------------------------------------------------------
 * Very simple stackless exception handlers.
 * -----------------------------------------------------------------------------
 */
	.globl vector_table
vector_base vector_table

	/* -----------------------------------------------------
	 * Current EL with SPt : 0x0 - 0x200
	 * -----------------------------------------------------
	*/
vector_entry sync_exception_spt
	b .
end_vector_entry sync_exception_spt

vector_entry irq_spt
	b .
end_vector_entry irq_spt

vector_entry fiq_spt
	b .
end_vector_entry fiq_spt

vector_entry serror_spt
	b .
end_vector_entry serror_spt

	/* -----------------------------------------------------
	 * Current EL with SPh: 0x200 - 0x400
	 * -----------------------------------------------------
	 */
vector_entry sync_exception_sph
	mrs x0, esr_el3
	mrs x1, far_el3
	mrs x2, elr_el3
	b .
end_vector_entry sync_exception_sph

vector_entry irq_sph
	/* Save volatile registers. */
	STP		X0, X1, [SP, #-0x10]!
	STP		X2, X3, [SP, #-0x10]!
	STP		X4, X5, [SP, #-0x10]!
	STP		X6, X7, [SP, #-0x10]!
	STP		X8, X9, [SP, #-0x10]!
	STP		X10, X11, [SP, #-0x10]!
	STP		X12, X13, [SP, #-0x10]!
	STP		X14, X15, [SP, #-0x10]!
	STP		X16, X17, [SP, #-0x10]!
	STP		X18, X19, [SP, #-0x10]!
	STP		X29, X30, [SP, #-0x10]!

	/* Save SPSR & ELR */
	MRS		X3, SPSR_EL3
	MRS		X2, ELR_EL3
	STP 	X2, X3, [SP, #-0x10]!

	bl		irqs_handler

	/* Restore volatile registers. */
	LDP 	X4, X5, [SP], #0x10  /* SPSR and ELR. */
	MSR		SPSR_EL3, X5 /*_RB_ Assumes started in EL3. */
	MSR		ELR_EL3, X4

	LDP		X29, X30, [SP], #0x10
	LDP		X18, X19, [SP], #0x10
	LDP		X16, X17, [SP], #0x10
	LDP		X14, X15, [SP], #0x10
	LDP		X12, X13, [SP], #0x10
	LDP		X10, X11, [SP], #0x10
	LDP		X8, X9, [SP], #0x10
	LDP		X6, X7, [SP], #0x10
	LDP		X4, X5, [SP], #0x10
	LDP		X2, X3, [SP], #0x10
	LDP		X0, X1, [SP], #0x10
	
	ERET
end_vector_entry irq_sph

vector_entry fiq_sph
	b .
end_vector_entry fiq_sph

vector_entry serror_sph
	b .
end_vector_entry serror_sph



	/* -----------------------------------------------------
	 * Lower EL using AArch64 : 0x400 - 0x600
	 * -----------------------------------------------------
	 */
vector_entry sync_exception_aarch64
	b .
end_vector_entry sync_exception_aarch64

vector_entry irq_aarch64
	b .
end_vector_entry irq_aarch64

vector_entry fiq_aarch64
	b .
end_vector_entry fiq_aarch64

vector_entry serror_aarch64
	b .
end_vector_entry serror_aarch64


	/* -----------------------------------------------------
	 * Lower EL using AArch32 : 0x600 - 0x800
	 * -----------------------------------------------------
	 */
vector_entry sync_exception_aarch32
	b .
end_vector_entry sync_exception_aarch32

vector_entry irq_aarch32
	b .
end_vector_entry irq_aarch32

vector_entry fiq_aarch32
	b .
end_vector_entry fiq_aarch32

vector_entry serror_aarch32
	b .
end_vector_entry serror_aarch32

