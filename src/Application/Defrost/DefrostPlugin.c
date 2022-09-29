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
#include "DefrostParameterSelector.h"
#include "DefrostHeaterMaxOnTime.h"
#include "FreshFoodOnlyDefrostArbitrator.h"
#include "ActivelyWaitingForDefrostOnCompareMatch.h"
#include "DefrostCompressorOnTimeCounter.h"
#include "DoorAccelerationCounter.h"
#include "ReadyToDefrost.h"
#include "TimeThatPrechillConditionsAreMet.h"
#include "FreezerFilteredTemperatureTooWarmOnPowerUp.h"
#include "uassert.h"

static struct
{
   Defrost_t defrost;
   DefrostHeaterMaxOnTime_t defrostHeaterMaxOnTime;
   FreshFoodOnlyDefrostArbitrator_t freshFoodOnlyDefrostArbitrator;
   DefrostCompressorOnTimeCounter_t defrostCompressorOnTimeCounter;
   DoorAccelerationCounter_t doorAccelerationCounter;
   ReadyToDefrost_t readyToDefrost;
   TimeThatPrechillConditionsAreMet_t timeThatPrechillConditionsAreMet;
} instance;

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostStateErd = Erd_DefrostState,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .readyToDefrost = Erd_ReadyToDefrost,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodThermistorIsValidErd = Erd_FreshFoodThermistor_IsValidResolved,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .maxPrechillTimeInMinutesErd = Erd_MaxPrechillTimeInMinutes,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .compressorSpeedVoteErd = Erd_CompressorSpeed_DefrostVote,
   .condenserFanSpeedVoteErd = Erd_CondenserFanSpeed_DefrostVote,
   .freezerFanSpeedVoteErd = Erd_FreezerFanSpeed_DefrostVote,
   .iceCabinetFanSpeedVoteErd = Erd_IceCabinetFanSpeed_DefrostVote,
   .freshFoodDamperPositionVoteErd = Erd_FreshFoodDamperPosition_DefrostVote,
   .freezerEvaporatorFilteredTemperatureResolvedErd = Erd_FreezerEvap_FilteredTemperatureResolved,
   .freezerFilteredTemperatureResolvedErd = Erd_Freezer_FilteredTemperatureResolved,
   .freshFoodFilteredTemperatureResolvedErd = Erd_FreshFood_FilteredTemperatureResolved,
   .freezerDefrostHeaterVoteErd = Erd_FreezerDefrostHeater_DefrostVote,
   .freezerDefrostCycleCountErd = Erd_FreezerDefrostCycleCount,
   .freezerAbnormalDefrostCycleCountErd = Erd_FreezerAbnormalDefrostCycleCount,
   .numberOfFreezerAbnormalDefrostCycleCountErd = Erd_NumberOfFreezerAbnormalDefrostCycles,
   .freezerDefrostHeaterOnTimeInMinutesErd = Erd_FreezerDefrostHeaterOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .timerModuleErd = Erd_TimerModule
};

static const DefrostHeaterMaxOnTimeConfiguration_t defrostHeaterMaxOnTimeConfig = {
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodEvaporatorThermistorIsValidErd = Erd_FreshFoodEvapThermistor_IsValidResolved,
   .convertibleCompartmentEvaporatorThermistorIsValidErd = Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .freshFoodDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes
};

static const FreshFoodOnlyDefrostArbitratorConfiguration_t freshFoodOnlyDefrostArbitratorConfig = {
   .numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd = Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .defrostIsFreshFoodOnlyErd = Erd_DefrostIsFreshFoodOnly,
   .defrostParameterSelectorReadyErd = Erd_DefrostParameterSelectorReady
};

static const DefrostCompressorOnTimeCounterConfiguration_t defrostCompressorOnTimeCounterConfig = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .activelyWaitingForNextDefrostErd = Erd_ActivelyWaitingForNextDefrost,
   .defrostCompressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .defrostCompressorOnTimeCounterFsmStateErd = Erd_DefrostCompressorOnTimeCounterFsmState,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .activelyWaitingForDefrostOnCompareMatchReadyErd = Erd_ActivelyWaitingForDefrostOnCompareMatchReady,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .defrostCompressorOnTimeCounterReadyErd = Erd_DefrostCompressorOnTimeCounterReady,
   .timerModuleErd = Erd_TimerModule
};

