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

enum
{
   MaxDoorOpenTimeInMsec = 15 * MSEC_PER_MIN
};

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

static void SetPwmVotedDutyCycleErdsTo(LightingDoorVoteResolver_t *instance, PwmDutyCycle_t pwmDutyCycle)
{
   PwmVotedDutyCycle_t pwmVotedDutyCycle = {
      .pwmDutyCycle = pwmDutyCycle,
      .care = Vote_Care
   };

   for(uint8_t i = 0; i < instance->_private.config->numberOfPwmVotedDutyCycleErds; i++)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->pwmVotedDutyCycleErds[i],
         &pwmVotedDutyCycle);
   }
}

static void MaxDoorOpenTimerExpired(void *context)
{
   LightingDoorVoteResolver_t *instance = context;
   SetPwmVotedDutyCycleErdsTo(instance, PwmDutyCycle_Min);
}

static void StartMaxDoorOpenTimer(LightingDoorVoteResolver_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.maxDoorOpenTimer,
      MaxDoorOpenTimeInMsec,
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

      if(*doorIsOpen)
      {
         SetPwmVotedDutyCycleErdsTo(instance, PwmDutyCycle_Max);
         StartMaxDoorOpenTimer(instance);
      }
      else if(AllDoorsAreClosed(instance))
      {
         SetPwmVotedDutyCycleErdsTo(instance, PwmDutyCycle_Min);
         StopMaxDoorOpenTimer(instance);
      }
   }
}

static void SetInitialPwmVotedDutyCycleBasedOnDoorState(LightingDoorVoteResolver_t *instance)
{
   if(AllDoorsAreClosed(instance))
   {
      SetPwmVotedDutyCycleErdsTo(instance, PwmDutyCycle_Min);
   }
   else
   {
      SetPwmVotedDutyCycleErdsTo(instance, PwmDutyCycle_Max);
      StartMaxDoorOpenTimer(instance);
   }
}

void LightingDoorVoteResolver_Init(
   LightingDoorVoteResolver_t *instance,
   I_DataModel_t *dataModel,
   const LightingDoorVoteResolverConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   SetInitialPwmVotedDutyCycleBasedOnDoorState(instance);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
