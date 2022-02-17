// Copyright 2016 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the “License”); you may not use this file except in
// compliance with the License. You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an “AS IS” BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.


#define EXCEPTION_STACK_SIZE 96


/* ========================================================= [ entry ] === */
  .section .text.bootrom

_start.bootrom:
  .global _start.bootrom

reset_handler:
  /* set all registers to zero */
  mv  x1, x0
  mv  x2, x1
  mv  x3, x1
  mv  x4, x1
  mv  x5, x1
  mv  x6, x1
  mv  x7, x1
  mv  x8, x1
  mv  x9, x1
  mv x10, x1
  mv x11, x1
  mv x12, x1
  mv x13, x1
  mv x14, x1
  mv x15, x1
  mv x16, x1
  mv x17, x1
  mv x18, x1
  mv x19, x1
  mv x20, x1
  mv x21, x1
  mv x22, x1
  mv x23, x1
  mv x24, x1
  mv x25, x1
  mv x26, x1
  mv x27, x1
  mv x28, x1
  mv x29, x1
  mv x30, x1
  mv x31, x1

  j relocate_data_end

  ############# load .text .data into ram
relocate_text:
  la    t0, __bootrom_text_end # source
  la    t1, __text_start # destination
  la    t2, __text_end
  # check test rom source staring location is not the target source starting location
  beq   t0, t1, relocate_text_end
  # check test target staring location is not greater or equal to target source ending location
  bgeu  t1, t2, relocate_text_end
relocate_text_loop:
  lw    a0, 0(t0)
  addi  t0, t0, 4
  sw    a0, 0(t1)
  addi  t1, t1, 4
  bltu  t1, t2, relocate_text_loop
relocate_text_end:

relocate_data:
  la    t0, __bootrom_text_end # source
  addi  t0,t0,__text_size
  la    t1, __data_start # destination
  la    t2, __data_end
  # check data rom source staring location is not the target source starting location
  beq   t0, t1, relocate_data_end
  # check data target staring location is not greater or equal to target source ending location
  bgeu  t1, t2, relocate_data_end
relocate_data_loop:
  lw    a0, 0(t0)
  addi  t0, t0, 4
  sw    a0, 0(t1)
  addi  t1, t1, 4
  bltu  t1, t2, relocate_data_loop
relocate_data_end:
  ############# load .text .data into ram

## jump to iram/sysram
  li s0, __text_start
  addi s0, s0, 0x80
  jr s0

  /* stack initilization */
  la   x2, _stack_start

  /* clear BSS */
  la x26, _bss_start
  la x27, _bss_end

  bge x26, x27, zero_loop_end

zero_loop:
  sw x0, 0(x26)
  addi x26, x26, 4
  ble x26, x27, zero_loop
zero_loop_end:

  /* jump to main program entry point */
  // j main
  j .

  /* if program exits call exit routine from library */
  // jal  x1, exit

//--leo--/* ========================================== [ I2C handler ] === */
//--leo--ISR_I2C_HANDLER:
//--leo--  addi x2, x2, -EXCEPTION_STACK_SIZE
//--leo--  sw x1, 0x5C(x2)
//--leo--  jal x1, store_regs
//--leo--  la x1, end_except
//--leo--  jal x0, ISR_I2C
//--leo--
//--leo--/* ========================================== [ UART handler ] === */
//--leo--ISR_UART_HANDLER:
//--leo--  addi x2, x2, -EXCEPTION_STACK_SIZE
//--leo--  sw x1, 0x5C(x2)
//--leo--  jal x1, store_regs
//--leo--  la x1, end_except
//--leo--  jal x0, ISR_UART
//--leo--
//--leo--/* ========================================== [ GPIO handler ] === */
//--leo--ISR_GPIO_HANDLER:
//--leo--  addi x2, x2, -EXCEPTION_STACK_SIZE
//--leo--  sw x1, 0x5C(x2)
//--leo--  jal x1, store_regs
//--leo--  la x1, end_except
//--leo--  jal x0, ISR_GPIO
//--leo--
//--leo--/* ========================================== [ SPI Master recieve/transmit finish handler ] === */
//--leo--ISR_SPIM_HANDLER:
//--leo--  addi x2, x2, -EXCEPTION_STACK_SIZE
//--leo--  sw x1, 0x5C(x2)
//--leo--  jal x1, store_regs
//--leo--  la x1, end_except
//--leo--  jal x0, ISR_SPIM

