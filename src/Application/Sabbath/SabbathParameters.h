/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SABBATHPARAMETERS_H
#define SABBATHPARAMETERS_H

#include "I_DataModel.h"
#include "DefrostData.h"
#include "SabbathData.h"

typedef struct
{
   Erd_t sabbathModeErd; // bool
   Erd_t maxTimeBetweenDefrostsInMinutesErd; // uint16_t
} SabbathParametersConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const SabbathParametersConfig_t *config;
      const DefrostData_t *defrostParametricData;
      const SabbathData_t *sabbathParametricData;
      EventSubscription_t sabbathModeSubscription;
   } _private;
} SabbathParameters_t;

void SabbathParameters_Init(
   SabbathParameters_t *instance,
   I_DataModel_t *dataModel,
   const SabbathParametersConfig_t *sabbathParametersConfig);

#endif
