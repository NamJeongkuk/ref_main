/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreshFoodEvapFanPlugin.h"
#include "PersonalityParametricData.h"
#include "FanData.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "utils.h"
#include "Constants_Binary.h"

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const FanVotedSpeed_t *data = votingErdData;
   return (data->care);
}

static const FanVotedSpeed_t defaultData = {
   .speed = FanSpeed_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t freshFoodEvapFanSpeedResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodEvapFanSpeed_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodEvapFanSpeed_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodEvapFanSpeed_GridVote - Erd_FreshFoodEvapFanSpeed_WinningVoteErd)
};

static const FanSpeedResolverConfig_t freshFoodEvapConfig = {
   .resolvedFanSpeedVoteErd = Erd_FreshFoodEvapFanSpeed_ResolvedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpointZone,
   .calculatedRequestFanControlErd = Erd_CalculatedFreshFoodEvapFanControl
};

static const FanControllerConfig_t freshFoodEvapFanControllerConfig = {
   .calculatedFanControlErd = Erd_CalculatedFreshFoodEvapFanControl,
   .inputCaptureErd = Erd_FreshFoodEvapFan_InputCaptureTime,
   .fanPwmErd = Erd_FreshFoodEvapFan_Pwm,
   .fanActualRpmErd = Erd_FreshFoodEvapFan_ActualRpm,
};

static const FanFaultHandlerConfiguration_t freshFoodEvapFanFaultHandlerConfig = {
   .calculatedFanControlErd = Erd_CalculatedFreshFoodEvapFanControl,
   .fanActualRpmErd = Erd_FreshFoodEvapFan_ActualRpm,
   .fanPwmErd = Erd_FreshFoodEvapFan_Pwm,
   .fanMissingFeedbackWhenOnFaultErd = Erd_FreshFoodEvapFanMissingFanFeedbackWhenFanIsRunningFault,
   .fanFeedbackPresentWhenFanOffFaultErd = Erd_FreshFoodEvapFanFeedbackPresentWhenFanIsOffFault,
   .fanCannotReachTargetRpmFaultErd = Erd_FreshFoodEvapFanCannotReachTargetRpmFault,
   .timerModuleErd = Erd_TimerModule
};

void FreshFoodEvapFanPlugin_Init(
   FreshFoodEvapFanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const CombinedFanData_t *fanData =
      PersonalityParametricData_Get(dataModel)->fanData;

   ErdResolver_Init(
      &instance->_private.fanSpeedVoteResolver,
      DataModel_AsDataSource(dataModel),
      &freshFoodEvapFanSpeedResolverConfiguration);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver,
      dataModel,
      &fanData->freshFoodEvapFan,
      &freshFoodEvapConfig);

   FanController_Init(
      &instance->_private.fanController,
      dataModel,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &fanData->freshFoodEvapFan,
      &freshFoodEvapFanControllerConfig);

   FanFaultHandler_Init(
      &instance->_private.fanFaultHandler,
      dataModel,
      &freshFoodEvapFanFaultHandlerConfig,
      &fanData->freshFoodEvapFan);
}
