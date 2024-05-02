/*!
 * @file
 * @brief A priority voting system for the convertible
 * compartment based on the FZ evap fan speed
 * If the resolved FZ evap fan is Off and Care, then
 * vote the convertible compartment damper Closed and Care.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANCONVERTIBLEFREEZEREVAPFANDEPENDENCYVOTING_H
#define FEATUREPANCONVERTIBLEFREEZEREVAPFANDEPENDENCYVOTING_H

#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t freezerEvapFanResolvedVoteErd; // FanVotedSpeed_t
   Erd_t convertibleCompartmentDamperVoteErd; // DamperVotedPosition_t
} FeaturePanConvertibleFreezerEvapFanDependencyVotingConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
      const FeaturePanConvertibleFreezerEvapFanDependencyVotingConfiguration_t *config;
      I_DataModel_t *dataModel;
   } _private;
} FeaturePanConvertibleFreezerEvapFanDependencyVoting_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void FeaturePanConvertibleFreezerEvapFanDependencyVoting_Init(
   FeaturePanConvertibleFreezerEvapFanDependencyVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanConvertibleFreezerEvapFanDependencyVotingConfiguration_t *config);

#endif
