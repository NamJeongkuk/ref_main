/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DamperMaxOpenTimeMonitor.h"
#include "DataModelErdPointerAccess.h"
#include "utils.h"
#include "SystemErds.h"
#include "Constants_Time.h"

static void MaxOpenDamperTimeCheck(DamperMaxOpenTime_t *, DamperPosition_t);

enum
{
   ZeroMinutes = 0,
};

static void SetMaxOpenDamperRequestPositionVote(DamperMaxOpenTime_t *instance, DamperVotedPosition_t damperMaxOpenVoteCareAndPosition)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperPositionMaxOpenTimeVoteErd,
      &damperMaxOpenVoteCareAndPosition);
}

static void MaxOpenDamperTimerExpired(void *context)
{
   DamperMaxOpenTime_t *instance = context;
   DamperVotedPosition_t damperMaxOpenVoteCareAndPosition;

   damperMaxOpenVoteCareAndPosition.care = Vote_Care;
   damperMaxOpenVoteCareAndPosition.position = DamperPosition_Closed;

   SetMaxOpenDamperRequestPositionVote(instance, damperMaxOpenVoteCareAndPosition);
}

static void StartMaxOpenDamperTimer(DamperMaxOpenTime_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxOpenTimer,
      instance->_private.maxOpenTimerTicks,
      MaxOpenDamperTimerExpired,
      instance);
}

static void StopMaxOpenDamperTimer(DamperMaxOpenTime_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxOpenTimer);
}

static DamperPosition_t CurrentDamperPosition(DamperMaxOpenTime_t *instance)
{
   DamperPosition_t damperCurrentPosition;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->damperCurrentPositionErd,
      &damperCurrentPosition);

   return damperCurrentPosition;
}

static void MaxOpenDamperTimeCheck(DamperMaxOpenTime_t *instance, DamperPosition_t currentDamperPosition)
{
   if(currentDamperPosition == DamperPosition_Closed)
   {
      DamperVotedPosition_t damperCurrentVotePosition;

      StopMaxOpenDamperTimer(instance);

      damperCurrentVotePosition.care = Vote_DontCare;
      damperCurrentVotePosition.position = DamperPosition_Closed;

      SetMaxOpenDamperRequestPositionVote(instance, damperCurrentVotePosition);
   }
   else
   {
      StartMaxOpenDamperTimer(instance);
   }
}

static void OnDataModelChange(void *context, const void *data)
{
   DamperMaxOpenTime_t *instance = context;
   const DamperPosition_t *damperPosition = data;

   MaxOpenDamperTimeCheck(instance, *damperPosition);
}

void DamperMaxOpenTimeMonitor_Init(
   DamperMaxOpenTime_t *instance,
   I_DataModel_t *dataModel,
   const DamperMaxOpenTimeConfiguration_t *configuration,
   const DamperData_t *damperData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.configuration = configuration;
   instance->_private.maxOpenTimerTicks =
      damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN;

   DamperPosition_t damperPosition = CurrentDamperPosition(instance);

   if(instance->_private.maxOpenTimerTicks != ZeroMinutes)
   {
      EventSubscription_Init(
         &instance->_private.damperCurrentPositionSubscription,
         instance,
         OnDataModelChange);

      DataModel_Subscribe(
         instance->_private.dataModel,
         instance->_private.configuration->damperCurrentPositionErd,
         &instance->_private.damperCurrentPositionSubscription);

      MaxOpenDamperTimeCheck(instance, damperPosition);
   }
}
