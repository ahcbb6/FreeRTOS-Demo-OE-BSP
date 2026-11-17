/* SPDX-License-Identifier: MIT */

#ifndef BSP_ARCH_H
#define BSP_ARCH_H

static inline void arch_disable_interrupts(void)
{
    __asm volatile ("csrci mstatus, 8\nfence iorw, iorw" ::: "memory");
}

#endif
