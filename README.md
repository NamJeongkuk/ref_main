# applcommon.multi-target-starter-kit
This includes starter kit projects for most commonly-used microcontrollers.

Projects are named after the target microcontroller and are broken down into 3 main categories:
- Projects with the `nano-` prefix either do not use boot loaders or use a micro-specific boot loader, use `applcommon.tiny` instead of `applcommon.applcommon`, and are optimized for minimum RAM/ROM usage.
- Projects with the `micro-` prefix use non-routing capable boot loaders, use `applcommon.tiny` instead of `applcommon.applcommon`, and are optimized for minimum RAM/ROM usage.
- Projects with the `mini-` prefix use non-routing capable boot loaders but are otherwise general-purpose projects using `applcommon.applcommon`.
- Projects with no prefix use routing capable boot loaders and are general-purpose projects using `applcommon.applcommon`.

Projects may also have a suffix indicating that they are configured for a specific application instead of being general-purpose.

## Creating a New Project
A boot loader framework (if applicable) and an application framework must be chosen before creating a new project. Boot loader and application frameworks are cross-compatible so may be freely mixed and matched provided they meet the project's requirements.

Once frameworks have been chosen, new projects should be based on the most similar starter kit project. If there are no relevant projects available, a new project can be requested by creating a ticket in [`applcommon.help`](https://github.com/geappliances/applcommon.help/).

### Choosing a Boot Loader Framework
There are three frameworks available for boot loaders:
- The "large" framework uses the older `applcommon` boot loader libraries (see [`BootLoaderConfigurator.h`](https://github.com/geappliances/applcommon.applcommon/blob/master/BootLoader/BootLoaderConfigurator.h)). This framework has all of the capabilities available in `applcommon` available to implement routing or otherwise extend the boot loader's functionality. Large boot loaders are generally 20k+.
- The "medium" framework uses the newer `applcommon` boot loader libraries (see [`TinyBootLoader.h`](https://github.com/geappliances/applcommon.applcommon/blob/master/BootLoader/TinyBootLoader.h)). This framework has all of the same capabilities as the large framework. Medium boot loaders are generally 15-20k.
- The "small" framework uses the `applcommon.tiny` boot loader libraries (see [`TinyBootLoader.h`](https://github.com/geappliances/applcommon.tiny/blob/master/src/BootLoader/TinyBootLoader.h)). This framework has limited support for routing and a much smaller set of available libraries for extending the boot loader's functionality. It also has somewhat reduced performance and will generally drop a slightly larger percentage of packets. Small boot loaders are generally 6.5-12k.

Large boot loaders should not be used for new projects since they provide no benefit over the medium framework. Most of the available targets that do not have a prefix have been migrated to the medium framework.

Medium boot loaders should be used when routing or extended functionality is required (though extended functionality should be avoided in general as it adds risk to the OTA process). They are generally appropriate for central hub/pyramid point boards with a system.

Small boot loaders should be used when no routing or only very limited routing is required. They are generally appropriate for "leaf" nodes within a system or single board systems and not for central hub/pyramid point nodes.

### Choosing an Application Framework
There are two application frameworks available:
- [`applcommon`](https://github.com/geappliances/applcommon.applcommon) is a full-featured framework optimized for both size and performance and contains hundreds of modules. It is designed to scale from small to large applications.
- [`applcommon.tiny`](https://github.com/geappliances/applcommon.tiny) is a much smaller framework optimized solely for size. It contains only the most widely used components and some application-specific components for building highly-optimized applications.

`applcommon` should be considered the default application framework. It can be configured for good performance even in applications with hundreds of ERDs and maximizes the opportunities for re-use.

`applcommon.tiny` should only be chosen for very small applications with limited functionality and when application size (cost) is the primary concern.

## Available Projects
### micro-psoc4100s-cap-touch
Small cap touch application using the [CY8C4127AZI-S455](https://www.cypress.com/part/cy8c4127azi-s455).

| Memory | Size   |
|--------|--------|
| Flash  | 128 KB |
| RAM    | 16 KB  |

### micro-rl78
Small RL78 application using the [R5F101AF](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rl78-low-power-8-16-bit-mcus).

| Memory | Size  |
|--------|-------|
| Flash  | 96 KB |
| RAM    | 8 KB  |

### nano-rl78g12
Small RL78 application using the [R5F1026A](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rl78-low-power-8-16-bit-mcus). Includes no boot loader.

| Memory | Size  |
|--------|-------|
| Flash  | 16 KB |
| RAM    | 1.5 KB  |

### nano-rl78g13
Small RL78 application using the [R5F100LE](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rl78-low-power-8-16-bit-mcus). Includes no boot loader.

| Memory | Size  |
|--------|-------|
| Flash  | 64 KB |
| RAM    | 4 KB  |

### rx130
General-purpose RX130 project using the R5F51308ADFL.

| Memory     | Size   |
|------------|--------|
| Flash      | 512 KB |
| RAM        | 48 KB  |
| Data Flash | 8 KB   |

### mini-rx130
General-purpose RX130 project using the R5F51308ADFL and a smaller, non-routing boot loader.

| Memory     | Size   |
|------------|--------|
| Flash      | 512 KB |
| RAM        | 48 KB  |
| Data Flash | 8 KB   |

### micro-rx130
Small RX130 project using the R5F51308ADFL and a smaller, non-routing boot loader.

| Memory     | Size   |
|------------|--------|
| Flash      | 512 KB |
| RAM        | 48 KB  |
| Data Flash | 8 KB   |

### mirco-rx130-cap-touch
Sample project using Renesas Cap Touch using the [RX130 Starter kit](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rx-32-bit-performance-efficiency-mcus/rx130-512kb-starter-kit-renesas-starter-kit-rx130-512kb).

| Memory     | Size   |
|------------|--------|
| Flash      | 512 KB |
| RAM        | 48 KB  |
| Data Flash | 8 KB   |

### rx231
General-purpose RX231 project using the R5F52318ADFL.

| Memory     | Size   |
|------------|--------|
| Flash      | 512 KB |
| RAM        | 64 KB  |
| Data Flash | 8 KB   |

### mini-rx231
General-purpose RX231 project using the R5F52318ADFL and a smaller, non-routing boot loader.

| Memory     | Size   |
|------------|--------|
| Flash      | 512 KB |
| RAM        | 64 KB  |
| Data Flash | 8 KB   |

### stm32f3
General-purpose STM32 project using the [STM32F303RB](https://www.st.com/en/microcontrollers-microprocessors/stm32f303rb.html).

| Memory Type | Size   |
|-------------|--------|
| Flash       | 128 KB |
| RAM         | 40 KB  |

### mini-stm32f3
General-purpose STM32 project using the [STM32F303RB](https://www.st.com/en/microcontrollers-microprocessors/stm32f303rb.html) and a smaller, non-routing boot loader.

| Memory Type | Size   |
|-------------|--------|
| Flash       | 128 KB |
| RAM         | 40 KB  |

### nano-stm3g0
STM32G0 project using the [STM32G070KB](https://www.st.com/en/microcontrollers-microprocessors/stm32g070kb.html). Includes no boot loader.

| Memory Type | Size   |
|-------------|--------|
| Flash       | 128 KB |
| RAM         | 32 KB  |

### micro-stm8s
Small STM8S project using the [STM8S207CBT6](https://www.st.com/resource/en/datasheet/stm8s207cb.pdf) and a non-routing boot loader.

| Memory Type | Size   |
|-------------|--------|
| Flash       | 32 KB  |
| RAM         | 2 KB   |
| EEPROM      | 1024 B |

### nano-stm8s
STM8S project using the [STM8S003F3U6TR](https://www.st.com/en/microcontrollers-microprocessors/stm8s003f3.html). Uses the special-purpose STM8S boot loader and includes a tiny data source.

| Memory Type | Size  |
|-------------|-------|
| Flash       | 8 KB  |
| RAM         | 1 KB  |
| EEPROM      | 128 B |

### nano-stm8s-cap-touch
STM8S project using the [STM8S003F3U6TR](https://www.st.com/en/microcontrollers-microprocessors/stm8s003f3.html). Uses the special-purpose STM8S boot loader. Supports cap touch and single ERD streaming.

| Memory Type | Size  |
|-------------|-------|
| Flash       | 8 KB  |
| RAM         | 1 KB  |
| EEPROM      | 128 B |

### ra6m3
General-purpose RA6M3 project using the [R7FA6M3AH3](https://www.renesas.com/eu/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ra6m3-32-bit-microcontrollers-120mhz-usb-high-speed-ethernet-and-tft-controller) on the [EK-RA6M3 evaluation board](https://www.renesas.com/eu/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra6m3-ra6m3-mcu-group-evaluation-board).

| Memory Type | Size   |
|-------------|--------|
| Flash       | 2 MB   |
| RAM         | 640 KB |
| EEPROM      | 64 KB  |
