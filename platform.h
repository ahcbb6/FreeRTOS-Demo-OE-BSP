/* SPDX-License-Identifier: MIT */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

void uart_putc(char value);
void uart_puts(const char *text);
int uart_getc(void);
void vPlatformIdle(void);
void vPlatformAssert(const char *file, unsigned int line) __attribute__((noreturn));
void vPlatformException(uintptr_t status, uintptr_t address, uintptr_t fault)
    __attribute__((noreturn));

#endif
