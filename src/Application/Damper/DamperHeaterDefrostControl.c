/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DamperHeaterDefrostControl.h"
#include "HeaterVotedDutyCycle.h"
#include "HeaterVotedState.h"
#include "Constants_Time.h"
#include "Vote.h"
#include "utils.h"

static void SetDefrostSyncVoteTo(DamperHeaterDefrostControl_t *instance, PercentageDutyCycle_t dutyCycle, Vote_t care)
{
   HeaterVotedDutyCycle_t votedState;

   votedState.dutyCycle = dutyCycle;
   votedState.care = care;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->damperHeaterDefrostHeaterSyncVoteErd,
      &votedState);
}

static void SetDamperHeaterVoteToDontCare(void *context)
{
   DamperHeaterDefrostControl_t *instance = context;
   SetDefrostSyncVoteTo(instance, PercentageDutyCycle_Min, Vote_DontCare);
}

static void StartDefrostOnTimer(DamperHeaterDefrostControl_t *instance)
{
   if(instance->_private.damperHeaterParametricData->damperHeaterDefrostOnTimeInMinutes > 0)
   {
      SetDefrostSyncVoteTo(instance, PercentageDutyCycle_Max, Vote_Care);

      TimerModule_StartOneShot(
         instance->_private.timerModule,
         &instance->_private.timer,
         instance->_private.damperHeaterParametricData->damperHeaterDefrostOnTimeInMinutes * MSEC_PER_MIN,
         SetDamperHeaterVoteToDontCare,
         instance);
   }
   else
   {
      SetDefrostSyncVoteTo(instance, PercentageDutyCycle_Min, Vote_DontCare);
   }
}

static void MatchDefrostHeaterState(DamperHeaterDefrostControl_t *instance)
{
   bool defrostState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostHeaterStateErd,
      &defrostState);

   if(defrostState)
   {
      SetDefrostSyncVoteTo(instance, PercentageDutyCycle_Max, Vote_Care);
   }
   else
   {
      SetDefrostSyncVoteTo(instance, PercentageDutyCycle_Min, Vote_DontCare);
   }
}

static void DefrostHeaterStateChanged(void *context, const void *args)
{
   DamperHeaterDefrostControl_t *instance = context;
   IGNORE(args);

   MatchDefrostHeaterState(instance);
}

static void DefrostHeaterVoteChanged(void *context, const void *args)
{
   DamperHeaterDefrostControl_t *instance = context;
   const HeaterVotedState_t *defrostHeaterVoteData = args;

   if(instance->_private.damperHeaterParametricData->damperHeaterOnWhenDefrostHeaterIsOnEnable)
   {
      if(defrostHeaterVoteData->care)
      {
         MatchDefrostHeaterState(instance);
         DataModel_Subscribe(
            instance->_private.dataModel,
            instance->_private.config->defrostHeaterStateErd,
            &instance->_private.defrostHeaterStateSubscription);
      }
      else
      {
         SetDefrostSyncVoteTo(instance, PercentageDutyCycle_Min, Vote_DontCare);
         DataModel_Unsubscribe(
            instance->_private.dataModel,
            instance->_private.config->defrostHeaterStateErd,
            &instance->_private.defrostHeaterStateSubscription);
      }
   }
   else if(defrostHeaterVoteData->state && (defrostHeaterVoteData->care == Vote_Care))
   {
      StartDefrostOnTimer(instance);
   }
}

void DamperHeaterDefrostControl_Init(
   DamperHeaterDefrostControl_t *instance,
   I_DataModel_t *dataModel,
   const DamperHeaterDefrostControlConfig_t *config,
   TimerModule_t *timerModule,
   const DamperHeaterData_t *damperHeaterData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.timerModule = timerModule;
   instance->_private.damperHeaterParametricData = damperHeaterData;

   EventSubscription_Init(
      &instance->_private.defrostHeaterVoteSubscription,
      instance,
      DefrostHeaterVoteChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->defrostHeaterVoteErd,
      &instance->_private.defrostHeaterVoteSubscription);

   EventSubscription_Init(
      &instance->_private.defrostHeaterStateSubscription,
      instance,
      DefrostHeaterStateChanged);
}
