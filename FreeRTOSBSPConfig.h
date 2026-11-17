/* SPDX-License-Identifier: MIT */
#ifndef FREERTOS_BSP_CONFIG_H
#define FREERTOS_BSP_CONFIG_H

#include "platform.h"

#define configINTERRUPT_CONTROLLER_BASE_ADDRESS 0x08000000UL
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET 0x10000UL
#define configUNIQUE_INTERRUPT_PRIORITIES 256
#define configMAX_API_CALL_INTERRUPT_PRIORITY 192

void vConfigureTickInterrupt(void);
void vClearTickInterrupt(void);

#define configASSERT(condition) \
    do { if (!(condition)) vPlatformAssert(__FILE__, __LINE__); } while (0)
#define configSETUP_TICK_INTERRUPT() vConfigureTickInterrupt()
#define configCLEAR_TICK_INTERRUPT() vClearTickInterrupt()

#endif
