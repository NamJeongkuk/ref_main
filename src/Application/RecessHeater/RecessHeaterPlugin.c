/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "RecessHeaterPlugin.h"
#include "SystemErds.h"

static bool RecessHeaterVotingErdCareDelegate(const void *votingErdData)
{
   const WaterValveVotedState_t *data = votingErdData;
   return (data->care);
}

static const PercentageDutyCycleVote_t defaultRecessHeaterData = {
   .percentageDutyCycle = PercentageDutyCycle_Min,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t recessHeaterResolverConfiguration = {
   .votingErdCare = RecessHeaterVotingErdCareDelegate,
   .defaultData = &defaultRecessHeaterData,
   .winningVoterErd = Erd_RecessHeater_WinningVoteErd,
   .resolvedStateErd = Erd_RecessHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_RecessHeater_VariableAntiSweatVote - Erd_RecessHeater_WinningVoteErd)
};

static const VariableSweatHeaterDutyCycleCalculatorConfig_t variableSweatHeaterDutyCycleCalculatorConfiguration = {
   .ambientFilteredTemperatureResolvedInDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .ambientFilteredHumidityResolvedPercentx100Erd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .freshFoodSetpointResolvedVoteErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .freezerSetpointResolvedVoteErd = Erd_FreezerSetpoint_ResolvedVote,
   .recessHeaterVariableAntiSweatVoteErd = Erd_RecessHeater_VariableAntiSweatVote,
   .ambientTemperatureIsValidResolvedErd = Erd_AmbientTemperature_IsValidResolved,
   .ambientHumidityIsValidResolvedErd = Erd_AmbientHumidity_IsValidResolved,
};

static const ResolvedVotePercentageDutyCycleConnectorConfiguration_t connectorConfiguration = {
   .percentageDutyCycleOutputErd = Erd_DispenserRecessHeaterDutyCyclePercent,
   .resolvedPercentageDutyCycleVoteErd = Erd_RecessHeater_ResolvedVote
};

void RecessHeaterPlugin_Init(
   RecessHeaterPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.recessHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &recessHeaterResolverConfiguration);

   VariableSweatHeaterDutyCycleCalculator_Init(
      &instance->_private.variableSweatHeaterDutyCycleCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->recessHeaterData->variableSweatHeaterData,
      &variableSweatHeaterDutyCycleCalculatorConfiguration);

   ResolvedVotePercentageDutyCycleConnector_Init(
      &instance->_private.resolvedVotePercentageDutyCycleConnector,
      dataModel,
      &connectorConfiguration);
}
