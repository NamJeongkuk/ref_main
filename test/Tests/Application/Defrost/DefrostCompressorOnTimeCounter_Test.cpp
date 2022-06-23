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
#include "PersonalityParametricData_TestDouble.h"
#include "DefrostData_TestDouble.h"
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

static const DefrostCompressorOnTimeCounterConfiguration_t config = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .activelyWaitingForNextDefrostErd = Erd_ActivelyWaitingForNextDefrost,
   .freezerFilteredTemperatureResolvedErd = Erd_Freezer_FilteredTemperatureResolved,
   .defrostCompressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .defrostCompressorOnTimeCounterFsmStateErd = Erd_DefrostCompressorOnTimeCounterFsmState,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(DefrostCompressorOnTimeCounter)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   DefrostCompressorOnTimeCounter_t instance;
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

   void DefrostCompressorOnTimeCounterIsInitialized()
   {
      DefrostCompressorOnTimeCounter_Init(&instance, dataModel, &config);
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolved, &temperature);
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
      Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxisGridLines[GridLine_FreezerExtremeHigh]);
      And CalculatedGridLinesAre(calcGridLines);
      And DefrostCompressorOnTimeCounterIsInitialized();

      DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Pause);
   }

   void DefrostCompressorOnTimeCounterIsInStopState()
   {
      Given FilteredFreezerCabinetTemperatureIs(defrostData.freezerDefrostTerminationTemperatureInDegFx100);
      And CalculatedGridLinesAre(calcGridLines);
      And DefrostCompressorOnTimeCounterIsInitialized();

      DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Stop);
   }

   void ActivelyWaitingForNextDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ActivelyWaitingForNextDefrost, &state);
   }

   void DefrostCompressorOnTimeCounterIsInRunState()
   {
      Given DefrostCompressorOnTimeCounterIsInPauseState();

      When ActivelyWaitingForNextDefrostIs(true);
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
};

TEST(DefrostCompressorOnTimeCounter, ShouldInitializeIntoStopStateIfFreezerFilteredTemperatureIsAboveGridFreezerExtremeHysteresis)
{
   Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxisGridLines[GridLine_FreezerExtremeHigh] + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And DefrostCompressorOnTimeCounterIsInitialized();

   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Stop);
}

TEST(DefrostCompressorOnTimeCounter, ShouldInitializeIntoPauseStateIfFreezerFilteredTemperatureIsEqualToGridFreezerExtremeHysteresis)
{
   Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxisGridLines[GridLine_FreezerExtremeHigh]);
   And CalculatedGridLinesAre(calcGridLines);
   And DefrostCompressorOnTimeCounterIsInitialized();

   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Pause);
}

TEST(DefrostCompressorOnTimeCounter, ShouldInitializeIntoStopStateIfFreezerFilteredTemperatureIsAboveFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData.freezerDefrostTerminationTemperatureInDegFx100 + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And DefrostCompressorOnTimeCounterIsInitialized();

   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Stop);
}

TEST(DefrostCompressorOnTimeCounter, ShouldInitializeIntoStopStateIfFreezerFilteredTemperatureIsEqualToFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData.freezerDefrostTerminationTemperatureInDegFx100);
   And CalculatedGridLinesAre(calcGridLines);
   And DefrostCompressorOnTimeCounterIsInitialized();

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

TEST(DefrostCompressorOnTimeCounter, ShouldTransitionToRunFromStopWhenActivelyWaitingForNextDefrostBecomesTrue)
{
   Given DefrostCompressorOnTimeCounterIsInStopState();

   When ActivelyWaitingForNextDefrostIs(true);
   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);
}

TEST(DefrostCompressorOnTimeCounter, ShouldTransitionToRunFromPauseWhenActivelyWaitingForNextDefrostBecomesTrue)
{
   Given DefrostCompressorOnTimeCounterIsInPauseState();

   When ActivelyWaitingForNextDefrostIs(true);
   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);
}

TEST(DefrostCompressorOnTimeCounter, ShouldTransitionToStopFromRunWhenActivelyWaitingForNextDefrostBecomesFalse)
{
   Given DefrostCompressorOnTimeCounterIsInRunState();

   When ActivelyWaitingForNextDefrostIs(false);
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
