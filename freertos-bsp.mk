# SPDX-License-Identifier: MIT

ARCH ?= aarch64
ifeq ($(ARCH),aarch64)
PORT_NAME = ARM_AARCH64
else ifeq ($(ARCH),arm)
PORT_NAME = ARM_CA9
else
$(error Unsupported ARCH '$(ARCH)')
endif

PORT = $(FREERTOS_SRC)/portable/GCC/$(PORT_NAME)
CPPFLAGS += -I. -I$(BSP_SRC) -I$(BSP_SRC)/arch/$(ARCH) -I$(FREERTOS_SRC)/include -I$(PORT)
VPATH = $(BSP_SRC)/arch/$(ARCH):$(BSP_SRC):$(FREERTOS_SRC):$(PORT):$(FREERTOS_SRC)/portable/MemMang
OBJECTS = $(APP_OBJECTS) startup.o platform.o tasks.o queue.o list.o port.o portASM.o heap_4.o

all: image.elf image.bin

image.elf: $(OBJECTS) $(BSP_SRC)/linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -static -nostdlib -no-pie -Wl,-T,$(BSP_SRC)/linker.ld \
		$(OBJECTS) -Wl,--start-group -lc -lgcc -Wl,--end-group -o $@

image.bin: image.elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c FreeRTOSConfig.h $(BSP_SRC)/FreeRTOSBSPConfig.h $(BSP_SRC)/platform.h $(BSP_SRC)/arch/$(ARCH)/arch.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJECTS) image.elf image.bin

.PHONY: all clean
