/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ColdestOfSetpointVotes.h"
#include "SetpointVotedTemperature.h"

static void VoteForColdestSetpointAndSetWinningVoteErd(ColdestOfSetpointVotes_t *instance)
{
   SetpointVotedTemperature_t coldestVote = { INT16_MAX, Vote_DontCare };
   Erd_t winningVoteErd = Erd_Invalid;

   for(uint8_t i = 0; i < instance->_private.config->listOfSetpointVoteErds.numberOfErds; i++)
   {
      SetpointVotedTemperature_t nextVote;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->listOfSetpointVoteErds.erds[i],
         &nextVote);

      if(nextVote.care && (nextVote.temperatureInDegFx100 < coldestVote.temperatureInDegFx100))
      {
         coldestVote = nextVote;
         winningVoteErd = instance->_private.config->listOfSetpointVoteErds.erds[i];
      }
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->coldestSetpointVoteErd,
      &coldestVote);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->coldestSetpointWinningVoteErd,
      &winningVoteErd);
}

static void DataModelChanged(void *context, const void *_args)
{
   ColdestOfSetpointVotes_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   for(uint8_t i = 0; i < instance->_private.config->listOfSetpointVoteErds.numberOfErds; i++)
   {
      if(args->erd == instance->_private.config->listOfSetpointVoteErds.erds[i])
      {
         VoteForColdestSetpointAndSetWinningVoteErd(instance);
         break;
      }
   }
}

void ColdestOfSetpointVotes_Init(
   ColdestOfSetpointVotes_t *instance,
   I_DataModel_t *dataModel,
   const ColdestOfSetpointVotesConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   VoteForColdestSetpointAndSetWinningVoteErd(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);
}
