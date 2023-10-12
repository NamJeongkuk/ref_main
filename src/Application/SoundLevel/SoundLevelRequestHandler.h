/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SOUNDLEVELREQUESTHANDLER_H
#define SOUNDLEVELREQUESTHANDLER_H

#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t soundLevelRequestErd; // uint8_t
   Erd_t soundLevelStateErd; // uint8_t
   Erd_t numberOfSoundLevelSettingsErd; // uint8_t
} SoundLevelRequestHandlerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t soundLevelRequestSubscription;
      const SoundLevelRequestHandlerConfig_t *config;
   } _private;
} SoundLevelRequestHandler_t;

void SoundLevelRequestHandler_Init(
   SoundLevelRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const SoundLevelRequestHandlerConfig_t *config);

#endif
