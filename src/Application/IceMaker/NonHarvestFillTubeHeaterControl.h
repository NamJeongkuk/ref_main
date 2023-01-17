/*!
 * @file fill tube heater non harvest operation. While not in Harvest state, keep the fill tube heater running at a parametrically defined duty cycle
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NONHARVESTFILLTUBEHEATERCONTROL_H
#define NONHARVESTFILLTUBEHEATERCONTROL_H

#include "I_DataModel.h"
#include "FillTubeHeaterData.h"
#include "I_Event.h"

typedef struct
{
   Erd_t iceAlgorithmIsActiveErd; // bool (Ice Rate, Ice formation)
   Erd_t nonHarvestFillTubeHeaterVoteErd; // PercentageDutyCycleVote_t
} NonHarvestFillTubeHeaterControlConfig_t;

typedef struct
{
   struct
   {
      const FillTubeHeaterData_t *fillTubeHeaterData;
      const NonHarvestFillTubeHeaterControlConfig_t *config;
      EventSubscription_t dataModelOnChangeSubscription;
      I_DataModel_t *dataModel;
   } _private;
} NonHarvestFillTubeHeaterControl_t;

void NonHarvestFillTubeHeaterControl_Init(
   NonHarvestFillTubeHeaterControl_t *instance,
   I_DataModel_t *dataModel,
   const NonHarvestFillTubeHeaterControlConfig_t *config,
   const FillTubeHeaterData_t *fillTubeHeaterData);

#endif
