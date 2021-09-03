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

#define GPIO_EXPAND_AS_ERD_NAMES(name, direction, port, pin) \
   name,

enum
{
   Erd_BspGpio_Start = (BspErdGpioStart - 1),
   GPIO_TABLE(GPIO_EXPAND_AS_ERD_NAMES)
      Erd_BspGpio_End
};

/*!
 * @param timerModule
 * @param onChangeEvent
 * @return
 */
I_DataSource_t *DataSource_Gpio_Init(
   TimerModule_t *timerModule,
   Event_Synchronous_t *onChangeEvent);

#endif
