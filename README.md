# FreeRTOS-Demo-OE-BSP

Reusable FreeRTOS board support for OpenEmbedded's QEMU `virt` machines:

| Machine | CPU | Execution mode | Upstream kernel port |
| --- | --- | --- | --- |
| `qemuarm` | Cortex-A15 | AArch32, privileged modes | `ARM_CA9` |
| `qemuarm64` | Cortex-A57 | AArch64, EL1 | `ARM_AARCH64` |
| `qemuriscv64` | RV64GC | RISC-V 64-bit, machine mode | `RISC-V` |

All use one core. The two ARM targets share GICv2, the architectural physical
timer, a PL011 console, and memory layout. RISC-V uses a 16550-compatible UART
and CLINT machine timer. Console formatting, idle handling, and fatal
diagnostics are common to all targets.

## Code organization

- [platform.c](platform.c) and [platform.h](platform.h): common console
  formatting, idle handling, and fatal diagnostics.
- [platform-arm.c](platform-arm.c): PL011, GICv2, and ARM tick scheduling.
- [platform-riscv.c](platform-riscv.c): 16550 UART and unexpected trap handling.
- [FreeRTOSBSPConfig.h](FreeRTOSBSPConfig.h): hardware-specific FreeRTOS
  configuration, tick hooks, and assertion handling.
- [linker.ld](linker.ld): memory layout and bootstrap/IRQ stacks.
- [arch/arm](arch/arm) and [arch/aarch64](arch/aarch64): startup, exception
  vectors, and architectural timer/register access.
- [arch/riscv64](arch/riscv64): machine-mode startup, interrupt masking,
  and RV64 memory layout.
- [freertos-bsp.mk](freertos-bsp.mk): reusable application/kernel/BSP build
  rules, selecting the upstream kernel port through `ARCH`.

No application tasks or FreeRTOS kernel sources are bundled here.

## Building with Yocto

The [meta-freertos](https://github.com/ahcbb6/meta-freertos) layer's
`freertos-qemuvirt` class fetches this BSP separately from the application and
[FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel). It supplies
Yocto's target compiler, architecture/IRQ-safe flags, source paths, kernel
compatibility patch, and QEMU boot settings.

For the [demo application](https://github.com/ahcbb6/FreeRTOS-Demo-OE), select
`DISTRO = "freertos"` and `MACHINE = "qemuarm"`, `"qemuarm64"`, or
`"qemuriscv64"`, then run:

```sh
bitbake freertos-demo
runqemu nographic
```

## RISC-V details

RV64 boots directly in machine mode with `-bios none`, without OpenSBI or
U-Boot. RAM starts at `0x80000000`; the distro builds both the application
and target runtime libraries with the `medany` code model, retaining the
RV64GC/LP64D compiler tune.
Startup initializes `gp`, the boot stack, BSS, FPU state, and `mtvec`.

The upstream `RISC-V` port supplies task context switching and the timer
interrupt handler, using its `RISCV_MTIME_CLINT_no_extensions` header.
`mtime` is at `0x0200bff8`, `mtimecmp` at `0x02004000`, and the QEMU timer
frequency is 10 MHz. The port's floating-point context support is enabled
consistently for both C and assembly. Vectors are not enabled in the compiler
tune or saved by this BSP.

UART access is polled, so no PLIC driver is needed for this demo. Unexpected
exceptions and external interrupts produce fatal diagnostics rather than
silently looping. RV32 is not enabled yet.

## Reusing the BSP

An application supplies `main`, its task/hooks implementation, and a
`FreeRTOSConfig.h` that includes `FreeRTOSBSPConfig.h`. Its Makefile lists
application objects and includes this repository's build rules:

```make
APP_OBJECTS = main.o
include $(BSP_SRC)/freertos-bsp.mk
```

Its Yocto recipe inherits `freertos-qemuvirt`, fetches the app as the named
source `app` with `SRCREV_app`, and sets `S`/`B` to the application directory
and `BAREMETAL_BINNAME = "image"`. The class passes `BSP_SRC`, `FREERTOS_SRC`,
`ARCH`, `CC`, `OBJCOPY`, and target flags. Build outputs are `image.elf` and
`image.bin`; compilation does not modify the BSP or kernel source trees.

The build rules use `heap_4` and the kernel's tasks, queues, and lists.
Applications enabling additional kernel services must add their required
objects to `APP_OBJECTS`. Use separate application build trees for each
architecture, as Yocto does, or run `make clean` before switching.

## License
MIT; see [LICENSE](LICENSE). FreeRTOS-Kernel has its own license.
