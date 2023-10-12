/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SoundLevelPlugin.h"
#include "SystemErds.h"

static const SoundLevelRequestHandlerConfig_t soundLevelRequestHandlerConfig = {
   .soundLevelRequestErd = Erd_UiSoundLevelRequest,
   .soundLevelStateErd = Erd_UiSoundLevelState,
   .numberOfSoundLevelSettingsErd = Erd_NumberOfSoundLevelSettings
};

void SoundLevelPlugin_Init(SoundLevelPlugin_t *instance, I_DataModel_t *dataModel)
{
   SoundLevelRequestHandler_Init(
      &instance->_private.soundLevelRequestHandler,
      dataModel,
      &soundLevelRequestHandlerConfig);
}
