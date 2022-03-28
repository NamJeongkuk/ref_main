/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataModelErdPointerAccess.h"
#include "DefrostTimerCounter.h"
#include "DefrostTimerCounterRequest.h"
#include "DefrostTimerCounterFsmState.h"
#include "DefrostTimerIsSatisfiedMonitorRequest.h"
#include "DoorAccelerationRequest.h"
#include "CompressorVotedSpeed.h"
#include "CompressorSpeedConfig.h"
#include "PersonalityParametricData.h"
#include "Constants_Time.h"
#include "utils.h"

enum
{
   Signal_EnableRequested = Fsm_UserSignalStart,
   Signal_DisableRequested,
   Signal_ResetRequested,
   Signal_PeriodicTimerComplete
};

static void State_Disabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Enabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static DefrostTimerCounter_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(DefrostTimerCounter_t, _private.fsm, fsm);
}

static void SetFsmStateTo(DefrostTimerCounter_t *instance, DefrostTimerCounterFsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerCounterFsmStateErd,
      &state);
}

static void DefrostTimerCounterRequested(DefrostTimerCounter_t *instance, const DefrostTimerCounterRequest_t *request)
{
   switch(request->request)
   {
      case DefrostTimer_Enable:
         Fsm_SendSignal(&instance->_private.fsm, Signal_EnableRequested, NULL);
         break;

      case DefrostTimer_Disable:
         Fsm_SendSignal(&instance->_private.fsm, Signal_DisableRequested, NULL);
         break;

      case DefrostTimer_Reset:
         Fsm_SendSignal(&instance->_private.fsm, Signal_ResetRequested, NULL);
         break;

      default:
         break;
   }
}

static void RequestReceived(void *context, const void *args)
{
   REINTERPRET(instance, context, DefrostTimerCounter_t *);
   REINTERPRET(request, args, const DefrostTimerCounterRequest_t *);

   DefrostTimerCounterRequested(instance, request);
}

static void ResetDefrostTimerCount(DefrostTimerCounter_t *instance)
{
   uint32_t count = 0;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerCountInSecondsErd,
      &count);
}

static void SendDoorAccelerationRequestTo(DefrostTimerCounter_t *instance, DoorAcceleration_t request)
{
   DoorAccelerationRequest_t doorAccelerationRequest;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->doorAccelerationRequestErd,
      &doorAccelerationRequest);

   doorAccelerationRequest.request = request;
   doorAccelerationRequest.requestId++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->doorAccelerationRequestErd,
      &doorAccelerationRequest);
}

static void SendDefrostTimerIsSatisfiedMonitorRequestTo(DefrostTimerCounter_t *instance, DefrostTimerIsSatisfiedMonitor_t request)
{
   DefrostTimerIsSatisfiedMonitorRequest_t defrostTimerIsSatisfiedMonitorRequest;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerIsSatisfiedMonitorRequestErd,
      &defrostTimerIsSatisfiedMonitorRequest);

   defrostTimerIsSatisfiedMonitorRequest.request = request;
   defrostTimerIsSatisfiedMonitorRequest.requestId++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerIsSatisfiedMonitorRequestErd,
      &defrostTimerIsSatisfiedMonitorRequest);
}

static bool SabbathModeIsEnabled(DefrostTimerCounter_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathModeErd,
      &state);

   return state;
}

static bool CompressorIsOn(DefrostTimerCounter_t *instance)
{
   CompressorVotedSpeed_t votedSpeed;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorResolvedSpeedErd,
      &votedSpeed);

   CompressorSpeedConfig_t speedConfig;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedConfigErd,
      &speedConfig);

   if(speedConfig == CompressorSpeedConfig_SingleSpeed)
   {
      return votedSpeed.speed == CompressorSpeed_On;
   }
   else
   {
      return (votedSpeed.speed > CompressorSpeed_Off &&
         votedSpeed.speed < CompressorSpeed_MaxNumberOfSpeedsInVariableType);
   }
}

static void PeriodicTimerComplete(void *context)
{
   REINTERPRET(instance, context, DefrostTimerCounter_t *);

   Fsm_SendSignal(&instance->_private.fsm, Signal_PeriodicTimerComplete, NULL);
}

