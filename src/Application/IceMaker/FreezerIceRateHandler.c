/*!
 * @file
 * @brief vote to setpoint and fans to set/maintain optimal ice rate conditions
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreezerIceRateHandler.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Time.h"
#include "Vote.h"
#include "SystemErds.h"

static void TimerExpired(void *context)
{
   FreezerIceRateHandler_t *instance = context;

   DataModel_UnsubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelOnChangeSubscription);

   SetpointVotedTemperature_t setpointVoteDontCare;
   FanVotedSpeed_t fanSpeedVoteDontCare;

   DataModel_Read(instance->_private.dataModel, instance->_private.config->freezerSetpointFreezerIceRateVote, &setpointVoteDontCare);
   DataModel_Read(instance->_private.dataModel, instance->_private.config->freezerEvapFanSpeedFreezerIceRateVote, &fanSpeedVoteDontCare);

   setpointVoteDontCare.care = Vote_DontCare;
   fanSpeedVoteDontCare.care = Vote_DontCare;

   DataModel_Write(instance->_private.dataModel, instance->_private.config->freezerSetpointFreezerIceRateVote, &setpointVoteDontCare);
   DataModel_Write(instance->_private.dataModel, instance->_private.config->freezerEvapFanSpeedFreezerIceRateVote, &fanSpeedVoteDontCare);
}

static void FreezerEvapFanSpeedChange(FreezerIceRateHandler_t *instance)
{
   FanVotedSpeed_t freezerEvapFanVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanSpeedResolvedVote,
      &freezerEvapFanVote);

   if(freezerEvapFanVote.speed < FanSpeed_Medium)
   {
      FanVotedSpeed_t adjustedFreezerEvapFanVote = {
         .speed = FanSpeed_Medium,
         .care = true
      };

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerEvapFanSpeedFreezerIceRateVote,
         &adjustedFreezerEvapFanVote);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerEvapFanSpeedFreezerIceRateVote,
         &freezerEvapFanVote);
   }
}

static void FreezerSetpointUserVoteChange(FreezerIceRateHandler_t *instance)
{
   SetpointVotedTemperature_t freezerSetpointVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointUserVote,
      &freezerSetpointVote);

   if(freezerSetpointVote.temperatureInDegFx100 > instance->_private.freezerIceRateData->freezerSetpointInDegFx100)
   {
      SetpointVotedTemperature_t adjustedFreezerSetpointVote = {
         .temperatureInDegFx100 = instance->_private.freezerIceRateData->freezerSetpointInDegFx100,
         .care = true
      };

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerSetpointFreezerIceRateVote,
         &adjustedFreezerSetpointVote);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerSetpointFreezerIceRateVote,
         &freezerSetpointVote);
   }
}

static void StartTimer(FreezerIceRateHandler_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.iceRateTimer,
      instance->_private.freezerIceRateData->timeInMinutes * MSEC_PER_MIN,
      TimerExpired,
      instance);

   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelOnChangeSubscription);

   FreezerEvapFanSpeedChange(instance);
   FreezerSetpointUserVoteChange(instance);
}

static void OnDataModelChange(void *context, const void *_args)
{
   FreezerIceRateHandler_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->freezerEvapFanSpeedResolvedVote)
   {
      FreezerEvapFanSpeedChange(instance);
   }
   else if(args->erd == instance->_private.config->freezerSetpointUserVote)
   {
      FreezerSetpointUserVoteChange(instance);
   }
}

static void IceRateSignalTriggered(void *context, const void *_args)
{
   FreezerIceRateHandler_t *instance = context;
   IGNORE(_args);

   StartTimer(instance);
}

void FreezerIceRateHandler_Init(
   FreezerIceRateHandler_t *instance,
   I_DataModel_t *dataModel,
   const FreezerIceRateHandlerConfig_t *config,
   const FreezerIceRateData_t *freezerIceRateData)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.freezerIceRateData = freezerIceRateData;

   EventSubscription_Init(
      &instance->_private.freezerIceRateTriggerSignalSubscription,
      instance,
      IceRateSignalTriggered);

   EventSubscription_Init(
      &instance->_private.dataModelOnChangeSubscription,
      instance,
      OnDataModelChange);

   if(instance->_private.freezerIceRateData->timeInMinutes != 0)
   {
      DataModel_Subscribe(
         dataModel,
         instance->_private.config->freezerIceRateTriggerSignal,
         &instance->_private.freezerIceRateTriggerSignalSubscription);
   }
}
