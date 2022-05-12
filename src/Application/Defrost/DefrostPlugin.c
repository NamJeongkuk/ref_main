/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostPlugin.h"
#include "Defrost.h"
#include "DefrostStateOnCompareMatch.h"
#include "DoorAcceleration.h"
#include "DefrostTimerCounter.h"
#include "DefrostTimerIsSatisfiedMonitor.h"
#include "SystemErds.h"
#include "DefrostDoorHoldoffTimer.h"
#include "DefrostHeaterMaxOnTime.h"

static struct
{
   Defrost_t defrost;
   DoorAccelerationCalculator_t doorAcceleration;
   DefrostDoorHoldoffTimer_t doorHoldoffTimer;
   DefrostTimerCounter_t defrostTimerCounter;
   DefrostTimerIsSatisfiedMonitor_t defrostTimerIsSatisfiedMonitor;
   DefrostHeaterMaxOnTime_t defrostHeaterMaxOnTime;
} instance;

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostDoorHoldoffRequestErd = Erd_DefrostDoorHoldOffRequest,
   .defrostTimerCounterRequestErd = Erd_DefrostTimerCounterRequest,
   .freezerFilteredTemperatureErd = Erd_Freezer_FilteredTemperature,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .defrostStateErd = Erd_DefrostState,
   .numberOfFreezerAbnormalDefrostCyclesErd = Erd_NumberofFreezerAbnormalDefrostCycles,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freezerAbnormalDefrostCycleCountErd = Erd_FreezerAbnormalDefrostCycleCount,
   .freezerDefrostCycleCountErd = Erd_FreezerDefrostCycleCount,
   .freshFoodDefrostHeaterDefrostVoteErd = Erd_FreshFoodDefrostHeater_DefrostVote,
   .freezerDefrostHeaterDefrostVoteErd = Erd_FreezerDefrostHeater_DefrostVote,
   .defrostTimerCounterFsmStateErd = Erd_DefrostTimerCounterFsmState,
   .sealedSystemValvePositionErd = Erd_SealedSystemValvePosition,
   .defrostTimerIsSatisfiedErd = Erd_DefrostTimerIsSatisfied,
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
   .timerModuleErd = Erd_TimerModule
};

static const DoorAccelerationConfig_t doorAccelerationConfig = {
   .doorAccelerationRequestErd = Erd_DoorAccelerationRequest,
   .doorAccelerationFsmStateErd = Erd_DoorAccelerationFsmState,
   .freezerDoorAccelerationCountsErd = Erd_DefrostFreezerDoorAccelerationCount,
   .freshFoodDoorAccelerationCountsErd = Erd_DefrostFreshFoodDoorAccelerationCount,
   .leftHandFreshFoodDoorIsOpenErd = Erd_LeftHandFreshFoodDoorIsOpen,
   .rightHandFreshFoodDoorIsOpenErd = Erd_RightHandFreshFoodDoorIsOpen,
   .doorInDoorIsOpenErd = Erd_DoorInDoorIsOpen,
   .freezerDoorIsOpenErd = Erd_FreezerDoorIsOpen,
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

static const DefrostTimerCounterConfig_t defrostTimerCounterConfig = {
   .defrostTimerCounterFsmStateErd = Erd_DefrostTimerCounterFsmState,
   .defrostTimerCounterRequestErd = Erd_DefrostTimerCounterRequest,
   .ramDefrostTimerCountInSecondsErd = Erd_DefrostTimerCountInSeconds,
   .doorAccelerationRequestErd = Erd_DoorAccelerationRequest,
   .freshFoodDoorAccelerationCountsErd = Erd_DefrostFreshFoodDoorAccelerationCount,
   .freezerDoorAccelerationCountsErd = Erd_DefrostFreezerDoorAccelerationCount,
   .defrostTimerIsSatisfiedMonitorRequestErd = Erd_DefrostTimerIsSatisfiedMonitorRequest,
   .compressorResolvedSpeedErd = Erd_CompressorSpeed_ResolvedVote,
   .compressorSpeedConfigErd = Erd_CompressorSpeedConfig,
   .sabbathModeErd = Erd_SabbathMode,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .maxTimeBetweenDefrostsInMinutesErd = Erd_MaxTimeBetweenDefrostsInMinutes,
   .eepromDefrostTimerCountInSecondsErd = Erd_Eeprom_DefrostTimerCountInSeconds,
   .timerModuleErd = Erd_TimerModule,
};

static const DefrostTimerIsSatisfiedMonitorConfig_t defrostTimerIsSatisfiedMonitorConfig = {
   .defrostTimerIsSatisfiedMonitorFsmStateErd = Erd_DefrostTimerIsSatisfiedMonitorFsmState,
   .ramDefrostTimerCountInSecondsErd = Erd_DefrostTimerCountInSeconds,
   .defrostTimerIsSatisfiedMonitorRequestErd = Erd_DefrostTimerIsSatisfiedMonitorRequest,
   .defrostTimerIsSatisfiedErd = Erd_DefrostTimerIsSatisfied,
   .sabbathModeErd = Erd_SabbathMode,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .demandResponseLevelErd = Erd_DemandResponseLevel,
   .maxTimeBetweenDefrostsInMinutesErd = Erd_MaxTimeBetweenDefrostsInMinutes
};

static const DefrostHeaterMaxOnTimeConfiguration_t defrostHeaterMaxOnTimeConfig = {
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .freshFoodEvaporatorThermistorIsValidErd = Erd_FreshFoodEvaporatorThermistorIsValid,
   .convertibleCompartmentEvaporatorThermistorIsValidErd = Erd_ConvertibleCompartmentEvaporatorThermistorIsValid,
   .convertibleCompartmentCabinetStateErd = Erd_ConvertibleCompartmentCabinetState,
   .freshFoodDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes
};

void DefrostPlugin_Init(I_DataModel_t *dataModel)
{
   DefrostStateOnCompareMatch(dataModel);

   DefrostHeaterMaxOnTime_Init(&instance.defrostHeaterMaxOnTime, dataModel, &defrostHeaterMaxOnTimeConfig);

   DoorAcceleration_Init(&instance.doorAcceleration, dataModel, &doorAccelerationConfig);

   DefrostDoorHoldoffTimer_Init(&instance.doorHoldoffTimer,
      &doorHoldoffTimerConfiguration,
      dataModel);

   DefrostTimerCounter_Init(&instance.defrostTimerCounter, dataModel, &defrostTimerCounterConfig);

   DefrostTimerIsSatisfiedMonitor_Init(&instance.defrostTimerIsSatisfiedMonitor, dataModel, &defrostTimerIsSatisfiedMonitorConfig);

   Defrost_Init(&instance.defrost, dataModel, &defrostConfig);
}
