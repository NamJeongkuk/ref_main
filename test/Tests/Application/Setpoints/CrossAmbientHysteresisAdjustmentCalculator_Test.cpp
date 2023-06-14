/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CrossAmbientHysteresisAdjustmentCalculator.h"
#include "PersonalityParametricData.h"
#include "TddPersonality.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricDataTestDouble.h"
#include "uassert_test.h"

enum
{
   SomeTemperatureInDegFx100 = 1000,
   SomeVeryHighTemperatureInDegFx100 = 8800,
   SomeVeryLowTemperatureInDegFx100 = 5500,
   AdjustmentThresholdInDegFx100 = 9000,
   DegFx1000 = 1000,
};

static const CrossAmbientHysteresisAdjustmentCalculatorConfig_t config = {
   .crossAmbientHysteresisAdjustmentErd = Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
   .crossAmbientWindowAveragedTemperatureErd = Erd_Ambient_WindowAveragedTemperatureInDegFx100,
};

TEST_GROUP(CrossAmbientHysteresisAdjustmentCalculator)
{
   CrossAmbientHysteresisAdjustmentCalculator_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const GridData_t *gridData;
   GridData_t modifiedGridData;
   PersonalityParametricData_t *parametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      gridData = PersonalityParametricData_Get(dataModel)->gridData;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenInitialization()
   {
      CrossAmbientHysteresisAdjustmentCalculator_Init(
         &instance,
         dataModel,
         gridData,
         &config);
   }

   void GivenInitializationWithHighFreshFoodMinimumCrossAmbientAdjustedHysteresis()
   {
      memcpy(&modifiedGridData, gridData, sizeof(GridData_t));
      modifiedGridData.freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100 = 500;

      CrossAmbientHysteresisAdjustmentCalculator_Init(
         &instance,
         dataModel,
         &modifiedGridData,
         &config);
   }

   void CrossAmbientHysteresisAdjustmentShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_CrossAmbientHysteresisAdjustmentInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenCrossAmbientWindowAveragedTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Ambient_WindowAveragedTemperatureInDegFx100, &temperature);
   }

   void GivenCrossAmbientHysteresisAdjustmentIs(TemperatureDegFx100_t adjustment)
   {
      DataModel_Write(dataModel, Erd_CrossAmbientHysteresisAdjustmentInDegFx100, &adjustment);
   }

   void WhenCrossAmbientWindowAveragedTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      GivenCrossAmbientWindowAveragedTemperatureIs(temperature);
   }

   TemperatureDegFx100_t LowestHysteresis()
   {
      return MIN(
         abs(gridData->deltaGridLines->gridLines[GridDelta_FreshFood].gridLineData[GridLine_FreshFoodLowHyst].gridLinesDegFx100),
         abs(gridData->deltaGridLines->gridLines[GridDelta_FreshFood].gridLineData[GridLine_FreshFoodHighHyst].gridLinesDegFx100));
   }

   TemperatureDegFx100_t LowestHysteresisMinusAmbientAdjustmentHysteresis()
   {
      return LowestHysteresis() - gridData->freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;
   }

   TemperatureDegFx100_t UncappedHysteresisAdjustment(TemperatureDegFx100_t crossAmbientWindowedTemperature)
   {
      return (gridData->freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF * (AdjustmentThresholdInDegFx100 - crossAmbientWindowedTemperature)) / DegFx1000;
   }
};

TEST(CrossAmbientHysteresisAdjustmentCalculator, ShouldCalculateCrossAmbientHysteresisAdjustmentOnInitialization)
{
   GivenCrossAmbientWindowAveragedTemperatureIs(SomeTemperatureInDegFx100);
   GivenInitialization();
   CrossAmbientHysteresisAdjustmentShouldBe(LowestHysteresisMinusAmbientAdjustmentHysteresis());
}

TEST(CrossAmbientHysteresisAdjustmentCalculator, ShouldCalculateCrossAmbientHysteresisAdjustmentAsTheLowestHysteresisMinusTheAdjustment)
{
   GivenCrossAmbientWindowAveragedTemperatureIs(SomeTemperatureInDegFx100);
   GivenInitialization();
   CrossAmbientHysteresisAdjustmentShouldBe(LowestHysteresisMinusAmbientAdjustmentHysteresis());

   WhenCrossAmbientWindowAveragedTemperatureChangesTo(SomeVeryLowTemperatureInDegFx100);
   CrossAmbientHysteresisAdjustmentShouldBe(LowestHysteresisMinusAmbientAdjustmentHysteresis());
}

TEST(CrossAmbientHysteresisAdjustmentCalculator, ShouldCalculateCrossAmbientHysteresisAdjustmentAsTheUncappedHysteresisAdjustment)
{
   GivenCrossAmbientWindowAveragedTemperatureIs(SomeTemperatureInDegFx100);
   GivenInitialization();
   CrossAmbientHysteresisAdjustmentShouldBe(LowestHysteresisMinusAmbientAdjustmentHysteresis());

   WhenCrossAmbientWindowAveragedTemperatureChangesTo(SomeVeryHighTemperatureInDegFx100);
   CrossAmbientHysteresisAdjustmentShouldBe(UncappedHysteresisAdjustment(SomeVeryHighTemperatureInDegFx100));

   WhenCrossAmbientWindowAveragedTemperatureChangesTo(SomeVeryLowTemperatureInDegFx100);
   CrossAmbientHysteresisAdjustmentShouldBe(LowestHysteresisMinusAmbientAdjustmentHysteresis());
}

TEST(CrossAmbientHysteresisAdjustmentCalculator, ShouldCalculateCrossAmbientHysteresisAdjustmentToZeroIfFreshFoodMinimumCrossAmbientAdjustedHysteresisIsHigherThanTheDeltaHysteresis)
{
   GivenCrossAmbientWindowAveragedTemperatureIs(SomeTemperatureInDegFx100);
   GivenInitializationWithHighFreshFoodMinimumCrossAmbientAdjustedHysteresis();
   CrossAmbientHysteresisAdjustmentShouldBe(0);

   WhenCrossAmbientWindowAveragedTemperatureChangesTo(SomeVeryHighTemperatureInDegFx100);
   CrossAmbientHysteresisAdjustmentShouldBe(0);

   WhenCrossAmbientWindowAveragedTemperatureChangesTo(SomeVeryLowTemperatureInDegFx100);
   CrossAmbientHysteresisAdjustmentShouldBe(0);
}

TEST(CrossAmbientHysteresisAdjustmentCalculator, ShouldCalculateCrossAmbientHysteresisAdjustmentToZeroIfCrossAmbientHysteresisAdjustmentIsHigherThanTheAdjustmentThreshold)
{
   GivenCrossAmbientHysteresisAdjustmentIs(AdjustmentThresholdInDegFx100 + 1);
   GivenCrossAmbientWindowAveragedTemperatureIs(SomeTemperatureInDegFx100);
   GivenInitialization();

   CrossAmbientHysteresisAdjustmentShouldBe(0);
}
