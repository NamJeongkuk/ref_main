/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDEFANPLUGIN_H
#define SIDEBYSIDEFANPLUGIN_H

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
} SideBySideFanPlugin_t;

void SideBySideFanPlugin_Init(
   SideBySideFanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
