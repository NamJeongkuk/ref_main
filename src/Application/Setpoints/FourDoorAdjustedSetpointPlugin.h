/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORADJUSTEDSETPOINTPLUGIN_H
#define FOURDOORADJUSTEDSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "ConvertibleCompartmentAdjustedSetpointPlugin.h"
#include "FreezerAdjustedSetpointPlugin.h"
#include "FreshFoodAdjustedSetpointPlugin.h"

typedef struct
{
   struct
   {
      FreezerAdjustedSetpointPlugin_t freezerAdjustedSetpointPlugin;
      FreshFoodAdjustedSetpointPlugin_t freshFoodAdjustedSetpointPlugin;
      ConvertibleCompartmentAdjustedSetpointPlugin_t convertibleCompartmentAdjustedSetpointPlugin;
   } _private;
} FourDoorAdjustedSetpointPlugin_t;

void FourDoorAdjustedSetpointPlugin_Init(
   FourDoorAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
