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

#define Given
#define When
#define Then
#define And

#define MinTimeBetweenDefrostsInMinutes (defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes)
#define MinTimeBetweenDefrostsInSeconds (defrostData->idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
#define MaxTimeBetweenDefrostInSeconds (defrostData->idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE)

static ReadyToDefrostConfiguration_t config = {
   .defrostCompressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freshFoodScaledDoorAccelerationInSecondsErd = Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds,
   .freezerScaledDoorAccelerationInSecondsErd = Erd_DefrostFreezerScaledDoorAccelerationInSeconds,
   .convertibleCompartmentScaledDoorAccelerationInSecondsErd = Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds,
   .readyToDefrostErd = Erd_ReadyToDefrost,
   .defrostCompressorOnTimeCounterReadyErd = Erd_DefrostCompressorOnTimeCounterReady,
   .doorAccelerationCounterReadyErd = Erd_DoorAccelerationCounterReady,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .useMinimumReadyToDefrostTimeErd = Erd_UseMinimumReadyToDefrostTime,
   .invalidFreezerEvaporatorThermistorDuringDefrostErd = Erd_InvalidFreezerEvaporatorThermistorDuringDefrost,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .hasConvertibleCompartment = Erd_HasConvertibleCompartment,
   .eepromClearedErd = Erd_Eeprom_ClearedDefrostEepromStartup,
   .waitingForDefrostTimeInSecondsErd = Erd_WaitingForDefrostTimeInSeconds,
   .waitingToDefrostErd = Erd_WaitingToDefrost
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

   void GivenReadyToDefrostModuleHasBeenInitialized()
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeCounterReady, set);
      DataModel_Write(dataModel, Erd_DoorAccelerationCounterReady, set);

      ReadyToDefrost_Init(&instance, dataModel, &config, defrostData);
   }

   void GivenReadyToDefrostHasBeen(bool state)
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, &state);
   }

   void GivenCompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void GivenFreshFoodDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds, &count);
   }

   void GivenFreezerDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreezerScaledDoorAccelerationInSeconds, &count);
   }

   void GivenConvertibleCompartmentDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds, &count);
   }

   void ReadyToDefrostShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_ReadyToDefrost, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FreezerFilteredTemperatureTooWarmAtPowerUpShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void UseMinimumReadyToDefrostTimeShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_UseMinimumReadyToDefrostTime, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void WaitingForDefrostTimeInSecondsShouldBe(uint32_t expectedValue)
   {
      uint32_t actualValue;
      DataModel_Read(dataModel, Erd_WaitingForDefrostTimeInSeconds, &actualValue);

      CHECK_EQUAL(expectedValue, actualValue);
   }

   void GivenFreezerFilteredTemperatureTooWarmAtPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }

   void WhenFreezerFilteredTemperatureTooWarmAtPowerUpChangesTo(bool state)
   {
      GivenFreezerFilteredTemperatureTooWarmAtPowerUpIs(state);
   }

   void GivenUseMinimumReadyToDefrostTimeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_UseMinimumReadyToDefrostTime, &state);
   }

   void WhenUseMinimumReadyToDefrostTimeChangesTo(bool state)
   {
      GivenUseMinimumReadyToDefrostTimeIs(state);
   }

   void GivenFreezerEvaporatorThermistorIs(bool valid)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &valid);
   }

   void WhenFreezerEvaporatorThermistorChangesTo(bool valid)
   {
      GivenFreezerEvaporatorThermistorIs(valid);
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

   void GivenEepromClearedFlagIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, &state);
   }

   void WhenEepromClearedFlagChangesTo(bool state)
   {
      GivenEepromClearedFlagIs(state);
   }

   void EepromClearedFlagShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenHasAConvertibleCompartmentIs(bool state)
   {
      DataModel_Write(dataModel, Erd_HasConvertibleCompartment, &state);
   }

   void WhenItNoLongerHasAConvertibleCompartment()
   {
      DataModel_Write(dataModel, Erd_HasConvertibleCompartment, clear);
   }

   void GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_InvalidFreezerEvaporatorThermistorDuringDefrost, &state);
   }

   void WhenInvalidFreezerEvaporatorThermistorDuringDefrostChangesTo(bool state)
   {
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(state);
   }

   void GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime()
   {
      GivenEepromClearedFlagIs(SET);
   }

   void GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime()
   {
      GivenFreezerFilteredTemperatureTooWarmAtPowerUpIs(CLEAR);
      GivenUseMinimumReadyToDefrostTimeIs(CLEAR);
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(CLEAR);
      GivenFreshFoodDefrostWasAbnormalIs(CLEAR);
      GivenFreezerDefrostWasAbnormalIs(CLEAR);
      GivenConvertibleCompartmentDefrostWasAbnormalIs(CLEAR);
      GivenEepromClearedFlagIs(CLEAR);
   }

   void WhenConditionsChangeSuchThatTimeBetweenDefrostsIsNowNormalTime()
   {
      GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   }

   void WhenConditionsChangeSuchThatTimeBetweenDefrostsIsNowMinimumTime()
   {
      GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   }

   void PreviousDefrostsAreNormal()
   {
      GivenFreshFoodDefrostWasAbnormalIs(CLEAR);
      GivenFreezerDefrostWasAbnormalIs(CLEAR);
      GivenConvertibleCompartmentDefrostWasAbnormalIs(CLEAR);
   }

   void PreviousDefrostsAreNormalAndTimeWhenGivenReadyToDefrostHasBeenTimeBetweenDefrostsIsNormalTimesInMinutes()
   {
      Given PreviousDefrostsAreNormal();
      And GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();

      And GivenReadyToDefrostModuleHasBeenInitialized();
   }

   void TheTimeBetweenDefrostsIsNormalTimeAndGivenReadyToDefrostHasBeenClearAndTheReadyToDefrostModuleIsInitialized()
   {
      Given GivenReadyToDefrostHasBeen(CLEAR);
      And GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
      And GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();

      And GivenReadyToDefrostModuleHasBeenInitialized();
   }

   void TheTimeBetweenDefrostsIsNormalTimeAndGivenReadyToDefrostHasBeenSetAndTheReadyToDefrostModuleIsInitialized()
   {
      Given GivenReadyToDefrostHasBeen(SET);
      And GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
      And GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();

      And GivenReadyToDefrostModuleHasBeenInitialized();
   }

   void WhenCompressorOnTimeInSecondsChangesTo(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void WhenFreshFoodDoorAccelerationChangesTo(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds, &count);
   }

   void WhenFreezerDoorAccelerationChangesTo(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreezerScaledDoorAccelerationInSeconds, &count);
   }

   void WhenConvertibleCompartmentDoorAccelerationChangesTo(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds, &count);
   }

   void GivenItIsWaitingToDefrost()
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, set);
   }

   void GivenItIsNotWaitingToDefrost()
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, clear);
   }

   void WhenItIsNoLongerWaitingToDefrost()
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, clear);
   }

   void WhenItIsWaitingToDefrost()
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, set);
   }
};

