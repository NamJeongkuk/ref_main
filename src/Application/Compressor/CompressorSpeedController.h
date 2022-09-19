/*!
 * @file
 * @brief Controls Compressor Speed Frequency
 *
 * This module controls and manages the requests to the lower level compressor. It enforces minimum off, on, and run times
 * as well as startup conditions.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORSPEEDCONTROLLER_H
#define COMPRESSORSPEEDCONTROLLER_H

#include "Timer.h"
#include "I_DataModel.h"
#include "CompressorData.h"

typedef struct
{
   Erd_t compressorSpeedRequestErd;
   Erd_t compressorSpeedResolvedVoteErd;
} CompressorSpeedControllerConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const CompressorData_t *compressorData;
      const CompressorSpeedControllerConfiguration_t *config;
      Timer_t startupTimer;
      TimerModule_t *timerModule;
   } _private;
} CompressorSpeedController_t;

/*!
 * @param instance the compressor speed controller instance
 * @param dataModel
 * @param timerMOdule
 * @param config the compressor speed controller configuration
 */
void CompressorSpeedController_Init(
   CompressorSpeedController_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   const CompressorSpeedControllerConfiguration_t *config);

#endif
