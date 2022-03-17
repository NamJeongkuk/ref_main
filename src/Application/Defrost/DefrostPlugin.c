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
#include "SystemErds.h"

static struct
{
   Defrost_t defrost;
   DoorAccelerationCalculator_t doorAcceleration;
} instance;

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostDoorHoldoffRequestErd = Erd_DefrostDoorHoldOffRequest,
   .defrostTimerRequestErd = Erd_DefrostTimerRequest,
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

void DefrostPlugin_Init(I_DataModel_t *dataModel)
{
   DefrostStateOnCompareMatch(dataModel);

   DoorAcceleration_Init(&instance.doorAcceleration, dataModel, &doorAccelerationConfig);

   Defrost_Init(&instance.defrost, dataModel, &defrostConfig);
}
