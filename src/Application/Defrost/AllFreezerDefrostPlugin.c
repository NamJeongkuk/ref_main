/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AllFreezerDefrostPlugin.h"
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
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .readyToDefrostErd = Erd_ReadyToDefrost,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUp,
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodThermistorIsValidResolvedErd = Erd_FreshFoodThermistor_IsValidResolved,
   .freezerThermistorIsValidResolvedErd = Erd_FreezerThermistor_IsValidResolved,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .coolingModeErd = Erd_CoolingMode,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .disableCompressorMinimumTimesVoteErd = Erd_DisableMinimumCompressorTimes_DefrostVote,
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
   .numberOfFreezerDefrostsErd = Erd_NumberOfFreezerDefrosts,
   .numberOfFreezerAbnormalDefrostsErd = Erd_NumberOfFreezerAbnormalDefrosts,
   .freezerDefrostHeaterOnTimeInMinutesErd = Erd_FreezerDefrostHeaterOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .nextDefrostTypeOverrideErd = Erd_NextDefrostTypeOverride,
   .currentDefrostTypeErd = Erd_CurrentDefrostType,
   .timerModuleErd = Erd_TimerModule,
   .eepromClearedErd = Erd_Eeprom_ClearedDefrostEepromStartup,
   .defrostTestStateRequestErd = Erd_DefrostTestStateRequest,
   .dontSkipDefrostPrechillErd = Erd_DontSkipDefrostPrechill,
   .invalidFreezerEvaporatorThermistorDuringDefrostErd = Erd_InvalidFreezerEvaporatorThermistorDuringDefrost,
   .useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd = Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts,
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .sabbathIsReadyToDefrostErd = Erd_SabbathIsReadyToDefrost,
   .enhancedSabbathIsRequestingDefrostErd = Erd_EnhancedSabbathIsRequestingDefrost,
   .freezerDefrostHeaterOnForMaxTimeFaultErd = Erd_FreezerDefrostHeaterOnForMaxTimeFault
};

static const DefrostHeaterMaxOnTimeConfiguration_t defrostHeaterMaxOnTimeConfig = {
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodEvaporatorThermistorIsValidErd = Erd_FreshFoodEvapThermistor_IsValidResolved,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentEvaporatorThermistorIsValidErd = Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .freshFoodDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes
};

static const DefrostHeaterOnTimeCounterConfig_t freezerDefrostHeaterOnTimeCounterConfig = {
   .defrostHeaterOnTimeErd = Erd_FreezerDefrostHeaterOnTimeInMinutes,
   .defrostHeaterStateErd = Erd_FreezerDefrostHeater_ResolvedVote,
   .defrostStateErd = Erd_DefrostState
};

static ReadyToDefrostDoorConfiguration_t doorsConfiguration[] = {
   {
      .doorIsOpenErd = Erd_LeftSideFreezerDoorStatusResolved,
      .doorAccelerationErd = Erd_FreezerScaledDoorAccelerationInSeconds,
      .offsetInParametricForDoorFactor = OFFSET_OF(DefrostData_t, idleData.freezerDoorIncrementFactorInSecondsPerSecond),
   },
};

static ReadyToDefrostConfiguration_t readyToDefrostConfig = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .defrostCompressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUp,
   .useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd = Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts,
   .invalidFreezerEvaporatorThermistorDuringDefrostErd = Erd_InvalidFreezerEvaporatorThermistorDuringDefrost,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .eepromClearedErd = Erd_Eeprom_ClearedDefrostEepromStartup,
   .waitingForDefrostTimeInSecondsErd = Erd_WaitingForDefrostTimeInSeconds,
   .waitingToDefrostErd = Erd_WaitingToDefrost,
   .readyToDefrostErd = Erd_ReadyToDefrost,
   .timeBetweenDefrostsInMinutesErd = Erd_TimeBetweenDefrostsInMinutes,
   .readyToDefrostHsmStateErd = Erd_ReadyToDefrostHsmState,
   .doorsConfiguration = doorsConfiguration,
   .numberOfDoors = NUM_ELEMENTS(doorsConfiguration)
};

static const TimeThatPrechillConditionsAreMetConfiguration_t timeThatPrechillConditionsAreMetConfig = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .coolingModeErd = Erd_CoolingMode,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .timerModuleErd = Erd_TimerModule
};

static const NextDefrostTypeOverrideArbiterConfig_t nextDefrostTypeOverrideArbiterConfig = {
   .nextDefrostTypeOverrideErd = Erd_NextDefrostTypeOverride,
   .defrostingErd = Erd_Defrosting,
   .numberOfSecondaryOnlyDefrostsErd = Erd_NumberOfSecondaryOnlyDefrosts,
   .numberOfSecondaryOnlyDefrostsBeforeAFullDefrostErd = Erd_NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .currentDefrostTypeErd = Erd_CurrentDefrostType,
   .freezerFilteredTemperatureTooWarmAtPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUp
};

