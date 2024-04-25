/*!
 * @file
 * @brief Votes for the ice cabinet fan off when the freezer fan is off
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETFREEZEREVAPFANDEPENDENCYVOTING_H
#define ICECABINETFREEZEREVAPFANDEPENDENCYVOTING_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t freezerEvapFanSpeedResolvedVoteErd; // FanVotedSpeed_t
   Erd_t iceCabinetDependencyOnFreezerFanVoteErd; // FanVotedSpeed_t
} IceCabinetFreezerEvapFanDependencyVotingConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
      I_DataModel_t *dataModel;
      const IceCabinetFreezerEvapFanDependencyVotingConfiguration_t *config;
   } _private;
} IceCabinetFreezerEvapFanDependencyVoting_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void IceCabinetFreezerEvapFanDependencyVoting_Init(
   IceCabinetFreezerEvapFanDependencyVoting_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetFreezerEvapFanDependencyVotingConfiguration_t *config);

#endif
