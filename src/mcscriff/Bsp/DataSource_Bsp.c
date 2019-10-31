/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "DataSource_Bsp.h"
#include "SystemErds.h"
#include "uassert.h"
#include "Event_Synchronous.h"

enum
{
   BspDataSourceCount = 2
};

static struct
{
   I_DataSource_t interface;
   I_DataSource_t *dataSources[BspDataSourceCount];
   Event_Synchronous_t OnDataChange;
} instance;

static I_DataSource_t *GetDataSource(const Erd_t erd)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(instance.dataSources); i++)
   {
      if(DataSource_Has(instance.dataSources[i], erd))
      {
         return instance.dataSources[i];
      }
   }

   return NULL;
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);
   I_DataSource_t *dataSource = GetDataSource(erd);

   uassert(dataSource);
   DataSource_Read(dataSource, erd, data);
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   I_DataSource_t *dataSource = GetDataSource(erd);

   uassert(dataSource);
   DataSource_Write(dataSource, erd, data);
}

static bool Has(const I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);

   for(uint8_t i = 0; i < NUM_ELEMENTS(instance.dataSources); i++)
   {
      if(DataSource_Has(instance.dataSources[i], erd))
      {
         return true;
      }
   }

   return false;
}

static uint8_t SizeOf(const I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   I_DataSource_t *dataSource = GetDataSource(erd);

   uassert(dataSource);
   return DataSource_SizeOf(dataSource, erd);
}

static const I_DataSource_Api_t api =
   { Read, Write, Has, SizeOf };

I_DataSource_t *DataSource_Bsp_Init(TimerModule_t *timerModule)
{
   instance.interface.api = &api;
   instance.interface.OnDataChange = &instance.OnDataChange.interface;
   Event_Synchronous_Init(&instance.OnDataChange);

   uint8_t index = 0;
   instance.dataSources[index++] = DataSource_Gpio_Init(timerModule, &instance.OnDataChange);
   instance.dataSources[index++] = DataSource_Adc_Init(timerModule);

   uassert(index <= NUM_ELEMENTS(instance.dataSources));

   return &instance.interface;
}
