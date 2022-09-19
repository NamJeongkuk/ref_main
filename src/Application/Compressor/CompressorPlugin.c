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
#include "DataModelErdPointerAccess.h"

static const CompressorSpeedControllerConfiguration_t compressorSpeedControllerConfig = {
   .compressorSpeedRequestErd = Erd_CompressorSpeedRequestInHz,
   .compressorSpeedResolvedVoteErd = Erd_CompressorSpeed_ResolvedVote
};

static CompressorSpeedController_t compressorSpeedControllerInstance;

void CompressorPlugin_Init(I_DataModel_t *dataModel)
{
   CompressorIsOn_Init(dataModel);
   CompressorSpeedController_Init(
      &compressorSpeedControllerInstance,
      dataModel,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &compressorSpeedControllerConfig);
}
