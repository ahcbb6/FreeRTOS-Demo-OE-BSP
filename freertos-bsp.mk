# SPDX-License-Identifier: MIT

ARCH ?= aarch64
BSP_ARCH = $(ARCH)
PLATFORM_OBJECT = platform-arm.o
LINKER_SCRIPT = $(BSP_SRC)/linker.ld
ifeq ($(ARCH),aarch64)
PORT_NAME = ARM_AARCH64
else ifeq ($(ARCH),arm)
PORT_NAME = ARM_CA9
else ifneq ($(filter $(ARCH),riscv32 riscv64),)
BSP_ARCH = riscv
PORT_NAME = RISC-V
PLATFORM_OBJECT = platform-riscv.o
LINKER_SCRIPT = $(BSP_SRC)/arch/$(BSP_ARCH)/linker.ld
CPPFLAGS += -I$(FREERTOS_SRC)/portable/GCC/RISC-V/chip_specific_extensions/RISCV_MTIME_CLINT_no_extensions
portASM.o: CPPFLAGS += -include FreeRTOSConfig.h
else
$(error Unsupported ARCH '$(ARCH)')
endif

PORT = $(FREERTOS_SRC)/portable/GCC/$(PORT_NAME)
CPPFLAGS += -I. -I$(BSP_SRC) -I$(BSP_SRC)/arch/$(BSP_ARCH) -I$(FREERTOS_SRC)/include -I$(PORT)
VPATH = $(BSP_SRC)/arch/$(BSP_ARCH):$(BSP_SRC):$(FREERTOS_SRC):$(PORT):$(FREERTOS_SRC)/portable/MemMang
OBJECTS = $(APP_OBJECTS) startup.o platform.o $(PLATFORM_OBJECT) tasks.o queue.o list.o port.o portASM.o heap_4.o

.DEFAULT_GOAL := all

all: image.elf image.bin

image.elf: $(OBJECTS) $(LINKER_SCRIPT)
	$(CC) $(CFLAGS) $(LDFLAGS) -static -nostdlib -no-pie -Wl,-T,$(LINKER_SCRIPT) \
		$(OBJECTS) -Wl,--start-group -lc -lgcc -Wl,--end-group -o $@

image.bin: image.elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c FreeRTOSConfig.h $(BSP_SRC)/FreeRTOSBSPConfig.h $(BSP_SRC)/platform.h $(BSP_SRC)/arch/$(BSP_ARCH)/arch.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.S FreeRTOSConfig.h $(BSP_SRC)/FreeRTOSBSPConfig.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJECTS) image.elf image.bin

.PHONY: all clean
