/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorConfigurationPlugin.h"

void FourDoorConfigurationPlugin_Init(FourDoorConfigurationPlugin_t *instance, I_DataModel_t *dataModel)
{
   FourDoorDualEvaporatorCoolingSystemPlugin_Init(&instance->_private.coolingSystemPlugin, dataModel);
   IsolationWaterValvePlugin_Init(&instance->_private.isolationWaterValvePlugin, dataModel);
   IceMakerSlotsPlugin_Init(&instance->_private.iceMakerSlotsPlugin, dataModel);
   DispenserWaterValvePlugin_Init(&instance->_private.dispenserWaterValvePlugin, dataModel);
   AugerMotorPlugin_Init(&instance->_private.augerMotorPlugin, dataModel);
   RecessHeaterPlugin_Init(&instance->_private.recessHeaterPlugin, dataModel);
   FourDoorBooleanRequestStatusUpdaterPlugin_Init(&instance->_private.booleanRequestStatusUpdaterPlugin, dataModel);
}
