/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PeriodicDamperCycling.h"
#include "DataModelErdPointerAccess.h"
#include "utils.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"

static void MaxDamperTimerExpired(void *context)
{
   PeriodicDamperCycling_t *instance = context;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperFullCycleRequestErd,
      set);
}

static void StartMaxOpenDamperTimer(PeriodicDamperCycling_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxTimer,
      instance->_private.damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN,
      MaxDamperTimerExpired,
      instance);
}

static void StartMaxClosedDamperTimer(PeriodicDamperCycling_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxTimer,
      instance->_private.damperData->maxTimeForDamperToBeClosedInMinutes * MSEC_PER_MIN,
      MaxDamperTimerExpired,
      instance);
}

static void StopMaxTimer(PeriodicDamperCycling_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxTimer);
}

static DamperPosition_t CurrentDamperPosition(PeriodicDamperCycling_t *instance)
{
   DamperPosition_t damperCurrentPosition;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->damperCurrentPositionErd,
      &damperCurrentPosition);

   return damperCurrentPosition;
}

static void MaxDamperTimeCheck(PeriodicDamperCycling_t *instance, DamperPosition_t currentDamperPosition)
{
   if(currentDamperPosition == DamperPosition_Closed)
   {
      if(instance->_private.damperData->maxTimeForDamperToBeClosedInMinutes != 0)
      {
         StartMaxClosedDamperTimer(instance);
      }
      else
      {
         StopMaxTimer(instance);
      }
   }
   else if(currentDamperPosition == DamperPosition_Open)
   {
      if(instance->_private.damperData->maxTimeForDamperToBeOpenInMinutes != 0)
      {
         StartMaxOpenDamperTimer(instance);
      }
      else
      {
         StopMaxTimer(instance);
      }
   }
}

static void OnDamperPositionChange(void *context, const void *data)
{
   PeriodicDamperCycling_t *instance = context;
   const DamperPosition_t *damperPosition = data;

   MaxDamperTimeCheck(instance, *damperPosition);
}

void PeriodicDamperCycling_Init(
   PeriodicDamperCycling_t *instance,
   I_DataModel_t *dataModel,
   const PeriodicDamperCyclingConfiguration_t *configuration,
   const DamperData_t *damperData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.configuration = configuration;
   instance->_private.damperData = damperData;

   if((instance->_private.damperData->maxTimeForDamperToBeOpenInMinutes != 0) ||
      (instance->_private.damperData->maxTimeForDamperToBeClosedInMinutes != 0))
   {
      MaxDamperTimeCheck(instance, CurrentDamperPosition(instance));

      EventSubscription_Init(
         &instance->_private.damperCurrentPositionSubscription,
         instance,
         OnDamperPositionChange);

      DataModel_Subscribe(
         instance->_private.dataModel,
         instance->_private.configuration->damperCurrentPositionErd,
         &instance->_private.damperCurrentPositionSubscription);
   }
}
