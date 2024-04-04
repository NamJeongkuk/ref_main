/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FanSpeedResolver.h"
#include "FanSpeed.h"
#include "SetpointZone.h"
#include "CoolingMode.h"
#include "TemperatureDegFx100.h"
#include "utils.h"

static bool CareAboutHighAmbientTemperature(FanSpeedResolver_t *instance)
{
   return instance->_private.fanData->careAboutHighAmbientTemperature;
}

static TemperatureDegFx100_t AmbientTemperature(FanSpeedResolver_t *instance)
{
   TemperatureDegFx100_t ambientTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientFilteredTemperatureResolvedInDegFx100Erd,
      &ambientTemperature);

   return ambientTemperature;
}

static RelativeHumidityPercentx100_t AmbientHumidity(FanSpeedResolver_t *instance)
{
   RelativeHumidityPercentx100_t ambientHumidity;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientFilteredHumidityPercentx100ResolvedErd,
      &ambientHumidity);

   return ambientHumidity;
}

static bool AmbientHumiditySensorIsValid(FanSpeedResolver_t *instance)
{
   bool ambientHumiditySensorIsValid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientHumiditySensorIsValidErd,
      &ambientHumiditySensorIsValid);

   return ambientHumiditySensorIsValid;
}

static bool AmbientThermistorIsValid(FanSpeedResolver_t *instance)
{
   bool ambientThermistorIsValid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientThermistorIsValidErd,
      &ambientThermistorIsValid);

   return ambientThermistorIsValid;
}

static bool FanDataCaresAboutCoolingMode(FanSpeedResolver_t *instance)
{
   return instance->_private.fanData->careAboutCoolingMode;
}

static bool CareAboutSetpoint(FanSpeedResolver_t *instance)
{
   return instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpoint;
}

static bool FanAntiSweatBehaviorIsEnabled(FanSpeedResolver_t *instance)
{
   bool fanAntiSweatBehaviorIsEnabled;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fanAntiSweatBehaviorEnabledErd,
      &fanAntiSweatBehaviorIsEnabled);

   return fanAntiSweatBehaviorIsEnabled;
}

static FanControl_t SuperLowFanControl(FanSpeedResolver_t *instance, FanControl_t defaultFanControl)
{
   FanControl_t control = defaultFanControl;

   if(CareAboutHighAmbientTemperature(instance) && FanAntiSweatBehaviorIsEnabled(instance))
   {
      if(AmbientHumiditySensorIsValid(instance) && AmbientThermistorIsValid(instance))
      {
         if(AmbientHumidity(instance) >= instance->_private.fanData->highAmbientTriggerHumidityInPercentx100)
         {
            control = instance->_private.fanData->superLowAtHighAmbientHumidityAndLowTemperature;

            if(AmbientTemperature(instance) >= instance->_private.fanData->highAmbientTriggerTemperatureInDegFx100)
            {
               control = instance->_private.fanData->superLowAtHighAmbientHumidityAndHighTemperature;
            }
         }
      }
   }

   return control;
}

static void CalculateFanSpeed(FanSpeedResolver_t *instance)
{
   FanVotedSpeed_t votedSpeed;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedFanSpeedVoteErd,
      &votedSpeed);

   FanControl_t control = {
      .type = FanControlType_DutyCycle,
      .dutyCycle = 0,
   };

   switch(votedSpeed.speed)
   {
      case FanSpeed_SuperLow:
         control = SuperLowFanControl(instance, instance->_private.fanData->speedData->superLowSpeed);
         break;

      case FanSpeed_Low:
         control = instance->_private.fanData->speedData->lowSpeed;
         break;

      case FanSpeed_Medium:
         control = instance->_private.fanData->speedData->mediumSpeed;
         break;

      case FanSpeed_High:
         control = instance->_private.fanData->speedData->highSpeed;
         break;

      case FanSpeed_SuperHigh:
         control = instance->_private.fanData->speedData->superHighSpeed;
         break;

      default:
         break;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->calculatedRequestFanControlErd,
      &control);
}

static FanControl_t FanControlFromCoolingModeAndFreezerSetpoint(FanSpeedResolver_t *instance)
{
   SetpointZone_t freezerSetpointZone;
   CoolingMode_t coolingMode;

   FanControl_t control = {
      .type = FanControlType_DutyCycle,
      .dutyCycle = 0,
   };

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointErd,
      &freezerSetpointZone);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->coolingModeErd,
      &coolingMode);

   if(coolingMode == CoolingMode_FreshFood)
   {
      control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood;
   }
   else if(coolingMode == CoolingMode_ConvertibleCompartment)
   {
      if(freezerSetpointZone == SetpointZone_Cold)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithColdSetpoint;
      }
      else if(freezerSetpointZone == SetpointZone_Middle)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithMediumSetpoint;
      }
      else if(freezerSetpointZone == SetpointZone_Warm)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithWarmSetpoint;
      }
   }
   else
   {
      if(freezerSetpointZone == SetpointZone_Cold)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint;
      }
      else if(freezerSetpointZone == SetpointZone_Middle)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithMediumSetpoint;
      }
      else if(freezerSetpointZone == SetpointZone_Warm)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithWarmSetpoint;
      }
   }

   return control;
}

