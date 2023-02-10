/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostPlugin.h"
#include "DefrostParameterSelector.h"
#include "FreezerFilteredTemperatureTooWarmOnPowerUp.h"
#include "SystemErds.h"
#include "uassert.h"

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostStateErd = Erd_DefrostState,
   .waitingForDefrostErd = Erd_WaitingToDefrost,
   .defrostingErd = Erd_Defrosting,
   .disableDefrostErd = Erd_DisableDefrost,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .hasConvertibleCompartment = Erd_HasConvertibleCompartment,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .readyToDefrost = Erd_ReadyToDefrost,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodThermistorIsValidErd = Erd_FreshFoodThermistor_IsValidResolved,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .coolingModeErd = Erd_CoolingMode,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .disableMinimumTimeRequestErd = Erd_DisableMinimumCompressorTimes,
   .maxPrechillTimeInMinutesErd = Erd_MaxPrechillTimeInMinutes,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .compressorSpeedVoteErd = Erd_CompressorSpeed_DefrostVote,
   .condenserFanSpeedVoteErd = Erd_CondenserFanSpeed_DefrostVote,
   .freezerEvapFanSpeedVoteErd = Erd_FreezerEvapFanSpeed_DefrostVote,
   .iceCabinetFanSpeedVoteErd = Erd_IceCabinetFanSpeed_DefrostVote,
   .freshFoodDamperPositionVoteErd = Erd_FreshFoodDamperPosition_DefrostVote,
   .freezerEvaporatorFilteredTemperatureResolvedInDegFx100Erd = Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100,
   .freezerFilteredTemperatureResolvedInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .freshFoodFilteredTemperatureResolvedInDegFx100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freezerDefrostHeaterVoteErd = Erd_FreezerDefrostHeater_DefrostVote,
   .freezerDefrostCycleCountErd = Erd_FreezerDefrostCycleCount,
   .numberOfFreezerAbnormalDefrostCycleCountErd = Erd_NumberOfFreezerAbnormalDefrostCycles,
   .freezerDefrostHeaterOnTimeInMinutesErd = Erd_FreezerDefrostHeaterOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .nextDefrostTypeErd = Erd_NextDefrostType,
   .currentDefrostTypeErd = Erd_CurrentDefrostType,
   .timerModuleErd = Erd_TimerModule,
   .clearedEepromStartup = Erd_Eeprom_ClearedDefrostEepromStartup,
   .defrostTestStateRequestErd = Erd_DefrostTestStateRequest,
   .dontSkipDefrostPrechillErd = Erd_DontSkipDefrostPrechill,
   .invalidFreezerEvaporatorThermistorDuringDefrostErd = Erd_InvalidFreezerEvaporatorThermistorDuringDefrost
};

static const DefrostHeaterMaxOnTimeConfiguration_t defrostHeaterMaxOnTimeConfig = {
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodEvaporatorThermistorIsValidErd = Erd_FreshFoodEvapThermistor_IsValidResolved,
   .hasConvertibleCompartment = Erd_HasConvertibleCompartment,
   .convertibleCompartmentEvaporatorThermistorIsValidErd = Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .freshFoodDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes
};

static const DefrostCompressorOnTimeCounterConfiguration_t defrostCompressorOnTimeCounterConfig = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .waitingToDefrostErd = Erd_WaitingToDefrost,
   .defrostCompressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .defrostCompressorOnTimeCounterFsmStateErd = Erd_DefrostCompressorOnTimeCounterFsmState,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .resetAndCountSignalErd = Erd_ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestSignal,
   .defrostCompressorOnTimeCounterReadyErd = Erd_DefrostCompressorOnTimeCounterReady,
   .timerModuleErd = Erd_TimerModule
};

static const DefrostHeaterOnTimeCounterConfig_t defrostHeaterOnTimeCounterConfig = {
   .defrostHeaterOnTimeErd = Erd_FreezerDefrostHeaterOnTimeInMinutes,
   .defrostHeaterStateErd = Erd_FreezerDefrostHeater_ResolvedVote,
   .defrostStateErd = Erd_DefrostState
};

static const DoorAccelerationCounterConfiguration_t doorAccelerationCounterConfig = {
   .waitingToDefrostErd = Erd_WaitingToDefrost,
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
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .resetAndCountSignalErd = Erd_ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestSignal,
   .doorAccelerationCounterReadyErd = Erd_DoorAccelerationCounterReady,
   .timerModuleErd = Erd_TimerModule
};

static ReadyToDefrostConfiguration_t readyToDefrostConfig = {
   .defrostCompressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freshFoodScaledDoorAccelerationInSecondsErd = Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds,
   .freezerScaledDoorAccelerationInSecondsErd = Erd_DefrostFreezerScaledDoorAccelerationInSeconds,
   .convertibleCompartmentScaledDoorAccelerationInSecondsErd = Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds,
   .readyToDefrostErd = Erd_ReadyToDefrost,
   .defrostCompressorOnTimeCounterReadyErd = Erd_DefrostCompressorOnTimeCounterReady,
   .doorAccelerationCounterReadyErd = Erd_DoorAccelerationCounterReady,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .freezerDefrostUseMinimumTimeErd = Erd_UseMinimumReadyToDefrostTime,
   .invalidFreezerEvaporatorThermistorDuringDefrostErd = Erd_InvalidFreezerEvaporatorThermistorDuringDefrost,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .hasConvertibleCompartment = Erd_HasConvertibleCompartment,
   .eepromClearedErd = Erd_Eeprom_ClearedDefrostEepromStartup,
   .waitingForDefrostTimeInSecondsErd = Erd_WaitingForDefrostTimeInSeconds
};

