/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "LightingDoorVoteResolver.h"
#include "utils.h"
#include "PwmVotedDutyCycle.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Time.h"
#include "RampingPwmDutyCycle.h"
#include "RampingPwmDutyCyclePercentageVote.h"
#include "PersonalityParametricData.h"
#include "RampingPwmDutyCyclePercentageBundleData.h"

enum
{
   Signal_AnAssociatedDoorIsOpened = Fsm_UserSignalStart,
   Signal_AllAssociatedDoorsAreClosed,
   Signal_MaxDoorOpenTimeExpires,
   Signal_TimeoutExpired
};

static void State_AnAssociatedDoorIsOpen(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_AllAssociatedDoorsAreClosed(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static LightingDoorVoteResolver_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(LightingDoorVoteResolver_t, _private.fsm, fsm);
}

static void SetRampingPwmVotedDutyCycleErdTo(
   LightingDoorVoteResolver_t *instance,
   RampingPwmDutyCyclePercentage_t rampingPercentage)
{
   RampingPwmDutyCyclePercentageVote_t vote = {
      .rampingPwmDutyCyclePercentage = rampingPercentage,
      .care = Vote_Care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rampingPwmDutyCyclePercentageErd,
      &vote);
}

static void SetRampingPwmDutyCyclePercentageToMinPwmDutyCyclePercentageThenToMaxPwmDutyCyclePercentage(
   LightingDoorVoteResolver_t *instance,
   RampingPwmDutyCyclePercentageBundleData_t rampingPwmDutyCyclePercentageBundleData)
{
   RampingPwmDutyCyclePercentage_t minPwmPercentage = {
      .pwmDutyCyclePercentage = rampingPwmDutyCyclePercentageBundleData.minPwmDutyCyclePercentage,
      .rampingUpCountInMsec = UINT8_MAX,
      .rampingDownCountInMsec = UINT8_MAX,
   };
   SetRampingPwmVotedDutyCycleErdTo(instance, minPwmPercentage);

   RampingPwmDutyCyclePercentage_t maxPwmPercentage = {
      .pwmDutyCyclePercentage = rampingPwmDutyCyclePercentageBundleData.maxPwmDutyCyclePercentage,
      .rampingUpCountInMsec = rampingPwmDutyCyclePercentageBundleData.maxPwmRampingUpCountInMsec,
      .rampingDownCountInMsec = rampingPwmDutyCyclePercentageBundleData.maxPwmRampingDownCountInMsec,
   };
   SetRampingPwmVotedDutyCycleErdTo(instance, maxPwmPercentage);
}

static void SetRampingPwmDutyCyclePercentageToMinCycle(
   LightingDoorVoteResolver_t *instance,
   RampingPwmDutyCyclePercentageBundleData_t rampingPwmDutyCyclePercentageBundleData)
{
   RampingPwmDutyCyclePercentage_t minCycle = {
      .pwmDutyCyclePercentage = PercentageDutyCycle_Min,
      .rampingUpCountInMsec = rampingPwmDutyCyclePercentageBundleData.maxPwmRampingUpCountInMsec,
      .rampingDownCountInMsec = rampingPwmDutyCyclePercentageBundleData.maxPwmRampingDownCountInMsec,
   };
   SetRampingPwmVotedDutyCycleErdTo(instance, minCycle);
}

static RampingPwmDutyCyclePercentageBundleData_t RampingDutyCycleBundleBasedOffCurrentSystemMode(LightingDoorVoteResolver_t *instance)
{
   return instance->_private.doorLightingData->normalOperationRampingPwmDutyCycle;
}

static void MaxDoorOpenTimerExpired(void *context)
{
   LightingDoorVoteResolver_t *instance = context;
   Fsm_SendSignal(&instance->_private.fsm, Signal_MaxDoorOpenTimeExpires, NULL);
}

static void StartMaxDoorOpenTimer(LightingDoorVoteResolver_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.maxDoorOpenTimer,
      PersonalityParametricData_Get(instance->_private.dataModel)->lightingData->maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN,
      MaxDoorOpenTimerExpired,
      instance);
}

