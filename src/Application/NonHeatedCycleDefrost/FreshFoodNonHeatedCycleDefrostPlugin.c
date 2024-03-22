/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreshFoodNonHeatedCycleDefrostPlugin.h"

static const FreshFoodNonHeatedCycleDefrostConfig_t freshFoodNonHeatedCycleDefrostConfig = {
   .freshFoodEvapFanSpeedNonHeatedCycleDefrostVoteErd = Erd_FreshFoodEvapFanSpeed_NonHeatedCycleDefrostVote,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .defrostStateErd = Erd_DefrostState,
   .freshFoodSetpointResolvedVoteErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .sealedSystemValvePreviousPositionErd = Erd_SealedSystemValvePreviousPosition,
   .sealedSystemValveCurrentPositionErd = Erd_SealedSystemValveCurrentPosition
};

void FreshFoodNonHeatedCycleDefrostPlugin_Init(
   FreshFoodNonHeatedCycleDefrostPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   FreshFoodNonHeatedCycleDefrost_Init(
      &instance->_private.freshFoodNonHeatedCycleDefrost,
      dataModel,
      &freshFoodNonHeatedCycleDefrostConfig,
      PersonalityParametricData_Get(dataModel)->freshFoodNonHeatedCycleDefrost,
      PersonalityParametricData_Get(dataModel)->setpointData->userSetpointData->freshFoodUserSetpoint);
}
