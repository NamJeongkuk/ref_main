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
#include "PersonalityParametricData_TestDouble.h"
#include "DefrostData_TestDouble.h"
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

enum
{
   SixGridLines = 6,
   TwoDimensional = 2,
};

static const DeltaGridLineData_t freshFoodGridLineData[] = {
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = 0b0010,
   },
   {
      .gridLinesDegFx100 = -450,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 150,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 450,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 950,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 1150,
      .bitMapping = 0b1000,
   },
};

static const DeltaGridLineData_t freezerGridLineData[] = {
   {
      .gridLinesDegFx100 = -250,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 250,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 600,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 750,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 5500,
      .bitMapping = 0b0010,
   },
};

static const DeltaAxisGridLines_t freshFoodAxis = {
   .numberOfLines = SixGridLines,
   .gridLineData = freshFoodGridLineData
};

static const DeltaAxisGridLines_t freezerAxis = {
   .numberOfLines = SixGridLines,
   .gridLineData = freezerGridLineData
};

static DeltaAxisGridLines_t parametricGrid[] = { freshFoodAxis, freezerAxis };
static DeltaGridLines_t deltaGrid = {
   .dimensions = TwoDimensional,
   .gridLines = parametricGrid
};

static const GridData_t gridData = {
   .gridId = 0,
   .deltaGridLines = &deltaGrid,
   .gridPeriodicRunRateInMSec = 1 * MSEC_PER_SEC
};

static TemperatureDegFx100_t freshFoodCalcAxisGridLines[] = { 0, -450, 150, 450, 950, 1150 };
static TemperatureDegFx100_t freezerCalcAxisGridLines[] = { -250, 0, 250, 600, 750, 5500 };

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = freshFoodCalcAxisGridLines
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = freezerCalcAxisGridLines
};

static CalculatedAxisGridLines_t calcGrid[] = { freshFoodCalcAxis, freezerCalcAxis };

static CalculatedGridLines_t calcGridLines = {
   .dimensions = TwoDimensional,
   .gridLines = calcGrid
};

