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
#include "I_Interrupt.h"

#define GPIO_EXPAND_AS_ERD_NAMES(name, direction, onDebounceCount, offDebounceCount, pullUp, driveCapacity, port, pin, inverted) \
   name,

#define INCLUDE_GpioDirection_Input(_x) _x
#define INCLUDE_GpioDirection_Output(_x)

//clang-format off
#define GPIO_EXPAND_AS_INPUT_ERD_NAMES(name, direction, onDebounceCount, offDebounceCount, pullUp, driveCapacity, port, pin, inverted) \
   CONCAT(INCLUDE_, direction)                                                                                                         \
   (CONCAT(name, _AsInput) COMMA)

//clang-format on
enum
{
   Erd_BspGpio_Start = (BspErdGpioStart - 1),
   GPIO_TABLE(GPIO_EXPAND_AS_ERD_NAMES)
      Erd_BspGpio_End
};

enum
{
   Erd_BspGpio_Start_AsInput,
   GPIO_TABLE(GPIO_EXPAND_AS_INPUT_ERD_NAMES)
      Erd_BspGpio_End_AsInput,
};

/*!
 * @return
 */
void DataSource_Gpio_Run(void);

/*!
 * @param onChangeEvent
 * @param debounceInterrupt
 * @return
 */
I_DataSource_t *DataSource_Gpio_Init(
   Event_Synchronous_t *onChangeEvent,
   I_Interrupt_t *debounceInterrupt);

#endif
