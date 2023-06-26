/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BooleanRequestStatusUpdater.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "SetpointTurboMode.h"
#include "SystemErds.h"

enum
{
   TurboModeUpdateTimeInMinutes = 1
};

static uint16_t TurboModeOnTimeInMinutes(SetpointTurboMode_t *instance)
{
   uint16_t minutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->turboModeOnTimeInMinutesErd,
      &minutes);

   return minutes;
}

static void TurboModeRequestedOff(SetpointTurboMode_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.turboModeTimer);

   SetpointVotedTemperature_t turboModeVote = {
      .temperatureInDegFx100 = 0,
      .care = Vote_DontCare
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->turboModeSetpointVoteErd,
      &turboModeVote);

   uint16_t minutes = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->turboModeOnTimeInMinutesErd,
      &minutes);
}

static void AddElapsedMinutesToTurboModeOnTime(void *context)
{
   SetpointTurboMode_t *instance = context;

   uint16_t updatedTimeInMinutes = TurboModeOnTimeInMinutes(instance);
   updatedTimeInMinutes = TRUNCATE_UNSIGNED_ADDITION(updatedTimeInMinutes, 1, UINT16_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->turboModeOnTimeInMinutesErd,
      &updatedTimeInMinutes);

   if(updatedTimeInMinutes >= instance->_private.turboModeData->turboModeOnTimeInMinutes)
   {
      TurboModeRequestedOff(instance);
   }
}

static void TurboModeRequestedOn(SetpointTurboMode_t *instance)
{
   SetpointVotedTemperature_t currentResolvedVoteTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->currentSetpointResolvedVoteErd,
      &currentResolvedVoteTemperature);

   SetpointVotedTemperature_t turboModeVote;
   turboModeVote.care = Vote_Care;

   if((currentResolvedVoteTemperature.temperatureInDegFx100 < instance->_private.turboModeData->turboModeSetpointInDegFx100) && (currentResolvedVoteTemperature.care == Vote_Care))
   {
      turboModeVote.temperatureInDegFx100 = currentResolvedVoteTemperature.temperatureInDegFx100;
   }
   else
   {
      turboModeVote.temperatureInDegFx100 = instance->_private.turboModeData->turboModeSetpointInDegFx100;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->turboModeSetpointVoteErd,
      &turboModeVote);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.turboModeTimer,
      TurboModeUpdateTimeInMinutes * MSEC_PER_MIN,
      AddElapsedMinutesToTurboModeOnTime,
      instance);
}

static void TurboModeStatusChanged(void *context, const void *_args)
{
   SetpointTurboMode_t *instance = context;
   const bool *turboOnOffStatus = _args;

   if(*turboOnOffStatus)
   {
      TurboModeRequestedOn(instance);
   }
   else
   {
      TurboModeRequestedOff(instance);
   }
}

void SetpointTurboMode_Init(
   SetpointTurboMode_t *instance,
   I_DataModel_t *dataModel,
   const SetpointTurboModeConfig_t *config,
   const TurboModeData_t *data)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.turboModeData = data;

   if(TurboModeOnTimeInMinutes(instance) > 0)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->turboModeOnOffStatusErd,
         set);
      TurboModeRequestedOn(instance);
   }

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      TurboModeStatusChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      config->turboModeOnOffStatusErd,
      &instance->_private.onDataModelChangeSubscription);
}
