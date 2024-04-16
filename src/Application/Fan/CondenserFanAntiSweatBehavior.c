/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CondenserFanAntiSweatBehavior.h"
#include "CoolingMode.h"
#include "FanSpeed.h"

static TemperatureDegFx100_t AmbientTemperature(CondenserFanAntiSweatBehavior_t *instance)
{
   TemperatureDegFx100_t ambientTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientFilteredTemperatureResolvedInDegFx100Erd,
      &ambientTemperature);

   return ambientTemperature;
}

static RelativeHumidityPercentx100_t AmbientHumidity(CondenserFanAntiSweatBehavior_t *instance)
{
   RelativeHumidityPercentx100_t ambientHumidity;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientFilteredHumidityPercentx100ResolvedErd,
      &ambientHumidity);

   return ambientHumidity;
}

static bool AmbientHumiditySensorIsValid(CondenserFanAntiSweatBehavior_t *instance)
{
   bool ambientHumiditySensorIsValid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientHumiditySensorIsValidErd,
      &ambientHumiditySensorIsValid);

   return ambientHumiditySensorIsValid;
}

static bool AmbientTemperatureSensorIsValid(CondenserFanAntiSweatBehavior_t *instance)
{
   bool ambientThermistorIsValid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientTemperatureSensorIsValidErd,
      &ambientThermistorIsValid);

   return ambientThermistorIsValid;
}

static bool CondenserFanAntiSweatIsEnabledByGrid(CondenserFanAntiSweatBehavior_t *instance)
{
   bool fanAntiSweatBehaviorIsEnabled;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->condenserFanAntiSweatBehaviorEnabledByGridErd,
      &fanAntiSweatBehaviorIsEnabled);

   return fanAntiSweatBehaviorIsEnabled;
}

static CoolingMode_t CoolingMode(CondenserFanAntiSweatBehavior_t *instance)
{
   CoolingMode_t mode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->coolingModeErd,
      &mode);

   return mode;
}

static void VoteForCondenserFanAntiSweatBehaviorBasedOnConditions(CondenserFanAntiSweatBehavior_t *instance)
{
   FanVotedSpeed_t votedSpeed;

   if((AmbientHumidity(instance) >= instance->_private.fanData->highAmbientTriggerHumidityInPercentx100) &&
      AmbientHumiditySensorIsValid(instance) &&
      AmbientTemperatureSensorIsValid(instance) &&
      CondenserFanAntiSweatIsEnabledByGrid(instance))
   {
      FanControl_t control;
      bool coolingModeIsFreshFood = (CoolingMode(instance) == CoolingMode_FreshFood);

      if(AmbientTemperature(instance) >= instance->_private.fanData->highAmbientTriggerTemperatureInDegFx100)
      {
         control = coolingModeIsFreshFood
            ? instance->_private.fanData->highAmbientHumidityAndHighTemperatureFreshFoodCoolingMode
            : instance->_private.fanData->highAmbientHumidityAndHighTemperature;
      }
      else
      {
         control = coolingModeIsFreshFood
            ? instance->_private.fanData->highAmbientHumidityAndLowTemperatureFreshFoodCoolingMode
            : instance->_private.fanData->highAmbientHumidityAndLowTemperature;
      }

      votedSpeed.care = Vote_Care;
      votedSpeed.speed = control.rpm;
   }
   else
   {
      votedSpeed.care = Vote_DontCare;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->condenserFanAntiSweatBehaviorVoteErd,
      &votedSpeed);
}

static void DataModelUpdated(void *context, const void *_args)
{
   CondenserFanAntiSweatBehavior_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->ambientFilteredHumidityPercentx100ResolvedErd ||
      args->erd == instance->_private.config->ambientFilteredTemperatureResolvedInDegFx100Erd ||
      args->erd == instance->_private.config->ambientHumiditySensorIsValidErd ||
      args->erd == instance->_private.config->ambientTemperatureSensorIsValidErd ||
      args->erd == instance->_private.config->condenserFanAntiSweatBehaviorEnabledByGridErd ||
      args->erd == instance->_private.config->coolingModeErd)
   {
      VoteForCondenserFanAntiSweatBehaviorBasedOnConditions(instance);
   }
}

void CondenserFanAntiSweatBehavior_Init(
   CondenserFanAntiSweatBehavior_t *instance,
   I_DataModel_t *dataModel,
   const CondenserFanAntiSweatBehaviorConfig_t *config,
   const FanData_t *fanData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.fanData = fanData;
   instance->_private.config = config;

   VoteForCondenserFanAntiSweatBehaviorBasedOnConditions(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelUpdated);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);
}
