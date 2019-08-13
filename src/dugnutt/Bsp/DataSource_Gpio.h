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
#include "Timer.h"
#include "GpioConfiguration.h"
#include "XMacroUtils.h"
#include "utils.h"

#define IS_INPUT_GpioDirection_Output(_x)
#define IS_INPUT_GpioDirection_Input(_x) _x

#define GPIO_EXPAND_AS_HARDWARE_ERDS(name, direction, pullUp, driveCapacity, port, pin) \
   Erd_BspGpio_##name,

#define GPIO_TABLE_EXPAND_AS_GPIO_COUNT(name, direction, pullUp, driveCapacity, port, pin) \
   +1

enum
{
   Erd_BspGpio_Start = (BspErdGpioStart - 1),
   GPIO_TABLE(GPIO_EXPAND_AS_HARDWARE_ERDS)
      Erd_BspGpio_End
};

enum
{
   BitsPerByte = 8,
   GpioCount = 0 GPIO_TABLE(GPIO_TABLE_EXPAND_AS_GPIO_COUNT)
};

typedef struct
{
   I_DataSource_t interface;

   struct
   {
      Event_Synchronous_t *onChangeEvent;
      Timer_t timer;
      uint8_t inputCache[((GpioCount - 1) / BitsPerByte) + 1];
   } _private;
} DataSource_Gpio_t;

/*!
 * @param instance
 * @param timerModule
 * @param onChangeEvent
 */
void DataSource_Gpio_Init(
   DataSource_Gpio_t *instance,
   TimerModule_t *timerModule,
   Event_Synchronous_t *onChangeEvent);

#endif
