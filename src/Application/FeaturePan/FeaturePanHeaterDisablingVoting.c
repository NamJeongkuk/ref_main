/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanHeaterDisablingVoting.h"
#include "utils.h"
#include "FeaturePanMode.h"
#include "PercentageDutyCycleVote.h"

static void Vote(FeaturePanHeaterDisablingVoting_t *instance, Vote_t vote)
{
   PercentageDutyCycleVote_t heaterVote = { .percentageDutyCycle = PercentageDutyCycle_Min, .care = vote };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->heaterVotedErd,
      &heaterVote);
};

static TemperatureDegFx100_t AmbientTemperature(FeaturePanHeaterDisablingVoting_t *instance)
{
   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientTemperatureDegFx100Erd,
      &temperature);

   return temperature;
}

static FeaturePanCoolingMode_t FeaturePanCoolingMode(FeaturePanHeaterDisablingVoting_t *instance)
{
   FeaturePanCoolingMode_t featurePanCoolingMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->featurePanCoolingModeErd,
      &featurePanCoolingMode);

   return featurePanCoolingMode;
}

static void UpdateDisableHeatingVote(
   FeaturePanHeaterDisablingVoting_t *instance,
   TemperatureDegFx100_t ambientTemperature,
   FeaturePanCoolingMode_t featurePanCoolingMode)
{
   if(ambientTemperature >= instance->_private.featurePanData->featurePanHeaterHighAmbientTemperatureLimitInDegFx100 &&
      featurePanCoolingMode == FeaturePanCoolingMode_ActiveHeating)
   {
      Vote(instance, Vote_Care);
   }
   else
   {
      Vote(instance, Vote_DontCare);
   }
}

static void OnDataModelChange(void *context, const void *_args)
{
   FeaturePanHeaterDisablingVoting_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->featurePanCoolingModeErd)
   {
      const FeaturePanCoolingMode_t *featurePanCoolingMode = args->data;
      UpdateDisableHeatingVote(instance, AmbientTemperature(instance), *featurePanCoolingMode);
   }
   else if(args->erd == instance->_private.config->ambientTemperatureDegFx100Erd)
   {
      const TemperatureDegFx100_t *ambientTemperature = args->data;
      UpdateDisableHeatingVote(instance, *ambientTemperature, FeaturePanCoolingMode(instance));
   }
}

void FeaturePanHeaterDisablingVoting_Init(
   FeaturePanHeaterDisablingVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanHeaterDisablingVotingConfig_t *config,
   const FeaturePanData_t *featurePanData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.featurePanData = featurePanData;

   UpdateDisableHeatingVote(instance, AmbientTemperature(instance), FeaturePanCoolingMode(instance));

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
