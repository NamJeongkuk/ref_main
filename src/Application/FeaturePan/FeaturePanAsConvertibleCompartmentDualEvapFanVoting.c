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

static FeaturePanCoolingMode_t FeaturePanCoolingMode(FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t *instance)
{
   FeaturePanCoolingMode_t featurePanCoolingMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->featurePanCoolingModeErd,
      &featurePanCoolingMode);

   return featurePanCoolingMode;
}

static void OnFeaturePanCoolingModeChange(void *context, const void *_args)
{
   FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t *instance = context;

   const FeaturePanCoolingMode_t *featurePanCoolingMode = _args;
   if(*featurePanCoolingMode == FeaturePanCoolingMode_ActiveCooling)
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

   if(FeaturePanCoolingMode(instance) == FeaturePanCoolingMode_ActiveCooling)
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
      OnFeaturePanCoolingModeChange);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->featurePanCoolingModeErd,
      &instance->_private.onDataModelChangeSubscription);
}
