/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerUserSetpointPlugin.h"
#include "Constants_Binary.h"
#include "PersonalityParametricData.h"
#include "SystemErds.h"

static const UserSetpointRequestHandlerConfig_t freezerConfig = {
   .userSetpointRequestErd = Erd_FreezerSetpointRequest,
   .userSetpointVoteErd = Erd_FreezerSetpoint_UserVote,
   .userSetpointRangeDataErd = Erd_FreezerSetpointRangeData,
   .userSetpointStatusErd = Erd_FreezerSetpointStatus
};

static void WriteSetpointRangeDataToErds(I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *personalityData = PersonalityParametricData_Get(dataModel);

   DataModel_Write(
      dataModel,
      Erd_FreezerSetpointRangeData,
      personalityData->setpointData->userSetpointData->freezerUserSetpoint);
}

void SingleDoorFreezerUserSetpointPlugin_Init(
   SingleDoorFreezerUserSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   WriteSetpointRangeDataToErds(dataModel);

   UserSetpointRequestHandler_Init(
      &instance->_private.freezerHandler,
      &freezerConfig,
      dataModel);

   DataModel_Write(
      dataModel, 
      Erd_UserSetpointPluginReady, 
      set);
}
