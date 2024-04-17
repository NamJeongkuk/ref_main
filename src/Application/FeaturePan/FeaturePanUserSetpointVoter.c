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

static FeaturePanCurrentMode_t FeaturePanCurrentMode(FeaturePanUserSetpointVoter_t *instance)
{
   FeaturePanCurrentMode_t currentMode;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->featurePanCurrentModeErd,
      &currentMode);

   return currentMode;
}

static void Vote(FeaturePanUserSetpointVoter_t *instance)
{
   FeaturePanCurrentMode_t currentMode = FeaturePanCurrentMode(instance);
   SetpointVotedTemperature_t vote = { .temperatureInDegFx100 = INT16_MAX, .care = Vote_DontCare };

   if(IN_RANGE(FeaturePanCurrentMode_Mode1, currentMode, FeaturePanCurrentMode_Mode7))
   {
      TemperatureDegFx100_t currentModeUserSetpoint;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->resolvedModeSetpointErds[currentMode - 1],
         &currentModeUserSetpoint);

      vote.temperatureInDegFx100 = currentModeUserSetpoint;
      vote.care = Vote_Care;
   }

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

   if(erd == instance->_private.config->featurePanCurrentModeErd ||
      erd == instance->_private.config->resolvedModeSetpointErds[CLAMP(FeaturePanCurrentMode(instance), FeaturePanCurrentMode_Mode1, FeaturePanCurrentMode_Mode7) - 1])
   {
      Vote(instance);
   }
}

void FeaturePanUserSetpointVoter_Init(
   FeaturePanUserSetpointVoter_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanUserSetpointVoterConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   Vote(instance);

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      OnDataModelChanged);

   DataModel_SubscribeAll(dataModel, &instance->_private.subscription);
}
