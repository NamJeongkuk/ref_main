/*!
 * @file
 * @brief vote to setpoint and fans to set/maintain optimal ice rate conditions
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceRateHandler.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Time.h"
#include "Vote.h"

enum
{
   IceMakerDatafreezerIceRateSetpointInDegFx100 = 250,
   IceMakerDatafreezerIceRateTimeInMinutes = 2,
};

static void TimerExpired(void *context)
{
   IceRateHandler_t *instance = context;

   DataModel_UnsubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelOnChangeSubscription);

   SetpointVotedTemperature_t setpointVoteDontCare;
   FanVotedSpeed_t fanSpeedVoteDontCare;

   DataModel_Read(instance->_private.dataModel, instance->_private.config->freezerSetpointIceRateVote, &setpointVoteDontCare);
   DataModel_Read(instance->_private.dataModel, instance->_private.config->freezerEvapFanSpeedIceRateVote, &fanSpeedVoteDontCare);

   setpointVoteDontCare.care = Vote_DontCare;
   fanSpeedVoteDontCare.care = Vote_DontCare;

   DataModel_Write(instance->_private.dataModel, instance->_private.config->freezerSetpointIceRateVote, &setpointVoteDontCare);
   DataModel_Write(instance->_private.dataModel, instance->_private.config->freezerEvapFanSpeedIceRateVote, &fanSpeedVoteDontCare);
}

static void FreezerEvapFanSpeedChange(IceRateHandler_t *instance)
{
   FanVotedSpeed_t freezerEvapFanVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanSpeedResolvedVote,
      &freezerEvapFanVote);

   if(freezerEvapFanVote.speed < FanSpeed_Medium)
   {
      FanVotedSpeed_t adjustedFreezerEvapFanVote = { .speed = FanSpeed_Medium, .care = true };

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerEvapFanSpeedIceRateVote,
         &adjustedFreezerEvapFanVote);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerEvapFanSpeedIceRateVote,
         &freezerEvapFanVote);
   }
}

static void FreezerSetpointUserVoteChange(IceRateHandler_t *instance)
{
   SetpointVotedTemperature_t freezerSetpointVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointUserVote,
      &freezerSetpointVote);

   if(freezerSetpointVote.temperature > IceMakerDatafreezerIceRateSetpointInDegFx100)
   {
      SetpointVotedTemperature_t adjustedFreezerSetpointVote = { .temperature = IceMakerDatafreezerIceRateSetpointInDegFx100, .care = true };

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerSetpointIceRateVote,
         &adjustedFreezerSetpointVote);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerSetpointIceRateVote,
         &freezerSetpointVote);
   }
}

static void StartTimer(IceRateHandler_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.iceRateTimer,
      IceMakerDatafreezerIceRateTimeInMinutes * MSEC_PER_MIN,
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
   IceRateHandler_t *instance = context;
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
   IceRateHandler_t *instance = context;
   IGNORE(_args);

   StartTimer(instance);
}

void IceRateHandler_Init(
   IceRateHandler_t *instance,
   I_DataModel_t *dataModel,
   const IceRateHandlerConfig_t *config)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;

   EventSubscription_Init(
      &instance->_private.iceRateTriggerSignalSubscription,
      instance,
      IceRateSignalTriggered);

   EventSubscription_Init(
      &instance->_private.dataModelOnChangeSubscription,
      instance,
      OnDataModelChange);

   if(IceMakerDatafreezerIceRateTimeInMinutes != 0)
   {
      DataModel_Subscribe(
         dataModel,
         instance->_private.config->iceRateTriggerSignal,
         &instance->_private.iceRateTriggerSignalSubscription);
   }
}
