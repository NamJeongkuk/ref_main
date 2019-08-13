/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BSP_H
#define BSP_H

#include "DataSource_Composite.h"
#include "DataSource_Gpio.h"
#include "Event_Synchronous.h"

typedef struct
{
   struct
   {
      struct
      {
         DataSource_Composite_t composite;

         DataSource_Gpio_t gpio;
         DataSource_CompositeComponent_t gpioComponent;
      } dataSource;
   } _private;
} Bsp_t;

/*!
 * @param instance
 * @param timerModule
 */
void Bsp_Init(
   Bsp_t *instance,
   TimerModule_t *timerModule);

/*!
 * @param instance
 * @return
 */
I_DataSource_t *Bsp_GetDataSource(
   Bsp_t *instance);

#endif
