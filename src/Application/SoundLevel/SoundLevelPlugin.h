/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SOUNDLEVELPLUGIN_H
#define SOUNDLEVELPLUGIN_H

#include "I_DataModel.h"
#include "SoundLevelRequestHandler.h"

typedef struct
{
   struct
   {
      SoundLevelRequestHandler_t soundLevelRequestHandler;
   } _private;
} SoundLevelPlugin_t;

void SoundLevelPlugin_Init(SoundLevelPlugin_t *instance, I_DataModel_t *dataModel);

#endif
