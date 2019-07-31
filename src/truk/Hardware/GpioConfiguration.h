/*!
 * @file
 * @brief GPIO Configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GPIOCONFIGURATION_H
#define GPIOCONFIGURATION_H

// Enum, Direction, PullUp, DriveCapacity, Port, Bit
#define GPIO_TABLE(ENTRY) \
   ENTRY(Gpio_HeartbeatLed,       GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortB, 3) \
   ENTRY(Gpio_Io1,                GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 7) \
   ENTRY(Gpio_Io2,                GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 6) \
   ENTRY(Gpio_Io3,                GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 5) \
   ENTRY(Gpio_Io4,                GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 4) \
   ENTRY(Gpio_Io5,                GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port3, 1) \
   ENTRY(Gpio_Pushbutton,         GpioDirection_Input,  GpioPullUp_Disable, GpioDriveCapacity_Normal, PortC, 5) \

#endif
