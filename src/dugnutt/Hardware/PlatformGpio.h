/*!
 * @file
 * @brief GPIO Hardware Driver
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PLATFORMGPIO_H
#define PLATFORMGPIO_H

#include "GpioConfiguration.h"
#include "I_GpioGroup.h"

enum
{
   Port0,
   Port1,
   Port2,
   Port3,
   Port4,
   Port5,
   PortA,
   PortB,
   PortC,
   PortD,
   PortE,
   PortH,
   PortJ
};

typedef enum
{
   GpioPullUp_Disable = 0,
   GpioPullUp_Enable,
   GpioPullUp_Max
} GpioPullUp_t;

typedef enum
{
   GpioDriveCapacity_Normal = 0,
   GpioDriveCapacity_High,
   GpioDriveCapacity_Max
} GpioDriveCapacity_t;

#define GPIO_EXPAND_AS_ENUM(name, direction, pullUp, driveCapacity, port, pin) name,

enum
{
   GPIO_TABLE(GPIO_EXPAND_AS_ENUM)
   Gpio_Count
};

/*!
 * Initialize the GPIO
 */
void PlatformGpio_Init(void);

/*!
 * Read a GPIO channel
 */
bool PlatformGpio_Read(const GpioChannel_t channel);

/*!
 * Write to a GPIO channel
 */
void PlatformGpio_Write(const GpioChannel_t channel, const bool state);

#endif
