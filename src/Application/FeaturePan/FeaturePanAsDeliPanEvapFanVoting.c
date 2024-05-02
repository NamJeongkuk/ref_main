/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanAsDeliPanEvapFanVoting.h"
#include "utils.h"

static void Vote(FeaturePanAsDeliPanEvapFanVoting_t *instance, Vote_t care, FanSpeed_t fanSpeed)
{
   FanVotedSpeed_t freshFoodEvapFanVotedSpeed = { .speed = fanSpeed, .care = care };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fanEvapVoteErd,
      &freshFoodEvapFanVotedSpeed);
};

static FeaturePanCoolingMode_t FeaturePanCoolingMode(FeaturePanAsDeliPanEvapFanVoting_t *instance)
{
   FeaturePanCoolingMode_t featurePanCoolingMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->featurePanCoolingModeErd,
      &featurePanCoolingMode);

   return featurePanCoolingMode;
}

static FanVotedSpeed_t GridVotedSpeed(FeaturePanAsDeliPanEvapFanVoting_t *instance)
{
   FanVotedSpeed_t fanVotedSpeed;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fanEvapGridVoteErd,
      &fanVotedSpeed);

   return fanVotedSpeed;
}

static bool FanVotedSpeedIsNotOffAndCare(FanVotedSpeed_t fanVotedSpeed)
{
   return (fanVotedSpeed.speed != FanSpeed_Off && fanVotedSpeed.care == Vote_Care);
}

static FanSpeed_t DeliCoolingFanSpeed(FeaturePanAsDeliPanEvapFanVoting_t *instance, FanVotedSpeed_t gridVotedSpeed)
{
   if(FanVotedSpeedIsNotOffAndCare(gridVotedSpeed))
   {
      return MAX(instance->_private.featurePanData->freshFoodFanDeliCoolingSpeed, gridVotedSpeed.speed);
   }
   
   return FanSpeed_Off;
}

static void UpdateVote(
   FeaturePanAsDeliPanEvapFanVoting_t *instance,
   FanVotedSpeed_t gridVotedSpeed,
   FeaturePanCoolingMode_t featurePanCoolingMode)
{
   if(featurePanCoolingMode == FeaturePanCoolingMode_ActiveCooling && gridVotedSpeed.care == Vote_Care)
   {
      Vote(instance, Vote_Care, DeliCoolingFanSpeed(instance, gridVotedSpeed));
   }
   else
   {
      Vote(instance, Vote_DontCare, FanSpeed_Off);
   }
}

static void OnDataModelChange(void *context, const void *_args)
{
   FeaturePanAsDeliPanEvapFanVoting_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->featurePanCoolingModeErd)
   {
      const FeaturePanCoolingMode_t *featurePanCoolingMode = args->data;
      UpdateVote(instance, GridVotedSpeed(instance), *featurePanCoolingMode);
   }
   else if(args->erd == instance->_private.config->fanEvapGridVoteErd)
   {
      const FanVotedSpeed_t *gridVotedSpeed = args->data;
      UpdateVote(instance, *gridVotedSpeed, FeaturePanCoolingMode(instance));
   }
}

void FeaturePanAsDeliPanEvapFanVoting_Init(
   FeaturePanAsDeliPanEvapFanVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanAsDeliPanEvapFanVotingConfig_t *config,
   const FeaturePanData_t *featurePanData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.featurePanData = featurePanData;

   UpdateVote(instance, 
      GridVotedSpeed(instance), 
      FeaturePanCoolingMode(instance));

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
