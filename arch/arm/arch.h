/* SPDX-License-Identifier: MIT */
#ifndef BSP_ARCH_H
#define BSP_ARCH_H

#include <stdint.h>

static inline void arch_disable_interrupts(void)
{
    __asm volatile ("cpsid aif\ndsb sy\nisb" ::: "memory");
}

static inline uint32_t arch_counter_frequency(void)
{
    uint32_t frequency;

    __asm volatile ("mrc p15, 0, %0, c14, c0, 0" : "=r"(frequency));
    return frequency;
}

static inline uint64_t arch_counter_read(void)
{
    uint64_t counter;

    __asm volatile ("mrrc p15, 0, %Q0, %R0, c14" : "=r"(counter));
    return counter;
}

static inline void arch_timer_compare(uint64_t value)
{
    __asm volatile ("mcrr p15, 2, %Q0, %R0, c14\nisb" :: "r"(value) : "memory");
}

static inline void arch_timer_enable(void)
{
    uint32_t control = 1;

    __asm volatile ("mcr p15, 0, %0, c14, c2, 1\nisb" :: "r"(control) : "memory");
}

#endif
