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
#include "AluminumMoldIceMakerMotorVotedState.h"
#include "TemperatureDegFx100.h"
#include "Constants_Binary.h"
#include "Vote.h"
#include "PercentageDutyCycleVote.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "utils.h"

enum
{
   Signal_HarvestCountIsReadyToHarvest = Hsm_UserSignalStart,
   Signal_FeelerArmIsReadyToEnterHarvest,
   Signal_IceMakerTemperatureIsReadyToHarvest,
   Signal_SabbathModeEnabled,
   Signal_SabbathModeDisabled,
   Signal_IceMakerIsEnabled,
   Signal_IceMakerIsDisabled,
   Signal_FillTubeHeaterTimerExpired,
   Signal_MaxHarvestTimeReached,
   Signal_MoldThermistorIsInvalid,
   Signal_RakeCompletedRevolution,
   Signal_MinimumHeaterOnTimeReached
};

static bool State_Global(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Freeze(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_IdleFreeze(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Fill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Harvest(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HarvestFix(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HarvestFault(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_ThermistorFault(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Global, HSM_NO_PARENT },
   { State_Freeze, State_Global },
   { State_IdleFreeze, State_Global },
   { State_Fill, State_Global },
   { State_Harvest, State_Global },
   { State_HarvestFix, State_Global },
   { State_HarvestFault, State_Global },
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
   AluminumMoldIceMakerMotorVotedState_t vote;
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

static bool MoldThermistorIsNotValid(AluminumMoldIceMaker_t *instance)
{
   bool valid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->moldThermistorIsValidErd,
      &valid);

   return !valid;
}

static bool RakeIsHome(AluminumMoldIceMaker_t *instance)
{
   RakePosition_t position;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rakePosition,
      &position);

   return position == RakePosition_Home;
}

static void VoteForFillTubeHeater(AluminumMoldIceMaker_t *instance, PercentageDutyCycle_t dutyCycle)
{
   PercentageDutyCycleVote_t vote;
   vote.percentageDutyCycle = dutyCycle;
   vote.care = Vote_Care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fillTubeHeaterVoteErd,
      &vote);
}

static void VoteFillTubeHeaterOff(AluminumMoldIceMaker_t *instance)
{
   PercentageDutyCycleVote_t vote;
   vote.percentageDutyCycle = PercentageDutyCycle_Min;
   vote.care = Vote_Care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fillTubeHeaterVoteErd,
      &vote);
}

static void FillTubeHeaterTimerExpired(void *context)
{
   AluminumMoldIceMaker_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_FillTubeHeaterTimerExpired, NULL);
}

static void StartFillTubeHeaterTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.fillTubeHeaterTimer,
      instance->_private.iceMakerParametricData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC,
      FillTubeHeaterTimerExpired,
      instance);
}

static void StopFillTubeHeaterTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.fillTubeHeaterTimer);
}

static void MaxHarvestTimeReached(void *context)
{
   AluminumMoldIceMaker_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_MaxHarvestTimeReached, NULL);
}

static void StartMaxHarvestTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxHarvestTimer,
      instance->_private.iceMakerParametricData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN,
      MaxHarvestTimeReached,
      instance);
}

static void StopMaxHarvestTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxHarvestTimer);
}

static void SetMoldHeaterControlRequestForHarvest(AluminumMoldIceMaker_t *instance)
{
   IceMakerMoldHeaterControlRequest_t request;
   request.enable = true;
   request.skipInitialOnTime = false;
   request.onTemperatureInDegFx100 = instance->_private.iceMakerParametricData->harvestData.heaterOnTemperatureInDegFx100;
   request.offTemperatureInDegFx100 = instance->_private.iceMakerParametricData->harvestData.heaterOffTemperatureInDegFx100;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->moldHeaterControlRequestErd,
      &request);
}

static void ClearMoldHeaterControlRequest(AluminumMoldIceMaker_t *instance)
{
   IceMakerMoldHeaterControlRequest_t request;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->moldHeaterControlRequestErd,
      &request);

   request.enable = false;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->moldHeaterControlRequestErd,
      &request);
}

static bool RakeCompletedRevolution(AluminumMoldIceMaker_t *instance)
{
   bool completeRevolution;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rakeCompletedRevolutionErd,
      &completeRevolution);

   return completeRevolution;
}

static bool FillTubeHeaterTimerHasExpired(AluminumMoldIceMaker_t *instance)
{
   return !TimerModule_IsRunning(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.fillTubeHeaterTimer);
}

static bool SkipFillFlagIsSet(AluminumMoldIceMaker_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->skipFillRequestErd,
      &state);

   return state;
}

