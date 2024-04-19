/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "FourDoorDualEvaporatorCoolingSystemPlugin.h"

static const CoolingSystemRequestVotePair_t votingPairs[] = {
   { Erd_CondenserFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_FreezerEvapFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_FreezerDefrostHeater_CoolingSystemOffVote, HeaterState_Off },
   { Erd_FreshFoodEvapFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_FreshFoodDefrostHeater_CoolingSystemOffVote, HeaterState_Off },
   { Erd_ConvertibleCompartmentDefrostHeater_CoolingSystemOffVote, HeaterState_Off },
   { Erd_ConvertibleCompartmentDamperPosition_CoolingSystemOffVote, DamperPosition_Open },
   { Erd_DamperHeater_CoolingSystemOffVote, PercentageDutyCycle_Min },
   { Erd_CompressorSpeed_CoolingSystemOffVote, CompressorSpeed_Off },
   { Erd_RecessHeater_CoolingSystemOffVote, PercentageDutyCycle_Min }
};

static const CoolingSystemRequestHandlerConfiguration_t coolingSystemRequestHandlerConfig = {
   .requestErd = Erd_CoolingOffRequest,
   .statusErd = Erd_CoolingOffStatus,
   .disableDefrostErd = Erd_DisableDefrost,
   .turboCoolOnOffRequestErd = Erd_TurboCoolOnOffRequest,
   .turboFreezeOnOffRequestErd = Erd_TurboFreezeOnOffRequest,
   .coolingSystemRequestVoteList.pairs = votingPairs,
   .coolingSystemRequestVoteList.numberOfPairs = NUM_ELEMENTS(votingPairs),
};

void FourDoorDualEvaporatorCoolingSystemPlugin_Init(FourDoorDualEvaporatorCoolingSystemPlugin_t *instance, I_DataModel_t *dataModel)
{
   I_ConstArrayMap_t *constArrayMapInterface = ConstArrayMap_FourDoorDualEvap_Init(&instance->_private.coolingStateBasedGridVotesTable);
   DataModelErdPointerAccess_Write(dataModel, Erd_FreshFoodAndFreezerCoolingStatesGridVotesConstArrayMapInterface, constArrayMapInterface);

   FourDoorSetpointPlugin_Init(&instance->_private.fourDoorSetpointPlugin, dataModel);
   FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_Init(
      &instance->_private.fourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin,
      dataModel);

   AmbientTemperatureAndHumidityPlugin_Init(&instance->_private.ambientTemperatureAndHumidityPlugin, dataModel);

   FreshFoodAndFreezerGridPlugin_Init(&instance->_private.freshFoodAndFreezerGridPlugin, dataModel);
   CompressorPlugin_Init(&instance->_private.compressorPlugin, dataModel);
   FourDoorFanPlugin_Init(&instance->_private.fourDoorFanPlugin, dataModel);
   ThreeWaySealedSystemValvePlugin_Init(&instance->_private.threeWaySealedSystemValvePlugin, dataModel);

   ConvertibleCompartmentDamperPlugin_Init(
      &instance->_private.convertibleCompartmentDamperPlugin,
      dataModel);

   CoolingSystemRequestHandler_Init(&instance->_private.coolingSystemRequestHandler, dataModel, &coolingSystemRequestHandlerConfig);

   ConvertibleCompartmentStatePlugin_Init(&instance->_private.convertibleCompartmentStatePlugin, dataModel);
   FourDoorDualEvapDefrostPlugin_Init(
      &instance->_private.fourDoorDualEvapDefrostPlugin,
      dataModel);

   FreshFoodNonHeatedCycleDefrostPlugin_Init(&instance->_private.freshFoodNonHeatedCycleDefrostPlugin, dataModel);
}
