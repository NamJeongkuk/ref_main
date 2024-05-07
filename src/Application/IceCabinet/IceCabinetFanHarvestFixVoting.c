/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetFanHarvestFixVoting.h"
#include "SystemErds.h"

static void UpdateIceCabinetFanHarvestFixVote(IceCabinetFanHarvestFixVoting_t *instance, const IceMakerStateMachineState_t iceMakerState)
{
   FanVotedSpeed_t vote = { .speed = FanSpeed_Off, .care = Vote_DontCare };

   if(iceMakerState == IceMakerStateMachineState_HarvestFix)
   {
      vote.care = Vote_Care;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetFanHarvestFixVoteErd,
      &vote);
}

static void OnIceMakerStateChange(void *context, const void *args)
{
   IceCabinetFanHarvestFixVoting_t *instance = context;
   const IceMakerStateMachineState_t *iceMakerState = args;

   UpdateIceCabinetFanHarvestFixVote(instance, *iceMakerState);
}

void IceCabinetFanHarvestFixVoting_Init(
   IceCabinetFanHarvestFixVoting_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetFanHarvestFixVotingConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   IceMakerStateMachineState_t iceMakerState;
   DataModel_Read(
      dataModel,
      instance->_private.config->iceMakerStateErd,
      &iceMakerState);

   UpdateIceCabinetFanHarvestFixVote(instance, iceMakerState);

   EventSubscription_Init(
      &instance->_private.iceMakerStateSubscription,
      instance,
      OnIceMakerStateChange);
   DataModel_Subscribe(
      dataModel,
      config->iceMakerStateErd,
      &instance->_private.iceMakerStateSubscription);
}
