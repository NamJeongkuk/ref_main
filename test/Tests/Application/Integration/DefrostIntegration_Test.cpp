/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "StackConfigurator.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "Vote.h"
#include "Rx2xxResetSource.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"
#include "TddPersonality.h"

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
   TenMinutes = 10,
   ZeroMinutes = 0,
   SomeTimeInMinutes = 60,
   SomeTimeInSeconds = SomeTimeInMinutes * SECONDS_PER_MINUTE,
   SomeTimeInMsec = SomeTimeInSeconds * MSEC_PER_SEC,
   WaitForEepromWritesToCompleteTimeInMsec = 1000,
   SomeSmallTimeInSeconds = 10,
   SomeSmallTimeInMsec = SomeSmallTimeInSeconds * MSEC_PER_SEC
};

enum
{
   SomeCompressorOnTimeInMinutes = 10,
   SomeDoorAccelerationsInSecondsPerSecond = 20
};

enum
{
   SomeCompressorOnTimeInMinutesBetweenMinimumAndMaximumTimeBetweenDefrosts = 200672,
   SomeRightSideFreshFoodScaledDoorAccelerationsInSeconds = 8305,
   SomeFreezerScaledDoorAccelerationInSeconds = 7427
};

enum
{
   Closed = 0,
   Open = 1
};

