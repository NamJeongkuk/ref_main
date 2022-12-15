/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "FreshFoodDamperHeaterVotingFrameworkPlugin.h"

static const HeaterVotedState_t defaultData = {
   .state = HeaterState_Off,
   .care = Vote_DontCare
};

static const SoftPwmConfiguration_t damperHeaterSoftPwmConfig = {
   .pwmDutyCycleErd = Erd_FreshFoodDamperHeaterPwmDutyCycle,
   .gpioErd = Erd_FreshFoodDamperHeater,
   .timerModuleErd = Erd_TimerModule,
   .frequencyX100 = 10
};

static const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t dutyCycleToPercentageCalculatorConfig = {
   .inputPercentageDutyCycleVoteErd = Erd_FreshFoodDamperHeater_ResolvedVote,
   .outputPwmDutyCycleErd = Erd_FreshFoodDamperHeaterPwmDutyCycle
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t freshFoodDamperHeaterErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodDamperHeater_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodDamperHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote - Erd_FreshFoodDamperHeater_WinningVoteErd)
};

void FreshFoodDamperHeaterVotingFrameworkPlugin_Init(FreshFoodDamperHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.freshFoodDamperHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &freshFoodDamperHeaterErdResolverConfiguration);

   PercentageDutyCycleVoteToPwmDutyCycleConverter_Init(
      &instance->_private.dutyCycleToPercentageCalculator,
      dataModel,
      &dutyCycleToPercentageCalculatorConfig);

   SoftPwm_Init(
      &instance->_private.damperHeaterSoftPwm,
      dataModel,
      &damperHeaterSoftPwmConfig);
}
