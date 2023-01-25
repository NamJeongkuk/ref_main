/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Defrost.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

enum
{
   Invalid = false,
   Valid = true
};

enum
{
   ThreeHundredMinutes = 300,
   TwoHundredFiftyFiveMinutes = 255,
   TenMinutes = 10,
   NineMinutes = 9,
   OneMinute = 1,
   ZeroMinutes = 0
};

enum
{
   FreezerDefrostHeaterMaxOnTimeInMinutes = 45
};

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
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .freshFoodThermistorIsValidErd = Erd_FreshFood_ThermistorIsValid,
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
   .invalidFreezerEvaporatorThermistorDuringDefrostErd = Erd_InvalidFreezerEvaporatorThermistorDuringDefrost
};

static const SabbathData_t sabbathData = {
   .maxTimeBetweenDefrostsInMinutes = 16 * MINUTES_PER_HOUR
};

static const EvaporatorData_t singleEvaporatorData = {
   .numberOfEvaporators = 1
};

static const EvaporatorData_t dualEvaporatorData = {
   .numberOfEvaporators = 2
};

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .gridLinesDegFx100 = { 0, -450, 150, 450, 950, 1150 }
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .gridLinesDegFx100 = { -250, 0, 250, 600, 750, 5500 }
};

static CalculatedGridLines_t calcGridLines = {
   .freshFoodGridLine = freshFoodCalcAxis,
   .freezerGridLine = freezerCalcAxis
};

