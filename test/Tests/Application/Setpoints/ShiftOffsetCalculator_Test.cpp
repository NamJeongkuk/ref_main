/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ShiftOffsetCalculator.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "Filter_LongTermAverage.h"
#include "PersonalityParametricData.h"
#include "Signal.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And

enum
{
   SomeAdjustedSetpointInDegFx100 = 100,
   SomeShiftOffset = 5,
   Beta = 3,
   Average1Low = 20,
};

static const ShiftOffsetCalculatorConfig_t config = {
   .filteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .shiftOffsetErd = Erd_FreshFood_ThermalShiftInDegFx100,
   .adjustedSetpointWithoutShiftErd = Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100,
   .postDwellCompletionSignalErd = Erd_PostDwellCompletionSignal,
   .resetThermalShiftOffsetSignalErd = Erd_FreshFood_ResetThermalShiftOffsetSignal,
   .longTermAverageErd = Erd_FreshFood_LongTermAverageInDegFx100,
};

static void OnChange(void *context, const void *args)
{
   I_DataModel_t *dataModel = (I_DataModel_t *)context;
   const DataModelOnDataChangeArgs_t *dataChangeArgs = (const DataModelOnDataChangeArgs_t *)args;

   switch(dataChangeArgs->erd)
   {
      case Erd_FreshFood_ThermalShiftInDegFx100:
      case Erd_FreshFood_AdjustedSetpointInDegFx100:
         break;

      default:
         return;
   }

   TemperatureDegFx100_t setpoint;
   DataModel_Read(dataModel, Erd_FreshFood_AdjustedSetpointInDegFx100, &setpoint);

   TemperatureDegFx100_t shift;
   DataModel_Read(dataModel, Erd_FreshFood_ThermalShiftInDegFx100, &shift);

   setpoint -= shift;

   DataModel_Write(dataModel,
      Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100,
      &setpoint);
}

TEST_GROUP(ShiftOffsetCalculator)
{
   I_DataModel_t *dataModel;
   Filter_LongTermAverage_t longTermAverageFilter;
   const ShiftOffsetCalculatorData_t *shiftOffsetCalculatorData;
   const ShiftOffsetData_t *shiftOffsetData;

   ShiftOffsetCalculator_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   EventSubscription_t subscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      shiftOffsetCalculatorData =
         PersonalityParametricData_Get(dataModel)->shiftOffsetCalculatorData;
      shiftOffsetData =
         PersonalityParametricData_Get(dataModel)->freshFoodThermalOffsetData->shiftOffsetData;

      Filter_LongTermAverage_Init(&longTermAverageFilter, Beta);

      EventSubscription_Init(&subscription, dataModel, OnChange);
      DataModel_SubscribeAll(dataModel, &subscription);
   }

   void TheModuleIsInitialized()
   {
      ShiftOffsetCalculator_Init(
         &instance,
         dataModel,
         &longTermAverageFilter.interface,
         &timerModuleTestDouble->timerModule,
         &config,
         shiftOffsetData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void ShiftOffsetIs(TemperatureDegFx100_t value)
   {
      DataModel_Write(dataModel, Erd_FreshFood_ThermalShiftInDegFx100, &value);
   }

   void ShiftOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_ThermalShiftInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void AdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel, Erd_FreshFood_AdjustedSetpointInDegFx100, &setpoint);
   }

   void PostDwellCompletionSignalRequested()
   {
      Signal_SendViaErd(DataModel_AsDataSource(dataModel), Erd_PostDwellCompletionSignal);
   }

   void ResolvedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel, Erd_FreshFood_ResolvedSetpointInDegFx100, &setpoint);
   }

   void LongTermAverageShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_LongTermAverageInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FilteredTemperatureIs(TemperatureDegFx100_t filteredTemperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &filteredTemperature);
   }

   void ResetThermalShiftOffsetRequested()
   {
      Signal_SendViaErd(DataModel_AsDataSource(dataModel), Erd_FreshFood_ResetThermalShiftOffsetSignal);
   }
};

TEST(ShiftOffsetCalculator, ShouldResetShiftOffsetToZeroOnInit)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   Given ShiftOffsetIs(255);

   When TheModuleIsInitialized();
   ShiftOffsetShouldBe(0);
}

TEST(ShiftOffsetCalculator, ShouldSeedAdjustedSetpointWhenOnInit)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);

   When TheModuleIsInitialized();
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100);
}

