/* SPDX-License-Identifier: MIT */

#include "platform.h"

#define MMIO8(address) (*(volatile uint8_t *)(uintptr_t)(address))
#define UART_BASE 0x10000000UL
#define UART_LSR (UART_BASE + 5)
#define UART_RX_READY (1U << 0)
#define UART_TX_READY (1U << 5)

void uart_putc(char value)
{
    while (!(MMIO8(UART_LSR) & UART_TX_READY))
        ;
    MMIO8(UART_BASE) = (unsigned char)value;
}

int uart_getc(void)
{
    if (!(MMIO8(UART_LSR) & UART_RX_READY))
        return -1;
    return MMIO8(UART_BASE);
}

void freertos_risc_v_application_exception_handler(void)
{
    uintptr_t cause, address, fault;

    __asm volatile ("csrr %0, mcause" : "=r"(cause));
    __asm volatile ("csrr %0, mepc" : "=r"(address));
    __asm volatile ("csrr %0, mtval" : "=r"(fault));
    vPlatformException(cause, address, fault);
}

void freertos_risc_v_application_interrupt_handler(void)
{
    freertos_risc_v_application_exception_handler();
}
