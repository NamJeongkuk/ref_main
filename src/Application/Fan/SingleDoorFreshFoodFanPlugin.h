/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODFANPLUGIN_H
#define SINGLEDOORFRESHFOODFANPLUGIN_H

#include "FreshFoodEvapFanPlugin.h"
#include "CondenserFanPlugin.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      FreshFoodEvapFanPlugin_t freshFoodEvapFanPlugin;
      CondenserFanPlugin_t condenserFanPlugin;
   } _private;
} SingleDoorFreshFoodFanPlugin_t;

void SingleDoorFreshFoodFanPlugin_Init(
   SingleDoorFreshFoodFanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