TEST_GROUP(DefrostIntegration_SingleEvap)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const DefrostData_t *defrostData;
   const CompressorData_t *compressorData;
   EventSubscription_t defrostHsmStateSubscription;
   EventSubscription_t readyToDefrostSubscription;
   const SystemMonitorData_t *systemMonitorData;
   const SabbathData_t *sabbathData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
      systemMonitorData = PersonalityParametricData_Get(dataModel)->systemMonitorData;
      sabbathData = PersonalityParametricData_Get(dataModel)->sabbathData;

      mock().strictOrder();
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void GivenThermistorsAreValidAndFreezerThermistorIsTooWarm()
   {
      GivenFreezerEvaporatorThermistorIsValid();
      GivenFreshFoodThermistorIsValid();
      GivenFreezerThermistorIsValidAndTooWarm();
   }

   void GivenThermistorsAreValidAndFreezerThermistorIsNotTooWarm()
   {
      GivenFreezerEvaporatorThermistorIsValid();
      GivenFreshFoodThermistorIsValid();
      GivenFreezerThermistorIsValidAndNotTooWarm();
   }

   void WhenRefrigeratorResetsWithFreezerTooWarm()
   {
      ReferDataModel_TestDouble_Reset(&dataModelDouble);

      GivenThermistorsAreValidAndFreezerThermistorIsTooWarm();

      WhenApplicationIsInitialized();
   }

   void GivenRefrigeratorResetsWithFreezerTooWarm()
   {
      WhenRefrigeratorResetsWithFreezerTooWarm();
   }

   void WhenRefrigeratorResetsWithFreezerNotTooWarm()
   {
      ReferDataModel_TestDouble_Reset(&dataModelDouble);

      GivenThermistorsAreValidAndFreezerThermistorIsNotTooWarm();

      WhenApplicationIsInitialized();
   }

   void WhenRefrigeratorResetsWithNewParametricThatHasShorterTimeBetweenDefrosts()
   {
      ReferDataModel_TestDouble_Reset(&dataModelDouble, TddPersonality_DevelopmentSingleEvaporatorShorterTimeBetweenDefrosts);

      GivenThermistorsAreValidAndFreezerThermistorIsNotTooWarm();

      WhenApplicationIsInitialized();
   }

   void WhenApplicationIsInitialized()
   {
      GivenApplicationHasBeenInitialized();
   }

   static void DefrostHsmStateHasChanged(void *context, const void *args)
   {
      IGNORE(context);
      const DefrostHsmState_t *state = (const DefrostHsmState_t *)args;

      mock().actualCall("DefrostHsmStateHasChanged").withParameter("state", *state);
   }

   void GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState()
   {
      EventSubscription_Init(
         &defrostHsmStateSubscription,
         NULL,
         DefrostHsmStateHasChanged);

      DataModel_Subscribe(
         dataModel,
         Erd_DefrostHsmState,
         &defrostHsmStateSubscription);
   }

   void TheDefrostHsmStateShouldChangeTo(DefrostHsmState_t state)
   {
      mock().expectOneCall("DefrostHsmStateHasChanged").withParameter("state", state);
   }

   void TheDefrostHsmStateShouldNotChange()
   {
      mock().expectNoCall("DefrostHsmStateHasChanged");
   }

   static void ReadyToDefrostHasChangedStates(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_ReadyToDefrost)
      {
         const bool *state = (const bool *)args->data;

         mock().actualCall("ReadyToDefrostHasChangedStates").withParameter("state", *state);
      }
   }

   void GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo()
   {
      EventSubscription_Init(
         &readyToDefrostSubscription,
         NULL,
         ReadyToDefrostHasChangedStates);

      Event_Subscribe(
         dataModel->OnDataChange,
         &readyToDefrostSubscription);
   }

   void ReadyToDefrostShouldChangeTo(bool state)
   {
      mock().expectOneCall("ReadyToDefrostHasChangedStates").withParameter("state", state);
   }

   void GivenDefrostStateWas(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void GivenFreezerEvaporatorThermistorIsValid()
   {
      AdcCounts_t validCounts = 26432;
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_AdcCount, &validCounts);
   }

   void GivenFreezerEvaporatorThermistorIsInvalid()
   {
      AdcCounts_t invalidCounts = 0;
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_AdcCount, &invalidCounts);
   }

   void GivenFreshFoodThermistorIsValid()
   {
      AdcCounts_t validCounts = 26432;
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_AdcCount, &validCounts);
   }

   void GivenFreezerThermistorIsValidAndNotTooWarm()
   {
      AdcCounts_t validCounts = 19072; // 4.96 F
      DataModel_Write(dataModel, Erd_FreezerThermistor_AdcCount, &validCounts);
   }

   void GivenFreezerThermistorIsInvalid()
   {
      AdcCounts_t invalidCounts = 0;
      DataModel_Write(dataModel, Erd_FreezerThermistor_AdcCount, &invalidCounts);
   }

   void WhenTheFreezerEvaporatorThermistorIsGreaterThanPrechillExitTemperature()
   {
      AdcCounts_t abovePrechillExitTemperatureCounts = 9728; // -2117 > -3000
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_AdcCount, &abovePrechillExitTemperatureCounts);
   }

   void WhenTheFreezerThermistorIsGreaterThanPrechillFreezerMinTemperature()
   {
      AdcCounts_t abovePrechillMinTemperatureCounts = 19072; // 496 > -600
      DataModel_Write(dataModel, Erd_FreezerThermistor_AdcCount, &abovePrechillMinTemperatureCounts);
   }

   void WhenTheFreshFoodThermistorIsInvalid()
   {
      AdcCounts_t abovePrechillFreshFoodMinTemperatureCounts = 61000;
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_AdcCount, &abovePrechillFreshFoodMinTemperatureCounts);
   }

   void CompressorOnTimeInSecondsShouldBe(uint32_t expectedSeconds)
   {
      uint32_t actualSeconds;
      DataModel_Read(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &actualSeconds);

      CHECK_EQUAL(expectedSeconds, actualSeconds);
   }

   void FreezerScaledDoorAccelerationsInSecondsShouldBe(uint32_t expectedSeconds)
   {
      uint32_t actualSeconds;
      DataModel_Read(dataModel, Erd_FreezerScaledDoorAccelerationInSeconds, &actualSeconds);

      CHECK_EQUAL(expectedSeconds, actualSeconds);
   }

   void RightSideFreshFoodScaledDoorAccelerationsInSecondsShouldBe(uint32_t expectedSeconds)
   {
      uint32_t actualSeconds;
      DataModel_Read(dataModel, Erd_RightSideFreshFoodScaledDoorAccelerationInSeconds, &actualSeconds);

      CHECK_EQUAL(expectedSeconds, actualSeconds);
   }

   void CompressorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorIsOn, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ReadyToDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_ReadyToDefrost, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CoolingModeShouldBe(CoolingMode_t expected)
   {
      CoolingMode_t actual;
      DataModel_Read(dataModel, Erd_CoolingMode, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CompressorSpeedWinningVoteShouldBe(Erd_t expected)
   {
      WinningVoteErd_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeed_WinningVoteErd, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void CompressorSpeedDefrostVoteShouldBe(CompressorVotedSpeed_t expected)
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void CompressorSpeedDefrostVoteShouldBeOffAndDontCare()
   {
      CompressorVotedSpeed_t compressorDefrostVote = { .speed = CompressorSpeed_Off, .care = Vote_DontCare };
      CompressorSpeedDefrostVoteShouldBe(compressorDefrostVote);
   }

   void WhenItBecomesReadyToDefrostAfterMaxTimeBetweenDefrostsWithCompressorOn()
   {
      WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
      CoolingModeShouldBe(CoolingMode_Freezer);

      After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN);
   }

   void WhenFilteredFreezerEvapTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureInDegFx100, &temperature);
   }

   void GivenAllPreviousDefrostsWereNormal()
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, clear);
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, clear);
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, clear);
   }

   void GivenFreezerDefrostWasAbnormal()
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, set);
   }

   void FreezerDefrostWasAbnormalShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostWasAbnormal, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenFreezerThermistorIsValidAndTooWarm()
   {
      AdcCounts_t validCounts = 47040; // 70.41 F
      DataModel_Write(dataModel, Erd_FreezerThermistor_AdcCount, &validCounts);
   }

   void FreezerCompartmentTemperatureShouldNotBeTooWarmOnPowerUp()
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUp, &actual);

      CHECK_FALSE(actual);
   }

   void FreezerCompartmentTemperatureShouldBeTooWarmOnPowerUp()
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUp, &actual);

      CHECK_TRUE(actual);
   }

   void GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_InvalidFreezerEvaporatorThermistorDuringDefrost, &state);
   }

   void GivenEepromWasNotClearedAtStartup()
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);
   }

   void GivenEepromWasClearedAtStartup()
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, set);
   }

   void GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle()
   {
      GivenThermistorsAreValidAndFreezerThermistorIsNotTooWarm();
      GivenAllPreviousDefrostsWereNormal();
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
      GivenEepromWasNotClearedAtStartup();

      GivenDefrostStateWas(DefrostState_Idle);
      GivenApplicationHasBeenInitialized();

      FreezerCompartmentTemperatureShouldNotBeTooWarmOnPowerUp();
      DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   }

   void GivenThatTheApplicationHasStartedWithInvalidFreezerThermistorAndDefrostIsInIdle()
   {
      GivenFreezerEvaporatorThermistorIsValid();
      GivenFreshFoodThermistorIsValid();
      GivenFreezerThermistorIsInvalid();
      GivenAllPreviousDefrostsWereNormal();
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
      GivenEepromWasNotClearedAtStartup();

      GivenDefrostStateWas(DefrostState_Idle);
      GivenApplicationHasBeenInitialized();

      FreezerCompartmentTemperatureShouldNotBeTooWarmOnPowerUp();
      DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   }

   void GivenThatTheApplicationHasStartedWithInvalidFreezerEvaporatorThermistorAndDefrostIsInIdle()
   {
      GivenFreezerEvaporatorThermistorIsInvalid();
      GivenFreshFoodThermistorIsValid();
      GivenFreezerThermistorIsValidAndNotTooWarm();
      GivenAllPreviousDefrostsWereNormal();
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
      GivenEepromWasNotClearedAtStartup();

      GivenDefrostStateWas(DefrostState_Idle);
      GivenApplicationHasBeenInitialized();

      FreezerCompartmentTemperatureShouldNotBeTooWarmOnPowerUp();
      DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   }

   void GivenThatTheApplicationHasStartedAndDefrostIsInPrechillPrep()
   {
      GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();

      WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();

      After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
         defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC);

      WhenGridVotesToTurnOffCompressorAndWinsAndDoesNotHaveToWaitMinimumOffTimeBecauseItAlreadyPassed();

      WhenFreezerDoorIs(Open);
      DefrostHsmStateShouldBe(DefrostHsmState_Idle);

      After(1 * MSEC_PER_SEC);
      DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   }

   void GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry()
   {
      GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();

      WhenItBecomesReadyToDefrostAfterMaxTimeBetweenDefrostsWithCompressorOn();
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
      CompressorOnTimeInSecondsShouldBe(0);

      WhenGridVotesToTurnOffCompressorWhileMinimumTimesAreDisabledAndGridWins();
   }

   void GivenThatTheApplicationHasStartedWithInvalidFreezerThermistorAndDefrostIsInHeaterOnEntry()
   {
      GivenThatTheApplicationHasStartedWithInvalidFreezerThermistorAndDefrostIsInIdle();

      WhenItBecomesReadyToDefrostAfterMaxTimeBetweenDefrostsWithCompressorOn();
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
      CompressorOnTimeInSecondsShouldBe(0);

      WhenGridVotesToTurnOffCompressorWhileMinimumTimesAreDisabledAndGridWins();
   }

   void GivenThatTheApplicationHasStartedWithInvalidFreezerEvaporatorThermistorAndDefrostIsInHeaterOnEntry()
   {
      GivenThatTheApplicationHasStartedWithInvalidFreezerEvaporatorThermistorAndDefrostIsInIdle();

      WhenItBecomesReadyToDefrostAfterMaxTimeBetweenDefrostsWithCompressorOn();
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
      CompressorOnTimeInSecondsShouldBe(0);

      WhenGridVotesToTurnOffCompressorWhileMinimumTimesAreDisabledAndGridWins();
   }

   void WhenDefrostTestIsRequested(DefrostTestRequest_t request)
   {
      DataModel_Write(dataModel, Erd_DefrostTestRequest, &request);
   }

   void UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void DontSkipPrechillShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DontSkipDefrostPrechill, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsHasBeenSetTo(bool state)
   {
      DataModel_Write(dataModel, Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts, &state);
   }

   void DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime()
   {
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOn);
      After(defrostData->heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   }

   void DefrostExitsHeaterOnAndIsInDwellBecauseFreezerEvaporatorReachesTerminationTemperature()
   {
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Dwell);
      WhenFilteredFreezerEvapTemperatureChangesTo(defrostData->heaterOnData.freezerDefrostTerminationTemperatureInDegFx100 - 1);
      WhenFilteredFreezerEvapTemperatureChangesTo(defrostData->heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
      DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
   }

   void DefrostExitsHeaterOnAndIsInDwellAfterMaxTimeout()
   {
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Dwell);
      After(defrostData->heaterOnData.freezerDefrostHeaterMaxOnTimeInMinutes * MSEC_PER_MIN);
      DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
   }

   void DefrostExitsDwellAndIsInPostDwellAfterDwellTime()
   {
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PostDwell);
      After(defrostData->dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
      DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
   }

   void DefrostExitsPostDwellAndIsInIdleAfterPostDwellTime()
   {
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
      After(defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
      DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   }

   void DefrostTransitedFromHeaterOnEntryToHeaterOnToDwellToPostDwellToIdleAndWasAbnormal()
   {
      DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime();
      DefrostExitsHeaterOnAndIsInDwellAfterMaxTimeout();
      FreezerDefrostWasAbnormalShouldBe(true);
      CompressorOnTimeInSecondsShouldBe(0);

      DefrostExitsDwellAndIsInPostDwellAfterDwellTime();
      DefrostExitsPostDwellAndIsInIdleAfterPostDwellTime();
   }

   void DefrostTransitedFromHeaterOnEntryToHeaterOnToDwellToPostDwellToIdleAndWasNormal()
   {
      DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime();
      DefrostExitsHeaterOnAndIsInDwellBecauseFreezerEvaporatorReachesTerminationTemperature();
      FreezerDefrostWasAbnormalShouldBe(false);
      CompressorOnTimeInSecondsShouldBe(0);

      DefrostExitsDwellAndIsInPostDwellAfterDwellTime();
      DefrostExitsPostDwellAndIsInIdleAfterPostDwellTime();
   }

   void GivenAhamPrechillTestHasBeenRequestedDuringDefrostAndHasNotSkippedPrechillPrepAndIsNowInHeaterOnEntry()
   {
      GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
      GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

      WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
      UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
      DontSkipPrechillShouldBe(true);

      DefrostTransitedFromHeaterOnEntryToHeaterOnToDwellToPostDwellToIdleAndWasAbnormal();

      WhenGridVotesToTurnOnCompressorWhileCompressorMinimumTimesAreEnabledAfterBeingOffAndGridWinsAndGridRanThisManyTimes(2);

      ReadyToDefrostShouldBe(false);
      UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
      DontSkipPrechillShouldBe(true);

      // Should use aham prechill time to exit Idle
      // Should not skip prechill prep (if it didn't use the flag, it would have gone to heater on entry)
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
      UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
      After(defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
      UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(false);
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
      ReadyToDefrostShouldBe(true);
   }

   void GivenEepromCompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_Eeprom_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void EepromCompressorOnTimeInSecondsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(dataModel, Erd_Eeprom_DefrostCompressorOnTimeInSeconds, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenEepromFreezerScaledDoorAccelerationInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_Eeprom_FreezerScaledDoorAccelerationInSeconds, &seconds);
   }

   void GivenEepromRightSideFreshFoodScaledDoorAccelerationInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_Eeprom_RightSideFreshFoodScaledDoorAccelerationInSeconds, &seconds);
   }

   void GivenThatTheApplicationHasStartedAndDefrostIsInPostDwell()
   {
      GivenThermistorsAreValidAndFreezerThermistorIsNotTooWarm();
      GivenAllPreviousDefrostsWereNormal();
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
      GivenEepromWasNotClearedAtStartup();

      GivenDefrostStateWas(DefrostState_Dwell);
      GivenApplicationHasBeenInitialized();

      FreezerCompartmentTemperatureShouldNotBeTooWarmOnPowerUp();
      DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

      mock().disable();
      DefrostExitsDwellAndIsInPostDwellAfterDwellTime();
      DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
      mock().enable();
   }

   void WhenFreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreezerDoorStatusResolved, &state);
   }

   void WhenRightSideFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightSideFreshFoodDoorStatusResolved, &state);
   }

   void WhenGridRunsByWaitingOneSecond()
   {
      After(MSEC_PER_SEC);
   }

   void WhenFactoryVotesForSetpointsToCauseCompressorToTurnOn()
   {
      SetpointVotedTemperature_t freezerSetpointVote;
      freezerSetpointVote.temperatureInDegFx100 = -3000;
      freezerSetpointVote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_FreezerSetpoint_FactoryVote, &freezerSetpointVote);

      SetpointVotedTemperature_t freshFoodSetpointVote;
      freshFoodSetpointVote.temperatureInDegFx100 = 2000;
      freshFoodSetpointVote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_FreshFoodSetpoint_FactoryVote, &freshFoodSetpointVote);
   }

   void WhenFactoryVotesForSetpointsToCauseCompressorToTurnOff()
   {
      SetpointVotedTemperature_t freezerSetpointVote;
      freezerSetpointVote.temperatureInDegFx100 = 1000;
      freezerSetpointVote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_FreezerSetpoint_FactoryVote, &freezerSetpointVote);

      SetpointVotedTemperature_t freshFoodSetpointVote;
      freshFoodSetpointVote.temperatureInDegFx100 = 5000;
      freshFoodSetpointVote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_FreshFoodSetpoint_FactoryVote, &freshFoodSetpointVote);
   }

   void WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins()
   {
      WhenFactoryVotesForSetpointsToCauseCompressorToTurnOn();
      WhenGridRunsByWaitingOneSecond();

      After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      CompressorShouldBe(OFF);

      After(1);
      CompressorShouldBe(ON);
   }

   void GivenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins()
   {
      WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
   }

   void WhenGridVotesToTurnOnCompressorWhileCompressorMinimumTimesAreEnabledAfterBeingOffAndGridWinsAndGridRanThisManyTimes(uint8_t numberOfGridRuns)
   {
      WhenFactoryVotesForSetpointsToCauseCompressorToTurnOn();
      WhenGridRunsByWaitingOneSecond();

      After(compressorData->compressorTimes.minimumOffTimeInMinutes * MSEC_PER_MIN - numberOfGridRuns * MSEC_PER_SEC);
      After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC);
      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      CompressorShouldBe(OFF);

      After(1);
      CompressorShouldBe(ON);
   }

   void WhenGridVotesToTurnOnCompressorWhileMinimumTimesAreDisabledAndGridWins()
   {
      WhenFactoryVotesForSetpointsToCauseCompressorToTurnOn();
      WhenGridRunsByWaitingOneSecond();
      CompressorShouldBe(ON);
   }

   void WhenGridVotesToTurnOffCompressorAndWinsAndDoesNotHaveToWaitMinimumOffTimeBecauseItAlreadyPassed()
   {
      WhenFactoryVotesForSetpointsToCauseCompressorToTurnOff();
      WhenGridRunsByWaitingOneSecond();
      CompressorShouldBe(OFF);
   }

   void WhenGridVotesToTurnOffCompressorWhileMinimumTimesAreDisabledAndGridWins()
   {
      WhenFactoryVotesForSetpointsToCauseCompressorToTurnOff();
      WhenGridRunsByWaitingOneSecond();
      CompressorShouldBe(OFF);
   }

   void NextDefrostTypeShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_NextDefrostType, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerDefrostHeaterShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeaterRelay, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenAhamPrechillTestRequestRanAndUnitJustFinishedPostDwell()
   {
      GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
      GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

      WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
      CoolingModeShouldBe(CoolingMode_Freezer);

      After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN);
      UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(false);
      DontSkipPrechillShouldBe(false);

      WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
      UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
      DontSkipPrechillShouldBe(true);
      CompressorOnTimeInSecondsShouldBe(0);

      // Should use aham prechill time to exit Idle
      // Should not skip prechill prep (if it didn't use the flag, it would have gone to heater on entry)
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
      After(defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN);
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
      ReadyToDefrostShouldBe(true);

      DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime();
      DefrostExitsHeaterOnAndIsInDwellBecauseFreezerEvaporatorReachesTerminationTemperature();
      DefrostExitsDwellAndIsInPostDwellAfterDwellTime();
      DefrostExitsPostDwellAndIsInIdleAfterPostDwellTime();
   }

   void WhenCompressorIsOnForMinimumTimeBetweenDefrosts()
   {
      WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
      After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN);
   }

   void WhenCompressorIsOnForMinimumTimePlusSomeTime()
   {
      WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
      After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN + SomeTimeInMsec);
   }

   void WhenRightSideFreshFoodDoorIsOpenForSomeSmallTime()
   {
      WhenRightSideFreshFoodDoorIs(Open);
      After(SomeSmallTimeInMsec);
      WhenRightSideFreshFoodDoorIs(Closed);
   }

   void WhenCompressorTurnsOnAfterSomeSmallTimeOfBeingOff()
   {
      WhenGridVotesToTurnOnCompressorWhileCompressorMinimumTimesAreEnabledAfterBeingOffAndGridWinsAndGridRanThisManyTimes(SomeSmallTimeInMsec / MSEC_PER_SEC + 1);
   }

   void WhenFreezerDoorIsOpenForSomeSmallTime()
   {
      WhenFreezerDoorIs(Open);
      After(SomeSmallTimeInMsec);
      WhenFreezerDoorIs(Closed);
   }

   void SabbathIsReadyToDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_SabbathIsReadyToDefrost, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SabbathModeEnable, &state);
   }

   void WhenSabbathModeIs(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void TheNormalTimeBetweenDefrostsShouldBe(uint16_t expectedTime)
   {
      uint16_t actualTime;
      DataModel_Read(dataModel, Erd_TimeBetweenDefrostsInMinutes, &actualTime);

      CHECK_EQUAL(expectedTime, actualTime);
   }

   void TheNormalTimeBetweenDefrostsShouldBeLessThanSabbathTimeBetweenDefrost()
   {
      uint16_t sabbathTime;
      DataModel_Read(dataModel, Erd_SabbathTimeBetweenDefrostsInMinutes, &sabbathTime);

      uint16_t normalTime;
      DataModel_Read(dataModel, Erd_TimeBetweenDefrostsInMinutes, &normalTime);

      CHECK_TRUE(sabbathTime > normalTime);
   }

   void CurrentDefrostTypeShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTimeBetweenDefrostsMeetsMaxTimeBetweenDefrostsWithCompressorOnTimeAndFreezerDoorOpenAndIsInPrechillPrep()
   {
      WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();

      After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
         defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC);

      WhenGridVotesToTurnOffCompressorAndWinsAndDoesNotHaveToWaitMinimumOffTimeBecauseItAlreadyPassed();

      WhenFreezerDoorIs(Open);
      DefrostHsmStateShouldBe(DefrostHsmState_Idle);

      After(1 * MSEC_PER_SEC);
      DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   }

   void RunTimerModuleToSaveErdInEeprom()
   {
      After(100);
   }
};

