/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorPlugin.h"
#include "CompressorIsOn.h"
#include "SystemErds.h"

static const CompressorSpeedDriverConfig_t compressorSpeedDriverConfig = {
   .compressorRelayErd = Erd_CompressorRelay,
   .compressorFrequencyErd = Erd_CompressorInverterDriver,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointZoneErd = Erd_FreezerSetpointZone,
   .compressorControllerSpeedErd = Erd_CompressorControllerSpeed
};

static const CompressorSpeedControllerConfiguration_t compressorSpeedControllerConfig = {
   .compressorStateErd = Erd_CompressorState,
   .compressorSpeedRequestErd = Erd_CompressorControllerSpeed,
   .compressorSpeedResolvedVoteErd = Erd_CompressorSpeed_ResolvedVote,
   .disableMinimumTimeRequestErd = Erd_DisableMinimumCompressorTimes
};

void CompressorPlugin_Init(CompressorPlugin_t *instance, I_DataModel_t *dataModel)
{
   CompressorIsOn_Init(dataModel);

   CompressorSpeedDriver_Init(
      &instance->_private.compressorSpeedDriver,
      dataModel,
      &compressorSpeedDriverConfig);

   CompressorSpeedController_Init(
      &instance->_private.compressorSpeedController,
      dataModel,
      &compressorSpeedControllerConfig);
}
