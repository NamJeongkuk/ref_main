/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorFanPlugin.h"

void FourDoorFanPlugin_Init(FourDoorFanPlugin_t *instance, I_DataModel_t *dataModel)
{
   FreshFoodEvapFanPlugin_Init(&instance->_private.freshFoodEvapFanPlugin, dataModel);
   FreezerEvapFanPlugin_Init(&instance->_private.freezerEvapFanPlugin, dataModel);
   CondenserFanPlugin_Init(&instance->_private.condenserFanPlugin, dataModel);
   IceCabinetFanPlugin_Init(&instance->_private.iceCabinetFanPlugin, dataModel);
}