static const DefrostTestRequestHandlerConfiguration_t defrostTestRequestHandlerConfig = {
   .defrostStateErd = Erd_DefrostState,
   .defrostTestRequestErd = Erd_DefrostTestRequest,
   .disableDefrostErd = Erd_DisableDefrost,
   .defrostTestStateRequestErd = Erd_DefrostTestStateRequest,
   .nextDefrostTypeOverrideErd = Erd_NextDefrostTypeOverride,
   .useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd = Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts,
   .defrostTestRequestStatusErd = Erd_DefrostTestRequestStatus,
   .dontSkipDefrostPrechillErd = Erd_DontSkipDefrostPrechill
};

static const SabbathReadyToDefrostConfig_t sabbathReadyToDefrostConfig = {
   .timerModuleErd = Erd_TimerModule,
   .waitingToDefrostErd = Erd_WaitingToDefrost,
   .sabbathIsReadyToDefrostErd = Erd_SabbathIsReadyToDefrost,
   .sabbathTimeBetweenDefrostsInMinutesErd = Erd_SabbathTimeBetweenDefrostsInMinutes,
   .sabbathWaitingForDefrostTimeInMinutesErd = Erd_SabbathWaitingForDefrostTimeInMinutes
};

static void UpdateDefrostStateBasedOnDefrostHsmState(I_DataModel_t *dataModel)
{
   DefrostHsmState_t initialHsmState;
   DataModel_Read(
      dataModel,
      Erd_DefrostHsmState,
      &initialHsmState);

   DefrostState_t initialDefrostState;

   switch(initialHsmState)
   {
      case DefrostHsmState_Idle:
         initialDefrostState = DefrostState_Idle;
         break;

      case DefrostHsmState_PrechillPrep:
      case DefrostHsmState_Prechill:
         initialDefrostState = DefrostState_Prechill;
         break;

      case DefrostHsmState_HeaterOnEntry:
      case DefrostHsmState_HeaterOn:
         initialDefrostState = DefrostState_HeaterOn;
         break;

      case DefrostHsmState_Dwell:
         initialDefrostState = DefrostState_Dwell;
         break;

      case DefrostHsmState_PostDwell:
         initialDefrostState = DefrostState_PostDwell;
         break;

      case DefrostHsmState_Disabled:
         initialDefrostState = DefrostState_Disabled;
         break;
   }

   DataModel_Write(
      dataModel,
      Erd_DefrostState,
      &initialDefrostState);
}

void AllFreezerDefrostPlugin_Init(AllFreezerDefrostPlugin_t *instance, I_DataModel_t *dataModel)
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

   uassert(sensorsReadyToBeRead &&
      setpointResolverReady &&
      ambientTemperaturePluginReady &&
      gridPluginReady &&
      periodicNvUpdaterReady);

   const DefrostData_t *defrostData = PersonalityParametricData_Get(dataModel)->defrostData;

   DefrostHeaterMaxOnTime_Init(
      &instance->_private.defrostHeaterMaxOnTime,
      dataModel,
      &defrostHeaterMaxOnTimeConfig);

   TimeThatPrechillConditionsAreMet_Init(
      &instance->_private.timeThatPrechillConditionsAreMet,
      dataModel,
      &timeThatPrechillConditionsAreMetConfig);

   FreezerFilteredTemperatureTooWarmOnPowerUp_Init(dataModel);

   FreezerDefrostHeaterVotingFrameworkPlugin_Init(&instance->_private.freezerDefrostHeaterVotingFramework, dataModel);

   DefrostStateOnCompareMatch_Init(&instance->_private.defrostStateOnCompareMatch, dataModel);

   Defrost_Init(
      &instance->_private.defrost,
      dataModel,
      &defrostConfig,
      PersonalityParametricData_Get(dataModel)->defrostData);

   DefrostHeaterOnTimeCounter_Init(
      &instance->_private.defrostHeaterOnTimeCounter,
      dataModel,
      &freezerDefrostHeaterOnTimeCounterConfig);

   ReadyToDefrost_Init(
      &instance->_private.readyToDefrost,
      dataModel,
      &readyToDefrostConfig,
      defrostData);

   NextDefrostTypeOverrideArbiter_Init(
      &instance->_private.nextDefrostTypeOverrideArbiter,
      dataModel,
      &nextDefrostTypeOverrideArbiterConfig);

   DefrostTestRequestHandler_Init(
      &instance->_private.defrostTestRequestHandler,
      dataModel,
      &defrostTestRequestHandlerConfig);

   SabbathReadyToDefrost_Init(
      &instance->_private.sabbathReadyToDefrost,
      dataModel,
      &sabbathReadyToDefrostConfig,
      PersonalityParametricData_Get(dataModel)->sabbathData);

   UpdateDefrostStateBasedOnDefrostHsmState(dataModel);
}
