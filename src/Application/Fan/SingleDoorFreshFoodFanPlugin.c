/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreshFoodFanPlugin.h"

void SingleDoorFreshFoodFanPlugin_Init(
   SingleDoorFreshFoodFanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   FreshFoodEvapFanPlugin_Init(&instance->_private.freshFoodEvapFanPlugin, dataModel);
   CondenserFanPlugin_Init(&instance->_private.condenserFanPlugin, dataModel);
}
