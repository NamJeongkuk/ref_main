/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "InputGroup_NonVolatileDataSourceDefaultData.h"
#include "utils.h"
#include "SystemErds.h"
#include "uassert.h"
#include "Event_Unsupported.h"

#define NotNv 0

enum
{
   Erd_NvMetadata = 0
};

// clang-format off

#define EXPAND_AS_NON_VOLATILE_DEFAULT_DATA_MAPPING(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NVALL_, StorageType)({ Name COMMA NvDefaultData } COMMA)

// clang-format on

typedef struct
{
   Erd_t erd;
   void (*GetDefaultData)(void *data, size_t dataSize);
} DataMap_t;

static const DataMap_t defaultDataMap[] = {
   { .erd = Erd_NvMetadata, .GetDefaultData = NonVolatileDataSourceDefaultData_Zeros },
   ERD_TABLE(EXPAND_AS_NON_VOLATILE_DEFAULT_DATA_MAPPING)
};

static uint16_t FindRecordIndexInTable(InputGroup_NonVolatileDataSourceDefaultData_t *instance, InputChannel_t channel)
{
   uint16_t index;

   for(index = 0; index < instance->_private.erdCount; index++)
   {
      if(channel == instance->_private.erdInfoTable[index].erd)
      {
         return index;
      }
   }

   uassert(!"Invalid record");
   return 0;
}

static void Read(I_InputGroup_t *_instance, InputChannel_t channel, void *value)
{
   REINTERPRET(instance, _instance, InputGroup_NonVolatileDataSourceDefaultData_t *);
   uint16_t recordIndex = FindRecordIndexInTable(instance, channel);
   size_t dataSize = instance->_private.erdInfoTable[recordIndex].size;

   if(NULL != defaultDataMap[recordIndex].GetDefaultData)
   {
      defaultDataMap[recordIndex].GetDefaultData(value, dataSize);
   }
}

static I_Event_t *GetOnChangeEvent(I_InputGroup_t *instance)
{
   IGNORE(instance);
   return Event_Unsupported_GetInstance();
}

static const I_InputGroup_Api_t api = { Read, GetOnChangeEvent };

void InputGroup_NonVolatileDataSourceDefaultData_Init(
   InputGroup_NonVolatileDataSourceDefaultData_t *instance,
   const AsyncDataSource_EepromErdInfo_t *erdInfoTable,
   const uint16_t erdCount)
{
   instance->interface.api = &api;
   instance->_private.erdInfoTable = erdInfoTable;
   instance->_private.erdCount = erdCount;
}
