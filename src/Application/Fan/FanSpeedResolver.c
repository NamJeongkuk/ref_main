/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FanSpeedResolver.h"
#include "FanSpeed.h"
#include "FreezerSetpoint.h"
#include "ValvePosition.h"
#include "TemperatureDegFx100.h"
#include "utils.h"

enum
{
   HighAmbientTempDegFx100 = 4500,
};

static bool FanDataIsDependentOnValvePosition(FanSpeedResolver_t *instance)
{
   return instance->_private.fanData->valveDependent;
}

static bool CareAboutSetpoint(FanSpeedResolver_t *instance)
{
   return instance->_private.fanData->dependentData.careAboutSetpoint;
}

static void CalculateIndependentValveSpeed(FanSpeedResolver_t *instance)
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
      case FanSpeed_Min:
         control = instance->_private.fanData->independentData.minSpeed;
         break;

      case FanSpeed_Low:
         control = instance->_private.fanData->independentData.lowSpeed;
         break;

      case FanSpeed_Medium:
         control = instance->_private.fanData->independentData.mediumSpeed;
         break;

      case FanSpeed_High:
         control = instance->_private.fanData->independentData.highSpeed;
         break;

      case FanSpeed_Max:
         control = instance->_private.fanData->independentData.maxSpeed;
         break;

      default:
         break;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->calculatedRequestFanControlErd,
      &control);
}

static FanControl_t FanControlFromAmbientTemperature(FanSpeedResolver_t *instance)
{
   TemperatureDegFx100_t ambientTemp;
   DataModel_Read(instance->_private.dataModel, instance->_private.config->ambientTempErd, &ambientTemp);

   if(ambientTemp >= HighAmbientTempDegFx100)
   {
      return instance->_private.fanData->dependentData.minSpeedHighAmbientTemperature;
   }
   else
   {
      return instance->_private.fanData->dependentData.minSpeed;
   }
}

static FanControl_t FanControlFromSetpointAndValve(FanSpeedResolver_t *instance)
{
   FreezerSetpoint_t freezerSetpoint;
   ValveVotedPosition_t votedValve;

   FanControl_t control = {
      .type = FanControlType_DutyCycle,
      .dutyCycle = 0,
   };

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointErd,
      &freezerSetpoint);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedValvePositionVoteErd,
      &votedValve);

   if(votedValve.position == ValvePosition_A)
   {
      if(freezerSetpoint == FreezerSetpoint_Cold)
      {
         control = instance->_private.fanData->dependentData.careAboutSetpointData.setpointSpeeds.lowSpeedWithColdSetpointValveA;
      }
      else if(freezerSetpoint == FreezerSetpoint_Medium)
      {
         control = instance->_private.fanData->dependentData.careAboutSetpointData.setpointSpeeds.lowSpeedWithMediumSetpointValveA;
      }
      else if(freezerSetpoint == FreezerSetpoint_Warm)
      {
         control = instance->_private.fanData->dependentData.careAboutSetpointData.setpointSpeeds.lowSpeedWithWarmSetpointValveA;
      }
   }
   else if(votedValve.position == ValvePosition_B)
   {
      if(freezerSetpoint == FreezerSetpoint_Cold)
      {
         control = instance->_private.fanData->dependentData.careAboutSetpointData.setpointSpeeds.lowSpeedWithColdSetpointValveB;
      }
      else if(freezerSetpoint == FreezerSetpoint_Medium)
      {
         control = instance->_private.fanData->dependentData.careAboutSetpointData.setpointSpeeds.lowSpeedWithMediumSetpointValveB;
      }
      else if(freezerSetpoint == FreezerSetpoint_Warm)
      {
         control = instance->_private.fanData->dependentData.careAboutSetpointData.setpointSpeeds.lowSpeedWithWarmSetpointValveB;
      }
   }
   else if(votedValve.position == ValvePosition_C)
   {
      if(freezerSetpoint == FreezerSetpoint_Cold)
      {
         control = instance->_private.fanData->dependentData.careAboutSetpointData.setpointSpeeds.lowSpeedWithColdSetpointValveC;
      }
      else if(freezerSetpoint == FreezerSetpoint_Medium)
      {
         control = instance->_private.fanData->dependentData.careAboutSetpointData.setpointSpeeds.lowSpeedWithMediumSetpointValveC;
      }
      else if(freezerSetpoint == FreezerSetpoint_Warm)
      {
         control = instance->_private.fanData->dependentData.careAboutSetpointData.setpointSpeeds.lowSpeedWithWarmSetpointValveC;
      }
   }

   return control;
}

