/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODDAMPERPLUGIN_H
#define FRESHFOODDAMPERPLUGIN_H

#include "I_DataModel.h"
#include "FreshFoodDamperMotorPlugin.h"
#include "FreshFoodDamperHeaterVotingFrameworkPlugin.h"

typedef struct
{
   struct
   {
      FreshFoodDamperMotorPlugin_t freshFoodDamperMotorPlugin;
      FreshFoodDamperHeaterVotingFrameworkPlugin_t freshFoodDamperHeaterVotingFrameworkPlugin;
   } _private;
} FreshFoodDamperPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FreshFoodDamperPlugin_Init(FreshFoodDamperPlugin_t *instance, I_DataModel_t *dataModel);

#endif
