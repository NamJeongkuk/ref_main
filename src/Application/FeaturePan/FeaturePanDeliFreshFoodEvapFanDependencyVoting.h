/*!
 * @file
 * @brief A priority voting system for the deli pan relays based on the FF fan
 * If the resolved FF fan is Off and Care, then vote the deli fan and deli damper
 * Off and Closed respectively.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANDELIFRESHFOODEVAPFANDEPENDENCYVOTING_H
#define FEATUREPANDELIFRESHFOODEVAPFANDEPENDENCYVOTING_H

#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t evapFanResolvedVoteErd; // FanVotedSpeed_t
   Erd_t fanVoteErd; // FanVotedSpeed_t
   Erd_t damperVoteErd; // DamperVotedPosition_t
} FeaturePanDeliFreshFoodEvapFanDependencyVotingConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
      const FeaturePanDeliFreshFoodEvapFanDependencyVotingConfig_t *config;
      I_DataModel_t *dataModel;
   } _private;
} FeaturePanDeliFreshFoodEvapFanDependencyVoting_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void FeaturePanDeliFreshFoodEvapFanDependencyVoting_Init(
   FeaturePanDeliFreshFoodEvapFanDependencyVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanDeliFreshFoodEvapFanDependencyVotingConfig_t *config);

#endif
