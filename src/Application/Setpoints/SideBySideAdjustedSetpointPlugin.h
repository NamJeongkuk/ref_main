/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDEADJUSTEDSETPOINTPLUGIN_H
#define SIDEBYSIDEADJUSTEDSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "FreezerAdjustedSetpointPlugin.h"
#include "FreshFoodAdjustedSetpointPlugin.h"

typedef struct
{
   struct
   {
      FreezerAdjustedSetpointPlugin_t freezerAdjustedSetpointPlugin;
      FreshFoodAdjustedSetpointPlugin_t freshFoodAdjustedSetpointPlugin;
   } _private;
} SideBySideAdjustedSetpointPlugin_t;

void SideBySideAdjustedSetpointPlugin_Init(
   SideBySideAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
