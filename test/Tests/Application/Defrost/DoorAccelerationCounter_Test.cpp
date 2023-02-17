/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DoorAccelerationCounter.h"
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
   Closed = 0,
   Open = 1
};

enum
{
   SomeFreshFoodDoorAcceleration = 100,
   SomeFreezerDoorAcceleration = 200,
   SomeConvertibleCompartmentDoorAcceleration = 300
};

enum
{
   PeriodicTimerTicksInMs = 1 * MSEC_PER_SEC
};

static const DoorAccelerationCounterConfiguration_t config = {
   .waitingToDefrostErd = Erd_WaitingToDefrost,
   .doorAccelerationCounterFsmStateErd = Erd_DoorAccelerationCounterFsmState,
   .freshFoodScaledDoorAccelerationInSecondsErd = Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds,
   .freezerScaledDoorAccelerationInSecondsErd = Erd_DefrostFreezerScaledDoorAccelerationInSeconds,
   .convertibleCompartmentScaledDoorAccelerationInSecondsErd = Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds,
   .leftHandFreshFoodDoorIsOpenErd = Erd_LeftHandFreshFoodDoorIsOpen,
   .rightHandFreshFoodDoorIsOpenErd = Erd_RightHandFreshFoodDoorIsOpen,
   .doorInDoorIsOpenErd = Erd_DoorInDoorIsOpen,
   .freezerDoorIsOpenErd = Erd_FreezerDoorIsOpen,
   .convertibleCompartmentDoorIsOpenErd = Erd_ConvertibleCompartmentDoorIsOpen,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .resetDefrostCountsErd = Erd_ResetDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestSignal,
   .doorAccelerationCounterReadyErd = Erd_DoorAccelerationCounterReady,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(DoorAccelerationCounter)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   DoorAccelerationCounter_t instance;
   const DefrostData_t *defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void DoorAccelerationCounterIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);

      DoorAccelerationCounter_Init(&instance, dataModel, &config);
   }

   void DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_t expectedState)
   {
      DoorAccelerationCounterFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DoorAccelerationCounterFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FreshFoodDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds, &count);
   }

   void FreezerDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreezerScaledDoorAccelerationInSeconds, &count);
   }

   void ConvertibleCompartmentDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds, &count);
   }

   void FreshFoodDoorAccelerationShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void FreezerDoorAccelerationShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostFreezerScaledDoorAccelerationInSeconds, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void ConvertibleCompartmentDoorAccelerationShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void DoorAccelerationCounterIsInStopState()
   {
      Given WaitingToDefrostIs(true);
      And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
      And DoorAccelerationCounterIsInitialized();

      DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);

      When WaitingToDefrostIs(false);
      DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Stop);
   }

   void DoorAccelerationCounterIsInPauseState()
   {
      Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
      And DoorAccelerationCounterIsInitialized();

      DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Pause);
   }

   void WaitingToDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, &state);
   }

   void DoorAccelerationCounterIsInRunState()
   {
      Given DoorAccelerationCounterIsInPauseState();

      When WaitingToDefrostIs(true);
      DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);
   }

   void LeftHandFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftHandFreshFoodDoorIsOpen, &state);
   }

   void RightHandFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightHandFreshFoodDoorIsOpen, &state);
   }

   void DoorInDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DoorInDoorIsOpen, &state);
   }

   void FreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerDoorIsOpen, &state);
   }

   void ConvertibleCompartmentDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDoorIsOpen, &state);
   }

   void ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &state);
   }

   void FreshFoodDoorAccelerationShouldBeIncrementedForThisManySeconds(uint8_t seconds)
   {
      for(uint8_t i = 0; i < seconds; i++)
      {
         After(PeriodicTimerTicksInMs - 1);
         FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

         After(1);
         FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
      }
   }

   void FreezerDoorAccelerationShouldBeIncrementedForThisManySeconds(uint8_t seconds)
   {
      for(uint8_t i = 0; i < seconds; i++)
      {
         After(PeriodicTimerTicksInMs - 1);
         FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * i);

         After(1);
         FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * (i + 1));
      }
   }

   void ConvertibleCompartmentDoorAccelerationShouldBeIncrementedForThisManySeconds(uint8_t seconds)
   {
      for(uint8_t i = 0; i < seconds; i++)
      {
         After(PeriodicTimerTicksInMs - 1);
         ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

         After(1);
         ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
      }
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }

   void ResetRequestReceived()
   {
      Signal_SendViaErd(
         DataModel_AsDataSource(dataModel),
         Erd_ResetDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestSignal);
   }
};

