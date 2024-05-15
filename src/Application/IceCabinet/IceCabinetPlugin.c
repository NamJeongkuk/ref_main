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
}