TEST(ReadyToDefrost, ShouldInitialize)
{
   GivenReadyToDefrostModuleHasBeenInitialized();
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostsAbnormalRunTimeOnInitIfSystemIsWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostsAbnormalRunTimeOnInitIfSystemIsWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds + 1);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdWhenCompressorOnTimeIsLessThanMinimumTimeBetweenDefrostsAbnormalRunTimeOnInitIfSystemIsWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeHasReachedMinimumTimeBetweenDefrostsAndSumOfCompressorOnTimeAndDoorAccelerationsAreGreaterThanTimeWhenReadyToDefrostOnInitIfSystemIsWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(1);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdWhenCompressorOnTimeIsLessThanMinimumTimeBetweenDefrostsButSumOfCompressorOnTimeAndDoorAccelerationsIsEqualtoTimeWhenReadyToDefrostOnInitIfSystemIsWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   GivenFreshFoodDoorAccelerationIs(1);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdWhenSumOfCompressorOnTimeAndDoorAccelerationsIsLessThanTimeBetweenDefrostsOnInitForNormalConditionsIfSystemIsWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostsAbnormalRunTimeGivenConditionsAreSuchThatTimeBetweenDefrostIsMinimumTimeIfSystemIsWaitingToDefrost)
{
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenCompressorOnTimeInSecondsChangesTo(MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostsAbnormalRunTimeGivenConditionsAreSuchThatTimeBetweenDefrostIsMinimumTimeIfSystemIsWaitingToDefrost)
{
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenCompressorOnTimeInSecondsChangesTo(MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenReadyToDefrostIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenFreshFoodDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenReadyToDefrostIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenFreezerDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsEqualToTimeWhenReadyToDefrostIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenConvertibleCompartmentDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreshFoodDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenReadyToDefrostIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenFreshFoodDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreezerDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenReadyToDefrostIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenFreezerDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenConvertibleCompartmentDoorAccelerationChangesSuchThatSumOfCompressorOnTimeAndDoorAccelerationIsGreaterThanTimeWhenReadyToDefrostIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenConvertibleCompartmentDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds + 1);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreezerFilteredTemperatureTooWarmAtPowerUpChangesFromClearToSetSoThatTimeWhenReadyToDefrostChangesFromNormalTimeToMinimumTimeIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenFreezerFilteredTemperatureTooWarmAtPowerUpChangesTo(SET);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreshFoodDefrostWasAbnormalChangesFromClearToSetSoThatTimeWhenReadyToDefrostChangesFromNormalTimeToMinimumTimeIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenFreshFoodDefrostWasAbnormalChangesTo(SET);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenFreezerDefrostWasAbnormalChangesFromClearToSetSoThatTimeWhenReadyToDefrostChangesFromNormalTimeToMinimumTimeIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenFreezerDefrostWasAbnormalChangesTo(SET);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenConvertibleCompartmentDefrostWasAbnormalChangesFromClearToSetSoThatTimeWhenReadyToDefrostChangesFromNormalTimeToMinimumTimeIfSystemIsWaitingToDefrost)
{
   GivenHasAConvertibleCompartmentIs(SET);
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenConvertibleCompartmentDefrostWasAbnormalChangesTo(SET);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenInvalidFreezerEvaporatorThermistorDuringDefrostChangesFromClearToSetSoThatTimeWhenReadyToDefrostChangesFromNormalTimeToMinimumTimeIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenInvalidFreezerEvaporatorThermistorDuringDefrostChangesTo(SET);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenUseMinimumReadyToDefrostTimeChangesFromClearToSetSoThatTimeWhenReadyToDefrostChangesFromNormalTimeToMinimumTimeIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenUseMinimumReadyToDefrostTimeChangesTo(SET);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenEepromClearedFlagChangesFromClearToSetSoThatTimeWhenReadyToDefrostChangesFromNormalTimeToMinimumTimeIfSystemIsWaitingToDefrost)
{
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenEepromClearedFlagChangesTo(SET);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdWhenCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostsAbnormalRunTimeOnInitAndConditionsAreSuchThatTimeBetweenDefrostsIsMinimumTimeBetweenDefrostsIfSystemIsNotWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenItIsNotWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostsButWithAdditionOfFreshFoodDoorAccelerationsTheCountIsEqualToNormalTimeBetweenDefrostsIfSystemIsNotWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsNotWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenFreshFoodDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostsButWithAdditionOfFreezerDoorAccelerationsTheCountIsEqualToNormalTimeBetweenDefrostsIfSystemIsNotWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsNotWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenFreezerDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostsButWithAdditionOfConvertibleCompartmentDoorAccelerationsTheCountIsEqualToNormalTimeBetweenDefrostsIfSystemIsNotWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenItIsNotWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenConvertibleCompartmentDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdWhenSystemIsNotWaitingToDefrost)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(0);
   GivenItIsNotWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(CLEAR);

   WhenCompressorOnTimeInSecondsChangesTo(MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(CLEAR);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdOnceSystemIsWaitingToDefrostAgainAndCompressorOnTimeChangesToMeetMinTimeBetweenDefrosts)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(SET);

   WhenItIsNoLongerWaitingToDefrost();
   WhenCompressorOnTimeInSecondsChangesTo(0);
   ReadyToDefrostShouldBe(SET);

   WhenItIsWaitingToDefrost();
   ReadyToDefrostShouldBe(CLEAR);

   WhenCompressorOnTimeInSecondsChangesTo(MinTimeBetweenDefrostsInSeconds);
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdOnceSystemIsWaitingToDefrostAgainAndCompressorOnTimeAlreadyMeetsMinTimeBetweenDefrosts)
{
   GivenReadyToDefrostHasBeen(CLEAR);
   GivenEepromWasClearedSoTimeBetweenDefrostsIsMinimumTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenItIsWaitingToDefrost();
   GivenReadyToDefrostModuleHasBeenInitialized();

   ReadyToDefrostShouldBe(SET);

   WhenItIsNoLongerWaitingToDefrost();
   ReadyToDefrostShouldBe(SET);

   WhenItIsWaitingToDefrost();
   ReadyToDefrostShouldBe(SET);
}