static const DoorAccelerationCounterConfiguration_t doorAccelerationCounterConfig = {
   .activelyWaitingForNextDefrostErd = Erd_ActivelyWaitingForNextDefrost,
   .doorAccelerationCounterFsmStateErd = Erd_DoorAccelerationCounterFsmState,
   .freshFoodScaledDoorAccelerationInSecondsErd = Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds,
   .freezerScaledDoorAccelerationInSecondsErd = Erd_DefrostFreezerScaledDoorAccelerationInSeconds,
   .convertibleCompartmentScaledDoorAccelerationInSecondsErd = Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds,
   .leftHandFreshFoodDoorIsOpenErd = Erd_LeftHandFreshFoodDoorIsOpen,
   .rightHandFreshFoodDoorIsOpenErd = Erd_RightHandFreshFoodDoorIsOpen,
   .doorInDoorIsOpenErd = Erd_DoorInDoorIsOpen,
   .freezerDoorIsOpenErd = Erd_FreezerDoorIsOpen,
   .convertibleCompartmentDoorIsOpenErd = Erd_ConvertibleCompartmentDoorIsOpen,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .activelyWaitingForDefrostOnCompareMatchReadyErd = Erd_ActivelyWaitingForDefrostOnCompareMatchReady,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .doorAccelerationCounterReadyErd = Erd_DoorAccelerationCounterReady,
   .timerModuleErd = Erd_TimerModule
};

static ReadyToDefrostConfiguration_t readyToDefrostConfig = {
   .compressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freshFoodScaledDoorAccelerationInSecondsErd = Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds,
   .freezerScaledDoorAccelerationInSecondsErd = Erd_DefrostFreezerScaledDoorAccelerationInSeconds,
   .convertibleCompartmentScaledDoorAccelerationInSecondsErd = Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds,
   .readyToDefrostErd = Erd_ReadyToDefrost,
   .timeInMinutesUntilReadyToDefrostErd = Erd_TimeInMinutesUntilReadyToDefrost,
   .defrostCompressorOnTimeCounterReadyErd = Erd_DefrostCompressorOnTimeCounterReady,
   .doorAccelerationCounterReadyErd = Erd_DoorAccelerationCounterReady
};

static const TimeThatPrechillConditionsAreMetConfiguration_t timeThatPrechillConditionsAreMetConfig = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .sealedSystemValvePositionResolvedVoteErd = Erd_ValvePosition_ResolvedVote,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .timerModuleErd = Erd_TimerModule
};

void DefrostPlugin_Init(I_DataModel_t *dataModel)
{
   bool sensorsReadyToBeRead;
   DataModel_Read(
      dataModel,
      Erd_SensorsReadyToBeRead,
      &sensorsReadyToBeRead);

   bool setpointResolverReady;
   DataModel_Read(
      dataModel,
      Erd_SetpointResolverReady,
      &setpointResolverReady);

   bool convertibleCompartmentStateResolverReady;
   DataModel_Read(
      dataModel,
      Erd_ConvertibleCompartmentStateResolverReady,
      &convertibleCompartmentStateResolverReady);

   bool overrideArbiterReady;
   DataModel_Read(
      dataModel,
      Erd_OverrideArbiterReady,
      &overrideArbiterReady);

   bool gridPluginReady;
   DataModel_Read(
      dataModel,
      Erd_GridPluginReady,
      &gridPluginReady);

   bool periodicNvUpdaterReady;
   DataModel_Read(
      dataModel,
      Erd_PeriodicNvUpdaterReady,
      &periodicNvUpdaterReady);

   bool sabbathPluginReady;
   DataModel_Read(
      dataModel,
      Erd_SabbathPluginReady,
      &sabbathPluginReady);

   uassert(sensorsReadyToBeRead &&
      setpointResolverReady &&
      convertibleCompartmentStateResolverReady &&
      overrideArbiterReady &&
      gridPluginReady &&
      periodicNvUpdaterReady &&
      sabbathPluginReady);

   DefrostHeaterMaxOnTime_Init(
      &instance.defrostHeaterMaxOnTime,
      dataModel,
      &defrostHeaterMaxOnTimeConfig);

   TimeThatPrechillConditionsAreMet_Init(
      &instance.timeThatPrechillConditionsAreMet,
      dataModel,
      &timeThatPrechillConditionsAreMetConfig);

   FreezerFilteredTemperatureTooWarmOnPowerUp_Init(dataModel);

   DefrostParameterSelector_Init(dataModel);

   FreshFoodOnlyDefrostArbitrator_Init(
      &instance.freshFoodOnlyDefrostArbitrator,
      dataModel,
      &freshFoodOnlyDefrostArbitratorConfig);

   DefrostStateOnCompareMatch(dataModel);

   Defrost_Init(&instance.defrost, dataModel, &defrostConfig);

   ActivelyWaitingForDefrostOnCompareMatch(dataModel);

   DefrostCompressorOnTimeCounter_Init(
      &instance.defrostCompressorOnTimeCounter,
      dataModel,
      &defrostCompressorOnTimeCounterConfig);

   DoorAccelerationCounter_Init(
      &instance.doorAccelerationCounter,
      dataModel,
      &doorAccelerationCounterConfig);

   ReadyToDefrost_Init(
      &instance.readyToDefrost,
      dataModel,
      &readyToDefrostConfig);
}