static FanControl_t CoolingModeFanSpeedWithSetpoint(FanSpeedResolver_t *instance)
{
   FanVotedSpeed_t votedSpeed;
   CoolingMode_t coolingMode;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedFanSpeedVoteErd,
      &votedSpeed);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->coolingModeErd,
      &coolingMode);

   FanControl_t control = {
      .type = FanControlType_DutyCycle,
      .dutyCycle = 0,
   };

   switch(votedSpeed.speed)
   {
      case FanSpeed_SuperLow:
         control = SuperLowFanControl(instance, instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.superLowSpeed);
         break;

      case FanSpeed_Low:
         control = FanControlFromCoolingModeAndFreezerSetpoint(instance);
         break;

      case FanSpeed_Medium:
         if(coolingMode == CoolingMode_FreshFood)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood;
         }
         else if(coolingMode == CoolingMode_ConvertibleCompartment)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedConvertibleCompartment;
         }
         else
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer;
         }
         break;

      case FanSpeed_High:
         if(coolingMode == CoolingMode_FreshFood)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood;
         }
         else if(coolingMode == CoolingMode_ConvertibleCompartment)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedConvertibleCompartment;
         }
         else
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer;
         }
         break;

      case FanSpeed_SuperHigh:
         control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.superHighSpeed;
         break;

      default:
         break;
   }

   return control;
}

static FanControl_t CoolingModeFanSpeedWithoutSetpoint(FanSpeedResolver_t *instance)
{
   FanVotedSpeed_t votedSpeed;
   CoolingMode_t coolingMode;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedFanSpeedVoteErd,
      &votedSpeed);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->coolingModeErd,
      &coolingMode);

   FanControl_t control = {
      .type = FanControlType_DutyCycle,
      .dutyCycle = 0,
   };

   switch(votedSpeed.speed)
   {
      case FanSpeed_SuperLow:
         control = SuperLowFanControl(instance, instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.nonSetpointSpeeds.superLowSpeed);
         break;

      case FanSpeed_Low:
         if(coolingMode == CoolingMode_FreshFood)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreshFood;
         }
         else
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreezer;
         }
         break;

      case FanSpeed_Medium:
         if(coolingMode == CoolingMode_FreshFood)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.nonSetpointSpeeds.mediumSpeedFreshFood;
         }
         else
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.nonSetpointSpeeds.mediumSpeedFreezer;
         }
         break;

      case FanSpeed_High:
         if(coolingMode == CoolingMode_FreshFood)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.nonSetpointSpeeds.highSpeedFreshFood;
         }
         else
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.nonSetpointSpeeds.highSpeedFreezer;
         }
         break;

      case FanSpeed_SuperHigh:
         control = instance->_private.fanData->careAboutCoolingModeSpeedData->careAboutSetpointData.nonSetpointSpeeds.superHighSpeed;
         break;

      default:
         break;
   }

   return control;
};

static bool VotedSpeedIsCustomSpeedInRpm(FanSpeedResolver_t *instance)
{
   FanVotedSpeed_t votedSpeed;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedFanSpeedVoteErd,
      &votedSpeed);

   return votedSpeed.speed >= FanSpeed_NumberOfSpeeds;
}

static void UpdateFanControlToResolvedFanSpeed(FanSpeedResolver_t *instance)
{
   FanVotedSpeed_t votedSpeed;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedFanSpeedVoteErd,
      &votedSpeed);

   FanControl_t control = {
      .type = FanControlType_Rpm,
      .rpm = votedSpeed.speed,
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->calculatedRequestFanControlErd,
      &control);
}

static void CalculateCoolingModeFanSpeed(FanSpeedResolver_t *instance)
{
   FanControl_t control = {
      .type = FanControlType_DutyCycle,
      .dutyCycle = 0,
   };

   if(CareAboutSetpoint(instance))
   {
      control = CoolingModeFanSpeedWithSetpoint(instance);
   }
   else
   {
      control = CoolingModeFanSpeedWithoutSetpoint(instance);
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->calculatedRequestFanControlErd,
      &control);
}

static void CalculateResolvedFanSpeedBasedOnDependence(FanSpeedResolver_t *instance)
{
   if(VotedSpeedIsCustomSpeedInRpm(instance))
   {
      UpdateFanControlToResolvedFanSpeed(instance);
   }
   else
   {
      if(FanDataCaresAboutCoolingMode(instance))
      {
         CalculateCoolingModeFanSpeed(instance);
      }
      else
      {
         CalculateFanSpeed(instance);
      }
   }
}

static void DataModelChanged(void *context, const void *_args)
{
   FanSpeedResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;
   Erd_t erd = args->erd;

   if((erd == instance->_private.config->resolvedFanSpeedVoteErd) ||
      erd == instance->_private.config->coolingModeErd ||
      erd == instance->_private.config->ambientHumiditySensorIsValidErd ||
      erd == instance->_private.config->ambientThermistorIsValidErd ||
      erd == instance->_private.config->ambientFilteredTemperatureResolvedInDegFx100Erd ||
      erd == instance->_private.config->ambientFilteredHumidityPercentx100ResolvedErd)
   {
      CalculateResolvedFanSpeedBasedOnDependence(instance);
   }
}

void FanSpeedResolver_Init(
   FanSpeedResolver_t *instance,
   I_DataModel_t *dataModel,
   const FanData_t *fanData,
   const FanSpeedResolverConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.fanData = fanData;
   instance->_private.config = config;

   CalculateResolvedFanSpeedBasedOnDependence(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);
   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
