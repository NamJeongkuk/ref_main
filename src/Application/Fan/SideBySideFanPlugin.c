/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideFanPlugin.h"

void SideBySideFanPlugin_Init(
   SideBySideFanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   FreezerEvapFanPlugin_Init(&instance->_private.freezerEvapFanPlugin, dataModel);
   CondenserFanPlugin_Init(&instance->_private.condenserFanPlugin, dataModel);
}
