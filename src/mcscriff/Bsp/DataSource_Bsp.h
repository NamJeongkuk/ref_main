/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_BSP_H
#define DATASOURCE_BSP_H

#include "I_DataSource.h"
#include "DataSource_Gpio.h"
#include "DataSource_Adc.h"
#include "Event_Synchronous.h"

typedef struct
{
   I_DataSource_t interface;

   struct
   {
      I_DataSource_t *dataSourceGpio;
      I_DataSource_t *dataSourceAdc;
      Event_Synchronous_t OnDataChange;
   } _private;
} DataSource_Bsp_t;

/*!
 * @param instance
 * @param timerModule
 */
void DataSource_Bsp_Init(
   DataSource_Bsp_t *instance,
   TimerModule_t *timerModule);

#endif
