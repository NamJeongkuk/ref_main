/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorPlugin.h"
#include "CompressorIsOn.h"
#include "CompressorSpeedController.h"
#include "SystemErds.h"

static const CompressorSpeedControllerConfiguration_t compressorSpeedControllerConfig = {
   .compressorStateErd = Erd_CompressorState,
   .compressorSpeedRequestErd = Erd_CompressorControllerSpeed,
   .compressorSpeedResolvedVoteErd = Erd_CompressorSpeed_ResolvedVote,
   .disableMinimumTimeRequestErd = Erd_DisableMinimumCompressorTimes
};

static CompressorSpeedController_t compressorSpeedControllerInstance;

void CompressorPlugin_Init(I_DataModel_t *dataModel)
{
   CompressorIsOn_Init(dataModel);
   CompressorSpeedController_Init(
      &compressorSpeedControllerInstance,
      dataModel,
      &compressorSpeedControllerConfig);
}