static void ClearSkipFillFlag(AluminumMoldIceMaker_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->skipFillRequestErd,
      clear);
}

static bool FillTubeHeaterDutyCycleIsZero(AluminumMoldIceMaker_t *instance)
{
   return (instance->_private.iceMakerParametricData->harvestData.freezeThawFillTubeHeaterDutyCyclePercentage == 0);
}

static bool FillTubeHeaterOnTimeIsZero(AluminumMoldIceMaker_t *instance)
{
   return (instance->_private.iceMakerParametricData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds == 0);
}

static void MinimumHeaterOnTimeReached(void *context)
{
   AluminumMoldIceMaker_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_MinimumHeaterOnTimeReached, NULL);
}

static void StartMinimumHeaterOnTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.minimumHeaterOnTimer,
      instance->_private.iceMakerParametricData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC,
      MinimumHeaterOnTimeReached,
      instance);
}

static void SetRakeControllerRequest(AluminumMoldIceMaker_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rakeControlRequestErd,
      set);
}

static void ClearRakeControllerRequest(AluminumMoldIceMaker_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->rakeControlRequestErd,
      clear);
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

      case Hsm_Exit:
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

      case Hsm_Exit:
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

      case Hsm_Exit:
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
         VoteForFillTubeHeater(instance, instance->_private.iceMakerParametricData->harvestData.freezeThawFillTubeHeaterDutyCyclePercentage);
         StartMaxHarvestTimer(instance);
         StartFillTubeHeaterTimer(instance);
         StartMinimumHeaterOnTimer(instance);
         SetMoldHeaterControlRequestForHarvest(instance);
         break;

      case Signal_MinimumHeaterOnTimeReached:
         SetRakeControllerRequest(instance);
         break;

      case Signal_MaxHarvestTimeReached:
         if(RakeCompletedRevolution(instance))
         {
            SkipFillFlagIsSet(instance) ? Hsm_Transition(hsm, State_Freeze) : Hsm_Transition(hsm, State_Fill);
         }
         else
         {
            Hsm_Transition(hsm, State_HarvestFix);
         }
         break;

      case Signal_FillTubeHeaterTimerExpired:
         VoteFillTubeHeaterOff(instance);
         if(RakeCompletedRevolution(instance))
         {
            SkipFillFlagIsSet(instance) ? Hsm_Transition(hsm, State_Freeze) : Hsm_Transition(hsm, State_Fill);
         }
         break;

      case Signal_RakeCompletedRevolution:
         ClearRakeControllerRequest(instance);

         if(FillTubeHeaterTimerHasExpired(instance) ||
            FillTubeHeaterDutyCycleIsZero(instance) ||
            FillTubeHeaterOnTimeIsZero(instance))
         {
            SkipFillFlagIsSet(instance) ? Hsm_Transition(hsm, State_Freeze) : Hsm_Transition(hsm, State_Fill);
         }
         break;

      case Signal_MoldThermistorIsInvalid:
         Hsm_Transition(hsm, State_ThermistorFault);
         break;

      case Hsm_Exit:
         VoteFillTubeHeaterOff(instance);
         StopFillTubeHeaterTimer(instance);
         StopMaxHarvestTimer(instance);
         ClearMoldHeaterControlRequest(instance);
         ClearSkipFillFlag(instance);
         ClearRakeControllerRequest(instance);
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

      case Signal_MoldThermistorIsInvalid:
         Hsm_Transition(hsm, State_ThermistorFault);
         break;

      case Hsm_Exit:
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

      case Hsm_Exit:
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

      case Hsm_Exit:
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
   else if(erd == instance->_private.config->moldThermistorIsValidErd)
   {
      const bool *state = onChangeData->data;
      if(!*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_MoldThermistorIsInvalid, NULL);
      }
   }
   else if(erd == instance->_private.config->rakeCompletedRevolutionErd)
   {
      const bool *state = onChangeData->data;
      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_RakeCompletedRevolution, NULL);
      }
   }
}

static HsmState_t InitialState(AluminumMoldIceMaker_t *instance)
{
   if(SabbathModeIsEnabled(instance) || IceMakerIsDisabled(instance))
   {
      return State_IdleFreeze;
   }
   else if(MoldThermistorIsNotValid(instance))
   {
      return State_ThermistorFault;
   }
   else if(RakeIsHome(instance))
   {
      return State_Freeze;
   }
   else
   {
      return State_Harvest;
   }
}

void AluminumMoldIceMaker_Init(
   AluminumMoldIceMaker_t *instance,
   I_DataModel_t *dataModel,
   const AluminumMoldIceMakerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.iceMakerParametricData = PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
