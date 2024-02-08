/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERFANPLUGIN_H
#define SINGLEDOORFREEZERFANPLUGIN_H

#include "CondenserFanPlugin.h"
#include "FreezerEvapFanPlugin.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      FreezerEvapFanPlugin_t freezerEvapFanPlugin;
      CondenserFanPlugin_t condenserFanPlugin;
   } _private;
} SingleDoorFreezerFanPlugin_t;

void SingleDoorFreezerFanPlugin_Init(
   SingleDoorFreezerFanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
