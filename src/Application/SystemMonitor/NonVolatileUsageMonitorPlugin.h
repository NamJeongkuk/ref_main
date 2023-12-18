/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NONVOLATILEUSAGEMONITORPLUGIN_H
#define NONVOLATILEUSAGEMONITORPLUGIN_H

#include "ConstArrayMap_BinarySearch.h"
#include "I_DataModel.h"
#include "NonVolatileUsageMonitor.h"
#include "SystemErds.h"

#define EXPAND_AS_NV_ERD_COUNT_STRUCT_ELEMENT(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)                                                                                                   \
   (uint8_t Name;)

typedef struct
{
   ERD_TABLE(EXPAND_AS_NV_ERD_COUNT_STRUCT_ELEMENT)
} NonVolatileUsageMonitorPluginErdCount_t;

typedef struct
{
   struct
   {
      NonVolatileUsageMonitor_t nonVolatileUsageMonitor;
      ConstArrayMap_BinarySearch_t erdConfigurationMap;
      uint16_t writesInPeriodWorkingBuffer[sizeof(NonVolatileUsageMonitorPluginErdCount_t)];
      uint16_t writesInPeriodResultsBuffer[sizeof(NonVolatileUsageMonitorPluginErdCount_t)];
   } _private;
} NonVolatileUsageMonitorPlugin_t;

void NonVolatileUsageMonitorPlugin_Init(NonVolatileUsageMonitorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
