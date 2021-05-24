/*!
 * @file
 * @brief GPIO Configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GPIOCONFIGURATION_H
#define GPIOCONFIGURATION_H

// clang-format off
#define Port
#define Bit

#define Inverted 1
#define NonInverted 0

// Name, Direction, PullUp, DriveCapacity, Port, Bit, Inverted
#define GPIO_TABLE(ENTRY) \
   ENTRY(Erd_BspGpio_HeartbeatLed,        GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port 13, Bit 4, NonInverted) \
   ENTRY(Erd_BspGpio_OtherLed,            GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port 0, Bit 0, NonInverted) \
   ENTRY(Erd_BspGpio_Io2,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port 0, Bit 1, NonInverted) \
   ENTRY(Erd_BspGpio_Io3,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port 0, Bit 4, NonInverted) \
   ENTRY(Erd_BspGpio_Io4,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port 0, Bit 5, NonInverted) \
   ENTRY(Erd_BspGpio_Io5,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port 1, Bit 0, NonInverted) \
   ENTRY(Erd_BspGpio_Io6,                 GpioDirection_Input,  GpioPullUp_Enable,  GpioDriveCapacity_Normal, Port 1, Bit 1, NonInverted) \
   ENTRY(Erd_BspGpio_PushButtonSwitch,    GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, Port 1, Bit 2, NonInverted)

// clang-format on

#endif
