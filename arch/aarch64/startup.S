/* SPDX-License-Identifier: MIT */

.section .text.boot, "ax"
.global _start
.type _start, %function
_start:
    msr daifset, #0xf
    mrs x0, mpidr_el1
    and x0, x0, #0xff
    cbnz x0, .Lpark

    msr spsel, #1
    ldr x0, =__stack_top
    mov sp, x0
    ldr x0, =_freertos_vector_table
    msr vbar_el1, x0
    mov x0, #(3 << 20)
    msr cpacr_el1, x0
    isb

    ldr x0, =__bss_start
    ldr x1, =__bss_end
.Lclear:
    cmp x0, x1
    b.hs .Lmain
    str xzr, [x0], #8
    b .Lclear
.Lmain:
    bl main
.Lpark:
    wfi
    b .Lpark
.size _start, . - _start

.balign 2048
.global _freertos_vector_table
_freertos_vector_table:
.macro vector handler
    .balign 128
    b \handler
.endm
    vector FreeRTOS_SWI_Handler
    vector FreeRTOS_IRQ_Handler
    vector .Lexception
    vector .Lexception
    vector FreeRTOS_SWI_Handler
    vector FreeRTOS_IRQ_Handler
    vector .Lexception
    vector .Lexception
    .rept 8
    vector .Lexception
    .endr
.balign 128
.Lexception:
    msr daifset, #0xf
    mrs x0, esr_el1
    mrs x1, elr_el1
    mrs x2, far_el1
    bl vPlatformException
    b .Lpark

.section .note.GNU-stack, "", %progbits
