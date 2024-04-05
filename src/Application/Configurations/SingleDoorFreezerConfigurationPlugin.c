/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerConfigurationPlugin.h"

void SingleDoorFreezerConfigurationPlugin_Init(SingleDoorFreezerConfigurationPlugin_t *instance, I_DataModel_t *dataModel)
{
   SingleDoorFreezerSingleEvaporatorCoolingSystemPlugin_Init(&instance->_private.coolingSystemPlugin, dataModel);
   IsolationWaterValvePlugin_Init(&instance->_private.isolationWaterValvePlugin, dataModel);
   IceMakerSlotsPlugin_Init(&instance->_private.iceMakerSlotsPlugin, dataModel);
   DispenserWaterValvePlugin_Init(&instance->_private.dispenserWaterValvePlugin, dataModel);
   AugerMotorPlugin_Init(&instance->_private.augerMotorPlugin, dataModel);
   RecessHeaterPlugin_Init(&instance->_private.recessHeaterPlugin, dataModel);
   SingleDoorFreezerDoorPlugin_Init(&instance->_private.singleDoorFreezerDoorPlugin, dataModel);
   SingleDoorFreezerFactoryPlugin_Init(&instance->_private.factoryPlugin, dataModel);
}
