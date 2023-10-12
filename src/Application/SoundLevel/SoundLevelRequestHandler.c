/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SoundLevelRequestHandler.h"

enum
{
   SoundLevelRequestClearValue = 0xFF
};

static void ClearSoundLevelRequest(SoundLevelRequestHandler_t *instance)
{
   uint8_t soundLevelRequestClearValue = SoundLevelRequestClearValue;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->soundLevelRequestErd,
      &soundLevelRequestClearValue);
}

static void OnSoundLevelRequested(void *context, const void *_args)
{
   SoundLevelRequestHandler_t *instance = context;
   const uint8_t *soundLevelRequest = _args;

   if(*soundLevelRequest != SoundLevelRequestClearValue)
   {
      uint8_t numberOfSoundLevelSettings;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->numberOfSoundLevelSettingsErd,
         &numberOfSoundLevelSettings);

      if(*soundLevelRequest < numberOfSoundLevelSettings)
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->soundLevelStateErd,
            soundLevelRequest);
      }

      ClearSoundLevelRequest(instance);
   }
}

void SoundLevelRequestHandler_Init(
   SoundLevelRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const SoundLevelRequestHandlerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   ClearSoundLevelRequest(instance);

   EventSubscription_Init(
      &instance->_private.soundLevelRequestSubscription,
      instance,
      OnSoundLevelRequested);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->soundLevelRequestErd,
      &instance->_private.soundLevelRequestSubscription);
}
