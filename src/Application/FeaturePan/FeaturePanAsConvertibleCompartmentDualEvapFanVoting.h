/*!
 * @file
 * @brief Priority Voting Module for Dual Evap Fan Voting
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANASCONVERTIBLECOMPARTMENTDUALEVAPVOTING_H
#define FEATUREPANASCONVERTIBLECOMPARTMENTDUALEVAPVOTING_H

#include "I_DataModel.h"
#include "FeaturePanMode.h"

typedef struct
{
   Erd_t featurePanModeErd; // FeaturePanMode_t
   Erd_t evapFanVote; // FanVotedSpeed_t
} FeaturePanAsConvertibleCompartmentDualEvapFanVotingConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const FeaturePanAsConvertibleCompartmentDualEvapFanVotingConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
   } _private;
} FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t;

void FeaturePanAsConvertibleCompartmentDualEvapFanVoting_Init(
   FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanAsConvertibleCompartmentDualEvapFanVotingConfig_t *config);

#endif
