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
#include "CompressorSpeedDriver.h"
#include "ErdResolver.h"
#include "CompressorStartupFanVotes.h"
#include "SabbathFanDelayHandler.h"

typedef struct
{
   struct
   {
      CompressorSpeedDriver_t compressorSpeedDriver;
      CompressorSpeedController_t compressorSpeedController;
      CompressorStartupFanVotes_t compressorStartupFanVotes;
      SabbathFanDelayHandler_t sabbathFanDelayHandler;
      ErdResolver_t compressorSpeedErdResolver;
      ErdResolver_t disableMinimumCompressorTimesErdResolver;
   } _private;
} CompressorPlugin_t;

void CompressorPlugin_Init(CompressorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
