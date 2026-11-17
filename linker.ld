/* SPDX-License-Identifier: MIT */

ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x40080000, LENGTH = 127M
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
    .rodata : { *(.rodata .rodata.*) } > RAM :text
    .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > RAM :text
    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > RAM :text
    .data : { *(.data .data.*) } > RAM :data
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.* COMMON)
        . = ALIGN(16);
        __bss_end = .;
    } > RAM :data
    .stack (NOLOAD) : {
        . = ALIGN(16);
        . += 0x4000;
        __irq_stack_top = .;
        . += 0xc000;
        __stack_top = .;
    } > RAM :data
}
