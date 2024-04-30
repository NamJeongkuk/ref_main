/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AllFreshFoodDefrostPlugin.h"
#include "DefrostConfiguration.h"
#include "SystemErds.h"
#include "uassert.h"
#include "DefrostConfiguration.h"

static const DefrostHeaterOnTimeCounterConfig_t freshFoodDefrostHeaterOnTimeCounterConfig = {
   .defrostHeaterOnTimeErd = Erd_FreshFoodDefrostHeaterOnTimeInMinutes,
   .defrostHeaterStateErd = Erd_FreshFoodDefrostHeater_ResolvedVote,
   .defrostStateErd = Erd_DefrostState
};

static ReadyToDefrostDoorConfiguration_t doorsConfiguration[] = {
   {
      .doorIsOpenErd = Erd_RightSideFreshFoodDoorStatusResolved,
      .doorAccelerationErd = Erd_FreshFoodScaledDoorAccelerationInSeconds,
      .offsetInParametricForDoorFactor = OFFSET_OF(DefrostData_t, idleData.freshFoodDoorIncrementFactorInSecondsPerSecond),
   }
};

static ReadyToDefrostConfiguration_t readyToDefrostConfig = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .defrostCompressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .defrostPowerUpReadyErd = Erd_DefrostPowerUpReady,
   .cabinetFilteredTemperatureTooWarmOnPowerUpErd = Erd_CabinetFilteredTemperatureTooWarmOnPowerUp,
   .useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd = Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts,
   .invalidEvaporatorThermistorDuringDefrostErd = Erd_InvalidEvaporatorThermistorDuringDefrost,
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
      &defrostHeaterMaxOnTimeConfig,
      &defrostData->heaterOnData);

   TimeThatPrechillConditionsAreMet_Init(
      &instance->_private.timeThatPrechillConditionsAreMet,
      dataModel,
      &timeThatPrechillConditionsAreMetConfig);

   FreshFoodDefrostHeaterVotingFrameworkPlugin_Init(&instance->_private.freshFoodDefrostHeaterVotingFramework, dataModel);

   DefrostStateOnCompareMatch_Init(&instance->_private.defrostStateOnCompareMatch, dataModel);

   Defrost_Init(
      &instance->_private.defrost,
      dataModel,
      &defrostConfiguration,
      PersonalityParametricData_Get(dataModel)->defrostData,
      PersonalityParametricData_Get(dataModel)->platformData,
      PersonalityParametricData_Get(dataModel)->enhancedSabbathData);

   DefrostHeaterOnTimeCounter_Init(
      &instance->_private.defrostHeaterOnTimeCounter,
      dataModel,
      &freshFoodDefrostHeaterOnTimeCounterConfig);

   ReadyToDefrost_Init(
      &instance->_private.readyToDefrost,
      dataModel,
      &readyToDefrostConfig,
      defrostData);

   DefrostTestRequestHandler_Init(
      &instance->_private.defrostTestRequestHandler,
      dataModel,
      &defrostTestRequestHandlerConfig);

   SabbathReadyToDefrost_Init(
      &instance->_private.sabbathReadyToDefrost,
      dataModel,
      &sabbathReadyToDefrostConfig,
      PersonalityParametricData_Get(dataModel)->sabbathData);
}
