/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODADJUSTEDSETPOINTPLUGIN_H
#define SINGLEDOORFRESHFOODADJUSTEDSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "FreshFoodAdjustedSetpointPlugin.h"

typedef struct
{
   struct
   {
      FreshFoodAdjustedSetpointPlugin_t freshFoodAdjustedSetpointPlugin;
   } _private;
} SingleDoorFreshFoodAdjustedSetpointPlugin_t;

void SingleDoorFreshFoodAdjustedSetpointPlugin_Init(
   SingleDoorFreshFoodAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
