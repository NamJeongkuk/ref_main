# applcommon.multi-target-starter-kit
This includes starter kit projects for most commonly-used microcontrollers.

Projects are named after the target microcontroller and are broken down into 3 main categories:
- Projects with the `nano-` prefix either do not use boot loaders or use a micro-specific boot loader, use `applcommon.tiny` instead of `applcommon.applcommon`, and are optimized for minimum resource usage.
- Projects with the `mini-` prefix use non-routing capable boot loaders but are otherwise general-purpose projects using `applcommon.applcommon`.
- Projects with no prefix use routing capable boot loaders and are general-purpose projects using `applcommon.applcommon`.

Projects may also have a suffix indicating that they are configured for a specific application instead of being general-purpose.

## rx130
General-purpose RX130 project using the [R5F51308ADFL](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rx/rx100/rx130/device/R5F51308ADFL.html).

| Memory     | Size   |
| ---------- | ------ |
| Flash      | 512 KB |
| RAM        | 48 KB  |
| Data Flash | 8 KB   |

## mini-rx130
General-purpose RX130 project using the [R5F51308ADFL](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rx/rx100/rx130/device/R5F51308ADFL.html) and a smaller, non-routing boot loader.

| Memory     | Size   |
| ---------- | ------ |
| Flash      | 512 KB |
| RAM        | 48 KB  |
| Data Flash | 8 KB   |

## rx130-cap-touch-client
Sample project that is an ERD stream receiver using the [R5F51308ADFL](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rx/rx100/rx130/device/R5F51308ADFL.html). Designed to work with `nano-stm8s-cap-touch`.

| Memory     | Size   |
| ---------- | ------ |
| Flash      | 512 KB |
| RAM        | 48 KB  |
| Data Flash | 8 KB   |

## rx231
General-purpose RX231 project using the [R5F52318ADFL](https://www.renesas.com/sg/en/products/microcontrollers-microprocessors/rx/rx200/rx231/device/R5F52318ADFL.html)

| Memory     | Size   |
| ---------- | ------ |
| Flash      | 512 KB |
| RAM        | 64 KB  |
| Data Flash | 8 KB   |

## mini-rx231
General-purpose RX231 project using the [R5F52318ADFL](https://www.renesas.com/sg/en/products/microcontrollers-microprocessors/rx/rx200/rx231/device/R5F52318ADFL.html) and a smaller, non-routing boot loader.

| Memory     | Size   |
| ---------- | ------ |
| Flash      | 512 KB |
| RAM        | 64 KB  |
| Data Flash | 8 KB   |

## stm32f3
General-purpose STM32 project using the [STM32F303RB](https://www.st.com/en/microcontrollers-microprocessors/stm32f303rb.html).

| Memory Type | Size   |
| ----------- | ------ |
| Flash       | 128 KB |
| RAM         | 40 KB  |

## nano-stm8s
STM8S project using the [STM8S003F3U6TR](https://www.st.com/en/microcontrollers-microprocessors/stm8s003f3.html). Uses the special-purpose STM8S boot loader and includes a tiny data source.

| Memory Type | Size  |
| ----------- | ----- |
| Flash       | 8 KB  |
| RAM         | 1 KB  |
| EEPROM      | 128 B |

## nano-stm8s-cap-touch
STM8S project using the [STM8S003F3U6TR](https://www.st.com/en/microcontrollers-microprocessors/stm8s003f3.html). Uses the special-purpose STM8S boot loader. Supports cap touch and single ERD streaming.

| Memory Type | Size  |
| ----------- | ----- |
| Flash       | 8 KB  |
| RAM         | 1 KB  |
| EEPROM      | 128 B |
