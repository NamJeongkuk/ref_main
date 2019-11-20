/*!
 * @file
 * @brief GPIO Configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GPIOCONFIGURATION_H
#define GPIOCONFIGURATION_H

// clang-format off
// Name, Mode, PullUp, Speed, Port, Pin
#define GPIO_TABLE(ENTRY) \
   ENTRY(Erd_BspGpio_HeartbeatLed,        GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,      GPIOB, GPIO_PIN_9)  \
   ENTRY(Erd_BspGpio_OtherLed,            GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,      GPIOB, GPIO_PIN_8)  \
   ENTRY(Erd_BspGpio_Io1,                 GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM,   GPIOC, GPIO_PIN_6)  \
   ENTRY(Erd_BspGpio_Io2,                 GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM,   GPIOC, GPIO_PIN_7)  \
   ENTRY(Erd_BspGpio_Io3,                 GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM,   GPIOC, GPIO_PIN_8)  \
   ENTRY(Erd_BspGpio_Io4,                 GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM,   GPIOC, GPIO_PIN_9)  \
   ENTRY(Erd_BspGpio_Io5,                 GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM,   GPIOA, GPIO_PIN_8)  \
   ENTRY(Erd_BspGpio_Io6,                 GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,     GPIOB, GPIO_PIN_14) \
   ENTRY(Erd_BspGpio_PushButtonSwitch,    GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,     GPIOB, GPIO_PIN_13)
// clang-format on

#endif
