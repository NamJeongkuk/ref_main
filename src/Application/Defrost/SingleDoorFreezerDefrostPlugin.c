/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerDefrostPlugin.h"
#include "DefrostConfiguration.h"
#include "SystemErds.h"
#include "uassert.h"
#include "DefrostConfiguration.h"

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

void SingleDoorFreezerDefrostPlugin_Init(SingleDoorFreezerDefrostPlugin_t *instance, I_DataModel_t *dataModel)
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

   FreezerDefrostHeaterVotingFrameworkPlugin_Init(&instance->_private.freezerDefrostHeaterVotingFramework, dataModel);

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
      &freezerDefrostHeaterOnTimeCounterConfig);

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

   UpdateDefrostStateBasedOnDefrostHsmState(dataModel);
}
