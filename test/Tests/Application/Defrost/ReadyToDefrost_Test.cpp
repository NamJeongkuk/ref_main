/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ReadyToDefrost.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Closed = 0,
   Open = 1,
   Opened = Open
};

enum
{
   SomeCompressorOnTimeInSeconds = 400,
   SomeDoorAccelerationsInSecondsPerSecond = 200
};

enum
{
   SomeSeconds = 20,
   UpdatePeriodInSeconds = 1 * SECONDS_PER_MINUTE,
   LessThanPeriodicUpdateInSeconds = UpdatePeriodInSeconds - 3,
};

static ReadyToDefrostDoorConfiguration_t doorsConfiguration[] = {
   {
      .doorIsOpenErd = Erd_LeftSideFreshFoodDoorIsOpen,
      .doorAccelerationErd = Erd_LeftHandFreshFoodScaledDoorAccelerationInSeconds,
      .offsetInParametricForDoorFactor = OFFSET_OF(DefrostData_t, idleData.freshFoodDoorIncrementFactorInSecondsPerSecond),
   },
   {
      .doorIsOpenErd = Erd_LeftSideFreezerDoorStatus,
      .doorAccelerationErd = Erd_FreezerScaledDoorAccelerationInSeconds,
      .offsetInParametricForDoorFactor = OFFSET_OF(DefrostData_t, idleData.freezerDoorIncrementFactorInSecondsPerSecond),
   },
   {
      .doorIsOpenErd = Erd_ConvertibleCompartmentAsFreshFoodDoorIsOpen,
      .doorAccelerationErd = Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds,
      .offsetInParametricForDoorFactor = OFFSET_OF(DefrostData_t, idleData.freshFoodDoorIncrementFactorInSecondsPerSecond),
   },
};

static ReadyToDefrostConfiguration_t config = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .defrostCompressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .freezerFilteredTemperatureWasTooWarmAtPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .useMinimumReadyToDefrostTimeAndResetDefrostCountsErd = Erd_UseMinimumReadyToDefrostTimeAndResetDefrostCounts,
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

