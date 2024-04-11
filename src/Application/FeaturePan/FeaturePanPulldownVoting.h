/*!
 * @file
 * @brief Priority voting for feature pan pulldown. Will close damper, and turn off fan and heater
 * when feature pan temperature is greater than the parametric limit value
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANPULLDOWNVOTING_H
#define FEATUREPANPULLDOWNVOTING_H

#include "I_DataModel.h"
#include "TemperatureDegFx100.h"
#include "FeaturePanData.h"

typedef struct
{
   Erd_t featurePanTemperatureErd; // TemperatureDegFx100_t
   Erd_t featurePanDamperPositionVoteErd; // DamperVotedPosition_t
   Erd_t featurePanFanVoteErd; // FanVotedSpeed_t
   Erd_t featurePanHeaterVoteErd; // PercentageDutyCycleVote_t
} FeaturePanPulldownVotingConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const FeaturePanPulldownVotingConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
      const FeaturePanData_t *featurePanData;
   } _private;
} FeaturePanPulldownVoting_t;

void FeaturePanPulldownVoting_Init(
   FeaturePanPulldownVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanPulldownVotingConfig_t *config,
   const FeaturePanData_t *featurePanData);

#endif
