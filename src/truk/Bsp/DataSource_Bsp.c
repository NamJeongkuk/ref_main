/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_Bsp.h"
#include "SystemErds.h"
#include "uassert.h"
#include <string.h>

static I_DataSource_t *GetDataSource(DataSource_Bsp_t *instance, const Erd_t erd)
{
   if(DataSource_Has(&instance->_private.dataSourceGpio.interface, erd))
   {
      return &instance->_private.dataSourceGpio.interface;
   }
   else if(DataSource_Has(&instance->_private.dataSourceAdc.interface, erd))
   {
      return &instance->_private.dataSourceAdc.interface;
   }

   return NULL;
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);
   I_DataSource_t *dataSource = GetDataSource(instance, erd);

   uassert(dataSource);
   DataSource_Read(dataSource, erd, data);
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);
   I_DataSource_t *dataSource = GetDataSource(instance, erd);

   uassert(dataSource);
   DataSource_Write(dataSource, erd, data);
}

static bool Has(const I_DataSource_t *_instance, const Erd_t erd)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);

   return
      DataSource_Has(&instance->_private.dataSourceGpio.interface, erd) ||
      DataSource_Has(&instance->_private.dataSourceAdc.interface, erd);
}

static uint8_t SizeOf(const I_DataSource_t *_instance, const Erd_t erd)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);
   I_DataSource_t *dataSource = GetDataSource(instance, erd);

   uassert(dataSource);
   return DataSource_SizeOf(dataSource, erd);
}

static const I_DataSource_Api_t api =
   { Read, Write, Has, SizeOf };

void DataSource_Bsp_Init(
   DataSource_Bsp_t *instance,
   TimerModule_t *timerModule)
{
   instance->interface.api = &api;
   instance->interface.OnDataChange = &instance->_private.OnDataChange.interface;
   Event_Synchronous_Init(&instance->_private.OnDataChange);

   DataSource_Gpio_Init(
      &instance->_private.dataSourceGpio,
      timerModule,
      &instance->_private.OnDataChange);

   DataSource_Adc_Init(&instance->_private.dataSourceAdc);
}
