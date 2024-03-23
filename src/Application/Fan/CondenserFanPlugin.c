/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CondenserFanPlugin.h"
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

static const ErdResolverConfiguration_t condenserFanSpeedResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_CondenserFanSpeed_WinningVoteErd,
   .resolvedStateErd = Erd_CondenserFanSpeed_ResolvedVote,
   .numberOfVotingErds = (Erd_CondenserFanSpeed_GridVote - Erd_CondenserFanSpeed_WinningVoteErd)
};

static const FanSpeedResolverConfig_t condenserConfig = {
   .resolvedFanSpeedVoteErd = Erd_CondenserFanSpeed_ResolvedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpointZone,
   .calculatedRequestFanControlErd = Erd_CalculatedCondenserFanControl,
   .ambientFilteredTemperatureResolvedInDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .ambientFilteredHumidityPercentx100ResolvedErd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .ambientThermistorIsValidErd = Erd_AmbientTemperature_IsValidResolved,
   .ambientHumiditySensorIsValidErd = Erd_AmbientHumidity_IsValidResolved,
   .fanAntiSweatBehaviorEnabledErd = Erd_CondenserFanAntiSweatBehaviorEnabledByGrid
};

static const FanControllerConfig_t condenserFanControllerConfig = {
   .calculatedFanControlErd = Erd_CalculatedCondenserFanControl,
   .inputCaptureErd = Erd_CondenserFan_InputCaptureTime,
   .fanPwmErd = Erd_CondenserFan_Pwm,
   .fanActualRpmErd = Erd_CondenserFan_ActualRpm,
};

static const FanFaultHandlerConfiguration_t condenserFanFaultHandlerConfig = {
   .calculatedFanControlErd = Erd_CalculatedCondenserFanControl,
   .fanActualRpmErd = Erd_CondenserFan_ActualRpm,
   .fanPwmErd = Erd_CondenserFan_Pwm,
   .fanMissingFeedbackWhenOnFaultErd = Erd_CondenserFanMissingFanFeedbackWhenFanIsRunningFault,
   .fanFeedbackPresentWhenFanOffFaultErd = Erd_CondenserFanFeedbackPresentWhenFanIsOffFault,
   .fanCannotReachTargetRpmFaultErd = Erd_CondenserFanCannotReachTargetRpmFault,
   .timerModuleErd = Erd_TimerModule
};

void CondenserFanPlugin_Init(
   CondenserFanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const CombinedFanData_t *fanData =
      PersonalityParametricData_Get(dataModel)->fanData;

   ErdResolver_Init(
      &instance->_private.fanSpeedVoteResolver,
      DataModel_AsDataSource(dataModel),
      &condenserFanSpeedResolverConfiguration);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver,
      dataModel,
      &fanData->condenserFan,
      &condenserConfig);

   FanController_Init(
      &instance->_private.fanController,
      dataModel,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &fanData->condenserFan,
      &condenserFanControllerConfig);

   FanFaultHandler_Init(
      &instance->_private.fanFaultHandler,
      dataModel,
      &condenserFanFaultHandlerConfig,
      &fanData->condenserFan);
}