TEST(DefrostIntegration_SingleEvap, ShouldInitialize)
{
   GivenApplicationHasBeenInitialized();
}

TEST(DefrostIntegration_SingleEvap, ShouldStartInDefrostHsmStateIdle)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
}

TEST(DefrostIntegration_SingleEvap, ShouldStartInDefrostHsmStateHeaterOnEntry)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
}

TEST(DefrostIntegration_SingleEvap, ShouldStartInDefrostHsmStatePostDwell)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInPostDwell();
}

TEST(DefrostIntegration_SingleEvap, ShouldStartInDefrostHsmStatePrechillPrep)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInPrechillPrep();
}

TEST(DefrostIntegration_SingleEvap, ShouldRunTheAhamPrechillTestRequestByEnteringPrechillPrepAfterMinimumRunTimeWhenRequestedWhileDefrostingOnlyUsingCompressorOnTimeDuringIdle)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
   DontSkipPrechillShouldBe(true);

   DefrostTransitedFromHeaterOnEntryToHeaterOnToDwellToPostDwellToIdleAndWasAbnormal();

   WhenGridVotesToTurnOnCompressorWhileCompressorMinimumTimesAreEnabledAfterBeingOffAndGridWinsAndGridRanThisManyTimes(2);

   ReadyToDefrostShouldBe(false);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
   DontSkipPrechillShouldBe(true);

   // Should use aham prechill time to exit Idle
   // Should not skip prechill prep (if it didn't use the flag, it would have gone to heater on entry)
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldResumeNormalDefrostAfterAhamPrechillTest)
{
   GivenAhamPrechillTestHasBeenRequestedDuringDefrostAndHasNotSkippedPrechillPrepAndIsNowInHeaterOnEntry();

   DontSkipPrechillShouldBe(false);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(false);

   DefrostTransitedFromHeaterOnEntryToHeaterOnToDwellToPostDwellToIdleAndWasNormal();
   CompressorShouldBe(ON); // grid voted for compressor on earlier, and once defrost released control, it's still on

   DontSkipPrechillShouldBe(false);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(false);

   // goes to prechill prep b / c the defrost was normal
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldResumeAbnormalDefrostAfterAhamPrechillTestWhenFreezerDefrostWasAbnormalAfterAbnormalDefrostDuringTest)
{
   GivenAhamPrechillTestHasBeenRequestedDuringDefrostAndHasNotSkippedPrechillPrepAndIsNowInHeaterOnEntry();

   DontSkipPrechillShouldBe(false);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(false);

   DefrostTransitedFromHeaterOnEntryToHeaterOnToDwellToPostDwellToIdleAndWasAbnormal();
   CompressorShouldBe(ON);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldRunTheAhamPrechillTestRequestByEnteringPrechillPrepAfterAhamPrechillTimeWhenRequestedAtTheStartOfIdleOnlyUsingCompressorOnTimeDuringIdle)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
   DontSkipPrechillShouldBe(true);

   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
   CoolingModeShouldBe(CoolingMode_Freezer);

   // Should use aham prechill time to exit Idle
   // Should not skip prechill prep (if it didn't use the flag, it would have gone to heater on entry)
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldRunTheAhamPrechillTestRequestByEnteringPrechillPrepAfterAhamPrechillTimeWhenRequestedWhileInIdleOnlyUsingCompressorOnTimeDuringIdle)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
   CoolingModeShouldBe(CoolingMode_Freezer);

   After(SomeTimeInMinutes * MSEC_PER_MIN);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(false);
   DontSkipPrechillShouldBe(false);

   WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
   DontSkipPrechillShouldBe(true);
   CompressorOnTimeInSecondsShouldBe(0);

   // Should use aham prechill time to exit Idle
   // Should not skip prechill prep (if it didn't use the flag, it would have gone to heater on entry)
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldRunTheAhamPrechillTestRequestByEnteringPrechillPrepAfterAhamPrechillTimeWhenRequestedWhileInIdleAndHavingAlreadyReachedMinimumTimeOnlyUsingCompressorOnTimeDuringIdle)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
   CoolingModeShouldBe(CoolingMode_Freezer);

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(false);
   DontSkipPrechillShouldBe(false);

   WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
   DontSkipPrechillShouldBe(true);
   CompressorOnTimeInSecondsShouldBe(0);

   // Should use aham prechill time to exit Idle
   // Should not skip prechill prep (if it didn't use the flag, it would have gone to heater on entry)
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldExitIdleBasedOnMaximumTimeAfterCompletingAhamPrechillTestRequest)
{
   GivenAhamPrechillTestRequestRanAndUnitJustFinishedPostDwell();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes - defrostData->postDwellData.postDwellExitTimeInMinutes) * MSEC_PER_MIN);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(false);
}

