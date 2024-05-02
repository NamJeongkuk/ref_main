/*!
 * @file
 * @brief Module to turn vote for deli heater off if the deli fan rpm is zero
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANDELIDELIFANDEPENDENCYVOTING_H
#define FEATUREPANDELIDELIFANDEPENDENCYVOTING_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t deliFanActualRpmErd; // FanRpm_t
   Erd_t deliHeaterVotedErd; // PercentageDutyCycleVote_t
} FeaturePanDeliDeliFanDependencyVotingConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const FeaturePanDeliDeliFanDependencyVotingConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
   } _private;
} FeaturePanDeliDeliFanDependencyVoting_t;

void FeaturePanDeliDeliFanDependencyVoting_Init(
   FeaturePanDeliDeliFanDependencyVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanDeliDeliFanDependencyVotingConfig_t *config);

#endif
