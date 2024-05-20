/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorFactoryPlugin.h"
#include "FactoryMode.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"
#include "WaterValveVotedState.h"

static const ErdOffValuePair_t factoryVotePairs[] = {
   { Erd_CompressorSpeed_FactoryVote, CompressorSpeed_Off },
   { Erd_CondenserFanSpeed_FactoryServiceVote, FanSpeed_Off },
   { Erd_FreezerEvapFanSpeed_FactoryServiceVote, FanSpeed_Off },
   { Erd_FreshFoodDefrostHeater_FactoryVote, HeaterState_Off },
   { Erd_IceMaker0_FillTubeHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_IceMaker1_FillTubeHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_IceMaker2_FillTubeHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_FreezerDefrostHeater_FactoryVote, HeaterState_Off },
   { Erd_IceMaker0_WaterValve_FactoryVote, WaterValveState_Off },
   { Erd_IceMaker1_WaterValve_FactoryVote, WaterValveState_Off },
   { Erd_IceMaker2_WaterValve_FactoryVote, WaterValveState_Off },
   { Erd_IceMaker0_HeaterRelay_FactoryVote, HeaterState_Off },
   { Erd_IceMaker1_HeaterRelay_FactoryVote, HeaterState_Off },
   { Erd_IceMaker2_HeaterRelay_FactoryVote, HeaterState_Off },
   { Erd_IceMaker0_RakeMotor_FactoryVote, IceMakerMotorState_Off },
   { Erd_IceMaker1_RakeMotor_FactoryVote, IceMakerMotorState_Off },
   { Erd_IceMaker2_RakeMotor_FactoryVote, IceMakerMotorState_Off },
   { Erd_IceMaker0_TwistMotor_FactoryVote, IceMakerMotorState_Homing },
   { Erd_IceMaker1_TwistMotor_FactoryVote, IceMakerMotorState_Homing },
   { Erd_IceMaker2_TwistMotor_FactoryVote, IceMakerMotorState_Homing },
   { Erd_IsolationWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_DispenserWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_FreshFoodEvapFanSpeed_FactoryServiceVote, FanSpeed_Off },
   { Erd_ConvertibleCompartmentDamperPosition_FactoryVote, DamperPosition_Closed },
   { Erd_ConvertibleCompartmentHeater_FactoryVote, HeaterState_Off },
   { Erd_ConvertibleCompartmentDefrostHeater_FactoryVote, HeaterState_Off },
   { Erd_SealedSystemValvePosition_FactoryVote, SealedSystemValvePosition_D },
   { Erd_DeliDamperPosition_FactoryVote, DamperPosition_Closed },
   { Erd_DeliPanHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_IceCabinetFanSpeed_FactoryServiceVote, FanSpeed_Off },
   { Erd_IceCabinetGasketHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_DuctDoorHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_IceDuctPortHeater_FactoryVote, PercentageDutyCycle_Min }
   // Light vote ERDs should not be configured here
   // It should be configured in lightVoteErds
};

static const ErdOffValuePair_t ErdValuePair[] = {
   { Erd_SealedSystemValveHomingRequest, true }
};

static const FactoryModeList_t nonVotingPairsList = {
   .pairs = ErdValuePair,
   .numberOfPairs = NUM_ELEMENTS(ErdValuePair)
};

static const Erd_t lightVoteErds[] = {
   Erd_FreshFoodBackWallLight_FactoryVote,
   Erd_FreshFoodTopLight_FactoryVote,
   Erd_FreezerTopLight_FactoryVote
};

static const ErdList_t lightVoteErdList = {
   .erds = lightVoteErds,
   .numberOfErds = NUM_ELEMENTS(lightVoteErds)
};

static const FactoryModeConfiguration_t factoryModeConfig = {
   .factoryModeTimeErd = Erd_FactoryModeEnableRequestInMinutes,
   .broadcastResetRequestErd = Erd_BroadcastResetRequestSignal,
   .lightVoteErdList = &lightVoteErdList,
   .erdValuePairList = &nonVotingPairsList,
   .factoryVoteList = {
      .pairs = factoryVotePairs,
      .numberOfPairs = NUM_ELEMENTS(factoryVotePairs),
   }
};

void FourDoorFactoryPlugin_Init(FourDoorFactoryPlugin_t *instance, I_DataModel_t *dataModel)
{
   FactoryMode_Init(
      &instance->_private.factoryMode,
      dataModel,
      &factoryModeConfig,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));
}