TEST(DefrostIntegration_SingleEvap, ShouldRunTheAhamPrechillTestRequestByEnteringPrechillPrepAfterAhamPrechillTimeWhenRequestedWhileInPostDwellOnlyUsingCompressorOnTimeDuringIdle)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInPostDwell();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   CompressorShouldBe(ON);
   CoolingModeShouldBe(CoolingMode_Freezer);

   After(defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN / 2);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenGridVotesToTurnOnCompressorWhileMinimumTimesAreDisabledAndGridWins();

   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(false);
   DontSkipPrechillShouldBe(false);

   WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
   DontSkipPrechillShouldBe(true);
   CompressorOnTimeInSecondsShouldBe(0);
   CompressorShouldBe(ON);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   After(defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN / 2);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN / 2);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldSetReadyToDefrostErdWhenItBecomesReadyToDefrostAfterMaxTimeBetweenDefrostsWithCompressorOnWhileInADefrostingStateWithCompressorOnAndThenClearReadyToDefrostOnceItStartsWaitingToDefrostAgain)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo();

   DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime();
   DefrostExitsHeaterOnAndIsInDwellBecauseFreezerEvaporatorReachesTerminationTemperature();

   ReadyToDefrostShouldChangeTo(false);
   DefrostExitsDwellAndIsInPostDwellAfterDwellTime();
   DefrostExitsPostDwellAndIsInIdleAfterPostDwellTime();

   WhenGridVotesToTurnOnCompressorWhileCompressorMinimumTimesAreEnabledAfterBeingOffAndGridWinsAndGridRanThisManyTimes(1);

   ReadyToDefrostShouldChangeTo(true);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);

   After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);

   DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime();
   DefrostExitsHeaterOnAndIsInDwellBecauseFreezerEvaporatorReachesTerminationTemperature();

   ReadyToDefrostShouldChangeTo(false);
   DefrostExitsDwellAndIsInPostDwellAfterDwellTime();
}

