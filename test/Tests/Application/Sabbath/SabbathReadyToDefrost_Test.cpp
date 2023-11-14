/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SabbathReadyToDefrost.h"
#include "Constants_Time.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   UpdatePeriodInMs = MSEC_PER_MIN
};

static const SabbathReadyToDefrostConfig_t config = {
   .timerModuleErd = Erd_TimerModule,
   .waitingToDefrostErd = Erd_WaitingToDefrost,
   .sabbathIsReadyToDefrostErd = Erd_SabbathIsReadyToDefrost,
   .sabbathTimeBetweenDefrostsInMinutesErd = Erd_SabbathTimeBetweenDefrostsInMinutes,
   .sabbathWaitingForDefrostTimeInMinutesErd = Erd_SabbathWaitingForDefrostTimeInMinutes
};

TEST_GROUP(SabbathReadyToDefrost)
{
   SabbathReadyToDefrost_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   const SabbathData_t *sabbathData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      sabbathData = PersonalityParametricData_Get(dataModel)->sabbathData;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      SabbathReadyToDefrost_Init(&instance, dataModel, &config, sabbathData);
   }

   void WhenTheModuleIsInitialized()
   {
      GivenTheModuleIsInitialized();
   }

   void After(TimerTicks_t delay)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, delay);
   }

   void GivenWaitingToDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, &state);
   }

   void WhenWaitingToDefrostIs(bool state)
   {
      GivenWaitingToDefrostIs(state);
   }

   void GivenSabbathIsReadyToDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SabbathIsReadyToDefrost, &state);
   }

   void GivenSabbathWaitingForDefrostTimeInMinutesIs(uint16_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_SabbathWaitingForDefrostTimeInMinutes, &timeInMinutes);
   }

   void SabbathIsReadyToDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_SabbathIsReadyToDefrost, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void SabbathTimeBetweenDefrostsInMinutesShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_SabbathTimeBetweenDefrostsInMinutes, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void SabbathWaitingForDefrostTimeInMinutesShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_SabbathWaitingForDefrostTimeInMinutes, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(SabbathReadyToDefrost, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(SabbathReadyToDefrost, ShouldClearSabbathReadyToDefrostWhenWaitingToDefrostIsTrueOnInit)
{
   GivenWaitingToDefrostIs(true);
   GivenSabbathIsReadyToDefrostIs(true);

   WhenTheModuleIsInitialized();
   SabbathIsReadyToDefrostShouldBe(false);
}

TEST(SabbathReadyToDefrost, ShouldSetSabbathTimeBetweenDefrostsInMinutesToSabbathTimeBetweenDefrostsParametricValueWhenWaitingToDefrostIsTrueOnInit)
{
   GivenWaitingToDefrostIs(true);

   WhenTheModuleIsInitialized();
   SabbathTimeBetweenDefrostsInMinutesShouldBe(sabbathData->timeBetweenDefrostsInMinutes);
}

TEST(SabbathReadyToDefrost, ShouldNotClearSabbathReadyToDefrostWhenWaitingToDefrostIsFalseOnInit)
{
   GivenWaitingToDefrostIs(false);
   GivenSabbathIsReadyToDefrostIs(true);

   WhenTheModuleIsInitialized();
   SabbathIsReadyToDefrostShouldBe(true);
}

TEST(SabbathReadyToDefrost, ShouldSetSabbathTimeBetweenDefrostsToSabbathTimeBetweenDefrostsParametricValueWhenWaitingToDefrostIsFalseOnInit)
{
   GivenWaitingToDefrostIs(false);

   WhenTheModuleIsInitialized();
   SabbathTimeBetweenDefrostsInMinutesShouldBe(sabbathData->timeBetweenDefrostsInMinutes);
}

TEST(SabbathReadyToDefrost, ShouldIncrementSabbathWaitingForDefrostTimeWhileWaitingToDefrost)
{
   GivenWaitingToDefrostIs(true);
   GivenTheModuleIsInitialized();

   After(UpdatePeriodInMs - 1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(0);

   After(1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(1);

   After(UpdatePeriodInMs);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(2);
}

TEST(SabbathReadyToDefrost, ShouldSetSabbathReadyToDefrostAndSabbathWaitingForDefrostTimeInMinutesBeEqualToSabbathTimeBetweenDefrostsInMinutesAfterSabbathTimeBetweenDefrostMinutes)
{
   GivenWaitingToDefrostIs(true);
   GivenTheModuleIsInitialized();

   After(sabbathData->timeBetweenDefrostsInMinutes * MSEC_PER_MIN - 1);
   SabbathIsReadyToDefrostShouldBe(false);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(sabbathData->timeBetweenDefrostsInMinutes - 1);

   After(1);
   SabbathIsReadyToDefrostShouldBe(true);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(sabbathData->timeBetweenDefrostsInMinutes);
}

TEST(SabbathReadyToDefrost, ShouldNotResetAndNotCountSabbathWaitingForDefrostTimeInMinutesWhenWaitingToDefrostChangesFromTrueToFalse)
{
   GivenWaitingToDefrostIs(true);
   GivenTheModuleIsInitialized();

   After(UpdatePeriodInMs - 1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(0);

   After(1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(1);

   WhenWaitingToDefrostIs(false);

   After(UpdatePeriodInMs - 1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(1);

   After(1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(1);
}

TEST(SabbathReadyToDefrost, ShouldResetAndStartCountingSabbathWaitingForDefrostTimeInMinutesWhenWaitingToDefrostChangesFromFalseToTrue)
{
   GivenWaitingToDefrostIs(true);
   GivenTheModuleIsInitialized();

   After(UpdatePeriodInMs);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(1);

   WhenWaitingToDefrostIs(false);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(1);

   WhenWaitingToDefrostIs(true);

   After(UpdatePeriodInMs - 1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(0);

   After(1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(1);
}

TEST(SabbathReadyToDefrost, ShouldNotStartCountingSabbathWaitingForDefrostTimeInMinutesWhileWaitingToDefrostIsFalse)
{
   GivenWaitingToDefrostIs(false);
   GivenTheModuleIsInitialized();
   SabbathWaitingForDefrostTimeInMinutesShouldBe(0);

   After(UpdatePeriodInMs);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(0);
}

TEST(SabbathReadyToDefrost, ShouldStartCountingSabbathWaitingForDefrostTimeInMinutesWhenWaitingToDefrostChangesToTrue)
{
   GivenWaitingToDefrostIs(false);
   GivenTheModuleIsInitialized();

   WhenWaitingToDefrostIs(true);

   After(UpdatePeriodInMs - 1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(0);

   After(1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(1);
}

TEST(SabbathReadyToDefrost, ShouldStartCountingFromSavedSabbathWaitingForDefrostTimeInMinutes)
{
   GivenWaitingToDefrostIs(true);
   GivenSabbathWaitingForDefrostTimeInMinutesIs(sabbathData->timeBetweenDefrostsInMinutes - 1);
   GivenTheModuleIsInitialized();

   After(UpdatePeriodInMs - 1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(sabbathData->timeBetweenDefrostsInMinutes - 1);

   After(1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(sabbathData->timeBetweenDefrostsInMinutes);
}

TEST(SabbathReadyToDefrost, ShouldStartCountingFromZeroWhenWaitingToDefrostChangesToTrueAfterPoweringUpWithANonZeroSabbathWaitingForDefrostTime)
{
   GivenWaitingToDefrostIs(false);
   GivenSabbathWaitingForDefrostTimeInMinutesIs(sabbathData->timeBetweenDefrostsInMinutes - 1);
   GivenTheModuleIsInitialized();

   WhenWaitingToDefrostIs(true);
   After(UpdatePeriodInMs - 1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(0);

   After(1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(1);
}

TEST(SabbathReadyToDefrost, ShouldSetSabbathIsReadyToDefrostIfSabbathWaitingForDefrostTimeAlreadyEqualsSabbathTimeBetweenDefrostsAndWaitingToDefrostIsTrueOnPowerUp)
{
   GivenWaitingToDefrostIs(true);
   GivenSabbathWaitingForDefrostTimeInMinutesIs(sabbathData->timeBetweenDefrostsInMinutes);
   GivenTheModuleIsInitialized();

   SabbathIsReadyToDefrostShouldBe(true);
}

TEST(SabbathReadyToDefrost, ShouldContinueCountingSabbathWaitingForDefrostTimeAfterSabbathIsReadyToDefrostIsSetIfSabbathWaitingForDefrostTimeAlreadyEqualsSabbathTimeBetweenDefrostsAndWaitingToDefrostIsTrueOnPowerUpAndWaitingToDefrostDoesNotChangeToFalse)
{
   GivenWaitingToDefrostIs(true);
   GivenSabbathWaitingForDefrostTimeInMinutesIs(sabbathData->timeBetweenDefrostsInMinutes);
   GivenTheModuleIsInitialized();

   SabbathIsReadyToDefrostShouldBe(true);

   After(UpdatePeriodInMs - 1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(sabbathData->timeBetweenDefrostsInMinutes);

   After(1);
   SabbathWaitingForDefrostTimeInMinutesShouldBe(sabbathData->timeBetweenDefrostsInMinutes + 1);
}

TEST(SabbathReadyToDefrost, ShouldNotSetSabbathIsReadyToDefrostIfSabbathWaitingForDefrostTimeAlreadyEqualsSabbathTimeBetweenDefrostsOnPowerUpWhenWaitingToDefrostChangesToTrue)
{
   GivenWaitingToDefrostIs(false);
   GivenSabbathWaitingForDefrostTimeInMinutesIs(sabbathData->timeBetweenDefrostsInMinutes);
   GivenTheModuleIsInitialized();

   WhenWaitingToDefrostIs(true);
   SabbathIsReadyToDefrostShouldBe(false);
}
