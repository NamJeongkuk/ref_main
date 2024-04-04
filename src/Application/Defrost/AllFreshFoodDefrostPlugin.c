/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AllFreshFoodDefrostPlugin.h"
#include "DefrostConfiguration.h"
#include "FreezerFilteredTemperatureTooWarmOnPowerUp.h"
#include "SystemErds.h"
#include "uassert.h"
#include "DefrostConfiguration.h"

static const DefrostHeaterMaxOnTimeConfiguration_t defrostHeaterMaxOnTimeConfig = {
   .freezerEvaporatorThermistorIsValidResolvedErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodEvaporatorThermistorIsValidResolvedErd = Erd_FreshFoodEvapThermistor_IsValidResolved,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentEvaporatorThermistorIsValidResolvedErd = Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .freshFoodDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes
};

static const DefrostHeaterOnTimeCounterConfig_t freshFoodDefrostHeaterOnTimeCounterConfig = {
   .defrostHeaterOnTimeErd = Erd_FreshFoodDefrostHeaterOnTimeInMinutes,
   .defrostHeaterStateErd = Erd_FreshFoodDefrostHeater_ResolvedVote,
   .defrostStateErd = Erd_DefrostState
};

static ReadyToDefrostDoorConfiguration_t doorsConfiguration[] = {
   {
      .doorIsOpenErd = Erd_RightSideFreshFoodDoorStatusResolved,
      .doorAccelerationErd = Erd_RightSideFreshFoodScaledDoorAccelerationInSeconds,
      .offsetInParametricForDoorFactor = OFFSET_OF(DefrostData_t, idleData.freshFoodDoorIncrementFactorInSecondsPerSecond),
   }
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

void AllFreshFoodDefrostPlugin_Init(AllFreshFoodDefrostPlugin_t *instance, I_DataModel_t *dataModel)
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
      Erd_FreshFoodAndFreezerGridPluginReady,
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

   FreshFoodDefrostHeaterVotingFrameworkPlugin_Init(&instance->_private.freshFoodDefrostHeaterVotingFramework, dataModel);

   DefrostStateOnCompareMatch_Init(&instance->_private.defrostStateOnCompareMatch, dataModel);

   Defrost_Init(
      &instance->_private.defrost,
      dataModel,
      &defrostConfiguration,
      PersonalityParametricData_Get(dataModel)->defrostData,
      PersonalityParametricData_Get(dataModel)->platformData);

   DefrostHeaterOnTimeCounter_Init(
      &instance->_private.defrostHeaterOnTimeCounter,
      dataModel,
      &freshFoodDefrostHeaterOnTimeCounterConfig);

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
