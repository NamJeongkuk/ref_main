/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorAdjustedSetpointPlugin.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

void FourDoorAdjustedSetpointPlugin_Init(FourDoorAdjustedSetpointPlugin_t *instance, I_DataModel_t *dataModel)
{
   DataModel_Write(dataModel, Erd_AdjustedSetpointPluginReady, set);

   FreezerAdjustedSetpointPlugin_Init(&instance->_private.freezerAdjustedSetpointPlugin, dataModel);
   FreshFoodAdjustedSetpointPlugin_Init(&instance->_private.freshFoodAdjustedSetpointPlugin, dataModel);
   ConvertibleCompartmentAdjustedSetpointPlugin_Init(&instance->_private.convertibleCompartmentAdjustedSetpointPlugin, dataModel);
}
