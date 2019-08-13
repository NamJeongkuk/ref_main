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

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);

   if(IN_RANGE(BspErdGpioStart, erd, BspErdGpioEnd))
   {
      DataSource_Read(&instance->_private.dataSourceGpio.interface, erd, data);
   }
   else
   {
      uassert(!"ERD not supported");
   }
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);

   if(IN_RANGE(BspErdGpioStart, erd, BspErdGpioEnd))
   {
      DataSource_Write(&instance->_private.dataSourceGpio.interface, erd, data);
   }
   else
   {
      uassert(!"ERD not supported");
   }
}

static bool Has(const I_DataSource_t *_instance, const Erd_t erd)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);

   if(IN_RANGE(BspErdGpioStart, erd, BspErdGpioEnd))
   {
      return DataSource_Has(&instance->_private.dataSourceGpio.interface, erd);
   }

   return false;
}

static uint8_t SizeOf(const I_DataSource_t *_instance, const Erd_t erd)
{
   REINTERPRET(instance, _instance, DataSource_Bsp_t *);

   if(IN_RANGE(BspErdGpioStart, erd, BspErdGpioEnd))
   {
      return DataSource_SizeOf(&instance->_private.dataSourceGpio.interface, erd);
   }
   else
   {
      uassert(!"ERD not supported");
   }

   return 0;
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
}
