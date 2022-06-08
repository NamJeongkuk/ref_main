/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostPlugin.h"
#include "Defrost.h"
#include "DefrostStateOnCompareMatch.h"
#include "SystemErds.h"
#include "DefrostDoorHoldoffTimer.h"
#include "DefrostParameterSelector.h"
#include "DefrostHeaterMaxOnTime.h"
#include "FreshFoodOnlyDefrostArbitrator.h"

static struct
{
   Defrost_t defrost;
   DefrostDoorHoldoffTimer_t doorHoldoffTimer;
   DefrostHeaterMaxOnTime_t defrostHeaterMaxOnTime;
   FreshFoodOnlyDefrostArbitrator_t freshFoodOnlyDefrostArbitrator;
} instance;

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostDoorHoldoffRequestErd = Erd_DefrostDoorHoldOffRequest,
   .freezerFilteredTemperatureResolvedErd = Erd_Freezer_FilteredTemperatureResolved,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .defrostStateErd = Erd_DefrostState,
   .numberOfFreezerAbnormalDefrostCyclesErd = Erd_NumberofFreezerAbnormalDefrostCycles,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freezerAbnormalDefrostCycleCountErd = Erd_FreezerAbnormalDefrostCycleCount,
   .freezerDefrostCycleCountErd = Erd_FreezerDefrostCycleCount,
   .freshFoodDefrostHeaterDefrostVoteErd = Erd_FreshFoodDefrostHeater_DefrostVote,
   .freezerDefrostHeaterDefrostVoteErd = Erd_FreezerDefrostHeater_DefrostVote,
   .sealedSystemValvePositionErd = Erd_SealedSystemValvePosition,
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd = Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
   .iceCabinetFanDefrostVoteErd = Erd_IceCabinetFanSpeed_DefrostVote,
   .freezerSetpointDefrostVoteErd = Erd_FreezerSetpoint_DefrostVote,
   .freezerResolvedSetpointErd = Erd_FreezerSetpoint_ResolvedVote,
   .sealedSystemValvePositionDefrostVoteErd = Erd_ValvePosition_DefrostVote,
   .defrostIsFreshFoodOnlyErd = Erd_DefrostIsFreshFoodOnly,
   .freshFoodSetpointDefrostVoteErd = Erd_FreshFoodSetpoint_DefrostVote,
   .extendDefrostSignalErd = Erd_ExtendDefrostSignal,
   .compressorStateErd = Erd_CompressorState,
   .defrostMaxHoldoffMetErd = Erd_DefrostMaxHoldoffMet,
   .defrostPrechillRunCounterInMinutesErd = Erd_DefrostPrechillRunCounterInMinutes,
   .timeInMinutesInValvePositionBErd = Erd_TimeInMinutesInValvePositionB,
   .prechillTimeMetErd = Erd_PrechillTimeMet,
   .maxPrechillTimeInMinutesErd = Erd_MaxPrechillTimeInMinutes,
   .freezerEvaporatorFilteredTemperatureErd = Erd_FreezerEvap_FilteredTemperatureResolved,
   .adjustedFreezerSetpointErd = Erd_Freezer_AdjustedSetpoint,
   .doorHoldoffTimeIsSatisfiedErd = Erd_DefrostDoorHoldoffTimeSatisfied,
   .noFreezeLimitIsActiveErd = Erd_NoFreezeLimitIsActive,
   .freezerEvapFanDefrostVoteErd = Erd_FreezerEvapFanSpeed_DefrostVote,
   .freshFoodEvapFanDefrostVoteErd = Erd_FreshFoodEvapFanSpeed_DefrostVote,
   .timerModuleErd = Erd_TimerModule
};

DefrostDoorHoldoffTimerConfiguration_t doorHoldoffTimerConfiguration = {
   .allFreshFoodDoorsAreClosedState = Erd_AllFreshFoodDoorsAreClosed,
   .freezerDoorOpenState = Erd_FreezerDoorIsOpen,
   .convertibleCompartmentDoorOpenState = Erd_ConvertibleCompartmentDoorIsOpen,
   .doorHoldoffRequest = Erd_DefrostDoorHoldOffRequest,
   .doorHoldoffSatisfied = Erd_DefrostDoorHoldoffTimeSatisfied,
   .freshFoodOnlyDefrost = Erd_DefrostIsFreshFoodOnly,
   .doorHoldoffTimerFsmState = Erd_DefrostDoorHoldoffTimerFsmState
};

static const DefrostHeaterMaxOnTimeConfiguration_t defrostHeaterMaxOnTimeConfig = {
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .freshFoodEvaporatorThermistorIsValidErd = Erd_FreshFoodEvaporatorThermistorIsValid,
   .convertibleCompartmentEvaporatorThermistorIsValidErd = Erd_ConvertibleCompartmentEvaporatorThermistorIsValid,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .freshFoodDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes
};

static const FreshFoodOnlyDefrostArbitratorConfiguration_t freshFoodOnlyDefrostArbitratorConfig = {
   .numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd = Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .defrostIsFreshFoodOnlyErd = Erd_DefrostIsFreshFoodOnly
};

void DefrostPlugin_Init(I_DataModel_t *dataModel)
{
   DefrostParameterSelector_Init(dataModel);

   DefrostStateOnCompareMatch(dataModel);

   DefrostHeaterMaxOnTime_Init(&instance.defrostHeaterMaxOnTime, dataModel, &defrostHeaterMaxOnTimeConfig);

   FreshFoodOnlyDefrostArbitrator_Init(&instance.freshFoodOnlyDefrostArbitrator, dataModel, &freshFoodOnlyDefrostArbitratorConfig);

   DefrostDoorHoldoffTimer_Init(&instance.doorHoldoffTimer,
      &doorHoldoffTimerConfiguration,
      dataModel);

   Defrost_Init(&instance.defrost, dataModel, &defrostConfig);
}
