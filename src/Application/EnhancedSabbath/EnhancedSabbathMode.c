/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "EnhancedSabbathMode.h"
#include "utils.h"
#include "SetpointVotedTemperature.h"
#include "CompressorVotedSpeed.h"
#include "BooleanVotedState.h"
#include "DamperVotedPosition.h"
#include "FanSpeed.h"
#include "EnhancedSabbathModeHsmState.h"
#include "Constants_Time.h"
#include "CoolingMode.h"
#include "RampingPwmDutyCyclePercentageVote.h"
#include "RampingPwmDutyCycle.h"
#include "RampingPwmDutyCyclePercentageBundleData.h"

enum
{
   Signal_EnhancedSabbathModeEnabled = Hsm_UserSignalStart,
   Signal_StageTimerExpired,
   Signal_EnhancedSabbathModeDisabled,
   Signal_WaitingToDefrost,
   Signal_SabbathIsReadyToDefrost,
   Signal_Defrosting
};

static bool State_Disabled(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Enabled(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Stage_FreshFood(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Stage_Freezer(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Stage_Defrosting(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Stage_Off(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Disabled, HSM_NO_PARENT },
   { State_Enabled, HSM_NO_PARENT },
   { State_Stage_FreshFood, State_Enabled },
   { State_Stage_Freezer, State_Enabled },
   { State_Stage_Defrosting, State_Enabled },
   { State_Stage_Off, State_Enabled }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static EnhancedSabbathMode_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(EnhancedSabbathMode_t, _private.hsm, hsm);
}

static bool EnhancedSabbathModeIsEnabled(EnhancedSabbathMode_t *instance)
{
   bool status;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->enhancedSabbathModeStatusErd,
      &status);

   return status;
}

static void SetFreshFoodSetpointVoteToDontCare(EnhancedSabbathMode_t *instance)
{
   SetpointVotedTemperature_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodSetpointVoteErd,
      &vote);

   vote.care = Vote_DontCare;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodSetpointVoteErd,
      &vote);
}

static void SetFreezerSetpointVoteToDontCare(EnhancedSabbathMode_t *instance)
{
   SetpointVotedTemperature_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointVoteErd,
      &vote);

   vote.care = Vote_DontCare;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointVoteErd,
      &vote);
}

static void SetCompressorSpeedVoteToDontCare(EnhancedSabbathMode_t *instance)
{
   CompressorVotedSpeed_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedVoteErd,
      &vote);

   vote.care = Vote_DontCare;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedVoteErd,
      &vote);
}

static void SetCompressorSpeedVoteToCareWithValueOf(EnhancedSabbathMode_t *instance, CompressorSpeed_t speed)
{
   CompressorVotedSpeed_t vote = {
      .speed = speed,
      .care = Vote_Care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedVoteErd,
      &vote);
}

static void SetCondenserFanSpeedVoteToDontCare(EnhancedSabbathMode_t *instance)
{
   FanVotedSpeed_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->condenserFanVoteErd,
      &vote);

   vote.care = Vote_DontCare;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->condenserFanVoteErd,
      &vote);
}

static void SetCondenserFanSpeedVoteToCareWithValueOf(EnhancedSabbathMode_t *instance, FanSpeed_t fanSpeed)
{
   FanVotedSpeed_t vote = {
      .speed = fanSpeed,
      .care = Vote_Care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->condenserFanVoteErd,
      &vote);
}

static void SetFreezerEvapFanSpeedVoteToDontCare(EnhancedSabbathMode_t *instance)
{
   FanVotedSpeed_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanVoteErd,
      &vote);

   vote.care = Vote_DontCare;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanVoteErd,
      &vote);
}

static void SetFreezerEvapFanSpeedVoteToCareWithValueOf(EnhancedSabbathMode_t *instance, FanSpeed_t fanSpeed)
{
   FanVotedSpeed_t vote = {
      .speed = fanSpeed,
      .care = Vote_Care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerEvapFanVoteErd,
      &vote);
}