TEST(DefrostIntegration_SingleEvap, ShouldNotClearUseAhamPrechillTimeErdOnInitIfSet)
{
   GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsHasBeenSetTo(true);
   GivenApplicationHasBeenInitialized();
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldStartDefrostingOnInitIfSavedEepromCompressorOnTimeIsMinimumTimeBetweenDefrostsAndConditionsAreSuchThatTimeBetweenDefrostsIsMinimumTimeAndItPowersUpInIdle)
{
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo();

   GivenEepromCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   GivenThermistorsAreValidAndFreezerThermistorIsNotTooWarm();
   GivenAllPreviousDefrostsWereNormal();
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenEepromWasClearedAtStartup();

   GivenDefrostStateWas(DefrostState_Idle);
   GivenApplicationHasBeenInitialized();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldChangeTo(true);
   After(0);
}

TEST(DefrostIntegration_SingleEvap, ShouldStartDefrostingOnInitIfSavedEepromCompressorOnTimeIsMinimumTimeBetweenDefrostsAndDoorAccelerationTotalIsMaxTimeBetweenDefrostsMinusMinimumTimeBetweenDefrostsAndConditionsAreSuchThatTimeBetweenDefrostsIsMaximumTimeAndItPowersUpInIdle)
{
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo();

   GivenEepromCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenEepromFreezerScaledDoorAccelerationInSecondsIs((defrostData->idleData.maxTimeBetweenDefrostsInMinutes - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) * SECONDS_PER_MINUTE);

   GivenThermistorsAreValidAndFreezerThermistorIsNotTooWarm();
   GivenAllPreviousDefrostsWereNormal();
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenEepromWasNotClearedAtStartup();

   GivenDefrostStateWas(DefrostState_Idle);
   GivenApplicationHasBeenInitialized();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   ReadyToDefrostShouldChangeTo(true);
   After(0);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotReenterIdleAndShouldNotRestartCompressorOnTimeCounterOrDoorAccelerationCounterWhenIdleTestRequestedDuringIdle)
{
   GivenEepromCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInMinutes);
   GivenEepromFreezerScaledDoorAccelerationInSecondsIs(SomeDoorAccelerationsInSecondsPerSecond);
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInMinutes);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(SomeDoorAccelerationsInSecondsPerSecond);

   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInMinutes);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(SomeDoorAccelerationsInSecondsPerSecond);

   WhenDefrostTestIsRequested(DefrostTestRequest_Idle);
   WhenFreezerDoorIs(Open);

   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInMinutes);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(SomeDoorAccelerationsInSecondsPerSecond);

   After(1 * MSEC_PER_SEC);

   WhenFreezerDoorIs(Closed);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInMinutes + 1);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(SomeDoorAccelerationsInSecondsPerSecond + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(DefrostIntegration_SingleEvap, ShouldTransitionToIdleAndStartCountingCompressorOnTimeAndDoorAccelerationsWhenIdleTestRequestedDuringNonIdleState)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   WhenDefrostTestIsRequested(DefrostTestRequest_Idle);

   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
   WhenFreezerDoorIs(Open);

   CompressorOnTimeInSecondsShouldBe(0);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(0);

   After(1 * MSEC_PER_SEC);

   WhenFreezerDoorIs(Closed);
   CompressorOnTimeInSecondsShouldBe(1);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(DefrostIntegration_SingleEvap, ShouldTransitionToHeaterOnEntryWhenDefrostTestRequestedDuringWaitingToDefrostStateAndDoAFullDefrost)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Defrost);

   NextDefrostTypeShouldBe(DefrostType_Full);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOn);
   After(defrostData->heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
   FreezerDefrostHeaterShouldBe(ON);
}

