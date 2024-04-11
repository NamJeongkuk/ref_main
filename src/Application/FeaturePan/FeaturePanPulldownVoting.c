/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanPulldownVoting.h"
#include "utils.h"
#include "PercentageDutyCycleVote.h"
#include "DamperVotedPosition.h"
#include "FanSpeed.h"

static void Vote(FeaturePanPulldownVoting_t *instance, Vote_t vote)
{
   DamperVotedPosition_t damperVote = { .position = DamperPosition_Closed, .care = vote };
   FanVotedSpeed_t fanVote = { .speed = FanSpeed_Off, .care = vote };
   PercentageDutyCycleVote_t dutyCycleVote = { .percentageDutyCycle = PercentageDutyCycle_Min, .care = vote };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->featurePanDamperPositionVoteErd,
      &damperVote);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->featurePanFanVoteErd,
      &fanVote);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->featurePanHeaterVoteErd,
      &dutyCycleVote);
};

static TemperatureDegFx100_t FeaturePanTemperature(FeaturePanPulldownVoting_t *instance)
{
   TemperatureDegFx100_t featurePanTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->featurePanTemperatureErd,
      &featurePanTemperature);

   return featurePanTemperature;
}

static void OnFeaturePanTemperatureChange(void *context, const void *_args)
{
   FeaturePanPulldownVoting_t *instance = context;
   const TemperatureDegFx100_t *temperature = _args;

   if(*temperature > instance->_private.featurePanData->featurePanPulldownTemperatureLimitInDegFx100)
   {
      Vote(instance, Vote_Care);
   }
   else
   {
      Vote(instance, Vote_DontCare);
   }
}

void FeaturePanPulldownVoting_Init(
   FeaturePanPulldownVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanPulldownVotingConfig_t *config,
   const FeaturePanData_t *featurePanData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.featurePanData = featurePanData;

   if(FeaturePanTemperature(instance) > instance->_private.featurePanData->featurePanPulldownTemperatureLimitInDegFx100)
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
      OnFeaturePanTemperatureChange);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->featurePanTemperatureErd,
      &instance->_private.onDataModelChangeSubscription);
}