static void SetDamperPositionVoteToDontCare(EnhancedSabbathMode_t *instance)
{
   DamperVotedPosition_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->damperPositionVoteErd,
      &vote);

   vote.care = Vote_DontCare;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->damperPositionVoteErd,
      &vote);
}

static void SetDamperPositionVoteToCareWithValueOf(EnhancedSabbathMode_t *instance, DamperPosition_t position)
{
   DamperVotedPosition_t vote = {
      .position = position,
      .care = Vote_Care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->damperPositionVoteErd,
      &vote);
}

static void SetDisableCompressorMinimumTimesToDontCare(EnhancedSabbathMode_t *instance)
{
   BooleanVotedState_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->disableMinimumCompressorTimesVoteErd,
      &vote);

   vote.care = Vote_DontCare;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->disableMinimumCompressorTimesVoteErd,
      &vote);
}

static void SetDisableCompressorMinimumTimesToCareWithValue(EnhancedSabbathMode_t *instance, bool state)
{
   BooleanVotedState_t vote = {
      .state = state,
      .care = Vote_Care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->disableMinimumCompressorTimesVoteErd,
      &vote);
}

static void SetLightVotesToOffAndDontCare(EnhancedSabbathMode_t *instance)
{
   RampingPwmDutyCyclePercentage_t lightingOffRampingPwmDutyCyclePercentage = {
      .pwmDutyCyclePercentage = 0,
      .rampingUpCountInMsec = UINT8_MAX,
      .rampingDownCountInMsec = UINT8_MAX
   };

   RampingPwmDutyCyclePercentageVote_t vote = {
      .rampingPwmDutyCyclePercentage = lightingOffRampingPwmDutyCyclePercentage,
      .care = Vote_DontCare
   };

   for(uint8_t i = 0; i < instance->_private.config->lightVoteErdList.numberOfErds; i++)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->lightVoteErdList.erds[i],
         &vote);
   }
}

static void SetLightVotesToParametricallyDefinedPwmDutyCyclePercentageAndCare(EnhancedSabbathMode_t *instance)
{
   RampingPwmDutyCyclePercentage_t lightingRampingPwmDutyCyclePercentage = {
      .pwmDutyCyclePercentage = instance->_private.enhancedSabbathData->lightsPwmDutyCyclePercentage,
      .rampingUpCountInMsec = UINT8_MAX,
      .rampingDownCountInMsec = UINT8_MAX
   };

   RampingPwmDutyCyclePercentageVote_t vote = {
      .rampingPwmDutyCyclePercentage = lightingRampingPwmDutyCyclePercentage,
      .care = Vote_Care
   };

   for(uint8_t i = 0; i < instance->_private.config->lightVoteErdList.numberOfErds; i++)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->lightVoteErdList.erds[i],
         &vote);
   }
}

static void SetAllLoadVotesToDontCare(EnhancedSabbathMode_t *instance)
{
   SetCompressorSpeedVoteToDontCare(instance);
   SetCondenserFanSpeedVoteToDontCare(instance);
   SetFreezerEvapFanSpeedVoteToDontCare(instance);
   SetDamperPositionVoteToDontCare(instance);
}

static void SetGridOverrideTo(EnhancedSabbathMode_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->gridOverrideEnabledErd,
      &state);
}

static void SetIceMakerEnabledOverrideRequestTo(EnhancedSabbathMode_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->overrideIceMakerEnabledRequestErd,
      &state);
}

static void SetIceMakerEnabledOverrideValueTo(EnhancedSabbathMode_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->overrideIceMakerEnabledValueErd,
      &state);
}

static void SetDispensingDisabledTo(EnhancedSabbathMode_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dispensingDisabledErd,
      &state);
}

static void SetHsmStateTo(EnhancedSabbathMode_t *instance, EnhancedSabbathModeHsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->hsmStateErd,
      &state);
}

static void SetCoolingModeTo(EnhancedSabbathMode_t *instance, CoolingMode_t coolingMode)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->coolingModeErd,
      &coolingMode);
}

static void SetEnhancedSabbathStatusTo(EnhancedSabbathMode_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->enhancedSabbathModeStatusErd,
      &state);
}

static void SetRegularSabbathStatusTo(EnhancedSabbathMode_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->regularSabbathModeStatusErd,
      &state);
}

