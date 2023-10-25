/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "WaterFilterLifeStatusUpdater.h"
#include "WaterFilterLifeStatus.h"
#include "Constants_Time.h"

enum
{
   ReplacementCalendarUsagePercent = 95,
   ReplacementVolumeUsagePercent = 95
};

static void UpdateWaterFilterLifeStatus(WaterFilterLifeStatusUpdater_t *instance)
{
   WaterFilterLifeStatus_t status = WaterFilterLifeStatus_NoOrderNecessary;

   uint32_t totalWaterVolumeUsageInOunces;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->totalWaterVolumeUsageInOuncesX100Erd,
      &totalWaterVolumeUsageInOunces);

   CalendarUsageInSeconds_t waterFilterCalendarUsageInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->filterCalendarUsageInSecondsErd,
      &waterFilterCalendarUsageInSeconds);

   if((totalWaterVolumeUsageInOunces >= instance->_private.data->filterRatedVolumeInOuncesX100) ||
      (waterFilterCalendarUsageInSeconds >= (instance->_private.data->filterRatedLifeInMinutes * SECONDS_PER_MINUTE)))
   {
      status = WaterFilterLifeStatus_Expired;
   }
   else if(totalWaterVolumeUsageInOunces > ((ReplacementVolumeUsagePercent * instance->_private.data->filterRatedVolumeInOuncesX100) / 100) ||
      waterFilterCalendarUsageInSeconds > (((ReplacementCalendarUsagePercent * instance->_private.data->filterRatedLifeInMinutes * SECONDS_PER_MINUTE) / 100)))
   {
      status = WaterFilterLifeStatus_Replace;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFilterLifeStatusErd,
      &status);
}

static void DataModelChanged(void *context, const void *args)
{
   WaterFilterLifeStatusUpdater_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->totalWaterVolumeUsageInOuncesX100Erd ||
      erd == instance->_private.config->filterCalendarUsageInSecondsErd)
   {
      UpdateWaterFilterLifeStatus(instance);
   }
}

void WaterFilterLifeStatusUpdater_Init(
   WaterFilterLifeStatusUpdater_t *instance,
   I_DataModel_t *dataModel,
   const WaterFilterLifeStatusUpdaterConfig_t *config,
   const CommonFilterData_t *data)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.data = data;

   UpdateWaterFilterLifeStatus(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);
}
