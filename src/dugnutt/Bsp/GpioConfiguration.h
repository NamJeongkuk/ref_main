/*!
 * @file
 * @brief GPIO Configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GPIOCONFIGURATION_H
#define GPIOCONFIGURATION_H

// clang-format off

// Name, Direction, PullUp, DriveCapacity, Port, Bit
#define GPIO_TABLE(ENTRY) \
   ENTRY(Erd_BspGpio_HeartbeatLed,        GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 3, GpioPolarity_Positive) \
   ENTRY(Erd_BspGpio_OtherLed,            GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 5, GpioPolarity_Positive) \
   ENTRY(Erd_BspGpio_Io1,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 7, GpioPolarity_Positive) \
   ENTRY(Erd_BspGpio_Io2,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 6, GpioPolarity_Positive) \
   ENTRY(Erd_BspGpio_Io3,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 5, GpioPolarity_Positive) \
   ENTRY(Erd_BspGpio_Io4,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 4, GpioPolarity_Positive) \
   ENTRY(Erd_BspGpio_Io5,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port3, 1, GpioPolarity_Positive) \
   ENTRY(Erd_BspGpio_Io6,                 GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortE, 3, GpioPolarity_Positive) \
   ENTRY(Erd_BspGpio_PushButtonSwitch,    GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 5, GpioPolarity_Positive)

// clang-format on

#endif