/* ================================= [ illegal instruction handler] === */
illegal_insn_handler:
//--leo--  addi x2, x2, -EXCEPTION_STACK_SIZE
//--leo--  sw x1, 0x5C(x2)
//--leo--  jal x1, store_regs
//--leo--  la x1, end_except
//--leo--  jal x0, illegal_insn_handler_c
loop:
  j loop

// saves all caller-saved registers (except return address)
store_regs:
  sw  x3, 0x00(x2)  // gp
  sw  x4, 0x04(x2)  // tp
  sw  x5, 0x08(x2)  // t0
  sw  x6, 0x0c(x2)  // t1
  sw  x7, 0x10(x2)  // t2
  sw x10, 0x14(x2)  // a0
  sw x11, 0x18(x2)  // a1
  sw x12, 0x1c(x2)  // a2
  sw x13, 0x20(x2)  // a3
  sw x14, 0x24(x2)  // a4
  sw x15, 0x28(x2)  // a5
  sw x16, 0x2c(x2)  // a6
  sw x17, 0x30(x2)  // a7
  sw x28, 0x34(x2)  // t3
  sw x29, 0x38(x2)  // t4
  sw x30, 0x3c(x2)  // t5
  sw x31, 0x40(x2)  // t6
  csrr x28, 0x7B0
  csrr x29, 0x7B1
  csrr x30, 0x7B2
  sw x28, 0x44(x2)  // lpstart[0]
  sw x29, 0x48(x2)  // lpend[0]
  sw x30, 0x4C(x2)  // lpcount[0]
  csrr x28, 0x7B4
  csrr x29, 0x7B5
  csrr x30, 0x7B6
  sw x28, 0x50(x2)  // lpstart[1]
  sw x29, 0x54(x2)  // lpend[1]
  sw x30, 0x58(x2)  // lpcount[1]
  jalr x0, x1

// load back registers from stack
end_except:
  lw x28, 0x50(x2)  // lpstart[1]
  lw x29, 0x54(x2)  // lpend[1]
  lw x30, 0x58(x2)  // lpcount[1]
  csrrw x0, 0x7B4, x28
  csrrw x0, 0x7B5, x29
  csrrw x0, 0x7B6, x30
  lw x28, 0x44(x2)  // lpstart[0]
  lw x29, 0x48(x2)  // lpend[0]
  lw x30, 0x4C(x2)  // lpcount[0]
  csrrw x0, 0x7B0, x28
  csrrw x0, 0x7B1, x29
  csrrw x0, 0x7B2, x30
  lw  x3, 0x00(x2)
  lw  x4, 0x04(x2)
  lw  x5, 0x08(x2)
  lw  x6, 0x0c(x2)
  lw  x7, 0x10(x2)
  lw x10, 0x14(x2)
  lw x11, 0x18(x2)
  lw x12, 0x1c(x2)
  lw x13, 0x20(x2)
  lw x14, 0x24(x2)
  lw x15, 0x28(x2)
  lw x16, 0x2c(x2)
  lw x17, 0x30(x2)
  lw x28, 0x34(x2)
  lw x29, 0x38(x2)
  lw x30, 0x3c(x2)
  lw x31, 0x40(x2)
  lw  x1, 0x5C(x2)
  addi x2, x2, EXCEPTION_STACK_SIZE
  //--leo--eret
  ret

  .global _init.bootrom
  .global _fini.bootrom
_init.bootrom:
_fini.bootrom:
  # These don't have to do anything since we use init_array/fini_array.
  ret

/* =================================================== [ exceptions ] === */
/* This section has to be down here, since we have to disable rvc for it  */

  .section .vectors.bootrom, "ax"
  .option norvc;

  // external interrupts are handled by the same callback
  // until compiler supports IRQ routines
  .org 0x00
  .rept 16
  nop                       // unused
  .endr

  //--leo--jal x0, ISR_I2C_HANDLER
  //--leo--jal x0, ISR_UART_HANDLER
  //--leo--jal x0, ISR_GPIO_HANDLER
  //--leo--jal x0, ISR_SPIM_HANDLER    // 28: SPIM

  // reset vector
  .org 0x80
  jal x0, reset_handler

  // illegal instruction exception
  .org 0x84
  jal x0, illegal_insn_handler

  // ecall handler
  .org 0x88
  jal x0, illegal_insn_handler

  // lsu error handler
  .org 0x8c
  jal x0, illegal_insn_handler
