/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AllFreezerDamperPlugin.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

static const DamperVotedPosition_t defaultData = {
   .position = DamperPosition_Closed,
   .care = Vote_DontCare
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const DamperVotedPosition_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t damperPositionErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodDamperPosition_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodDamperPosition_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodDamperPosition_FactoryVote - Erd_FreshFoodDamperPosition_WinningVoteErd)
};

static const DamperRequestManagerConfiguration_t requestManagerConfig = {
   .damperPositionRequestResolvedVoteErd = Erd_FreshFoodDamperPosition_ResolvedVote,
   .damperStepperMotorPositionRequestErd = Erd_DamperStepperMotorPositionRequest,
   .damperHomingRequestErd = Erd_DamperHomingRequest,
   .damperCurrentPositionErd = Erd_DamperCurrentPosition,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState
};

void AllFreezerDamperPlugin_Init(AllFreezerDamperPlugin_t *instance, I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.damperPositionErdResolver,
      DataModel_AsDataSource(dataModel),
      &damperPositionErdResolverConfiguration);

   DamperMotorPlugin_Init(
      &instance->_private.damperMotorPlugin,
      dataModel);

   DamperRequestManager_Init(
      &instance->_private.damperRequestManager,
      dataModel,
      &requestManagerConfig,
      PersonalityParametricData_Get(dataModel)->damperData);
}