TEST(DefrostIntegration_SingleEvap, ShouldTransitionToHeaterOnEntryWhenDefrostTestRequestedDuringDefrostingStateAndDoAFullDefrost)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInPrechillPrep();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Defrost);

   NextDefrostTypeShouldBe(DefrostType_Full);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOn);
   After(defrostData->heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
   FreezerDefrostHeaterShouldBe(ON);
}

TEST(DefrostIntegration_SingleEvap, ShouldReenterHeaterOnEntryWhenDefrostTestRequestedDuringHeaterOnStateAndDoAFullDefrost)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Defrost);

   NextDefrostTypeShouldBe(DefrostType_Full);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOn);
   After(defrostData->heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
   FreezerDefrostHeaterShouldBe(ON);
}

TEST(DefrostIntegration_SingleEvap, ShouldTransitionToPrechillPrepWhenPrechillTestRequestedDuringWaitingToDefrostStateAndDoAFullDefrost)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   WhenDefrostTestIsRequested(DefrostTestRequest_Prechill);

   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotTransitionToPrechillPrepButToHeaterOnEntryWhenPrechillTestRequestedAndFreezerThermistorIsInvalidDuringWaitingToDefrostState)
{
   GivenThatTheApplicationHasStartedWithInvalidFreezerThermistorAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Prechill);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotTransitionToPrechillPrepButToHeaterOnEntryWhenPrechillTestRequestedAndFreezerEvaporatorThermistorIsInvalidDuringWaitingToDefrostState)
{
   GivenThatTheApplicationHasStartedWithInvalidFreezerEvaporatorThermistorAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Prechill);
}

TEST(DefrostIntegration_SingleEvap, ShouldTransitionToPrechillPrepWhenPrechillTestRequestedDuringDefrostingStateAndDoAFullDefrost)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   WhenDefrostTestIsRequested(DefrostTestRequest_Prechill);

   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotTransitionToPrechillPrepButToHeaterOnEntryWhenPrechillTestRequestedAndFreezerThermistorIsInvalidDuringDefrostingState)
{
   GivenThatTheApplicationHasStartedWithInvalidFreezerThermistorAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Prechill);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotTransitionToPrechillPrepButToHeaterOnEntryWhenPrechillTestRequestedAndFreezerEvaporatorThermistorIsInvalidDuringDefrostingState)
{
   GivenThatTheApplicationHasStartedWithInvalidFreezerEvaporatorThermistorAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenFreezerEvaporatorThermistorIsValid();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Prechill);
}

