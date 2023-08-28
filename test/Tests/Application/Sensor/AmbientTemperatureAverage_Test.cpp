/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "AmbientTemperatureAverage.h"
#include "Filter_WindowAverage.h"
#include "SystemErds.h"
#include "Timer.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
}

#include "ReferDataModel_TestDouble.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

#define Given
#define When

enum
{
   AmbientTemperatureAverageSampleCountDividedByTwo = 2,
   AmbientTemperatureAverageSampleFrequencyForFiftyMinutes = 50,
   AmbientTemperatureAverageSampleFrequencyForOneHourAndFiveMinutes = 65,
   AmbientTemperatureAverageSampleFrequencyForTwoHoursInMinutes = 120
};

TEST_GROUP(AmbientTemperatureAverage)
{
   AmbientTemperatureAverage_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenTheWindowAverageFilterHasBeenInitialized()
   {
      AmbientTemperatureAverage_Init(&instance, dataModel);
   }

   void WhenTheFilteredAmbientTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void GivenTheFilteredAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      WhenTheFilteredAmbientTemperatureChangesTo(temperature);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1000);
   }

   void WindowAverageShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(
         dataModel,
         Erd_Ambient_WindowAveragedTemperatureInDegFx100,
         &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(AmbientTemperatureAverage, ShouldSetWindowAverageToFilteredTemperatureOnInit)
{
   GivenTheFilteredAmbientTemperatureIs(7500);
   GivenTheWindowAverageFilterHasBeenInitialized();

   WindowAverageShouldBe(7500);
}

TEST(AmbientTemperatureAverage, ShouldSeedAmbientWindowAverageTemperatureAfterTheFirstOnChangeEvent)
{
   GivenTheFilteredAmbientTemperatureIs(7500);
   GivenTheWindowAverageFilterHasBeenInitialized();

   WhenTheFilteredAmbientTemperatureChangesTo(9000);
   WindowAverageShouldBe(9000);
}

TEST(AmbientTemperatureAverage, ShouldFeedFilterEveryFiveMinutesAndWriteFilterValueToWindowAverage)
{
   GivenTheWindowAverageFilterHasBeenInitialized();
   GivenTheFilteredAmbientTemperatureIs(4);

   After(AmbientTemperatureAverageSampleFrequencyInMinutes * MSEC_PER_MIN);

   WhenTheFilteredAmbientTemperatureChangesTo(5);
   WindowAverageShouldBe(4);

   After(AmbientTemperatureAverageSampleFrequencyForFiftyMinutes * MSEC_PER_MIN);
   WindowAverageShouldBe(4);

   After(AmbientTemperatureAverageSampleFrequencyForOneHourAndFiveMinutes * MSEC_PER_MIN);
   WindowAverageShouldBe(5);

   After((AmbientTemperatureAverageSampleFrequencyForOneHourAndFiveMinutes + AmbientTemperatureAverageSampleFrequencyInMinutes) * MSEC_PER_MIN);
   WindowAverageShouldBe(5);
}

TEST(AmbientTemperatureAverage, ShouldFeedFilterEveryTwoHoursAndWriteFilterValueToWindowAverage)
{
   GivenTheWindowAverageFilterHasBeenInitialized();
   GivenTheFilteredAmbientTemperatureIs(14);

   After(AmbientTemperatureAverageSampleFrequencyForTwoHoursInMinutes * MSEC_PER_MIN - 1);

   WhenTheFilteredAmbientTemperatureChangesTo(16);
   WindowAverageShouldBe(14);

   After(1);
   WindowAverageShouldBe(14);

   WhenTheFilteredAmbientTemperatureChangesTo(48);
   After(AmbientTemperatureAverageSampleFrequencyForTwoHoursInMinutes * MSEC_PER_MIN - 1);
   WindowAverageShouldBe(47);

   After(1);
   WindowAverageShouldBe(48);
}

TEST(AmbientTemperatureAverage, ShouldUpdateWindowFilterWhenTemperatureChangesMultipleTimes)
{
   GivenTheWindowAverageFilterHasBeenInitialized();
   GivenTheFilteredAmbientTemperatureIs(4);

   After(AmbientTemperatureAverageSampleFrequencyInMinutes * MSEC_PER_MIN * (AmbientTemperatureAverageSampleCount / AmbientTemperatureAverageSampleCountDividedByTwo) - 1);
   WindowAverageShouldBe(4);

   After(1);
   WindowAverageShouldBe(4);

   WhenTheFilteredAmbientTemperatureChangesTo(8);
   After(AmbientTemperatureAverageSampleFrequencyInMinutes * MSEC_PER_MIN * (AmbientTemperatureAverageSampleCount / AmbientTemperatureAverageSampleCountDividedByTwo) - 1);
   WindowAverageShouldBe(6);

   After(1);
   WindowAverageShouldBe(6);
}
