/*!
 * @file
 * @brief Polls flow meter counts and calculates amount of water dispensed
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FLOWMETERMONITOR_H
#define FLOWMETERMONITOR_H

#include "I_DataModel.h"
#include "FlowMeterData.h"
#include "Timer.h"

typedef struct
{
   Erd_t flowMeterMonitoringRequest; // FlowMeterMonitoringRequest_t
   Erd_t flowMeterCountsErd; // InputCaptureCounts_t
   Erd_t flowMeterWaterDispensedOzX100Erd; // uint32_t
} FlowMeterMonitorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const FlowMeterMonitorConfig_t *config;
      const FlowMeterData_t *flowMeterData;
      Timer_t pollTimer;
      EventSubscription_t flowMeterMonitoringRequestSubscription;
      uint32_t previousFlowMeterCounts;
      uint32_t flowMeterCountsSinceBeginningMonitoring;
   } _private;
} FlowMeterMonitor_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param flowMeterData
 */
void FlowMeterMonitor_Init(
   FlowMeterMonitor_t *instance,
   I_DataModel_t *dataModel,
   const FlowMeterMonitorConfig_t *config,
   const FlowMeterData_t *flowMeterData);

#endif