TEST_GROUP(Defrost_SingleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   const DefrostData_t *defrostData;
   const GridData_t *gridData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
      gridData = PersonalityParametricData_Get(dataModel)->gridData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);

      Defrost_Init(&instance, dataModel, &defrostConfig);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FreezerFilteredTemperatureTooWarmAtPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreezerEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void LastFreshFoodDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, &state);
   }

   void LastFreshFoodDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, &state);
   }

   void LastConvertibleCompartmentDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void LastConvertibleCompartmentDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void LastFreezerDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void LastFreezerDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      switch(state)
      {
         case DefrostHsmState_Idle:
            Given DefrostStateIs(DefrostState_Idle);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
            Given FreezerEvaporatorThermistorValidityIs(Valid);
            Given FreshFoodThermistorValidityIs(Valid);

            When ReadyToDefrost();
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         case DefrostHsmState_Prechill:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

            When PrechillConditionsAreMet();
            DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
            break;

         case DefrostHsmState_HeaterOnEntry:
            Given MaxPrechillTimeInMinutesIs(TenMinutes);
            Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
            Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
            Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
            Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

            After(TenMinutes * MSEC_PER_MIN);
            DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
            break;

         case DefrostHsmState_HeaterOn:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

            After(defrostData->heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
            DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
            break;

         case DefrostHsmState_Dwell:
            Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
            And DefrostStateIs(DefrostState_Dwell);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
            break;

         case DefrostHsmState_PostDwell:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

            After(defrostData->dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
            DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
            break;

         case DefrostHsmState_Disabled:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

            When DisableDefrostIs(true);
            DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
            break;

         default:
            break;
      }
   }

   void NothingShouldHappen()
   {
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorIsValid, &state);
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFood_ThermistorIsValid, &state);
   }

   void CompressorStateIs(CompressorState_t state)
   {
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void ReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, set);
   }

   void ClearReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, clear);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void PrechillConditionsAreMet()
   {
      CompressorIs(ON);
      CoolingModeIs(CoolingMode_Freezer);
   }

   void MaxPrechillTimeInMinutesIs(uint8_t maxPrechillTime)
   {
      DataModel_Write(dataModel, Erd_MaxPrechillTimeInMinutes, &maxPrechillTime);
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t timeThatPrechillConditionsAreMet)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &timeThatPrechillConditionsAreMet);
   }

   void CompressorSpeedVoteShouldBe(CompressorSpeed_t expected)
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void CondenserFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreezerEvapFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreshFoodDamperPositionVoteShouldBe(DamperPosition_t expected)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.position);
      CHECK_TRUE(actual.care);
   }

   void PrechillLoadVotesShouldBeDontCare()
   {
      CompressorVotedSpeed_t compressorVote;
      DataModel_Read(dataModel, Erd_CompressorSpeed_DefrostVote, &compressorVote);

      FanVotedSpeed_t fanVote;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &fanVote);

      DamperVotedPosition_t damperVote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &damperVote);

      CHECK_FALSE(compressorVote.care);
      CHECK_FALSE(fanVote.care);
      CHECK_FALSE(damperVote.care);
   }

   void DamperVoteShouldBeDontCare()
   {
      DamperVotedPosition_t damperVote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &damperVote);

      CHECK_FALSE(damperVote.care);
   }

   void FreezerDefrostHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void CompressorSpeedVoteShouldBeDontCare()
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeed_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void DisableMinimumCompressorTimesShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DisableMinimumCompressorTimes, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FanSpeedVotesShouldBeDontCare()
   {
      FanVotedSpeed_t actual;

      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &actual);
      CHECK_FALSE(actual.care);

      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_DefrostVote, &actual);
      CHECK_FALSE(actual.care);

      DataModel_Read(dataModel, Erd_CondenserFanSpeed_DefrostVote, &actual);
      CHECK_FALSE(actual.care);
   }

   void FreezerDefrostHeaterVoteIs(HeaterState_t state)
   {
      HeaterVotedState_t vote;
      vote.state = state;
      vote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &vote);
   }

   void FreezerDefrostHeaterVoteShouldBe(HeaterState_t expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_TRUE(actual.care);
   }

   void IceCabinetFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreezerDefrostCycleCountIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostCycleCount, &count);
   }

   void FreezerDefrostCycleCountShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostCycleCount, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void NumberOfFreezerAbnormalDefrostCycleCountIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfFreezerAbnormalDefrostCycles, &count);
   }

   void NumberOfFreezerAbnormalDefrostCycleCountShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_NumberOfFreezerAbnormalDefrostCycles, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterOnTimeInMinutes, &timeInMinutes);
   }

   void FreezerDefrostHeaterMaxOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterMaxOnTimeInMinutes, &timeInMinutes);
   }

   void FreezerDefrostWasAbnormalFlagShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostWasAbnormal, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WaitingToDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, &state);
   }

   void WaitingToDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_WaitingToDefrost, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DefrostingIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Defrosting, &state);
   }

   void DefrostingShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_Defrosting, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DisableDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DisableDefrost, &state);
   }

   void NextDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_NextDefrostType, &defrostType);
   }

   void CurrentDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_CurrentDefrostType, &defrostType);
   }

   void CurrentDefrostTypeShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ClearedEepromOnStartupIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, &state);
   }

   void ClearedEepromOnStartupShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void InvalidFreezerEvaporatorThermistorDuringDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_InvalidFreezerEvaporatorThermistorDuringDefrost, &state);
   }

   void InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_InvalidFreezerEvaporatorThermistorDuringDefrost, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void DefrostTestIsRequested(DefrostTestStateRequest_t request)
   {
      DefrostTestStateRequestMessage_t requestMessage;
      DataModel_Read(dataModel, Erd_DefrostTestStateRequest, &requestMessage);
      requestMessage.request = request;
      requestMessage.requestId++;
      DataModel_Write(dataModel, Erd_DefrostTestStateRequest, &requestMessage);
   }

   void DefrostTestStateRequestShouldBeNone(void)
   {
      DefrostTestStateRequest_t expectedRequest = DefrostTestStateRequest_None;
      DefrostTestStateRequestMessage_t actual;
      DataModel_Read(dataModel, Erd_DefrostTestStateRequest, &actual);

      CHECK_EQUAL(expectedRequest, actual.request);
   }
};

