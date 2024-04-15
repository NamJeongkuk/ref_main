/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANASDELIEVAPFANVOTING_H
#define FEATUREPANASDELIEVAPFANVOTING_H

#include "I_DataModel.h"
#include "FanSpeed.h"
#include "FeaturePanMode.h"
#include "FeaturePanData.h"

typedef struct
{
   Erd_t featurePanCoolingModeErd; // FeaturePanMode_t
   Erd_t fanEvapVoteErd; // FanVotedSpeed_t
   Erd_t fanEvapGridVoteErd; // FanVoteSpeed_t
} FeaturePanAsDeliPanEvapFanVotingConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const FeaturePanAsDeliPanEvapFanVotingConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
      const FeaturePanData_t *featurePanData;
   } _private;
} FeaturePanAsDeliPanEvapFanVoting_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param featurePanData
 */
void FeaturePanAsDeliPanEvapFanVoting_Init(
   FeaturePanAsDeliPanEvapFanVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanAsDeliPanEvapFanVotingConfig_t *config,
   const FeaturePanData_t *featurePanData);

#endif
