/*
 * Copyright (c) 2015-2020, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>
#include <assert_macros.S>
# #include <console_macros.S>
# #include <drivers/ti/uart/uart_16550.h>


/* UART16550 Registers */
#define UARTTX			0x0
#define UARTRX			0x0
#define UARTDLL			0x0
#define UARTIER			0x4
#define UARTDLLM		0x4
#define UARTIIR			0x8
#define UARTFCR			0x8
#define UARTLCR			0xc
#define UARTMCR			0x10
#define UARTLSR			0x14
#define UARTMSR			0x18
#define UARTSPR			0x1c
#define UARTCSR			0x20
/* Some instances have MDR1 defined as well */
#define UARTMDR1		0x20
#define UARTRXFIFOCFG		0x24
#define UARTMIE			0x28
#define UARTVNDR		0x2c
#define UARTASR			0x3c

/* FIFO Control Register bits */
#define UARTFCR_FIFOMD_16450	(0 << 6)
#define UARTFCR_FIFOMD_16550	(1 << 6)
#define UARTFCR_RXTRIG_1	(0 << 6)
#define UARTFCR_RXTRIG_4	(1 << 6)
#define UARTFCR_RXTRIG_8	(2 << 6)
#define UARTFCR_RXTRIG_16	(3 << 6)
#define UARTFCR_TXTRIG_1	(0 << 4)
#define UARTFCR_TXTRIG_4	(1 << 4)
#define UARTFCR_TXTRIG_8	(2 << 4)
#define UARTFCR_TXTRIG_16	(3 << 4)
#define UARTFCR_DMAEN		(1 << 3)	/* Enable DMA mode */
#define UARTFCR_TXCLR		(1 << 2)	/* Clear contents of Tx FIFO */
#define UARTFCR_RXCLR		(1 << 1)	/* Clear contents of Rx FIFO */
#define UARTFCR_FIFOEN		(1 << 0)	/* Enable the Tx/Rx FIFO */

/* Line Control Register bits */
#define UARTLCR_DLAB		(1 << 7)	/* Divisor Latch Access */
#define UARTLCR_SETB		(1 << 6)	/* Set BREAK Condition */
#define UARTLCR_SETP		(1 << 5)	/* Set Parity to LCR[4] */
#define UARTLCR_EVEN		(1 << 4)	/* Even Parity Format */
#define UARTLCR_PAR		(1 << 3)	/* Parity */
#define UARTLCR_STOP		(1 << 2)	/* Stop Bit */
#define UARTLCR_WORDSZ_5	0		/* Word Length of 5 */
#define UARTLCR_WORDSZ_6	1		/* Word Length of 6 */
#define UARTLCR_WORDSZ_7	2		/* Word Length of 7 */
#define UARTLCR_WORDSZ_8	3		/* Word Length of 8 */

/* Line Status Register bits */
#define UARTLSR_RXFIFOEMT	(1 << 9)	/* Rx Fifo Empty */
#define UARTLSR_TXFIFOFULL	(1 << 8)	/* Tx Fifo Full */
#define UARTLSR_RXFIFOERR	(1 << 7)	/* Rx Fifo Error */
#define UARTLSR_TEMT		(1 << 6)	/* Tx Shift Register Empty */
#define UARTLSR_THRE		(1 << 5)	/* Tx Holding Register Empty */
#define UARTLSR_BRK		(1 << 4)	/* Break Condition Detected */
#define UARTLSR_FERR		(1 << 3)	/* Framing Error */
#define UARTLSR_PERR		(1 << 3)	/* Parity Error */
#define UARTLSR_OVRF		(1 << 2)	/* Rx Overrun Error */
#define UARTLSR_RDR_BIT		(0)		/* Rx Data Ready Bit */
#define UARTLSR_RDR		(1 << UARTLSR_RDR_BIT)	/* Rx Data Ready */


	/*
	 * "core" functions are low-level implementations that don't require
	 * writable memory and are thus safe to call in BL1 crash context.
	 */
	.globl console_16550_core_init
	.globl console_16550_core_putc
	.globl console_16550_core_getc
	.globl console_16550_core_flush

	// .globl console_16550_putc
	// .globl console_16550_getc
	// .globl console_16550_flush

	/* -----------------------------------------------
	 * int console_16550_core_init(uintptr_t base_addr,
	 * unsigned int uart_clk, unsigned int baud_rate)
	 * Function to initialize the console without a
	 * C Runtime to print debug information. This
	 * function will be accessed by console_init and
	 * crash reporting.
	 * In: x0 - console base address
	 *     w1 - Uart clock in Hz
	 *     w2 - Baud rate
	 * Out: return 1 on success, 0 on error
	 * Clobber list : x1, x2, x3
	 * -----------------------------------------------
	 */
