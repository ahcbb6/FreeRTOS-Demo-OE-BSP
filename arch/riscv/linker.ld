/* SPDX-License-Identifier: MIT */

ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

PHDRS
{
    text PT_LOAD FLAGS(5);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    .text : {
        KEEP(*(.text.boot))
        *(.text .text.*)
    } > RAM :text
    .rodata : { *(.rodata .rodata.* .srodata .srodata.*) } > RAM :text
    .data : {
        __global_pointer$ = . + 0x800;
        *(.sdata .sdata.* .data .data.*)
    } > RAM :data
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.sbss .sbss.* .bss .bss.* COMMON)
        . = ALIGN(16);
        __bss_end = .;
    } > RAM :data
    .stack (NOLOAD) : {
        . = ALIGN(16);
        . += 0x4000;
        __freertos_irq_stack_top = .;
        . += 0xc000;
        __stack_top = .;
    } > RAM :data
}
