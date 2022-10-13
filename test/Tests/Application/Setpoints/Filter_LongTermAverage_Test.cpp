/*!
 * @file
 * @brief Test for long term average filter
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

extern "C"
{
#include "Filter_LongTermAverage.h"
#include "utils.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

enum
{
   Beta = 2,
   SomeBeta = 4
};

TEST_GROUP(Filter_LongTermAverage)
{
   Filter_LongTermAverage_t instance;
   EventSubscription_t readySubscription;

   void setup()
   {
      EventSubscription_Init(&readySubscription, NULL, InputChangeCallback);
   }

   static void InputChangeCallback(void *context, const void *_args)
   {
      REINTERPRET(result, _args, const bool *);
      mock()
         .actualCall("ReadyChanged")
         .withParameter("state", *result);
   }

   void GivenThatTheFilterHasBeenInitializedWithBeta(uint8_t passedInBeta)
   {
      Filter_LongTermAverage_Init(&instance, passedInBeta);
   }

   void GivenThatTheFilterHasBeenInitialized(void)
   {
      Filter_LongTermAverage_Init(&instance, Beta);
   }

   void WhenTheFilterIsReset()
   {
      Filter_Reset(&instance.interface);
   }

   void AfterTheFilterIsReset()
   {
      Filter_Reset(&instance.interface);
   }

   void TheFilterReadyStateShouldBe(bool expected)
   {
      bool actual;
      Input_Read(Filter_GetReadyInput(&instance.interface), &actual);
      CHECK(expected == actual);
   }

   void TheFilterShouldBeReady()
   {
      TheFilterReadyStateShouldBe(true);
   }

   void TheFilterShouldNotBeReady()
   {
      TheFilterReadyStateShouldBe(false);
   }

   void NothingShouldHappen()
   {
   }

   void TheFilteredValueShouldBe(int16_t expected)
   {
      int16_t actual;
      Filter_Read(&instance.interface, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenThatTheFilterHasBeenSeededWith(int16_t sample)
   {
      WhenTheFilterIsSeededWith(sample);
   }

   void AfterThatTheFilterHasBeenSeededWith(int16_t sample)
   {
      GivenThatTheFilterHasBeenSeededWith(sample);
   }

   void WhenTheFilterIsSeededWith(int16_t sample)
   {
      Filter_Seed(&instance.interface, &sample);
   }

   void AfterFeedingTheFilterWith(int16_t sample)
   {
      Filter_Feed(&instance.interface, &sample);
   }

   void ExpectTheFilterShouldBecomeReady()
   {
      mock()
         .expectOneCall("ReadyChanged")
         .withParameter("state", true);
   }

   void ExpectTheFilterShouldBecomeNotReady()
   {
      mock()
         .expectOneCall("ReadyChanged")
         .withParameter("state", false);
   }

   void GivenThatASubscriptionToTheReadyEventIsActive()
   {
      Event_Subscribe(
         Input_GetOnChangeEvent(Filter_GetReadyInput(&instance.interface)),
         &readySubscription);
   }
};

TEST(Filter_LongTermAverage, ShouldAssertWhenBetaIsLessThanOne)
{
   ShouldFailAssertion(GivenThatTheFilterHasBeenInitializedWithBeta(0));
}

TEST(Filter_LongTermAverage, ShouldNotBeReadyAfterInitialization)
{
   GivenThatTheFilterHasBeenInitialized();
   TheFilterShouldNotBeReady();
}

TEST(Filter_LongTermAverage, ShouldBeReadyAfterSeeded)
{
   GivenThatTheFilterHasBeenInitialized();
   GivenThatASubscriptionToTheReadyEventIsActive();

   ExpectTheFilterShouldBecomeReady();
   AfterThatTheFilterHasBeenSeededWith(1);
   TheFilterShouldBeReady();
}

TEST(Filter_LongTermAverage, ShouldNotBeReadyAfterReset)
{
   GivenThatTheFilterHasBeenInitialized();
   GivenThatASubscriptionToTheReadyEventIsActive();

   ExpectTheFilterShouldBecomeReady();
   AfterThatTheFilterHasBeenSeededWith(1);
   TheFilterShouldBeReady();

   ExpectTheFilterShouldBecomeNotReady();
   AfterTheFilterIsReset();
   TheFilterShouldNotBeReady();
}

TEST(Filter_LongTermAverage, ShouldAllowTheFilteredValueToBeReadAfterInitialization)
{
   GivenThatTheFilterHasBeenInitialized();
   GivenThatTheFilterHasBeenSeededWith(21);
   TheFilteredValueShouldBe(21);
}

TEST(Filter_LongTermAverage, ShouldSetTheFilteredValueWhenSeeded)
{
   GivenThatTheFilterHasBeenInitialized();

   WhenTheFilterIsSeededWith(513);
   TheFilteredValueShouldBe(513);

   WhenTheFilterIsSeededWith(-1042);
   TheFilteredValueShouldBe(-1042);
}

TEST(Filter_LongTermAverage, ShouldUpdateFilteredValueWhenDataIsFed)
{
   GivenThatTheFilterHasBeenInitializedWithBeta(SomeBeta);
   GivenThatTheFilterHasBeenSeededWith(100);
   TheFilteredValueShouldBe(100);

   AfterFeedingTheFilterWith(8000);
   TheFilteredValueShouldBe(223);

   AfterFeedingTheFilterWith(8000);
   TheFilteredValueShouldBe(500);

   AfterFeedingTheFilterWith(10000);
   TheFilteredValueShouldBe(947);
}

TEST(Filter_LongTermAverage, ShouldReachStableFilteredValueWhenSameDataIsFed)
{
   GivenThatTheFilterHasBeenInitializedWithBeta(Beta);
   GivenThatTheFilterHasBeenSeededWith(100);
   TheFilteredValueShouldBe(100);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(337);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(693);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1049);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1346);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1568);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1724);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1828);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1895);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1937);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1962);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1977);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1986);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1991);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1994);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1996);

   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1997);

   //Stable Value
   AfterFeedingTheFilterWith(2000);
   TheFilteredValueShouldBe(1997);
}

TEST(Filter_LongTermAverage, ShouldResetTheFilteredValueToZeroAfterReset)
{
   GivenThatTheFilterHasBeenInitialized();
   GivenThatTheFilterHasBeenSeededWith(21);
   TheFilteredValueShouldBe(21);

   WhenTheFilterIsReset();
   TheFilteredValueShouldBe(0);
}