TEST(ReadyToDefrost, ShouldSetWaitingForDefrostTimeInSecondsToDefrostCompressorOnTimeInSecondsIfDefrostCompressorOnTimeInSecondsIsLessThanMinimumTimeBetweenDefrostsAbnormalRunTimeInMinutesOnInit)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MinTimeBetweenDefrostsInSeconds - 1);
}

TEST(ReadyToDefrost, ShouldSetWaitingForDefrostTimeInSecondsToSumOfDefrostCompressorOnTimeInSecondsAndDoorAccelerationsIfDefrostCompressorOnTimeInSecondsIsGreaterThanOrEqualToMinimumTimeBetweenDefrostsAbnormalRunTimeInMinutesButBelowTimeBetweenDefrostsOnInit)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(1000);
   GivenFreezerDoorAccelerationIs(200);
   GivenConvertibleCompartmentDoorAccelerationIs(245);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MinTimeBetweenDefrostsInSeconds + 1000 + 200 + 245);
}

TEST(ReadyToDefrost, ShouldSetWaitingForDefrostTimeInSecondsToSumOfDefrostCompressorOnTimeInSecondsAndDoorAccelerationsIfDefrostCompressorOnTimeInSecondsIsGreaterThanOrEqualToMinimumTimeBetweenDefrostsAbnormalRunTimeInMinutesAndEqualToTimeBetweenDefrostsOnInit)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MaxTimeBetweenDefrostInSeconds);
}

