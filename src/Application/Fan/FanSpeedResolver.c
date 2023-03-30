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

enum
{
   HighAmbientTempDegFx100 = 4500,
};

static bool CareAboutHighAmbientTemperature(FanSpeedResolver_t *instance)
{
   return instance->_private.fanData->speedData.careAboutHighAmbientTemperature;
}

static bool FanDataCaresAboutCoolingMode(FanSpeedResolver_t *instance)
{
   return instance->_private.fanData->careAboutCoolingMode;
}

static bool CareAboutSetpoint(FanSpeedResolver_t *instance)
{
   return instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpoint;
}

static FanControl_t FanControlFromAmbientTemperature(FanSpeedResolver_t *instance)
{
   TemperatureDegFx100_t ambientTemp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientTempErd,
      &ambientTemp);

   if(CareAboutHighAmbientTemperature(instance) && ambientTemp >= HighAmbientTempDegFx100)
   {
      return instance->_private.fanData->speedData.superLowSpeedHighAmbientTemperature;
   }
   else
   {
      return instance->_private.fanData->speedData.superLowSpeed;
   }
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
         control = FanControlFromAmbientTemperature(instance);
         break;

      case FanSpeed_Low:
         control = instance->_private.fanData->speedData.lowSpeed;
         break;

      case FanSpeed_Medium:
         control = instance->_private.fanData->speedData.mediumSpeed;
         break;

      case FanSpeed_High:
         control = instance->_private.fanData->speedData.highSpeed;
         break;

      case FanSpeed_SuperHigh:
         control = instance->_private.fanData->speedData.superHighSpeed;
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
      control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood;
   }
   else if(coolingMode == CoolingMode_ConvertibleCompartment)
   {
      if(freezerSetpointZone == SetpointZone_Cold)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithColdSetpoint;
      }
      else if(freezerSetpointZone == SetpointZone_Middle)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithMediumSetpoint;
      }
      else if(freezerSetpointZone == SetpointZone_Warm)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.lowSpeedConvertibleCompartmentWithWarmSetpoint;
      }
   }
   else
   {
      if(freezerSetpointZone == SetpointZone_Cold)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint;
      }
      else if(freezerSetpointZone == SetpointZone_Middle)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithMediumSetpoint;
      }
      else if(freezerSetpointZone == SetpointZone_Warm)
      {
         control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithWarmSetpoint;
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
         control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.superLowSpeed;
         break;

      case FanSpeed_Low:
         control = FanControlFromCoolingModeAndFreezerSetpoint(instance);
         break;

      case FanSpeed_Medium:
         if(coolingMode == CoolingMode_FreshFood)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood;
         }
         else if(coolingMode == CoolingMode_ConvertibleCompartment)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.mediumSpeedConvertibleCompartment;
         }
         else
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer;
         }
         break;

      case FanSpeed_High:
         if(coolingMode == CoolingMode_FreshFood)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.highSpeedFreshFood;
         }
         else if(coolingMode == CoolingMode_ConvertibleCompartment)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.highSpeedConvertibleCompartment;
         }
         else
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.highSpeedFreezer;
         }
         break;

      case FanSpeed_SuperHigh:
         control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.setpointSpeeds.superHighSpeed;
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
         control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.nonSetpointSpeeds.superLowSpeed;
         break;

      case FanSpeed_Low:
         if(coolingMode == CoolingMode_FreshFood)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreshFood;
         }
         else
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.nonSetpointSpeeds.lowSpeedFreezer;
         }
         break;

      case FanSpeed_Medium:
         if(coolingMode == CoolingMode_FreshFood)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.nonSetpointSpeeds.mediumSpeedFreezer;
         }
         else
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.nonSetpointSpeeds.mediumSpeedFreezer;
         }
         break;

      case FanSpeed_High:
         if(coolingMode == CoolingMode_FreshFood)
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.nonSetpointSpeeds.highSpeedSpeedFreshFood;
         }
         else
         {
            control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.nonSetpointSpeeds.highSpeedSpeedFreezer;
         }
         break;

      case FanSpeed_SuperHigh:
         control = instance->_private.fanData->careAboutCoolingModeSpeedData.careAboutSetpointData.nonSetpointSpeeds.superHighSpeed;
         break;

      default:
         break;
   }

   return control;
};

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
   if(FanDataCaresAboutCoolingMode(instance))
   {
      CalculateCoolingModeFanSpeed(instance);
   }
   else
   {
      CalculateFanSpeed(instance);
   }
}

static void DataModelChanged(void *context, const void *_args)
{
   FanSpeedResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if((args->erd == instance->_private.config->resolvedFanSpeedVoteErd) ||
      args->erd == instance->_private.config->coolingModeErd)
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