TEST(DoorAccelerationCounter, ShouldTransitionFromRunStateToStopStateWhenNoLongerWaitingToDefrost)
{
   Given WaitingToDefrostIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DoorAccelerationCounterIsInitialized();

   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);

   When WaitingToDefrostIs(false);
   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Stop);
}

TEST(DoorAccelerationCounter, ShouldInitializeIntoPauseStateWhenFreezerFilteredTemperatureWasNotTooWarmOnPowerUp)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DoorAccelerationCounterIsInitialized();

   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Pause);
}

TEST(DoorAccelerationCounter, ShouldResetDoorAccelerationsToZeroWhenEnteringStopState)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And DoorAccelerationCounterIsInStopState();

   FreshFoodDoorAccelerationShouldBe(0);
   And FreezerDoorAccelerationShouldBe(0);
   And ConvertibleCompartmentDoorAccelerationShouldBe(0);
}

TEST(DoorAccelerationCounter, ShouldNotResetDoorAccelerationsToZeroWhenEnteringPauseState)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And DoorAccelerationCounterIsInPauseState();

   FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration);
}

TEST(DoorAccelerationCounter, ShouldResetDoorAccelerationsToZeroWhenFreezerFilteredTemperatureTooWarmOnPowerUpAndWhenWaitingToDefrostIsTrue)
{
   Given WaitingToDefrostIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And DoorAccelerationCounterIsInitialized();

   FreshFoodDoorAccelerationShouldBe(0);
   And FreezerDoorAccelerationShouldBe(0);
   And ConvertibleCompartmentDoorAccelerationShouldBe(0);
}

TEST(DoorAccelerationCounter, ShouldThenStartCountingDoorAccelerationWhenFreezerFilteredTemperatureTooWarmOnPowerUpAndWhenWaitingToDefrostIsTrue)
{
   Given WaitingToDefrostIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And LeftHandFreshFoodDoorIs(Open);
   And FreezerDoorIs(Open);
   And ConvertibleCompartmentDoorIs(Open);
   And ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   And DoorAccelerationCounterIsInitialized();

   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);
      FreezerDoorAccelerationShouldBe(defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * i);
      ConvertibleCompartmentDoorAccelerationShouldBe(defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
      FreezerDoorAccelerationShouldBe(defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * (i + 1));
      ConvertibleCompartmentDoorAccelerationShouldBe(defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldNotResetDoorAccelerationsToZeroWhenFreezerFilteredTemperatureIsNotTooWarmOnPowerUpAndWhenWaitingToDefrostIsTrue)
{
   Given WaitingToDefrostIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And DoorAccelerationCounterIsInitialized();

   FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration);
}

TEST(DoorAccelerationCounter, ShouldThenStartCountingDoorAccelerationWhenFreezerFilteredTemperatureIsNotTooWarmOnPowerUpAndWhenWaitingToDefrostIsTrue)
{
   Given WaitingToDefrostIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And LeftHandFreshFoodDoorIs(Open);
   And FreezerDoorIs(Open);
   And ConvertibleCompartmentDoorIs(Open);
   And ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   And DoorAccelerationCounterIsInitialized();

   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * i);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * (i + 1));
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldTransitionFromStopToRunWhenWaitingToDefrost)
{
   Given DoorAccelerationCounterIsInStopState();

   When WaitingToDefrostIs(true);
   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);
}

TEST(DoorAccelerationCounter, ShouldTransitionFromPauseToRunWhenWaitingToDefrost)
{
   Given DoorAccelerationCounterIsInPauseState();

   When WaitingToDefrostIs(true);
   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);
}

TEST(DoorAccelerationCounter, ShouldTransitionFromRunToStopWhenNoLongerWaitingToDefrost)
{
   Given DoorAccelerationCounterIsInRunState();

   When WaitingToDefrostIs(false);
   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Stop);
}

