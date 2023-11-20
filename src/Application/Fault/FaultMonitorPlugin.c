/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FaultMonitorPlugin.h"
#include "SystemErds.h"

static const FreshFoodNoFreezeLimitTrippedFaultDetectorConfig_t freshFoodNoFreezeLimitTrippedFaultDetectorConfig = {
   .calculateGridLineErd = Erd_Grid_CalculatedGridLines,
   .freshFoodFilteredTemperatureErd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freshFoodNoFreezeLimitTrippedFaultErd = Erd_FreshFoodNoFreezeLimitTrippedFault
};

void FaultMonitorPlugin_Init(FaultMonitorPlugin_t *instance, I_DataModel_t *dataModel)
{
   FreshFoodNoFreezeLimitTrippedFaultDetector_Init(
      &instance->freshFoodNoFreezeLimitTrippedFaultDetector,
      dataModel,
      &freshFoodNoFreezeLimitTrippedFaultDetectorConfig);
}
