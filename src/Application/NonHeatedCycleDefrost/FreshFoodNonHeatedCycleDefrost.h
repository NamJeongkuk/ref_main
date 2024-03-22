/*!
 * @file
 * @brief The module runs fresh food evaporator fan when compressor changes to off or valve position changes
 *        to stop to flow cooling air to fresh food compartment
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODNONHEATEDCYCLEDEFROST
#define FRESHFOODNONHEATEDCYCLEDEFROST

#include "DataModel.h"
#include "EventSubscription.h"
#include "Hsm.h"
#include "Timer.h"

typedef struct
{
   Erd_t freshFoodEvapFanSpeedNonHeatedCycleDefrostVoteErd;
   Erd_t compressorIsOnErd;
   Erd_t defrostStateErd;
   Erd_t freshFoodSetpointResolvedVoteErd;
   Erd_t sealedSystemValvePreviousPositionErd;
   Erd_t sealedSystemValveCurrentPositionErd;
} FreshFoodNonHeatedCycleDefrostConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      Timer_t timer;
      Hsm_t hsm;
      const FreshFoodNonHeatedCycleDefrostConfig_t *config;
      const FreshFoodNonHeatedCycleDefrostData_t *freshFoodNonHeatedCycleDefrostData;
      const UserSetpointRangeData_t *freshFoodUserSetpointRangeData;
   } _private;
} FreshFoodNonHeatedCycleDefrost_t;

void FreshFoodNonHeatedCycleDefrost_Init(
   FreshFoodNonHeatedCycleDefrost_t *instance,
   I_DataModel_t *dataModel,
   const FreshFoodNonHeatedCycleDefrostConfig_t *config,
   const FreshFoodNonHeatedCycleDefrostData_t *freshFoodNonHeatedCycleDefrostData,
   const UserSetpointRangeData_t *freshFoodUserSetpointRangeData);

#endif
