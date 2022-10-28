/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "AluminumMoldIceMakerPlugin.h"

IceRateHandlerConfig_t iceRateHandlerConfig = {
   .iceRateTriggerSignal = Erd_IceRateTriggerSignal,
   .freezerSetpointUserVote = Erd_FreezerSetpoint_UserVote,
   .freezerSetpointIceRateVote = Erd_FreezerSetpoint_IceRateVote,
   .freezerEvapFanSpeedIceRateVote = Erd_FreezerEvapFanSpeed_IceRateVote,
   .freezerEvapFanSpeedResolvedVote = Erd_FreezerEvapFanSpeed_ResolvedVote,
};

static const SoftPwmConfiguration_t softPwmConfig = {
   .pwmDutyCycleErd = Erd_FillTubeHeaterPwmDutyCycle,
   .gpioErd = Erd_FillTubeHeater,
   .timerModuleErd = Erd_TimerModule,
   .frequencyX100 = 10
};

void AluminumMoldIceMakerPlugin_Init(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel)
{
   FillTubeHeaterVotingFrameworkPlugin_Init(&instance->_private.fillTubeHeaterVotingFrameworkPlugin, dataModel);
   IceRateHandler_Init(&instance->_private.iceRatehandler, dataModel, &iceRateHandlerConfig);
   SoftPwm_Init(&instance->_private.fillTubeHeaterSoftPwm, dataModel, &softPwmConfig);
}
