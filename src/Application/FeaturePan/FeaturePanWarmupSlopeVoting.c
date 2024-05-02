/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanWarmupSlopeVoting.h"
#include "utils.h"
#include "FeaturePanMode.h"
#include "DataModelErdPointerAccess.h"
#include "I_DataModel.h"
#include "Constants_Time.h"
#include "PercentageDutyCycleVote.h"

enum
{
   SlopeCalculationTimeInMinutes = 1,
   SlopeCalculationTimeInMsec = SlopeCalculationTimeInMinutes * MSEC_PER_MIN,
};

static void Vote(FeaturePanWarmupSlopeVoting_t *instance, Vote_t vote, PercentageDutyCycle_t dutyCycle)
{
   PercentageDutyCycleVote_t heaterVote = { .care = vote, .percentageDutyCycle = dutyCycle };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->heaterVotedErd,
      &heaterVote);
};

static void PerformSlopeVote(FeaturePanWarmupSlopeVoting_t *instance)
{
   if(instance->_private.currentFeaturePanMode == FeaturePanCoolingMode_ActiveHeating)
   {
      if(instance->_private.slopeOfFeaturePanTemperature < instance->_private.featurePanData->featurePanWarmupSlopeLowerLimitInDegFx100PerMinute)
      {
         Vote(instance, Vote_Care, PercentageDutyCycle_Max);
      }
      else if(instance->_private.slopeOfFeaturePanTemperature > instance->_private.featurePanData->featurePanWarmupSlopeUpperLimitInDegFx100PerMinute)
      {
         Vote(instance, Vote_Care, PercentageDutyCycle_Min);
      }
   }
   else
   {
      Vote(instance, Vote_DontCare, PercentageDutyCycle_Min);
   }
}

static void CalculateSlope(void *context)
{
   FeaturePanWarmupSlopeVoting_t *instance = context;

   TemperatureDegFx100_t featurePanTemperatureDegFx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->featurePanTemperatureDegFx100Erd,
      &featurePanTemperatureDegFx100);

   instance->_private.slopeOfFeaturePanTemperature = (featurePanTemperatureDegFx100 - instance->_private.oldTemperature) / SlopeCalculationTimeInMinutes;
   instance->_private.oldTemperature = featurePanTemperatureDegFx100;
   instance->_private.slopeHasBeenCalculated = true;

   PerformSlopeVote(instance);
}

static void OnDataFeaturePanModeChange(void *context, const void *_args)
{
   FeaturePanWarmupSlopeVoting_t *instance = context;
   const FeaturePanCoolingMode_t *featurePanMode = _args;

   instance->_private.currentFeaturePanMode = *featurePanMode;

   if(instance->_private.slopeHasBeenCalculated)
   {
      PerformSlopeVote(instance);
   }
}

void FeaturePanWarmupSlopeVoting_Init(
   FeaturePanWarmupSlopeVoting_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   const FeaturePanWarmupSlopeVotingConfig_t *config,
   const FeaturePanData_t *featurePanData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.featurePanData = featurePanData;

   DataModel_Read(
      dataModel,
      instance->_private.config->featurePanCoolingModeErd,
      &instance->_private.currentFeaturePanMode);

   DataModel_Read(
      dataModel,
      instance->_private.config->featurePanTemperatureDegFx100Erd,
      &instance->_private.oldTemperature);

   if(instance->_private.currentFeaturePanMode == FeaturePanCoolingMode_ActiveHeating)
   {
      Vote(instance, Vote_Care, PercentageDutyCycle_Min);
   }
   else
   {
      Vote(instance, Vote_DontCare, PercentageDutyCycle_Min);
   }

   instance->_private.slopeHasBeenCalculated = false;

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataFeaturePanModeChange);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->featurePanCoolingModeErd,
      &instance->_private.onDataModelChangeSubscription);

   TimerModule_StartPeriodic(
      timerModule,
      &instance->_private.slopeCalculatorTimer,
      SlopeCalculationTimeInMsec,
      CalculateSlope,
      instance);
}
