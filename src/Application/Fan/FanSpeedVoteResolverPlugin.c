/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FanSpeedVoteResolverPlugin.h"
#include "SystemErds.h"
#include "ErdResolver.h"
#include "Constants_Binary.h"

// (name, winningVoteErd, resolvedStateErd, numberVotingErds)
#define ERD_RESOLVER_TABLE(ENTRY)                                                                                                                                                              \
   ENTRY(condenserFanSpeed, Erd_CondenserFanSpeed_WinningVoteErd, Erd_CondenserFanSpeed_ResolvedVote, (Erd_CondenserFanSpeed_GridVote - Erd_CondenserFanSpeed_WinningVoteErd))                 \
   ENTRY(iceCabinetFanSpeed, Erd_IceCabinetFanSpeed_WinningVoteErd, Erd_IceCabinetFanSpeed_ResolvedVote, (Erd_IceCabinetFanSpeed_DefrostVote - Erd_IceCabinetFanSpeed_WinningVoteErd))         \
   ENTRY(freshFoodFanSpeed, Erd_FreshFoodEvapFanSpeed_WinningVoteErd, Erd_FreshFoodEvapFanSpeed_ResolvedVote, (Erd_FreshFoodEvapFanSpeed_GridVote - Erd_FreshFoodEvapFanSpeed_WinningVoteErd)) \
   ENTRY(freezerEvapFanSpeed, Erd_FreezerEvapFanSpeed_WinningVoteErd, Erd_FreezerEvapFanSpeed_ResolvedVote, (Erd_FreezerEvapFanSpeed_GridVote - Erd_FreezerEvapFanSpeed_WinningVoteErd))       \
   ENTRY(convertibleCompartmentFanSpeed, Erd_ConvertibleCompartmentFanSpeed_WinningVoteErd, Erd_ConvertibleCompartmentFanSpeed_ResolvedVote, (Erd_ConvertibleCompartmentFanSpeed_GridVote - Erd_ConvertibleCompartmentFanSpeed_WinningVoteErd))

#define EXPAND_AS_RESOLVERS(name, _winningVoteErd, _resolvedStateErd, _numberVotingErds) \
   static const ErdResolverConfiguration_t name##ResolverConfiguration = {               \
      .votingErdCare = VotingErdCareDelegate,                                            \
      .defaultData = &defaultData,                                                       \
      .winningVoterErd = _winningVoteErd,                                                \
      .resolvedStateErd = _resolvedStateErd,                                             \
      .numberOfVotingErds = _numberVotingErds                                            \
   };                                                                                    \
                                                                                         \
   static ErdResolver_t name##Resolver;                                                  \
   ErdResolver_Init(&name##Resolver, DataModel_AsDataSource(dataModel), &name##ResolverConfiguration);

static const FanVotedSpeed_t defaultData = {
   .speed = FanSpeed_Off,
   .care = false
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const FanVotedSpeed_t *data = votingErdData;
   return (data->care);
}

void FanSpeedVoteResolverPlugin_Init(I_DataModel_t *dataModel)
{
   ERD_RESOLVER_TABLE(EXPAND_AS_RESOLVERS);
}
