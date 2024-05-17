/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetPlugin.h"
#include "SystemErds.h"
#include "ParametricData.h"
#include "SystemErds.h"

static const IceCabinetFanStartupSpeedVotingConfig_t iceCabinetFanStartupSpeedVotingConfig = {
   .iceCabinetFanResolvedVoteErd = Erd_IceCabinetFanSpeed_ResolvedVote,
   .iceCabinetFanStartupSpeedVoteErd = Erd_IceCabinetFanSpeed_StartupVote
};

static const IceCabinetForcedCoolingFreezerEvaporatorFanVoterConfig_t iceCabinetForcedCoolingFreezerEvaporatorFanVoterConfig = {
   .freezerEvapFanSpeedIceCabinetVoteErd = Erd_FreezerEvapFanSpeed_IceCabinetVote,
   .freezerEvapFanSpeedGridVoteErd = Erd_FreezerEvapFanSpeed_GridVote,
   .iceCabinetCoolingModeErd = Erd_IceCabinetCoolingMode
};

static const FreezerSetpointToIceCabinetSetpointMapperConfig_t freezerSetpointToIceCabinetSetpointMapperConfig = {
   .freezerResolvedSetpoint = Erd_Freezer_ResolvedSetpointInDegFx100,
   .iceCabinetSetpoint = Erd_IceCabinetSetpoint
};

static const IceCabinetFreezerEvapFanDependencyVotingConfiguration_t iceCabinetFreezerEvapFanDependencyVotingConfig = {
   .freezerEvapFanSpeedResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .iceCabinetDependencyOnFreezerFanVoteErd = Erd_IceCabinetFanSpeed_FreezerFanDependencyVote
};

static const IceCabinetGammaCalculatorConfig_t gammaCalculatorConfig = {
   .freezerFilteredTemperatureResolvedInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .iceCabinetTemperatureResolvedInDegFx100Erd = Erd_IceCabinet_FilteredTemperatureResolvedInDegFx100,
   .freezerAdjustedSetpointErd = Erd_Freezer_AdjustedSetpointInDegFx100,
   .iceCabinetAdjustedSetpointErd = Erd_IceCabinet_AdjustedSetpointInDegFx100,
   .gammaErd = Erd_IceCabinetGamma
};

static bool IceCabinetGasketHeaterVotingErdCareDelegate(const void *votingErdData)
{
   const WaterValveVotedState_t *data = votingErdData;
   return (data->care);
}

static const PercentageDutyCycleVote_t defaultIceCabinetGasketHeaterData = {
   .percentageDutyCycle = PercentageDutyCycle_Min,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t iceCabinetGasketHeaterResolverConfiguration = {
   .votingErdCare = IceCabinetGasketHeaterVotingErdCareDelegate,
   .defaultData = &defaultIceCabinetGasketHeaterData,
   .winningVoterErd = Erd_IceCabinetGasketHeater_WinningVoteErd,
   .resolvedStateErd = Erd_IceCabinetGasketHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_IceCabinetGasketHeater_IceCabinetHeaterControlVote - Erd_IceCabinetGasketHeater_WinningVoteErd)
};

static const IceCabinetHeaterControlConfig_t iceCabinetHeaterControlConfig = {
   .iceFormationIsActiveErd = Erd_IceCabinet_IceFormationIsActive,
   .iceCabinetFanSpeedVoteErd = Erd_IceCabinetFanSpeed_ResolvedVote,
   .iceCabinetHeaterVoteErd = Erd_IceCabinetGasketHeater_IceCabinetHeaterControlVote,
   .freezerResolvedSetpointErd = Erd_Freezer_ResolvedSetpointInDegFx100
};

void IceCabinetPlugin_Init(
   IceCabinetPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   IceCabinetFanStartupSpeedVoting_Init(
      &instance->_private.iceCabinetFanStartupSpeedVoting,
      dataModel,
      PersonalityParametricData_Get(dataModel)->iceCabinetData->iceCabinetFanStartupData,
      &iceCabinetFanStartupSpeedVotingConfig);

   IceCabinetForcedCoolingFreezerEvaporatorFanVoter_Init(
      &instance->_private.iceCabinetForcedCoolingFreezerEvaporatorFanVoter,
      dataModel,
      &iceCabinetForcedCoolingFreezerEvaporatorFanVoterConfig);

   FreezerSetpointToIceCabinetSetpointMapper_Init(
      &instance->_private.freezerSetpointToIceCabinetSetpointMapper,
      dataModel,
      &freezerSetpointToIceCabinetSetpointMapperConfig,
      PersonalityParametricData_Get(dataModel)->iceCabinetData->freezerSetpointToIceCabinetSetpointTable);

   IceCabinetFreezerEvapFanDependencyVoting_Init(
      &instance->_private.iceCabinetFreezerEvapFanDependencyVoting,
      dataModel,
      &iceCabinetFreezerEvapFanDependencyVotingConfig);

   IceCabinetGammaCalculator_Init(
      &instance->_private.gammaCalculator,
      dataModel,
      &gammaCalculatorConfig,
      &PersonalityParametricData_Get(dataModel)->freshFoodAndFreezerDeltaGridLines->gridLines[GridDelta_SecondDimension],
      &PersonalityParametricData_Get(dataModel)->iceCabinetDeltaGridLines->gridLines[GridDelta_FirstDimension]);

   IceCabinetGridPlugin_Init(
      &instance->_private.iceCabinetGridPlugin,
      dataModel);

   ErdResolver_Init(
      &instance->_private.iceCabinetGasketHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &iceCabinetGasketHeaterResolverConfiguration);

   IceCabinetHeaterControl_Init(
      &instance->_private.iceCabinetHeaterControl,
      dataModel,
      &iceCabinetHeaterControlConfig,
      PersonalityParametricData_Get(dataModel)->iceCabinetGasketHeaterData);
}