static const DoorAccelerationCounterConfiguration_t config = {
   .activelyWaitingForNextDefrostErd = Erd_ActivelyWaitingForNextDefrost,
   .freezerFilteredTemperatureResolvedErd = Erd_Freezer_FilteredTemperatureResolved,
   .doorAccelerationCounterFsmStateErd = Erd_DoorAccelerationCounterFsmState,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .freshFoodDoorAccelerationCountErd = Erd_DefrostFreshFoodDoorAccelerationCount,
   .freezerDoorAccelerationCountErd = Erd_DefrostFreezerDoorAccelerationCount,
   .convertibleCompartmentDoorAccelerationCountErd = Erd_DefrostConvertibleCompartmentDoorAccelerationCount,
   .leftHandFreshFoodDoorIsOpenErd = Erd_LeftHandFreshFoodDoorIsOpen,
   .rightHandFreshFoodDoorIsOpenErd = Erd_RightHandFreshFoodDoorIsOpen,
   .doorInDoorIsOpenErd = Erd_DoorInDoorIsOpen,
   .freezerDoorIsOpenErd = Erd_FreezerDoorIsOpen,
   .convertibleCompartmentDoorIsOpenErd = Erd_ConvertibleCompartmentDoorIsOpen,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(DoorAccelerationCounter)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   DoorAccelerationCounter_t instance;
   DefrostData_t defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      DefrostData_TestDouble_Init(&defrostData);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      PersonalityParametricData_TestDouble_SetGrid(&personalityParametricData, &gridData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void DoorAccelerationCounterIsInitialized()
   {
      DoorAccelerationCounter_Init(&instance, dataModel, &config);
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolved, &temperature);
   }

   void DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_t expectedState)
   {
      DoorAccelerationCounterFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DoorAccelerationCounterFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FreshFoodDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreshFoodDoorAccelerationCount, &count);
   }

   void FreezerDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreezerDoorAccelerationCount, &count);
   }

   void ConvertibleCompartmentDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostConvertibleCompartmentDoorAccelerationCount, &count);
   }

   void FreshFoodDoorAccelerationShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostFreshFoodDoorAccelerationCount, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void FreezerDoorAccelerationShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostFreezerDoorAccelerationCount, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void ConvertibleCompartmentDoorAccelerationShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostConvertibleCompartmentDoorAccelerationCount, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void DoorAccelerationCounterIsInStopState()
   {
      Given FilteredFreezerCabinetTemperatureIs(defrostData.freezerDefrostTerminationTemperatureInDegFx100);
      And CalculatedGridLinesAre(calcGridLines);
      And DoorAccelerationCounterIsInitialized();

      DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Stop);
   }

   void DoorAccelerationCounterIsInPauseState()
   {
      Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxisGridLines[GridLine_FreezerExtremeHigh]);
      And CalculatedGridLinesAre(calcGridLines);
      And DoorAccelerationCounterIsInitialized();

      DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Pause);
   }

   void ActivelyWaitingForNextDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ActivelyWaitingForNextDefrost, &state);
   }

   void DoorAccelerationCounterIsInRunState()
   {
      Given DoorAccelerationCounterIsInPauseState();

      When ActivelyWaitingForNextDefrostIs(true);
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

   void ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &state);
   }

   void FreshFoodDoorAccelerationShouldBeIncrementedForThisManySeconds(uint8_t seconds)
   {
      for(uint8_t i = 0; i < 10; i++)
      {
         After(PeriodicTimerTicksInMs - 1);
         FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

         After(1);
         FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
      }
   }

   void FreezerDoorAccelerationShouldBeIncrementedForThisManySeconds(uint8_t seconds)
   {
      for(uint8_t i = 0; i < 10; i++)
      {
         After(PeriodicTimerTicksInMs - 1);
         FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData.freezerDoorIncrementFactorInSecondsPerSecond * i);

         After(1);
         FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData.freezerDoorIncrementFactorInSecondsPerSecond * (i + 1));
      }
   }

   void ConvertibleCompartmentDoorAccelerationShouldBeIncrementedForThisManySeconds(uint8_t seconds)
   {
      for(uint8_t i = 0; i < 10; i++)
      {
         After(PeriodicTimerTicksInMs - 1);
         ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

         After(1);
         ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
      }
   }
};

TEST(DoorAccelerationCounter, ShouldInitializeIntoStopStateIfFreezerFilteredTemperatureIsAboveGridFreezerExtremeHysteresis)
{
   Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxisGridLines[GridLine_FreezerExtremeHigh] + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And DoorAccelerationCounterIsInitialized();

   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Stop);
}

TEST(DoorAccelerationCounter, ShouldInitializeIntoPauseStateIfFreezerFilteredTemperatureIsEqualToGridFreezerExtremeHysteresis)
{
   Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxisGridLines[GridLine_FreezerExtremeHigh]);
   And CalculatedGridLinesAre(calcGridLines);
   And DoorAccelerationCounterIsInitialized();

   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Pause);
}

TEST(DoorAccelerationCounter, ShouldInitializeIntoStopStateIfFreezerFilteredTemperatureIsAboveFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData.freezerDefrostTerminationTemperatureInDegFx100 + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And DoorAccelerationCounterIsInitialized();

   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Stop);
}

TEST(DoorAccelerationCounter, ShouldInitializeIntoStopStateIfFreezerFilteredTemperatureIsEqualToFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData.freezerDefrostTerminationTemperatureInDegFx100);
   And CalculatedGridLinesAre(calcGridLines);
   And DoorAccelerationCounterIsInitialized();

   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Stop);
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
   And FreezerDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And DoorAccelerationCounterIsInPauseState();

   FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration);
   And FreezerDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration);
   And ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration);
}

TEST(DoorAccelerationCounter, ShouldTransitionToRunFromStopWhenActivelyWaitingForNextDefrostBecomesTrue)
{
   Given DoorAccelerationCounterIsInStopState();

   When ActivelyWaitingForNextDefrostIs(true);
   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);
}

TEST(DoorAccelerationCounter, ShouldTransitionToRunFromPauseWhenActivelyWaitingForNextDefrostBecomesTrue)
{
   Given DoorAccelerationCounterIsInPauseState();

   When ActivelyWaitingForNextDefrostIs(true);
   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Run);
}

