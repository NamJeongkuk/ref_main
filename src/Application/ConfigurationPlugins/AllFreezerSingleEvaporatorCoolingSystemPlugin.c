/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AllFreezerSingleEvaporatorCoolingSystemPlugin.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

static const CoolingSystemRequestVotePair_t votingPairs[] = {
   { Erd_CondenserFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_FreezerEvapFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_FreezerDefrostHeater_CoolingSystemOffVote, HeaterState_Off },
   { Erd_CompressorSpeed_CoolingSystemOffVote, CompressorSpeed_Off },
   { Erd_RecessHeater_CoolingSystemOffVote, PercentageDutyCycle_Min },
};

static const CoolingSystemRequestHandlerConfiguration_t coolingSystemRequestHandlerConfig = {
   .requestErd = Erd_CoolingOffRequest,
   .statusErd = Erd_CoolingOffStatus,
   .disableDefrostErd = Erd_DisableDefrost,
   .turboCoolOnOffRequestErd = Erd_TurboCoolOnOffRequest,
   .turboFreezeOnOffRequestErd = Erd_TurboFreezeOnOffRequest,
   .coolingSystemRequestVoteList = {
      .pairs = votingPairs,
      .numberOfPairs = NUM_ELEMENTS(votingPairs),
   }
};

void AllFreezerSingleEvaporatorCoolingSystemPlugin_Init(
   AllFreezerSingleEvaporatorCoolingSystemPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   I_ConstArrayMap_t *constArrayMapInterface = ConstArrayMap_SingleDoorSingleEvap_Init(&instance->_private.coolingStateBasedGridVotesTable);
   DataModelErdPointerAccess_Write(dataModel, Erd_CoolingStatesGridVotesConstArrayMapInterface, constArrayMapInterface);

   SingleDoorFreezerSetpointPlugin_Init(&instance->_private.singleDoorFreezerSetpointPlugin, dataModel);
   SingleDoorFreezerCoolingSystemSensorFilteringPlugin_Init(
      &instance->_private.singleDoorFreezerCoolingSystemSensorFilteringPlugin,
      dataModel);

   AmbientTemperatureAndHumidityPlugin_Init(&instance->_private.ambientTemperatureAndHumidityPlugin, dataModel);

   GridPlugin_Init(&instance->_private.gridPlugin, dataModel);

   CompressorPlugin_Init(&instance->_private.compressorPlugin, dataModel);
   SingleDoorFreezerFanPlugin_Init(&instance->_private.singleDoorFreezerFanPlugin, dataModel);

   CoolingSystemRequestHandler_Init(&instance->_private.coolingSystemRequestHandler, dataModel, &coolingSystemRequestHandlerConfig);

   AllFreezerDefrostPlugin_Init(&instance->_private.allFreezerDefrostPlugin, dataModel);

   AllFreezerDamperPlugin_Init(&instance->_private.allFreezerDamperPlugin, dataModel);
}
