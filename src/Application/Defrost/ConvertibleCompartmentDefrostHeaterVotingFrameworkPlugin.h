/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTDEFROSTHEATERVOTINGFRAMEWORKPLUGIN_H
#define CONVERTIBLECOMPARTMENTDEFROSTHEATERVOTINGFRAMEWORKPLUGIN_H

#include "ErdResolver.h"
#include "I_DataModel.h"
#include "ResolvedVoteRelayConnector.h"

typedef struct
{
   struct
   {
      ErdResolver_t erdResolver;
      ResolvedVoteRelayConnector_t relayConnector;
   } _private;
} ConvertibleCompartmentDefrostHeaterVotingFrameworkPlugin_t;

/*!
 * @brief Plugin for the convertible compartment defrost heater voting
 * @param instance
 * @param dataModel
 */
void ConvertibleCompartmentDefrostHeaterVotingFrameworkPlugin_Init(
   ConvertibleCompartmentDefrostHeaterVotingFrameworkPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
