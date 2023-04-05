/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AugerMotorPlugin.h"
#include "SystemErds.h"

static bool AugerMotorVotingErdCareDelegate(const void *votingErdData)
{
   const AugerMotorVotedIceType_t *data = votingErdData;
   return (data->care);
}

static const AugerMotorVotedIceType_t defaultAugerMotorData = {
   .iceType = AugerMotorIceType_Off,
   .care = Vote_DontCare
};

static const ErdResolverConfiguration_t augerMotorResolverConfiguration = {
   .votingErdCare = AugerMotorVotingErdCareDelegate,
   .defaultData = &defaultAugerMotorData,
   .winningVoterErd = Erd_AugerMotor_WinningVoteErd,
   .resolvedStateErd = Erd_AugerMotor_ResolvedVote,
   .numberOfVotingErds = (Erd_AugerMotor_DispensingVote - Erd_AugerMotor_WinningVoteErd)
};

void AugerMotorPlugin_Init(
   AugerMotorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.augerMotorErdResolver,
      DataModel_AsDataSource(dataModel),
      &augerMotorResolverConfiguration);
}
