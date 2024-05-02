/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorStartup.h"
#include "DataModelErdPointerAccess.h"
#include "FanSpeed.h"
#include "Constants_Time.h"
#include "CoolingMode.h"
#include "SystemErds.h"
#include "Vote.h"

typedef CompressorStartup_t Instance_t;

static void VoteForCondenserFanSpeed(Instance_t *instance, FanSpeed_t speed, Vote_t care)
{
   FanVotedSpeed_t vote = { .speed = speed, .care = care };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->condenserFanSpeedStartupVoteErd,
      &vote);
}

static void VoteForFreshFoodEvaporatorFanSpeed(Instance_t *instance, FanSpeed_t speed, Vote_t care)
{
   FanVotedSpeed_t vote = { .speed = speed, .care = care };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->freshFoodEvaporatorFanSpeedStartupVoteErd,
      &vote);
}

static void VoteForFreezerEvaporatorFanSpeed(Instance_t *instance, FanSpeed_t speed, Vote_t care)
{
   FanVotedSpeed_t vote = { .speed = speed, .care = care };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->freezerEvaporatorFanSpeedStartupVoteErd,
      &vote);
}

static void VoteForCompressor(Instance_t *instance, CompressorSpeed_t speed, Vote_t care)
{
   CompressorVotedSpeed_t vote = { .speed = speed, .care = care };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->compressorSpeedStartupVoteErd,
      &vote);
}

static CoolingMode_t CoolingMode(Instance_t *instance)
{
   CoolingMode_t coolingMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->coolingModeErd,
      &coolingMode);

   return coolingMode;
}

static void TimerExpired(void *context)
{
   Instance_t *instance = context;

   VoteForCompressor(instance, CompressorSpeed_Off, Vote_DontCare);
   VoteForCondenserFanSpeed(instance, FanSpeed_Off, Vote_DontCare);
   VoteForFreshFoodEvaporatorFanSpeed(instance, FanSpeed_Off, Vote_DontCare);
   VoteForFreezerEvaporatorFanSpeed(instance, FanSpeed_Off, Vote_DontCare);
}

static void StartStartupTimer(Instance_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer,
      instance->_private.parametric->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC,
      TimerExpired,
      instance);
}

static bool CompressorIsSwitchingToOn(Instance_t *instance, CompressorSpeed_t newSpeed)
{
   return ((instance->_private.previousCompressorSpeed == CompressorSpeed_Off) && (newSpeed != CompressorSpeed_Off));
}

static void CompressorStateChanged(void *context, const void *args)
{
   Instance_t *instance = context;
   const CompressorVotedSpeed_t *resolvedVote = args;

   if(CompressorIsSwitchingToOn(instance, resolvedVote->speed))
   {
      if(!instance->_private.parametric->compressorIsSingleSpeed)
      {
         VoteForCompressor(
            instance,
            instance->_private.parametric->compressorSpeeds.startupSpeedFrequencyInHz,
            Vote_Care);
      }

      VoteForCondenserFanSpeed(instance, FanSpeed_Low, Vote_Care);

      switch(instance->_private.numberOfEvaporators)
      {
         case 1:
            VoteForFreshFoodEvaporatorFanSpeed(instance, FanSpeed_Low, Vote_Care);
            VoteForFreezerEvaporatorFanSpeed(instance, FanSpeed_Low, Vote_Care);
            break;

         case 2:
         case 3:
            if(CoolingMode(instance) == CoolingMode_FreshFood)
            {
               VoteForFreshFoodEvaporatorFanSpeed(instance, FanSpeed_Low, Vote_Care);
               VoteForFreezerEvaporatorFanSpeed(instance, FanSpeed_Off, Vote_Care);
            }
            else
            {
               VoteForFreshFoodEvaporatorFanSpeed(instance, FanSpeed_Off, Vote_Care);
               VoteForFreezerEvaporatorFanSpeed(instance, FanSpeed_Low, Vote_Care);
            }
            break;

         default:
            break;
      }

      StartStartupTimer(instance);
   }

   instance->_private.previousCompressorSpeed = resolvedVote->speed;
}

void CompressorStartup_Init(
   Instance_t *instance,
   I_DataModel_t *dataModel,
   const CompressorData_t *parametric,
   uint8_t numberOfEvaporators,
   const CompressorStartupConfiguration_t *configuration)
{
   instance->_private.dataModel = dataModel;
   instance->_private.configuration = configuration;
   instance->_private.parametric = parametric;
   instance->_private.numberOfEvaporators = numberOfEvaporators;

   CompressorVotedSpeed_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->compressorResolvedVotedSpeedErd,
      &vote);
   instance->_private.previousCompressorSpeed = vote.speed;

   EventSubscription_Init(
      &instance->_private.compressorStateSubscription,
      instance,
      CompressorStateChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.configuration->compressorResolvedVotedSpeedErd,
      &instance->_private.compressorStateSubscription);
}
