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

static struct
{
   Defrost_t defrost;
   DoorAccelerationCalculator_t doorAcceleration;
   DefrostTimerCounter_t defrostTimerCounter;
   DefrostTimerIsSatisfiedMonitor_t defrostTimerIsSatisfiedMonitor;
} instance;

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostDoorHoldoffRequestErd = Erd_DefrostDoorHoldOffRequest,
   .defrostTimerCounterRequestErd = Erd_DefrostTimerCounterRequest,
   .freezerFilteredTemperatureErd = Erd_Freezer_FilteredTemperature,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .defrostStateErd = Erd_DefrostState,
   .numberOfFzAbnormalDefrostCyclesErd = Erd_NumberofFzAbnormalDefrostCycles,
   .fzDefrostWasAbnormalErd = Erd_FzDefrostWasAbnormal,
   .fzAbnormalDefrostCycleCountErd = Erd_FzAbnormalDefrostCycleCount,
   .fzDefrostCycleCountErd = Erd_FzDefrostCycleCount,
   .timerModuleErd = Erd_TimerModule
};

static const DoorAccelerationConfig_t doorAccelerationConfig = {
   .doorAccelerationRequestErd = Erd_DoorAccelerationRequest,
   .doorAccelerationFsmStateErd = Erd_DoorAccelerationFsmState,
   .fzDoorAccelerationCountsErd = Erd_DefrostFzDoorAccelerationCount,
   .ffDoorAccelerationCountsErd = Erd_DefrostFfDoorAccelerationCount,
   .leftHandFfDoorIsOpenErd = Erd_LeftHandFfDoorIsOpen,
   .rightHandFfDoorIsOpenErd = Erd_RightHandFfDoorIsOpen,
   .doorInDoorIsOpenErd = Erd_DoorInDoorIsOpen,
   .fzDoorIsOpenErd = Erd_FzDoorIsOpen,
   .timerModuleErd = Erd_TimerModule
};

static const DefrostTimerCounterConfig_t defrostTimerCounterConfig = {
   .defrostTimerCounterFsmStateErd = Erd_DefrostTimerCounterFsmState,
   .defrostTimerCounterRequestErd = Erd_DefrostTimerCounterRequest,
   .defrostTimerCountInSecondsErd = Erd_DefrostTimerCountInSeconds,
   .doorAccelerationRequestErd = Erd_DoorAccelerationRequest,
   .ffDoorAccelerationCountsErd = Erd_DefrostFfDoorAccelerationCount,
   .fzDoorAccelerationCountsErd = Erd_DefrostFzDoorAccelerationCount,
   .defrostTimerIsSatisfiedMonitorRequestErd = Erd_DefrostTimerIsSatisfiedMonitorRequest,
   .compressorResolvedSpeedErd = Erd_CompressorSpeed_ResolvedVote,
   .compressorSpeedConfigErd = Erd_CompressorSpeedConfig,
   .sabbathModeErd = Erd_SabbathMode,
   .fzDefrostWasAbnormalErd = Erd_FzDefrostWasAbnormal,
   .maxTimeBetweenDefrostsInMinutesErd = Erd_MaxTimeBetweenDefrostsInMinutes,
   .timerModuleErd = Erd_TimerModule,
};

static const DefrostTimerIsSatisfiedMonitorConfig_t defrostTimerIsSatisfiedMonitorConfig = {
   .defrostTimerIsSatisfiedMonitorFsmStateErd = Erd_DefrostTimerIsSatisfiedMonitorFsmState,
   .defrostTimerCountInSecondsErd = Erd_DefrostTimerCountInSeconds,
   .defrostTimerIsSatisfiedMonitorRequestErd = Erd_DefrostTimerIsSatisfiedMonitorRequest,
   .defrostTimerIsSatisfiedErd = Erd_DefrostTimerIsSatisfied,
   .sabbathModeErd = Erd_SabbathMode,
   .fzDefrostWasAbnormalErd = Erd_FzDefrostWasAbnormal,
   .ffDefrostWasAbnormalErd = Erd_FfDefrostWasAbnormal,
   .demandResponseLevelErd = Erd_DemandResponseLevel,
   .maxTimeBetweenDefrostsInMinutesErd = Erd_MaxTimeBetweenDefrostsInMinutes
};

void DefrostPlugin_Init(I_DataModel_t *dataModel)
{
   DefrostStateOnCompareMatch(dataModel);

   DoorAcceleration_Init(&instance.doorAcceleration, dataModel, &doorAccelerationConfig);

   DefrostTimerCounter_Init(&instance.defrostTimerCounter, dataModel, &defrostTimerCounterConfig);

   DefrostTimerIsSatisfiedMonitor_Init(&instance.defrostTimerIsSatisfiedMonitor, dataModel, &defrostTimerIsSatisfiedMonitorConfig);

   Defrost_Init(&instance.defrost, dataModel, &defrostConfig);
}
