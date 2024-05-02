/*!
 * @file
 * @brief Votes for the feature pan heater during active heating mode if temperature
 * slope is below lower limit and votes off if it is above upper limit
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANWARMUPSLOPEVOTING_H
#define FEATUREPANWARMUPSLOPEVOTING_H

#include "I_DataModel.h"
#include "Timer.h"
#include "TemperatureDegFx100.h"
#include "FeaturePanData.h"
#include "FeaturePanMode.h"

typedef struct
{
   Erd_t featurePanCoolingModeErd; // FeaturePanCoolingMode_t
   Erd_t heaterVotedErd; // PercentageDutyCycleVote_t
   Erd_t featurePanTemperatureDegFx100Erd; // TemperatureDegFx100_t
} FeaturePanWarmupSlopeVotingConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const FeaturePanWarmupSlopeVotingConfig_t *config;
      const FeaturePanData_t *featurePanData;
      EventSubscription_t onDataModelChangeSubscription;
      Timer_t slopeCalculatorTimer;
      TemperatureDegFx100_t oldTemperature;
      int16_t slopeOfFeaturePanTemperature;
      FeaturePanCoolingMode_t currentFeaturePanMode;
      bool slopeHasBeenCalculated;
   } _private;
} FeaturePanWarmupSlopeVoting_t;

/*!
 * @param instance
 * @param dataModel
 * @param timerModule
 * @param config
 * @param featurePanData
 */
void FeaturePanWarmupSlopeVoting_Init(
   FeaturePanWarmupSlopeVoting_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   const FeaturePanWarmupSlopeVotingConfig_t *config,
   const FeaturePanData_t *featurePanData);

#endif
