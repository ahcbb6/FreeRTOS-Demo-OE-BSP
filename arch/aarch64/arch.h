/* SPDX-License-Identifier: MIT */
#ifndef BSP_ARCH_H
#define BSP_ARCH_H

#include <stdint.h>

static inline void arch_disable_interrupts(void)
{
    __asm volatile ("msr daifset, #0xf\ndsb sy\nisb" ::: "memory");
}

static inline uint32_t arch_counter_frequency(void)
{
    uint64_t frequency;

    __asm volatile ("mrs %0, cntfrq_el0" : "=r"(frequency));
    return frequency;
}

static inline uint64_t arch_counter_read(void)
{
    uint64_t counter;

    __asm volatile ("mrs %0, cntpct_el0" : "=r"(counter));
    return counter;
}

static inline void arch_timer_compare(uint64_t value)
{
    __asm volatile ("msr cntp_cval_el0, %0\nisb" :: "r"(value) : "memory");
}

static inline void arch_timer_enable(void)
{
    uint64_t control = 1;

    __asm volatile ("msr cntp_ctl_el0, %0\nisb" :: "r"(control) : "memory");
}

#endif
