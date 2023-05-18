/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideFactoryPlugin.h"
#include "FactoryMode.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"
#include "WaterValveVotedState.h"
#include "TwistTrayIceMakerMotorAction.h"

static const FactoryVotePair_t factoryVotePairs[] = {
   { Erd_CompressorSpeed_FactoryVote, CompressorSpeed_Off },
   { Erd_CondenserFanSpeed_FactoryVote, FanSpeed_Off },
   { Erd_FreezerEvapFanSpeed_FactoryVote, FanSpeed_Off },
   { Erd_FreshFoodDefrostHeater_FactoryVote, HeaterState_Off },
   { Erd_FreshFoodDamperPosition_FactoryVote, DamperPosition_Closed },
   { Erd_FreshFoodDamperHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_FillTubeHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_FreezerDefrostHeater_FactoryVote, HeaterState_Off },
   { Erd_AluminumMoldIceMakerWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_AluminumMoldIceMakerHeaterRelay_FactoryVote, HeaterState_Off },
   { Erd_AluminumMoldIceMakerRakeMotor_FactoryVote, MotorState_Off },
   { Erd_TwistTrayIceMakerWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_TwistTrayIceMakerMotor_FactoryVote, TwistTrayIceMakerMotorAction_RunHomingRoutine },
   { Erd_IsolationWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_AugerMotor_FactoryVote, AugerMotorIceType_Off },
   { Erd_DispenserWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_FreshFoodBackWallLight_FactoryVote, PwmDutyCycle_Min },
   { Erd_FreshFoodTopLight_FactoryVote, PwmDutyCycle_Min },
   { Erd_FreezerBackWallLight_FactoryVote, PwmDutyCycle_Min },
   { Erd_FreezerTopLight_FactoryVote, PwmDutyCycle_Min }
};

static const FactoryVoteList_t factoryVoteList = {
   .pairs = factoryVotePairs,
   .numberOfPairs = NUM_ELEMENTS(factoryVotePairs)
};

static const FactoryModeConfiguration_t factoryModeConfig = {
   .factoryModeTimeErd = Erd_FactoryModeEnableRequestInMinutes,
   .disableMinimumCompressorTimesErd = Erd_DisableMinimumCompressorTimes,
   .resetErd = Erd_Reset,
   .factoryVoteList = factoryVoteList
};

void SideBySideFactoryPlugin_Init(SideBySideFactoryPlugin_t *instance, I_DataModel_t *dataModel)
{
   FactoryMode_Init(&instance->_private.factoryMode,
      dataModel,
      &factoryModeConfig,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));
}
