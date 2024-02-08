/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERADJUSTEDSETPOINTPLUGIN_H
#define SINGLEDOORFREEZERADJUSTEDSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "FreezerAdjustedSetpointPlugin.h"
#include "FreshFoodAdjustedSetpointPlugin.h"

typedef struct
{
   struct
   {
      FreezerAdjustedSetpointPlugin_t freezerAdjustedSetpointPlugin;
   } _private;
} SingleDoorFreezerAdjustedSetpointPlugin_t;

void SingleDoorFreezerAdjustedSetpointPlugin_Init(
   SingleDoorFreezerAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
