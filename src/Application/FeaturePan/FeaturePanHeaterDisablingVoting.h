/*!
 * @file
 * @brief Votes for Feature Pan Heater to be off when feature pan mode is Active Heating and 
 * ambient temperature is greater than or equal to parametric value
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANHEATERDISABLINGVOTING_H
#define FEATUREPANHEATERDISABLINGVOTING_H

#include "I_DataModel.h"
#include "TemperatureDegFx100.h"
#include "FeaturePanData.h"

typedef struct
{
   Erd_t featurePanModeErd; // FeaturePanMode_t
   Erd_t heaterVotedErd; // PercentageDutyCycleVote_t
   Erd_t ambientTemperatureDegFx100Erd; // TemperatureDegFx100_t
} FeaturePanHeaterDisablingVotingConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const FeaturePanHeaterDisablingVotingConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
      const FeaturePanData_t *featurePanData;
   } _private;
} FeaturePanHeaterDisablingVoting_t;

/*!
 * @param instance 
 * @param dataModel 
 * @param config 
 * @param featurePanData 
 */
void FeaturePanHeaterDisablingVoting_Init(
   FeaturePanHeaterDisablingVoting_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanHeaterDisablingVotingConfig_t *config,
   const FeaturePanData_t *featurePanData);

#endif
