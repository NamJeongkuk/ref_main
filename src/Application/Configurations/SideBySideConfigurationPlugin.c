/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideConfigurationPlugin.h"

void SideBySideConfigurationPlugin_Init(SideBySideConfigurationPlugin_t *instance, I_DataModel_t *dataModel)
{
   SideBySideSingleEvaporatorCoolingSystemPlugin_Init(&instance->_private.coolingSystemPlugin, dataModel);
   IsolationWaterValvePlugin_Init(&instance->_private.isolationWaterValvePlugin, dataModel);
   AluminumMoldIceMakerPlugin_Init(&instance->_private.aluminumMoldIceMakerPlugin, dataModel);
   SideBySideStaggeredHeater_Init(&instance->_private.staggeredHeaterPlugin, dataModel);
   SideBySideFactoryPlugin_Init(&instance->_private.factoryPlugin, dataModel);
}
