/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "AluminumMoldIceMakerPlugin.h"
#include "PersonalityParametricData.h"

FreezerIceRateHandlerConfig_t iceRateHandlerConfig = {
   .freezerIceRateTriggerSignal = Erd_FreezerIceRateTriggerSignal,
   .freezerSetpointUserVote = Erd_FreezerSetpoint_UserVote,
   .freezerSetpointFreezerIceRateVote = Erd_FreezerSetpoint_FreezerIceRateVote,
   .freezerEvapFanSpeedFreezerIceRateVote = Erd_FreezerEvapFanSpeed_FreezerIceRateVote,
   .freezerEvapFanSpeedResolvedVote = Erd_FreezerEvapFanSpeed_ResolvedVote,
};

static const SoftPwmConfiguration_t softPwmConfig = {
   .pwmDutyCycleErd = Erd_FillTubeHeaterPwmDutyCycle,
   .gpioErd = Erd_FillTubeHeater,
   .timerModuleErd = Erd_TimerModule,
   .frequencyX100 = 10
};

static const AluminumMoldIceMakerConfig_t aluminumMoldIceMakerConfig = {
   .aluminumMoldIceMakerHsmStateErd = Erd_AluminumMoldIceMakerHsmState
};

static const FeelerArmMonitorConfig_t feelerArmMonitorConfig = {
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .timerModuleErd = Erd_TimerModule,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .feelerArmPositionErd = Erd_FeelerArmPosition
};

void AluminumMoldIceMakerPlugin_Init(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel)
{
   FillTubeHeaterVotingFrameworkPlugin_Init(
      &instance->_private.fillTubeHeaterVotingFrameworkPlugin,
      dataModel);

   FreezerIceRateHandler_Init(
      &instance->_private.iceRatehandler,
      dataModel,
      &iceRateHandlerConfig,
      PersonalityParametricData_Get(dataModel)->freezerIceRateData);

   SoftPwm_Init(
      &instance->_private.fillTubeHeaterSoftPwm,
      dataModel,
      &softPwmConfig);

   FeelerArmMonitor_Init(
      &instance->_private.feelerArmMonitor,
      dataModel,
      &feelerArmMonitorConfig);

   AluminumMoldIceMaker_Init(
      &instance->_private.aluminumMoldIceMaker,
      dataModel,
      &aluminumMoldIceMakerConfig);
}