TEST(ShiftOffsetCalculator, ShouldFeedFilteredTemperatureEveryLongTermAverageUpdateTime)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   Given TheModuleIsInitialized();
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100);

   When FilteredTemperatureIs(SomeAdjustedSetpointInDegFx100 + 100);
   After(shiftOffsetCalculatorData->longTermAverageUpdateTimeInMinutes * MSEC_PER_MIN - 1);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100);

   After(1);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100 + 3);

   When FilteredTemperatureIs(SomeAdjustedSetpointInDegFx100 + 200);
   After(shiftOffsetCalculatorData->longTermAverageUpdateTimeInMinutes * MSEC_PER_MIN);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100 + 14);
}

TEST(ShiftOffsetCalculator, ShouldNotCalculateShiftOffsetWhenLongTermAverageIsEqualToAdjustedSetpointMinusLowerAdjustmentLimit)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   Given FilteredTemperatureIs(SomeAdjustedSetpointInDegFx100);
   Given TheModuleIsInitialized();

   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN - 1);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100);
   And ShiftOffsetShouldBe(0);

   When AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100 + shiftOffsetData->lowerAdjustmentLimitInDegFx100);
   After(1);
   ShiftOffsetShouldBe(0);
}

TEST(ShiftOffsetCalculator, ShouldNotCalculateShiftOffsetWhenLongTermAverageIsLessThanAdjustedSetpointMinusLowerAdjustmentLimit)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100 - shiftOffsetData->lowerAdjustmentLimitInDegFx100 - 1);
   Given FilteredTemperatureIs(SomeAdjustedSetpointInDegFx100 - shiftOffsetData->lowerAdjustmentLimitInDegFx100 - 1);
   Given TheModuleIsInitialized();

   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN - 1);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100 - shiftOffsetData->lowerAdjustmentLimitInDegFx100 - 1);
   And ShiftOffsetShouldBe(0);

   When AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   After(1);
   ShiftOffsetShouldBe(0);
}

TEST(ShiftOffsetCalculator, ShouldNotCalculateShiftOffsetWhenLongTermAverageIsGreaterThanAdjustedSetPointPlusUpperAdjustmentLimit)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100 + shiftOffsetData->upperAdjustmentLimitInDegFx100 + 1);
   Given FilteredTemperatureIs(SomeAdjustedSetpointInDegFx100 + shiftOffsetData->upperAdjustmentLimitInDegFx100 + 1);
   Given TheModuleIsInitialized();

   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN - 1);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100 + shiftOffsetData->upperAdjustmentLimitInDegFx100 + 1);
   And ShiftOffsetShouldBe(0);

   When AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   After(1);
   ShiftOffsetShouldBe(0);
}

TEST(ShiftOffsetCalculator, ShouldNotCalculateShiftOffsetWhenLongTermAverageIsEqualToAdjustedSetpointPlusUpperAdjustmentLimit)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100 + shiftOffsetData->upperAdjustmentLimitInDegFx100);
   Given FilteredTemperatureIs(SomeAdjustedSetpointInDegFx100 + shiftOffsetData->upperAdjustmentLimitInDegFx100);
   Given TheModuleIsInitialized();

   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN - 1);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100 + shiftOffsetData->upperAdjustmentLimitInDegFx100);
   And ShiftOffsetShouldBe(0);

   When AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   After(1);
   ShiftOffsetShouldBe(0);
}

TEST(ShiftOffsetCalculator, ShouldUpdateShiftOffsetToShiftOffsetMinusTenWhenLongTermAverageMinusAdjustedSetpointIsGreaterThanHundred)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100 + 101);
   Given FilteredTemperatureIs(SomeAdjustedSetpointInDegFx100 + 101);
   Given TheModuleIsInitialized();
   Given ShiftOffsetIs(SomeShiftOffset);

   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN - 1);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100 + 101);
   And ShiftOffsetShouldBe(SomeShiftOffset);

   When AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   After(1);
   ShiftOffsetShouldBe(SomeShiftOffset - 10);
}

