/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_Bsp.h"
#include "DataSource_Ram.h"
#include "SystemErds.h"
#include "uassert.h"
#include <string.h>

#define GPIO_ERD_TABLE(ENTRY) \
   ENTRY(Erd_BspGpio_HeartbeatLed, bool) \
   ENTRY(Erd_BspGpio_OtherLed, bool) \
   ENTRY(Erd_BspGpio_PushButtonSwitch, bool) \
   ENTRY(Erd_BspAdc_SomeAnalogInput, AdcCounts_t) \
   ENTRY(Erd_BspAdc_AnotherAnalogInput, AdcCounts_t)

#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, DataType) \
   (uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];)

#define EXPAND_AS_CONFIGURATION(Name, DataType) \
   { Name COMMA OFFSET_OF(DataSourceStorage_t, MACRO_SAFE_CONCATENATE(erd, Name)) } COMMA

typedef struct
{
   uint8_t erdErd_BspGpio_HeartbeatLed[sizeof(bool)];
   uint8_t erdErd_BspGpio_OtherLed[sizeof(bool)];
   uint8_t erdErd_BspGpio_PushButtonSwitch[sizeof(bool)];
   uint8_t erdErd_BspAdc_SomeAnalogInput[sizeof(AdcCounts_t)];
   uint8_t erdErd_BspAdc_AnotherAnalogInput[sizeof(AdcCounts_t)];
} DataSourceStorage_t;

static DataSource_Ram_t dataSource;
static DataSourceStorage_t storage;
static ConstArrayMap_LinearSearch_t dataSourceConstArrayMap;

static const DataSource_RamErdConfigurationElement_t erds[] =
   {
      { Erd_BspGpio_HeartbeatLed, OFFSET_OF(DataSourceStorage_t, erdErd_BspGpio_HeartbeatLed) },
      { Erd_BspGpio_OtherLed, OFFSET_OF(DataSourceStorage_t, erdErd_BspGpio_OtherLed) },
      { Erd_BspGpio_PushButtonSwitch, OFFSET_OF(DataSourceStorage_t, erdErd_BspGpio_PushButtonSwitch) },
      { Erd_BspAdc_SomeAnalogInput, OFFSET_OF(DataSourceStorage_t, erdErd_BspAdc_SomeAnalogInput) },
      { Erd_BspAdc_AnotherAnalogInput, OFFSET_OF(DataSourceStorage_t, erdErd_BspAdc_AnotherAnalogInput) },
   };

static const ConstArrayMap_LinearSearchConfiguration_t constArrayMapConfig =
   {
      erds,
      NUM_ELEMENTS(erds),
      ELEMENT_SIZE(erds),
      MEMBER_SIZE(DataSource_RamErdConfigurationElement_t, erd),
      OFFSET_OF(DataSource_RamErdConfigurationElement_t, erd)
   };

void DataSource_Bsp_Init(
   DataSource_Bsp_t *instance,
   TimerModule_t *timerModule)
{
   IGNORE_ARG(timerModule);

   ConstArrayMap_LinearSearch_Init(&dataSourceConstArrayMap, &constArrayMapConfig);
   DataSource_Ram_Init(&dataSource, &storage, sizeof(storage), &dataSourceConstArrayMap.interface);

   instance->interface.api = dataSource.interface;
}
