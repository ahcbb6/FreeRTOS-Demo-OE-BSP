/* SPDX-License-Identifier: MIT */

#include "FreeRTOS.h"
#include "task.h"
#include "platform.h"
#include "arch.h"

#define MMIO32(address) (*(volatile uint32_t *)(uintptr_t)(address))
#define UART_BASE 0x09000000UL
#define UART_FR (UART_BASE + 0x18)
#define UART_RX_EMPTY (1U << 4)
#define UART_TX_FULL (1U << 5)
#define GICD_BASE configINTERRUPT_CONTROLLER_BASE_ADDRESS
#define GICC_BASE (GICD_BASE + configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET)
#define TIMER_IRQ 30

static uint64_t timer_interval;
static uint64_t next_tick;

void uart_putc(char value)
{
    while (MMIO32(UART_FR) & UART_TX_FULL)
        ;
    MMIO32(UART_BASE) = (unsigned char)value;
}

void uart_puts(const char *text)
{
    while (*text)
        uart_putc(*text++);
}

int uart_getc(void)
{
    if (MMIO32(UART_FR) & UART_RX_EMPTY)
        return -1;
    return MMIO32(UART_BASE) & 0xff;
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

void vClearTickInterrupt(void)
{
    next_tick += timer_interval;
    arch_timer_compare(next_tick);
}

void vConfigureTickInterrupt(void)
{
    timer_interval = arch_counter_frequency() / configTICK_RATE_HZ;
    configASSERT(timer_interval != 0);

    MMIO32(GICD_BASE) = 0;
    MMIO32(GICD_BASE + 0x180) = 1U << TIMER_IRQ;
    MMIO32(GICD_BASE + 0x280) = 1U << TIMER_IRQ;
    *(volatile uint8_t *)(GICD_BASE + 0x400 + TIMER_IRQ) =
        portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT;
    MMIO32(GICD_BASE + 0x100) = 1U << TIMER_IRQ;
    MMIO32(GICC_BASE + 0x04) = 0xff;
    MMIO32(GICC_BASE + 0x08) = 0;
    MMIO32(GICC_BASE) = 1;
    MMIO32(GICD_BASE) = 1;
    __asm volatile ("dsb sy\nisb" ::: "memory");

    next_tick = arch_counter_read();
    vClearTickInterrupt();
    arch_timer_enable();
}

void vApplicationIRQHandler(uint32_t acknowledge)
{
    uint32_t interrupt = acknowledge & 0x3ff;

    if (interrupt == TIMER_IRQ)
        FreeRTOS_Tick_Handler();
    else
        configASSERT(interrupt >= 1020);
}
