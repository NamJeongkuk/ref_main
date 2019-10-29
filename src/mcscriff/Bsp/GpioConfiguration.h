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
   ENTRY(HeartbeatLed,        GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,      GPIOB, GPIO_PIN_9)  \
   ENTRY(OtherLed,            GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,      GPIOB, GPIO_PIN_8)  \
   ENTRY(Io1,                 GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM,   GPIOC, GPIO_PIN_6)  \
   ENTRY(Io2,                 GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM,   GPIOC, GPIO_PIN_7)  \
   ENTRY(Io3,                 GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM,   GPIOC, GPIO_PIN_8)  \
   ENTRY(Io4,                 GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM,   GPIOC, GPIO_PIN_9)  \
   ENTRY(Io5,                 GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM,   GPIOA, GPIO_PIN_8)  \
   ENTRY(Io6,                 GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,     GPIOB, GPIO_PIN_14) \
   ENTRY(PushButtonSwitch,    GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,     GPIOB, GPIO_PIN_13)
// clang-format on

#endif
