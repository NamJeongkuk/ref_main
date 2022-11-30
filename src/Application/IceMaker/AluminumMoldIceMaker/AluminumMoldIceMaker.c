/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AluminumMoldIceMaker.h"
#include "AluminumMoldIceMakerHsmState.h"
#include "PersonalityParametricData.h"
#include "WaterValveVotedState.h"
#include "HeaterVotedState.h"
#include "MotorVotedState.h"
#include "TemperatureDegFx100.h"
#include "Constants_Binary.h"
#include "Vote.h"
#include "utils.h"

enum
{
   Signal_HarvestCountIsReadyToHarvest = Hsm_UserSignalStart,
   Signal_FeelerArmIsReadyToEnterHarvest,
   Signal_IceMakerTemperatureIsReadyToHarvest,
   Signal_SabbathModeEnabled,
   Signal_SabbathModeDisabled,
   Signal_IceMakerIsEnabled,
   Signal_IceMakerIsDisabled
};

static bool State_Global(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Freeze(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_IdleFreeze(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Harvest(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Fill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HarvestFix(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HarvestFault(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_ThermistorFault(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Global, HSM_NO_PARENT },
   { State_Freeze, State_Global },
   { State_IdleFreeze, State_Global },
   { State_Harvest, State_Global },
   { State_Fill, State_Global },
   { State_HarvestFix, State_Harvest },
   { State_HarvestFault, State_Harvest },
   { State_ThermistorFault, State_Global }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static AluminumMoldIceMaker_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(AluminumMoldIceMaker_t, _private.hsm, hsm);
}

static void UpdateHsmStateTo(AluminumMoldIceMaker_t *instance, AluminumMoldIceMakerHsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->aluminumMoldIceMakerHsmStateErd,
      &state);
}

static void VoteForWaterValve(AluminumMoldIceMaker_t *instance, WaterValveState_t state, bool care)
{
   WaterValveVotedState_t vote;
   vote.state = state;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterValveVoteErd,
      &vote);
}

static void VoteForIceMakerHeater(AluminumMoldIceMaker_t *instance, HeaterState_t state, bool care)
{
   HeaterVotedState_t vote;
   vote.state = state;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceMakerHeaterVoteErd,
      &vote);
}

static void VoteForIceMakerMotor(AluminumMoldIceMaker_t *instance, MotorState_t state, bool care)
{
   AluminumMoldIceMakerMotorState_t vote;
   vote.state = state;
   vote.care = care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceMakerMotorVoteErd,
      &vote);
}

static void RequestHarvestCountCalculation(AluminumMoldIceMaker_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->harvestCountCalculationRequestErd,
      set);
}

static void StopHarvestCountCalculation(AluminumMoldIceMaker_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->harvestCountCalculationRequestErd,
      clear);
}

static void RequestFeelerArmMonitoring(AluminumMoldIceMaker_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->feelerArmMonitoringRequestErd,
      set);
}

static void StopFeelerArmMonitoring(AluminumMoldIceMaker_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->feelerArmMonitoringRequestErd,
      clear);
}

static bool IceMakerTemperatureIsReadyToHarvest(AluminumMoldIceMaker_t *instance)
{
   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceMakerFilteredTemperatureInDegFx100Erd,
      &temperature);

   return (temperature < instance->_private.iceMakerParametricData->freezeData.maximumHarvestTemperatureInDegFx100);
}

static bool FeelerArmIsReadyToEnterHarvest(AluminumMoldIceMaker_t *instance)
{
   bool ready;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->feelerArmIsReadyToEnterHarvestErd,
      &ready);

   return ready;
}

static bool HarvestCountIsReadyToHarvest(AluminumMoldIceMaker_t *instance)
{
   bool ready;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->harvestCountIsReadyToHarvestErd,
      &ready);

   return ready;
}

static bool SabbathModeIsEnabled(AluminumMoldIceMaker_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathModeErd,
      &state);

   return state;
}

static bool IceMakerIsDisabled(AluminumMoldIceMaker_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceMakerEnabledErd,
      &state);

   return !state;
}

static bool State_Global(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_Global);
         break;

      case Signal_SabbathModeEnabled:
      case Signal_IceMakerIsDisabled:
         Hsm_Transition(hsm, State_IdleFreeze);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Freeze(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_Freeze);
         VoteForWaterValve(instance, OFF, Vote_Care);
         VoteForIceMakerHeater(instance, OFF, Vote_Care);
         VoteForIceMakerMotor(instance, OFF, Vote_Care);
         RequestHarvestCountCalculation(instance);
         RequestFeelerArmMonitoring(instance);
         break;

      case Signal_HarvestCountIsReadyToHarvest:
      case Signal_IceMakerTemperatureIsReadyToHarvest:
      case Signal_FeelerArmIsReadyToEnterHarvest:
         if(IceMakerTemperatureIsReadyToHarvest(instance) &&
            FeelerArmIsReadyToEnterHarvest(instance) &&
            HarvestCountIsReadyToHarvest(instance))
         {
            Hsm_Transition(hsm, State_Harvest);
         }
         break;

      case Hsm_Exit:
         StopHarvestCountCalculation(instance);
         StopFeelerArmMonitoring(instance);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_IdleFreeze(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_IdleFreeze);
         break;

      case Signal_SabbathModeDisabled:
      case Signal_IceMakerIsEnabled:
         Hsm_Transition(hsm, State_Freeze);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Harvest(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_Harvest);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Fill(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_Fill);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_HarvestFix(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_HarvestFix);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_HarvestFault(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_HarvestFault);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_ThermistorFault(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_ThermistorFault);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static void DataModelChanged(void *context, const void *args)
{
   AluminumMoldIceMaker_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->harvestCountIsReadyToHarvestErd)
   {
      Hsm_SendSignal(&instance->_private.hsm, Signal_HarvestCountIsReadyToHarvest, NULL);
   }
   else if(erd == instance->_private.config->iceMakerFilteredTemperatureInDegFx100Erd)
   {
      const TemperatureDegFx100_t *temperature = onChangeData->data;
      if(*temperature < instance->_private.iceMakerParametricData->freezeData.maximumHarvestTemperatureInDegFx100)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_IceMakerTemperatureIsReadyToHarvest, NULL);
      }
   }
   else if(erd == instance->_private.config->feelerArmIsReadyToEnterHarvestErd)
   {
      Hsm_SendSignal(&instance->_private.hsm, Signal_FeelerArmIsReadyToEnterHarvest, NULL);
   }
   else if(erd == instance->_private.config->sabbathModeErd)
   {
      const bool *state = onChangeData->data;
      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_SabbathModeEnabled, NULL);
      }
      else
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_SabbathModeDisabled, NULL);
      }
   }
   else if(erd == instance->_private.config->iceMakerEnabledErd)
   {
      const bool *state = onChangeData->data;
      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_IceMakerIsEnabled, NULL);
      }
      else
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_IceMakerIsDisabled, NULL);
      }
   }
}

static HsmState_t InitialState(AluminumMoldIceMaker_t *instance)
{
   if(SabbathModeIsEnabled(instance) || IceMakerIsDisabled(instance))
   {
      return State_IdleFreeze;
   }
   return State_Freeze;
}

void AluminumMoldIceMaker_Init(
   AluminumMoldIceMaker_t *instance,
   I_DataModel_t *dataModel,
   const AluminumMoldIceMakerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.iceMakerParametricData = PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
