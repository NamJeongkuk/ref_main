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
#include "Vote.h"
#include "Rx2xxResetSource.h"
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
   TenMinutes = 10,
   ZeroMinutes = 0
};

enum
{
   Closed = 0,
   Open = 1
};

enum
{
   CompressorSabbathDelayTimeInSeconds = 20
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

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;

      mock().strictOrder();
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
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

   void GivenFreezerEvaporatorThermistorIsValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidOverrideRequest, set);
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidOverrideValue, &state);
   }

   void GivenFreshFoodThermistorIsValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidOverrideRequest, set);
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidOverrideValue, &state);
   }

   void GivenFreezerThermistorIsValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidOverrideRequest, set);
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidOverrideValue, &state);
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
      DataModel_Read(dataModel, Erd_DefrostFreezerScaledDoorAccelerationInSeconds, &actualSeconds);

      CHECK_EQUAL(expectedSeconds, actualSeconds);
   }

   void CompressorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorIsOn, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CompressorStateShouldBe(CompressorState_t expected)
   {
      CompressorState_t actual;
      DataModel_Read(dataModel, Erd_CompressorState, &actual);

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

   void WhenItBecomesReadyToDefrostAfterMaxTimeBetweenDefrostsWithCompressorOn()
   {
      // I want to replace this by having the compressor on because of the grid and same with cooling mode
      // CompressorSpeedWinningVoteShouldBe(Erd_CompressorSpeed_DefrostVote);

      WhenFactoryHasVotedForCompressor(CompressorSpeed_Low, Vote_Care);
      After(CompressorSabbathDelayTimeInSeconds * MSEC_PER_SEC);
      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      CompressorShouldBe(OFF);

      After(1);
      CompressorShouldBe(ON);
      CompressorStateShouldBe(CompressorState_MinimumOnTime);
      CoolingModeShouldBe(CoolingMode_Freezer);

      After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN);
   }

   void WhenFactoryHasVotedForCompressor(CompressorSpeed_t speed, Vote_t care)
   {
      CompressorVotedSpeed_t vote = { .speed = speed, .care = care };
      DataModel_Write(dataModel, Erd_CompressorSpeed_FactoryVote, &vote);
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

   void GivenFreezerCompartmentTemperatureIsNotTooWarm()
   {
      TemperatureDegFx100_t temperature = 0;
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureInDegFx100, &temperature);
   }

   void FreezerCompartmentTemperatureShouldNotBeTooWarmOnPowerUp()
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &actual);

      CHECK_FALSE(actual);
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

   void GivenThatTheApplicationHasStartedAndDefrostIsInIdle()
   {
      GivenFreezerEvaporatorThermistorIsValidIs(Valid);
      GivenFreshFoodThermistorIsValidIs(Valid);
      GivenFreezerThermistorIsValidIs(Valid);
      GivenAllPreviousDefrostsWereNormal();
      GivenFreezerCompartmentTemperatureIsNotTooWarm();
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
      GivenEepromWasNotClearedAtStartup();

      GivenDefrostStateWas(DefrostState_Idle);
      GivenApplicationHasBeenInitialized();

      FreezerCompartmentTemperatureShouldNotBeTooWarmOnPowerUp();
      DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   }

   void GivenThatTheApplicationHasStartedAndDefrostIsInPrechillPrep()
   {
      GivenThatTheApplicationHasStartedAndDefrostIsInIdle();

      WhenCompressorIsTurnedOnWhileCompressorMinimumTimesAreEnabled();
      After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1 * MSEC_PER_SEC);
      CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
      FreezerScaledDoorAccelerationsInSecondsShouldBe(0);

      After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
         defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
         defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC);

      WhenFactoryHasVotedForCompressor(CompressorSpeed_Off, Vote_Care);
      WhenFreezerDoorIs(Open);

      DefrostHsmStateShouldBe(DefrostHsmState_Idle);
      After(1 * MSEC_PER_SEC);
      DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   }

   void GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry()
   {
      GivenThatTheApplicationHasStartedAndDefrostIsInIdle();

      WhenItBecomesReadyToDefrostAfterMaxTimeBetweenDefrostsWithCompressorOn();
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
      CompressorOnTimeInSecondsShouldBe(0);

      WhenFactoryHasVotedForCompressor(CompressorSpeed_Off, Vote_DontCare);
   }

   void WhenDefrostTestIsRequested(DefrostTestRequest_t request)
   {
      DataModel_Write(dataModel, Erd_DefrostTestRequest, &request);
   }

   void UseMinimumReadyToDefrostTimeShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_UseMinimumReadyToDefrostTime, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_t expectedState)
   {
      DefrostCompressorOnTimeCounterFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostCompressorOnTimeCounterFsmState, &actualState);
      CHECK_EQUAL(expectedState, actualState);
   }

   void DontSkipPrechillShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DontSkipDefrostPrechill, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenUseMinimumReadyToDefrostTimeHasBeenSetTo(bool state)
   {
      DataModel_Write(dataModel, Erd_UseMinimumReadyToDefrostTime, &state);
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

   void DefrostTransitedFromHeaterOnEntryToPostDwellAndWasAbnormal()
   {
      DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime();
      DefrostExitsHeaterOnAndIsInDwellAfterMaxTimeout();
      FreezerDefrostWasAbnormalShouldBe(true);

      DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Stop);
      CompressorOnTimeInSecondsShouldBe(0);

      DefrostExitsDwellAndIsInPostDwellAfterDwellTime();
      DefrostExitsPostDwellAndIsInIdleAfterPostDwellTime();
   }

   void DefrostTransitedFromHeaterOnEntryToPostDwellAndWasNormal()
   {
      DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime();
      DefrostExitsHeaterOnAndIsInDwellBecauseFreezerEvaporatorReachesTerminationTemperature();
      FreezerDefrostWasAbnormalShouldBe(false);

      DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Stop);
      CompressorOnTimeInSecondsShouldBe(0);

      DefrostExitsDwellAndIsInPostDwellAfterDwellTime();
      DefrostExitsPostDwellAndIsInIdleAfterPostDwellTime();
   }

   void GivenAhamPrechillTestHasBeenRequestedDuringDefrostAndHasNotSkippedPrechillPrepAndIsNowInHeaterOnEntry()
   {
      GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
      GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

      WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
      UseMinimumReadyToDefrostTimeShouldBe(true);
      DontSkipPrechillShouldBe(true);

      DefrostTransitedFromHeaterOnEntryToPostDwellAndWasAbnormal();

      ReadyToDefrostShouldBe(false);
      UseMinimumReadyToDefrostTimeShouldBe(true);
      DontSkipPrechillShouldBe(true);

      CompressorOnTimeInSecondsShouldBe(defrostData->postDwellData.postDwellExitTimeInMinutes * SECONDS_PER_MINUTE + 1);

      // Should use minimum time to exit Idle
      // Should not skip prechill prep (if it didn't use the flag, it would have gone to heater on entry)
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
      TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
      After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
      ReadyToDefrostShouldBe(true);
   }

   void GivenEepromCompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_Eeprom_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void GivenEepromFreezerScaledDoorAccelerationInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_Eeprom_DefrostFreezerScaledDoorAccelerationInSeconds, &seconds);
   }

   void GivenThatTheApplicationHasStartedAndDefrostIsInPostDwell()
   {
      GivenFreezerEvaporatorThermistorIsValidIs(Valid);
      GivenFreshFoodThermistorIsValidIs(Valid);
      GivenFreezerThermistorIsValidIs(Valid);
      GivenAllPreviousDefrostsWereNormal();
      GivenFreezerCompartmentTemperatureIsNotTooWarm();
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
      DataModel_Write(dataModel, Erd_FreezerDoorIsOpen, &state);
   }

   void WhenCompressorIsTurnedOnWhileCompressorMinimumTimesAreEnabled()
   {
      WhenFactoryHasVotedForCompressor(CompressorSpeed_Low, Vote_Care);

      After(CompressorSabbathDelayTimeInSeconds * MSEC_PER_SEC);
      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      CompressorShouldBe(OFF);

      After(1);
      CompressorShouldBe(ON);
      CompressorStateShouldBe(CompressorState_MinimumOnTime);
   }

   void WhenCompressorIsTurnedOnWhileCompressorMinimumTimesHaveBeenDisabled()
   {
      WhenFactoryHasVotedForCompressor(CompressorSpeed_Low, Vote_Care);
      CompressorStateShouldBe(CompressorState_OnAndReadyToChange);
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
};