static const TimeThatPrechillConditionsAreMetConfiguration_t timeThatPrechillConditionsAreMetConfig = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .coolingModeErd = Erd_CoolingMode,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .timerModuleErd = Erd_TimerModule
};

static const NextDefrostTypeArbiterConfig_t nextDefrostTypeArbiterConfig = {
   .nextDefrostTypeErd = Erd_NextDefrostType,
   .defrostingErd = Erd_Defrosting,
   .freshFoodDefrostCountErd = Erd_FreshFoodDefrostCount,
   .numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd = Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathMode,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .currentDefrostTypeErd = Erd_CurrentDefrostType,
   .clearedDefrostEepromStartupErd = Erd_Eeprom_ClearedDefrostEepromStartup,
   .freezerFilteredTemperatureTooWarmAtPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp
};

static const DefrostTestRequestHandlerConfiguration_t defrostTestRequestHandlerConfig = {
   .defrostStateErd = Erd_DefrostState,
   .defrostTestRequestErd = Erd_DefrostTestRequest,
   .disableDefrostErd = Erd_DisableDefrost,
   .defrostTestStateRequestErd = Erd_DefrostTestStateRequest,
   .nextDefrostTypeErd = Erd_NextDefrostType,
   .useMinimumReadyToDefrostTimeErd = Erd_UseMinimumReadyToDefrostTime,
   .resetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestErd = Erd_ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestSignal,
   .defrostTestRequestStatusErd = Erd_DefrostTestRequestStatus,
   .dontSkipDefrostPrechillErd = Erd_DontSkipDefrostPrechill
};

static bool ThereIsAConvertibleCompartment(I_DataModel_t *dataModel)
{
   bool hasConvertibleCompartment;
   DataModel_Read(
      dataModel,
      Erd_HasConvertibleCompartment,
      &hasConvertibleCompartment);

   return hasConvertibleCompartment;
}

void DefrostPlugin_Init(DefrostPlugin_t *instance, I_DataModel_t *dataModel)
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
   if(ThereIsAConvertibleCompartment(dataModel))
   {
      DataModel_Read(
         dataModel,
         Erd_ConvertibleCompartmentStateResolverReady,
         &convertibleCompartmentStateResolverReady);
   }
   else
   {
      convertibleCompartmentStateResolverReady = true;
   }

   bool ambientTemperaturePluginReady;
   DataModel_Read(
      dataModel,
      Erd_AmbientTemperaturePluginReady,
      &ambientTemperaturePluginReady);

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
      ambientTemperaturePluginReady &&
      gridPluginReady &&
      periodicNvUpdaterReady &&
      sabbathPluginReady);

   DefrostHeaterMaxOnTime_Init(
      &instance->_private.defrostHeaterMaxOnTime,
      dataModel,
      &defrostHeaterMaxOnTimeConfig);

   TimeThatPrechillConditionsAreMet_Init(
      &instance->_private.timeThatPrechillConditionsAreMet,
      dataModel,
      &timeThatPrechillConditionsAreMetConfig);

   FreezerFilteredTemperatureTooWarmOnPowerUp_Init(dataModel);

   DefrostParameterSelector_Init(&instance->_private.defrostParameterSelector, dataModel);

   FreezerDefrostHeaterVotingFrameworkPlugin_Init(&instance->_private.freezerDefrostHeaterVotingFramework, dataModel);

   DefrostStateOnCompareMatch_Init(&instance->_private.defrostStateOnCompareMatch, dataModel);

   Defrost_Init(&instance->_private.defrost, dataModel, &defrostConfig);

   DefrostCompressorOnTimeCounter_Init(
      &instance->_private.defrostCompressorOnTimeCounter,
      dataModel,
      &defrostCompressorOnTimeCounterConfig);

   DefrostHeaterOnTimeCounter_Init(
      &instance->_private.defrostHeaterOnTimeCounter,
      dataModel,
      &defrostHeaterOnTimeCounterConfig);

   DoorAccelerationCounter_Init(
      &instance->_private.doorAccelerationCounter,
      dataModel,
      &doorAccelerationCounterConfig);

   ReadyToDefrost_Init(
      &instance->_private.readyToDefrost,
      dataModel,
      &readyToDefrostConfig);

   NextDefrostTypeArbiter_Init(
      &instance->_private.nextDefrostTypeArbiter,
      dataModel,
      &nextDefrostTypeArbiterConfig);

   DefrostTestRequestHandler_Init(
      &instance->_private.defrostTestRequestHandler,
      dataModel,
      &defrostTestRequestHandlerConfig);
}
