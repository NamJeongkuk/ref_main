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
#include "Constants_Binary.h"

static void TimerExpired(void *context)
{
   FreezerIceRateHandler_t *instance = context;

   DataModel_UnsubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelOnChangeSubscription);

   SetpointVotedTemperature_t setpointVoteDontCare;
   FanVotedSpeed_t fanSpeedVoteDontCare;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointFreezerIceRateVoteErd,
      &setpointVoteDontCare);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanSpeedFreezerIceRateVoteErd,
      &fanSpeedVoteDontCare);

   setpointVoteDontCare.care = Vote_DontCare;
   fanSpeedVoteDontCare.care = Vote_DontCare;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointFreezerIceRateVoteErd,
      &setpointVoteDontCare);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanSpeedFreezerIceRateVoteErd,
      &fanSpeedVoteDontCare);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerIceRateIsActiveErd,
      clear);
}

static void UpdateFreezerIceRateVoteBasedOnFreezerEvapFanSpeed(FreezerIceRateHandler_t *instance)
{
   FanVotedSpeed_t freezerEvapFanVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanSpeedResolvedVoteErd,
      &freezerEvapFanVote);

   if(freezerEvapFanVote.speed < instance->_private.freezerIceRateData->freezerEvaporatorFanSpeed)
   {
      FanVotedSpeed_t adjustedFreezerEvapFanVote = {
         .speed = instance->_private.freezerIceRateData->freezerEvaporatorFanSpeed,
         .care = true
      };

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerEvapFanSpeedFreezerIceRateVoteErd,
         &adjustedFreezerEvapFanVote);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerEvapFanSpeedFreezerIceRateVoteErd,
         &freezerEvapFanVote);
   }
}

static void UpdateFreezerSetpointIceRateVote(FreezerIceRateHandler_t *instance)
{
   SetpointVotedTemperature_t adjustedFreezerSetpointVote = {
      .temperatureInDegFx100 = instance->_private.freezerIceRateData->freezerSetpointInDegFx100,
      .care = true
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointFreezerIceRateVoteErd,
      &adjustedFreezerSetpointVote);
}

static void SubscribeToDataModel(FreezerIceRateHandler_t *instance)
{
   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelOnChangeSubscription);
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
}

static void OnDataModelChange(void *context, const void *_args)
{
   FreezerIceRateHandler_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->freezerEvapFanSpeedResolvedVoteErd)
   {
      UpdateFreezerIceRateVoteBasedOnFreezerEvapFanSpeed(instance);
   }
}

static void IceRateSignalTriggered(void *context, const void *_args)
{
   FreezerIceRateHandler_t *instance = context;
   IGNORE(_args);

   StartTimer(instance);
   SubscribeToDataModel(instance);
   UpdateFreezerIceRateVoteBasedOnFreezerEvapFanSpeed(instance);
   UpdateFreezerSetpointIceRateVote(instance);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerIceRateIsActiveErd,
      set);
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

   if(instance->_private.freezerIceRateData->timeInMinutes > 0)
   {
      DataModel_Subscribe(
         dataModel,
         instance->_private.config->freezerIceRateTriggerSignalErd,
         &instance->_private.freezerIceRateTriggerSignalSubscription);
   }
}