TEST(DefrostIntegration_SingleEvap, ShouldInitialize)
{
   GivenApplicationHasBeenInitialized();
}

TEST(DefrostIntegration_SingleEvap, ShouldStartInDefrostHsmStateIdle)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInIdle();
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
   UseMinimumReadyToDefrostTimeShouldBe(true);
   DontSkipPrechillShouldBe(true);

   DefrostTransitedFromHeaterOnEntryToPostDwellAndWasAbnormal();

   ReadyToDefrostShouldBe(false);
   UseMinimumReadyToDefrostTimeShouldBe(true);
   DontSkipPrechillShouldBe(true);

   CompressorOnTimeInSecondsShouldBe(defrostData->postDwellData.postDwellExitTimeInMinutes * SECONDS_PER_MINUTE + 1);

   // Should use minimum time to exit Idle
   // Should not skip prechill prep (if it didn't use the flag, it would have gone to heater on entry)
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldResumeNormalDefrostAfterAhamPrechillTest)
{
   GivenAhamPrechillTestHasBeenRequestedDuringDefrostAndHasNotSkippedPrechillPrepAndIsNowInHeaterOnEntry();

   DontSkipPrechillShouldBe(false);
   UseMinimumReadyToDefrostTimeShouldBe(false);

   DefrostTransitedFromHeaterOnEntryToPostDwellAndWasNormal();

   DontSkipPrechillShouldBe(false);
   UseMinimumReadyToDefrostTimeShouldBe(false);

   // goes to prechill prep b/c the defrost was normal
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
   UseMinimumReadyToDefrostTimeShouldBe(false);

   DefrostTransitedFromHeaterOnEntryToPostDwellAndWasAbnormal();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldRunTheAhamPrechillTestRequestByEnteringPrechillPrepAfterMinimumRunTimeWhenRequestedAtTheStartOfIdleOnlyUsingCompressorOnTimeDuringIdle)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
   UseMinimumReadyToDefrostTimeShouldBe(true);
   DontSkipPrechillShouldBe(true);

   WhenFactoryHasVotedForCompressor(CompressorSpeed_Low, Vote_Care);
   After(CompressorSabbathDelayTimeInSeconds * MSEC_PER_SEC);
   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   CompressorShouldBe(OFF);

   After(1);
   CompressorShouldBe(ON);
   CompressorStateShouldBe(CompressorState_MinimumOnTime);
   CoolingModeShouldBe(CoolingMode_Freezer);

   // Should use minimum time to exit Idle
   // Should not skip prechill prep (if it didn't use the flag, it would have gone to heater on entry)
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldRunTheAhamPrechillTestRequestByEnteringPrechillPrepAfterMinimumRunTimeWhenRequestedWhileInIdleOnlyUsingCompressorOnTimeDuringIdle)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   WhenFactoryHasVotedForCompressor(CompressorSpeed_Low, Vote_Care);
   After(CompressorSabbathDelayTimeInSeconds * MSEC_PER_SEC);
   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   CompressorShouldBe(OFF);

   After(1);
   CompressorShouldBe(ON);
   CompressorStateShouldBe(CompressorState_MinimumOnTime);
   CoolingModeShouldBe(CoolingMode_Freezer);

   After(60 * MSEC_PER_MIN);
   UseMinimumReadyToDefrostTimeShouldBe(false);
   DontSkipPrechillShouldBe(false);

   CompressorOnTimeInSecondsShouldBe(60 * SECONDS_PER_MINUTE + 1);

   WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
   UseMinimumReadyToDefrostTimeShouldBe(true);
   DontSkipPrechillShouldBe(true);
   CompressorOnTimeInSecondsShouldBe(0);

   // Should use minimum time to exit Idle
   // Should not skip prechill prep (if it didn't use the flag, it would have gone to heater on entry)
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldRunTheAhamPrechillTestRequestByEnteringPrechillPrepAfterMinimumRunTimeWhenRequestedWhileInPostDwellOnlyUsingCompressorOnTimeDuringIdle)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInPostDwell();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   CompressorSpeedDefrostVoteShouldBe({ .speed = CompressorSpeed_Low, .care = Vote_Care });
   CompressorStateShouldBe(CompressorState_OnAndReadyToChange);
   CoolingModeShouldBe(CoolingMode_Freezer);

   After(defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN / 2);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
   CompressorOnTimeInSecondsShouldBe(defrostData->postDwellData.postDwellExitTimeInMinutes * SECONDS_PER_MINUTE / 2);

   UseMinimumReadyToDefrostTimeShouldBe(false);
   DontSkipPrechillShouldBe(false);

   WhenDefrostTestIsRequested(DefrostTestRequest_AhamPrechill);
   UseMinimumReadyToDefrostTimeShouldBe(true);
   DontSkipPrechillShouldBe(true);
   CompressorOnTimeInSecondsShouldBe(0);
   CompressorShouldBe(ON);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   After(defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN / 2);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN / 2);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldSetReadyToDefrostErdWhenItBecomesReadyToDefrostAfterMaxTimeBetweenDefrostsWithCompressorOnWhileInADefrostingStateWithCompressorOnAndThenClearReadyToDefrostOnceItStartsWaitingToDefrostAgain)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

   DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime();
   DefrostExitsHeaterOnAndIsInDwellBecauseFreezerEvaporatorReachesTerminationTemperature();

   ReadyToDefrostShouldChangeTo(false);
   DefrostExitsDwellAndIsInPostDwellAfterDwellTime();
   DefrostExitsPostDwellAndIsInIdleAfterPostDwellTime();

   ReadyToDefrostShouldChangeTo(true);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Prechill);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - defrostData->postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldBe(true);

   DefrostExitsHeaterOnEntryAndIsInHeaterAfterHeaterOnDelayAfterCompressorOffTime();
   DefrostExitsHeaterOnAndIsInDwellBecauseFreezerEvaporatorReachesTerminationTemperature();

   ReadyToDefrostShouldChangeTo(false);
   DefrostExitsDwellAndIsInPostDwellAfterDwellTime();
}

