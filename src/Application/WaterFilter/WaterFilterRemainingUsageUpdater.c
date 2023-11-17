/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "WaterFilterRemainingUsageUpdater.h"
#include "WaterFilterRemainingUsage.h"
#include "CalendarUsageInSeconds.h"
#include "VolumeInOuncesX100.h"
#include "Constants_Time.h"
#include "utils.h"

static uint8_t CalculatePercentUsageRemaining(
   WaterFilterRemainingUsageUpdater_t *instance,
   const VolumeInOuncesX100_t totalWaterVolumeUsageInOuncesx100,
   const CalendarUsageInSeconds_t waterFilterCalendarUsageInSeconds)
{
   uint8_t volumePercentUsageRemaining = (TRUNCATE_UNSIGNED_SUBTRACTION(
                                             instance->_private.commonFilterData->filterRatedVolumeInOuncesX100,
                                             totalWaterVolumeUsageInOuncesx100) *
                                            100) /
      instance->_private.commonFilterData->filterRatedVolumeInOuncesX100;

   uint8_t timePercentUsageRemaining = (TRUNCATE_UNSIGNED_SUBTRACTION(
                                           instance->_private.commonFilterData->filterRatedLifeInMinutes,
                                           waterFilterCalendarUsageInSeconds / SECONDS_PER_MINUTE) *
                                          100) /
      instance->_private.commonFilterData->filterRatedLifeInMinutes;

   return MIN(volumePercentUsageRemaining, timePercentUsageRemaining);
}

static uint16_t CalculateDaysUsageRemaining(
   WaterFilterRemainingUsageUpdater_t *instance,
   const uint8_t percentUsageRemaining)
{
   return UNSIGNED_DIVIDE_WITH_CEILING(((percentUsageRemaining * instance->_private.commonFilterData->filterRatedLifeInMinutes) / 100), MINUTES_PER_DAY);
}

static void UpdateWaterFilterRemainingUsage(WaterFilterRemainingUsageUpdater_t *instance)
{
   VolumeInOuncesX100_t currentTotalWaterVolumeUsageInOuncesx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->totalWaterVolumeUsageInOuncesx100Erd,
      &currentTotalWaterVolumeUsageInOuncesx100);

   CalendarUsageInSeconds_t currentWaterFilterCalendarUsageInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waterFilterCalendarUsageInSecondsErd,
      &currentWaterFilterCalendarUsageInSeconds);

   WaterFilterRemainingUsage_t waterFilterRemainingUsage;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waterFilterRemainingUsageErd,
      &waterFilterRemainingUsage);

   waterFilterRemainingUsage.percentUsageRemaining =
      CalculatePercentUsageRemaining(instance, currentTotalWaterVolumeUsageInOuncesx100, currentWaterFilterCalendarUsageInSeconds);
   waterFilterRemainingUsage.daysUsageRemaining =
      CalculateDaysUsageRemaining(instance, waterFilterRemainingUsage.percentUsageRemaining);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFilterRemainingUsageErd,
      &waterFilterRemainingUsage);
}

static void DataModelChanged(void *context, const void *_args)
{
   WaterFilterRemainingUsageUpdater_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;
   Erd_t erd = args->erd;

   if((erd == instance->_private.config->totalWaterVolumeUsageInOuncesx100Erd) ||
      (erd == instance->_private.config->waterFilterCalendarUsageInSecondsErd))
   {
      UpdateWaterFilterRemainingUsage(instance);
   }
}

void WaterFilterRemainingUsageUpdater_Init(
   WaterFilterRemainingUsageUpdater_t *instance,
   I_DataModel_t *dataModel,
   const WaterFilterRemainingUsageUpdaterConfig_t *config,
   const CommonFilterData_t *commonFilterData)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.commonFilterData = commonFilterData;

   UpdateWaterFilterRemainingUsage(instance);

   EventSubscription_Init(
      &instance->_private.dataModelChangedSubscription,
      instance,
      DataModelChanged);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelChangedSubscription);
}
