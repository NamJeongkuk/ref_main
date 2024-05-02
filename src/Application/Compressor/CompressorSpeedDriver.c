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
#include "PwmFrequency.h"

static void SetCompressorFrequency(CompressorSpeedDriver_t *instance, PwmFrequency_t frequency)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorFrequencyErd,
      &frequency);
}

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
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreshFoodWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreshFoodMidFrequencyInHz);
         }
         else
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreshFoodColdFrequencyInHz);
         }
         break;

      case CoolingMode_Freezer:
         if(freezerSetpointZone == SetpointZone_Warm)
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreezerWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreezerMidFrequencyInHz);
         }
         else
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreezerColdFrequencyInHz);
         }
         break;

      case CoolingMode_ConvertibleCompartment:
         if(freezerSetpointZone == SetpointZone_Warm)
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedConvertibleCompartmentWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedConvertibleCompartmentMidFrequencyInHz);
         }
         else
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedConvertibleCompartmentColdFrequencyInHz);
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
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreshFoodWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreshFoodMidFrequencyInHz);
         }
         else
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreshFoodColdFrequencyInHz);
         }
         break;

      case CoolingMode_Freezer:
         if(freezerSetpointZone == SetpointZone_Warm)
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreezerWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreezerMidFrequencyInHz);
         }
         else
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreezerColdFrequencyInHz);
         }
         break;

      case CoolingMode_ConvertibleCompartment:
         if(freezerSetpointZone == SetpointZone_Warm)
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedConvertibleCompartmentWarmFrequencyInHz);
         }
         else if(freezerSetpointZone == SetpointZone_Middle)
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedConvertibleCompartmentMidFrequencyInHz);
         }
         else
         {
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedConvertibleCompartmentColdFrequencyInHz);
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
         SetCompressorFrequency(
            instance,
            compressorSpeeds->coolingModeDependentCompressorSpeeds.highSpeedFreshFoodFrequencyInHz);
         break;

      case CoolingMode_Freezer:
         SetCompressorFrequency(
            instance,
            compressorSpeeds->coolingModeDependentCompressorSpeeds.highSpeedFreezerFrequencyInHz);
         break;

      case CoolingMode_ConvertibleCompartment:
         SetCompressorFrequency(
            instance,
            compressorSpeeds->coolingModeDependentCompressorSpeeds.highSpeedConvertibleCompartmentFrequencyInHz);
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
      (compressorSpeed == CompressorSpeed_SuperHigh))
   {
      switch(compressorSpeed)
      {
         case CompressorSpeed_SuperLow:
            SetCompressorFrequency(
               instance,
               compressorSpeeds->superLowSpeedFrequencyInHz);
            break;

         case CompressorSpeed_SuperHigh:
            SetCompressorFrequency(
               instance,
               compressorSpeeds->superHighSpeedFrequencyInHz);
            break;

         case CompressorSpeed_Off:
            SetCompressorFrequency(
               instance,
               PwmFrequency_Min);
            break;
      }
   }
   else if(compressorSpeed >= CompressorSpeed_MaxNumberOfSpeeds)
   {
      SetCompressorFrequency(
         instance,
         compressorSpeed);
   }
   else if(!compressorSpeeds->coolingModeDependent)
   {
      switch(compressorSpeed)
      {
         case CompressorSpeed_Low:
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeIndependentCompressorSpeeds.lowSpeedFrequencyInHz);
            break;

         case CompressorSpeed_Medium:
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeIndependentCompressorSpeeds.mediumSpeedFrequencyInHz);
            break;

         case CompressorSpeed_High:
            SetCompressorFrequency(
               instance,
               compressorSpeeds->coolingModeIndependentCompressorSpeeds.highSpeedFrequencyInHz);
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

   bool compressorIsOn = (compressorVotedSpeed->speed != CompressorSpeed_Off);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorIsOnErd,
      &compressorIsOn);
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
      instance->_private.config->resolvedVoteCompressorSpeedErd,
      &instance->_private.compressorSpeedSubscription);
}
