/*!
 * @file
 * @brief A priority voting system for the deli pan relays based on the FF fan
 * If the resolved FF fan is Off and Care, then vote the deli fan and deli damper
 * Off and Closed respectively.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANDELIDEPENDENCYVOTING_H
#define FEATUREPANDELIDEPENDENCYVOTING_H

#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t evapFanResolvedVoteErd; // FanVotedSpeed_t
   Erd_t fanVoteErd; // FanVotedSpeed_t
   Erd_t damperVoteErd; // DamperVotedPosition_t
} FeaturePanDeliDependencyVotingConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
      const FeaturePanDeliDependencyVotingConfiguration_t *config;
      I_DataModel_t *dataModel;
   } _private;
} FeaturePanDeliDependencyVoting_t;

/*!
 * @param instance 
 * @param dataModel 
 * @param config 
 */
void FeaturePanDeliDependencyVoting_Init(
   FeaturePanDeliDependencyVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanDeliDependencyVotingConfiguration_t *config);

#endif
