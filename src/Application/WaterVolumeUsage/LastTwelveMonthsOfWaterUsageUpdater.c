/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "LastTwelveMonthsOfWaterUsageUpdater.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Time.h"
#include "SystemErds.h"
#include "RfidTypes.h"

enum
{
   RecentMonthIndex = 0,
   OneMinute = 1,
   OuncesX100PerGallon = 128 * 100
};

static VolumeInOuncesX100_t CurrentWaterFilterVolumeUsageInOuncesX100(LastTwelveMonthsOfWaterUsageUpdater_t *instance)
{
   VolumeInOuncesX100_t currentWaterFilterVolumeUsageInOuncesX100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->currentWaterFilterVolumeUsageInOuncesX100Erd,
      &currentWaterFilterVolumeUsageInOuncesX100);

   return currentWaterFilterVolumeUsageInOuncesX100;
}

static VolumeInOuncesX100_t PreviousWaterFilterVolumeUsageInOuncesX100(LastTwelveMonthsOfWaterUsageUpdater_t *instance)
{
   VolumeInOuncesX100_t previousWaterFilterVolumeUsageInOuncesX100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->previousWaterFilterVolumeUsageInOuncesX100Erd,
      &previousWaterFilterVolumeUsageInOuncesX100);

   return previousWaterFilterVolumeUsageInOuncesX100;
}

static void ShiftUsageArrayElementsToTheRight(RfidTwelveMonthsGallonUsage_t *lastTwelveMonthsWaterUsageInGallons)
{
   for(int8_t i = (MonthlyGallonUsageSizeInBytes - 1); i > 0; i--)
   {
      lastTwelveMonthsWaterUsageInGallons->gallonUsage[i] = lastTwelveMonthsWaterUsageInGallons->gallonUsage[i - 1];
   }
}

static void StoreNewMonthWaterUsageInGallons(LastTwelveMonthsOfWaterUsageUpdater_t *instance)
{
   RfidTwelveMonthsGallonUsage_t lastTwelveMonthsWaterUsageInGallons;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->lastTwelveMonthsOfWaterUsageInGallonsEepromErd,
      &lastTwelveMonthsWaterUsageInGallons);

   ShiftUsageArrayElementsToTheRight(&lastTwelveMonthsWaterUsageInGallons);

   if(CurrentWaterFilterVolumeUsageInOuncesX100(instance) == PreviousWaterFilterVolumeUsageInOuncesX100(instance))
   {
      lastTwelveMonthsWaterUsageInGallons.gallonUsage[RecentMonthIndex] = 0;
   }
   else
   {
      uint8_t newMonthWaterUsageInGallons =
         MIN(
            TRUNCATE_UNSIGNED_SUBTRACTION(
               CurrentWaterFilterVolumeUsageInOuncesX100(instance),
               PreviousWaterFilterVolumeUsageInOuncesX100(instance)) /
               OuncesX100PerGallon,
            UINT8_MAX);
      lastTwelveMonthsWaterUsageInGallons.gallonUsage[RecentMonthIndex] = newMonthWaterUsageInGallons;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->lastTwelveMonthsOfWaterUsageInGallonsEepromErd,
      &lastTwelveMonthsWaterUsageInGallons);
}

static void UpdatePreviousWithCurrentWaterFilterVolumeUsageInOuncesX100(LastTwelveMonthsOfWaterUsageUpdater_t *instance)
{
   VolumeInOuncesX100_t waterFilterVolumeUsageInOuncesX100;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->currentWaterFilterVolumeUsageInOuncesX100Erd,
      &waterFilterVolumeUsageInOuncesX100);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->previousWaterFilterVolumeUsageInOuncesX100Erd,
      &waterFilterVolumeUsageInOuncesX100);
}

static void ClearWaterFilterMonthTimeInMinutes(LastTwelveMonthsOfWaterUsageUpdater_t *instance)
{
   uint16_t waterFilterMonthTimeInMinutes = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->currentWaterFilterMonthTimeInMinutesErd,
      &waterFilterMonthTimeInMinutes);
}

static uint16_t CurrentWaterFilterMonthTimeInMinutes(LastTwelveMonthsOfWaterUsageUpdater_t *instance)
{
   uint16_t currentWaterFilterMonthTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->currentWaterFilterMonthTimeInMinutesErd,
      &currentWaterFilterMonthTimeInMinutes);

   return currentWaterFilterMonthTimeInMinutes;
}

static void IncrementWaterFilterMonthTimeInMinutes(void *context)
{
   LastTwelveMonthsOfWaterUsageUpdater_t *instance = context;
   uint16_t currentWaterFilterMonthTimeInMinutes =
      CurrentWaterFilterMonthTimeInMinutes(instance) + OneMinute;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->currentWaterFilterMonthTimeInMinutesErd,
      &currentWaterFilterMonthTimeInMinutes);

   if(currentWaterFilterMonthTimeInMinutes >= instance->_private.rfidFilterData->filterMonthInMinutes)
   {
      StoreNewMonthWaterUsageInGallons(instance);
      UpdatePreviousWithCurrentWaterFilterVolumeUsageInOuncesX100(instance);
      ClearWaterFilterMonthTimeInMinutes(instance);
   }
}

void LastTwelveMonthsOfWaterUsageUpdater_Init(
   LastTwelveMonthsOfWaterUsageUpdater_t *instance,
   I_DataModel_t *dataModel,
   const LastTwelveMonthsOfWaterUsageUpdaterConfig_t *config,
   const RfidFilterData_t *rfidFilterData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.rfidFilterData = rfidFilterData;

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         dataModel,
         Erd_TimerModule),
      &instance->_private.monthlyWaterUsageTimer,
      MSEC_PER_MIN,
      IncrementWaterFilterMonthTimeInMinutes,
      instance);
}
