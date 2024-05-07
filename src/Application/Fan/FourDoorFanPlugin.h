/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORFANPLUGIN_H
#define FOURDOORFANPLUGIN_H

#include "I_DataModel.h"
#include "FreshFoodEvapFanPlugin.h"
#include "FreezerEvapFanPlugin.h"
#include "CondenserFanPlugin.h"
#include "IceCabinetFanPlugin.h"

typedef struct
{
   struct
   {
      FreshFoodEvapFanPlugin_t freshFoodEvapFanPlugin;
      FreezerEvapFanPlugin_t freezerEvapFanPlugin;
      CondenserFanPlugin_t condenserFanPlugin;
      IceCabinetFanPlugin_t iceCabinetFanPlugin;
   } _private;
} FourDoorFanPlugin_t;

void FourDoorFanPlugin_Init(FourDoorFanPlugin_t *instance, I_DataModel_t *dataModel);

#endif
