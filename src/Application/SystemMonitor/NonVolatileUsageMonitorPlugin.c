/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "NonVolatileUsageMonitorPlugin.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"

#define EXPAND_AS_CONFIGURATION_MAP_ENTRY(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)                                                                                                 \
   ({ Name COMMA YearlyWrites } COMMA)

static const NonVolatileUsageMonitorErdConfigurationEntry_t erdConfigurationMapEntries[] = {
   ERD_TABLE(EXPAND_AS_CONFIGURATION_MAP_ENTRY)
};

static const ConstArrayMap_BinarySearchConfiguration_t erdConfigurationMapConfiguration = {
   .array = erdConfigurationMapEntries,
   .elementCount = ELEMENT_COUNT(erdConfigurationMapEntries),
   .elementSize = ELEMENT_SIZE(erdConfigurationMapEntries),
   .keySize = sizeof(Erd_t),
   .keyOffset = OFFSET_OF(NonVolatileUsageMonitorErdConfigurationEntry_t, erd),
   .keyIsSigned = IS_SIGNED(Erd_t)
};

static const NonVolatileUsageMonitorConfiguration_t configuration = {
   .monitoringPeriod = MSEC_PER_MIN * MINUTES_PER_DAY * 7,
   .warningThresholdPercent = 80,
   .errorThresholdPercent = 200,
   .detailsRequestErd = Erd_NvUsageDetailsRequestErd,
   .detailsResponseErd = Erd_NvUsageDetailsResponseErd,
   .warningOrErrorDetailsErd = Erd_NvUsageWarningOrErrorDetailsErd,
   .warningThresholdExceededErd = Erd_NvUsageWarningThresholdExceededErd,
   .errorThresholdExceededErd = Erd_NvUsageErrorThresholdExceededErd
};

void NonVolatileUsageMonitorPlugin_Init(NonVolatileUsageMonitorPlugin_t *instance, I_DataModel_t *dataModel)
{
   ConstArrayMap_BinarySearch_Init(
      &instance->_private.erdConfigurationMap,
      &erdConfigurationMapConfiguration);

   NonVolatileUsageMonitor_Init(
      &instance->_private.nonVolatileUsageMonitor,
      &configuration,
      DataModel_AsDataSource(dataModel),
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &instance->_private.erdConfigurationMap.interface,
      instance->_private.writesInPeriodWorkingBuffer,
      instance->_private.writesInPeriodResultsBuffer);
}
