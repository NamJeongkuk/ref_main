/*!
 * @file
 * @brief main grid control
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Grid.h"
#include "utils.h"
#include "DataModelErdPointerAccess.h"
#include "GridBlockNumber.h"

static bool GridIdIsValid(const GridConfiguration_t *config)
{
   return (config->gridId < config->gridFunctions->numOfGrids);
}

static void Grid_Run(void *context)
{
   REINTERPRET(instance, context, Grid_t *);

   instance->_private.configuration->gridFunctions->grids[instance->_private.configuration->gridId](instance);
}

void Grid_Init(
   Grid_t *instance,
   const GridConfiguration_t *configuration,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   instance->_private.configuration = configuration;

   if(GridIdIsValid(configuration))
   {
      TimerModule_StartPeriodic(
         DataModelErdPointerAccess_GetTimerModule(dataModel, instance->_private.configuration->timerModuleErd),
         &instance->_private.gridTimer,
         configuration->periodicGridLineCalcRate,
         Grid_Run,
         instance);
   }
}
