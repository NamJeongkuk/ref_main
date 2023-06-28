/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Constants_Binary.h"
#include "SideBySideTurboModeSetpointPlugin.h"
#include "SystemErds.h"
#include "TurboModeSetpointData.h"

static const SetpointTurboModeConfig_t freshFoodSetpointTurboCoolConfiguration = {
   .currentSetpointResolvedVoteErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .turboModeSetpointVoteErd = Erd_FreshFoodSetpoint_TurboCoolVote,
   .turboModeOnOffStatusErd = Erd_TurboCoolOnOffStatus,
   .turboModeOnTimeInMinutesErd = Erd_TurboCoolOnTimeInMinutes,
   .timerModuleErd = Erd_TimerModule,
};

static const SetpointTurboModeConfig_t freezerSetpointTurboFreezeConfiguration = {
   .currentSetpointResolvedVoteErd = Erd_FreezerSetpoint_ResolvedVote,
   .turboModeSetpointVoteErd = Erd_FreezerSetpoint_TurboFreezeVote,
   .turboModeOnOffStatusErd = Erd_TurboFreezeOnOffStatus,
   .turboModeOnTimeInMinutesErd = Erd_TurboFreezeOnTimeInMinutes,
   .timerModuleErd = Erd_TimerModule,
};

void SideBySideTurboModeSetpointPlugin_Init(
   SideBySideTurboModeSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const TurboModeSetpointData_t *turboModeSetpointData =
      PersonalityParametricData_Get(dataModel)->turboModeSetpointData;

   SetpointTurboMode_Init(
      &instance->_private.freshFoodSetpointTurboCool,
      dataModel,
      &freshFoodSetpointTurboCoolConfiguration,
      turboModeSetpointData->freshFoodTurboCoolSetpointData);

   SetpointTurboMode_Init(
      &instance->_private.freezerSetpointTurboFreeze,
      dataModel,
      &freezerSetpointTurboFreezeConfiguration,
      turboModeSetpointData->freezerTurboFreezeSetpointData);
}
