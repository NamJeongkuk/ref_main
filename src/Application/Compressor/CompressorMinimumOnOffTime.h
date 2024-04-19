/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORMINIMUMONOFFTIME_H
#define COMPRESSORMINIMUMONOFFTIME_H

#include "I_DataModel.h"
#include "CompressorTimes.h"
#include "EventSubscription.h"
#include "Erd.h"
#include "Timer.h"
#include "CompressorVotedSpeed.h"

typedef struct
{
   Erd_t resolvedVoteErd; // CompressorVotedSpeed_t
   Erd_t minimumOnOffTimeVoteErd; // CompressorVotedSpeed_t
} CompressorMinimumOnOffTimeConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t resolvedVoteSubscription;
      Timer_t timer;
      I_DataModel_t *dataModel;
      const CompressorTimes_t *timesData;
      const CompressorMinimumOnOffTimeConfiguration_t *config;
      CompressorSpeed_t previousCompressorSpeed;
   } _private;
} CompressorMinimumOnOffTime_t;

void CompressorMinimumOnOffTime_Init(
   CompressorMinimumOnOffTime_t *instance,
   I_DataModel_t *dataModel,
   const CompressorTimes_t *timesData,
   const CompressorMinimumOnOffTimeConfiguration_t *config);

#endif
