/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_SOFTPWMGPIO_H
#define DATASOURCE_SOFTPWMGPIO_H

#include "BspErdRanges.h"
#include "Event_Synchronous.h"
#include "I_DataSource.h"
#include "SoftPwmGpioConfiguration.h"

#define SOFTPWMGPIO_EXPAND_AS_ERD_NAMES(name, pullUp, driveCapacity, port, pin, inverted) \
   name,

// clang-format off
enum
{
   Erd_BspSoftPwmGpio_Start = (BspErdSoftPwmGpioStart - 1),
   SOFTPWMGPIO_TABLE(SOFTPWMGPIO_EXPAND_AS_ERD_NAMES)
   Erd_BspSoftPwmGpio_End
};
// clang-format on

I_DataSource_t *DataSource_SoftPwmGpio_Init(Event_Synchronous_t *onChangeEvent);

#endif
