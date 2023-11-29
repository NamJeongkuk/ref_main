/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideSingleEvaporatorCoolingSystemPlugin.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

static const CoolingSystemRequestVotePair_t votingPairs[] = {
   { Erd_CondenserFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_FreezerEvapFanSpeed_CoolingSystemOffVote, FanSpeed_Off },
   { Erd_FreezerDefrostHeater_CoolingSystemOffVote, HeaterState_Off },
   { Erd_FreshFoodDamperHeater_CoolingSystemOffVote, PercentageDutyCycle_Min },
   { Erd_FreshFoodDamperPosition_CoolingSystemOffVote, DamperPosition_Open },
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

void SideBySideSingleEvaporatorCoolingSystemPlugin_Init(SideBySideSingleEvaporatorCoolingSystemPlugin_t *instance, I_DataModel_t *dataModel)
{
   I_ConstArrayMap_t *constArrayMapInterface = ConstArrayMap_SideBySideSingleEvap_Init(&instance->_private.coolingStateBasedGridVotesTable);
   DataModelErdPointerAccess_Write(dataModel, Erd_CoolingStatesGridVotesConstArrayMapInterface, constArrayMapInterface);

   SideBySideSetpointPlugin_Init(&instance->_private.sideBySideSetpointPlugin, dataModel);
   SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin_Init(&instance->_private.sensorFilteringPlugin, dataModel);

   AmbientTemperatureAndHumidityPlugin_Init(&instance->_private.ambientTemperaturePlugin, dataModel);

   GridPlugin_Init(&instance->_private.gridPlugin, dataModel);
   CompressorPlugin_Init(&instance->_private.compressorPlugin, dataModel);

   SideBySideDefrostPlugin_Init(&instance->_private.defrostPlugin, dataModel);
   SideBySideParallelMotorControlPlugin_Init(&instance->_private.sideBySideParallelMotorControlPlugin, dataModel);
   FreshFoodDamperPlugin_Init(&instance->_private.freshFoodDamperPlugin, dataModel);
   SideBySideFanPlugin_Init(&instance->_private.sideBySideFanPlugin, dataModel);

   CoolingSystemRequestHandler_Init(&instance->_private.coolingSystemRequestHandler, dataModel, &coolingSystemRequestHandlerConfig);
   SideBySideSingleEvaporatorLoadOffDoorOpenPlugin_Init(&instance->_private.sideBySideSingleEvaporatorLoadOffDoorOpenPlugin, dataModel);
}