TEST(DefrostIntegration_SingleEvap, ShouldNotClearUseMinimumTimeErdOnInitIfSet)
{
   GivenUseMinimumReadyToDefrostTimeHasBeenSetTo(true);
   GivenApplicationHasBeenInitialized();
   UseMinimumReadyToDefrostTimeShouldBe(true);
}

TEST(DefrostIntegration_SingleEvap, ShouldStartDefrostingOnInitIfSavedEepromCompressorOnTimeIsMinimumTimeBetweenDefrostsAndConditionsAreSuchThatTimeBetweenDefrostsIsMinimumTimeAndItPowersUpInIdle)
{
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo();

   GivenEepromCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   GivenFreezerEvaporatorThermistorIsValidIs(Valid);
   GivenFreshFoodThermistorIsValidIs(Valid);
   GivenFreezerThermistorIsValidIs(Valid);
   GivenAllPreviousDefrostsWereNormal();
   GivenFreezerCompartmentTemperatureIsNotTooWarm();
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenEepromWasClearedAtStartup();

   GivenDefrostStateWas(DefrostState_Idle);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   ReadyToDefrostShouldChangeTo(true);
   WhenApplicationIsInitialized();
}

TEST(DefrostIntegration_SingleEvap, ShouldStartDefrostingOnInitIfSavedEepromCompressorOnTimeIsMinimumTimeBetweenDefrostsAndDoorAccelerationTotalIsMaxTimeBetweenDefrostsMinusMinimumTimeBetweenDefrostsAndConditionsAreSuchThatTimeBetweenDefrostsIsMaximumTimeAndItPowersUpInIdle)
{
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenReadyToDefrostSubscriptionHasBeenInitializedAndSubscribedTo();

   GivenEepromCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenEepromFreezerScaledDoorAccelerationInSecondsIs((defrostData->idleData.maxTimeBetweenDefrostsInMinutes - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) * SECONDS_PER_MINUTE);

   GivenFreezerEvaporatorThermistorIsValidIs(Valid);
   GivenFreshFoodThermistorIsValidIs(Valid);
   GivenFreezerThermistorIsValidIs(Valid);
   GivenAllPreviousDefrostsWereNormal();
   GivenFreezerCompartmentTemperatureIsNotTooWarm();
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenEepromWasNotClearedAtStartup();

   GivenDefrostStateWas(DefrostState_Idle);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   ReadyToDefrostShouldChangeTo(true);
   WhenApplicationIsInitialized();
}

