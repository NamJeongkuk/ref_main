/*!
 * @file
 * @brief Gpio group
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GPIOGROUP_H
#define GPIOGROUP_H

#include "I_GpioGroup.h"
#include "GpioConfiguration.h"

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
 * Initialize the Gpio Group
 * @return I_GpioGroup_t The Gpio Group interface
 */
I_GpioGroup_t * GpioGroup_Init(void);

#endif
