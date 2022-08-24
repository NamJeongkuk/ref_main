/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Filter_Slew.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "Filter_TestDouble.h"

TEST_GROUP(Filter_Slew)
{
   Filter_Slew_t instance;
   Filter_TestDouble_t wrappedFilter;
   int16_t seedBuffer;
   int16_t feedBuffer;
   int16_t readBuffer;

   void setup()
   {
      Filter_TestDouble_Init(
         &wrappedFilter,
         &seedBuffer,
         &feedBuffer,
         &readBuffer,
         sizeof(seedBuffer));
   }

   void GivenTheFilterHasBeenInitializedWithSlewRateLimit(uint16_t slewRateLimitx100)
   {
      Filter_Slew_Init(
         &instance,
         &wrappedFilter.interface,
         (uint16_t)slewRateLimitx100);
   }

   void AfterTheFilterIsSeededWith(int16_t value)
   {
      Filter_Seed(&instance.interface, &value);
   }

   void TheWrappedFilterShouldHaveBeenSeededWith(int16_t expected)
   {
      int16_t actual;
      Filter_TestDouble_GetLastSeedValue(&wrappedFilter, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void AfterTheFilterIsFedWith(int16_t value)
   {
      Filter_Feed(&instance.interface, &value);
   }

   void TheWrappedFilterShouldHaveBeenFedWith(int16_t expected)
   {
      int16_t actual;
      Filter_TestDouble_GetLastFeedValue(&wrappedFilter, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheFilterShouldRead(int16_t expected)
   {
      int16_t actual;
      Filter_Read(&instance.interface, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheWrappedFilterCurrentFilterValueIs(int16_t value)
   {
      Filter_TestDouble_SetCurrentFilterValue(&wrappedFilter, &value);
   }

   void GivenTheWrappedFilterIsReady()
   {
      Filter_TestDouble_ChangeReadyStateTo(&wrappedFilter, true);
   }

   void TheFilterShouldBeReady()
   {
      I_Input_t *readyInput = Filter_GetReadyInput(&instance.interface);
      bool ready;
      Input_Read(readyInput, &ready);

      CHECK_TRUE(ready);
   }

   void GivenTheWrappedFilterIsNotReady()
   {
      Filter_TestDouble_ChangeReadyStateTo(&wrappedFilter, false);
   }

   void TheFilterShouldBeNotReady()
   {
      I_Input_t *readyInput = Filter_GetReadyInput(&instance.interface);
      bool ready;
      Input_Read(readyInput, &ready);

      CHECK_FALSE(ready);
   }

   void WhenTheFilterIsReset()
   {
      Filter_Reset(&instance.interface);
   }

   void GivenTheFilterHasBeenSeeded()
   {
      GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);
      AfterTheFilterIsSeededWith(2);
      TheWrappedFilterShouldHaveBeenSeededWith(2);
   }
};

TEST(Filter_Slew, ShouldSeedTheWrappedFilterWithSameValueWhenSeeded)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);

   AfterTheFilterIsSeededWith(2);
   TheWrappedFilterShouldHaveBeenSeededWith(2);
}

TEST(Filter_Slew, ShouldFeedTheWrappedFilterWithSameValue)
{
   GivenTheFilterHasBeenSeeded();

   AfterTheFilterIsFedWith(20);
   TheWrappedFilterShouldHaveBeenFedWith(20);
}

TEST(Filter_Slew, ShouldSlewTheReadValueWhenWrappedFilterValueChangesMoreThanTheSlewLimitFromPreviousValueInNegativeDirection)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);

   WhenTheWrappedFilterCurrentFilterValueIs(20);
   TheFilterShouldRead(20);

   WhenTheWrappedFilterCurrentFilterValueIs(5);
   TheFilterShouldRead(18);
}

TEST(Filter_Slew, ShouldSlewTheReadValueWhenWrappedFilterValueChangesMoreThanTheSlewLimitFromPreviousValueInPositiveDirection)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);

   WhenTheWrappedFilterCurrentFilterValueIs(20);
   TheFilterShouldRead(20);

   WhenTheWrappedFilterCurrentFilterValueIs(40);
   TheFilterShouldRead(22);
}

