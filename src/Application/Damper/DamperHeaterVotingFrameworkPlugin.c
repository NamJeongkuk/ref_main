/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "DamperHeaterVotingFrameworkPlugin.h"

static const PercentageDutyCycleVote_t defaultData = {
   .percentageDutyCycle = PercentageDutyCycle_Min,
   .care = Vote_DontCare,
};

static const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t dutyCycleToPercentageCalculatorConfig = {
   .inputPercentageDutyCycleVoteErd = Erd_DamperHeater_ResolvedVote,
   .outputPwmDutyCycleErd = Erd_DamperHeater_Pwm
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const PercentageDutyCycleVote_t *data = votingErdData;
   return data->care;
}

static const ErdResolverConfiguration_t damperHeaterErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_DamperHeater_WinningVoteErd,
   .resolvedStateErd = Erd_DamperHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_DamperHeater_DefrostHeaterSyncVote - Erd_DamperHeater_WinningVoteErd)
};

void DamperHeaterVotingFrameworkPlugin_Init(DamperHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.damperHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &damperHeaterErdResolverConfiguration);

   PercentageDutyCycleVoteToPwmDutyCycleConverter_Init(
      &instance->_private.dutyCycleToPercentageCalculator,
      dataModel,
      &dutyCycleToPercentageCalculatorConfig);
}
