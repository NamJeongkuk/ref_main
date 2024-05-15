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
   .ambientWindowAveragedTemperatureInDegFx100Erd = Erd_Ambient_WindowAveragedTemperatureInDegFx100,
};

TEST_GROUP(CrossAmbientHysteresisAdjustmentCalculator)
{
   CrossAmbientHysteresisAdjustmentCalculator_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   TemperatureDegFx100_t freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;
   TemperatureDegFx100_t modifiedFreshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;
   uint16_t freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF;
   const DeltaGridLines_t *deltaGridLines;
   PersonalityParametricData_t *parametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble, TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100 = PersonalityParametricData_Get(dataModel)->freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;
      freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF = PersonalityParametricData_Get(dataModel)->freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF;
      deltaGridLines = PersonalityParametricData_Get(dataModel)->freshFoodAndFreezerDeltaGridLines;
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
         freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100,
         freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF,
         deltaGridLines,
         &config);
   }

   void GivenInitializationWithHighFreshFoodMinimumCrossAmbientAdjustedHysteresis()
   {
      modifiedFreshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100 = 500;

      CrossAmbientHysteresisAdjustmentCalculator_Init(
         &instance,
         dataModel,
         modifiedFreshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100,
         freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF,
         deltaGridLines,
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
         abs(deltaGridLines->gridLines[GridDelta_FreshFood].gridLineData[GridLine_FreshFoodLowHyst].gridLinesDegFx100),
         abs(deltaGridLines->gridLines[GridDelta_FreshFood].gridLineData[GridLine_FreshFoodHighHyst].gridLinesDegFx100));
   }

   TemperatureDegFx100_t LowestHysteresisMinusAmbientAdjustmentHysteresis()
   {
      return LowestHysteresis() - freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;
   }

   TemperatureDegFx100_t UncappedHysteresisAdjustment(TemperatureDegFx100_t crossAmbientWindowedTemperature)
   {
      return (freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF * (AdjustmentThresholdInDegFx100 - crossAmbientWindowedTemperature)) / DegFx1000;
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

TEST(CrossAmbientHysteresisAdjustmentCalculator, ShouldCalculateCrossAmbientHysteresisAdjustmentToZeroIfCrossAmbientWindowedTemperaureIsEqualToTheAdjustmentThreshold)
{
   GivenCrossAmbientWindowAveragedTemperatureIs(AdjustmentThresholdInDegFx100);
   GivenInitialization();

   CrossAmbientHysteresisAdjustmentShouldBe(0);
}

TEST(CrossAmbientHysteresisAdjustmentCalculator, ShouldCalculateCrossAmbientHysteresisAdjustmentToZeroIfCrossAmbientWindowedTemperaureIsGreaterThanTheAdjustmentThreshold)
{
   GivenCrossAmbientWindowAveragedTemperatureIs(AdjustmentThresholdInDegFx100 + 1);
   GivenInitialization();

   CrossAmbientHysteresisAdjustmentShouldBe(0);
}
