/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideUserSetpointPlugin.h"
#include "PersonalityParametricData.h"
#include "SystemErds.h"

static const UserSetpointRequestHandlerConfig_t freshFoodConfig = {
   .userSetpointRequestErd = Erd_FreshFoodSetpointRequest,
   .userSetpointVoteErd = Erd_FreshFoodSetpoint_UserVote,
   .userSetpointRangeDataErd = Erd_FreshFoodSetpointRangeData,
   .userSetpointStatusErd = Erd_FreshFoodSetpointStatus
};

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
      Erd_FreshFoodSetpointRangeData,
      personalityData->setpointData->userSetpointData->freshFoodUserSetpoint);
   DataModel_Write(
      dataModel,
      Erd_FreezerSetpointRangeData,
      personalityData->setpointData->userSetpointData->freezerUserSetpoint);
}

void SideBySideUserSetpointPlugin_Init(
   SideBySideUserSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   WriteSetpointRangeDataToErds(dataModel);

   UserSetpointRequestHandler_Init(
      &instance->_private.freshFoodHandler,
      &freshFoodConfig,
      dataModel);

   UserSetpointRequestHandler_Init(
      &instance->_private.freezerHandler,
      &freezerConfig,
      dataModel);
}
