/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include <stdint.h>
#include "DataSource_Bsp.h"
#include "DataSource_Ram.h"
#include "SystemErds.h"
#include "uassert.h"

#define GPIO_ERD_TABLE(ENTRY) \
   ENTRY(Erd_BspGpio_HeartbeatLed, bool) \
   ENTRY(Erd_BspGpio_OtherLed, bool) \
   ENTRY(Erd_BspGpio_PushButtonSwitch, bool) \
   ENTRY(Erd_BspAdc_SomeAnalogInput, AdcCounts_t) \
   ENTRY(Erd_BspAdc_AnotherAnalogInput, AdcCounts_t)

#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, DataType) \
   uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];

#define EXPAND_AS_CONFIGURATION(Name, DataType) \
   { Name, OFFSET_OF(DataSourceStorage_t, MACRO_SAFE_CONCATENATE(erd, Name)) },

typedef struct
{
   GPIO_ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
} DataSourceStorage_t;

static const DataSource_RamErdConfigurationElement_t erds[] =
   {
      GPIO_ERD_TABLE(EXPAND_AS_CONFIGURATION)
   };

static const ConstArrayMap_LinearSearchConfiguration_t constArrayMapConfig =
   {
      erds,
      NUM_ELEMENTS(erds),
      ELEMENT_SIZE(erds),
      MEMBER_SIZE(DataSource_RamErdConfigurationElement_t, erd),
      OFFSET_OF(DataSource_RamErdConfigurationElement_t, erd)
   };

static DataSource_Ram_t dataSource;
static DataSourceStorage_t storage;
static ConstArrayMap_LinearSearch_t dataSourceConstArrayMap;

static void Read(I_DataSource_t *instance, const Erd_t erd, void *data)
{
   IGNORE(instance);
   DataSource_Read(&dataSource.interface, erd, data);
}

static void Write(I_DataSource_t *instance, const Erd_t erd, const void *data)
{
   IGNORE(instance);
   DataSource_Write(&dataSource.interface, erd, data);
}

static bool Has(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   return DataSource_Has(&dataSource.interface, erd);
}

static uint8_t SizeOf(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   return DataSource_SizeOf(&dataSource.interface, erd);
}

static const I_DataSource_Api_t api =
   { Read, Write, Has, SizeOf };

void DataSource_Bsp_Init(
   DataSource_Bsp_t *instance,
   TimerModule_t *timerModule)
{
   IGNORE_ARG(timerModule);

   ConstArrayMap_LinearSearch_Init(&dataSourceConstArrayMap, &constArrayMapConfig);
   DataSource_Ram_Init(&dataSource, &storage, sizeof(storage), &dataSourceConstArrayMap.interface);

   instance->interface.api = &api;
   instance->interface.OnDataChange = dataSource.interface.OnDataChange;
}