static void CalculateDependentValveSpeed(FanSpeedResolver_t *instance)
{
   FanVotedSpeed_t votedSpeed;
   ValveVotedPosition_t votedValve;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedFanSpeedVoteErd,
      &votedSpeed);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedValvePositionVoteErd,
      &votedValve);

   FanControl_t control = {
      .type = FanControlType_DutyCycle,
      .dutyCycle = 0,
   };

   switch(votedSpeed.speed)
   {
      case FanSpeed_Min:
         control = FanControlFromAmbientTemperature(instance);
         break;

      case FanSpeed_Low:
         if(CareAboutSetpoint(instance))
         {
            control = FanControlFromSetpointAndValve(instance);
         }
         else if(votedValve.position == ValvePosition_A)
         {
            control = instance->_private.fanData->dependentData.careAboutSetpointData.nonSetpointSpeeds.lowSpeedValveA;
         }
         else if(votedValve.position == ValvePosition_B)
         {
            control = instance->_private.fanData->dependentData.careAboutSetpointData.nonSetpointSpeeds.lowSpeedValveB;
         }
         else if(votedValve.position == ValvePosition_C)
         {
            control = instance->_private.fanData->dependentData.careAboutSetpointData.nonSetpointSpeeds.lowSpeedValveC;
         }

         break;

      case FanSpeed_Medium:
         if(votedValve.position == ValvePosition_A)
         {
            control = instance->_private.fanData->dependentData.mediumSpeedValveA;
         }
         else if(votedValve.position == ValvePosition_B)
         {
            control = instance->_private.fanData->dependentData.mediumSpeedValveB;
         }
         else if(votedValve.position == ValvePosition_C)
         {
            control = instance->_private.fanData->dependentData.mediumSpeedValveC;
         }
         break;

      case FanSpeed_High:
         if(votedValve.position == ValvePosition_A)
         {
            control = instance->_private.fanData->dependentData.highSpeedValveA;
         }
         else if(votedValve.position == ValvePosition_B)
         {
            control = instance->_private.fanData->dependentData.highSpeedValveB;
         }
         else if(votedValve.position == ValvePosition_C)
         {
            control = instance->_private.fanData->dependentData.highSpeedValveC;
         }
         break;

      case FanSpeed_Max:
         control = instance->_private.fanData->dependentData.maxSpeed;
         break;

      default:
         break;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->calculatedRequestFanControlErd,
      &control);
}

static void CalculateResolvedFanSpeedBasedOnDependence(FanSpeedResolver_t *instance)
{
   if(FanDataIsDependentOnValvePosition(instance))
   {
      CalculateDependentValveSpeed(instance);
   }
   else
   {
      CalculateIndependentValveSpeed(instance);
   }
}

static void ResolvedFanSpeedVoteChanged(void *context, const void *args)
{
   FanSpeedResolver_t *instance = context;
   IGNORE(args);

   CalculateResolvedFanSpeedBasedOnDependence(instance);
}

void FanSpeedResolver_Init(
   FanSpeedResolver_t *instance,
   I_DataModel_t *dataModel,
   const ValveFanData_t *fanData,
   const FanSpeedResolverConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.fanData = fanData;
   instance->_private.config = config;

   CalculateResolvedFanSpeedBasedOnDependence(instance);

   EventSubscription_Init(
      &instance->_private.resolvedVoteOnChangeSubscription,
      instance,
      ResolvedFanSpeedVoteChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->resolvedFanSpeedVoteErd,
      &instance->_private.resolvedVoteOnChangeSubscription);
}
