/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FanPlugin.h"
#include "PersonalityParametricData.h"
#include "FanData.h"
#include "FanSpeedVoteResolverPlugin.h"
#include "SystemErds.h"
#include "utils.h"

static const FanSpeedResolverConfig_t freezerEvapConfig = {
   .resolvedFanSpeedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .resolvedValvePositionVoteErd = Erd_ValvePosition_ResolvedVote,
   .freezerSetpointErd = Erd_EnumeratedFreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedFreezerEvapFanSpeed,
   .ambientTempErd = Erd_Ambient_FilteredTemperature,
};

static const FanSpeedResolverConfig_t freezerConfig = {
   .resolvedFanSpeedVoteErd = Erd_FreezerFanSpeed_ResolvedVote,
   .resolvedValvePositionVoteErd = Erd_ValvePosition_ResolvedVote,
   .freezerSetpointErd = Erd_EnumeratedFreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedFreezerFanSpeed,
   .ambientTempErd = Erd_Ambient_FilteredTemperature,
};

static const FanSpeedResolverConfig_t condenserConfig = {
   .resolvedFanSpeedVoteErd = Erd_CondenserFanSpeed_ResolvedVote,
   .resolvedValvePositionVoteErd = Erd_ValvePosition_ResolvedVote,
   .freezerSetpointErd = Erd_EnumeratedFreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedCondenserFanSpeed,
   .ambientTempErd = Erd_Ambient_FilteredTemperature,
};

static const FanSpeedResolverConfig_t freshFoodEvapConfig = {
   .resolvedFanSpeedVoteErd = Erd_FreshFoodEvapFanSpeed_ResolvedVote,
   .resolvedValvePositionVoteErd = Erd_ValvePosition_ResolvedVote,
   .freezerSetpointErd = Erd_EnumeratedFreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedFreshFoodEvapFanSpeed,
   .ambientTempErd = Erd_Ambient_FilteredTemperature,
};

static const FanSpeedResolverConfig_t freshFoodConfig = {
   .resolvedFanSpeedVoteErd = Erd_FreshFoodFanSpeed_ResolvedVote,
   .resolvedValvePositionVoteErd = Erd_ValvePosition_ResolvedVote,
   .freezerSetpointErd = Erd_EnumeratedFreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedFreshFoodFanSpeed,
   .ambientTempErd = Erd_Ambient_FilteredTemperature,
};

static const FanSpeedResolverConfig_t iceCabinetConfig = {
   .resolvedFanSpeedVoteErd = Erd_IceCabinetFanSpeed_ResolvedVote,
   .resolvedValvePositionVoteErd = Erd_ValvePosition_ResolvedVote,
   .freezerSetpointErd = Erd_EnumeratedFreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedIceCabinetFanSpeed,
   .ambientTempErd = Erd_Ambient_FilteredTemperature,
};

static const FanSpeedResolverConfig_t convertibleCompartmentConfig = {
   .resolvedFanSpeedVoteErd = Erd_ConvertibleCompartmentFanSpeed_ResolvedVote,
   .resolvedValvePositionVoteErd = Erd_ValvePosition_ResolvedVote,
   .freezerSetpointErd = Erd_EnumeratedFreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedConvertibleCompartmentFanSpeed,
   .ambientTempErd = Erd_Ambient_FilteredTemperature,
};

static const FanSpeedResolverConfig_t deliConfig = {
   .resolvedFanSpeedVoteErd = Erd_DeliFanSpeed_ResolvedVote,
   .resolvedValvePositionVoteErd = Erd_ValvePosition_ResolvedVote,
   .freezerSetpointErd = Erd_EnumeratedFreezerSetpoint,
   .calculatedRequestFanControlErd = Erd_CalculatedDeliFanSpeed,
   .ambientTempErd = Erd_Ambient_FilteredTemperature,
};

static void InitializeFanSpeedResolvers(FanPlugin_t *instance, I_DataModel_t *dataModel)
{
   const FanData_t *fanData = PersonalityParametricData_Get(dataModel)->fanData;

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver[FanId_Freezer],
      dataModel,
      &fanData->freezerFan,
      &freezerConfig);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver[FanId_Condenser],
      dataModel,
      &fanData->condenserFan,
      &condenserConfig);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver[FanId_FreshFood],
      dataModel,
      &fanData->freshFoodFan,
      &freshFoodConfig);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver[FanId_IceCabinet],
      dataModel,
      &fanData->iceCabinetFan,
      &iceCabinetConfig);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver[FanId_ConvertibleCompartment],
      dataModel,
      &fanData->convertibleCompartmentFan,
      &convertibleCompartmentConfig);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver[FanId_Deli],
      dataModel,
      &fanData->deliPanFan,
      &deliConfig);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver[FanId_FreezerEvap],
      dataModel,
      &fanData->freezerEvapFan,
      &freezerEvapConfig);

   FanSpeedResolver_Init(
      &instance->_private.fanSpeedResolver[FanId_FreshFoodEvap],
      dataModel,
      &fanData->freshFoodEvapFan,
      &freshFoodEvapConfig);
}

void FanPlugin_Init(
   FanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   FanSpeedVoteResolverPlugin_Init(dataModel);
   InitializeFanSpeedResolvers(instance, dataModel);
}