TEST(DefrostIntegration_SingleEvap, ShouldNotReenterIdleAndShouldNotRestartCompressorOnTimeCounterOrDoorAccelerationCounterWhenIdleTestRequestedDuringIdle)
{
   GivenEepromCompressorOnTimeInSecondsIs(10);
   GivenEepromFreezerScaledDoorAccelerationInSecondsIs(20);
   GivenThatTheApplicationHasStartedAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   CompressorOnTimeInSecondsShouldBe(10);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(20);

   WhenCompressorIsTurnedOnWhileCompressorMinimumTimesAreEnabled();
   CompressorOnTimeInSecondsShouldBe(11);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(20);

   WhenDefrostTestIsRequested(DefrostTestRequest_Idle);
   WhenFreezerDoorIs(Open);

   After(1 * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(11);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(20);

   After(1);
   CompressorOnTimeInSecondsShouldBe(12);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(20 + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(DefrostIntegration_SingleEvap, ShouldTransitionToIdleAndStartCountingCompressorOnTimeAndDoorAccelerationsWhenIdleTestRequestedDuringNonIdleState)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   WhenDefrostTestIsRequested(DefrostTestRequest_Idle);

   WhenFreezerDoorIs(Open);
   WhenCompressorIsTurnedOnWhileCompressorMinimumTimesHaveBeenDisabled();

   After(1 * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(0);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(0);

   After(1);
   CompressorOnTimeInSecondsShouldBe(1);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(DefrostIntegration_SingleEvap, ShouldTransitionToHeaterOnEntryWhenDefrostTestRequestedDuringWaitingToDefrostStateAndDoAFullDefrost)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInIdle();
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
   GivenThatTheApplicationHasStartedAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_PrechillPrep);
   WhenDefrostTestIsRequested(DefrostTestRequest_Prechill);

   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotTransitionToPrechillPrepButToHeaterOnEntryWhenPrechillTestRequestedAndFreezerThermistorIsInvalidDuringWaitingToDefrostState)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenFreezerThermistorIsValidIs(Invalid);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Prechill);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotTransitionToPrechillPrepButToHeaterOnEntryWhenPrechillTestRequestedAndFreezerEvaporatorThermistorIsInvalidDuringWaitingToDefrostState)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenFreezerEvaporatorThermistorIsValidIs(Invalid);

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
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenFreezerThermistorIsValidIs(Invalid);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Prechill);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotTransitionToPrechillPrepButToHeaterOnEntryWhenPrechillTestRequestedAndFreezerEvaporatorThermistorIsInvalidDuringDefrostingState)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInHeaterOnEntry();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();
   GivenFreezerEvaporatorThermistorIsValidIs(Invalid);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_HeaterOnEntry);
   WhenDefrostTestIsRequested(DefrostTestRequest_Prechill);
}

