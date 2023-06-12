/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ResolvedVoteRelayConnector.h"
#include "SystemErds.h"
#include "RelayVotedState.h"
#include "Constants_Binary.h"

static void VoteForRelay(ResolvedVoteRelayConnector_t *instance, RelayState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->relayOutputErd,
      &state);
}

static void VotedStateChanged(void *context, const void *args)
{
   ResolvedVoteRelayConnector_t *instance = context;
   const RelayVotedState_t *votedState = args;
   VoteForRelay(instance, votedState->state);
}

void ResolvedVoteRelayConnector_Init(
   ResolvedVoteRelayConnector_t *instance,
   I_DataModel_t *dataModel,
   const ResolvedVoteRelayConnectorConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   RelayVotedState_t relayVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedRelayVoteErd,
      &relayVote);
   VoteForRelay(instance, relayVote.state);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      VotedStateChanged);

   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->resolvedRelayVoteErd,
      &instance->_private.dataModelSubscription);
}
