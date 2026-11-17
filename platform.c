/* SPDX-License-Identifier: MIT */

#include "FreeRTOS.h"
#include "task.h"
#include "platform.h"
#include "arch.h"

void uart_puts(const char *text)
{
    while (*text)
        uart_putc(*text++);
}

static void uart_hex(uint64_t value)
{
    static const char digits[] = "0123456789abcdef";

    for (int shift = 60; shift >= 0; shift -= 4)
        uart_putc(digits[(value >> shift) & 0xf]);
}

void vPlatformIdle(void)
{
    __asm volatile ("wfi" ::: "memory");
}

static void halt(void) __attribute__((noreturn));
static void halt(void)
{
    arch_disable_interrupts();
    for (;;)
        vPlatformIdle();
}

void vPlatformAssert(const char *file, unsigned int line)
{
    arch_disable_interrupts();
    uart_puts("ASSERT: ");
    uart_puts(file);
    uart_puts(":0x");
    uart_hex(line);
    uart_puts("\r\n");
    halt();
}

void vPlatformException(uintptr_t status, uintptr_t address, uintptr_t fault)
{
    arch_disable_interrupts();
    uart_puts("Unexpected exception: status=0x");
    uart_hex(status);
    uart_puts(" return address=0x");
    uart_hex(address);
    uart_puts(" fault address=0x");
    uart_hex(fault);
    uart_puts("\r\n");
    halt();
}
