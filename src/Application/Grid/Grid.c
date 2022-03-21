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

static bool GridIdIsValid(Grid_t *instance)
{
   return (instance->_private.gridData->gridId < instance->_private.configuration->gridFunctions->numOfGrids);
}

static void Grid_Run(void *context)
{
   REINTERPRET(instance, context, Grid_t *);

   instance->_private.configuration->gridFunctions->grids[instance->_private.gridData->gridId](instance->_private.dataModel);
}

void Grid_Init(
   Grid_t *instance,
   const GridConfiguration_t *configuration,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   instance->_private.configuration = configuration;
   instance->_private.gridData = PersonalityParametricData_Get(instance->_private.dataModel)->gridData;

   if(GridIdIsValid(instance))
   {
      TimerModule_StartPeriodic(
         DataModelErdPointerAccess_GetTimerModule(dataModel, instance->_private.configuration->timerModuleErd),
         &instance->_private.gridTimer,
         instance->_private.gridData->gridPeriodicRunRateInMSec,
         Grid_Run,
         instance);
   }
}
