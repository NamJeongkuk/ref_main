/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODONLYDEFROSTARBITRATOR_H
#define FRESHFOODONLYDEFROSTARBITRATOR_H
#include "Erd.h"
#include "I_DataModel.h"
#include "DefrostData.h"
#include "EventSubscription.h"

typedef struct
{
   Erd_t numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd; // uint8_t
   Erd_t freezerDefrostWasAbnormalErd; // bool
   Erd_t defrostIsFreshFoodOnlyErd; // bool
} FreshFoodOnlyDefrostArbitratorConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t onDataModelChange;
      I_DataModel_t *dataModel;
      const DefrostData_t *defrostParametricData;
      const FreshFoodOnlyDefrostArbitratorConfiguration_t *configuration;
   } _private;
} FreshFoodOnlyDefrostArbitrator_t;

void FreshFoodOnlyDefrostArbitrator_Init(
   FreshFoodOnlyDefrostArbitrator_t *instance,
   I_DataModel_t *dataModel,
   const FreshFoodOnlyDefrostArbitratorConfiguration_t *configuration);

#endif