TEST(ShiftOffsetCalculator, ShouldUpdateShiftOffsetToShiftOffsetMinusTwoWhenLongTermAverageMinusAdjustedSetpointIsBetweenTwentyAndHundred)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100 + 21);
   Given FilteredTemperatureIs(SomeAdjustedSetpointInDegFx100 + 21);
   Given TheModuleIsInitialized();
   Given ShiftOffsetIs(SomeShiftOffset);

   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN - 1);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100 + 21);
   And ShiftOffsetShouldBe(SomeShiftOffset);

   When AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   After(1);
   ShiftOffsetShouldBe(SomeShiftOffset - 2);
}

TEST(ShiftOffsetCalculator, ShouldUpdateShiftOffsetToShiftOffsetPlusTenWhenLongTermAverageMinusAdjustedSetpointWithoutShiftOffsetIsLessThanMinusHundred)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100 - 110);
   Given FilteredTemperatureIs(SomeAdjustedSetpointInDegFx100 - 110);
   Given TheModuleIsInitialized();
   Given ShiftOffsetIs(SomeShiftOffset);

   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN - 1);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100 - 110);
   And ShiftOffsetShouldBe(SomeShiftOffset);

   When AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   After(1);
   ShiftOffsetShouldBe(SomeShiftOffset + 10);
}

TEST(ShiftOffsetCalculator, ShouldUpdateShiftOffsetToShiftOffsetPlusTwoWhenLongTermAverageMinusAdjustedSetpointWithoutShiftOffsetIsBetweenMinusTwentyAndMinusHundred)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100 - 26);
   Given FilteredTemperatureIs(SomeAdjustedSetpointInDegFx100 - 26);
   Given TheModuleIsInitialized();
   Given ShiftOffsetIs(SomeShiftOffset);

   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN - 1);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100 - 26);
   And ShiftOffsetShouldBe(SomeShiftOffset);

   When AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   After(1);
   ShiftOffsetShouldBe(SomeShiftOffset + 2);
}

TEST(ShiftOffsetCalculator, ShouldUpdateShiftOffsetToMaxShiftOffsetWhenShiftOffsetIsGreaterThanMaxShift)
{
   Given TheModuleIsInitialized();
   Given AdjustedSetpointIs(0);
   Given ShiftOffsetIs(shiftOffsetData->maxShiftInDegFx100 + 11);

   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN - 1);
   ShiftOffsetShouldBe(shiftOffsetData->maxShiftInDegFx100 + 11);

   After(1);
   ShiftOffsetShouldBe(shiftOffsetData->maxShiftInDegFx100);
}

TEST(ShiftOffsetCalculator, ShouldUpdateShiftOffsetToMinShiftOffsetWhenShiftOffsetIsLessThanMin)
{
   Given TheModuleIsInitialized();
   Given ShiftOffsetIs(shiftOffsetData->minShiftInDegFx100 - 11);

   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN - 1);
   And ShiftOffsetShouldBe(shiftOffsetData->minShiftInDegFx100 - 11);

   After(1);
   ShiftOffsetShouldBe(shiftOffsetData->minShiftInDegFx100);
}

TEST(ShiftOffsetCalculator, ShouldSetShiftOffsetToPostDefrostShiftWhenPostDwellCompletion)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   Given TheModuleIsInitialized();
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100);
   And ShiftOffsetShouldBe(0);

   When PostDwellCompletionSignalRequested();
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100);
   And ShiftOffsetShouldBe(shiftOffsetData->postDefrostShiftInDegFx100);
}

TEST(ShiftOffsetCalculator, ShouldSeedAdjustedSetpointMinusShiftOffsetAndResetShiftOffsetToZeroWhenResetThermalShiftOffsetSignalChanges)
{
   Given AdjustedSetpointIs(SomeAdjustedSetpointInDegFx100);
   Given TheModuleIsInitialized();
   Given ShiftOffsetIs(SomeShiftOffset);
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100);

   When ResetThermalShiftOffsetRequested();
   LongTermAverageShouldBe(SomeAdjustedSetpointInDegFx100 - SomeShiftOffset);
   And ShiftOffsetShouldBe(0);
}

TEST(ShiftOffsetCalculator, ShouldOnlyChangeShiftIfAverageMinusAdjustedSetpointWithoutShiftIsInRange)
{
   Given AdjustedSetpointIs(0);
   Given TheModuleIsInitialized();

   ShiftOffsetIs(Average1Low + 1);
   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN);
   ShiftOffsetShouldBe(Average1Low - 1);

   ShiftOffsetIs(Average1Low);
   After(shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN);
   ShiftOffsetShouldBe(Average1Low);
}
