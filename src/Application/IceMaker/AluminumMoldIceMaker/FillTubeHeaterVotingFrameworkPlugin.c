/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "FillTubeHeaterVotingFrameworkPlugin.h"
#include "PercentageDutyCycleVote.h"

static const PercentageDutyCycleVote_t defaultData = {
   .percentageDutyCycle = 0,
   .care = false
};

static const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t dutyCycleToPercentageCalculatorConfig = {
   .inputPercentageDutyCycleVoteErd = Erd_AluminumMoldIceMakerFillTubeHeater_ResolvedVote,
   .outputPwmDutyCycleErd = Erd_FillTubeHeater_Pwm
};

static const SoftPwmConfiguration_t fillTubeHeaterSoftPwmConfig = {
   .pwmDutyCycleErd = Erd_FillTubeHeater_Pwm,
   .gpioErd = Erd_FillTubeHeater,
   .timerModuleErd = Erd_TimerModule,
   .frequencyX100 = 10
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const PercentageDutyCycleVote_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t fillTubeHeaterErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_AluminumMoldIceMakerFillTubeHeater_WinningVoteErd,
   .resolvedStateErd = Erd_AluminumMoldIceMakerFillTubeHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_AluminumMoldIceMakerFillTubeHeater_ContinuousVote - Erd_AluminumMoldIceMakerFillTubeHeater_WinningVoteErd)
};

void FillTubeHeaterVotingFrameworkPlugin_Init(FillTubeHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.fillTubeHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &fillTubeHeaterErdResolverConfiguration);

   PercentageDutyCycleVoteToPwmDutyCycleConverter_Init(
      &instance->_private.dutyCycleToPercentageCalculator,
      dataModel,
      &dutyCycleToPercentageCalculatorConfig);

   SoftPwm_Init(
      &instance->_private.fillTubeHeaterSoftPwm,
      dataModel,
      &fillTubeHeaterSoftPwmConfig);
}