TEST(ReadyToDefrost, ShouldSetWaitingForDefrostTimeInSecondsToSumOfDefrostCompressorOnTimeInSecondsAndDoorAccelerationsIfDefrostCompressorOnTimeInSecondsIsGreaterThanOrEqualsToMinimumTimeBetweenDefrostsAbnormalRunTimeInMinutesAndGreaterThanTimeBetweenDefrostsOnInit)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds + 1);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MaxTimeBetweenDefrostInSeconds + 1);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingForDefrostTimeInSecondsWhenCompressorOnTimeChangesToLessThanMinTimeBetweenDefrostsButIsLessThanTimeBetweenDefrosts)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 2);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MinTimeBetweenDefrostsInSeconds - 2);

   WhenCompressorOnTimeInSecondsChangesTo(MinTimeBetweenDefrostsInSeconds - 1);
   WaitingForDefrostTimeInSecondsShouldBe(MinTimeBetweenDefrostsInSeconds - 1);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingForDefrostTimeInSecondsWhenCompressorOnTimeChangesToMinTimeBetweenDefrostsButIsLessThanTimeBetweenDefrosts)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds - 1);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MinTimeBetweenDefrostsInSeconds - 1);

   WhenCompressorOnTimeInSecondsChangesTo(MinTimeBetweenDefrostsInSeconds);
   WaitingForDefrostTimeInSecondsShouldBe(MinTimeBetweenDefrostsInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingForDefrostTimeInSecondsWhenCompressorOnTimeChangesToMinTimeBetweenDefrostsButIsEqualToTimeBetweenDefrosts)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MinTimeBetweenDefrostsInSeconds);

   WhenCompressorOnTimeInSecondsChangesTo(MaxTimeBetweenDefrostInSeconds);
   WaitingForDefrostTimeInSecondsShouldBe(MaxTimeBetweenDefrostInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingForDefrostTimeInSecondsWhenCompressorOnTimeChangesToMoreThanMinTimeBetweenDefrostsButTotalTimeIsLessThanTimeBetweenDefrosts)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(1000);
   GivenFreezerDoorAccelerationIs(200);
   GivenConvertibleCompartmentDoorAccelerationIs(245);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MinTimeBetweenDefrostsInSeconds + 1000 + 200 + 245);

   WhenCompressorOnTimeInSecondsChangesTo(MinTimeBetweenDefrostsInSeconds + 123);
   WaitingForDefrostTimeInSecondsShouldBe(MinTimeBetweenDefrostsInSeconds + 1000 + 200 + 245 + 123);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingForDefrostTimeInSecondsWhenFreshFoodDoorAccelerationChangesSoThatTotalTimeIsEqualToTimeBetweenDefrosts)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MaxTimeBetweenDefrostInSeconds - 1);

   WhenFreshFoodDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   WaitingForDefrostTimeInSecondsShouldBe(MaxTimeBetweenDefrostInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingForDefrostTimeInSecondsWhenFreezerDoorAccelerationChangesSoThatTotalTimeIsEqualToTimeBetweenDefrosts)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   GivenConvertibleCompartmentDoorAccelerationIs(0);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MaxTimeBetweenDefrostInSeconds - 1);

   WhenFreezerDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   WaitingForDefrostTimeInSecondsShouldBe(MaxTimeBetweenDefrostInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingForDefrostTimeInSecondsWhenConvertibleCompartmentDoorAccelerationChangesSoThatTotalTimeIsEqualToTimeBetweenDefrosts)
{
   GivenItIsWaitingToDefrost();
   GivenConditionsAreSuchThatTimeBetweenDefrostsIsNormalTime();
   GivenCompressorOnTimeInSecondsIs(MinTimeBetweenDefrostsInSeconds);
   GivenFreshFoodDoorAccelerationIs(0);
   GivenFreezerDoorAccelerationIs(0);
   GivenConvertibleCompartmentDoorAccelerationIs(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds - 1);
   GivenReadyToDefrostModuleHasBeenInitialized();

   WaitingForDefrostTimeInSecondsShouldBe(MaxTimeBetweenDefrostInSeconds - 1);

   WhenConvertibleCompartmentDoorAccelerationChangesTo(MaxTimeBetweenDefrostInSeconds - MinTimeBetweenDefrostsInSeconds);
   WaitingForDefrostTimeInSecondsShouldBe(MaxTimeBetweenDefrostInSeconds);
}
