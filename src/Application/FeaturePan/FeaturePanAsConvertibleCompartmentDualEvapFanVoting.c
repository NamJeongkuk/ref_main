/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanAsConvertibleCompartmentDualEvapFanVoting.h"
#include "utils.h"
#include "FanSpeed.h"

static void Vote(FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t *instance, Vote_t vote)
{
   FanVotedSpeed_t freezerFanVote = { .speed = FanSpeed_High, .care = vote };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->evapFanVote,
      &freezerFanVote);
};

static FeaturePanMode_t FeaturePanMode(FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t *instance)
{
   FeaturePanMode_t featurePanMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->featurePanModeErd,
      &featurePanMode);

   return featurePanMode;
}

static void OnFeaturePanModeChange(void *context, const void *_args)
{
   FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t *instance = context;

   const FeaturePanMode_t *featurePanMode = _args;
   if(*featurePanMode == FeaturePanMode_ActiveCooling)
   {
      Vote(instance, Vote_Care);
   }
   else
   {
      Vote(instance, Vote_DontCare);
   }
}

void FeaturePanAsConvertibleCompartmentDualEvapFanVoting_Init(
   FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanAsConvertibleCompartmentDualEvapFanVotingConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   if(FeaturePanMode(instance) == FeaturePanMode_ActiveCooling)
   {
      Vote(instance, Vote_Care);
   }
   else
   {
      Vote(instance, Vote_DontCare);
   }

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnFeaturePanModeChange);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->featurePanModeErd,
      &instance->_private.onDataModelChangeSubscription);
}
