/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorSpeedDriver.h"
#include "Constants_Binary.h"
#include "CompressorVotedSpeed.h"
#include "CoolingMode.h"
#include "SetpointZone.h"
#include "PersonalityParametricData.h"

static void ControlCompressorRelay(
   CompressorSpeedDriver_t *instance,
   CompressorSpeed_t compressorSpeed)
{
   if(compressorSpeed == CompressorSpeed_Off)
   {
      DataModel_Write(instance->_private.dataModel, instance->_private.config->compressorRelayErd, off);
   }
   else
   {
      DataModel_Write(instance->_private.dataModel, instance->_private.config->compressorRelayErd, on);
   }
}

static void UpdateCompressorFrequencyWhenCompressorSpeedIsLow(
   CompressorSpeedDriver_t *instance,
   const CompressorSpeeds_t *compressorSpeeds,
   CoolingMode_t coolingMode,
   SetpointZone_t freezerSetpointZone)
{
   switch(coolingMode)
   {
      case CoolingMode_FreshFood:
         if(freezerSetpointZone == SetpointZone_Warm)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreshFoodWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreshFoodMidFrequencyInHz);
         }
         else
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreshFoodColdFrequencyInHz);
         }
         break;

      case CoolingMode_Freezer:
         if(freezerSetpointZone == SetpointZone_Warm)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreezerWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreezerMidFrequencyInHz);
         }
         else
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreezerColdFrequencyInHz);
         }
         break;

      case CoolingMode_ConvertibleCompartment:
         if(freezerSetpointZone == SetpointZone_Warm)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedConvertibleCompartmentWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedConvertibleCompartmentMidFrequencyInHz);
         }
         else
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedConvertibleCompartmentColdFrequencyInHz);
         }
         break;
   }
}

static void UpdateCompressorFrequencyWhenCompressorSpeedIsMedium(
   CompressorSpeedDriver_t *instance,
   const CompressorSpeeds_t *compressorSpeeds,
   CoolingMode_t coolingMode,
   SetpointZone_t freezerSetpointZone)
{
   switch(coolingMode)
   {
      case CoolingMode_FreshFood:
         if(freezerSetpointZone == SetpointZone_Warm)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreshFoodWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreshFoodMidFrequencyInHz);
         }
         else
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreshFoodColdFrequencyInHz);
         }
         break;

      case CoolingMode_Freezer:
         if(freezerSetpointZone == SetpointZone_Warm)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreezerWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreezerMidFrequencyInHz);
         }
         else
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreezerColdFrequencyInHz);
         }
         break;

      case CoolingMode_ConvertibleCompartment:
         if(freezerSetpointZone == SetpointZone_Warm)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedConvertibleCompartmentWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedConvertibleCompartmentMidFrequencyInHz);
         }
         else
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedConvertibleCompartmentColdFrequencyInHz);
         }
         break;
   }
}

static void UpdateCompressorFrequencyWhenCompressorSpeedIsHigh(
   CompressorSpeedDriver_t *instance,
   const CompressorSpeeds_t *compressorSpeeds,
   CoolingMode_t coolingMode)
{
   switch(coolingMode)
   {
      case CoolingMode_FreshFood:
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->compressorFrequencyErd,
            &compressorSpeeds->coolingModeDependentCompressorSpeeds.highSpeedFreshFoodFrequencyInHz);
         break;

      case CoolingMode_Freezer:
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->compressorFrequencyErd,
            &compressorSpeeds->coolingModeDependentCompressorSpeeds.highSpeedFreezerFrequencyInHz);
         break;

      case CoolingMode_ConvertibleCompartment:
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->compressorFrequencyErd,
            &compressorSpeeds->coolingModeDependentCompressorSpeeds.highSpeedConvertibleCompartmentFrequencyInHz);
         break;
   }
}

static void UpdateCompressorFrequency(
   CompressorSpeedDriver_t *instance,
   CompressorSpeed_t compressorSpeed)
{
   const CompressorSpeeds_t *compressorSpeeds = &instance->_private.compressorParametricData->compressorSpeeds;

   if((compressorSpeed == CompressorSpeed_Off) ||
      (compressorSpeed == CompressorSpeed_SuperLow) ||
      (compressorSpeed == CompressorSpeed_SuperHigh) ||
      (compressorSpeed == CompressorSpeed_Startup))
   {
      switch(compressorSpeed)
      {
         case CompressorSpeed_Startup:
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->startupSpeedFrequencyInHz);
            break;

         case CompressorSpeed_SuperLow:
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->superLowSpeedFrequencyInHz);
            break;

         case CompressorSpeed_SuperHigh:
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->superHighSpeedFrequencyInHz);
            break;

         case CompressorSpeed_Off:
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               off);
            break;
      }
   }
   else if(!compressorSpeeds->coolingModeDependent)
   {
      switch(compressorSpeed)
      {
         case CompressorSpeed_Low:
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeIndependentCompressorSpeeds.lowSpeedFrequencyInHz);
            break;

         case CompressorSpeed_Medium:
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeIndependentCompressorSpeeds.mediumSpeedFrequencyInHz);
            break;

         case CompressorSpeed_High:
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->compressorFrequencyErd,
               &compressorSpeeds->coolingModeIndependentCompressorSpeeds.highSpeedFrequencyInHz);
            break;
      }
   }
   else
   {
      CoolingMode_t coolingMode;
      SetpointZone_t freezerSetpointZone;
      DataModel_Read(instance->_private.dataModel, instance->_private.config->coolingModeErd, &coolingMode);
      DataModel_Read(instance->_private.dataModel, instance->_private.config->freezerSetpointZoneErd, &freezerSetpointZone);

      switch(compressorSpeed)
      {
         case CompressorSpeed_Low:
            UpdateCompressorFrequencyWhenCompressorSpeedIsLow(instance, compressorSpeeds, coolingMode, freezerSetpointZone);
            break;

         case CompressorSpeed_Medium:
            UpdateCompressorFrequencyWhenCompressorSpeedIsMedium(instance, compressorSpeeds, coolingMode, freezerSetpointZone);
            break;

         case CompressorSpeed_High:
            UpdateCompressorFrequencyWhenCompressorSpeedIsHigh(instance, compressorSpeeds, coolingMode);
            break;
      }
   }
}

static void CompressorControllerSpeedUpdated(void *context, const void *args)
{
   CompressorSpeedDriver_t *instance = context;
   const CompressorVotedSpeed_t *compressorVotedSpeed = args;

   if(instance->_private.compressorParametricData->compressorIsSingleSpeed ||
      instance->_private.compressorParametricData->variableSpeedCompressorRelayIsEnabled)
   {
      ControlCompressorRelay(instance, compressorVotedSpeed->speed);
   }
   else
   {
      DataModel_Write(instance->_private.dataModel, instance->_private.config->compressorRelayErd, off);
   }

   if(!instance->_private.compressorParametricData->compressorIsSingleSpeed)
   {
      UpdateCompressorFrequency(instance, compressorVotedSpeed->speed);
   }
}

void CompressorSpeedDriver_Init(
   CompressorSpeedDriver_t *instance,
   I_DataModel_t *dataModel,
   const CompressorSpeedDriverConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.compressorParametricData = PersonalityParametricData_Get(dataModel)->compressorData;

   EventSubscription_Init(
      &instance->_private.compressorSpeedSubscription,
      instance,
      CompressorControllerSpeedUpdated);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->compressorControllerSpeedErd,
      &instance->_private.compressorSpeedSubscription);
}
