/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SOFTPWMGPIOCONFIGURATION_H
#define SOFTPWMGPIOCONFIGURATION_H

// clang-format off
#define Inverted 1
#define NonInverted 0

// Name, PullUp, DriveCapacity, Port, Bit, Inverted
#define SOFTPWMGPIO_TABLE(ENTRY) \
   ENTRY(Erd_BspSoftPwmGpio_HTR_00, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortE, 6, NonInverted) \
   ENTRY(Erd_BspSoftPwmGpio_HTR_01, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortJ, 7, NonInverted) \
   ENTRY(Erd_BspSoftPwmGpio_HTR_02, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 3, NonInverted) \
   ENTRY(Erd_BspSoftPwmGpio_HTR_03, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortJ, 6, NonInverted) \
   ENTRY(Erd_BspSoftPwmGpio_HTR_04, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port0, 7, NonInverted)
// clang-format on

#endif
