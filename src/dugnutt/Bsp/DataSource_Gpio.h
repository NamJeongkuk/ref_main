/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_GPIO_H
#define DATASOURCE_GPIO_H

#include "I_DataSource.h"
#include "BspErdRanges.h"
#include "Event_Synchronous.h"
#include "GpioConfiguration.h"

#define GPIO_EXPAND_AS_HARDWARE_ERDS(name, direction, pullUp, driveCapacity, port, pin) name ,

enum
{
   Erd_BspGpio_Start = (BspErdGpioStart - 1),
   GPIO_TABLE(GPIO_EXPAND_AS_HARDWARE_ERDS)
   Erd_BspGpio_End
};

typedef struct
{
   I_DataSource_t interface;

   struct
   {
      Event_Synchronous_t OnDataChange;
   } _private;
} DataSource_Gpio_t;

void DataSource_Gpio_Init(DataSource_Gpio_t *instance);

#endif
