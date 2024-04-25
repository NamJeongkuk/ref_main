/*!
 * @file
 * @brief When the compressor turns ON - the compressor and various fans need to be turned on or off for the same
 * start up time.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORSTARTUP_H
#define COMPRESSORSTARTUP_H

#include "CompressorData.h"
#include "I_DataModel.h"
#include "Timer.h"
#include "CompressorVotedSpeed.h"

typedef struct
{
   Erd_t compressorResolvedVotedSpeedErd; // CompressorVotedSpeed_t
   Erd_t condenserFanSpeedStartupVoteErd; // FanVotedSpeed_t
   Erd_t freezerEvaporatorFanSpeedStartupVoteErd; // FanVotedSpeed_t
   Erd_t freshFoodEvaporatorFanSpeedStartupVoteErd; // FanVotedSpeed_t
   Erd_t compressorSpeedStartupVoteErd; // CompressorVotedSpeed_t
   Erd_t coolingModeErd; // CoolingMode_t
} CompressorStartupConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Timer_t timer;
      EventSubscription_t compressorStateSubscription;
      const CompressorStartupConfiguration_t *configuration;
      const CompressorData_t *parametric;
      CompressorSpeed_t previousCompressorSpeed;
      uint8_t numberOfEvaporators;
   } _private;
} CompressorStartup_t;

void CompressorStartup_Init(
   CompressorStartup_t *instance,
   I_DataModel_t *dataModel,
   const CompressorData_t *parametric,
   uint8_t numberOfEvaporators,
   const CompressorStartupConfiguration_t *configuration);

#endif
