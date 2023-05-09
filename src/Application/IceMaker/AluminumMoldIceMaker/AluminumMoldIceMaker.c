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
   Signal_MaxHarvestFixTimeReached,
   Signal_RakeOnTimerExpired,
   Signal_RakeOffTimerExpired,
   Signal_MoldThermistorIsInvalid,
   Signal_MoldThermistorIsValid,
   Signal_RakeCompletedRevolution,
   Signal_MinimumHeaterOnTimeReached,
   Signal_StopFill,
   Signal_TestRequest_Fill,
   Signal_TurnOnRakeMotor,
   Signal_TurnOffRakeMotor,
   Signal_HarvestFaultMaxTimerExpired,
   Signal_TestRequest_Harvest,
   Signal_PauseFill,
   Signal_ResumeFill,
};

static bool State_Global(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Freeze(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_IdleFreeze(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_FillParent(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Fill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_IdleFill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Harvest(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HarvestFix(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HarvestFault(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_ThermistorFault(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Global, HSM_NO_PARENT },
   { State_Freeze, State_Global },
   { State_IdleFreeze, State_Global },
   { State_FillParent, State_Global },
   { State_Fill, State_FillParent },
   { State_IdleFill, State_FillParent },
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

static void VoteForIceMakerWaterValve(AluminumMoldIceMaker_t *instance, WaterValveState_t state, bool care)
{
   WaterValveVotedState_t vote = {
      .state = state,
      .care = care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceMakerWaterValveVoteErd,
      &vote);
}

static void VoteForIsolationWaterValve(AluminumMoldIceMaker_t *instance, WaterValveState_t state, bool care)
{
   WaterValveVotedState_t vote = {
      .state = state,
      .care = care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->isolationWaterValveVoteErd,
      &vote);
}

static void VoteForIceMakerHeater(AluminumMoldIceMaker_t *instance, HeaterState_t state, bool care)
{
   HeaterVotedState_t vote = {
      .state = state,
      .care = care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceMakerHeaterVoteErd,
      &vote);
}

static void VoteForIceMakerMotor(AluminumMoldIceMaker_t *instance, MotorState_t state, bool care)
{
   AluminumMoldIceMakerMotorVotedState_t vote = {
      .state = state,
      .care = care
   };

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

static bool IceMakerIsEnabled(AluminumMoldIceMaker_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceMakerEnabledErd,
      &state);

   return state;
}

static bool SabbathModeIsDisabled(AluminumMoldIceMaker_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathModeErd,
      &state);

   return !state;
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
      instance->_private.config->rakePositionErd,
      &position);

   return position == RakePosition_Home;
}

static void VoteForFillTubeHeater(AluminumMoldIceMaker_t *instance, PercentageDutyCycle_t dutyCycle)
{
   PercentageDutyCycleVote_t vote = {
      .percentageDutyCycle = dutyCycle,
      .care = Vote_Care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fillTubeHeaterVoteErd,
      &vote);
}

static void VoteFillTubeHeaterDontCare(AluminumMoldIceMaker_t *instance)
{
   PercentageDutyCycleVote_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fillTubeHeaterVoteErd,
      &vote);

   vote.care = Vote_DontCare;
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
      instance->_private.iceMakerParametricData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC,
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
   IceMakerMoldHeaterControlRequest_t request = {
      .enable = true,
      .skipInitialOnTime = false,
      .onTemperatureInDegFx100 = instance->_private.iceMakerParametricData->harvestData.heaterOnTemperatureInDegFx100,
      .offTemperatureInDegFx100 = instance->_private.iceMakerParametricData->harvestData.heaterOffTemperatureInDegFx100
   };

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

static void SetSkipFillFlag(AluminumMoldIceMaker_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->skipFillRequestErd,
      set);
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
   return (instance->_private.iceMakerParametricData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage == 0);
}

static bool FillTubeHeaterOnTimeIsZero(AluminumMoldIceMaker_t *instance)
{
   return (instance->_private.iceMakerParametricData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds == 0);
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

static void StartWaterFillMonitoring(AluminumMoldIceMaker_t *instance)
{
   IceMakerWaterFillMonitoringRequest_t request = IceMakerWaterFillMonitoringRequest_Start;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFillMonitoringRequestErd,
      &request);
}

static void StopWaterFillMonitoring(AluminumMoldIceMaker_t *instance)
{
   IceMakerWaterFillMonitoringRequest_t request = IceMakerWaterFillMonitoringRequest_Stop;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFillMonitoringRequestErd,
      &request);
}

static void PauseWaterFillMonitoring(AluminumMoldIceMaker_t *instance)
{
   IceMakerWaterFillMonitoringRequest_t request = IceMakerWaterFillMonitoringRequest_Pause;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFillMonitoringRequestErd,
      &request);
}

static void ResumeWaterFillMonitoring(AluminumMoldIceMaker_t *instance)
{
   IceMakerWaterFillMonitoringRequest_t request = IceMakerWaterFillMonitoringRequest_Resume;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFillMonitoringRequestErd,
      &request);
}

static void SendFreezerIceRateSignal(AluminumMoldIceMaker_t *instance)
{
   Signal_SendViaErd(
      DataModel_AsDataSource(instance->_private.dataModel),
      instance->_private.config->freezerIceRateTriggerSignalErd);
}

static void ClearIceMakerTestRequest(AluminumMoldIceMaker_t *instance)
{
   IceMakerTestRequest_t clearRequest = IceMakerTestRequest_None;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->aluminumMoldIceMakerTestRequestErd,
      &clearRequest);
}

