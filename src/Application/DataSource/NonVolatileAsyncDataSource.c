/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "NonVolatileAsyncDataSource.h"
#include "uassert.h"
#include "utils.h"

enum
{
   TimerTicksBetweenRetriesInMsec = 100,
   NumberOfRetriesBeforeErase = 10,
   HarnessEepromRetriesBeforeErase = 0,
   HarnessEepromIndex = EepromPartitionIndex_PartitionCount
};

static void VerifyAllCountsMatch(uint16_t partitionCount, uint16_t mapCount)
{
   uassert(mapCount == partitionCount);
}

void NonVolatileAsyncDataSource_Init(
   NonVolatileAsyncDataSource_t *instance,
   I_Crc16Calculator_t *crc16Calculator,
   TimerModule_t *timerModule,
   I_Eeprom_t *eeprom,
   I_Eeprom_t *personalityEeprom,
   const PartitionedEepromPartitionConfiguration_t *partitionedEepromConfiguration,
   I_InputGroup_t *nvDefaultDataInput,
   NonVolatileAsyncDataSourceResources_t *resources,
   const NonVolatileAsyncDataSourceConfiguration_t *configuration)
{
   VerifyAllCountsMatch(partitionedEepromConfiguration->partitionCount, configuration->mapCount);

   PartitionedEeprom_Init(
      &instance->_private.partitionedEeprom,
      partitionedEepromConfiguration,
      instance->_private.partitionResources,
      &instance->_private.eraseBuffer,
      EraseBufferSize,
      eeprom);

   for(uint8_t i = 0; i < EepromPartitionIndex_PartitionCount; i++)
   {
      AsyncDataSource_Eeprom_Init(
         &instance->_private.asyncDataSourceEeprom[i],
         resources[i].map,
         PartitionedEeprom_GetPartition(&instance->_private.partitionedEeprom, i),
         crc16Calculator,
         nvDefaultDataInput,
         resources[i].readWriteBuffer,
         resources[i].readWriteBufferSize,
         configuration->metadataErd,
         ClientVersion,
         timerModule,
         TimerTicksBetweenRetriesInMsec,
         NumberOfRetriesBeforeErase,
         true);
   }

   AsyncDataSource_Eeprom_Init(
      &instance->_private.asyncDataSourceEeprom[HarnessEepromIndex],
      resources[HarnessEepromIndex].map,
      personalityEeprom,
      crc16Calculator,
      nvDefaultDataInput,
      resources[HarnessEepromIndex].readWriteBuffer,
      resources[HarnessEepromIndex].readWriteBufferSize,
      configuration->harnessEepromMetadataErd,
      HarnessEepromClientVersion,
      timerModule,
      TimerTicksBetweenRetriesInMsec,
      HarnessEepromRetriesBeforeErase,
      false);

   for(uint8_t i = 0; i < AsyncDataSourceCount; i++)
   {
      instance->_private.asyncDataSourceCompositeComponents[i].asyncDataSource = &instance->_private.asyncDataSourceEeprom[i].interface;
   }

   AsyncDataSource_CompositeStatic_Init(
      &instance->_private.asyncCompositeStatic,
      instance->_private.asyncDataSourceCompositeComponents,
      AsyncDataSourceCount);
}

I_AsyncDataSource_t *NonVolatileAsyncDataSource_GetAsyncDataSource(NonVolatileAsyncDataSource_t *instance)
{
   return &instance->_private.asyncCompositeStatic.interface;
}
