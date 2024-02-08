/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerTurboModeSetpointPlugin.h"
#include "SystemErds.h"
#include "TurboModeSetpointData.h"

static const SetpointTurboModeConfig_t freezerSetpointTurboFreezeConfiguration = {
   .turboModeSetpointVoteErd = Erd_FreezerSetpoint_TurboFreezeVote,
   .turboModeOnOffStatusErd = Erd_TurboFreezeOnOffStatus,
   .turboModeOnTimeInMinutesErd = Erd_TurboFreezeOnTimeInMinutes,
   .timerModuleErd = Erd_TimerModule
};

void SingleDoorFreezerTurboModeSetpointPlugin_Init(
   SingleDoorFreezerTurboModeSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const TurboModeSetpointData_t *turboModeSetpointData =
      PersonalityParametricData_Get(dataModel)->turboModeSetpointData;

   SetpointTurboMode_Init(
      &instance->_private.freezerSetpointTurboFreeze,
      dataModel,
      &freezerSetpointTurboFreezeConfiguration,
      turboModeSetpointData->freezerTurboFreezeSetpointData);
}