static void MaxHarvestFixTimeReached(void *context)
{
   AluminumMoldIceMaker_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_MaxHarvestFixTimeReached, NULL);
}

static void StartMaxHarvestFixTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxHarvestFixTimer,
      instance->_private.iceMakerParametricData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN,
      MaxHarvestFixTimeReached,
      instance);
}

static void StopMaxHarvestFixTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.maxHarvestFixTimer);
}

static void SetMoldHeaterControlRequestForHarvestFix(AluminumMoldIceMaker_t *instance)
{
   IceMakerMoldHeaterControlRequest_t request = {
      .enable = true,
      .skipInitialOnTime = true,
      .onTemperatureInDegFx100 = instance->_private.iceMakerParametricData->harvestFixData.heaterOnTemperatureInDegFx100,
      .offTemperatureInDegFx100 = instance->_private.iceMakerParametricData->harvestFixData.heaterOffTemperatureInDegFx100
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->moldHeaterControlRequestErd,
      &request);
}

static void HarvestFixRakeOnTimerExpired(void *context)
{
   AluminumMoldIceMaker_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_RakeOnTimerExpired, NULL);
}

static void StartHarvestFixRakeOnTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.rakeMotorControlTimer,
      instance->_private.iceMakerParametricData->harvestFixData.motorOnTimeInSeconds * MSEC_PER_SEC,
      HarvestFixRakeOnTimerExpired,
      instance);
}

static void StopHarvestFixRakeOnAndOffTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.rakeMotorControlTimer);
}

static void HarvestFixRakeOffTimerExpired(void *context)
{
   AluminumMoldIceMaker_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_RakeOffTimerExpired, NULL);
}

static void StartHarvestFixRakeOffTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.rakeMotorControlTimer,
      instance->_private.iceMakerParametricData->harvestFixData.motorOffTimeInSeconds * MSEC_PER_SEC,
      HarvestFixRakeOffTimerExpired,
      instance);
}

static void HarvestFaultMaxTimeExpired(void *context)
{
   AluminumMoldIceMaker_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_HarvestFaultMaxTimerExpired, NULL);
}

static void StartHarvestFaultMaxTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.harvestFaultMaxTimer,
      instance->_private.iceMakerParametricData->harvestFaultData.harvestFaultMaxTimeInMinutes * MSEC_PER_MIN,
      HarvestFaultMaxTimeExpired,
      instance);
}

static void StopHarvestFaultMaxTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.harvestFaultMaxTimer);
}

static void HarvestFaultRakeOnTimerExpired(void *context)
{
   AluminumMoldIceMaker_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_TurnOffRakeMotor, NULL);
}

static void StartHarvestFaultRakeOnTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.rakeMotorControlTimer,
      instance->_private.iceMakerParametricData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC,
      HarvestFaultRakeOnTimerExpired,
      instance);
}

static void HarvestFaultRakeOffTimerExpired(void *context)
{
   AluminumMoldIceMaker_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_TurnOnRakeMotor, NULL);
}

static void StartHarvestFaultRakeOffTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.rakeMotorControlTimer,
      instance->_private.iceMakerParametricData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC,
      HarvestFaultRakeOffTimerExpired,
      instance);
}

static void StopHarvestFaultRakeOnAndOffTimer(AluminumMoldIceMaker_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.rakeMotorControlTimer);
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

      case Signal_MoldThermistorIsInvalid:
         Hsm_Transition(hsm, State_ThermistorFault);
         break;

      case Signal_TestRequest_Fill:
         Hsm_Transition(hsm, State_Fill);
         break;

      case Signal_TestRequest_Harvest:
         Hsm_Transition(hsm, State_Harvest);
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
         VoteForIceMakerWaterValve(instance, OFF, Vote_DontCare);
         VoteForIceMakerHeater(instance, OFF, Vote_DontCare);
         VoteForIceMakerMotor(instance, OFF, Vote_DontCare);
         RequestHarvestCountCalculation(instance);
         RequestFeelerArmMonitoring(instance);

         if(!instance->_private.initialFreezeStateTransition)
         {
            SendFreezerIceRateSignal(instance);
         }
         instance->_private.initialFreezeStateTransition = false;
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
         VoteForIceMakerWaterValve(instance, OFF, Vote_DontCare);
         VoteForIceMakerHeater(instance, OFF, Vote_DontCare);
         VoteForIceMakerMotor(instance, OFF, Vote_DontCare);
         break;

      case Signal_MoldThermistorIsInvalid:
      case Signal_IceMakerIsEnabled:
      case Signal_SabbathModeDisabled:
         if(SabbathModeIsDisabled(instance) && IceMakerIsEnabled(instance))
         {
            if(MoldThermistorIsNotValid(instance))
            {
               Hsm_Transition(hsm, State_ThermistorFault);
            }
            else if(!RakeIsHome(instance))
            {
               Hsm_Transition(hsm, State_Harvest);
            }
            else
            {
               Hsm_Transition(hsm, State_Freeze);
            }
         }
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
         VoteForFillTubeHeater(instance, instance->_private.iceMakerParametricData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage);
         StartMaxHarvestTimer(instance);
         StartFillTubeHeaterTimer(instance);
         StartMinimumHeaterOnTimer(instance);
         SetMoldHeaterControlRequestForHarvest(instance);
         break;

      case Signal_MinimumHeaterOnTimeReached:
         SetRakeControllerRequest(instance);
         break;

      case Signal_MaxHarvestTimeReached:
         if(SabbathModeIsDisabled(instance) && IceMakerIsEnabled(instance))
         {
            if(RakeCompletedRevolution(instance))
            {
               SkipFillFlagIsSet(instance) ? Hsm_Transition(hsm, State_Freeze) : Hsm_Transition(hsm, State_Fill);
            }
            else
            {
               Hsm_Transition(hsm, State_HarvestFix);
            }
         }
         else
         {
            Hsm_Transition(hsm, State_IdleFreeze);
         }
         break;

      case Signal_FillTubeHeaterTimerExpired:
         VoteFillTubeHeaterDontCare(instance);
         if(RakeCompletedRevolution(instance))
         {
            if(SabbathModeIsDisabled(instance) && IceMakerIsEnabled(instance))
            {
               SkipFillFlagIsSet(instance) ? Hsm_Transition(hsm, State_Freeze) : Hsm_Transition(hsm, State_Fill);
            }
            else
            {
               Hsm_Transition(hsm, State_IdleFreeze);
            }
         }
         break;

      case Signal_RakeCompletedRevolution:
         if(FillTubeHeaterTimerHasExpired(instance) ||
            FillTubeHeaterDutyCycleIsZero(instance) ||
            FillTubeHeaterOnTimeIsZero(instance))
         {
            if(SabbathModeIsDisabled(instance) && IceMakerIsEnabled(instance))
            {
               SkipFillFlagIsSet(instance) ? Hsm_Transition(hsm, State_Freeze) : Hsm_Transition(hsm, State_Fill);
            }
            else
            {
               Hsm_Transition(hsm, State_IdleFreeze);
            }
         }
         break;

      case Signal_TestRequest_Harvest:
         break;

      case Hsm_Exit:
         VoteFillTubeHeaterDontCare(instance);
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
         SetSkipFillFlag(instance);
         StartMaxHarvestFixTimer(instance);
         SetMoldHeaterControlRequestForHarvestFix(instance);
         SetRakeControllerRequest(instance);
         StartHarvestFixRakeOnTimer(instance);
         break;

      case Signal_RakeOnTimerExpired:
         ClearRakeControllerRequest(instance);
         StartHarvestFixRakeOffTimer(instance);
         break;

      case Signal_RakeOffTimerExpired:
         SetRakeControllerRequest(instance);
         StartHarvestFixRakeOnTimer(instance);
         break;

      case Signal_MaxHarvestFixTimeReached:
         Hsm_Transition(hsm, State_HarvestFault);
         break;

      case Signal_RakeCompletedRevolution:
         if(!instance->_private.revolutionCompletedDuringHarvestFix)
         {
            instance->_private.revolutionCompletedDuringHarvestFix = true;
         }
         else
         {
            Hsm_Transition(hsm, State_Freeze);
         }
         break;

      case Hsm_Exit:
         instance->_private.revolutionCompletedDuringHarvestFix = false;
         StopMaxHarvestFixTimer(instance);
         StopHarvestFixRakeOnAndOffTimer(instance);
         ClearMoldHeaterControlRequest(instance);
         ClearRakeControllerRequest(instance);
         break;

      case Signal_TestRequest_Harvest:
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
         ClearMoldHeaterControlRequest(instance);
         StartHarvestFaultMaxTimer(instance);
         Hsm_SendSignal(&instance->_private.hsm, Signal_TurnOnRakeMotor, NULL);
         break;

      case Signal_TurnOnRakeMotor:
         SetRakeControllerRequest(instance);
         StartHarvestFaultRakeOnTimer(instance);
         break;

      case Signal_TurnOffRakeMotor:
         ClearRakeControllerRequest(instance);
         StartHarvestFaultRakeOffTimer(instance);
         break;

      case Signal_HarvestFaultMaxTimerExpired:
      case Signal_RakeCompletedRevolution:
         Hsm_Transition(hsm, State_Harvest);
         break;

      case Hsm_Exit:
         StopHarvestFaultMaxTimer(instance);
         StopHarvestFaultRakeOnAndOffTimer(instance);
         ClearRakeControllerRequest(instance);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_FillParent(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Signal_TestRequest_Fill:
         break;

      case Signal_MoldThermistorIsInvalid:
         // Consume signal so it doesn't get deferred to global - transition to thermistor fault state will occur when fill completes
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
         VoteForIceMakerWaterValve(instance, ON, Vote_Care);
         VoteForIsolationWaterValve(instance, ON, Vote_Care);
         if(instance->_private.pauseFillMonitoring)
         {
            ResumeWaterFillMonitoring(instance);
            instance->_private.pauseFillMonitoring = false;
         }
         else
         {
            StartWaterFillMonitoring(instance);
         }
         break;

      case Signal_TestRequest_Fill:
         break;

      case Signal_StopFill:
         if(SabbathModeIsEnabled(instance) || IceMakerIsDisabled(instance))
         {
            Hsm_Transition(hsm, State_IdleFreeze);
         }
         else if(MoldThermistorIsNotValid(instance))
         {
            Hsm_Transition(hsm, State_ThermistorFault);
         }
         else if(!RakeIsHome(instance))
         {
            Hsm_Transition(hsm, State_Harvest);
         }
         else
         {
            Hsm_Transition(hsm, State_Freeze);
         }
         break;

      case Signal_PauseFill:
         instance->_private.pauseFillMonitoring = true;
         Hsm_Transition(hsm, State_IdleFill);
         break;

      case Hsm_Exit:
         if(instance->_private.pauseFillMonitoring)
         {
            PauseWaterFillMonitoring(instance);
         }
         else
         {
            StopWaterFillMonitoring(instance);
         }
         VoteForIceMakerWaterValve(instance, OFF, Vote_DontCare);
         VoteForIsolationWaterValve(instance, OFF, Vote_DontCare);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_IdleFill(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   IGNORE(data);
   AluminumMoldIceMaker_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         UpdateHsmStateTo(instance, AluminumMoldIceMakerHsmState_IdleFill);
         break;

      case Signal_ResumeFill:
         Hsm_Transition(hsm, State_Fill);
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
         VoteForIceMakerWaterValve(instance, OFF, Vote_DontCare);
         VoteForIsolationWaterValve(instance, OFF, Vote_DontCare);
         VoteForIceMakerHeater(instance, OFF, Vote_DontCare);
         VoteForIceMakerMotor(instance, OFF, Vote_DontCare);
         break;

      case Signal_MoldThermistorIsValid:
         if(RakeIsHome(instance))
         {
            Hsm_Transition(hsm, State_Freeze);
         }
         else
         {
            SetSkipFillFlag(instance);
            Hsm_Transition(hsm, State_Harvest);
         }
         break;

      case Signal_TestRequest_Harvest:
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
      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_MoldThermistorIsValid, NULL);
      }
      else
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
   else if(erd == instance->_private.config->stopFillSignalErd)
   {
      Hsm_SendSignal(&instance->_private.hsm, Signal_StopFill, NULL);
   }
   else if(erd == instance->_private.config->aluminumMoldIceMakerTestRequestErd)
   {
      const IceMakerTestRequest_t *request = onChangeData->data;

      if(*request != IceMakerTestRequest_None)
      {
         if(*request == IceMakerTestRequest_Fill)
         {
            Hsm_SendSignal(&instance->_private.hsm, Signal_TestRequest_Fill, NULL);
         }
         else if(*request == IceMakerTestRequest_Harvest)
         {
            Hsm_SendSignal(&instance->_private.hsm, Signal_TestRequest_Harvest, NULL);
         }
         ClearIceMakerTestRequest(instance);
      }
   }
   else if(erd == instance->_private.config->dispensingRequestStatusErd)
   {
      const DispensingRequestStatus_t *dispensingRequestStatus = onChangeData->data;
      switch(dispensingRequestStatus->status)
      {
         case DispenseStatus_Dispensing:
            if((dispensingRequestStatus->selection == DispensingRequestSelection_Water) ||
               (dispensingRequestStatus->selection == DispensingRequestSelection_Autofill))
            {
               Hsm_SendSignal(&instance->_private.hsm, Signal_PauseFill, NULL);
            }
            break;

         default:
            Hsm_SendSignal(&instance->_private.hsm, Signal_ResumeFill, NULL);
            break;
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
      instance->_private.initialFreezeStateTransition = true;
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
   instance->_private.initialFreezeStateTransition = false;
   instance->_private.revolutionCompletedDuringHarvestFix = false;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
