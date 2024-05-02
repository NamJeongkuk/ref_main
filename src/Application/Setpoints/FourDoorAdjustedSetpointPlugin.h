/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORADJUSTEDSETPOINTPLUGIN_H
#define FOURDOORADJUSTEDSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "FeaturePanAdjustedSetpointPlugin.h"
#include "FreezerAdjustedSetpointPlugin.h"
#include "FreshFoodAdjustedSetpointPlugin.h"
#include "IceCabinetAdjustedSetpointPlugin.h"

typedef struct
{
   struct
   {
      FreezerAdjustedSetpointPlugin_t freezerAdjustedSetpointPlugin;
      FreshFoodAdjustedSetpointPlugin_t freshFoodAdjustedSetpointPlugin;
      FeaturePanAdjustedSetpointPlugin_t featurePanAdjustedSetpointPlugin;
      IceCabinetAdjustedSetpointPlugin_t iceCabinetAdjustedSetpointPlugin;
   } _private;
} FourDoorAdjustedSetpointPlugin_t;

void FourDoorAdjustedSetpointPlugin_Init(
   FourDoorAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
