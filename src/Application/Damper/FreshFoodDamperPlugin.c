/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreshFoodDamperPlugin.h"

void FreshFoodDamperPlugin_Init(FreshFoodDamperPlugin_t *instance, I_DataModel_t *dataModel)
{
   FreshFoodDamperMotorPlugin_Init(&instance->_private.freshFoodDamperMotorPlugin, dataModel);
   FreshFoodDamperHeaterVotingFrameworkPlugin_Init(&instance->_private.freshFoodDamperHeaterVotingFrameworkPlugin, dataModel);
}
