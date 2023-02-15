/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PersonalityParametricData.h"
#include "CompressorStartupFanVotes.h"
#include "DataModelErdPointerAccess.h"
#include "CompressorState.h"
#include "FanSpeed.h"
#include "Constants_Time.h"
#include "CoolingMode.h"
#include "SystemErds.h"
#include "Vote.h"

static void VoteForCondenserFanSpeed(CompressorStartupFanVotes_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t vote;
   vote.speed = speed;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->condenserFanSpeedVoteErd,
      &vote);
}

static void VoteForFreshFoodEvaporatorFanSpeed(CompressorStartupFanVotes_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t vote;
   vote.speed = speed;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->freshFoodEvaporatorFanSpeedVoteErd,
      &vote);
}

static void VoteForFreezerEvaporatorFanSpeed(CompressorStartupFanVotes_t *instance, FanSpeed_t speed, bool care)
{
   FanVotedSpeed_t vote;
   vote.speed = speed;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->freezerEvaporatorFanSpeedVoteErd,
      &vote);
}

static int NumberOfEvaporators(CompressorStartupFanVotes_t *instance)
{
   const EvaporatorData_t *evaporatorParametricData = PersonalityParametricData_Get(instance->_private.dataModel)->evaporatorData;
   return evaporatorParametricData->numberOfEvaporators;
}

static CoolingMode_t CoolingMode(CompressorStartupFanVotes_t *instance)
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
   CompressorStartupFanVotes_t *instance = context;

   VoteForCondenserFanSpeed(instance, FanSpeed_Off, Vote_DontCare);
   VoteForFreshFoodEvaporatorFanSpeed(instance, FanSpeed_Off, Vote_DontCare);
   VoteForFreezerEvaporatorFanSpeed(instance, FanSpeed_Off, Vote_DontCare);
}

static void StartStartupTimer(CompressorStartupFanVotes_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer,
      instance->_private.compressorParametricData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC,
      TimerExpired,
      instance);
}

static void CompressorStateChanged(void *context, const void *args)
{
   CompressorStartupFanVotes_t *instance = context;
   const CompressorState_t *state = args;

   uint8_t numberOfEvaporators = NumberOfEvaporators(instance);

   if(*state == CompressorState_Startup)
   {
      VoteForCondenserFanSpeed(instance, FanSpeed_Low, Vote_Care);

      switch(numberOfEvaporators)
      {
         case 1:
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
}

void CompressorStartupFanVotes_Init(
   CompressorStartupFanVotes_t *instance,
   I_DataModel_t *dataModel,
   const CompressorStartupFanVotesConfiguration_t *configuration)
{
   instance->_private.dataModel = dataModel;
   instance->_private.configuration = configuration;
   instance->_private.compressorParametricData = PersonalityParametricData_Get(dataModel)->compressorData;

   EventSubscription_Init(
      &instance->_private.compressorStateSubscription,
      instance,
      CompressorStateChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.configuration->compressorStateErd,
      &instance->_private.compressorStateSubscription);
}
