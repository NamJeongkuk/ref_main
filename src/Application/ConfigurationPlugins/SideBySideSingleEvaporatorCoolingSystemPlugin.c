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

   GridPlugin_Init(&instance->_private.gridPlugin, dataModel);
   CompressorPlugin_Init(&instance->_private.compressorPlugin, dataModel);
   DefrostPlugin_Init(&instance->_private.defrostPlugin, dataModel);
   FreshFoodDamperPlugin_Init(&instance->_private.freshFoodDamperPlugin, dataModel);
   FreshFoodShiftOffsetCalculatorPlugin_Init(&instance->_private.freshFoodShiftOffsetCalculatorPlugin, dataModel);
   FreezerShiftOffsetCalculatorPlugin_Init(&instance->_private.freezerShiftOffsetCalculatorPlugin, dataModel);
}
