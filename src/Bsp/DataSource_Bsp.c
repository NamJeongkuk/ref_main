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
#include "DataSource_Gpio.h"
#include "DataSource_Adc.h"
#include "DataSource_OldHw_Pwm.h"
#include "DataSource_OldHw_InputCapture.h"
#include "DataSource_OldHw_Personality.h"

enum
{
   BspDataSourceCount = 5
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

static bool Has(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   return GetDataSource(erd) != NULL;
}

static uint8_t SizeOf(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   I_DataSource_t *dataSource = GetDataSource(erd);

   uassert(dataSource);
   return DataSource_SizeOf(dataSource, erd);
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

I_DataSource_t *DataSource_Bsp_Init(TimerModule_t *timerModule)
{
   instance.interface.api = &api;
   instance.interface.OnDataChange = &instance.OnDataChange.interface;
   Event_Synchronous_Init(&instance.OnDataChange);

   uint8_t index = 0;
   instance.dataSources[index++] = DataSource_Gpio_Init(timerModule, &instance.OnDataChange);
   instance.dataSources[index++] = DataSource_Adc_Init();
#ifdef OLD_HW
   instance.dataSources[index++] = DataSource_OldHw_Personality_Init();
   instance.dataSources[index++] = DataSource_OldHw_Pwm_Init();
   instance.dataSources[index++] = DataSource_OldHw_InputCapture_Init(timerModule, &instance.OnDataChange);
#else
   // instance.dataSources[index++] = DataSource_Personality_Init();
   // instance.dataSources[index++] = DataSource_Pwm_Init();
   // instance.dataSources[index++] = DataSource_InputCapture_Init(timerModule, &instance.OnDataChange);
#endif

   uassert(index <= NUM_ELEMENTS(instance.dataSources));

   return &instance.interface;
}
