/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// #include <asm_macros.S>

// 	.globl	semihosting_call

// func semihosting_call
// 	bkpt 0xab
// 	bx	lr
// endfunc semihosting_call


	.thumb_func
	.globl	 semihosting_call
	.type    semihosting_call, %function
	//.weak    semihosting_call
	// .fnstart
semihosting_call:
	bkpt 0xab
	bx	lr
	.size    semihosting_call, . - semihosting_call