func console_16550_core_init
	/* Check the input base address */
	cbz	x0, init_fail
	/* Check baud rate and uart clock for sanity */
	cbz	w1, init_fail
	cbz	w2, init_fail

	/* Program the baudrate */
	/* Divisor =  Uart clock / (16 * baudrate) */
	lsl	w2, w2, #4
	udiv	w2, w1, w2
	and	w1, w2, #0xff		/* w1 = DLL */
	lsr	w2, w2, #8
	and	w2, w2, #0xff		/* w2 = DLLM */
	ldr	w3, [x0, #UARTLCR]
	orr	w3, w3, #UARTLCR_DLAB
	str	w3, [x0, #UARTLCR]	/* enable DLL, DLLM programming */
	str	w1, [x0, #UARTDLL]	/* program DLL */
	str	w2, [x0, #UARTDLLM]	/* program DLLM */
	mov	w2, #~UARTLCR_DLAB
	and	w3, w3, w2
	str	w3, [x0, #UARTLCR]	/* disable DLL, DLLM programming */

	/* 8n1 */
	mov	w3, #3
	str	w3, [x0, #UARTLCR]
	/* no interrupt */
	mov	w3, #0
	str	w3, [x0, #UARTIER]
#ifdef TI_16550_MDR_QUIRK
	/* UART must be enabled on some platforms via the MDR register */
	str	w3, [x0, #UARTMDR1]
#endif /* TI_16550_MDR_QUIRK */
	/* enable fifo, DMA */
	mov	w3, #(UARTFCR_FIFOEN | UARTFCR_DMAEN)
	str	w3, [x0, #UARTFCR]
	/* DTR + RTS */
	mov	w3, #3
	str	w3, [x0, #UARTMCR]
	mov	w0, #1
	ret
init_fail:
	mov	w0, #0
	ret
endfunc console_16550_core_init

	# .globl console_16550_register

	/* -----------------------------------------------
	 * int console_16550_register(uintptr_t baseaddr,
	 *     uint32_t clock, uint32_t baud,
	 *     console_t *console);
	 * Function to initialize and register a new 16550
	 * console. Storage passed in for the console struct
	 * *must* be persistent (i.e. not from the stack).
	 * If w1 (UART clock) is 0, initialisation will be
	 * skipped, relying on previous code to have done
	 * this already. w2 is ignored then as well.
	 * In: x0 - UART register base address
	 *     w1 - UART clock in Hz
	 *     w2 - Baud rate (ignored if w1 is 0)
	 *     x3 - pointer to empty console_t struct
	 * Out: return 1 on success, 0 on error
	 * Clobber list : x0, x1, x2, x6, x7, x14
	 * -----------------------------------------------
	 */
# func console_16550_register
# 	mov	x7, x30
# 	mov	x6, x3
# 	cbz	x6, register_fail
# 	str	x0, [x6, #CONSOLE_T_BASE]

# 	/* A clock rate of zero means to skip the initialisation. */
# 	cbz	w1, register_16550

# 	bl	console_16550_core_init
# 	cbz	x0, register_fail

# register_16550:
# 	mov	x0, x6
# 	mov	x30, x7
# 	finish_console_register 16550 putc=1, getc=1, flush=1

# register_fail:
# 	ret	x7
# endfunc console_16550_register

	/* --------------------------------------------------------
	 * int console_16550_core_putc(int c, uintptr_t base_addr)
	 * Function to output a character over the console. It
	 * returns the character printed on success or -1 on error.
	 * In : w0 - character to be printed
	 *      x1 - console base address
	 * Out : return -1 on error else return character.
	 * Clobber list : x2
	 * --------------------------------------------------------
	 */
func console_16550_core_putc
#if ENABLE_ASSERTIONS
	cmp	x1, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */

	/* Prepend '\r' to '\n' */
	cmp	w0, #0xA
	b.ne	2f
	/* Check if the transmit FIFO is full */
1:	ldr	w2, [x1, #UARTLSR]
	and	w2, w2, #(UARTLSR_TEMT | UARTLSR_THRE)
	cmp	w2, #(UARTLSR_TEMT | UARTLSR_THRE)
	b.ne	1b
	mov	w2, #0xD		/* '\r' */
	str	w2, [x1, #UARTTX]

	/* Check if the transmit FIFO is full */
2:	ldr	w2, [x1, #UARTLSR]
	and	w2, w2, #(UARTLSR_TEMT | UARTLSR_THRE)
	cmp	w2, #(UARTLSR_TEMT | UARTLSR_THRE)
	b.ne	2b
	str	w0, [x1, #UARTTX]
	ret
endfunc console_16550_core_putc

	/* --------------------------------------------------------
	 * int console_16550_putc(int c, console_t *console)
	 * Function to output a character over the console. It
	 * returns the character printed on success or -1 on error.
	 * In : w0 - character to be printed
	 *      x1 - pointer to console_t structure
	 * Out : return -1 on error else return character.
	 * Clobber list : x2
	 * --------------------------------------------------------
	 */
# func console_16550_putc
# #if ENABLE_ASSERTIONS
# 	cmp	x1, #0
# 	ASM_ASSERT(ne)
# #endif /* ENABLE_ASSERTIONS */
# 	ldr	x1, [x1, #CONSOLE_T_BASE]
# 	b	console_16550_core_putc
# endfunc console_16550_putc

	/* ---------------------------------------------
	 * int console_16550_core_getc(uintptr_t base_addr)
	 * Function to get a character from the console.
	 * It returns the character grabbed on success
	 * or -1 on if no character is available.
	 * In :  x0 - console base address
	 * Out : w0 - character if available, else -1
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
func console_16550_core_getc
#if ENABLE_ASSERTIONS
	cmp	x0, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */

	/* Check if the receive FIFO is empty */
1:	ldr	w1, [x0, #UARTLSR]
	tbz	w1, #UARTLSR_RDR_BIT, no_char
	ldr	w0, [x0, #UARTRX]
	ret
no_char:
	mov	w0, #-1
	ret
endfunc console_16550_core_getc

	/* ---------------------------------------------
	 * int console_16550_getc(console_t *console)
	 * Function to get a character from the console.
	 * It returns the character grabbed on success
	 * or -1 on if no character is available.
	 * In :  x0 - pointer to console_t stucture
	 * Out : w0 - character if available, else -1
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
# func console_16550_getc
# #if ENABLE_ASSERTIONS
# 	cmp	x1, #0
# 	ASM_ASSERT(ne)
# #endif /* ENABLE_ASSERTIONS */
# 	ldr	x0, [x0, #CONSOLE_T_BASE]
# 	b	console_16550_core_getc
# endfunc console_16550_getc

	/* ---------------------------------------------
	 * void console_16550_core_flush(uintptr_t base_addr)
	 * Function to force a write of all buffered
	 * data that hasn't been output.
	 * In : x0 - console base address
	 * Out : void.
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
func console_16550_core_flush
#if ENABLE_ASSERTIONS
	cmp	x0, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */

	/* Loop until the transmit FIFO is empty */
1:	ldr	w1, [x0, #UARTLSR]
	and	w1, w1, #(UARTLSR_TEMT | UARTLSR_THRE)
	cmp	w1, #(UARTLSR_TEMT | UARTLSR_THRE)
	b.ne	1b

	ret
endfunc console_16550_core_flush

	/* ---------------------------------------------
	 * void console_16550_flush(console_t *console)
	 * Function to force a write of all buffered
	 * data that hasn't been output.
	 * In : x0 - pointer to console_t structure
	 * Out : void.
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
# func console_16550_flush
# #if ENABLE_ASSERTIONS
# 	cmp	x0, #0
# 	ASM_ASSERT(ne)
# #endif /* ENABLE_ASSERTIONS */
# 	ldr	x0, [x0, #CONSOLE_T_BASE]
# 	b	console_16550_core_flush
# endfunc console_16550_flush
