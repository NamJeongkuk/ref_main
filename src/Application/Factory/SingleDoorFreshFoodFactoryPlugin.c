/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreshFoodFactoryPlugin.h"
#include "FactoryMode.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"
#include "WaterValveVotedState.h"

static const ErdOffValuePair_t factoryVotePairs[] = {
   { Erd_CompressorSpeed_FactoryVote, CompressorSpeed_Off },
   { Erd_CondenserFanSpeed_FactoryServiceVote, FanSpeed_Off },
   { Erd_FreshFoodDefrostHeater_FactoryVote, HeaterState_Off },
   { Erd_IsolationWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_DispenserWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_DisableMinimumCompressorTimes_FactoryVote, true },
   { Erd_FreshFoodEvapFanSpeed_FactoryServiceVote, FanSpeed_Off },
   // Light vote ERDs should not be configured here
   // It should be configured in lightVoteErds
};

static const Erd_t lightVoteErds[] = {
   Erd_FreshFoodBackWallLight_FactoryVote,
   Erd_FreshFoodTopLight_FactoryVote
};

static const ErdList_t lightVoteErdList = {
   .erds = lightVoteErds,
   .numberOfErds = NUM_ELEMENTS(lightVoteErds)
};

static const FactoryModeConfiguration_t factoryModeConfig = {
   .factoryModeTimeErd = Erd_FactoryModeEnableRequestInMinutes,
   .broadcastResetRequestErd = Erd_BroadcastResetRequestSignal,
   .lightVoteErdList = &lightVoteErdList,
   .erdValuePairList = NULL,
   .factoryVoteList = {
      .pairs = factoryVotePairs,
      .numberOfPairs = NUM_ELEMENTS(factoryVotePairs),
   }
};

void SingleDoorFreshFoodFactoryPlugin_Init(SingleDoorFreshFoodFactoryPlugin_t *instance, I_DataModel_t *dataModel)
{
   FactoryMode_Init(
      &instance->_private.factoryMode,
      dataModel,
      &factoryModeConfig,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));
}
