# FreeRTOS-Demo-OE-BSP

Reusable FreeRTOS board support for OpenEmbedded's QEMU `virt` machines:

| Machine | CPU | Execution mode | Upstream kernel port |
| --- | --- | --- | --- |
| `qemuarm` | Cortex-A15 | AArch32, privileged modes | `ARM_CA9` |
| `qemuarm64` | Cortex-A57 | AArch64, EL1 | `ARM_AARCH64` |

Both use one core, GICv2, the architectural physical timer, a PL011 console,
and the same memory layout. Hardware support is shared where possible;
only startup, exception vectors, and register access differ by architecture.

## Code organization

- [platform.c](platform.c) and [platform.h](platform.h): UART access, GICv2,
  tick scheduling, idle handling, and fatal diagnostics.
- [FreeRTOSBSPConfig.h](FreeRTOSBSPConfig.h): hardware-specific FreeRTOS
  configuration, tick hooks, and assertion handling.
- [linker.ld](linker.ld): memory layout and bootstrap/IRQ stacks.
- [arch/arm](arch/arm) and [arch/aarch64](arch/aarch64): startup, exception
  vectors, and architectural timer/register access.
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
`DISTRO = "freertos"` and `MACHINE = "qemuarm"` or `"qemuarm64"`, then run:

```sh
bitbake freertos-demo
runqemu nographic
```

The layer fetches a pinned commit from `main` through `SRCREV_bsp` for
reproducible builds.

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
