/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "EnhancedSabbathAverageTemperature.h"
#include "InputOutput_Simple.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "Filter_TestDouble.h"
#include "uassert_test.h"

enum
{
   TwoMinutesInMin = 2,
   TwoMinutesInMsec = TwoMinutesInMin * MSEC_PER_MIN,
   ThreeMinutesInMsec = 3 * MSEC_PER_MIN,
   OneMinuteInMsec = 1 * MSEC_PER_MIN,
   SomeFilteredTemperature = 1000,
   SomeOtherFilteredTemperature = 1100,
   SomeUnfilteredTemperature = 1001,
   TimeBetweenReadsInMsec = 100,
};

static const EnhancedSabbathAverageTemperatureConfig_t config = {
   .filteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureInDegFx100,
   .unfilteredTemperatureInDegFx100Erd = Erd_FreshFood_UnfilteredTemperatureInDegFx100,
   .overrideRequestErd = Erd_FreshFood_AveragedTemperatureOverrideRequest,
   .overrideValueErd = Erd_FreshFood_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100,
   .enhancedSabbathModeStatusErd = Erd_EnhancedSabbathModeEnable,
   .timerModuleErd = Erd_TimerModule,
};

TEST_GROUP(EnhancedSabbathAverageTemperature)
{
   EnhancedSabbathAverageTemperature_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   Filter_TestDouble_t filter;
   int16_t seedBuffer;
   int16_t feedBuffer;
   int16_t readBuffer;

   InputOutput_Simple_t inputOutputSimple;
   uint32_t randomNumber;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      InputOutput_Simple_Init(&inputOutputSimple, &randomNumber, sizeof(randomNumber));

      Filter_TestDouble_Init(&filter, &seedBuffer, &feedBuffer, &readBuffer, sizeof(seedBuffer));

      TemperatureDegFx100_t initTemp = 0;
      Filter_Seed(&filter.interface, &initTemp);
   }

   void GivenInitialization()
   {
      EnhancedSabbathAverageTemperature_Init(
         &instance,
         dataModel,
         &filter.interface,
         InputOutput_AsInput(&inputOutputSimple.interface),
         TwoMinutesInMin,
         &config);
   }

   void GivenTheRandomAdditionalTimeHasBeenGeneratedAs(uint32_t timeInMsec)
   {
      InputOutput_Write(&inputOutputSimple.interface, &timeInMsec);
   }

   void WhenTheRandomAdditionalTimeHasBeenGeneratedAs(uint32_t timeInMsec)
   {
      GivenTheRandomAdditionalTimeHasBeenGeneratedAs(timeInMsec);
   }

   void TheRandomTimeShouldBe(uint32_t expected)
   {
      uint32_t actual;
      InputOutput_Read(&inputOutputSimple.interface, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenEnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeEnable, &state);
   }

   void WhenEnhancedSabbathModeChangesTo(bool state)
   {
      GivenEnhancedSabbathModeIs(state);
   }

   void GivenTheFilteredTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureInDegFx100, &temperature);
   }

   void GivenTheUnfilteredTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_UnfilteredTemperatureInDegFx100, &temperature);
   }

   void WhenTheUnfilteredTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      GivenTheUnfilteredTemperatureIs(temperature);
   }

   void TheOverrideValueShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheOverrideRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFood_AveragedTemperatureOverrideRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheFilterShouldHaveBeenFedWith(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      Filter_TestDouble_GetLastFeedValue(&filter, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheFilterShouldHaveBeenSeededWith(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      Filter_TestDouble_GetLastSeedValue(&filter, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheFilterShouldRead(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      Filter_Read(&filter.interface, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheFilterHasAValueOf(TemperatureDegFx100_t temperature)
   {
      Filter_TestDouble_SetCurrentFilterValue(&filter, &temperature);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }
};

TEST(EnhancedSabbathAverageTemperature, ShouldSetFilteredTemperatureIntoOverrideValueWhenEnhancedSabbathModeBecomesActive)
{
   GivenTheRandomAdditionalTimeHasBeenGeneratedAs(0);
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenInitialization();
   WhenEnhancedSabbathModeChangesTo(SET);

   TheOverrideRequestShouldBe(SET);
   TheOverrideValueShouldBe(SomeFilteredTemperature);
}

TEST(EnhancedSabbathAverageTemperature, ShouldSetFilteredTemperatureIntoOverrideValueIfEnhancedSabbathModeIsActiveOnInit)
{
   GivenEnhancedSabbathModeIs(SET);
   GivenTheRandomAdditionalTimeHasBeenGeneratedAs(0);
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenInitialization();

   TheOverrideRequestShouldBe(SET);
   TheOverrideValueShouldBe(SomeFilteredTemperature);
}

TEST(EnhancedSabbathAverageTemperature, ShouldStartFeedingPeriodicallyWhenEnhancedSabbathModeBecomesActive)
{
   GivenTheRandomAdditionalTimeHasBeenGeneratedAs(0);
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenTheUnfilteredTemperatureIs(SomeUnfilteredTemperature);
   GivenInitialization();
   WhenEnhancedSabbathModeChangesTo(SET);

   TheFilterShouldHaveBeenFedWith(0);

   After(TimeBetweenReadsInMsec - 1);
   TheFilterShouldHaveBeenFedWith(0);

   After(1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature);

   WhenTheUnfilteredTemperatureChangesTo(SomeUnfilteredTemperature + 1);
   After(TimeBetweenReadsInMsec - 1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature);

   After(1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature + 1);
}

TEST(EnhancedSabbathAverageTemperature, ShouldStartFeedingPeriodicallyIfEnhancedSabbathModeIsActiveOnInit)
{
   GivenEnhancedSabbathModeIs(SET);
   GivenTheRandomAdditionalTimeHasBeenGeneratedAs(0);
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenTheUnfilteredTemperatureIs(SomeUnfilteredTemperature);
   GivenInitialization();

   TheFilterShouldHaveBeenFedWith(0);

   After(TimeBetweenReadsInMsec - 1);
   TheFilterShouldHaveBeenFedWith(0);

   After(1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature);

   WhenTheUnfilteredTemperatureChangesTo(SomeUnfilteredTemperature + 1);
   After(TimeBetweenReadsInMsec - 1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature);

   After(1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature + 1);
}

TEST(EnhancedSabbathAverageTemperature, ShouldStopFeedingPeriodicallyWhenEnhancedSabbathModeIsDisabled)
{
   GivenTheRandomAdditionalTimeHasBeenGeneratedAs(0);
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenTheUnfilteredTemperatureIs(SomeUnfilteredTemperature);
   GivenInitialization();
   WhenEnhancedSabbathModeChangesTo(SET);

   TheFilterShouldHaveBeenFedWith(0);

   After(TimeBetweenReadsInMsec - 1);
   TheFilterShouldHaveBeenFedWith(0);

   After(1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature);

   WhenTheUnfilteredTemperatureChangesTo(SomeUnfilteredTemperature + 1);
   After(TimeBetweenReadsInMsec - 1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature);

   After(1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature + 1);

   WhenTheUnfilteredTemperatureChangesTo(SomeUnfilteredTemperature + 2);
   WhenEnhancedSabbathModeChangesTo(CLEAR);
   After(TimeBetweenReadsInMsec - 1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature + 1);

   After(1);
   TheFilterShouldHaveBeenFedWith(SomeUnfilteredTemperature + 1);
}

TEST(EnhancedSabbathAverageTemperature, ShouldWriteTheFilteredValueToTheOverrideValueAndResetTheFilterAfterRandomTimeHasElapsed)
{
   GivenTheRandomAdditionalTimeHasBeenGeneratedAs(OneMinuteInMsec);
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenTheUnfilteredTemperatureIs(SomeUnfilteredTemperature);
   GivenInitialization();
   WhenEnhancedSabbathModeChangesTo(SET);

   GivenTheFilterHasAValueOf(SomeOtherFilteredTemperature);
   After(ThreeMinutesInMsec - 1);

   TheFilterShouldRead(SomeOtherFilteredTemperature);
   TheOverrideValueShouldBe(SomeFilteredTemperature);

   After(1);
   TheFilterShouldRead(0);
   TheOverrideValueShouldBe(SomeOtherFilteredTemperature);
}

TEST(EnhancedSabbathAverageTemperature, ShouldPeriodicallyWriteTheFilteredValueToTheOverrideValueAndResetTheFilterAfterRandomTimeHasElapsed)
{
   GivenTheRandomAdditionalTimeHasBeenGeneratedAs(0);
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenTheUnfilteredTemperatureIs(SomeUnfilteredTemperature);
   GivenInitialization();
   WhenEnhancedSabbathModeChangesTo(SET);

   GivenTheFilterHasAValueOf(SomeOtherFilteredTemperature);
   After(TwoMinutesInMsec - 1);

   TheFilterShouldRead(SomeOtherFilteredTemperature);
   TheOverrideValueShouldBe(SomeFilteredTemperature);

   WhenTheRandomAdditionalTimeHasBeenGeneratedAs(OneMinuteInMsec);
   After(1);
   TheFilterShouldRead(0);
   TheOverrideValueShouldBe(SomeOtherFilteredTemperature);

   GivenTheFilterHasAValueOf(SomeFilteredTemperature);
   GivenTheFilteredTemperatureIs(SomeOtherFilteredTemperature);
   After(ThreeMinutesInMsec - 1);

   TheFilterShouldRead(SomeFilteredTemperature);
   TheOverrideValueShouldBe(SomeOtherFilteredTemperature);

   After(1);
   TheFilterShouldRead(0);
   TheOverrideValueShouldBe(SomeFilteredTemperature);
}

TEST(EnhancedSabbathAverageTemperature, ShouldPeriodicallyWriteTheFilteredValueToTheOverrideValueAndResetTheFilterAfterRandomTimeHasElapsedIfEnhancedSabbathModeIsActiveOnInit)
{
   GivenEnhancedSabbathModeIs(SET);
   GivenTheRandomAdditionalTimeHasBeenGeneratedAs(0);
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenTheUnfilteredTemperatureIs(SomeUnfilteredTemperature);
   GivenInitialization();

   GivenTheFilterHasAValueOf(SomeOtherFilteredTemperature);
   After(TwoMinutesInMsec - 1);

   TheFilterShouldRead(SomeOtherFilteredTemperature);
   TheOverrideValueShouldBe(SomeFilteredTemperature);

   WhenTheRandomAdditionalTimeHasBeenGeneratedAs(OneMinuteInMsec);
   After(1);
   TheFilterShouldRead(0);
   TheOverrideValueShouldBe(SomeOtherFilteredTemperature);

   GivenTheFilterHasAValueOf(SomeFilteredTemperature);
   GivenTheFilteredTemperatureIs(SomeOtherFilteredTemperature);
   After(ThreeMinutesInMsec - 1);

   TheFilterShouldRead(SomeFilteredTemperature);
   TheOverrideValueShouldBe(SomeOtherFilteredTemperature);

   After(1);
   TheFilterShouldRead(0);
   TheOverrideValueShouldBe(SomeFilteredTemperature);
}

TEST(EnhancedSabbathAverageTemperature, ShouldStopPeriodicallyWritingTheFilteredValueToTheOverrideValueAndResetTheFilterAfterRandomTimeHasElapsedWhenEnhancedSabbathIsDisabled)
{
   GivenTheRandomAdditionalTimeHasBeenGeneratedAs(0);
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenTheUnfilteredTemperatureIs(SomeUnfilteredTemperature);
   GivenInitialization();
   WhenEnhancedSabbathModeChangesTo(SET);

   GivenTheFilterHasAValueOf(SomeOtherFilteredTemperature);
   After(TwoMinutesInMsec - 1);

   TheFilterShouldRead(SomeOtherFilteredTemperature);
   TheOverrideValueShouldBe(SomeFilteredTemperature);

   After(1);
   TheFilterShouldRead(0);
   TheOverrideValueShouldBe(SomeOtherFilteredTemperature);

   GivenTheFilterHasAValueOf(SomeFilteredTemperature);
   GivenTheFilteredTemperatureIs(SomeOtherFilteredTemperature);
   After(TwoMinutesInMsec - 1);

   TheFilterShouldRead(SomeFilteredTemperature);
   TheOverrideValueShouldBe(SomeOtherFilteredTemperature);

   WhenTheRandomAdditionalTimeHasBeenGeneratedAs(OneMinuteInMsec);
   After(1);
   TheFilterShouldRead(0);
   TheOverrideValueShouldBe(SomeFilteredTemperature);

   GivenTheFilterHasAValueOf(SomeOtherFilteredTemperature);
   WhenEnhancedSabbathModeChangesTo(CLEAR);
   TheOverrideRequestShouldBe(CLEAR);
   After(ThreeMinutesInMsec - 1);

   TheFilterShouldRead(0);
   TheOverrideValueShouldBe(SomeFilteredTemperature);

   After(1);
   TheFilterShouldRead(0);
   TheOverrideValueShouldBe(SomeFilteredTemperature);
}

TEST(EnhancedSabbathAverageTemperature, ShouldSeedTheFilterWhenEnteringEnhancedSabbathMode)
{
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenInitialization();

   WhenEnhancedSabbathModeChangesTo(SET);
   TheFilterShouldHaveBeenSeededWith(SomeFilteredTemperature);
}

TEST(EnhancedSabbathAverageTemperature, ShouldSeedTheFilterWhenEnteringEnhancedSabbathModeOnInit)
{
   GivenEnhancedSabbathModeIs(SET);
   GivenTheFilteredTemperatureIs(SomeFilteredTemperature);
   GivenInitialization();

   TheFilterShouldHaveBeenSeededWith(SomeFilteredTemperature);
}
