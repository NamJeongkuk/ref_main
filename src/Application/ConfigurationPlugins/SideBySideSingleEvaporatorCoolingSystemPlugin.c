/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideSingleEvaporatorCoolingSystemPlugin.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

void SideBySideSingleEvaporatorCoolingSystemPlugin_Init(SideBySideSingleEvaporatorCoolingSystemPlugin_t *instance, I_DataModel_t *dataModel)
{
   I_ConstArrayMap_t *constArrayMapInterface = ConstArrayMap_SideBySideSingleEvap_Init(&instance->_private.coolingStateBasedGridVotesTable);
   DataModelErdPointerAccess_Write(dataModel, Erd_CoolingStatesGridVotesConstArrayMapInterface, constArrayMapInterface);

   SideBySideSetpointPlugin_Init(&instance->_private.sideBySideSetpointPlugin, dataModel);
   SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin_Init(&instance->_private.sensorFilteringPlugin, dataModel);
   GridPlugin_Init(&instance->_private.gridPlugin, dataModel);
   CompressorPlugin_Init(&instance->_private.compressorPlugin, dataModel);

   ConvertibleCompartmentStatePlugin_Init(&instance->_private.convertibleCompartmentStatePlugin, dataModel); // we need this for now until we get defrost to not rely on it :(

   DefrostPlugin_Init(&instance->_private.defrostPlugin, dataModel);
   FreshFoodDamperPlugin_Init(&instance->_private.freshFoodDamperPlugin, dataModel);
   SideBySideFanPlugin_Init(&instance->_private.sideBySideFanPlugin, dataModel);
}