static void SetFreshFoodSetpointToCareWithTheValue(EnhancedSabbathMode_t *instance, TemperatureDegFx100_t temperature)
{
   SetpointVotedTemperature_t vote = {
      .temperatureInDegFx100 = temperature,
      .care = Vote_Care,
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodSetpointVoteErd,
      &vote);
}

static void SetFreezerSetpointToCareWithTheValue(EnhancedSabbathMode_t *instance, TemperatureDegFx100_t temperature)
{
   SetpointVotedTemperature_t vote = {
      .temperatureInDegFx100 = temperature,
      .care = Vote_Care,
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerSetpointVoteErd,
      &vote);
}

static TemperatureDegFx100_t FreshFoodAverageCabinetTemperature(EnhancedSabbathMode_t *instance)
{
   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodAverageCabinetTemperatureErd,
      &temperature);

   return temperature;
}

static TemperatureDegFx100_t FreshFoodCabinetSetpoint(EnhancedSabbathMode_t *instance)
{
   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodCabinetSetpointErd,
      &temperature);

   return temperature;
}

static TemperatureDegFx100_t FreezerAverageCabinetTemperature(EnhancedSabbathMode_t *instance)
{
   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerAverageCabinetTemperatureErd,
      &temperature);

   return temperature;
}

static TemperatureDegFx100_t FreezerCabinetSetpoint(EnhancedSabbathMode_t *instance)
{
   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerCabinetSetpointErd,
      &temperature);

   return temperature;
}

static void StageTimerExpired(void *context)
{
   EnhancedSabbathMode_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_StageTimerExpired, NULL);
}

static void MaxEnhancedSabbathTimerExpired(void *context)
{
   EnhancedSabbathMode_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_EnhancedSabbathModeDisabled, NULL);
}

static void StartStageTimerWithTimeSetTo(EnhancedSabbathMode_t *instance, TimerTicks_t timeInMsec)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.stageTimer,
      timeInMsec,
      StageTimerExpired,
      instance);
}

static void StartEnhancedSabbathMaxTimer(EnhancedSabbathMode_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.enhancedSabbathModeEnabledTimer,
      instance->_private.enhancedSabbathData->maxTimeInEnhancedSabbathModeInMinutes * MSEC_PER_MIN,
      MaxEnhancedSabbathTimerExpired,
      instance);
}

static void StopAllTimers(EnhancedSabbathMode_t *instance)
{
   TimerModule_Stop(
      instance->_private.timerModule,
      &instance->_private.stageTimer);
   TimerModule_Stop(
      instance->_private.timerModule,
      &instance->_private.enhancedSabbathModeEnabledTimer);
}

static void VoteCompressorFansAndDamperToOff(EnhancedSabbathMode_t *instance)
{
   SetCompressorSpeedVoteToCareWithValueOf(instance, CompressorSpeed_Off);
   SetCondenserFanSpeedVoteToCareWithValueOf(instance, FanSpeed_Off);
   SetFreezerEvapFanSpeedVoteToCareWithValueOf(instance, FanSpeed_Off);
   SetDamperPositionVoteToCareWithValueOf(instance, DamperPosition_Closed);
}

static void VoteCompressorAndFansToLow(EnhancedSabbathMode_t *instance)
{
   SetCompressorSpeedVoteToCareWithValueOf(instance, CompressorSpeed_Low);
   SetCondenserFanSpeedVoteToCareWithValueOf(instance, FanSpeed_Low);
   SetFreezerEvapFanSpeedVoteToCareWithValueOf(instance, FanSpeed_Low);
}

static bool Defrosting(EnhancedSabbathMode_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostingErd,
      &state);

   return state;
}

static bool WaitingToDefrost(EnhancedSabbathMode_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waitingToDefrostErd,
      &state);

   return state;
}

static void SetEnhancedSabbathIsRequestingDefrostTo(EnhancedSabbathMode_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->enhancedSabbathIsRequestingDefrostErd,
      &state);
}

