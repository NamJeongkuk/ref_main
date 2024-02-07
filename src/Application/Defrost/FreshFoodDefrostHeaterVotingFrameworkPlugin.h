/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODDEFROSTHEATERVOTINGFRAMEWORKPLUGIN_H
#define FRESHFOODDEFROSTHEATERVOTINGFRAMEWORKPLUGIN_H

#include "ErdResolver.h"
#include "ResolvedVoteRelayConnector.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      ErdResolver_t freshFoodDefrostHeaterErdResolver;
      ResolvedVoteRelayConnector_t freshFoodDefrostHeaterRelayConnector;
   } _private;
} FreshFoodDefrostHeaterVotingFrameworkPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FreshFoodDefrostHeaterVotingFrameworkPlugin_Init(
   FreshFoodDefrostHeaterVotingFrameworkPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
