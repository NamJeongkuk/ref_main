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
#include "CompressorStartup.h"
#include "SabbathFanDelayHandler.h"
#include "CompressorFaultHandler.h"

typedef struct
{
   struct
   {
      CompressorSpeedDriver_t compressorSpeedDriver;
      CompressorSpeedController_t compressorSpeedController;
      CompressorStartup_t compressorStartup;
      SabbathFanDelayHandler_t sabbathFanDelayHandler;
      ErdResolver_t compressorSpeedErdResolver;
      ErdResolver_t disableMinimumCompressorTimesErdResolver;
      CompressorFaultHandler_t compressorFaultHandler;
   } _private;
} CompressorPlugin_t;

void CompressorPlugin_Init(CompressorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
