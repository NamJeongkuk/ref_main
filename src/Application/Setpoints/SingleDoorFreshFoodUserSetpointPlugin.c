/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreshFoodSetpointPlugin.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

static const UserSetpointRequestHandlerConfig_t freshFoodConfig = {
   .userSetpointRequestErd = Erd_FreshFoodSetpointRequest,
   .userSetpointVoteErd = Erd_FreshFoodSetpoint_UserVote,
   .userSetpointRangeDataErd = Erd_FreshFoodSetpointRangeData,
   .userSetpointStatusErd = Erd_FreshFoodSetpointStatus
};

static void WriteSetpointRangeDataToErds(I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *personalityData = PersonalityParametricData_Get(dataModel);

   DataModel_Write(
      dataModel,
      Erd_FreshFoodSetpointRangeData,
      personalityData->setpointData->userSetpointData->freshFoodUserSetpoint);
}

void SingleDoorFreshFoodUserSetpointPlugin_Init(
   SingleDoorFreshFoodUserSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   WriteSetpointRangeDataToErds(dataModel);

   UserSetpointRequestHandler_Init(
      &instance->_private.freshFoodHandler,
      &freshFoodConfig,
      dataModel);

   DataModel_Write(
      dataModel, 
      Erd_UserSetpointPluginReady, 
      set);
}
