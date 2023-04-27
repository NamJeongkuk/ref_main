/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideLightingPlugin.h"
#include "PwmVotedDutyCycle.h"
#include "SystemErds.h"
#include "VotedPwmDutyCyclePair.h"

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const PwmVotedDutyCycle_t *data = votingErdData;
   return (data->care);
}

static const PwmVotedDutyCycle_t defaultData = {
   .pwmDutyCycle = PwmDutyCycle_Min,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t freshFoodBackWallLightErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodBackWallLight_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodBackWallLight_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodBackWallLight_DoorVote - Erd_FreshFoodBackWallLight_WinningVoteErd)
};

static const ErdResolverConfiguration_t freshFoodTopLightErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreshFoodTopLight_WinningVoteErd,
   .resolvedStateErd = Erd_FreshFoodTopLight_ResolvedVote,
   .numberOfVotingErds = (Erd_FreshFoodTopLight_DoorVote - Erd_FreshFoodTopLight_WinningVoteErd)
};

static const ErdResolverConfiguration_t freezerBackWallLightErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreezerBackWallLight_WinningVoteErd,
   .resolvedStateErd = Erd_FreezerBackWallLight_ResolvedVote,
   .numberOfVotingErds = (Erd_FreezerBackWallLight_DoorVote - Erd_FreezerBackWallLight_WinningVoteErd)
};

static const ErdResolverConfiguration_t freezerTopLightErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_FreezerTopLight_WinningVoteErd,
   .resolvedStateErd = Erd_FreezerTopLight_ResolvedVote,
   .numberOfVotingErds = (Erd_FreezerTopLight_DoorVote - Erd_FreezerTopLight_WinningVoteErd)
};

static const VotedPwmDutyCyclePair_t votedPwmPairs[] = {
   { Erd_FreshFoodBackWallLight_ResolvedVote, Erd_FreshFoodBackWallLight_Pwm },
   { Erd_FreshFoodTopLight_ResolvedVote, Erd_FreshFoodTopLight_Pwm },
   { Erd_FreezerBackWallLight_ResolvedVote, Erd_FreezerBackWallLight_Pwm },
   { Erd_FreezerTopLight_ResolvedVote, Erd_FreezerTopLight_Pwm }
};

static const ConstArrayMap_LinearSearchConfiguration_t linearMapConfiguration = {
   votedPwmPairs,
   NUM_ELEMENTS(votedPwmPairs),
   ELEMENT_SIZE(votedPwmPairs),
   MEMBER_SIZE(VotedPwmDutyCyclePair_t, pwmDutyCycleResolvedVoteErd),
   OFFSET_OF(VotedPwmDutyCyclePair_t, pwmDutyCycleResolvedVoteErd)
};

static void InitializeErdResolvers(
   SideBySideLightingPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.freshFoodBackWallLightErdResolver,
      DataModel_AsDataSource(dataModel),
      &freshFoodBackWallLightErdResolverConfiguration);

   ErdResolver_Init(
      &instance->_private.freshFoodTopLightErdResolver,
      DataModel_AsDataSource(dataModel),
      &freshFoodTopLightErdResolverConfiguration);

   ErdResolver_Init(
      &instance->_private.freezerBackWallLightErdResolver,
      DataModel_AsDataSource(dataModel),
      &freezerBackWallLightErdResolverConfiguration);

   ErdResolver_Init(
      &instance->_private.freezerTopLightErdResolver,
      DataModel_AsDataSource(dataModel),
      &freezerTopLightErdResolverConfiguration);
}

void SideBySideLightingPlugin_Init(
   SideBySideLightingPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   InitializeErdResolvers(
      instance,
      dataModel);

   // next pr will have the door stuff. it will go here ignore this it wont actually be merged in lol

   ConstArrayMap_LinearSearch_Init(
      &instance->_private.linearMap,
      &linearMapConfiguration);

   LightingController_Init(
      &instance->_private.lightingController,
      dataModel,
      &instance->_private.linearMap.interface);
}
