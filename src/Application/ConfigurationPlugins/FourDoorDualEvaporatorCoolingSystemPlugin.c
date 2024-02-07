/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorDualEvaporatorCoolingSystemPlugin.h"
#include "SystemErds.h"

static const CoolingSystemRequestVotePair_t votingPairs[] = {
   { Erd_CondenserFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_FreezerEvapFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_FreezerDefrostHeater_CoolingSystemOffVote, HeaterState_Off },
   { Erd_FreshFoodEvapFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_FreshFoodDefrostHeater_CoolingSystemOffVote, HeaterState_Off },
   { Erd_CompressorSpeed_CoolingSystemOffVote, CompressorSpeed_Off },
   { Erd_RecessHeater_CoolingSystemOffVote, PercentageDutyCycle_Min }
};

static const CoolingSystemRequestVoteList_t coolingSystemRequestVoteList = {
   .pairs = votingPairs,
   .numberOfPairs = NUM_ELEMENTS(votingPairs)
};

static const CoolingSystemRequestHandlerConfiguration_t coolingSystemRequestHandlerConfig = {
   .requestErd = Erd_CoolingOffRequest,
   .statusErd = Erd_CoolingOffStatus,
   .disableDefrostErd = Erd_DisableDefrost,
   .turboCoolOnOffRequestErd = Erd_TurboCoolOnOffRequest,
   .turboFreezeOnOffRequestErd = Erd_TurboFreezeOnOffRequest,
   .coolingSystemRequestVoteList = coolingSystemRequestVoteList
};

void FourDoorDualEvaporatorCoolingSystemPlugin_Init(FourDoorDualEvaporatorCoolingSystemPlugin_t *instance, I_DataModel_t *dataModel)
{
   FourDoorSetpointPlugin_Init(&instance->_private.fourDoorSetpointPlugin, dataModel);
   FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_Init(
      &instance->_private.fourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin,
      dataModel);

   AmbientTemperatureAndHumidityPlugin_Init(&instance->_private.ambientTemperatureAndHumidityPlugin, dataModel);

   CompressorPlugin_Init(&instance->_private.compressorPlugin, dataModel);
   FourDoorFanPlugin_Init(&instance->_private.fourDoorFanPlugin, dataModel);
   ThreeWaySealedSystemValvePlugin_Init(&instance->_private.threeWaySealedSystemValvePlugin, dataModel);

   CoolingSystemRequestHandler_Init(&instance->_private.coolingSystemRequestHandler, dataModel, &coolingSystemRequestHandlerConfig);
}
