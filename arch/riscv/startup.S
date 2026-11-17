/* SPDX-License-Identifier: MIT */

.section .text.boot, "ax"
.global _start
.type _start, @function
_start:
    csrw mie, zero
    csrr t0, mhartid
    bnez t0, .Lpark

    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    la sp, __stack_top
    la t0, freertos_risc_v_trap_handler
    csrw mtvec, t0
    li t0, 0x2000
    csrw mstatus, t0
    csrw fcsr, zero

    la t0, __bss_start
    la t1, __bss_end
.Lclear:
    bgeu t0, t1, .Lmain
#if __riscv_xlen == 64
    sd zero, 0(t0)
    addi t0, t0, 8
#else
    sw zero, 0(t0)
    addi t0, t0, 4
#endif
    j .Lclear
.Lmain:
    call main
.Lpark:
    wfi
    j .Lpark
.size _start, . - _start

.section .note.GNU-stack, "", @progbits