TEST(Filter_Slew, ShouldNotSlewTheReadValueWhenWrappedFilterValueChangesLessThanTheSlewLimitFromPreviousValueInPositiveDirection)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);

   WhenTheWrappedFilterCurrentFilterValueIs(20);
   TheFilterShouldRead(20);

   WhenTheWrappedFilterCurrentFilterValueIs(21);
   TheFilterShouldRead(21);
}

TEST(Filter_Slew, ShouldNotSlewTheReadValueWhenWrappedFilterValueChangesLessThanTheSlewLimitFromPreviousValueInNegativeDirection)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);

   WhenTheWrappedFilterCurrentFilterValueIs(20);
   TheFilterShouldRead(20);

   WhenTheWrappedFilterCurrentFilterValueIs(19);
   TheFilterShouldRead(19);
}

TEST(Filter_Slew, ShouldSlewMultipleTimesInARow)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);

   WhenTheWrappedFilterCurrentFilterValueIs(20);
   TheFilterShouldRead(20);

   WhenTheWrappedFilterCurrentFilterValueIs(40);
   TheFilterShouldRead(22);

   WhenTheWrappedFilterCurrentFilterValueIs(20);
   TheFilterShouldRead(20);
}

TEST(Filter_Slew, ShouldNotAllowFilterValueToChangeWhenSlewRateLimitIsZero)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(0);

   WhenTheWrappedFilterCurrentFilterValueIs(20);
   TheFilterShouldRead(20);

   WhenTheWrappedFilterCurrentFilterValueIs(40);
   TheFilterShouldRead(20);

   WhenTheWrappedFilterCurrentFilterValueIs(60);
   TheFilterShouldRead(20);
}

TEST(Filter_Slew, ShouldSlewWhenValuesAreNegative)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);

   WhenTheWrappedFilterCurrentFilterValueIs(-20);
   TheFilterShouldRead(-20);

   WhenTheWrappedFilterCurrentFilterValueIs(-40);
   TheFilterShouldRead(-22);

   WhenTheWrappedFilterCurrentFilterValueIs(-15);
   TheFilterShouldRead(-20);
}

TEST(Filter_Slew, ShouldSlewWhenValuesSwitchBetweenNegativeAndPositive)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);

   WhenTheWrappedFilterCurrentFilterValueIs(-20);
   TheFilterShouldRead(-20);

   WhenTheWrappedFilterCurrentFilterValueIs(0);
   TheFilterShouldRead(-18);

   WhenTheWrappedFilterCurrentFilterValueIs(20);
   TheFilterShouldRead(-16);

   WhenTheWrappedFilterCurrentFilterValueIs(-20);
   TheFilterShouldRead(-18);
}

TEST(Filter_Slew, ShouldNotSlewWithMaxSlewRateAndLargestTemperatureChange)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(65535);

   WhenTheWrappedFilterCurrentFilterValueIs(-32768);
   TheFilterShouldRead(-32768);

   WhenTheWrappedFilterCurrentFilterValueIs(32767);
   TheFilterShouldRead(32767);

   WhenTheWrappedFilterCurrentFilterValueIs(-32768);
   TheFilterShouldRead(-32768);
}

TEST(Filter_Slew, ShouldSlewWithSlewRateGreaterThanInt16Max)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(4000);

   WhenTheWrappedFilterCurrentFilterValueIs(-32768);
   TheFilterShouldRead(-32768);

   WhenTheWrappedFilterCurrentFilterValueIs(32767);
   TheFilterShouldRead(-28768);
}

TEST(Filter_Slew, ShouldBeReadyWhenWrappedFilterIsReady)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);
   GivenTheWrappedFilterIsReady();

   TheFilterShouldBeReady();
}

TEST(Filter_Slew, ShouldBeNotReadyWhenWrappedFilterIsNotReady)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);
   GivenTheWrappedFilterIsNotReady();

   TheFilterShouldBeNotReady();
}

TEST(Filter_Slew, ShouldBeNotReadyWhenWrappedFilterIsReset)
{
   GivenTheFilterHasBeenInitializedWithSlewRateLimit(2);
   GivenTheWrappedFilterIsReady();

   WhenTheFilterIsReset();
   TheFilterShouldBeNotReady();
}
