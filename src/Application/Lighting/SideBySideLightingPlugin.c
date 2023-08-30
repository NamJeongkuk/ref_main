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
   const RampingPwmDutyCyclePercentageVote_t *data = votingErdData;
   return (data->care);
}

static const RampingPwmDutyCyclePercentageVote_t defaultData = {
   .rampingPwmDutyCyclePercentage = {
      .pwmDutyCyclePercentage = PwmDutyCycle_Min,
      .rampingUpCountInMsec = UINT8_MAX,
      .rampingDownCountInMsec = UINT8_MAX,
   },
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
   { Erd_FreshFoodBackWallLight_ResolvedVote, Erd_FreshFoodBackWallLight_RampingPwm },
   { Erd_FreshFoodTopLight_ResolvedVote, Erd_FreshFoodTopLight_RampingPwm },
   { Erd_FreezerBackWallLight_ResolvedVote, Erd_FreezerBackWallLight_RampingPwm },
   { Erd_FreezerTopLight_ResolvedVote, Erd_FreezerTopLight_RampingPwm }
};

static const ConstArrayMap_LinearSearchConfiguration_t linearMapConfiguration = {
   votedPwmPairs,
   NUM_ELEMENTS(votedPwmPairs),
   ELEMENT_SIZE(votedPwmPairs),
   MEMBER_SIZE(VotedPwmDutyCyclePair_t, pwmDutyCycleResolvedVoteErd),
   OFFSET_OF(VotedPwmDutyCyclePair_t, pwmDutyCycleResolvedVoteErd)
};

static const Erd_t freshFoodDoorErds[] = {
   Erd_RightSideFreshFoodDoorStatusResolved
};

static const LightingDoorVoteResolverConfig_t freshFoodBackWallLightingDoorVoteResolverConfig = {
   .timerModuleErd = Erd_TimerModule,
   .rampingPwmDutyCyclePercentageErd = Erd_FreshFoodBackWallLight_DoorVote,
   .doorIsOpenErds = freshFoodDoorErds,
   .numberOfDoorErds = NUM_ELEMENTS(freshFoodDoorErds),
};

static const LightingDoorVoteResolverConfig_t freshFoodTopLightLightingDoorVoteResolverConfig = {
   .timerModuleErd = Erd_TimerModule,
   .rampingPwmDutyCyclePercentageErd = Erd_FreshFoodTopLight_DoorVote,
   .doorIsOpenErds = freshFoodDoorErds,
   .numberOfDoorErds = NUM_ELEMENTS(freshFoodDoorErds),
};

static const Erd_t freezerDoorErds[] = {
   Erd_LeftSideFreezerDoorStatusResolved,
};

static const LightingDoorVoteResolverConfig_t freezerBackWallLightingDoorVoteResolverConfig = {
   .timerModuleErd = Erd_TimerModule,
   .rampingPwmDutyCyclePercentageErd = Erd_FreezerBackWallLight_DoorVote,
   .doorIsOpenErds = freezerDoorErds,
   .numberOfDoorErds = NUM_ELEMENTS(freezerDoorErds),
};

static const LightingDoorVoteResolverConfig_t freezerTopLightLightingDoorVoteResolverConfig = {
   .timerModuleErd = Erd_TimerModule,
   .rampingPwmDutyCyclePercentageErd = Erd_FreezerTopLight_DoorVote,
   .doorIsOpenErds = freezerDoorErds,
   .numberOfDoorErds = NUM_ELEMENTS(freezerDoorErds),
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

   ConstArrayMap_LinearSearch_Init(
      &instance->_private.linearMap,
      &linearMapConfiguration);

   LightingController_Init(
      &instance->_private.lightingController,
      dataModel,
      &instance->_private.linearMap.interface,
      PersonalityParametricData_Get(dataModel)->lightingData);

   LightingDoorVoteResolver_Init(
      &instance->_private.freshFoodBackWallLightingDoorVoteResolver,
      dataModel,
      &freshFoodBackWallLightingDoorVoteResolverConfig,
      PersonalityParametricData_Get(dataModel)->lightingData->freshFoodBackWallDoorLightingData);

   LightingDoorVoteResolver_Init(
      &instance->_private.freshFoodTopLightLightingDoorVoteResolver,
      dataModel,
      &freshFoodTopLightLightingDoorVoteResolverConfig,
      PersonalityParametricData_Get(dataModel)->lightingData->freshFoodTopAndSideDoorLightingData);

   LightingDoorVoteResolver_Init(
      &instance->_private.freezerBackWallLightingDoorVoteResolver,
      dataModel,
      &freezerBackWallLightingDoorVoteResolverConfig,
      PersonalityParametricData_Get(dataModel)->lightingData->freezerBackWallDoorLightingData);

   LightingDoorVoteResolver_Init(
      &instance->_private.freezerTopLightLightingDoorVoteResolver,
      dataModel,
      &freezerTopLightLightingDoorVoteResolverConfig,
      PersonalityParametricData_Get(dataModel)->lightingData->freezerTopAndSideDoorLightingData);
}
