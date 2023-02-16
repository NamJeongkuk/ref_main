/*!
 * @file
 * @brief On change of resolvedSetpointVoteErd (SetpointVotedTemperature_t),
 * writes the new temperature to resolvedSetpointErd (TemperatureDegFx100_t)
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RESOLVEDSETPOINTWRITER_H
#define RESOLVEDSETPOINTWRITER_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t resolvedSetpointVoteErd; // SetpointVotedTemperature_t
   Erd_t resolvedSetpointErd; // TemperatureDegFx100_t
   Erd_t userSetpointPluginReadyErd; // bool
} ResolvedSetpointWriterConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t resolvedSetpointVoteChangedSubscription;
      I_DataModel_t *dataModel;
      const ResolvedSetpointWriterConfiguration_t *config;
   } _private;
} ResolvedSetpointWriter_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void ResolvedSetpointWriter_Init(
   ResolvedSetpointWriter_t *instance,
   I_DataModel_t *dataModel,
   const ResolvedSetpointWriterConfiguration_t *config);

#endif
