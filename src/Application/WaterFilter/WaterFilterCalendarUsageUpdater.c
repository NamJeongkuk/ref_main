/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "WaterFilterCalendarUsageUpdater.h"
#include "DataModelErdPointerAccess.h"
#include "Signal.h"
#include "Constants_Time.h"
#include "utils.h"

enum
{
   WaterFilterCalendarUsageUpdateIntervalMinutes = 1,
   WaterFilterCalendarUsageUpdateIntervalSeconds = (WaterFilterCalendarUsageUpdateIntervalMinutes * SECONDS_PER_MINUTE),
   WaterFilterCalendarUsageUpdateIntervalMsecs = (WaterFilterCalendarUsageUpdateIntervalMinutes * MSEC_PER_MIN),
};

static VolumeInOuncesX100_t TotalLifetimeWaterUsageInOzX100(WaterFilterCalendarUsageUpdater_t *instance)
{
   VolumeInOuncesX100_t currentTotalLifetimeWaterUsageInOuncesX100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->lifetimeTotalWaterUsageInOuncesX100Erd,
      &currentTotalLifetimeWaterUsageInOuncesX100);

   return currentTotalLifetimeWaterUsageInOuncesX100;
}

static ApplianceRunTimeMinutes_t CurrentServiceDiagnosticsRunTimeInMinutes(WaterFilterCalendarUsageUpdater_t *instance)
{
   ApplianceRunTimeMinutes_t currentServiceDiagnosticsRunTimeInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->serviceDiagnosticsRunTimeInMinutesErd,
      &currentServiceDiagnosticsRunTimeInMinutes);

   return currentServiceDiagnosticsRunTimeInMinutes;
}

static CalendarUsageInSeconds_t CurrentWaterFilterCalendarUsageTimeInSeconds(WaterFilterCalendarUsageUpdater_t *instance)
{
   CalendarUsageInSeconds_t currentWaterFilterCalendarUsageTimeInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waterFilterCalendarUsageInSecondsErd,
      &currentWaterFilterCalendarUsageTimeInSeconds);

   return currentWaterFilterCalendarUsageTimeInSeconds;
}

static void IncrementWaterFilterCalendarUsageByWaterFilterCalendarUsageUpdateIntervalSeconds(void *context)
{
   WaterFilterCalendarUsageUpdater_t *instance = context;

   CalendarUsageInSeconds_t newWaterFilterCalendarUsageTimeInSeconds =
      TRUNCATE_UNSIGNED_ADDITION(CurrentWaterFilterCalendarUsageTimeInSeconds(instance), WaterFilterCalendarUsageUpdateIntervalSeconds, UINT32_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFilterCalendarUsageInSecondsErd,
      &newWaterFilterCalendarUsageTimeInSeconds);
}

static void StartIncrementWaterFilterCalendarUsageTimer(WaterFilterCalendarUsageUpdater_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.waterFilterCalendarUsageTimer,
      WaterFilterCalendarUsageUpdateIntervalMsecs,
      IncrementWaterFilterCalendarUsageByWaterFilterCalendarUsageUpdateIntervalSeconds,
      instance);
}

static void DataModelChanged(void *context, const void *_args)
{
   WaterFilterCalendarUsageUpdater_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;
   Erd_t erd = args->erd;

   if(erd == instance->_private.config->lifetimeTotalWaterUsageInOuncesX100Erd)
   {
      const VolumeInOuncesX100_t *currentTotalLifetimeWaterUsageInOuncesX100 = args->data;

      if(*currentTotalLifetimeWaterUsageInOuncesX100 >
         instance->_private.commonFilterData->minimumVolumeNeededToStartFilterLifeTimerInOunces * 100)
      {
         DataModel_UnsubscribeAll(
            instance->_private.dataModel,
            &instance->_private.dataModelChangedSubscription);

         StartIncrementWaterFilterCalendarUsageTimer(instance);
      }
   }
   else if(erd == instance->_private.config->serviceDiagnosticsRunTimeInMinutesErd)
   {
      const ApplianceRunTimeMinutes_t *currentServiceDiagnosticsRunTimeInMinutes = args->data;

      if(*currentServiceDiagnosticsRunTimeInMinutes >
         instance->_private.commonFilterData->waterFilterCalendarUsageDelayForLowUsageInMinutes)
      {
         DataModel_UnsubscribeAll(
            instance->_private.dataModel,
            &instance->_private.dataModelChangedSubscription);

         StartIncrementWaterFilterCalendarUsageTimer(instance);
      }
   }
}

void WaterFilterCalendarUsageUpdater_Init(
   WaterFilterCalendarUsageUpdater_t *instance,
   I_DataModel_t *dataModel,
   const WaterFilterCalendarUsageUpdaterConfig_t *config,
   const CommonFilterData_t *commonFilterData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.commonFilterData = commonFilterData;

   if((CurrentServiceDiagnosticsRunTimeInMinutes(instance) >
         instance->_private.commonFilterData->waterFilterCalendarUsageDelayForLowUsageInMinutes) ||
      (TotalLifetimeWaterUsageInOzX100(instance) >
         instance->_private.commonFilterData->minimumVolumeNeededToStartFilterLifeTimerInOunces * 100))
   {
      StartIncrementWaterFilterCalendarUsageTimer(instance);
   }
   else
   {
      EventSubscription_Init(
         &instance->_private.dataModelChangedSubscription,
         instance,
         DataModelChanged);

      DataModel_SubscribeAll(dataModel, &instance->_private.dataModelChangedSubscription);
   }
}