TEST(DefrostIntegration_SingleEvap, ShouldTransitionToIdleWhenExitingDisabledStateAndStartCountingCompressorOnTimeAndDoorAccelerationsFromZeroWhenItWasInIdleBeforeBeingDisabled)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   CompressorOnTimeInSecondsShouldBe(0);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(0);

   WhenFreezerDoorIs(Open);
   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();

   WhenFreezerDoorIs(Closed);
   CompressorOnTimeInSecondsShouldBe(0);
   FreezerScaledDoorAccelerationsInSecondsShouldBe((1 + compressorData->compressorTimes.sabbathDelayTimeInSeconds +
                                                      compressorData->compressorTimes.startupOnTimeInSeconds) *
      defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Disabled);
   WhenDefrostTestIsRequested(DefrostTestRequest_Disable);

   WhenFreezerDoorIs(Open);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   WhenDefrostTestIsRequested(DefrostTestRequest_Enable);

   After(1 * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(0);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(0);

   After(1);
   WhenFreezerDoorIs(Closed);
   CompressorOnTimeInSecondsShouldBe(1);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(DefrostIntegration_SingleEvap, ShouldTransitionToIdleWhenExitingDisabledStateAndStartCountingCompressorOnTimeAndDoorAccelerationsFromZeroWhenItWasInPostDwellBeforeBeingDisabled)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInPostDwell();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   CompressorOnTimeInSecondsShouldBe(0);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(0);

   WhenFreezerDoorIs(Open);

   After(1 * MSEC_PER_SEC);

   WhenFreezerDoorIs(Closed);
   CompressorOnTimeInSecondsShouldBe(1);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(1 * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);

   WhenGridVotesToTurnOnCompressorWhileMinimumTimesAreDisabledAndGridWins();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Disabled);
   WhenDefrostTestIsRequested(DefrostTestRequest_Disable);

   WhenFreezerDoorIs(Open);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   WhenDefrostTestIsRequested(DefrostTestRequest_Enable);

   After(1 * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(0);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(0);

   After(1);

   WhenFreezerDoorIs(Closed);
   CompressorOnTimeInSecondsShouldBe(1);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotBeAbleToEnableDefrostThatIsAlreadyEnabled)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldNotChange();
   WhenDefrostTestIsRequested(DefrostTestRequest_Enable);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotClearAbnormalFreezerDefrostFlagWhenDefrostTestIsRequested)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
   GivenFreezerDefrostWasAbnormal();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Defrost);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOn);
   After(defrostData->heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
   FreezerDefrostHeaterShouldBe(ON);
   FreezerDefrostWasAbnormalShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldStartDefrostingFromZeroWhenResettingWithFreezerTooWarmAfterHavingResetWhileWaitingForADefrostAfterHavingAlreadyReachedMinimumTimeBetweenDefrosts)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   WhenCompressorIsOnForMinimumTimeBetweenDefrosts();
   WhenGridVotesToTurnOffCompressorAndWinsAndDoesNotHaveToWaitMinimumOffTimeBecauseItAlreadyPassed();

   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   After(systemMonitorData->periodicNvUpdateInMinutes * MSEC_PER_MIN);
   EepromCompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);

   WhenRefrigeratorResetsWithFreezerTooWarm();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo();

   EepromCompressorOnTimeInSecondsShouldBe(0);
   CompressorOnTimeInSecondsShouldBe(0);
   FreezerCompartmentTemperatureShouldBeTooWarmOnPowerUp();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();

   ReadyToDefrostShouldChangeTo(true);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN);
}

TEST(DefrostIntegration_SingleEvap, ShouldContinueWaitingToDefrostWhereItLeftOffIfFreezerIsNotTooWarmAndThereIsAccumulatedCompressorOnTimeAndItResetsWhileInIdle)
{
   GivenEepromCompressorOnTimeInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - SomeTimeInSeconds);
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   EepromCompressorOnTimeInSecondsShouldBe(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - SomeTimeInSeconds);
   After(WaitForEepromWritesToCompleteTimeInMsec);

   WhenRefrigeratorResetsWithFreezerNotTooWarm();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo();

   EepromCompressorOnTimeInSecondsShouldBe(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - SomeTimeInSeconds);
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - SomeTimeInSeconds);

   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();

   ReadyToDefrostShouldChangeTo(true);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(SomeTimeInSeconds * MSEC_PER_SEC);
}

TEST(DefrostIntegration_SingleEvap, ShouldSetReadyToDefrostAfterMaximumTimeReachedDueToAccumulatedDoorAccelerationsThatHappenedWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo();

   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - SomeTimeInMsec);
   WhenGridVotesToTurnOffCompressorAndWinsAndDoesNotHaveToWaitMinimumOffTimeBecauseItAlreadyPassed();

   WhenRightSideFreshFoodDoorIs(Open);

   WhenGridVotesToTurnOnCompressorWhileCompressorMinimumTimesAreEnabledAfterBeingOffAndGridWinsAndGridRanThisManyTimes(1);

   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - SomeTimeInSeconds + 1);
   RightSideFreshFoodScaledDoorAccelerationsInSecondsShouldBe((compressorData->compressorTimes.minimumOffTimeInMinutes * SECONDS_PER_MINUTE +
                                                                 compressorData->compressorTimes.sabbathDelayTimeInSeconds +
                                                                 compressorData->compressorTimes.startupOnTimeInSeconds) *
      defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   ReadyToDefrostShouldChangeTo(true);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(SomeTimeInSeconds * MSEC_PER_SEC);
}

TEST(DefrostIntegration_SingleEvap, ShouldSetReadyToDefrostAfterMaximumTimeReachedDueToAccumulatedDoorAccelerationsThatHappenedWhileWaitingForRemainingTimeBetweenDefrostsAndCompressorOnTime)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo();

   WhenCompressorIsOnForMinimumTimePlusSomeTime();
   WhenGridVotesToTurnOffCompressorAndWinsAndDoesNotHaveToWaitMinimumOffTimeBecauseItAlreadyPassed();

   WhenRightSideFreshFoodDoorIsOpenForSomeSmallTime();
   WhenCompressorTurnsOnAfterSomeSmallTimeOfBeingOff();
   WhenFreezerDoorIsOpenForSomeSmallTime();
   WhenRightSideFreshFoodDoorIs(Open);
   WhenFreezerDoorIs(Open);

   ReadyToDefrostShouldChangeTo(true);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            (SomeSmallTimeInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
               SomeSmallTimeInSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond + defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
               SomeTimeInSeconds +
               SomeSmallTimeInSeconds) *
               MSEC_PER_SEC) /
      (1 + (defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond)));
}

