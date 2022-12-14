/*!
 * @file
 * @brief Connect any vote ERD that has a bool to a relay output
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RESOLVEDVOTERELAYCONNECTOR_H
#define RESOLVEDVOTERELAYCONNECTOR_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t resolvedRelayVoteErd; // xVotedState_t
   Erd_t relayOutputErd; // bool
} ResolvedVoteRelayConnectorConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const ResolvedVoteRelayConnectorConfiguration_t *config;
   } _private;
} ResolvedVoteRelayConnector_t;

/*!
 * @param instance
 * @param dataModel
 * @param configuration
 */
void ResolvedVoteRelayConnector_Init(
   ResolvedVoteRelayConnector_t *instance,
   I_DataModel_t *dataModel,
   const ResolvedVoteRelayConnectorConfiguration_t *config);

#endif