TEST(DoorAccelerationCounter, ShouldIncrementFreshFoodScaledDoorAccelerationInSecondsWhileInRunStateWhenLeftHandFreshFoodDoorIsOpen)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And LeftHandFreshFoodDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementFreshFoodScaledDoorAccelerationInSecondsWhileInRunStateWhenRightHandFreshFoodDoorIsOpen)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And RightHandFreshFoodDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementFreshFoodScaledDoorAccelerationInSecondsWhileInRunStateWhenDoorInDoorIsOpen)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And DoorInDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementFreshFoodScaledDoorAccelerationInSecondsWhileInRunStateWhenFreezerDoorIsOpen)
{
   Given FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And FreezerDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementConvertibleCompartmentAccelerationCountWithFreshFoodIncrementFactorWhileInRunStateWhenConvertibleCompartmentDoorIsOpenWhenConvertibleCompartmentStateTypeIsActingAsFreshFood)
{
   Given ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   And ConvertibleCompartmentDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementConvertibleCompartmentAccelerationCountWithFreezerIncrementFactorWhileInRunStateWhenConvertibleCompartmentDoorIsOpenWhenConvertibleCompartmentStateTypeIsActingAsFreezer)
{
   Given ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   And ConvertibleCompartmentDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementAllDoorAccelerationsWhileInRunStateWhenAllDoorTypesAreOpen)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);

   And LeftHandFreshFoodDoorIs(Open);
   And FreezerDoorIs(Open);
   And ConvertibleCompartmentDoorIs(Open);
   And ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);

   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * i);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * (i + 1));
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldOnlyIncrementFreshFoodScaledDoorAccelerationInSecondsByOneTimesTheFactorPerSecondEvenIfMultipleFreshFoodDoorsAreOpenWhileInRunState)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);

   And LeftHandFreshFoodDoorIs(Open);
   And RightHandFreshFoodDoorIs(Open);
   And DoorInDoorIs(Open);

   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldStopIncrementingWhenFreshFoodDoorClosesWhileInRunState)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And LeftHandFreshFoodDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   FreshFoodDoorAccelerationShouldBeIncrementedForThisManySeconds(10);

   When LeftHandFreshFoodDoorIs(Closed);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * 10);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * 10);
   }
}

TEST(DoorAccelerationCounter, ShouldStopIncrementingWhenFreezerFoodDoorClosesWhileInRunState)
{
   Given FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And FreezerDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   FreezerDoorAccelerationShouldBeIncrementedForThisManySeconds(10);

   When FreezerDoorIs(Closed);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * 10);

      After(1);
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData->idleData.freezerDoorIncrementFactorInSecondsPerSecond * 10);
   }
}

TEST(DoorAccelerationCounter, ShouldStopIncrementingWhenConvertibleCompartmentDoorClosesWhileInRunState)
{
   Given ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   And ConvertibleCompartmentDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   ConvertibleCompartmentDoorAccelerationShouldBeIncrementedForThisManySeconds(10);

   When ConvertibleCompartmentDoorIs(Closed);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * 10);

      After(1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData->idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * 10);
   }
}

TEST(DoorAccelerationCounter, ShouldResetDoorAccelerationsWhenReceivingResetSignalDuringRun)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And DoorAccelerationCounterIsInRunState();

   When ResetRequestReceived();
   FreshFoodDoorAccelerationShouldBe(0);
   FreezerDoorAccelerationShouldBe(0);
   ConvertibleCompartmentDoorAccelerationShouldBe(0);
   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);
}

TEST(DoorAccelerationCounter, ShouldResetDoorAccelerationsWhenReceivingResetSignalDuringPause)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And DoorAccelerationCounterIsInPauseState();

   When ResetRequestReceived();
   FreshFoodDoorAccelerationShouldBe(0);
   FreezerDoorAccelerationShouldBe(0);
   ConvertibleCompartmentDoorAccelerationShouldBe(0);
   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Pause);
}

TEST(DoorAccelerationCounter, ShouldClearCountsInRunStateOnResetRequest)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And DoorAccelerationCounterIsInRunState();

   When ResetRequestReceived();
   FreshFoodDoorAccelerationShouldBe(0);
   FreezerDoorAccelerationShouldBe(0);
   ConvertibleCompartmentDoorAccelerationShouldBe(0);
   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);
}
