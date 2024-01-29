/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORFANPLUGIN_H
#define FOURDOORFANPLUGIN_H

#include "I_DataModel.h"
#include "FreezerEvapFanPlugin.h"
#include "CondenserFanPlugin.h"

typedef struct
{
   struct
   {
      FreezerEvapFanPlugin_t freezerEvapFanPlugin;
      CondenserFanPlugin_t condenserFanPlugin;
   } _private;
} FourDoorFanPlugin_t;

void FourDoorFanPlugin_Init(FourDoorFanPlugin_t *instance, I_DataModel_t *dataModel);

#endif