static void StopMaxDoorOpenTimer(LightingDoorVoteResolver_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.maxDoorOpenTimer);
}

void State_AnAssociatedDoorIsOpen(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   LightingDoorVoteResolver_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetRampingPwmDutyCyclePercentageToMinPwmDutyCyclePercentageThenToMaxPwmDutyCyclePercentage(
            instance,
            RampingDutyCycleBundleBasedOffCurrentSystemMode(instance));
         StartMaxDoorOpenTimer(instance);
         break;

      case Signal_AnAssociatedDoorIsOpened:
         StartMaxDoorOpenTimer(instance);
         break;

      case Signal_AllAssociatedDoorsAreClosed:
         Fsm_Transition(&instance->_private.fsm, State_AllAssociatedDoorsAreClosed);
         break;

      case Signal_MaxDoorOpenTimeExpires:
         SetRampingPwmDutyCyclePercentageToMinCycle(
            instance,
            RampingDutyCycleBundleBasedOffCurrentSystemMode(instance));
         break;

      case Fsm_Exit:
         StopMaxDoorOpenTimer(instance);
         break;
   }
}

void State_AllAssociatedDoorsAreClosed(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   LightingDoorVoteResolver_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetRampingPwmDutyCyclePercentageToMinCycle(
            instance,
            RampingDutyCycleBundleBasedOffCurrentSystemMode(instance));
         break;

      case Signal_AnAssociatedDoorIsOpened:
         Fsm_Transition(&instance->_private.fsm, State_AnAssociatedDoorIsOpen);
         break;

      case Fsm_Exit:
         break;
   }
}

static bool AnAssociatedDoorChanged(LightingDoorVoteResolver_t *instance, Erd_t erdToCheck)
{
   for(uint8_t i = 0; i < instance->_private.config->numberOfDoorErds; i++)
   {
      if(instance->_private.config->doorIsOpenErds[i] == erdToCheck)
      {
         return true;
      }
   }
   return false;
}

static bool AnAssociatedDoorIsOpen(LightingDoorVoteResolver_t *instance)
{
   for(uint8_t i = 0; i < instance->_private.config->numberOfDoorErds; i++)
   {
      bool doorIsOpen;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->doorIsOpenErds[i],
         &doorIsOpen);

      if(doorIsOpen)
      {
         return true;
      }
   }
   return false;
}

static bool AllAssociatedDoorsAreClosed(LightingDoorVoteResolver_t *instance)
{
   return !(AnAssociatedDoorIsOpen(instance));
}

static bool TheDoorOpened(LightingDoorVoteResolver_t *instance, Erd_t erd)
{
   bool doorOpened;
   DataModel_Read(
      instance->_private.dataModel,
      erd,
      &doorOpened);

   return doorOpened;
}

static void OnDataModelChange(void *context, const void *_args)
{
   LightingDoorVoteResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;
   Erd_t erd = args->erd;

   if(AnAssociatedDoorChanged(instance, args->erd))
   {
      if(AllAssociatedDoorsAreClosed(instance))
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_AllAssociatedDoorsAreClosed, NULL);
      }
      else if(TheDoorOpened(instance, erd))
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_AnAssociatedDoorIsOpened, NULL);
      }
   }
}

static FsmState_t InitialState(LightingDoorVoteResolver_t *instance)
{
   if(AnAssociatedDoorIsOpen(instance))
   {
      return State_AnAssociatedDoorIsOpen;
   }

   return State_AllAssociatedDoorsAreClosed;
}

void LightingDoorVoteResolver_Init(
   LightingDoorVoteResolver_t *instance,
   I_DataModel_t *dataModel,
   const LightingDoorVoteResolverConfig_t *config,
   const DoorLightingData_t *doorLightingData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.doorLightingData = doorLightingData;

   Fsm_Init(
      &instance->_private.fsm,
      InitialState(instance));

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
