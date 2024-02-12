/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentDamperHeaterVotingFrameworkPlugin.h"
#include "SystemErds.h"

enum
{
   WinningVoteErd = Erd_ConvertibleCompartmentDamperHeater_WinningVoteErd,
   ResolvedVoteErd = Erd_ConvertibleCompartmentDamperHeater_ResolvedVote,
   LastVoteErd = Erd_ConvertibleCompartmentDamperHeater_DefrostHeaterSyncVote,
   PwmDutyCycleOutputErd = Erd_ConvertibleCompartmentDamperHeaterPwmDutyCycle,
};

static const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t dutyCycleToPercentageCalculatorConfig = {
   .inputPercentageDutyCycleVoteErd = ResolvedVoteErd,
   .outputPwmDutyCycleErd = PwmDutyCycleOutputErd,
};

static const PercentageDutyCycleVote_t defaultData = {
   .percentageDutyCycle = PercentageDutyCycle_Min,
   .care = Vote_DontCare,
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const PercentageDutyCycleVote_t *data = votingErdData;
   return data->care;
}

static const ErdResolverConfiguration_t erdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = WinningVoteErd,
   .resolvedStateErd = ResolvedVoteErd,
   .numberOfVotingErds = LastVoteErd - WinningVoteErd,
};

void ConvertibleCompartmentDamperHeaterVotingFrameworkPlugin_Init(
   ConvertibleCompartmentDamperHeaterVotingFrameworkPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.erdResolver,
      DataModel_AsDataSource(dataModel),
      &erdResolverConfiguration);

   PercentageDutyCycleVoteToPwmDutyCycleConverter_Init(
      &instance->_private.dutyCycleToPercentageCalculator,
      dataModel,
      &dutyCycleToPercentageCalculatorConfig);
}