static void StartPeriodicTimer(DefrostTimerCounter_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timer,
      instance->_private.defrostParametricData->defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC,
      PeriodicTimerComplete,
      instance);
}

static void IncrementDefrostTimerCount(DefrostTimerCounter_t *instance)
{
   uint32_t count;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerCountInSecondsErd,
      &count);

   count = TRUNCATE_UNSIGNED_ADDITION(count, 1, UINT32_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerCountInSecondsErd,
      &count);
}

static void AddAllDoorAccelerationsToCount(DefrostTimerCounter_t *instance, uint32_t count)
{
   uint32_t ffDoorAcceleration;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ffDoorAccelerationCountsErd,
      &ffDoorAcceleration);

   uint32_t fzDoorAcceleration;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fzDoorAccelerationCountsErd,
      &fzDoorAcceleration);

   uint32_t adjustedCount = count + ffDoorAcceleration + fzDoorAcceleration;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerCountInSecondsErd,
      &adjustedCount);
}

static void ResetOrAddDoorAccelerationsToDefrostTimerCount(DefrostTimerCounter_t *instance)
{
   bool fzDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fzDefrostWasAbnormalErd,
      &fzDefrostWasAbnormal);

   uint32_t count;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerCountInSecondsErd,
      &count);

   uint16_t maxTimeBetweenDefrostsInMinutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->maxTimeBetweenDefrostsInMinutesErd,
      &maxTimeBetweenDefrostsInMinutes);

   if(!fzDefrostWasAbnormal)
   {
      if(count >= instance->_private.defrostParametricData->fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
      {
         if(count < maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE)
         {
            AddAllDoorAccelerationsToCount(instance, count);
         }
         SendDoorAccelerationRequestTo(instance, DoorAcceleration_Reset);
      }
   }
}

static void State_Enabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DefrostTimerCounter_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DefrostTimerCounterFsmState_Enabled);
         SendDoorAccelerationRequestTo(instance, DoorAcceleration_Enable);
         SendDefrostTimerIsSatisfiedMonitorRequestTo(instance, DefrostTimerIsSatisfiedMonitor_Enable);
         StartPeriodicTimer(instance);
         break;

      case Signal_DisableRequested:
         Fsm_Transition(&instance->_private.fsm, State_Disabled);
         break;

      case Signal_ResetRequested:
         ResetDefrostTimerCount(instance);
         SendDoorAccelerationRequestTo(instance, DoorAcceleration_Reset);
         SendDefrostTimerIsSatisfiedMonitorRequestTo(instance, DefrostTimerIsSatisfiedMonitor_Reset);
         break;

      case Signal_PeriodicTimerComplete:
         if(SabbathModeIsEnabled(instance) || CompressorIsOn(instance))
         {
            IncrementDefrostTimerCount(instance);
         }
         ResetOrAddDoorAccelerationsToDefrostTimerCount(instance);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Disabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DefrostTimerCounter_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DefrostTimerCounterFsmState_Disabled);
         ResetDefrostTimerCount(instance);
         SendDoorAccelerationRequestTo(instance, DoorAcceleration_Disable);
         SendDefrostTimerIsSatisfiedMonitorRequestTo(instance, DefrostTimerIsSatisfiedMonitor_Disable);
         break;

      case Signal_EnableRequested:
         Fsm_Transition(&instance->_private.fsm, State_Enabled);
         break;

      case Fsm_Exit:
         break;
   }
}

void DefrostTimerCounter_Init(
   DefrostTimerCounter_t *instance,
   I_DataModel_t *dataModel,
   const DefrostTimerCounterConfig_t *defrostTimerConfig)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = defrostTimerConfig;
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;
   instance->_private.sabbathParametricData = PersonalityParametricData_Get(dataModel)->sabbathData;

   EventSubscription_Init(
      &instance->_private.requestSubscription,
      instance,
      RequestReceived);

   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerCounterRequestErd,
      &instance->_private.requestSubscription);

   Fsm_Init(&instance->_private.fsm, State_Disabled);
}
