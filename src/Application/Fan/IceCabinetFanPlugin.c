/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetFanPlugin.h"
#include "DataModelErdPointerAccess.h"
#include "FanSpeed.h"
#include "SystemErds.h"

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const FanVotedSpeed_t *data = votingErdData;
   return (data->care);
}

static const FanVotedSpeed_t defaultData = {
   .speed = FanSpeed_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t iceCabinetFanSpeedResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_IceCabinetFanSpeed_WinningVoteErd,
   .resolvedStateErd = Erd_IceCabinetFanSpeed_ResolvedVote,
   .numberOfVotingErds = (Erd_IceCabinetFanSpeed_IceCabinetGridVote - Erd_IceCabinetFanSpeed_WinningVoteErd)
};

static const FanSpeedResolverConfig_t fanSpeedResolverConfig = {
   .resolvedFanSpeedVoteErd = Erd_IceCabinetFanSpeed_ResolvedVoteWithSabbathDelay,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpointZone,
   .calculatedRequestFanControlErd = Erd_CalculatedIceCabinetFanControl
};

static const FanControllerConfig_t fanControllerConfig = {
   .calculatedFanControlErd = Erd_CalculatedIceCabinetFanControl,
   .inputCaptureErd = Erd_IceCabinetFan_InputCaptureTime,
   .fanPwmErd = Erd_IceCabinetFan_Pwm,
   .fanActualRpmErd = Erd_IceCabinetFan_ActualRpm,
};

static const FanFaultHandlerConfiguration_t fanFaultHandlerConfig = {
   .calculatedFanControlErd = Erd_CalculatedIceCabinetFanControl,
   .fanActualRpmErd = Erd_IceCabinetFan_ActualRpm,
   .fanPwmErd = Erd_IceCabinetFan_Pwm,
   .fanMissingFeedbackWhenOnFaultErd = Erd_IceCabinetFanMissingFanFeedbackWhenFanIsRunningFault,
   .fanFeedbackPresentWhenFanOffFaultErd = Erd_IceCabinetFanFeedbackPresentWhenFanIsOffFault,
   .fanCannotReachTargetRpmFaultErd = Erd_IceCabinetFanCannotReachTargetRpmFault,
   .timerModuleErd = Erd_TimerModule
};

static const SabbathDelayHandlerConfiguration_t sabbathDelayHandlerConfig = {
   .compressorResolvedVote = Erd_CompressorSpeed_ResolvedVote,
   .loadResolvedVote = Erd_IceCabinetFanSpeed_ResolvedVote,
   .loadResolvedVoteWithSabbathDelay = Erd_IceCabinetFanSpeed_ResolvedVoteWithSabbathDelay
};

void IceCabinetFanPlugin_Init(
   IceCabinetFanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const CombinedFanData_t *fanData =
      PersonalityParametricData_Get(dataModel)->fanData;

   ErdResolver_Init(
      &instance->_private.fanSpeedVoteResolver,
      DataModel_AsDataSource(dataModel),
      &iceCabinetFanSpeedResolverConfiguration);

   SabbathDelayHandler_Init(
      &instance->_private.sabbathDelayHandler,
      dataModel,
      &sabbathDelayHandlerConfig);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver,
      dataModel,
      &fanData->iceCabinetFan,
      &fanSpeedResolverConfig);

   FanController_Init(
      &instance->_private.fanController,
      dataModel,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &fanData->iceCabinetFan,
      &fanControllerConfig);

   FanFaultHandler_Init(
      &instance->_private.fanFaultHandler,
      dataModel,
      &fanFaultHandlerConfig,
      &fanData->iceCabinetFan);
}
