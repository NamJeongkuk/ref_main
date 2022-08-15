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
#include "DefrostReadyTimerIsSatisfied.h"
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
   DefrostReadyTimerIsSatisfied_t defrostReadyTimerIsSatisfied;
   TimeThatPrechillConditionsAreMet_t timeThatPrechillConditionsAreMet;
} instance;

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostStateErd = Erd_DefrostState,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .defrostReadyTimerIsSatisfied = Erd_DefrostReadyTimerIsSatisfied,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .maxPrechillTimeInMinutesErd = Erd_MaxPrechillTimeInMinutes,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .compressorSpeedVoteErd = Erd_CompressorSpeed_DefrostVote,
   .freezerFanSpeedVoteErd = Erd_FreezerFanSpeed_DefrostVote,
   .freshFoodDamperPositionVoteErd = Erd_FreshFoodDamperPosition_DefrostVote,
   .timerModuleErd = Erd_TimerModule
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

static DefrostReadyTimerIsSatisfiedConfiguration_t defrostReadyTimerIsSatisfiedConfig = {
   .compressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freshFoodScaledDoorAccelerationInSecondsErd = Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds,
   .freezerScaledDoorAccelerationInSecondsErd = Erd_DefrostFreezerScaledDoorAccelerationInSeconds,
   .convertibleCompartmentScaledDoorAccelerationInSecondsErd = Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds,
   .defrostReadyTimerIsSatisfiedErd = Erd_DefrostReadyTimerIsSatisfied,
   .timeInMinutesWhenDefrostReadyTimerIsSatisfiedErd = Erd_TimeInMinutesWhenDefrostReadyTimerIsSatisfied,
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

   DefrostReadyTimerIsSatisfied_Init(
      &instance.defrostReadyTimerIsSatisfied,
      dataModel,
      &defrostReadyTimerIsSatisfiedConfig);
}
