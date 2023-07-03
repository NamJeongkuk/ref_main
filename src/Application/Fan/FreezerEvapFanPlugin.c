/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreezerEvapFanPlugin.h"
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

static const ErdResolverConfiguration_t freezerEvapFanSpeedResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreezerEvapFanSpeed_WinningVoteErd,
   .resolvedStateErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .numberOfVotingErds = (Erd_FreezerEvapFanSpeed_GridVote - Erd_FreezerEvapFanSpeed_WinningVoteErd)
};

static const FanSpeedResolverConfig_t freezerEvapConfig = {
   .resolvedFanSpeedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointErd = Erd_FreezerSetpointZone,
   .calculatedRequestFanControlErd = Erd_CalculatedFreezerEvapFanControl,
   .ambientFilteredTemperatureResolvedInDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .ambientFilteredHumidityPercentx100ResolvedErd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .ambientThermistorIsValidErd = Erd_AmbientTemperature_IsValidResolved,
   .ambientHumiditySensorIsValidErd = Erd_AmbientHumidity_IsValidResolved,
   .pullDownIsActiveErd = Erd_SingleEvaporatorPulldownActive,
   .fanAntiSweatBehaviorEnabledErd = Erd_FreezerEvapFanAntiSweatBehaviorEnabledByGrid
};

static const FanControllerConfig_t freezerEvapFanControllerConfig = {
   .calculatedFanControlErd = Erd_CalculatedFreezerEvapFanControl,
   .inputCaptureErd = Erd_FreezerEvapFan_InputCaptureTime,
   .fanPwmErd = Erd_FreezerEvapFan_Pwm,
   .fanActualRpmErd = Erd_FreezerEvapFan_ActualRpm,
};

void FreezerEvapFanPlugin_Init(
   FreezerEvapFanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const CombinedFanData_t *fanData =
      PersonalityParametricData_Get(dataModel)->fanData;

   ErdResolver_Init(
      &instance->_private.fanSpeedVoteResolver,
      DataModel_AsDataSource(dataModel),
      &freezerEvapFanSpeedResolverConfiguration);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver,
      dataModel,
      &fanData->freezerEvapFan,
      &freezerEvapConfig);

   FanController_Init(
      &instance->_private.fanController,
      dataModel,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &fanData->freezerEvapFan,
      &freezerEvapFanControllerConfig);
}
