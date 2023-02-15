/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef NONVOLATILEASYNCDATASOURCE_H
#define NONVOLATILEASYNCDATASOURCE_H

#include "AsyncDataSource_CompositeStatic.h"
#include "AsyncDataSource_Eeprom.h"
#include "ConstArrayMap_BinarySearch.h"
#include "EepromPartitionIndex.h"
#include "InputGroup_NonVolatileDataSourceDefaultData.h"
#include "I_Eeprom.h"
#include "I_AsyncDataSource.h"
#include "I_Action.h"
#include "I_Crc16Calculator.h"
#include "PartitionedEeprom.h"
#include "Timer.h"

enum
{
   ClientVersion = 1,
   EraseBufferSize = 248, // this is also the size of NonVolatileAsyncDataSourceReadWriteBuffer_t
   AsyncDataSourceCount = EepromPartitionIndex_PartitionCount
};

typedef struct
{
   I_ConstArrayMap_t *map;
   void *readWriteBuffer;
   uint16_t readWriteBufferSize;
} NonVolatileAsyncDataSourceResources_t;

typedef struct
{
   Erd_t metadataErd;
   uint16_t mapCount;
} NonVolatileAsyncDataSourceConfiguration_t;

typedef struct
{
   struct
   {
      AsyncDataSource_Eeprom_t asyncDataSourceEeprom[AsyncDataSourceCount];
      AsyncDataSource_CompositeStatic_t asyncCompositeStatic;
      AsyncDataSource_CompositeStaticComponent_t asyncDataSourceCompositeComponents[AsyncDataSourceCount];
      PartitionedEepromPartitionResources_t partitionResources[EepromPartitionIndex_PartitionCount];
      uint16_t eraseBuffer[EraseBufferSize];
      InputGroup_NonVolatileDataSourceDefaultData_t defaultDataInputGroup;
      PartitionedEeprom_t partitionedEeprom;
   } _private;
} NonVolatileAsyncDataSource_t;

/*!
 * @param instance
 * @param crc16Calculator
 * @param timerModule
 * @param eeprom
 * @param partitionedEepromConfiguration
 * @param nvDefaultDataInput
 * @param resources
 * @param configuration
 */
void NonVolatileAsyncDataSource_Init(
   NonVolatileAsyncDataSource_t *instance,
   I_Crc16Calculator_t *crc16Calculator,
   TimerModule_t *timerModule,
   I_Eeprom_t *eeprom,
   const PartitionedEepromPartitionConfiguration_t *partitionedEepromConfiguration,
   I_InputGroup_t *nvDefaultDataInput,
   NonVolatileAsyncDataSourceResources_t *resources,
   const NonVolatileAsyncDataSourceConfiguration_t *configuration);

/*!
 * @param instance
 */
I_AsyncDataSource_t *NonVolatileAsyncDataSource_GetAsyncDataSource(NonVolatileAsyncDataSource_t *instance);

#endif
