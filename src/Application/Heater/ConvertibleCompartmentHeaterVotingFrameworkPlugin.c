/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentHeaterVotingFrameworkPlugin.h"
#include "SystemErds.h"

static const PercentageDutyCycleVote_t defaultData = {
   .percentageDutyCycle = PercentageDutyCycle_Min,
   .care = Vote_DontCare
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return data->care;
}

static const ErdResolverConfiguration_t erdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_ConvertibleCompartmentHeater_WinningVoteErd,
   .resolvedStateErd = Erd_ConvertibleCompartmentHeater_ResolvedVote,
   .numberOfVotingErds = Erd_ConvertibleCompartmentHeater_FactoryVote - Erd_ConvertibleCompartmentHeater_WinningVoteErd
};

static const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t dutyCycleToPercentageCalculatorConfig = {
   .inputPercentageDutyCycleVoteErd = Erd_ConvertibleCompartmentHeater_ResolvedVote,
   .outputPwmDutyCycleErd = Erd_ConvertibleCompartmentHeater_Pwm
};

void ConvertibleCompartmentHeaterVotingFrameworkPlugin_Init(
   ConvertibleCompartmentHeaterVotingFrameworkPlugin_t *instance,
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
