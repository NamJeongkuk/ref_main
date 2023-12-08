/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideWaterFilterTrackingPlugin.h"
#include "SystemErds.h"

static const WaterFilterLifeStatusUpdaterConfig_t waterFilterLifeStatusUpdaterConfig = {
   .waterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100,
   .filterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .waterFilterLifeStatusErd = Erd_WaterFilterLifeStatus
};

void SideBySideWaterFilterTrackingPlugin_Init(
   SideBySideWaterFilterTrackingPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const CommonFilterData_t *data = PersonalityParametricData_Get(dataModel)->filterData->commonFilterData;
   WaterFilterLifeStatusUpdater_Init(
      &instance->_private.waterFilterLifeStatusUpdater,
      dataModel,
      &waterFilterLifeStatusUpdaterConfig,
      data);
}
