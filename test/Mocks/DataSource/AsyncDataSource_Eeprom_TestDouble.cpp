/*!
 * @file
 * @brief TDD DataSource/Model for the next-gen refrigerator. Provides AsyncDataSource test double using Eeprom_model.
 * Writes specified default values to ERDs on init.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include <cstring>
}

#include "AsyncDataSource_Eeprom_TestDouble.h"
#include "Eeprom_Model.h"

enum
{
   NumberOfRecordBackups = 1,

   writeTimeInMs = 5,
   readTimeInMs = 10,
   eraseTimeInMs = 20,

   ClientVersion = 0,

   byteAlignment = 1,
   startAddress = 0,

   timerTicksBetweenRetries = 100,
   numberOfRetriesBeforeErase = 10,

   Erd_NvMetadata = 0
};

#define EXPAND_AS_ASYNC_MAP_ELEMENTS(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_NVALL_, StorageType)                                                                                       \
   ({ Name COMMA sizeof(DataType) COMMA NumberOfRecordBackups } COMMA)

static const AsyncDataSource_EepromErdInfo_t asyncMapElements[] = { { .erd = Erd_NvMetadata, .size = sizeof(AsyncDataSource_EepromMetadata_t), .backupCount = 1 },
   ERD_TABLE(EXPAND_AS_ASYNC_MAP_ELEMENTS) };

static const ConstArrayMap_BinarySearchConfiguration_t asyncMapConfiguration = {
   asyncMapElements,
   NUM_ELEMENTS(asyncMapElements),
   ELEMENT_SIZE(asyncMapElements),
   sizeof(asyncMapElements[0].erd),
   OFFSET_OF(AsyncDataSource_EepromErdInfo_t, erd),
   IS_SIGNED(Erd_t)
};

static void ClearEeprom(Instance_t *instance)
{
   memset(instance->_private.eeprom, 0, sizeof(instance->_private.eeprom));
}

void AsyncDataSource_Eeprom_TestDouble_Init(
   Instance_t *instance,
   TimerModule_t *timerModule,
   I_Crc16Calculator_t *crc16Calculator,
   bool clearEeprom)
{
   ConstArrayMap_BinarySearch_Init(&instance->_private.asyncMap, &asyncMapConfiguration);

   InputGroup_NonVolatileDataSourceDefaultData_Init(
      &instance->_private.defaultDataInputGroup,
      asyncMapElements,
      NUM_ELEMENTS(asyncMapElements));

   if(clearEeprom)
   {
      ClearEeprom(instance);
   }

   Eeprom_Model_Init(
      &instance->_private.eepromModel,
      instance->_private.eeprom,
      byteAlignment,
      0,
      sizeof(instance->_private.eeprom),
      timerModule,
      readTimeInMs,
      writeTimeInMs,
      eraseTimeInMs);

   AsyncDataSource_Eeprom_Init(
      &instance->_private.asyncDataSourceEeprom,
      &instance->_private.asyncMap.interface,
      &instance->_private.eepromModel.interface,
      crc16Calculator,
      &instance->_private.defaultDataInputGroup.interface,
      &instance->_private.asyncReadWriteBuffer,
      sizeof(instance->_private.asyncReadWriteBuffer),
      Erd_NvMetadata,
      ClientVersion,
      timerModule,
      timerTicksBetweenRetries,
      numberOfRetriesBeforeErase);
}

I_AsyncDataSource_t *AsyncDataSource_Eeprom_TestDouble_GetAsyncDataSource(Instance_t *instance)
{
   return &instance->_private.asyncDataSourceEeprom.interface;
}
