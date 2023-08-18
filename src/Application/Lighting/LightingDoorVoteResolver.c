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

static bool DoorErdChanged(LightingDoorVoteResolver_t *instance, Erd_t erdToCheck)
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

static bool AllDoorsAreClosed(LightingDoorVoteResolver_t *instance)
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
         return false;
      }
   }

   return true;
}

static void SetRampingPwmVotedDutyCycleErdsTo(LightingDoorVoteResolver_t *instance, RampingPwmDutyCyclePercentage_t rampingPercentage)
{
   RampingPwmDutyCyclePercentageVote_t vote = {
      .rampingPwmDutyCyclePercentage = rampingPercentage,
      .care = Vote_Care
   };

   for(uint8_t i = 0; i < instance->_private.config->numberOfRampingPwmDutyCyclePercentageErds; i++)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->rampingPwmDutyCyclePercentageErds[i],
         &vote);
   }
}

static void SetRampingPwmDutyCyclePercentageToMinPwmDutyCyclePercentageThenToMaxPwmDutyCyclePercentage(LightingDoorVoteResolver_t *instance, RampingPwmDutyCyclePercentageBundleData_t rampingPwmDutyCyclePercentageBundleData)
{
   RampingPwmDutyCyclePercentage_t minPwmPercentage = {
      .pwmDutyCyclePercentage = rampingPwmDutyCyclePercentageBundleData.minPwmDutyCyclePercentage,
      .rampingUpCountInMsec = UINT8_MAX,
      .rampingDownCountInMsec = UINT8_MAX,
   };
   SetRampingPwmVotedDutyCycleErdsTo(instance, minPwmPercentage);

   RampingPwmDutyCyclePercentage_t maxPwmPercentage = {
      .pwmDutyCyclePercentage = rampingPwmDutyCyclePercentageBundleData.maxPwmDutyCyclePercentage,
      .rampingUpCountInMsec = rampingPwmDutyCyclePercentageBundleData.maxPwmRampingUpCountInMsec,
      .rampingDownCountInMsec = rampingPwmDutyCyclePercentageBundleData.maxPwmRampingDownCountInMsec,
   };

   SetRampingPwmVotedDutyCycleErdsTo(instance, maxPwmPercentage);
}

static void SetRampingPwmDutyCyclePercentageToMinCycle(LightingDoorVoteResolver_t *instance, RampingPwmDutyCyclePercentageBundleData_t rampingPwmDutyCyclePercentageBundleData)
{
   RampingPwmDutyCyclePercentage_t minCycle = {
      .pwmDutyCyclePercentage = PercentageDutyCycle_Min,
      .rampingUpCountInMsec = rampingPwmDutyCyclePercentageBundleData.maxPwmRampingUpCountInMsec,
      .rampingDownCountInMsec = rampingPwmDutyCyclePercentageBundleData.maxPwmRampingDownCountInMsec,
   };

   SetRampingPwmVotedDutyCycleErdsTo(instance, minCycle);
}

static RampingPwmDutyCyclePercentageBundleData_t RampingDutyCycleBundleBasedOffCurrentSystemMode(LightingDoorVoteResolver_t *instance)
{
   return *(instance->_private.doorLightingDataType->normalOperationRampingPwmDutyCycle);
}

static void MaxDoorOpenTimerExpired(void *context)
{
   LightingDoorVoteResolver_t *instance = context;
   RampingPwmDutyCyclePercentageBundleData_t rampingPwmDutyCyclePercentageBundleData = RampingDutyCycleBundleBasedOffCurrentSystemMode(instance);

   SetRampingPwmDutyCyclePercentageToMinCycle(instance, rampingPwmDutyCyclePercentageBundleData);
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

static void OnDataModelChange(void *context, const void *_args)
{
   LightingDoorVoteResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(DoorErdChanged(instance, args->erd))
   {
      const bool *doorIsOpen = args->data;
      RampingPwmDutyCyclePercentageBundleData_t rampingPwmDutyCyclePercentageBundleData = RampingDutyCycleBundleBasedOffCurrentSystemMode(instance);

      if(*doorIsOpen)
      {
         SetRampingPwmDutyCyclePercentageToMinPwmDutyCyclePercentageThenToMaxPwmDutyCyclePercentage(instance, rampingPwmDutyCyclePercentageBundleData);
         StartMaxDoorOpenTimer(instance);
      }
      else if(AllDoorsAreClosed(instance))
      {
         SetRampingPwmDutyCyclePercentageToMinCycle(instance, rampingPwmDutyCyclePercentageBundleData);
         StopMaxDoorOpenTimer(instance);
      }
   }
}
static void SetInitialPwmVotedDutyCycleBasedOnDoorState(LightingDoorVoteResolver_t *instance)
{
   RampingPwmDutyCyclePercentageBundleData_t rampingPwmDutyCyclePercentageBundleData = RampingDutyCycleBundleBasedOffCurrentSystemMode(instance);

   if(AllDoorsAreClosed(instance))
   {
      SetRampingPwmDutyCyclePercentageToMinCycle(instance, rampingPwmDutyCyclePercentageBundleData);
   }
   else
   {
      SetRampingPwmDutyCyclePercentageToMinPwmDutyCyclePercentageThenToMaxPwmDutyCyclePercentage(instance, rampingPwmDutyCyclePercentageBundleData);
      StartMaxDoorOpenTimer(instance);
   }
}

void LightingDoorVoteResolver_Init(
   LightingDoorVoteResolver_t *instance,
   I_DataModel_t *dataModel,
   const LightingDoorVoteResolverConfig_t *config,
   const DoorLightingData_t *doorLightingDataType)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.doorLightingDataType = doorLightingDataType;

   SetInitialPwmVotedDutyCycleBasedOnDoorState(instance);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
