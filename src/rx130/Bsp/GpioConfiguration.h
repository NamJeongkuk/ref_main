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

#ifdef OLD_HW
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
   ENTRY(Erd_BspGpio_MotorPinB2,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 2,  NonInverted) \
   ENTRY(Erd_BspGpio_MotorPinB6,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_MotorPinB0,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 0,  NonInverted) \
   ENTRY(Erd_BspGpio_MotorPinB4,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_MotorPinC0,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 0,  NonInverted)

#else
// Name, Direction, PullUp, DriveCapacity, Port, Bit, Inverted
#define GPIO_TABLE(ENTRY) \
   ENTRY(Erd_BspGpio_HeartbeatLed,        GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortE, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_RelayWatchdog,       GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortA, 5,  NonInverted) \
   ENTRY(Erd_BspGpio_MotorPinB2,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 2,  NonInverted) \
   ENTRY(Erd_BspGpio_MotorPinB6,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 6,  NonInverted) \
   ENTRY(Erd_BspGpio_MotorPinB0,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 0,  NonInverted) \
   ENTRY(Erd_BspGpio_MotorPinB4,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 4,  NonInverted) \
   ENTRY(Erd_BspGpio_MotorPinC0,          GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 0,  NonInverted)
#endif

// clang-format on

#endif
