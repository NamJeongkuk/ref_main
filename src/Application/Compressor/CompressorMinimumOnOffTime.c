/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorMinimumOnOffTime.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Time.h"

static void VoteForCompressor(
   CompressorMinimumOnOffTime_t *instance,
   CompressorSpeed_t speed,
   Vote_t care)
{
   CompressorVotedSpeed_t vote = { .speed = speed, .care = care };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->minimumOnOffTimeVoteErd,
      &vote);
}

static void VoteDontCare(void *context)
{
   CompressorMinimumOnOffTime_t *instance = context;

   VoteForCompressor(instance, CompressorSpeed_Off, Vote_DontCare);
}

static bool CompressorIsSwitchingToOn(CompressorMinimumOnOffTime_t *instance, CompressorSpeed_t newSpeed)
{
   return ((instance->_private.previousCompressorSpeed == CompressorSpeed_Off) && (newSpeed != CompressorSpeed_Off));
}

static void ResolvedVoteChanged(void *context, const void *args)
{
   CompressorMinimumOnOffTime_t *instance = context;
   const CompressorVotedSpeed_t *resolvedVote = args;

   if(CompressorIsSwitchingToOn(instance, resolvedVote->speed))
   {
      VoteForCompressor(instance, resolvedVote->speed, Vote_Care);

      TimerModule_StartOneShot(
         DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
         &instance->_private.timer,
         instance->_private.timesData->minimumOnTimeInMinutes * MSEC_PER_MIN,
         VoteDontCare,
         instance);
   }
   else if(resolvedVote->speed == CompressorSpeed_Off)
   {
      VoteForCompressor(instance, resolvedVote->speed, Vote_Care);

      TimerModule_StartOneShot(
         DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
         &instance->_private.timer,
         instance->_private.timesData->minimumOffTimeInMinutes * MSEC_PER_MIN,
         VoteDontCare,
         instance);
   }

   instance->_private.previousCompressorSpeed = resolvedVote->speed;
}

void CompressorMinimumOnOffTime_Init(
   CompressorMinimumOnOffTime_t *instance,
   I_DataModel_t *dataModel,
   const CompressorTimes_t *timesData,
   const CompressorMinimumOnOffTimeConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.timesData = timesData;
   instance->_private.config = config;

   CompressorVotedSpeed_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedVoteErd,
      &vote);
   instance->_private.previousCompressorSpeed = vote.speed;

   EventSubscription_Init(
      &instance->_private.resolvedVoteSubscription,
      instance,
      ResolvedVoteChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->resolvedVoteErd,
      &instance->_private.resolvedVoteSubscription);
}