TEST(Defrost_SingleEvap, ShouldInitializeIntoDwellHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(true);
   And DefrostStateIs(DefrostState_HeaterOn);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasIdle)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(true);
   And DefrostStateIs(DefrostState_Idle);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(true);
   And DefrostStateIs(DefrostState_Prechill);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(true);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoHeaterOnHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
   And DefrostStateIs(DefrostState_Prechill);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoHeaterOnHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
   And DefrostStateIs(DefrostState_HeaterOn);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoDwellHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPostDwell)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
   And DefrostStateIs(DefrostState_PostDwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasIdle)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
   And DefrostStateIs(DefrostState_Idle);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasDisabled)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(true);
   And DefrostStateIs(DefrostState_Disabled);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDisabled)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
   And DefrostStateIs(DefrostState_Disabled);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndLastFreshFoodDefrostWasAbnormal)
{
   Given LastFreshFoodDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndLastFreezerDefrostWasAbnormal)
{
   Given LastFreezerDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndLastConvertibleCompartmentDefrostWasAbnormal)
{
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerWasTooWarmAtPowerUpAndResetFreezerWasTooWarmErdToFalse)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerWasTooWarmAtPowerUpAndLastFreezerDefrostWasAbnormalAndResetFreezerWasTooWarmErdToFalse)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And LastFreezerDefrostWasAbnormal();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndInvalidFreezerEvaporatorThermistorDuringDefrostIsFalseAndFreshFoodThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   And FreshFoodThermistorValidityIs(Valid);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndInvalidFreezerEvaporatorThermistorDuringDefrostIsTrueAndFreshFoodThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And FreshFoodThermistorValidityIs(Valid);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndInvalidFreezerEvaporatorThermistorDuringDefrostIsTrueAndFreshFoodThermistorIsInvalid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And FreshFoodThermistorValidityIs(Invalid);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsValidAndInvalidFreezerEvaporatorThermistorDuringDefrostIsTrueAndFreshFoodThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And FreshFoodThermistorValidityIs(Valid);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldSetDisableMinimumCompressorOnTimesWhenEnteringHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);
   DisableMinimumCompressorTimesShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenReadyToDefrostAndLastDefrostsWereNormalAndThermistorsAreValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreshFoodThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValid)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given PrechillConditionsAreMet();

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldNotGoToPrechillWhenPrechillConditionsNotMetWhileInPrechillPrep)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorIs(OFF);
   And CoolingModeIs(CoolingMode_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   When CoolingModeIs(CoolingMode_FreshFood);
   And CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenPrechillConditionsMetWhileInPrechillPrep)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenPrechillPrepTimerExpiresWhileInPrechillPrep)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   After((defrostData->prechillPrepData.maxPrechillPrepTimeInMinutes * MSEC_PER_MIN) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepTheNextTimeThroughIdleAfterItWasTooWarmOnPowerUp)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreshFoodThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ClearReadyToDefrost();

   After(defrostData->heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(defrostData->dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillPrepAndFreshFoodThermistorIsAlreadyInvalid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreshFoodThermistorValidityIs(Invalid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreshFoodThermistorBecomesInvalidWhileInPrechillPrep)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreshFoodThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrep)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeWhileTimeThatPrechillConditionsAreMetIsZeroInPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   After(TenMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeMinusTimeThatPrechillConditionsAreMetWhileTimeThatPrechillConditionsAreMetIsNotZeroInPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(NineMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   After(OneMinute * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndMaxPrechillTimeAlreadyMet)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(TenMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndPrechillConditionsHaveBeenMetForLongerThanMaxPrechillTime)
{
   Given MaxPrechillTimeInMinutesIs(TwoHundredFiftyFiveMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ThreeHundredMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreezerEvapTemperatureGoesBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerEvapTemperatureAlreadyBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreezerTemperatureGoesBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerTemperatureAlreadyBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureGoesBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreshFoodTemperatureAlreadyBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureGoesAboveExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMaxTempInDegFx100 - 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMaxTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreshFoodTemperatureAlreadyAboveExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMaxTempInDegFx100);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreshFoodThermistorBecomesInvalidWhileInPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FreshFoodThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldVoteForLoadsOnEntryToPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   CompressorSpeedVoteShouldBe(defrostData->prechillData.prechillCompressorSpeed);
   FreezerEvapFanSpeedVoteShouldBe(defrostData->prechillData.prechillFreezerEvapFanSpeed);
   FreshFoodDamperPositionVoteShouldBe(defrostData->prechillData.prechillFreshFoodDamperPosition);
}

TEST(Defrost_SingleEvap, ShouldTurnOffCompressorAndFansAndNotCareAboutDamperVoteOnHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   DamperVoteShouldBeDontCare();
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(Defrost_SingleEvap, ShouldExitOnHeaterEntryStateAndTurnOnTheDefrostHeaterAfterDefrostHeaterOnDelayTimerExpired)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData->heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerDefrostHeaterVoteShouldBeDontCare();

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   And FreezerDefrostHeaterVoteShouldBe(HeaterState_On);
}

TEST(Defrost_SingleEvap, ShouldTurnOffDefrostHeaterAndIncrementFreezerDefrostCountAndTransitToDwellStateWhenFreezerEvaporatorTemperatureReachesFreezerDefrostTerminationTemperature)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostCycleCountIs(1);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FilteredFreezerEvapTemperatureIs(defrostData->heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   And FreezerDefrostCycleCountShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldClearFreezerDefrostWasAbnormalWhenFreezerHeaterOnTimeIsLessThanFreezerHeaterOnTimeToSetAbnormalDefrost)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData->heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes - 1);

   When FilteredFreezerEvapTemperatureIs(defrostData->heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(CLEAR);
}

TEST(Defrost_SingleEvap, ShouldNotClearFreezerDefrostWasAbnormalWhenFreezerHeaterOnTimeIsEqualToFreezerHeaterOnTimeToSetAbnormalDefrost)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData->heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes);

   When FilteredFreezerEvapTemperatureIs(defrostData->heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldTurnOffDefrostHeaterAndIncrementFreezerDefrostCountAndTransitionToDwellStateWhenFreezerHeaterOnTimeReachesMaxOnTime)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostCycleCountIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   And FreezerDefrostCycleCountShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldIncrementNumberOfFreezerAbnormalDefrostCycleCountAndSetFreezerDefrostWasAbnormalAndSaveTheFreezerDefrostCountAsTheLastAbnormalFreezerDefrostCountWhenFreezerHeaterOnTimeReachesAbnormalTime)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerAbnormalDefrostCycleCountIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given LastFreezerDefrostWasNormal();
   Given FreezerDefrostCycleCountIs(3);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData->heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfFreezerAbnormalDefrostCycleCountShouldBe(2);
   And FreezerDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldIncrementNumberOfFreezerAbnormalDefrostCycleCountJustOnce)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerAbnormalDefrostCycleCountIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData->heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfFreezerAbnormalDefrostCycleCountShouldBe(2);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData->heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes + 1);
   NumberOfFreezerAbnormalDefrostCycleCountShouldBe(2);
}

