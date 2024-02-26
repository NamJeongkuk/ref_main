/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODCONFIGURATIONPLUGIN_H
#define SINGLEDOORFRESHFOODCONFIGURATIONPLUGIN_H

#include "AllFreshFoodSingleEvaporatorCoolingSystemPlugin.h"
#include "DispenserWaterValvePlugin.h"
#include "I_DataModel.h"
#include "IsolationWaterValvePlugin.h"
#include "RecessHeaterPlugin.h"
#include "SingleDoorFreshFoodDoorPlugin.h"

typedef struct
{
   struct
   {
      AllFreshFoodSingleEvaporatorCoolingSystemPlugin_t coolingSystemPlugin;
      IsolationWaterValvePlugin_t isolationWaterValvePlugin;
      DispenserWaterValvePlugin_t dispenserWaterValvePlugin;
      RecessHeaterPlugin_t recessHeaterPlugin;
      SingleDoorFreshFoodDoorPlugin_t singleDoorFreshFoodDoorPlugin;
   } _private;
} SingleDoorFreshFoodConfigurationPlugin_t;

void SingleDoorFreshFoodConfigurationPlugin_Init(SingleDoorFreshFoodConfigurationPlugin_t *instance, I_DataModel_t *dataModel);

#endif
