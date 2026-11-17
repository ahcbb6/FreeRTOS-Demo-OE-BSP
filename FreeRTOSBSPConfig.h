/* SPDX-License-Identifier: MIT */
#ifndef FREERTOS_BSP_CONFIG_H
#define FREERTOS_BSP_CONFIG_H

#ifndef __ASSEMBLER__
    #include "platform.h"
#endif

#if defined(__riscv)
    /* The upstream port uses this value as the mtime frequency. */
    #define configCPU_CLOCK_HZ 10000000
    #define configMTIME_BASE_ADDRESS 0x0200bff8UL
    #define configMTIMECMP_BASE_ADDRESS 0x02004000UL
    #define configENABLE_FPU 1
#else

#define configINTERRUPT_CONTROLLER_BASE_ADDRESS 0x08000000UL
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET 0x10000UL
#define configUNIQUE_INTERRUPT_PRIORITIES 256
#define configMAX_API_CALL_INTERRUPT_PRIORITY 192

#ifndef __ASSEMBLER__
void vConfigureTickInterrupt(void);
void vClearTickInterrupt(void);
#endif

#define configSETUP_TICK_INTERRUPT() vConfigureTickInterrupt()
#define configCLEAR_TICK_INTERRUPT() vClearTickInterrupt()

#endif

#define configASSERT(condition) \
    do { if (!(condition)) vPlatformAssert(__FILE__, __LINE__); } while (0)

#endif
