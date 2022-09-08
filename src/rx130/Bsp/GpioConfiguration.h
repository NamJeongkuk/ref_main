/*!
 * @file
 * @brief GPIO Configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GPIOCONFIGURATION_H
#define GPIOCONFIGURATION_H

// clang-format off
#define Inverted 1
#define NonInverted 0

// Name, Direction, PullUp, DriveCapacity, Port, Bit, Inverted
#define GPIO_TABLE(ENTRY) \
   ENTRY(Erd_BspGpio_HeartbeatLed,        GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortE, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_RelayWatchdog,       GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortA, 5,  NonInverted) \
   ENTRY(Erd_BspGpio_OtherLed,            GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 5,  NonInverted) \
   ENTRY(Erd_BspGpio_Io1,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 7,  NonInverted) \
   ENTRY(Erd_BspGpio_Io2,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_Io3,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 5,  NonInverted) \
   ENTRY(Erd_BspGpio_Io4,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_Io5,                 GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port3, 1,  NonInverted) \
   ENTRY(Erd_BspGpio_Io6,                 GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortE, 3,  NonInverted) \
   ENTRY(Erd_BspGpio_PushButtonSwitch,    GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 5,  NonInverted) \
   ENTRY(GpioChannel_FFDamperPin1,        GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortJ, 9,  NonInverted) \
   ENTRY(GpioChannel_FFDamperPin2,        GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortJ, 10, NonInverted) \
   ENTRY(GpioChannel_FFDamperPin3,        GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortJ, 11, NonInverted) \
   ENTRY(GpioChannel_FFDamperPin4,        GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortJ, 12, NonInverted)

// clang-format on

#endif
