/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanMode.h"
#include "FeaturePanUserSetpointVoter.h"
#include "SetpointVotedTemperature.h"
#include "TemperatureDegFx100.h"
#include "utils.h"

static void Vote(FeaturePanUserSetpointVoter_t *instance, const FeaturePanCurrentMode_t mode)
{
   TemperatureDegFx100_t currentModeUserSetpoint;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedModeSetpointErds[mode - 1],
      &currentModeUserSetpoint);

   SetpointVotedTemperature_t vote = { .temperatureInDegFx100 = currentModeUserSetpoint, .care = Vote_Care };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->userSetpointVoteErd,
      &vote);
}

static void OnDataModelChanged(void *context, const void *args)
{
   FeaturePanUserSetpointVoter_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   const Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->featurePanCurrentModeErd)
   {
      const FeaturePanCurrentMode_t *currentMode = onChangeData->data;

      Vote(instance, CLAMP(*currentMode, FeaturePanCurrentMode_Mode1, FeaturePanCurrentMode_Mode7));
   }
}

void FeaturePanUserSetpointVoter_Init(
   FeaturePanUserSetpointVoter_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanUserSetpointVoterConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   FeaturePanCurrentMode_t currentMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->featurePanCurrentModeErd,
      &currentMode);

   Vote(instance, CLAMP(currentMode, FeaturePanCurrentMode_Mode1, FeaturePanCurrentMode_Mode7));

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      OnDataModelChanged);

   DataModel_SubscribeAll(dataModel, &instance->_private.subscription);
}
