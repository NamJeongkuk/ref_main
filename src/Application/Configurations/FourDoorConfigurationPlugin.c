/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorConfigurationPlugin.h"
#include "Constants_Binary.h"

void FourDoorConfigurationPlugin_Init(FourDoorConfigurationPlugin_t *instance, I_DataModel_t *dataModel)
{
   FourDoorDualEvaporatorCoolingSystemPlugin_Init(&instance->_private.coolingSystemPlugin, dataModel);
   IsolationWaterValvePlugin_Init(&instance->_private.isolationWaterValvePlugin, dataModel);
   IceMakerSlotsPlugin_Init(&instance->_private.iceMakerSlotsPlugin, dataModel);
   DispenserWaterValvePlugin_Init(&instance->_private.dispenserWaterValvePlugin, dataModel);
   AugerMotorPlugin_Init(&instance->_private.augerMotorPlugin, dataModel);
   FourDoorStaggeredHeater_Init(&instance->_private.staggeredHeaterPlugin, dataModel);
   RecessHeaterPlugin_Init(&instance->_private.recessHeaterPlugin, dataModel);
   FourDoorBooleanRequestStatusUpdaterPlugin_Init(&instance->_private.booleanRequestStatusUpdaterPlugin, dataModel);
   FourDoorDoorPlugin_Init(&instance->_private.fourDoorDoorPlugin, dataModel);
   FourDoorFactoryPlugin_Init(&instance->_private.factoryPlugin, dataModel);
   FeaturePanPlugin_Init(&instance->_private.FeaturePanPlugin, dataModel);
   IceCabinetPlugin_Init(&instance->_private.IceCabinetPlugin, dataModel);

   // Activate Subscription Client To Enable DoorBoard Communication
   DataModel_Write(dataModel, Erd_DoorBoardEnable, set);
}
