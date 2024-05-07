/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetFanStartupSpeedVoting.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Time.h"

static void VoteWithStartupSpeed(IceCabinetFanStartupSpeedVoting_t *instance)
{
   FanVotedSpeed_t startupVote = {
      .speed = instance->_private.iceCabinetFanStartupData->startupSpeed,
      .care = Vote_Care
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetFanStartupSpeedVoteErd,
      &startupVote);
}

static void VoteDontCareWithStartupVote(IceCabinetFanStartupSpeedVoting_t *instance)
{
   FanVotedSpeed_t startupVote = {
      .speed = FanSpeed_Off,
      .care = Vote_DontCare
   };
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetFanStartupSpeedVoteErd,
      &startupVote);
}

static void StartupTimeExpired(void *context)
{
   IceCabinetFanStartupSpeedVoting_t *instance = context;

   VoteDontCareWithStartupVote(instance);
}

static void StartStartupTimer(IceCabinetFanStartupSpeedVoting_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer,
      instance->_private.iceCabinetFanStartupData->startupTimeInSeconds * MSEC_PER_SEC,
      StartupTimeExpired,
      instance);
}

static void IceCabinetFanResolvedVoteChanged(void *context, const void *args)
{
   IceCabinetFanStartupSpeedVoting_t *instance = context;
   const FanVotedSpeed_t *vote = args;

   if((instance->_private.previousResolvedFanSpeed == FanSpeed_Off) &&
      (vote->speed != FanSpeed_Off))
   {
      VoteWithStartupSpeed(instance);
      StartStartupTimer(instance);
   }

   instance->_private.previousResolvedFanSpeed = vote->speed;
}

void IceCabinetFanStartupSpeedVoting_Init(
   IceCabinetFanStartupSpeedVoting_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetFanStartupData_t *iceCabinetFanStartupData,
   const IceCabinetFanStartupSpeedVotingConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.iceCabinetFanStartupData = iceCabinetFanStartupData;
   instance->_private.config = config;
   instance->_private.previousResolvedFanSpeed = FanSpeed_Off;

   FanVotedSpeed_t resolvedVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetFanResolvedVoteErd,
      &resolvedVote);

   IceCabinetFanResolvedVoteChanged(instance, &resolvedVote);

   EventSubscription_Init(
      &instance->_private.iceCabinetFanResolvedVoteSubscription,
      instance,
      IceCabinetFanResolvedVoteChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->iceCabinetFanResolvedVoteErd,
      &instance->_private.iceCabinetFanResolvedVoteSubscription);
}
