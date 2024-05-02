/*!
 * @file
 * @brief This module wraps the resolved vote in order to delay it if necessary during Sabbath Mode.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SABBATHDELAYHANDLER_H
#define SABBATHDELAYHANDLER_H

#include "Erd.h"
#include "EventSubscription.h"
#include "I_DataModel.h"
#include "Timer.h"
#include "CompressorVotedSpeed.h"

typedef struct
{
   Erd_t compressorResolvedVote; // CompressorVotedSpeed_t
   Erd_t loadResolvedVote;
   Erd_t loadResolvedVoteWithSabbathDelay;
} SabbathDelayHandlerConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubcription;
      Timer_t timer;
      const SabbathDelayHandlerConfiguration_t *config;
      I_DataModel_t *dataModel;
      CompressorSpeed_t previousCompressorSpeed;
   } _private;
} SabbathDelayHandler_t;

void SabbathDelayHandler_Init(
   SabbathDelayHandler_t *instance,
   I_DataModel_t *dataModel,
   const SabbathDelayHandlerConfiguration_t *config);

#endif