static void SetEnhancedSabbathStageFreshFoodCoolingIsActiveTo(EnhancedSabbathMode_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->enhancedSabbathStageFreshFoodCoolingIsActiveErd,
      &state);
}

static void SetEnhancedSabbathStageFreezerCoolingIsActiveTo(EnhancedSabbathMode_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->enhancedSabbathStageFreezerCoolingIsActiveErd,
      &state);
}

static bool EnhancedSabbathStageFreezerCoolingIsActive(EnhancedSabbathMode_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->enhancedSabbathStageFreezerCoolingIsActiveErd,
      &state);

   return state;
}

static bool SabbathIsReadyToDefrost(EnhancedSabbathMode_t *instance)
{
   bool sabbathIsReadyToDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathIsReadyToDefrostErd,
      &sabbathIsReadyToDefrost);

   return sabbathIsReadyToDefrost;
}

static bool State_Disabled(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   EnhancedSabbathMode_t *instance = InstanceFromHsm(hsm);
   IGNORE(instance);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetEnhancedSabbathStatusTo(instance, false);
         SetHsmStateTo(instance, EnhancedSabbathModeHsmState_Disabled);
         SetAllLoadVotesToDontCare(instance);
         SetFreshFoodSetpointVoteToDontCare(instance);
         SetFreezerSetpointVoteToDontCare(instance);
         SetDisableCompressorMinimumTimesToDontCare(instance);
         SetGridOverrideTo(instance, false);
         SetDispensingDisabledTo(instance, false);
         SetIceMakerEnabledOverrideRequestTo(instance, false);
         SetIceMakerEnabledOverrideValueTo(instance, true);
         SetLightVotesToOffAndDontCare(instance);
         break;

      case Signal_EnhancedSabbathModeEnabled:
         if(WaitingToDefrost(instance))
         {
            Hsm_Transition(hsm, State_Stage_FreshFood);
         }
         else if(Defrosting(instance))
         {
            Hsm_Transition(hsm, State_Stage_Defrosting);
         }
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Enabled(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   EnhancedSabbathMode_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetFreshFoodSetpointToCareWithTheValue(instance, instance->_private.enhancedSabbathData->freshFoodSetpointTemperatureInDegFx100);
         SetFreezerSetpointToCareWithTheValue(instance, instance->_private.enhancedSabbathData->freezerSetpointTemperatureInDegFx100);
         SetDispensingDisabledTo(instance, true);
         SetGridOverrideTo(instance, true);
         SetIceMakerEnabledOverrideRequestTo(instance, true);
         SetIceMakerEnabledOverrideValueTo(instance, false);
         SetDisableCompressorMinimumTimesToCareWithValue(instance, true);
         StartEnhancedSabbathMaxTimer(instance);
         SetLightVotesToParametricallyDefinedPwmDutyCyclePercentageAndCare(instance);
         break;

      case Signal_EnhancedSabbathModeDisabled:
         SetRegularSabbathStatusTo(instance, false);
         Hsm_Transition(hsm, State_Disabled);
         break;

      case Hsm_Exit:
         StopAllTimers(instance);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Stage_FreshFood(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   EnhancedSabbathMode_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, EnhancedSabbathModeHsmState_Stage_FreshFood);
         StartStageTimerWithTimeSetTo(instance, instance->_private.enhancedSabbathData->freshFoodStageTimeInMinutes * MSEC_PER_MIN);

         SetCoolingModeTo(instance, CoolingMode_FreshFood);
         if(FreshFoodAverageCabinetTemperature(instance) < FreshFoodCabinetSetpoint(instance))
         {
            SetEnhancedSabbathStageFreshFoodCoolingIsActiveTo(instance, false);
            VoteCompressorFansAndDamperToOff(instance);
         }
         else
         {
            SetEnhancedSabbathStageFreshFoodCoolingIsActiveTo(instance, true);
            VoteCompressorAndFansToLow(instance);
            SetDamperPositionVoteToCareWithValueOf(instance, DamperPosition_Open);
         }
         break;

      case Signal_StageTimerExpired:
         Hsm_Transition(hsm, State_Stage_Freezer);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Stage_Freezer(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   EnhancedSabbathMode_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, EnhancedSabbathModeHsmState_Stage_Freezer);

         StartStageTimerWithTimeSetTo(instance, instance->_private.enhancedSabbathData->freezerStageTimeInMinutes * MSEC_PER_MIN);
         SetCoolingModeTo(instance, CoolingMode_Freezer);

         if(FreezerAverageCabinetTemperature(instance) < FreezerCabinetSetpoint(instance))
         {
            SetEnhancedSabbathStageFreezerCoolingIsActiveTo(instance, false);
            VoteCompressorFansAndDamperToOff(instance);

            if(SabbathIsReadyToDefrost(instance))
            {
               SetEnhancedSabbathIsRequestingDefrostTo(instance, true);
            }
         }
         else
         {
            SetEnhancedSabbathStageFreezerCoolingIsActiveTo(instance, true);
            VoteCompressorAndFansToLow(instance);
            SetDamperPositionVoteToCareWithValueOf(instance, DamperPosition_Closed);
         }
         break;

      case Signal_StageTimerExpired:
         if(SabbathIsReadyToDefrost(instance))
         {
            SetEnhancedSabbathIsRequestingDefrostTo(instance, true);
         }
         else
         {
            Hsm_Transition(hsm, State_Stage_Off);
         }
         break;

      case Signal_SabbathIsReadyToDefrost:
         if(!EnhancedSabbathStageFreezerCoolingIsActive(instance))
         {
            SetEnhancedSabbathIsRequestingDefrostTo(instance, true);
         }
         break;

      case Signal_Defrosting:
         Hsm_Transition(hsm, State_Stage_Defrosting);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Stage_Defrosting(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   EnhancedSabbathMode_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, EnhancedSabbathModeHsmState_Stage_Defrosting);
         SetEnhancedSabbathIsRequestingDefrostTo(instance, false);
         break;

      case Signal_WaitingToDefrost:
         Hsm_Transition(hsm, State_Stage_FreshFood);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Stage_Off(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   EnhancedSabbathMode_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, EnhancedSabbathModeHsmState_Stage_Off);
         StartStageTimerWithTimeSetTo(instance, instance->_private.enhancedSabbathData->offStageTimeInMinutes * MSEC_PER_MIN);

         SetCoolingModeTo(instance, CoolingMode_Off);
         VoteCompressorFansAndDamperToOff(instance);
         break;

      case Signal_StageTimerExpired:
         Hsm_Transition(hsm, State_Stage_FreshFood);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static void OnDataModelChange(void *context, const void *_args)
{
   EnhancedSabbathMode_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;
   Erd_t erd = args->erd;

   if(erd == instance->_private.config->waitingToDefrostErd)
   {
      const bool *waitingToDefrost = args->data;
      if(*waitingToDefrost)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_WaitingToDefrost, NULL);
      }
   }
   else if(erd == instance->_private.config->enhancedSabbathModeStatusErd)
   {
      const bool *enabled = args->data;
      if(*enabled)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_EnhancedSabbathModeEnabled, NULL);
      }
      else
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_EnhancedSabbathModeDisabled, NULL);
      }
   }
   else if(erd == instance->_private.config->sabbathIsReadyToDefrostErd)
   {
      const bool *sabbathIsReadyToDefrost = args->data;
      if(*sabbathIsReadyToDefrost)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_SabbathIsReadyToDefrost, NULL);
      }
   }
   else if(erd == instance->_private.config->defrostingErd)
   {
      const bool *defrosting = args->data;
      if(*defrosting)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_Defrosting, NULL);
      }
   }
}

static HsmState_t InitialState(EnhancedSabbathMode_t *instance)
{
   if(EnhancedSabbathModeIsEnabled(instance))
   {
      return State_Stage_FreshFood;
   }
   else
   {
      return State_Disabled;
   }
}

void EnhancedSabbathMode_Init(
   EnhancedSabbathMode_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   const EnhancedSabbathData_t *enhancedSabbathData,
   const EnhancedSabbathModeConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.timerModule = timerModule;
   instance->_private.enhancedSabbathData = enhancedSabbathData;
   instance->_private.config = config;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);
}