TEST(Defrost_SingleEvap, ShouldTurnOffFreezerDefrostHeaterCompressorAndAllFansWhenEnteringDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreshFoodDamperWhenEnteringDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);
   FreshFoodDamperPositionVoteShouldBe(defrostData->dwellData.dwellFreshFoodDamperPosition);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPostDwellAfterDwellTimeHasPassed)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   After(defrostData->dwellData.dwellTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
}

TEST(Defrost_SingleEvap, ShouldVoteForCompressorAndCondenserFanAndDamperWhenEnteringPostDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   CompressorSpeedVoteShouldBe(defrostData->postDwellData.postDwellCompressorSpeed);
   CondenserFanSpeedVoteShouldBe(defrostData->postDwellData.postDwellCondenserFanSpeed);
   FreshFoodDamperPositionVoteShouldBe(defrostData->postDwellData.postDwellFreshFoodDamperPosition);
   DisableMinimumCompressorTimesShouldBe(true);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAfterPostDwellTimeHasPassed)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   After(defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleWhenFreezerEvaporatorTemperatureIsLessThanExitTemperature)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData->postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleWhenFreezerEvaporatorTemperatureIsEqualToExitTemperature)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData->postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldNotCareAboutFreezerDefrostHeaterCompressorAndDamperAndAllFansAndEnableMinimumCompressorTimesWhenEnteringIdleFromPostDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);
   When FilteredFreezerEvapTemperatureIs(defrostData->postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   FreezerDefrostHeaterVoteShouldBeDontCare();
   CompressorSpeedVoteShouldBeDontCare();
   DamperVoteShouldBeDontCare();
   FanSpeedVotesShouldBeDontCare();
   DisableMinimumCompressorTimesShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldWriteTrueToWaitingToDefrostErdWhenEnteringIdleOnInit)
{
   Given WaitingToDefrostIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WaitingToDefrostShouldBe(true);
}

