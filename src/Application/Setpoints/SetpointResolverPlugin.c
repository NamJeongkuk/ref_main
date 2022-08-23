/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SetpointResolverPlugin.h"
#include "SystemErds.h"
#include "ErdResolver.h"
#include "Constants_Binary.h"

// (name, winningVoteErd, resolvedStateErd, numberVotingErds)
#define ERD_RESOLVER_TABLE(ENTRY)                                                                                                                                                                                                                       \
   ENTRY(convertibleCompartmentSetPointResolver, Erd_ConvertibleCompartmentSetpoint_WinningVoteErd, Erd_ConvertibleCompartmentSetpoint_ResolvedVote, (Erd_ConvertibleCompartmentSetpoint_UserVote - Erd_ConvertibleCompartmentSetpoint_WinningVoteErd)) \
   ENTRY(freezerSetPointResolver, Erd_FreezerSetpoint_WinningVoteErd, Erd_FreezerSetpoint_ResolvedVote, (Erd_FreezerSetpoint_UserVote - Erd_FreezerSetpoint_WinningVoteErd))                                                                            \
   ENTRY(freshFoodSetPointResolver, Erd_FreshFoodSetpoint_WinningVoteErd, Erd_FreshFoodSetpoint_ResolvedVote, (Erd_FreshFoodSetpoint_UserVote - Erd_FreshFoodSetpoint_WinningVoteErd))

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

static const SetpointVotedTemperature_t defaultData = {
   .temperature = INT16_MAX,
   .care = false
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   REINTERPRET(data, votingErdData, const SetpointVotedTemperature_t *);
   return (data->care);
}

void SetpointResolverPlugin_Init(I_DataModel_t *dataModel)
{
   ERD_RESOLVER_TABLE(EXPAND_AS_RESOLVERS);

   DataModel_Write(
      dataModel,
      Erd_SetpointResolverReady,
      set);
}
