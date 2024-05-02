/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerFactoryPlugin.h"
#include "FactoryMode.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"
#include "WaterValveVotedState.h"

static const ErdOffValuePair_t factoryVotePairs[] = {
   { Erd_CompressorSpeed_FactoryVote, CompressorSpeed_Off },
   { Erd_CondenserFanSpeed_FactoryServiceVote, FanSpeed_Off },
   { Erd_FreezerEvapFanSpeed_FactoryServiceVote, FanSpeed_Off },
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
   { Erd_AugerMotor_FactoryVote, AugerMotorIceType_Off },
   // Light vote ERDs should not be configured here
   // It should be configured in lightVoteErds
};

static const Erd_t lightVoteErds[] = {
   Erd_FreezerBackWallLight_FactoryVote,
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
   .erdValuePairList = NULL,
   .factoryVoteList = {
      .pairs = factoryVotePairs,
      .numberOfPairs = NUM_ELEMENTS(factoryVotePairs),
   }
};

void SingleDoorFreezerFactoryPlugin_Init(SingleDoorFreezerFactoryPlugin_t *instance, I_DataModel_t *dataModel)
{
   FactoryMode_Init(
      &instance->_private.factoryMode,
      dataModel,
      &factoryModeConfig,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));
}
