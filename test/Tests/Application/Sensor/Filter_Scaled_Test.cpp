/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Filter_Scaled.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "Filter_TestDouble.h"

enum
{
   ScalingFactor = UINT8_MAX
};

enum
{
   SomeSeededValue = 2,
   SomeFedValue = 10
};

TEST_GROUP(Filter_Scaled)
{
   Filter_Scaled_t instance;
   Filter_TestDouble_t wrappedFilter;
   int32_t seedBuffer;
   int32_t feedBuffer;
   int32_t readBuffer;

   void setup()
   {
      Filter_TestDouble_Init(&wrappedFilter, &seedBuffer, &feedBuffer, &readBuffer, sizeof(seedBuffer));
   }

   void GivenTheFilterHasBeenInitialized()
   {
      Filter_Scaled_Init(&instance, &wrappedFilter.interface, ScalingFactor);
   }

   void AfterTheFilterIsSeededWith(int16_t value)
   {
      Filter_Seed(&instance.interface, &value);
   }

   void TheWrappedFilterShouldHaveBeenSeededWith(int32_t expected)
   {
      int32_t actual;
      Filter_TestDouble_GetLastSeedValue(&wrappedFilter, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void AfterTheFilterIsFedWith(int16_t value)
   {
      Filter_Feed(&instance.interface, &value);
   }

   void TheWrappedFilterShouldHaveBeenFedWith(int32_t expected)
   {
      int32_t actual;
      Filter_TestDouble_GetLastFeedValue(&wrappedFilter, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheFilterShouldRead(int16_t expected)
   {
      int16_t actual;
      Filter_Read(&instance.interface, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheWrappedFilterCurrentFilterValueIs(int32_t value)
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
      GivenTheFilterHasBeenInitialized();

      AfterTheFilterIsSeededWith(SomeSeededValue);
      TheWrappedFilterShouldHaveBeenSeededWith(SomeSeededValue * ScalingFactor);
   }
};

TEST(Filter_Scaled, ShouldScaleTheFilteredValueWhenSeeded)
{
   GivenTheFilterHasBeenInitialized();

   AfterTheFilterIsSeededWith(SomeSeededValue);
   TheWrappedFilterShouldHaveBeenSeededWith(SomeSeededValue * ScalingFactor);
}

TEST(Filter_Scaled, ShouldScaleTheFilteredValueWhenFed)
{
   GivenTheFilterHasBeenSeeded();

   AfterTheFilterIsFedWith(SomeFedValue);
   TheWrappedFilterShouldHaveBeenFedWith(SomeFedValue * ScalingFactor);
}

TEST(Filter_Scaled, ShouldReadTheFilteredValue)
{
   GivenTheFilterHasBeenSeeded();
   TheWrappedFilterCurrentFilterValueIs(SomeSeededValue * ScalingFactor);

   TheFilterShouldRead(SomeSeededValue);
}

TEST(Filter_Scaled, ShouldBeReadyWhenWrappedFilterIsReady)
{
   GivenTheFilterHasBeenInitialized();
   GivenTheWrappedFilterIsReady();

   TheFilterShouldBeReady();
}

TEST(Filter_Scaled, ShouldBeNotReadyWhenWrappedFilterIsNotReady)
{
   GivenTheFilterHasBeenInitialized();
   GivenTheWrappedFilterIsNotReady();

   TheFilterShouldBeNotReady();
}

TEST(Filter_Scaled, ShouldBeNotReadyWhenWrappedFilterIsReset)
{
   GivenTheFilterHasBeenInitialized();
   GivenTheWrappedFilterIsReady();

   WhenTheFilterIsReset();
   TheFilterShouldBeNotReady();
}
