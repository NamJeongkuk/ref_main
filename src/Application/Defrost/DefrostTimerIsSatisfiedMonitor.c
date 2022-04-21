/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostTimerIsSatisfiedMonitor.h"
#include "DefrostTimerIsSatisfiedMonitorRequest.h"
#include "DefrostTimerIsSatisfiedMonitorFsmState.h"
#include "PersonalityParametricData.h"
#include "EnergyDemandLevel.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "utils.h"

enum
{
   Signal_EnableRequested = Fsm_UserSignalStart,
   Signal_DisableRequested,
   Signal_ResetRequested,
   Signal_FzAbnormalRunTimeReached,
   Signal_MaxTimeBetweenDefrostsReached
};

static void State_Disabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Enabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static DefrostTimerIsSatisfiedMonitor_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(DefrostTimerIsSatisfiedMonitor_t, _private.fsm, fsm);
}

static void SetFsmStateTo(DefrostTimerIsSatisfiedMonitor_t *instance, DefrostTimerIsSatisfiedMonitorFsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerIsSatisfiedMonitorFsmStateErd,
      &state);
}

static void DefrostTimerIsSatisfiedMonitorRequested(DefrostTimerIsSatisfiedMonitor_t *instance, const DefrostTimerIsSatisfiedMonitorRequest_t *request)
{
   switch(request->request)
   {
      case DefrostTimerIsSatisfied_Enable:
         Fsm_SendSignal(&instance->_private.fsm, Signal_EnableRequested, NULL);
         break;

      case DefrostTimerIsSatisfied_Disable:
         Fsm_SendSignal(&instance->_private.fsm, Signal_DisableRequested, NULL);
         break;

      case DefrostTimerIsSatisfied_Reset:
         Fsm_SendSignal(&instance->_private.fsm, Signal_ResetRequested, NULL);
         break;

      default:
         break;
   }
}

static bool SabbathModeIsEnabled(DefrostTimerIsSatisfiedMonitor_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathModeErd,
      &state);

   return state;
}

static void SendSignalOnce(DefrostTimerIsSatisfiedMonitor_t *instance, const FsmSignal_t signal)
{
   if(instance->_private.lastPublishedSignal != signal)
   {
      instance->_private.lastPublishedSignal = signal;
      Fsm_SendSignal(&instance->_private.fsm, instance->_private.lastPublishedSignal, NULL);
   }
}

static uint32_t MaxTimeBetweenDefrostsInSeconds(DefrostTimerIsSatisfiedMonitor_t *instance)
{
   uint16_t maxTimeBetweenDefrostsInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->maxTimeBetweenDefrostsInMinutesErd,
      &maxTimeBetweenDefrostsInMinutes);

   return (uint32_t)(maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE);
}

static void DataModelChanged(void *context, const void *args)
{
   REINTERPRET(instance, context, DefrostTimerIsSatisfiedMonitor_t *);
   REINTERPRET(onChangeData, args, const DataModelOnDataChangeArgs_t *);
   REINTERPRET(erd, onChangeData->erd, Erd_t);

   if(erd == instance->_private.config->defrostTimerIsSatisfiedMonitorRequestErd)
   {
      REINTERPRET(request, onChangeData->data, const DefrostTimerIsSatisfiedMonitorRequest_t *);

      DefrostTimerIsSatisfiedMonitorRequested(instance, request);
   }
   else if(erd == instance->_private.config->ramDefrostTimerCountInSecondsErd)
   {
      REINTERPRET(defrostTimerCountsInSeconds, onChangeData->data, const uint32_t *);

      uint32_t maxTimeBetweenDefrostsInSeconds = MaxTimeBetweenDefrostsInSeconds(instance);

      if((*defrostTimerCountsInSeconds >= instance->_private.defrostParametricData->fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE) &&
         (*defrostTimerCountsInSeconds < maxTimeBetweenDefrostsInSeconds))
      {
         SendSignalOnce(instance, Signal_FzAbnormalRunTimeReached);
      }
      else if(*defrostTimerCountsInSeconds >= maxTimeBetweenDefrostsInSeconds)
      {
         SendSignalOnce(instance, Signal_MaxTimeBetweenDefrostsReached);
      }
   }
}

static void ResetLastPublishedSignal(DefrostTimerIsSatisfiedMonitor_t *instance)
{
   instance->_private.lastPublishedSignal = 0;
}

static void SetDefrostTimerIsSatisfied(DefrostTimerIsSatisfiedMonitor_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerIsSatisfiedErd,
      set);
}

static void ClearDefrostTimerIsSatisfied(DefrostTimerIsSatisfiedMonitor_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerIsSatisfiedErd,
      clear);
}

static void DetermineIfDefrostTimerIsSatisfiedWhenFzAbnormalRunTimeIsReached(DefrostTimerIsSatisfiedMonitor_t *instance)
{
   EnergyDemandLevel_t level;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->demandResponseLevelErd,
      &level);

   bool lastFzDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fzDefrostWasAbnormalErd,
      &lastFzDefrostWasAbnormal);

   bool lastFfDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ffDefrostWasAbnormalErd,
      &lastFfDefrostWasAbnormal);

   if((level == EnergyDemandLevel_Off) &&
      (lastFzDefrostWasAbnormal || lastFfDefrostWasAbnormal) &&
      !SabbathModeIsEnabled(instance))
   {
      SetDefrostTimerIsSatisfied(instance);
   }
}

static void DetermineIfDefrostTimerIsSatisfiedWhenMaxTimeBetweenDefrosts(DefrostTimerIsSatisfiedMonitor_t *instance)
{
   EnergyDemandLevel_t level;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->demandResponseLevelErd,
      &level);

   if(level == EnergyDemandLevel_Off)
   {
      SetDefrostTimerIsSatisfied(instance);
   }
}

static void State_Enabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DefrostTimerIsSatisfiedMonitor_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         ResetLastPublishedSignal(instance);
         SetFsmStateTo(instance, DefrostTimerIsSatisfiedMonitorFsmState_Enabled);
         break;

      case Signal_DisableRequested:
         Fsm_Transition(&instance->_private.fsm, State_Disabled);
         break;

      case Signal_ResetRequested:
         ResetLastPublishedSignal(instance);
         ClearDefrostTimerIsSatisfied(instance);
         break;

      case Signal_FzAbnormalRunTimeReached:
         DetermineIfDefrostTimerIsSatisfiedWhenFzAbnormalRunTimeIsReached(instance);
         break;

      case Signal_MaxTimeBetweenDefrostsReached:
         DetermineIfDefrostTimerIsSatisfiedWhenMaxTimeBetweenDefrosts(instance);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Disabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DefrostTimerIsSatisfiedMonitor_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DefrostTimerIsSatisfiedMonitorFsmState_Disabled);
         ClearDefrostTimerIsSatisfied(instance);
         break;

      case Signal_EnableRequested:
         Fsm_Transition(&instance->_private.fsm, State_Enabled);
         break;

      case Fsm_Exit:
         break;
   }
}

void DefrostTimerIsSatisfiedMonitor_Init(
   DefrostTimerIsSatisfiedMonitor_t *instance,
   I_DataModel_t *dataModel,
   const DefrostTimerIsSatisfiedMonitorConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;
   instance->_private.sabbathParametricData = PersonalityParametricData_Get(dataModel)->sabbathData;

   EventSubscription_Init(
      &instance->_private.dataModelChangedSubscription,
      instance,
      DataModelChanged);
   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelChangedSubscription);

   Fsm_Init(&instance->_private.fsm, State_Disabled);
}
