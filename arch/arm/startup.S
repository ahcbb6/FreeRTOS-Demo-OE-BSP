/* SPDX-License-Identifier: MIT */

.syntax unified
.arm
.section .text.boot, "ax"
.global _start
.type _start, %function
_start:
    cpsid aif
    mrc p15, 0, r0, c0, c0, 5
    ands r0, r0, #0xff
    bne .Lpark

    cps #0x12
    ldr sp, =__irq_stack_top
    cps #0x13
    ldr sp, =__stack_top
    ldr r0, =_freertos_vector_table
    mcr p15, 0, r0, c12, c0, 0

    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #(1 << 13)
    mcr p15, 0, r0, c1, c0, 0
    mrc p15, 0, r0, c1, c0, 2
    orr r0, r0, #(0xf << 20)
    mcr p15, 0, r0, c1, c0, 2
    isb
    mov r0, #(1 << 30)
    vmsr fpexc, r0

    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0
.Lclear:
    cmp r0, r1
    strlo r2, [r0], #4
    blo .Lclear
    bl main
.Lpark:
    wfi
    b .Lpark
.size _start, . - _start

.balign 32
.global _freertos_vector_table
_freertos_vector_table:
    b _start
    b .Lexception
    b FreeRTOS_SWI_Handler
    b .Lprefetch_abort
    b .Ldata_abort
    b .Lexception
    b FreeRTOS_IRQ_Handler
    b .Lexception

.Lprefetch_abort:
    mrc p15, 0, r0, c5, c0, 1
    mrc p15, 0, r2, c6, c0, 2
    b .Lfatal
.Ldata_abort:
    mrc p15, 0, r0, c5, c0, 0
    mrc p15, 0, r2, c6, c0, 0
    b .Lfatal
.Lexception:
    mrs r0, spsr
    mov r2, #0
.Lfatal:
    mov r1, lr
    cpsid aif, #0x13
    bl vPlatformException
    b .Lpark

.section .note.GNU-stack, "", %progbits
