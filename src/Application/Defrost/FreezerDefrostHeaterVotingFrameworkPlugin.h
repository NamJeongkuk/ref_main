/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FREEZERDEFROSTHEATERVOTINGFRAMEWORKPLUGIN_H
#define FREEZERDEFROSTHEATERVOTINGFRAMEWORKPLUGIN_H

#include "ErdResolver.h"
#include "ResolvedVoteRelayConnector.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      ErdResolver_t freezerDefrostHeaterErdResolver;
      ResolvedVoteRelayConnector_t freezerDefrostHeaterRelayConnector;
   } _private;
} FreezerDefrostHeaterVotingFrameworkPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FreezerDefrostHeaterVotingFrameworkPlugin_Init(
   FreezerDefrostHeaterVotingFrameworkPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
