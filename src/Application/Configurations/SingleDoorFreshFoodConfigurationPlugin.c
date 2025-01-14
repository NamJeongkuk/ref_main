/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreshFoodConfigurationPlugin.h"

void SingleDoorFreshFoodConfigurationPlugin_Init(SingleDoorFreshFoodConfigurationPlugin_t *instance, I_DataModel_t *dataModel)
{
   SingleDoorFreshFoodEvaporatorCoolingSystemPlugin_Init(&instance->_private.coolingSystemPlugin, dataModel);
   IsolationWaterValvePlugin_Init(&instance->_private.isolationWaterValvePlugin, dataModel);
   DispenserWaterValvePlugin_Init(&instance->_private.dispenserWaterValvePlugin, dataModel);
   RecessHeaterPlugin_Init(&instance->_private.recessHeaterPlugin, dataModel);
   SingleDoorFreshFoodDoorPlugin_Init(&instance->_private.singleDoorFreshFoodDoorPlugin, dataModel);
   SingleDoorFreshFoodFactoryPlugin_Init(&instance->_private.factoryPlugin, dataModel);
}
