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
	b .
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

