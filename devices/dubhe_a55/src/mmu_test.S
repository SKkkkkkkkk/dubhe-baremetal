#include <arch.h>
#include <asm_macros.S>

// ------------------------------------------------------------

.equ TT_S1_TABLE,          0x00000000000000003    // NSTable=0, PXNTable=0, UXNTable=0, APTable=0

// TT block entries templates   (L1 and L2, NOT L3)
// Assuming table contents:
// 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
// 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
// 2 = b00000000 = Device-nGnRnE
.equ TT_S1_FAULT,           0x0
.equ TT_S1_NORMAL_NO_CACHE, 0x00000000000000401    // Index = 0, AF=1
.equ TT_S1_NORMAL_WBWA,     0x00000000000000405    // Index = 1, AF=1
.equ TT_S1_DEVICE_nGnRnE,   0x00600000000000409    // Index = 2, AF=1, PXN=1, UXN=1

.equ TT_S1_UXN,             (1 << 54)
.equ TT_S1_PXN,             (1 << 53)
.equ TT_S1_nG,              (1 << 11)
.equ TT_S1_NS,              (1 << 5)

.equ TT_S1_NON_SHARED,      (0 << 8)               // Non-shareable
.equ TT_S1_INNER_SHARED,    (3 << 8)               // Inner-shareable
.equ TT_S1_OUTER_SHARED,    (2 << 8)               // Outer-shareable

.equ TT_S1_PRIV_RW,         (0x0)
.equ TT_S1_PRIV_RO,         (0x2 << 6)
.equ TT_S1_USER_RW,         (0x1 << 6)
.equ TT_S1_USER_RO,         (0x3 << 6)

// ------------------------------------------------------------



	.globl config_mmu
func config_mmu
	// Configure SCR_EL3
	// ------------------
	MOV      x0, #1                           // NS=1
	ORR      x0, x0, #(1 << 1)                // IRQ=1         IRQs routed to EL3
	ORR      x0, x0, #(1 << 2)                // FIQ=1         FIQs routed to EL3
	ORR      x0, x0, #(1 << 3)                // EA=1          SError routed to EL3
	ORR      x0, x0, #(1 << 8)                // HCE=1         HVC instructions are enabled
	ORR      x0, x0, #(1 << 10)               // RW=1          Next EL down uses AArch64
	ORR      x0, x0, #(1 << 11)               // ST=1          Secure EL1 can access CNTPS_TVAL_EL1, CNTPS_CTL_EL1 & CNTPS_CVAL_EL1
												// SIF=0         Secure state instruction fetches from Non-secure memory are permitted
												// SMD=0         SMC instructions are enabled
												// TWI=0         EL2, EL1 and EL0 execution of WFI instructions is not trapped to EL3
												// TWE=0         EL2, EL1 and EL0 execution of WFE instructions is not trapped to EL3
	MSR      SCR_EL3, x0 //0xd0f


	// Set the Base address
	// ---------------------
	LDR      x0, =tt_l1_base                  // Get address of level 1 for TTBR0_EL3
	MSR      TTBR0_EL3, x0                    // Set TTBR0_EL3 (NOTE: There is no TTBR1 at EL3)


	// Set up memory attributes
	// -------------------------
	// This equates to:
	// 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
	// 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
	// 2 = b00000000 = Device-nGnRnE
	MOV      x0, #0x000000000000FF44
	MSR      MAIR_EL3, x0

	// Set up TCR_EL3
	// ---------------
	MOV      x0, #0x19                        // T0SZ=0b011001 Limits VA space to 39 bits, translation starts @ l1
	ORR      x0, x0, #(0x1 << 8)              // IGRN0=0b01    Walks to TTBR0 are Inner WB/WA
	ORR      x0, x0, #(0x1 << 10)             // OGRN0=0b01    Walks to TTBR0 are Outer WB/WA
	ORR      x0, x0, #(0x3 << 12)             // SH0=0b11      Inner Shareable
											// TBI0=0b0      Top byte not ignored
											// TG0=0b00      4KB granule
											// IPS=0         32-bit PA space
	MSR      TCR_EL3, x0


	// Ensure changes to system register are visible before MMU enabled
	ISB


	// Invalidate TLBs
	// ----------------
	TLBI     ALLE3
	DSB      SY
	ISB

	//
	// Very basic translation table
	//
	LDR      x1, =tt_l1_base                   // Address of L1 table
	
	// [0]: 0x0000,0000 - 0x3FFF,FFFF
	LDR      x0, =TT_S1_DEVICE_nGnRnE          // Entry template
											// AP=0, RW
											// Don't need to OR in address, as it is 0
	STR      x0, [x1, #0]

	// // [1]: 0x4000,0000 - 0x7FFF,FFFF
	// LDR      x0, =TT_S1_NORMAL_WBWA          // Entry template
	// 										// AP=0, RW
	// ORR      x0, x0, #0x40000000               // 'OR' template with base physical address
	// STR      x0, [x1, #8]

	// // [2]: 0x4000,0000 - 0x7FFF,FFFF
	// LDR      x0, =TT_S1_NORMAL_WBWA          // Entry template
	// 										// AP=0, RW
	// ORR      x0, x0, #0x80000000               // 'OR' template with base physical address
	// STR      x0, [x1, #16]

	// // [3]: 0x4000,0000 - 0x7FFF,FFFF
	// LDR      x0, =TT_S1_NORMAL_WBWA          // Entry template
	// 										// AP=0, RW
	// ORR      x0, x0, #0xC0000000               // 'OR' template with base physical address
	// STR      x0, [x1, #24]

	// // [0]: 0x0000,0000 - 0x3FFF,FFFF
	// LDR      x2, =tt_l2_base                    // Get address of L2 table
	// LDR      x0, =TT_S1_TABLE                   // Entry template for pointer to next level table
	// ORR      x0, x0, x2                         // Combine template with L2 table Base address
	// STR      x0, [x1, #0]                      // Write template into entry table[2]


	// LDR      x1, =tt_l2_base                   // Address of L1 table

	// LDR      x0, =TT_S1_NORMAL_WBWA            // Entry template
	// ORR      x0, x0, #TT_S1_INNER_SHARED       // 'OR' with inner-shareable attribute
	// 											// AP=0, RW
	// //ORR      x0, x0, #0x00000000               // 'OR' template with base physical address
	// STR      x0, [x1]

	DSB      SY


	// Enable MMU
	// -----------
	MOV      x0, #(1 << 0)                     // M=1 bit       Enable the stage 1 MMU
	ORR      x0, x0, #(1 << 2)                 // C=1 bit       Enable data and unified caches
	ORR      x0, x0, #(1 << 12)                // I=1           Enable instruction fetches to allocate into unified caches
												// A=0           Strict alignment checking disabled
												// SA=0          Stack alignment checking disabled
												// WXN=0         Write permission does not imply XN
												// EE=0          EL3 data accesses are little endian
	MSR      SCTLR_EL3, x0
	ISB

	ret
endfunc config_mmu


func clear_l2_tt


endfunc clear_l2_tt



// ------------------------------------------------------------
// Translation tables
// ------------------------------------------------------------

	.section  .data
	.align 12

	.global tt_l1_base
tt_l1_base:
	.fill 4096 , 1 , 0

	.global tt_l2_base
tt_l2_base:
	.fill 4096 , 1 , 0
