/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORPLUGIN_H
#define COMPRESSORPLUGIN_H

#include "I_DataModel.h"
#include "CompressorSpeedDriver.h"
#include "ErdResolver.h"
#include "CompressorStartup.h"
#include "CompressorFaultHandler.h"
#include "CompressorMinimumOnOffTime.h"
#include "CompressorSealedSystemValveDelay.h"
#include "SabbathDelayHandler.h"

typedef struct
{
   struct
   {
      CompressorSpeedDriver_t compressorSpeedDriver;
      CompressorStartup_t compressorStartup;
      CompressorMinimumOnOffTime_t compressorMinimumOnOffTime;
      CompressorSealedSystemValveDelay_t compressorSealedSystemValveDelay;
      ErdResolver_t compressorSpeedErdResolver;
      CompressorFaultHandler_t compressorFaultHandler;
      SabbathDelayHandler_t sabbathDelayHandler;
   } _private;
} CompressorPlugin_t;

void CompressorPlugin_Init(CompressorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
