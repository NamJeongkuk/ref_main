/*!
 * @file
 * @brief GPIO Configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GPIOCONFIGURATION_H
#define GPIOCONFIGURATION_H

// clang-format off

// Name, Direction, Port, Bit
#define GPIO_TABLE(ENTRY) \
   ENTRY(Erd_BspGpio_HeartbeatLed,        GpioDirection_Output, Port7, 1) \
   ENTRY(Erd_BspGpio_OtherLed,            GpioDirection_Output, Port7, 2) \
   ENTRY(Erd_BspGpio_PushButtonSwitch,    GpioDirection_Input,  PortB, 4)

// clang-format on
#endif