TEST_GROUP(ReadyToDefrost)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const DefrostData_t *defrostData;

   ReadyToDefrost_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
   }

   void GivenReadyToDefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);

      ReadyToDefrost_Init(&instance, dataModel, &config, defrostData);
   }

   void ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_t expected)
   {
      ReadyToDefrostHsmState_t actual;
      DataModel_Read(dataModel, Erd_ReadyToDefrostHsmState, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenWaitingToDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, &state);
   }

   void WhenWaitingToDefrostIs(bool state)
   {
      GivenWaitingToDefrostIs(state);
   }

   void WhenDefrostChangesFromWaitingToDefrostToDefrostingBackToWaitingToDefrost()
   {
      WhenWaitingToDefrostIs(false);
      // cleared by defrost HSM
      WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(false);
      WhenWaitingToDefrostIs(true);
   }
   void GivenFreezerTooWarmAtPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }

   void WhenFreezerTooWarmAtPowerUpChangesTo(bool state)
   {
      GivenFreezerTooWarmAtPowerUpIs(state);
   }

   void GivenCompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void CompressorOnTimeInSecondsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &actual);

      CHECK_TRUE((actual <= (expected + expected / 100)) && (actual >= (expected - expected / 100)));
   }

   void GivenDoorAccelerationsAre(uint32_t seconds)
   {
      for(uint8_t i = 0; i < config.numberOfDoors; i++)
      {
         DataModel_Write(
            dataModel,
            config.doorsConfiguration[i].doorAccelerationErd,
            &seconds);
      }
   }

   void DoorAccelerationsShouldBe(uint32_t expected)
   {
      for(uint8_t i = 0; i < config.numberOfDoors; i++)
      {
         uint32_t actual;
         DataModel_Read(
            dataModel,
            config.doorsConfiguration[i].doorAccelerationErd,
            &actual);

         CHECK_EQUAL(expected, actual);
      }
   }

   void GivenFreshFoodDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, &state);
   }

   void WhenFreshFoodDefrostWasAbnormalChangesTo(bool state)
   {
      GivenFreshFoodDefrostWasAbnormalIs(state);
   }

   void GivenFreezerDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void WhenFreezerDefrostWasAbnormalChangesTo(bool state)
   {
      GivenFreezerDefrostWasAbnormalIs(state);
   }

   void GivenConvertibleCompartmentDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void WhenConvertibleCompartmentDefrostWasAbnormalChangesTo(bool state)
   {
      GivenConvertibleCompartmentDefrostWasAbnormalIs(state);
   }

   void GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_InvalidFreezerEvaporatorThermistorDuringDefrost, &state);
   }

   void WhenInvalidFreezerEvaporatorThermistorDuringDefrostChangesTo(bool state)
   {
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(state);
   }

   void GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(bool state)
   {
      DataModel_Write(dataModel, Erd_UseMinimumReadyToDefrostTimeAndResetDefrostCounts, &state);
   }

   void WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(bool state)
   {
      GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(state);
   }

   void GivenEepromClearedFlagIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, &state);
   }

   void WhenEepromClearedFlagChangesTo(bool state)
   {
      GivenEepromClearedFlagIs(state);
   }

   void TimeBetweenDefrostsInMinutesShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_TimeBetweenDefrostsInMinutes, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenItHasAConvertibleCompartmentIs(bool state)
   {
      DataModel_Write(dataModel, Erd_HasConvertibleCompartment, &state);
   }

   void WaitingDefrostTimeInSecondsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(dataModel, Erd_WaitingForDefrostTimeInSeconds, &actual);

      CHECK_TRUE((actual <= (expected + expected / 100)) && (actual >= (expected - expected / 100)));
   }

   void ReadyToDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_ReadyToDefrost, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenCompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void WhenCompressorTurns(bool state)
   {
      GivenCompressorIs(state);
   }

   void GivenLeftHandFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreshFoodDoorIsOpen, &state);
   }

   void GivenRightHandFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightSideFreshFoodDoorStatus, &state);
   }

   void GivenDoorInDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DoorInDoorIsOpen, &state);
   }

   void GivenFreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreezerDoorStatus, &state);
   }

   void GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentAsFreshFoodDoorIsOpen, &state);
   }

   void GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &state);
   }

   void GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal()
   {
      GivenFreshFoodDefrostWasAbnormalIs(false);
      GivenFreezerDefrostWasAbnormalIs(true);
      GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
      GivenFreezerTooWarmAtPowerUpIs(false);
      GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(false);
      GivenEepromClearedFlagIs(false);
   }

   void GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum()
   {
      GivenFreshFoodDefrostWasAbnormalIs(false);
      GivenFreezerDefrostWasAbnormalIs(false);
      GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
      GivenFreezerTooWarmAtPowerUpIs(false);
      GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(false);
      GivenEepromClearedFlagIs(false);
   }

   void GivenCompressorOnTimeIsZeroAndTimeBetweenDefrostsIsMinimumAndSystemIsInWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
      GivenCompressorOnTimeInSecondsIs(0);
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();

      ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
   }

   void GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsFalse()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
      GivenWaitingToDefrostIs(false);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsTrue()
   {
      GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized()
   {
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenWaitingToDefrostIsFalseAndReadyToDefrostIsInitialized()
   {
      GivenWaitingToDefrostIs(false);
      GivenReadyToDefrostIsInitialized();
   }

   void WhenLeftHandFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreshFoodDoorIsOpen, &state);
   }

   void WhenFreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreezerDoorStatus, &state);
   }

   void GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(
         dataModel,
         Erd_LeftHandFreshFoodScaledDoorAccelerationInSeconds,
         &seconds);
   }

   void LeftHandFreshFoodDoorAccelerationsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(
         dataModel,
         Erd_LeftHandFreshFoodScaledDoorAccelerationInSeconds,
         &actual);

      CHECK_TRUE((actual <= (expected + expected / 100)) && (actual >= (expected - expected / 100)));
   }

   void GivenFreezerDoorAccelerationInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(
         dataModel,
         Erd_FreezerScaledDoorAccelerationInSeconds,
         &seconds);
   }

   void FreezerDoorAccelerationsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreezerScaledDoorAccelerationInSeconds,
         &actual);

      CHECK_TRUE((actual <= (expected + expected / 100)) && (actual >= (expected - expected / 100)));
   }

   void ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(
         dataModel,
         Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds,
         &actual);

      CHECK_TRUE((actual <= (expected + expected / 100)) && (actual >= (expected - expected / 100)));
   }

   void WhenConvertibleCompartmentAsFreshFoodDoorIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_ConvertibleCompartmentAsFreshFoodDoorIsOpen,
         &state);
   }

   void GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsFalse()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenWaitingToDefrostIs(false);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsBelowMinimumByOneSecondAndWaitingToDefrostIsTrue()
   {
      GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMinimumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsFalseAndStateIsReadyAndDefrosting()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
      GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
      GivenWaitingToDefrostIs(false);
      GivenReadyToDefrostIsInitialized();

      ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
   }

   void GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsFalseAndStateIsReadyAndDefrosting()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
      GivenWaitingToDefrostIs(false);
      GivenReadyToDefrostIsInitialized();

      ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
   }

   void GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
      ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Closed);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Closed);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void GivenLeftHandFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Closed);
      GivenFreezerDoorIs(Open);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsClosedAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Closed);
      GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void WhenCompressorTurnsOffAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenCompressorTurns(OFF);
   }

   void WhenCompressorTurnsOnAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenCompressorTurns(ON);
   }

   void WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenLeftHandFreshFoodDoorIs(Open);
   }

   void WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenLeftHandFreshFoodDoorIs(Closed);
   }

   void WhenConvertibleCompartmentAsFreshFoodDoorClosesAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenConvertibleCompartmentAsFreshFoodDoorIs(Closed);
   }

   void WhenFreezerDoorOpensAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenFreezerDoorIs(Open);
   }

   void WhenFreezerDoorClosesAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenFreezerDoorIs(Closed);
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentAsFreshFoodDoorIsClosedAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(Closed);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentAsFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(Closed);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Closed);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftHandFreshFoodDoorIs(Closed);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(Open);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOffAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenLeftHandFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndCompressorIsOnAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenLeftHandFreshFoodDoorIs(Closed);
      GivenFreezerDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftHandFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenLeftHandFreshFoodDoorIs(Closed);
      GivenFreezerDoorIs(Open);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenCompressorIsOnForMinimumTimeAndUseMinimumReadyToDefrostTimeAndResetDefrostCountsErdChangedToTrueOneMillisecondBeforeTheMinimumTime()
   {
      GivenCompressorIs(ON);
      GivenCompressorOnTimeInSecondsIs(0);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

      After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
      ReadyToDefrostShouldBe(false);

      WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   }
   void GivenLeftHandFreshFoodDoorHasBeenOpenForTenPeriodicUpdatePeriodsAndCompressorIsOff()
   {
      GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

      WhenLeftHandFreshFoodDoorIs(Open);

      for(uint8_t i = 0; i < 10; i++)
      {
         After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
         LeftHandFreshFoodDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

         After(1);
         LeftHandFreshFoodDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      }
   }

   void CompressorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorIsOn, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenCompressorIsOnForOneUpdatePeriod()
   {
      CompressorShouldBe(ON);
      After(UpdatePeriodInSeconds * MSEC_PER_SEC);
   }

   void WhenCompressorIsOffForOneUpdatePeriod()
   {
      WhenCompressorTurns(OFF);
      After(UpdatePeriodInSeconds * MSEC_PER_SEC);
   }
};

