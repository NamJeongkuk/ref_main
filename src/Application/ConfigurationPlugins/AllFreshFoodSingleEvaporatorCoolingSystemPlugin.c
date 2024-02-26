/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AllFreshFoodSingleEvaporatorCoolingSystemPlugin.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

static const CoolingSystemRequestVotePair_t votingPairs[] = {
   { Erd_CondenserFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_CompressorSpeed_CoolingSystemOffVote, CompressorSpeed_Off },
   { Erd_RecessHeater_CoolingSystemOffVote, PercentageDutyCycle_Min },
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

void AllFreshFoodSingleEvaporatorCoolingSystemPlugin_Init(
   AllFreshFoodSingleEvaporatorCoolingSystemPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   // remove when grid is plugged in
   DataModel_Write(
      dataModel,
      Erd_GridPluginReady,
      set);

   I_ConstArrayMap_t *constArrayMapInterface = ConstArrayMap_SingleDoorSingleEvap_Init(&instance->_private.coolingStateBasedGridVotesTable);
   DataModelErdPointerAccess_Write(dataModel, Erd_CoolingStatesGridVotesConstArrayMapInterface, constArrayMapInterface);

   SingleDoorFreshFoodSetpointPlugin_Init(&instance->_private.singleDoorFreshFoodSetpointPlugin, dataModel);
   SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin_Init(
      &instance->_private.singleDoorFreshFoodCoolingSystemSensorFilteringPlugin,
      dataModel);

   AmbientTemperatureAndHumidityPlugin_Init(&instance->_private.ambientTemperatureAndHumidityPlugin, dataModel);

   CompressorPlugin_Init(&instance->_private.compressorPlugin, dataModel);
   SingleDoorFreshFoodFanPlugin_Init(&instance->_private.singleDoorFreshFoodFanPlugin, dataModel);

   CoolingSystemRequestHandler_Init(&instance->_private.coolingSystemRequestHandler, dataModel, &coolingSystemRequestHandlerConfig);

   AllFreshFoodDefrostPlugin_Init(&instance->_private.allFreshFoodDefrostPlugin, dataModel);
}
