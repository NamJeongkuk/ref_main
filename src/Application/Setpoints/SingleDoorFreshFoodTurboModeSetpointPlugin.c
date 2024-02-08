/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreshFoodTurboModeSetpointPlugin.h"
#include "SystemErds.h"

static const SetpointTurboModeConfig_t freshFoodSetpointTurboCoolConfiguration = {
   .turboModeSetpointVoteErd = Erd_FreshFoodSetpoint_TurboCoolVote,
   .turboModeOnOffStatusErd = Erd_TurboCoolOnOffStatus,
   .turboModeOnTimeInMinutesErd = Erd_TurboCoolOnTimeInMinutes,
   .timerModuleErd = Erd_TimerModule
};

void SingleDoorFreshFoodTurboModeSetpointPlugin_Init(
   SingleDoorFreshFoodTurboModeSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const TurboModeSetpointData_t *turboModeSetpointData =
      PersonalityParametricData_Get(dataModel)->turboModeSetpointData;

   SetpointTurboMode_Init(
      &instance->_private.freshFoodSetpointTurboCool,
      dataModel,
      &freshFoodSetpointTurboCoolConfiguration,
      turboModeSetpointData->freshFoodTurboCoolSetpointData);
}
