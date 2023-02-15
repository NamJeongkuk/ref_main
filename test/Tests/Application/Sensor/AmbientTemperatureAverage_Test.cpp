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

   void TheWindowAverageFilterHasBeenInitialized()
   {
      AmbientTemperatureAverage_Init(&instance, dataModel);
   }

   void TheFilteredAmbientTemperatureChangesTo(TemperatureDegFx100_t seedValue)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
         &seedValue);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1000);
   }

   void CrossWindowAverageShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(
         dataModel,
         Erd_Ambient_CrossAmbientWindowAveragedTemperatureInDegFx100,
         &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(AmbientTemperatureAverage, ShouldSetCrossWindowAverageToZeroOnInit)
{
   Given TheWindowAverageFilterHasBeenInitialized();
   CrossWindowAverageShouldBe(0);
}

TEST(AmbientTemperatureAverage, ShouldSeedCrossAmbientWindowAverageTemperatureOnInit)
{
   Given TheFilteredAmbientTemperatureChangesTo(16);
   Given TheWindowAverageFilterHasBeenInitialized();

   CrossWindowAverageShouldBe(16);
}

TEST(AmbientTemperatureAverage, ShouldFeedFilterEveryFiveMinutesAndWriteFilterValueToCrossWindowAverage)
{
   Given TheFilteredAmbientTemperatureChangesTo(4);
   TheWindowAverageFilterHasBeenInitialized();

   After(AmbientTemperatureAverageSampleFrequencyInMinutes * MSEC_PER_MIN);

   Given TheFilteredAmbientTemperatureChangesTo(5);
   CrossWindowAverageShouldBe(4);

   After(AmbientTemperatureAverageSampleFrequencyForFiftyMinutes * MSEC_PER_MIN);
   CrossWindowAverageShouldBe(4);

   After(AmbientTemperatureAverageSampleFrequencyForOneHourAndFiveMinutes * MSEC_PER_MIN);
   CrossWindowAverageShouldBe(5);

   After((AmbientTemperatureAverageSampleFrequencyForOneHourAndFiveMinutes + AmbientTemperatureAverageSampleFrequencyInMinutes) * MSEC_PER_MIN);
   CrossWindowAverageShouldBe(5);
}

TEST(AmbientTemperatureAverage, ShouldFeedFilterTwoHoursAndWriteFilterValueToCrossWindowAverage)
{
   Given TheFilteredAmbientTemperatureChangesTo(14);
   TheWindowAverageFilterHasBeenInitialized();

   After(AmbientTemperatureAverageSampleFrequencyForTwoHoursInMinutes * MSEC_PER_MIN - 1);

   Given TheFilteredAmbientTemperatureChangesTo(16);
   CrossWindowAverageShouldBe(14);

   After(1);
   CrossWindowAverageShouldBe(14);

   Given TheFilteredAmbientTemperatureChangesTo(48);
   After(AmbientTemperatureAverageSampleFrequencyForTwoHoursInMinutes * MSEC_PER_MIN - 1);
   CrossWindowAverageShouldBe(47);

   After(1);
   CrossWindowAverageShouldBe(48);
}

TEST(AmbientTemperatureAverage, ShouldUpdateCrossWindowFilterWhenTemperatureChangesMultipleTimes)
{
   Given TheFilteredAmbientTemperatureChangesTo(4);
   TheWindowAverageFilterHasBeenInitialized();

   After(AmbientTemperatureAverageSampleFrequencyInMinutes * MSEC_PER_MIN * (AmbientTemperatureAverageSampleCount / AmbientTemperatureAverageSampleCountDividedByTwo) - 1);
   CrossWindowAverageShouldBe(4);

   After(1);
   CrossWindowAverageShouldBe(4);

   Given TheFilteredAmbientTemperatureChangesTo(8);
   After(AmbientTemperatureAverageSampleFrequencyInMinutes * MSEC_PER_MIN * (AmbientTemperatureAverageSampleCount / AmbientTemperatureAverageSampleCountDividedByTwo) - 1);
   CrossWindowAverageShouldBe(6);

   After(1);
   CrossWindowAverageShouldBe(6);
}
