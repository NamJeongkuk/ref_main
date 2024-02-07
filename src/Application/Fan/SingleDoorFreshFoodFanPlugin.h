/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODFANPLUGIN_H
#define SINGLEDOORFRESHFOODFANPLUGIN_H

#include "I_DataModel.h"
#include "FreshFoodEvapFanPlugin.h"

typedef struct
{
   struct
   {
      FreshFoodEvapFanPlugin_t freshFoodEvapFanPlugin;
   } _private;
} SingleDoorFreshFoodFanPlugin_t;

void SingleDoorFreshFoodFanPlugin_Init(SingleDoorFreshFoodFanPlugin_t *instance, I_DataModel_t *dataModel);

#endif
