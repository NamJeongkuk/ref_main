/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorSealedSystemValveDelay.h"
#include "CompressorVotedSpeed.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "FanSpeed.h"

typedef CompressorSealedSystemValveDelay_t Instance_t;

static void VoteForCompressor(Instance_t *instance, CompressorSpeed_t speed, Vote_t care)
{
   CompressorVotedSpeed_t vote;
   vote.speed = speed;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorDelayVoteErd,
      &vote);
}

static void VoteForFans(Instance_t *instance, FanSpeed_t speed, Vote_t care)
{
   FanVotedSpeed_t vote;
   vote.speed = speed;
   vote.care = care;

   ErdList_WriteAll(
      instance->_private.dataModel,
      &instance->_private.config->fanDelayVotesErdList,
      &vote);
}

static bool AmbientTemperatureIsValidAndLessThanTriggerTemperature(Instance_t *instance)
{
   bool isValid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientTemperatureIsValidErd,
      &isValid);

   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientTemperatureErd,
      &temperature);

   return isValid && (temperature <= instance->_private.data->lowAmbientTriggerTemperatureInDegFx100);
}

static bool CompressorIsOffAndUnderGridControl(Instance_t *instance)
{
   Erd_t winningVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->winningVoteErd,
      &winningVote);

   CompressorVotedSpeed_t gridVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->gridCompressorVoteErd,
      &gridVote);

   return ((winningVote == instance->_private.config->gridCompressorVoteErd) && (gridVote.speed == CompressorSpeed_Off));
}

static void VoteDontCare(void *context)
{
   Instance_t *instance = context;

   VoteForCompressor(instance, CompressorSpeed_Off, Vote_DontCare);
   VoteForFans(instance, FanSpeed_Off, Vote_DontCare);
}

static void SealedSystemValveChanged(void *context, const void *args)
{
   Instance_t *instance = context;
   (void)args;

   if(CompressorIsOffAndUnderGridControl(instance))
   {
      VoteForCompressor(instance, CompressorSpeed_Off, Vote_Care);
      VoteForFans(instance, FanSpeed_Off, Vote_Care);

      TimerTicks_t ticks;
      if(AmbientTemperatureIsValidAndLessThanTriggerTemperature(instance))
      {
         ticks = instance->_private.data->compressorTimes.remainOffAfterValveMoveLowAmbientTimeInMinutes * MSEC_PER_MIN;
      }
      else
      {
         ticks = instance->_private.data->compressorTimes.remainOffAfterValveMoveTimeInMinutes * MSEC_PER_MIN;
      }

      TimerModule_StartOneShot(
         DataModelErdPointerAccess_GetTimerModule(
            instance->_private.dataModel,
            Erd_TimerModule),
         &instance->_private.timer,
         ticks,
         VoteDontCare,
         instance);
   }
}

static void CompressorChanged(void *context, const void *args)
{
   Instance_t *instance = context;
   const CompressorVotedSpeed_t *compressorVote = args;

   if(compressorVote->speed != CompressorSpeed_Off)
   {
      DataModel_Unsubscribe(
         instance->_private.dataModel,
         instance->_private.config->gridCompressorVoteErd,
         &instance->_private.dataModelSubscription);
      EventSubscription_Init(
         &instance->_private.dataModelSubscription,
         instance,
         SealedSystemValveChanged);
      DataModel_Subscribe(
         instance->_private.dataModel,
         instance->_private.config->sealedSystemValveVotedPositionErd,
         &instance->_private.dataModelSubscription);
   }
}

void CompressorSealedSystemValveDelay_Init(
   Instance_t *instance,
   I_DataModel_t *dataModel,
   const CompressorData_t *data,
   const CompressorSealedSystemValveDelayConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.data = data;
   instance->_private.config = config;

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      CompressorChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->gridCompressorVoteErd,
      &instance->_private.dataModelSubscription);
}
