/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GridPlugin.h"
#include "SystemErds.h"
#include "Grid_ThreeDoor.h"
#include "Grid_QuadDoor.h"
#include "Grid_FourDoor.h"

enum
{
   GridRunCallbackRate = 1000,
   GridId = 1,
};

static const GridFunction_t grids[] = { Grid_ThreeDoor, Grid_FourDoor, Grid_QuadDoor };

static const GridFunctionArray_t gridFunctionArray = {
   grids,
   NUM_ELEMENTS(grids)
};

static const GridConfiguration_t gridConfig = {
   .timerModuleErd = Erd_TimerModule,
   .personalityParametricDataErd = Erd_PersonalityParametricData,
   .periodicGridLineCalcRate = GridRunCallbackRate, // get this from parametric
   .gridFunctions = &gridFunctionArray,
   .gridId = GridId, // get this from parametric
};

void GridPlugin_Init(
   GridPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   Grid_Init(&instance->gridInstance, &gridConfig, dataModel);
}
