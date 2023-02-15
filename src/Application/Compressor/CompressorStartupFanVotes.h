/*!
 * @file
 * @brief When the compressor state changes to start up
 * (compressor will turn on at a startup speed for a startup time),
 * various fans need to be turned on or off for the same
 * start up time.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORSTARTUPFANVOTES_H
#define COMPRESSORSTARTUPFANVOTES_H

#include "CompressorData.h"
#include "I_DataModel.h"
#include "Timer.h"

typedef struct
{
   Erd_t compressorStateErd; // CompressorState_t
   Erd_t condenserFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t freezerEvaporatorFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t freshFoodEvaporatorFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t coolingModeErd; // CoolingMode_t
} CompressorStartupFanVotesConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Timer_t timer;
      EventSubscription_t compressorStateSubscription;
      const CompressorStartupFanVotesConfiguration_t *configuration;
      const CompressorData_t *compressorParametricData;
   } _private;
} CompressorStartupFanVotes_t;

void CompressorStartupFanVotes_Init(
   CompressorStartupFanVotes_t *instance,
   I_DataModel_t *dataModel,
   const CompressorStartupFanVotesConfiguration_t *configuration);

#endif