TEST(ReadyToDefrost, ShouldInitialize)
{
   GivenReadyToDefrostIsInitialized();
}

TEST(ReadyToDefrost, ShouldInitializeIntoWaitingForMinimumTimeBetweenDefrostsWhenSystemIsWaitingToDefrostAndCompressorOnTimeIsLessThanMinimumTimeBetweenDefrostsOnInit)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldInitializeIntoWaitingForRemainingTimeBetweenDefrostsWhenSystemIsWaitingToDefrostAndCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMaximumOnInit)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldInitializeIntoWaitingForRemainingTimeBetweenDefrostsWhenSystemIsWaitingToDefrostAndCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostsButLessThanMaxAndTimeBetweenDefrostsIsMaximumOnInit)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldInitializeIntoReadyAndDefrostingWhenSystemIsNotWaitingToDefrostOnInit)
{
   GivenWaitingToDefrostIs(false);
   GivenReadyToDefrostIsInitialized();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
}

TEST(ReadyToDefrost, ShouldResetCompressorOnTimeAndDoorAccelerationsToZeroIfFreezerIsTooWarmAtPowerUpOnInit)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenFreezerTooWarmAtPowerUpIs(true);
   GivenReadyToDefrostIsInitialized();

   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldNotResetCompressorOnTimeAndDoorAccelerationsToZeroIfFreezerIsNotTooWarmAtPowerUpOnInit)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenFreezerTooWarmAtPowerUpIs(false);
   GivenReadyToDefrostIsInitialized();

   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
   DoorAccelerationsShouldBe(SomeDoorAccelerationsInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldTransitionFromWaitingForMinimumTimeBetweenDefrostsToReadyAndDefrostingWhenNoLongerWaitingToDefrostAndResetCompressorOnTimeAndDoorAccelerationsToZero)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldTransitionFromWaitingForRemainingTimeBetweenDefrostsToReadyAndDefrostingWhenNoLongerWaitingToDefrostAndResetCompressorOnTimeAndDoorAccelerationsToZero)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeHasNotReachedMinimumButSumOfCompressorOnTimeAndDoorAccelerationsIsEqualToMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeHasNotReachedMinimumButSumOfCompressorOnTimeAndDoorAccelerationsIsGreaterThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 2);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeIsEqualToMinimumTimeButSumOfCompressorOnTimeAndDoorAccelerationsIsLessThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeIsGreaterThanMinimumTimeButSumOfCompressorOnTimeAndDoorAccelerationsIsLessThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 2);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeIsLessThanMinimumTimeAndSumOfCompressorOnTimeAndDoorAccelerationsIsLessThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeIsLessThanMinimumTimeAndTimeBetweenDefrostsIsMin)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitIfCompressorOnTimeIsEqualToMinimumTimeAndTimeBetweenDefrostsIsMinAndTransitionToReadyAndDefrostingWhenWaitingToDefrostChangesToFalse)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitIfCompressorOnTimeIsGreaterThanMinimumTimeAndTimeBetweenDefrostsIsMin)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitWhenCompressorOnTimeIsEqualToMinimumTimeAndTheSumOfCompressorOnTimeAndDoorAccelerationsIsEqualToMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitWhenCompressorOnTimeIsEqualToMinimumTimeAndTheSumOfCompressorOnTimeAndDoorAccelerationsIsGreaterThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitWhenCompressorOnTimeIsGreaterThanMinimumTimeAndTheSumOfCompressorOnTimeAndDoorAccelerationsIsEqualToMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitWhenCompressorOnTimeIsGreaterThanMinimumTimeAndTheSumOfCompressorOnTimeAndDoorAccelerationsIsGreaterThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeIsLessThanMinimumTimeAndTheSumOfCompressorOnTimeAndDoorAccelerationsIsGreaterThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 2);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldTransitionFromReadyAndDefrostingToWaitingForMinimumTimeBetweenDefrostsWhenWaitingToDefrostChangesToTrueAndShouldNotSetReadyToDefrostIfTimeBetweenDefrostsIsMaxAndCompressorOnTimeLessThanMinimumTimeBetweenDefrostsAndSumOfCompressorOnTimeAndDoorAccelerationsIsEqualToMaximumTimeBetweenDefrosts)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsFalse();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldTransitionFromReadyAndDefrostingToWaitingForMinimumTimeBetweenDefrostsWhenWaitingToDefrostChangesToTrueAndShouldNotSetReadyToDefrostIfTimeBetweenDefrostsIsMaxAndCompressorOnTimeLessThanMinimumTimeBetweenDefrostsAndSumOfCompressorOnTimeAndDoorAccelerationsIsGreaterThanMaximumTimeBetweenDefrosts)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 2);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsFalse();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldTransitionFromReadyAndDefrostingToWaitingForMinimumTimeBetweenDefrostsWhenWaitingToDefrostChangesToTrueAndShouldNotSetReadyToDefrostIfTimeBetweenDefrostsIsMinAndCompressorOnTimeLessThanMinimumTimeBetweenDefrosts)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsFalse();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldTransitionFromReadyAndDefrostingToWaitingForMinimumTimeBetweenDefrostsWhenWaitingToDefrostChangesToTrueAndShouldNotSetReadyToDefrostIfTimeBetweenDefrostsIsMaxAndCompressorOnTimeLessThanMinimumTimeBetweenDefrostsAndSumOfCompressorOnTimeAndDoorAccelerationsIsLessThanMaximumTimeBetweenDefrosts)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsFalse();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMaxTimeBetweenDefrostsWhenNoneOfTheConditionsCausingItToBeMinimumAreTrueOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmAtPowerUpIs(false);
   GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.maxTimeBetweenDefrostsInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMaxTimeBetweenDefrostsWhenNoneOfTheConditionsCausingItToBeMinimumAreTrueOnEntryOfWaitingForRemainingTimeBetweenDefrosts)
{
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsTrue();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.maxTimeBetweenDefrostsInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenFreshFoodDefrostWasAbnormalOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(true);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmAtPowerUpIs(false);
   GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenFreezerDefrostWasAbnormalOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(true);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmAtPowerUpIs(false);
   GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenConvertibleCompartmentDefrostWasAbnormalAndItHasAConvertibleCompartmentOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(true);
   GivenItHasAConvertibleCompartmentIs(true);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmAtPowerUpIs(false);
   GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldNotUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenConvertibleCompartmentDefrostWasAbnormalAndItDoesNotHaveAConvertibleCompartmentOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(true);
   GivenItHasAConvertibleCompartmentIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmAtPowerUpIs(false);
   GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.maxTimeBetweenDefrostsInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsOnEntryOfWaitingForMinimumTimeBetweenDefrostsBecauseInvalidFreezerEvaporatorThermistorChangedToTrueWhileDefrosting)
{
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsFalse();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenInvalidFreezerEvaporatorThermistorDuringDefrostChangesTo(true);
   WhenWaitingToDefrostIs(true);
   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenFreezerTooWarmAtPowerIsTrueOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmAtPowerUpIs(true);
   GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenEepromClearedFlagIsTrueOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmAtPowerUpIs(false);
   GivenUseMinimumReadyToDefrostTimeAndDefrostCountsIs(false);
   GivenEepromClearedFlagIs(true);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeWhenUseMinimumReadyToDefrostIsSetToTrueWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(0);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeWhenUseMinimumReadyToDefrostIsSetToTrueWhileInWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   TimeBetweenDefrostsInMinutesShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMinimumTimeWhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsErdChangedToTrueDuringWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);

   WhenCompressorTurns(ON);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMinimumTimeWhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsErdChangedToTrueDuringWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);

   WhenCompressorTurns(ON);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldResetCompressorOnTimeAndDoorAccelerationsWhenReceivingResetDefrostCountsSignalWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldResetCompressorOnTimeAndDoorAccelerationsWhenReceivingResetDefrostCountsSignalWhileInWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldResetCompressorOnTimeAndDoorAccelerationsWhenReceivingResetDefrostCountsSignalWhileInWaitingForReadyAndDefrostingState)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenWaitingToDefrostIsFalseAndReadyToDefrostIsInitialized();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingToDefrostTimeToTotalCompressorOnTimeOnEntryToWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateTotalCompressorOnTimeForTimeThatCompressorIsOnDuringWaitingForMinimumTimeBetweenDefrostsStateIfCompressorIsOnDuringEntryAndTurnsOffLater)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(MSEC_PER_SEC);
   WhenCompressorTurns(OFF);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + 1);
}

