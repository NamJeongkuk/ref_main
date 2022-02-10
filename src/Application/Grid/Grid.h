/*!
 * @file
 * @brief Main Grid Functionality
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GRID_H
#define GRID_H

#include <stdint.h>
#include "I_DataModel.h"
#include "Timer.h"
#include "GridLines.h"

typedef void (*GridFunction_t)(void *context);

typedef uint8_t GridId_t;

typedef struct
{
   const GridFunction_t *grids;
   uint8_t numOfGrids;
} GridFunctionArray_t;

typedef struct
{
   Erd_t timerModuleErd;
   Erd_t personalityParametricDataErd;
   TimerTicks_t periodicGridLineCalcRate;
   const GridFunctionArray_t *gridFunctions;
   GridId_t gridId;
   const GridLines_t *parametricGridLines;
} GridConfiguration_t;

typedef struct
{
   struct
   {
      const GridConfiguration_t *configuration;
      I_DataModel_t *dataModel;
      Timer_t gridTimer;
   } _private;
} Grid_t;

/*!
 * Initialize a Grid Module
 * @param instance The object instance.
 * @param configuration contains erds and other unique configs for instance.
 * @param dataModel instance of dataModel
 */
void Grid_Init(
   Grid_t *instance,
   const GridConfiguration_t *configuration,
   I_DataModel_t *dataModel);

#endif