TEST(DefrostIntegration_SingleEvap, ShouldTransitionToIdleWhenExitingDisabledStateAndStartCountingCompressorOnTimeAndDoorAccelerationsFromZeroWhenItWasInIdleBeforeBeingDisabled)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInIdle();
   GivenDefrostHsmStateSubscriptionHasBeenInitializedAndSubscribedToTheDefrostHsmState();

   CompressorOnTimeInSecondsShouldBe(0);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(0);

   WhenFreezerDoorIs(Open);
   WhenCompressorIsTurnedOnWhileCompressorMinimumTimesAreEnabled();

   CompressorOnTimeInSecondsShouldBe(1);
   FreezerScaledDoorAccelerationsInSecondsShouldBe((CompressorSabbathDelayTimeInSeconds + compressorData->compressorTimes.startupOnTimeInSeconds) * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Disabled);
   WhenDefrostTestIsRequested(DefrostTestRequest_Disable);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   WhenDefrostTestIsRequested(DefrostTestRequest_Enable);

   After(1 * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(0);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(0);

   After(1);
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
   WhenCompressorIsTurnedOnWhileCompressorMinimumTimesHaveBeenDisabled();

   After(1 * MSEC_PER_SEC);
   CompressorOnTimeInSecondsShouldBe(1);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(1 * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Disabled);
   WhenDefrostTestIsRequested(DefrostTestRequest_Disable);

   TheDefrostHsmStateShouldChangeTo(DefrostHsmState_Idle);
   WhenDefrostTestIsRequested(DefrostTestRequest_Enable);

   After(1 * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(0);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(0);

   After(1);
   CompressorOnTimeInSecondsShouldBe(1);
   FreezerScaledDoorAccelerationsInSecondsShouldBe(defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(DefrostIntegration_SingleEvap, ShouldNotBeAbleToEnableDefrostThatIsAlreadyEnabled)
{
   GivenThatTheApplicationHasStartedAndDefrostIsInIdle();
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