TEST(DefrostIntegration_SingleEvap, ShouldDefrostAfterSabbathTimeBetweenDefrostsInMinutesWhileSabbathIsEnabled)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenSabbathModeIs(ENABLED);

   After(sabbathData->timeBetweenDefrostsInMinutes * MSEC_PER_MIN - 1);
   SabbathIsReadyToDefrostShouldBe(false);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   After(1);
   SabbathIsReadyToDefrostShouldBe(true);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotStartDefrostingWhenSabbathIsReadyToDefrostIsSetAndStartDefrostingWhenReadyToDefrostIsSetWhileSabbathIsDisabled)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenSabbathModeIs(ENABLED);

   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();

   After(sabbathData->timeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC -
      compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC -
      MSEC_PER_SEC - 1);
   SabbathIsReadyToDefrostShouldBe(false);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   WhenSabbathModeIs(DISABLED);

   After(1);
   SabbathIsReadyToDefrostShouldBe(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      sabbathData->timeBetweenDefrostsInMinutes * MSEC_PER_MIN +
      compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC +
      compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC +
      MSEC_PER_SEC - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(DefrostIntegration_SingleEvap, ShouldDefrostWhenSabbathIsEnabledWhileSabbathReadyToDefrostIsSet)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();

   After(sabbathData->timeBetweenDefrostsInMinutes * MSEC_PER_MIN);
   SabbathIsReadyToDefrostShouldBe(true);
   ReadyToDefrostShouldBe(false);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   WhenSabbathModeIs(ENABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(DefrostIntegration_SingleEvap, ShouldDefrostWhenSabbathEnabledAfterSabbathTimeBetweenDefrostsElapsesIfSabbathTimeIsLessThanNormalTimeBetweenDefrosts)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();

   After(sabbathData->timeBetweenDefrostsInMinutes * MSEC_PER_MIN);
   WhenSabbathModeIs(DISABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   WhenSabbathModeIs(ENABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(DefrostIntegration_SingleEvap, ShouldDefrostAfterSabbathTimeBetweenDefrostsWhenSabbathIsEnabledBeforeSabbathTimeBetweenDefrostCompletes)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenSabbathModeIs(DISABLED);

   WhenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();

   After(sabbathData->timeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC -
      compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC -
      MSEC_PER_SEC - 1);
   SabbathIsReadyToDefrostShouldBe(false);
   WhenSabbathModeIs(ENABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   After(1);
   SabbathIsReadyToDefrostShouldBe(true);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(DefrostIntegration_SingleEvap, ShouldDefrostWhenSabbathDisabledBeforeSabbathTimeBetweenDefrostElapsesButNormalTimeIsMetBecauseNormalTimeIsLessThanSabbathTime)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenRefrigeratorResetsWithFreezerTooWarm();
   GivenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
   GivenSabbathModeIs(ENABLED);
   TheNormalTimeBetweenDefrostsShouldBeLessThanSabbathTimeBetweenDefrost();

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   WhenSabbathModeIs(DISABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(DefrostIntegration_SingleEvap, ShouldDefrostAfterWaitingForFullSabbathTimeBetweenDefrostsToElapseWhenSabbathIsEnabledBeforeNormalTimeBetweenDefrostsElapses)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   GivenRefrigeratorResetsWithFreezerTooWarm();
   GivenGridVotesToTurnCompressorOnWhileCompressorMinimumTimesAreEnabledAndGridWins();
   GivenSabbathModeIs(DISABLED);
   TheNormalTimeBetweenDefrostsShouldBeLessThanSabbathTimeBetweenDefrost();

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
   WhenSabbathModeIs(ENABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   After(sabbathData->timeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      (defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN +
         compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC +
         compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC) -
      MSEC_PER_SEC);
   SabbathIsReadyToDefrostShouldBe(false);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   After(1);
   SabbathIsReadyToDefrostShouldBe(true);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(DefrostIntegration_SingleEvap, ShouldStartWithAFullDefrostAndCompletePrechillViaTheAppropriateMaxPrechillTimeWhenPrechillConditionsAreNotMet)
{
   GivenThatTheApplicationHasStartedWithValidThermistorsAndDefrostIsInIdle();
   CurrentDefrostTypeShouldBe(DefrostType_Full);

   WhenTimeBetweenDefrostsMeetsMaxTimeBetweenDefrostsWithCompressorOnTimeAndFreezerDoorOpenAndIsInPrechillPrep();

   CompressorShouldBe(OFF);
   CoolingModeShouldBe(CoolingMode_Freezer);

   WhenTheFreezerEvaporatorThermistorIsGreaterThanPrechillExitTemperature();
   WhenTheFreshFoodThermistorIsInvalid();
   WhenTheFreezerThermistorIsGreaterThanPrechillFreezerMinTemperature();

   After(defrostData->prechillPrepData.maxPrechillPrepTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(defrostData->prechillData.maxPrechillTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(DefrostIntegration_SingleEvap, ShouldExitPostDwellAfterNormalPostDwellExitTimeIfUnitPowersOnInDwell)
{
   GivenDefrostStateWas(DefrostState_Dwell);
   RunTimerModuleToSaveErdInEeprom();
   WhenRefrigeratorResetsWithFreezerNotTooWarm();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
   CurrentDefrostTypeShouldBe(DefrostType_Full);
   After(defrostData->dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(DefrostIntegration_SingleEvap, ShouldExitPostDwellAfterNormalPostDwellExitTimeIfUnitPowersOnInHeaterOn)
{
   GivenDefrostStateWas(DefrostState_HeaterOn);
   RunTimerModuleToSaveErdInEeprom();
   WhenRefrigeratorResetsWithFreezerNotTooWarm();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   CurrentDefrostTypeShouldBe(DefrostType_Full);

   DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime();
   DefrostExitsHeaterOnAndIsInDwellBecauseFreezerEvaporatorReachesTerminationTemperature();
   DefrostExitsDwellAndIsInPostDwellAfterDwellTime();

   After(defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   After(1);
}

TEST(DefrostIntegration_SingleEvap, ShouldDefrostWhenEepromCompressorOnTimeAndDoorAccelerationTimesAlreadyMeetMaxTimeBetweenDefrostsAfterBeingResetWithShorterTimeBetweenDefrostParametricData)
{
   GivenEepromWasNotClearedAtStartup();
   GivenDefrostStateWas(DefrostState_Idle);
   GivenEepromCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInMinutesBetweenMinimumAndMaximumTimeBetweenDefrosts);
   GivenEepromRightSideFreshFoodScaledDoorAccelerationInSecondsIs(SomeRightSideFreshFoodScaledDoorAccelerationsInSeconds);
   GivenEepromFreezerScaledDoorAccelerationInSecondsIs(SomeFreezerScaledDoorAccelerationInSeconds);

   RunTimerModuleToSaveErdInEeprom();
   WhenRefrigeratorResetsWithFreezerNotTooWarm();

   CompressorShouldBe(OFF);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   TheNormalTimeBetweenDefrostsShouldBe(defrostData->idleData.maxTimeBetweenDefrostsInMinutes);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   WhenRefrigeratorResetsWithNewParametricThatHasShorterTimeBetweenDefrosts();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   After(0);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}
