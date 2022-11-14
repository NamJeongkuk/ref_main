/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DefrostCompressorOnTimeCounter.h"
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

enum
{
   SomeCompressorOnTimeInSeconds = 2350
};

enum
{
   PeriodicTimerTicksInMs = 1 * MSEC_PER_SEC
};

static const DefrostCompressorOnTimeCounterConfiguration_t config = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .waitingToDefrostErd = Erd_WaitingToDefrost,
   .defrostCompressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .defrostCompressorOnTimeCounterFsmStateErd = Erd_DefrostCompressorOnTimeCounterFsmState,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .defrostCompressorOnTimeCounterReadyErd = Erd_DefrostCompressorOnTimeCounterReady,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(DefrostCompressorOnTimeCounter)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   DefrostCompressorOnTimeCounter_t instance;

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

   void DefrostCompressorOnTimeCounterIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);

      DefrostCompressorOnTimeCounter_Init(&instance, dataModel, &config);
   }

   void DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_t expectedState)
   {
      DefrostCompressorOnTimeCounterFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostCompressorOnTimeCounterFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void CompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void CompressorOnTimeInSecondsShouldBe(uint32_t expectedSeconds)
   {
      uint32_t actualSeconds;
      DataModel_Read(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &actualSeconds);

      CHECK_EQUAL(expectedSeconds, actualSeconds);
   }

   void DefrostCompressorOnTimeCounterIsInPauseState()
   {
      Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
      And DefrostCompressorOnTimeCounterIsInitialized();

      DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Pause);
   }

   void DefrostCompressorOnTimeCounterIsInStopState()
   {
      Given WaitingToDefrostIs(true);
      And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
      And DefrostCompressorOnTimeCounterIsInitialized();

      DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);

      When WaitingToDefrostIs(false);
      DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Stop);
   }

   void WaitingToDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, &state);
   }

   void DefrostCompressorOnTimeCounterIsInRunState()
   {
      Given DefrostCompressorOnTimeCounterIsInPauseState();

      When WaitingToDefrostIs(true);
      DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);
   }

   void CompressorIsOn()
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, on);
   }

   void CompressorIsOff()
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, off);
   }

   void CompressorOnTimeShouldBeIncrementedForThisManySeconds(uint8_t seconds)
   {
      for(uint8_t i = 0; i < seconds; i++)
      {
         After(PeriodicTimerTicksInMs - 1);
         CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i);

         After(1);
         CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i + 1);
      }
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }
};

TEST(DefrostCompressorOnTimeCounter, ShouldInitializeIntoPauseStateWhenFreezerFilteredTemperatureWasNotTooWarmOnPowerUp)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostCompressorOnTimeCounterIsInitialized();

   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Pause);
}

TEST(DefrostCompressorOnTimeCounter, ShouldTransitionFromRunStateToStopStateWhenNoLongerWaitingToDefrost)
{
   Given WaitingToDefrostIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostCompressorOnTimeCounterIsInitialized();

   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);

   When WaitingToDefrostIs(false);
   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Stop);
}

TEST(DefrostCompressorOnTimeCounter, ShouldResetCompressorOnTimeInSecondsToZeroWhenEnteringStopState)
{
   Given CompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   And DefrostCompressorOnTimeCounterIsInStopState();

   CompressorOnTimeInSecondsShouldBe(0);
}

TEST(DefrostCompressorOnTimeCounter, ShouldNotResetCompressorOnTimeInSecondsToZeroWhenEnteringPauseState)
{
   Given CompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   And DefrostCompressorOnTimeCounterIsInPauseState();

   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
}

TEST(DefrostCompressorOnTimeCounter, ShouldResetCompressorOnTimeInSecondsWhenFreezerFilteredTemperatureTooWarmOnPowerUpAndWhenWaitingToDefrostIsTrue)
{
   Given WaitingToDefrostIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And CompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   And DefrostCompressorOnTimeCounterIsInitialized();

   CompressorOnTimeInSecondsShouldBe(0);
}

TEST(DefrostCompressorOnTimeCounter, ShouldThenStartCountingCompressorOnTimeWhenFreezerFilteredTemperatureTooWarmOnPowerUpAndWhenWaitingToDefrostIsTrue)
{
   Given WaitingToDefrostIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And CompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   And CompressorIsOn();
   And DefrostCompressorOnTimeCounterIsInitialized();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      CompressorOnTimeInSecondsShouldBe(i);

      After(1);
      CompressorOnTimeInSecondsShouldBe(i + 1);
   }
}

TEST(DefrostCompressorOnTimeCounter, ShouldNotResetCompressorOnTimeInSecondsWhenFreezerFilteredTemperatureIsNotTooWarmOnPowerUpAndWhenWaitingToDefrostIsTrue)
{
   Given WaitingToDefrostIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And CompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   And DefrostCompressorOnTimeCounterIsInitialized();

   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
}

TEST(DefrostCompressorOnTimeCounter, ShouldThenStartCountingCompressorOnTimeWhenFreezerFilteredTemperatureIsNotTooWarmOnPowerUpAndWhenWaitingToDefrostIsTrue)
{
   Given WaitingToDefrostIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And CompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   And CompressorIsOn();
   And DefrostCompressorOnTimeCounterIsInitialized();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i);

      After(1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i + 1);
   }
}

TEST(DefrostCompressorOnTimeCounter, ShouldNotStartCountingCompressorOnTimeWhenFreezerFilteredTemperatureTooWarmOnPowerUpAndWhenWaitingToDefrostIsFalse)
{
   Given WaitingToDefrostIs(false);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And CompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   And CompressorIsOn();
   And DefrostCompressorOnTimeCounterIsInitialized();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      CompressorOnTimeInSecondsShouldBe(0);

      After(1);
      CompressorOnTimeInSecondsShouldBe(0);
   }
}

TEST(DefrostCompressorOnTimeCounter, ShouldTransitionFromStopToRunWhenWaitingToDefrost)
{
   Given DefrostCompressorOnTimeCounterIsInStopState();

   When WaitingToDefrostIs(true);
   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);
}

TEST(DefrostCompressorOnTimeCounter, ShouldTransitionFromPauseToRunWhenWaitingToDefrost)
{
   Given DefrostCompressorOnTimeCounterIsInPauseState();

   When WaitingToDefrostIs(true);
   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);
}

TEST(DefrostCompressorOnTimeCounter, ShouldTransitionFromRunToStopWhenNoLongerWaitingToDefrost)
{
   Given DefrostCompressorOnTimeCounterIsInRunState();

   When WaitingToDefrostIs(false);
   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Stop);
}

TEST(DefrostCompressorOnTimeCounter, ShouldIncrementCompressorOnTimeInSecondsWhileInRunStateWhenCompressorIsOn)
{
   Given CompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   And CompressorIsOn();
   And DefrostCompressorOnTimeCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i);

      After(1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i + 1);
   }
}

TEST(DefrostCompressorOnTimeCounter, ShouldStopIncrementingCompressorOnTimeInSecondsWhileInRunStateWhenCompressorTurnsOff)
{
   Given CompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   And CompressorIsOn();
   And DefrostCompressorOnTimeCounterIsInRunState();

   CompressorOnTimeShouldBeIncrementedForThisManySeconds(10);

   When CompressorIsOff();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + 10);

      After(1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + 10);
   }
}