TEST(ReadyToDefrost, ShouldUpdateTotalCompressorOnTimeForTimeThatCompressorIsOnDuringWaitingForMinimumTimeBetweenDefrostsStateIfCompressorTurnsOnAfterEntryThenTurnsOffLater)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(ON);

   After(MSEC_PER_SEC);
   WhenCompressorTurns(OFF);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + 1);
}

TEST(ReadyToDefrost, ShouldAccumulateCompressorOnTimeAsItTurnsOnAndOffDuringWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   for(uint8_t i = 0; i < 10; i++)
   {
      WhenCompressorTurns(ON);

      After(MSEC_PER_SEC);
      WhenCompressorTurns(OFF);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + (i + 1));
   }
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdOnceCompressorOnTimeReachesMinimumTimeBetweenDefrostsWhenCompressorIsOnFromEntryOfWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(0);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN);
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdOnceCompressorOnTimeReachesMinimumTimeBetweenDefrostsWithCompressorTurningOnAndOffNotAtPeriodicUpdateTimeIntervalsDuringMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - (5 * UpdatePeriodInSeconds));
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(OFF);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2);

   WhenCompressorTurns(ON);
   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2);
   WhenCompressorTurns(OFF);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2);

   WhenCompressorTurns(ON);
   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2);
   WhenCompressorTurns(OFF);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2);

   WhenCompressorTurns(ON);
   After((4 * UpdatePeriodInSeconds + 1) * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdOnceCompressorOnTimeReachesMinimumTimeBetweenDefrostsWithCompressorTurningOnAndOffDuringMinimumTimeBetweenDefrostsStateAndStartingCompressorOnTimeGreaterThanZero)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 5 * UpdatePeriodInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   ReadyToDefrostShouldBe(false);

   After(4 * UpdatePeriodInSeconds * MSEC_PER_SEC);
   WhenCompressorTurns(OFF);
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1 * UpdatePeriodInSeconds);
   ReadyToDefrostShouldBe(false);

   WhenCompressorTurns(ON);
   After(1 * UpdatePeriodInSeconds * MSEC_PER_SEC);
   WhenCompressorTurns(OFF);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeToTotalCompressorTimeDuringWaitingForMinimumTimeBetweenDefrostsStatesWheneverCompressorTurnsOff)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   for(uint8_t i = 0; i < 10; i++)
   {
      WhenCompressorTurns(ON);

      After(SomeSeconds * MSEC_PER_SEC);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i * SomeSeconds);
      WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i * SomeSeconds);

      WhenCompressorTurns(OFF);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + (i + 1) * SomeSeconds);
   }
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostWhenStartingToWaitToDefrostTheNextTime)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(1 * MSEC_PER_SEC - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldStartOverAtZeroCompressorOnTimeWhenWaitingToDefrostChangesFromFalseToTrueAndThereWasAccumulatedCompressorOnTimeBeforeTheChange)
{
   GivenCompressorIs(ON);
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(1 * MSEC_PER_SEC - 1);
   ReadyToDefrostShouldBe(false);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   After(1);
   ReadyToDefrostShouldBe(false);
   CompressorOnTimeInSecondsShouldBe(0);

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostAfterMinimumTimeBetweenDefrostsIfCompressorWasNotOnForMinimumTimeBetweenDefrostsAndThenShouldSetReadyToDefrostOnceCompressorIsOnForThatLong)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(ON);

   After(SomeSeconds * MSEC_PER_SEC);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   WhenCompressorTurns(OFF);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + SomeSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + SomeSeconds);

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - SomeSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(false);

   WhenCompressorTurns(ON);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - SomeSeconds * MSEC_PER_SEC - SomeCompressorOnTimeInSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeAfterPeriodicUpdateTimeEvenIfCompressorDidNotTurnOff)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   After(1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeAfterPeriodicUpdateTimeEvenIfCompressorIsOff)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   After(1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsAfterPeriodicUpdateTimeEvenIfCompressorDidNotTurnOff)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   After(1);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeEveryPeriodicUpdateTimeWhenCompressorDoesNotTurnOff)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i * UpdatePeriodInSeconds);

      After(1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + (i + 1) * UpdatePeriodInSeconds);
   }
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenThereIsPeriodicUpdateTimeLeftToReachMinimumCompressorOnTime)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - UpdatePeriodInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeAfterPeriodicUpdateTimeAfterCompressorTurnedOn)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(OFF);

   WhenCompressorTurns(ON);
   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   After(1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeEveryPeriodicUpdateTimeAfterCompressorTurnedOn)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(OFF);

   WhenCompressorTurns(ON);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i * UpdatePeriodInSeconds);

      After(1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + (i + 1) * UpdatePeriodInSeconds);
   }
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostAfterCompressorTurnsOnAndRemainingMinimumTimeIsLessThanPeriodicUpdateTime)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - LessThanPeriodicUpdateInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(OFF);

   WhenCompressorTurns(ON);

   After(LessThanPeriodicUpdateInSeconds * MSEC_PER_SEC - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeAfterPeriodicUpdateTimeAfterCompressorTurnsOff)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   After(1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);

   WhenCompressorTurns(OFF);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);

   After(1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);
};

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenADoorClosesThatWasOpenOnInitWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   After(SomeSeconds * MSEC_PER_SEC);
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenMoreThanOneDoorIsAlreadyOpenOnInitAndThenOneClosesAfterTheOtherWhileWaitingForMinimumTimeBetweenDefrostsAndOnlyIncrementDoorAccelerationsForTimeThatADoorIsOpen)
{
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenFreezerDoorIs(Open);
   GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(Open);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   WhenFreezerDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   WhenConvertibleCompartmentAsFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(3 * SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenDoorClosesWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsAlreadyOnAndADoorIsAlreadyOpenOnInit)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeWhenDoorClosesWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsOffAndADoorIsAlreadyOpenOnInit)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeWhenDoorClosesWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsAlreadyOnAndADoorIsAlreadyOpenOnInit)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenDoorOpensWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsAlreadyOnAndDoorsAreClosedOnInit)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeWhenADoorOpensWithAnotherOneClosedWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsAlreadyOffAndAtLeastOneDoorIsOpenOnInit)
{
   GivenLeftHandFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeWhenDoorOpensWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsAlreadyOnAndADoorIsClosedOnInit)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationAndCompressorOnTimeAfterDoorIsOpenThenClosed)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();

   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   CompressorOnTimeInSecondsShouldBe(2 * SomeSeconds);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostAfterCompressorIsOnForMinimumTimeWhileADoorOpenedThenClosedDuringMinimumTime)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 2 * SomeSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostAfterCompressorIsOnForMinimumTimeWhileADoorClosedThenOpenedDuringMinimumTime)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 2 * SomeSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationForOpenDoorWhenCompressorTurnsOffDuringMinimumTime)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldNotUpdateDoorAccelerationsWhenCompressorTurnsOffIfDoorIsClosedDuringMinimumTime)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationForOpenDoorThatClosesAfterCompressorTurnsOffDuringMinimumTime)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForDoorThatOpensThenClosesWhileCompressorIsOffDuringMinimumTime)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(0);

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenCompressorTurnsOnAndAfterTheDoorClosesForAnOpenDoorDuringMinimumTime)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForDoorThatIsStillOpenWhileAnotherDoorOpenThenClosesAndCompressorIsOffDuringMinimumTime)
{
   GivenLeftHandFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostWhenTimerExpiresAfterInfiniteTimeFromWhenADoorClosedWhenCompressorWasNotOnForMinimumTime)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   After(UINT32_MAX);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenMinimumTimeReached)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsClosedAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(0);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   WhenFreezerDoorOpensAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(0);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 2 * SomeSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe((defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE -
                                       2 * SomeSeconds) *
      defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe((defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE) * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeAndWaitingToDefrostTimeWhenMinimumTimeReachedAndCompressorIsOn)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsClosedAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);

   WhenCompressorTurnsOnAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - SomeSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   WaitingDefrostTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeOrWaitingToDefrostTimeWhenTimerExpiresAfterInfiniteTimeAndCompressorIsNotOn)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsClosedAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeSeconds);

   After(UINT32_MAX);
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsOnEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(SomeSeconds);
   GivenFreezerDoorAccelerationInSecondsIs(SomeSeconds);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WaitingDefrostTimeInSecondsShouldBe(2 * SomeSeconds + defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenCompressorTurnsOffAfterBeingOnUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   After(SomeSeconds * MSEC_PER_SEC);

   WhenCompressorTurns(OFF);
   CompressorOnTimeInSecondsShouldBe(SomeSeconds + defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsIsReachedWithCompressorOn)
{
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) *
         MSEC_PER_MIN -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsIsReachedWithCompressorOnAndSomePreviousDoorAccelerations)
{
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(SomeSeconds);
   GivenFreezerDoorAccelerationInSecondsIs(SomeSeconds);
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) *
         MSEC_PER_MIN -
      (2 * SomeSeconds * MSEC_PER_SEC) - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsIsReachedWithCompressorOnAndADoorOpen)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   After(((defrostData->idleData.maxTimeBetweenDefrostsInMinutes -
             defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) *
            MSEC_PER_MIN) /
         (1 + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForOpenDoorsWhenCompressorTurnsOffAfterBeingOnUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsWhenCompressorTurnsOffAfterBeingOnUponEntryAndADoorWasAlsoOpenUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds +
      SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
      SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsIsReachedWithCompressorOffAndADoorOpen)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   After(((defrostData->idleData.maxTimeBetweenDefrostsInMinutes - defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) * MSEC_PER_MIN) /
         (defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForOpenDoorsWhenCompressorTurnsOnAfterBeingOffUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentAsFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsWhenCompressorTurnsOnAfterBeingOffUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentAsFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
      SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForDoorThatClosedWhenDoorClosesAfterBeingOpenUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForDoorsThatRemainOpenWhenADoorClosesAfterBeingOpenUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenDoorClosesAndCompressorWasOnUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeWhenDoorClosesAndCompressorWasOffUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsWhenDoorClosesAfterBeingOpenUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds +
      SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
      SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForDoorThatWasOpenWhenADifferentDoorOpensAfterBeingClosedUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(0);
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenDoorOpensAndCompressorWasOnUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeWhenDoorOpensAndCompressorWasOffUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsWhenDoorOpensAfterBeingOpenUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndCompressorIsOnAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds +
      SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterTheCompressorTurnsOffButADoorRemainsOpen)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * MSEC_PER_SEC -
            SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterTheCompressorTurnsOffAndDoorsAreClosed)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds);

   After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
      SomeSeconds * MSEC_PER_SEC -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterTheCompressorTurnsOnAndDoorsAreClosed)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();

   After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterTheCompressorTurnsOnAndADoorIsOpen)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 + 1 * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorOpensAndTheCompressorIsOn)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * MSEC_PER_SEC) /
         (1 + 1 * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorOpensAndTheCompressorIsOff)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorOpensAfterSomeSeconds();

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN) /
         (1 * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorClosesAndTheCompressorIsOn)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();

   After(
      defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
      SomeSeconds * MSEC_PER_SEC -
      SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorClosesAndTheCompressorIsOff)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorClosesAndTheCompressorIsOnAndAnotherDoorIsStillOpen)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * MSEC_PER_SEC -
            SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC -
            SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 + 1 * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorClosesAndTheCompressorIsOffAndAnotherDoorIsStillOpen)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOffAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC -
            SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdWhenUseMinimumReadyToDefrostIsSetToTrueWhileInWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenMaximumTimeReachedForOpenDoors)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOffAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC -
            SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
   LeftHandFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
                                       defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE -
                                       SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) /
      (1 * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond) * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenMaximumTimeReachedIfCompressorIsOn)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);

   After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
      SomeSeconds * MSEC_PER_SEC -
      SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
   CompressorOnTimeInSecondsShouldBe((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
      SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond));
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenMaximumTimeReachedIfCompressorIsTurnedOnLater)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 + 1 * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
   CompressorOnTimeInSecondsShouldBe((defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
                                        defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
                                        SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) /
         (1 + 1 * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) +
      SomeSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsWhenMaximumTimeReached)
{
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 + 1 * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
   WaitingDefrostTimeInSecondsShouldBe((defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
                                          defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
                                          SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) /
         (1 + 1 * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) +
      defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond *
         (((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
             defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)) /
            (1 + 1 * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond)));
}

