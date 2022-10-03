/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORPLUGIN_H
#define COMPRESSORPLUGIN_H

#include "I_DataModel.h"
#include "CompressorSpeedController.h"

typedef struct
{
   struct
   {
      CompressorSpeedController_t compressorSpeedController;
   } _private;
} CompressorPlugin_t;

void CompressorPlugin_Init(CompressorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
