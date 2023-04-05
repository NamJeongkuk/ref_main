/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "FlowMeterPlugin.h"

static const FlowMeterMonitorConfig_t flowMeterMonitorConfig = {
   .flowMeterMonitoringRequest = Erd_FlowMeterMonitoringRequest,
   .flowMeterCountsErd = Erd_FlowMeter_InputCaptureCount,
   .flowMeterWaterDispensedOzX100Erd = Erd_FlowMeterWaterDispensedOzX100
};

void FlowMeterPlugin_Init(
   FlowMeterPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   FlowMeterMonitor_Init(
      &instance->_private.flowMeterMonitor,
      dataModel,
      &flowMeterMonitorConfig,
      PersonalityParametricData_Get(dataModel)->flowMeterData);
}