TEST(ReadyToDefrost, ShouldResetWaitingDefrostTimeInSecondsToZeroWhenNoLongerWaitingToDefrost)
{
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenCompressorTurnsOffAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);

   WhenWaitingToDefrostIs(false);
   WaitingDefrostTimeInSecondsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldTransitionFromWaitingForMinimumTimeBetweenDefrostsToWaitingForRemainingTimeBetweenDefrostsWhenTimerExpiresAndItIsNotReadyToDefrost)
{
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsBelowMinimumByOneSecondAndWaitingToDefrostIsTrue();

   After(1 * MSEC_PER_SEC - 1);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   After(1);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdAfterMinimumTimeWhenResetDefrostCountsSignalWasReceivedWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(0);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);

   After(1);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldRestartMinimumTimeWhenResetDefrostCountsSignalWasReceivedAndSetReadyToDefrostErdAfterCompressorHasBeenOnForMinimumTimeWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIsOnForMinimumTimeAndUseMinimumReadyToDefrostTimeAndResetDefrostCountsErdChangedToTrueOneMillisecondBeforeTheMinimumTime();

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsPeriodicallyEvenIfDoorsDoNotCloseWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsClosedAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      LeftHandFreshFoodDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      FreezerDoorAccelerationsShouldBe(0);
      ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

      After(1);
      LeftHandFreshFoodDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      FreezerDoorAccelerationsShouldBe(0);
      ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsPeriodicallyAfterADoorOpensAndCompressorIsOffWhileWaitingForMinimumTimeBetweenDefrostsAndStopUpdatingAfterTheDoorCloses)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftHandFreshFoodDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      LeftHandFreshFoodDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

      After(1);
      LeftHandFreshFoodDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(ReadyToDefrost, ShouldStopUpdatingDoorAccelerationsPeriodicallyAfterTheDoorClosesWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenLeftHandFreshFoodDoorHasBeenOpenForTenPeriodicUpdatePeriodsAndCompressorIsOff();

   WhenLeftHandFreshFoodDoorIs(Closed);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      LeftHandFreshFoodDoorAccelerationsShouldBe(10 * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

      After(1);
      LeftHandFreshFoodDoorAccelerationsShouldBe(10 * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeOrSetReadyToDefrostErdAfterCompressorTurnsOffAndWasNotOnLongEnoughToReachMinimumTimeWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorIsOnForOneUpdatePeriod();
   CompressorOnTimeInSecondsShouldBe(UpdatePeriodInSeconds);

   WhenCompressorIsOffForOneUpdatePeriod();
   CompressorOnTimeInSecondsShouldBe(UpdatePeriodInSeconds);

   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 2 * UpdatePeriodInSeconds * MSEC_PER_SEC);
   CompressorOnTimeInSecondsShouldBe(UpdatePeriodInSeconds);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterALessThanPeriodicUpdateTimeAfterADoorOpensWhenCompressorHasAccumulatedMinimumTimeBetweenDefrostsWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 2 * (UpdatePeriodInSeconds));
   GivenLeftHandFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   After((3 * UpdatePeriodInSeconds * MSEC_PER_SEC) / 2);
   WhenLeftHandFreshFoodDoorIs(Opened);

   After((UpdatePeriodInSeconds * MSEC_PER_SEC / 2) - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenSomeDoorOpenTimeAfterResetDefrostCountsRequestReceivedWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(0);
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(SomeDoorAccelerationsInSecondsPerSecond);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   LeftHandFreshFoodDoorAccelerationsShouldBe(0);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN + SomeSeconds * MSEC_PER_SEC);

   WhenLeftHandFreshFoodDoorIs(Closed);
   LeftHandFreshFoodDoorAccelerationsShouldBe((defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds) *
      defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldResetWaitingForDefrostTimeInSecondsWhenResetDefrostCountRequestReceivedWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   WhenCompressorTurns(ON);

   After(SomeSeconds * MSEC_PER_SEC);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   WaitingDefrostTimeInSecondsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldResetWaitingForDefrostTimeInSecondsWhenResetDefrostCountRequestReceivedWhileInWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftHandFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds +
      SomeSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
      SomeSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   WaitingDefrostTimeInSecondsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMinimumTimeWhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsErdChangesToTrueDuringReadyAndDefrostingAfterWaitingToDefrostIsTrue)
{
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsFalseAndStateIsReadyAndDefrosting();

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   WhenWaitingToDefrostIs(true);

   WhenCompressorTurns(ON);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUseMaximumTimeOnNextDefrostAfterBeingForcedToUseMinimumTimeByUseMinimumTimeErd)
{
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsFalseAndStateIsReadyAndDefrosting();

   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   WhenWaitingToDefrostIs(true);

   WhenCompressorTurns(ON);
   After(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);

   WhenDefrostChangesFromWaitingToDefrostToDefrostingBackToWaitingToDefrost();

   After(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}
TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterALessThanPeriodicUpdateTimeAfterADoorClosesWhenCompressorHasAccumulatedMinimumTimeBetweenDefrostsWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 2 * (UpdatePeriodInSeconds));
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   After((3 * UpdatePeriodInSeconds * MSEC_PER_SEC) / 2);
   WhenLeftHandFreshFoodDoorIs(Closed);

   After((UpdatePeriodInSeconds * MSEC_PER_SEC / 2) - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsCorrectlyAfterUseMinimumReadyToDefrostTimeAndResetDefrostCountsErdChangesToTrueDuringAPeriodicUpdatePeriod)
{
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(SomeDoorAccelerationsInSecondsPerSecond);
   GivenLeftHandFreshFoodDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   After((UpdatePeriodInSeconds * MSEC_PER_SEC / 2));
   WhenUseMinimumReadyToDefrostTimeAndResetDefrostCountsChangesTo(true);
   LeftHandFreshFoodDoorAccelerationsShouldBe(0);

   After((UpdatePeriodInSeconds * MSEC_PER_SEC / 2));
   LeftHandFreshFoodDoorAccelerationsShouldBe(0);

   After((UpdatePeriodInSeconds * MSEC_PER_SEC / 2) - 1);
   LeftHandFreshFoodDoorAccelerationsShouldBe(0);

   After(1);
   LeftHandFreshFoodDoorAccelerationsShouldBe(((UpdatePeriodInSeconds * MSEC_PER_SEC) / MSEC_PER_SEC) * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterHalfAPeriodicUpdatePeriodWithTheCompressorOnIfCompressorOnTimeIsMaxMinusHalfAPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (UpdatePeriodInSeconds) / 2);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2 - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterHalfAPeriodicUpdatePeriodWithADoorOpenAndCompressorOffIfCompressorOnTimeIsMaxMinusHalfAPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds)) / 2);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2 - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterHalfAPeriodicUpdatePeriodFromWhenTheCompressorTurnedOnIfCompressorOnTimeIsMaxMinusHalfAPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (UpdatePeriodInSeconds) / 2);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenCompressorTurns(ON);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2 - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterHalfAPeriodicUpdatePeriodFromWhenCompressorTurnsOffIfThereIsADoorOpenAndIfCompressorOnTimeIsMaxMinusHalfAPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(ON);
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds)) / 2);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenCompressorTurns(OFF);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2 - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeAfterCompressorHasBeenTurnedOffWhileWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(ON);
   GivenLeftHandFreshFoodDoorIs(Closed);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - 10 * (UpdatePeriodInSeconds));
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenCompressorTurns(OFF);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC);
      CompressorOnTimeInSecondsShouldBe(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - 10 * (UpdatePeriodInSeconds));
   }
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterATimeLessThanPeriodicUpdatePeriodFromWhenADoorOpensIfCompressorIsOnAndIfRemainingTimeUntilReadyIsLessThanPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(ON);
   GivenLeftHandFreshFoodDoorIs(Closed);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds)) / 2);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftHandFreshFoodDoorIs(Opened);

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
            (defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
               (defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds * MSEC_PER_SEC)) / 2)) /
         (1 + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterATimeLessThanPeriodicUpdatePeriodFromWhenADoorOpensIfCompressorIsOffAndIfRemainingTimeUntilReadyIsLessThanPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenLeftHandFreshFoodDoorIs(Closed);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds / 2)));
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftHandFreshFoodDoorIs(Opened);

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
            (defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
               (defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds * MSEC_PER_SEC)) / 2)) /
         (defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterATimeLessThanPeriodicUpdatePeriodFromWhenADoorClosesIfCompressorIsOnAndIfRemainingTimeUntilReadyIsLessThanPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(ON);
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (UpdatePeriodInSeconds / 2));
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftHandFreshFoodDoorIs(Closed);

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
      (defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
         (UpdatePeriodInSeconds * MSEC_PER_SEC / 2)) -
      1));
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterATimeLessThanPeriodicUpdatePeriodFromWhenADoorClosesIfCompressorIsOffAndADoorIsStillOpenAndIfRemainingTimeUntilReadyIsLessThanPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenFreezerDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds / 2)));
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftHandFreshFoodDoorIs(Closed);

   After((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
            (defrostData->idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
               (defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds * MSEC_PER_SEC)) / 2)) /
         (defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond) -
      1);

   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsPeriodicallyWhileWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenFreezerDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   uint8_t maxPeriodicUpdatesBeforeReadyToDefrost = ((defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
                                                        defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE) /
                                                       (defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
                                                          defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond)) /
      UpdatePeriodInSeconds;

   for(uint8_t i = 0; i < maxPeriodicUpdatesBeforeReadyToDefrost; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      LeftHandFreshFoodDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      FreezerDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);

      After(1);
      LeftHandFreshFoodDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      FreezerDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(ReadyToDefrost, ShouldNotUpdateDoorAccelerationsAfterAllDoorsHaveBeenClosedWhileWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenLeftHandFreshFoodDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenLeftHandFreshFoodDoorAccelerationInSecondsIs(SomeDoorAccelerationsInSecondsPerSecond);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftHandFreshFoodDoorIs(Closed);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC);
      LeftHandFreshFoodDoorAccelerationsShouldBe(SomeDoorAccelerationsInSecondsPerSecond);
   }
}
