/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CALCGRIDBLOCKANDGRIDLINES_H
#define CALCGRIDBLOCKANDGRIDLINES_H

#include "DeltaGridLines.h"
#include "CalculatedGridLines.h"
#include "GridBlockNumber.h"
#include "I_DataModel.h"
#include "Timer.h"
#include "RingBuffer.h"
#include "EventSubscription.h"
#include "PersonalityParametricData.h"

typedef struct
{
   Erd_t rawSetpointErd;
   Erd_t offsetErd;
   Erd_t shiftErd;
   Erd_t adjustedSetpointErd;
} GridLineErds_t;

typedef struct
{
   Erd_t calculatedGridBlockErd;
   Erd_t previousGridBlocksErd;
   Erd_t calculatedGridLinesErd;
   Erd_t freshFoodFilteredTempErd;
   Erd_t freezerFilteredTempErd;
   Erd_t timerModuleErd;
   const GridLineErds_t freezerGridLineErds;
   const GridLineErds_t freshFoodGridLineErds;
} GridBlockAndLinesConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t gridBlockSubscription;
      PreviousGridBlockNumbers_t *previousGridBlocks;
      GridBlockNumber_t lastBlock;
      RingBuffer_t bufferInstance;
      const GridBlockAndLinesConfig_t *config;
      const GridData_t *gridData;
      I_DataModel_t *dataModel;
      Timer_t timer;
   } _private;
} CalcGridBlockAndLines_t;

void CalcGridBlockAndGridLines_Init(
   CalcGridBlockAndLines_t *instance,
   const GridBlockAndLinesConfig_t *config,
   I_DataModel_t *dataModel);

#endif
