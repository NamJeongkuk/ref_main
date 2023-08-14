/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ResolvedSetpointWriter.h"
#include "SetpointVotedTemperature.h"
#include "uassert.h"

static void WriteResolvedSetpoint(void *context, const void *args)
{
   ResolvedSetpointWriter_t *instance = context;
   const SetpointVotedTemperature_t *setpointVotedTemperature = args;
   TemperatureDegFx100_t temperature = setpointVotedTemperature->temperatureInDegFx100;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->resolvedSetpointErd,
      &temperature);
}

static bool UserSetpointPluginIsReady(
   I_DataModel_t *dataModel,
   Erd_t userSetpointPluginReadyErd)
{
   bool state;
   DataModel_Read(dataModel, userSetpointPluginReadyErd, &state);
   return state;
}

void ResolvedSetpointWriter_Init(
   ResolvedSetpointWriter_t *instance,
   I_DataModel_t *dataModel,
   const ResolvedSetpointWriterConfiguration_t *config)
{
   uassert(UserSetpointPluginIsReady(dataModel, config->userSetpointPluginReadyErd));

   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   SetpointVotedTemperature_t setpointVotedTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedSetpointVoteErd,
      &setpointVotedTemperature);
   WriteResolvedSetpoint(instance, &setpointVotedTemperature);

   EventSubscription_Init(
      &instance->_private.resolvedSetpointVoteChangedSubscription,
      instance,
      WriteResolvedSetpoint);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->resolvedSetpointVoteErd,
      &instance->_private.resolvedSetpointVoteChangedSubscription);
}
