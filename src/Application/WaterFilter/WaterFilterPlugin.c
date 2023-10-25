/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "WaterFilterPlugin.h"

static const WaterFilterCalendarUsageUpdaterConfig_t waterFilterCalendarUsageUpdaterConfig = {
   .waterFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .lifetimeTotalWaterUsageInOuncesX100Erd = Erd_LifetimeTotalWaterVolumeUsageInOuncesX100,
   .serviceDiagnosticsRunTimeInMinutesErd = Erd_ServiceDiagnosticsRunTimeInMinutes
};

void WaterFilterPlugin_Init(
   WaterFilterPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   WaterFilterCalendarUsageUpdater_Init(
      &instance->_private.waterFilterCalendarUsageUpdater,
      dataModel,
      &waterFilterCalendarUsageUpdaterConfig,
      PersonalityParametricData_Get(dataModel)->filterData->commonFilterData);

   RfidCommunicationPlugin_Init(
      &instance->_private.rfidCommunicationPlugin,
      dataModel);
}
