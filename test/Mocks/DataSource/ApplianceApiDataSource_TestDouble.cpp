/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ApplianceApiDataSource.h"
#include "SystemErds.h"
#include "Header.h"

#include <string.h>
#include <stdint.h>
#include "DataSource_Ram.h"
#include "ConstArrayMap_LinearSearch.h"
#include "SystemErds.h"
#include "uassert.h"
#include "Constants_Time.h"
}

#include "GpioGroup_TestDouble.h"

// clang-format off

#define EXPAND_AS_VIRTUAL_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_VIRTUAL_, StorageType)(uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];)

#define EXPAND_AS_VIRTUAL_CONFIGURATION(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_VIRTUAL_, StorageType)({ Name COMMA OFFSET_OF(DataSourceStorage_t, MACRO_SAFE_CONCATENATE(erd, Name)) } COMMA)

// clang-format on

typedef struct
{
   ERD_TABLE(EXPAND_AS_VIRTUAL_OFFSET_STRUCT_MEMBER)
} DataSourceStorage_t;

static const DataSource_RamErdConfigurationElement_t erds[] = {
   ERD_TABLE(EXPAND_AS_VIRTUAL_CONFIGURATION)
};

static const ConstArrayMap_LinearSearchConfiguration_t constArrayMapConfig = {
   erds,
   NUM_ELEMENTS(erds),
   ELEMENT_SIZE(erds),
   MEMBER_SIZE(DataSource_RamErdConfigurationElement_t, erd),
   OFFSET_OF(DataSource_RamErdConfigurationElement_t, erd)
};

static DataSource_Ram_t dataSource;
static DataSourceStorage_t storage;
static ConstArrayMap_LinearSearch_t dataSourceConstArrayMap;
static GpioGroup_TestDouble_t gpioGroup;
static bool inputStates[UINT16_MAX];
static bool outputStates[UINT16_MAX];

static DataSource_ApplianceApiRevision1_t instance;

static void InvokeReset(void *context)
{
   IGNORE(context);
   Action_Invoke(instance._private.resetAction);
}

static void Read(I_DataSource_t *instance, const Erd_t erd, void *data)
{
   IGNORE(instance);
   if(erd == Erd_GpioGroupInterface)
   {
      *(I_GpioGroup_t **)data = &gpioGroup.interface;
   }
   else
   {
      DataSource_Read(&dataSource.interface, erd, data);
   }
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);

   if(erd == Erd_Reset)
   {
      REINTERPRET(resetDelay, data, const uint8_t *);

      if(*resetDelay)
      {
         TimerModule_StartOneShot(
            instance._private.timerModule,
            &instance._private.resetTimer,
            *resetDelay * MSEC_PER_SEC,
            InvokeReset,
            &instance);
      }
      else
      {
         InvokeReset(&instance);
      }
   }
   else
   {
      DataSource_Write(&dataSource.interface, erd, data);
   }
}

static bool Has(I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   return (erd == Erd_GpioGroupInterface) || DataSource_Has(&dataSource.interface, erd);
}

static uint8_t SizeOf(I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   return DataSource_SizeOf(&dataSource.interface, erd);
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

extern "C" void ApplianceApiDataSource_Init(
   ApplianceApiDataSource_t *_instance,
   I_DataSource_t *_dataSource,
   TimerModule_t *timerModule,
   I_Action_t *resetAction,
   I_Crc16Calculator_t *crcCalculator)
{
   IGNORE(_instance);
   IGNORE(_dataSource);
   IGNORE(crcCalculator);
   instance._private.timerModule = timerModule;
   instance._private.resetAction = resetAction;

   ConstArrayMap_LinearSearch_Init(&dataSourceConstArrayMap, &constArrayMapConfig);
   DataSource_Ram_Init(&dataSource, &storage, sizeof(storage), &dataSourceConstArrayMap.interface);

   instance.interface.api = &api;
   instance.interface.OnDataChange = dataSource.interface.OnDataChange;

   GpioGroup_TestDouble_Init(&gpioGroup, inputStates, outputStates);
}

extern "C" I_DataSource_t *ApplianceApiDataSource_DataSource(ApplianceApiDataSource_t *_instance)
{
   IGNORE(_instance);

   return &instance.interface;
}
