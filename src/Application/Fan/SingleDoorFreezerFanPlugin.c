/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerFanPlugin.h"

void SingleDoorFreezerFanPlugin_Init(
   SingleDoorFreezerFanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   FreezerEvapFanPlugin_Init(&instance->_private.freezerEvapFanPlugin, dataModel);
   CondenserFanPlugin_Init(&instance->_private.condenserFanPlugin, dataModel);
}
