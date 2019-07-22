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
   ENTRY(Gpio_HeartbeatLed,       GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, PortE, 1) \
   ENTRY(Gpio_AccelReset,         GpioDirection_Output, GpioPullUp_Disable, GpioDriveCapacity_Normal, Port1, 4) \

#endif