TEST(DoorAccelerationCounter, ShouldTransitionToStopFromRunWhenActivelyWaitingForNextDefrostBecomesFalse)
{
   Given DoorAccelerationCounterIsInRunState();

   When ActivelyWaitingForNextDefrostIs(false);
   DoorAccelerationCounterFsmStateShouldBe(DoorAccelerationCounterFsmState_Stop);
}

TEST(DoorAccelerationCounter, ShouldIncrementFreshFoodDoorAccelerationCountWhileInRunStateWhenLeftHandFreshFoodDoorIsOpen)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And LeftHandFreshFoodDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementFreshFoodDoorAccelerationCountWhileInRunStateWhenRightHandFreshFoodDoorIsOpen)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And RightHandFreshFoodDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementFreshFoodDoorAccelerationCountWhileInRunStateWhenDoorInDoorIsOpen)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);
   And DoorInDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementFreshFoodDoorAccelerationCountWhileInRunStateWhenFreezerDoorIsOpen)
{
   Given FreezerDoorAccelerationIs(SomeFreezerDoorAcceleration);
   And FreezerDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData.freezerDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData.freezerDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementConvertibleCompartmentAccelerationCountWithFreshFoodIncrementFactorWhileInRunStateWhenConvertibleCompartmentDoorIsOpenWhenConvertibleCompartmentStateIsActingAsFreshFood)
{
   Given ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentState_FreshFood);
   And ConvertibleCompartmentDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldIncrementConvertibleCompartmentAccelerationCountWithFreezerIncrementFactorWhileInRunStateWhenConvertibleCompartmentDoorIsOpenWhenConvertibleCompartmentStateIsActingAsFreezer)
{
   Given ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentState_Freezer);
   And ConvertibleCompartmentDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData.freezerDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData.freezerDoorIncrementFactorInSecondsPerSecond * (i + 1));
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
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentState_FreshFood);

   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData.freezerDoorIncrementFactorInSecondsPerSecond * i);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData.freezerDoorIncrementFactorInSecondsPerSecond * (i + 1));
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
   }
}

TEST(DoorAccelerationCounter, ShouldOnlyIncrementFreshFoodDoorAccelerationCountByOneTimesTheFactorPerSecondEvenIfMultipleFreshFoodDoorsAreOpenWhileInRunState)
{
   Given FreshFoodDoorAccelerationIs(SomeFreshFoodDoorAcceleration);

   And LeftHandFreshFoodDoorIs(Open);
   And RightHandFreshFoodDoorIs(Open);
   And DoorInDoorIs(Open);

   And DoorAccelerationCounterIsInRunState();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * i);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * (i + 1));
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
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * 10);

      After(1);
      FreshFoodDoorAccelerationShouldBe(SomeFreshFoodDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * 10);
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
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData.freezerDoorIncrementFactorInSecondsPerSecond * 10);

      After(1);
      FreezerDoorAccelerationShouldBe(SomeFreezerDoorAcceleration + defrostData.freezerDoorIncrementFactorInSecondsPerSecond * 10);
   }
}

TEST(DoorAccelerationCounter, ShouldStopIncrementingWhenConvertibleCompartmentDoorClosesWhileInRunState)
{
   Given ConvertibleCompartmentDoorAccelerationIs(SomeConvertibleCompartmentDoorAcceleration);
   And ConvertibleCompartmentStateIs(ConvertibleCompartmentState_FreshFood);
   And ConvertibleCompartmentDoorIs(Open);
   And DoorAccelerationCounterIsInRunState();

   ConvertibleCompartmentDoorAccelerationShouldBeIncrementedForThisManySeconds(10);

   When ConvertibleCompartmentDoorIs(Closed);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(PeriodicTimerTicksInMs - 1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * 10);

      After(1);
      ConvertibleCompartmentDoorAccelerationShouldBe(SomeConvertibleCompartmentDoorAcceleration + defrostData.freshFoodDoorIncrementFactorInSecondsPerSecond * 10);
   }
}