TEST(Defrost_SingleEvap, ShouldWriteTrueToWaitingToDefrostErdWhenEnteringPostDwellOnInit)
{
   Given WaitingToDefrostIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   WaitingToDefrostShouldBe(true);
}

TEST(Defrost_SingleEvap, ShouldWriteTrueToWaitingToDefrostErdWhenEnteringPostDwellFromDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);
   WaitingToDefrostShouldBe(false);

   After(defrostData->dwellData.dwellTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
   WaitingToDefrostIs(true);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToDefrostingErdWhenEnteringIdleOnInit)
{
   Given DefrostingIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToDefrostingErdWhenEnteringPostDwellOnInit)
{
   Given DefrostingIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldWriteTrueToDefrostingErdWhenEnteringDwellOnInit)
{
   Given DefrostingIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   DefrostingShouldBe(true);
}

TEST(Defrost_SingleEvap, ShouldWriteTrueToDefrostingErdWhenEnteringHeaterOnEntryOnInit)
{
   Given DefrostingIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   DefrostingShouldBe(true);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToDefrostingErdWhenEnteringPostDwellFromDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);
   DefrostingShouldBe(true);

   After(defrostData->dwellData.dwellTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToWaitingToDefrostErdWhenEnteringPrechillPrepFromIdle)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreshFoodThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   WaitingToDefrostShouldBe(true);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   WaitingToDefrostShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldTransitionFromWaitingToDefrostStateToDisabledWhenDisableIsRequested)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
}

TEST(Defrost_SingleEvap, ShouldTransitionFromDefrostingStateToDisabledWhenDisableIsRequested)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToWaitingToDefrostErdWhenEnteringDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   WaitingToDefrostShouldBe(true);

   When DisableDefrostIs(true);
   WaitingToDefrostShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToDefrostingErdWhenEnteringDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   DefrostingShouldBe(true);

   When DisableDefrostIs(true);
   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldVoteFreezerDefrostHeaterOffWhenTransitioningFromHeaterOnToDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterVoteIs(HeaterState_On);

   When DisableDefrostIs(true);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
}

TEST(Defrost_SingleEvap, ShouldVoteFreezerDefrostHeaterOffWhenTransitioningFromIdleToDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given FreezerDefrostHeaterVoteIs(HeaterState_On);

   When DisableDefrostIs(true);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
}

TEST(Defrost_SingleEvap, ShouldTransitionFromDisabledToIdleWhenEnabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);

   When DisableDefrostIs(false);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldWriteToCurrentDefrostTypeWhenExitingWaitingToDefrost)
{
   Given NextDefrostTypeIs(DefrostType_Full);
   And CurrentDefrostTypeIs(DefrostType_FreshFood);
   And LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreshFoodThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryFromIdleWhenReadyToDefrostAndEepromIsClearedOnStartup)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given ClearedEepromOnStartupIs(true);

   When ReadyToDefrost();
   ClearedEepromOnStartupShouldBe(false);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldSetInvalidFreezerEvaporatorThermistorDuringDefrostWhenTheThermistorIsInvalidAndMaxOnTimeReachesOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldClearInvalidFreezerEvaporatorThermistorDuringDefrostWhenTheThermistorIsValidAndMaxOnTimeReachesOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given InvalidFreezerEvaporatorThermistorDuringDefrostIs(SET);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
}

TEST(Defrost_SingleEvap, ShouldClearInvalidFreezerEvaporatorThermistorDuringDefrostWhenTheThermistorIsValidAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given InvalidFreezerEvaporatorThermistorDuringDefrostIs(SET);

   When FilteredFreezerEvapTemperatureIs(defrostData->heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInIdleState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInIdleState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInIdleState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInPostDwellState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInPostDwellState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInPostDwellState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInPrechillPrepState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInPrechillPrepState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInPrechillPrepState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInPrechillState)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInPrechillState)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInPrechillState)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInHeaterOnEntryState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInHeaterOnEntryState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInHeaterOnEntryState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInHeaterOnState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInHeaterOnState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInHeaterOnState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInDwellState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInDwellState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInDwellState)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST_GROUP(Defrost_DualEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      Defrost_Init(&instance, dataModel, &defrostConfig);
   }
};

TEST(Defrost_DualEvap, ShouldInitialize)
{
   DefrostIsInitialized();
}
