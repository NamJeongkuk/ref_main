/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Event_OneMillisecondTimer.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "MemoryUtils.h"
#include "TimerModule_TestDouble.h"

TEST_GROUP(Event_OneMillisecondTimer)
{
   Event_OneMillisecondTimer_t instance;
   EventSubscription_t subscription;
   EventSubscription_t anotherSubscription;
   TimerModule_TestDouble_t timerModuleTestDouble;

   static void Callback(void *, const void *args)
   {
      mock().actualCall("Callback").withParameter("args", args);
   }

   void setup()
   {
      TimerModule_TestDouble_Init(&timerModuleTestDouble);

      EventSubscription_Init(&subscription, NULL, Callback);
      EventSubscription_Init(&anotherSubscription, NULL, Callback);

      ASSUME_UNINITIALIZED(instance);
   }

   void GivenThatTheEventHasBeenInitialized()
   {
      Event_OneMillisecondTimer_Init(&instance, &timerModuleTestDouble.timerModule);
   }

   void APublicationShouldBeReceivedWithArgs(const void *args)
   {
      mock().expectOneCall("Callback").withParameter("args", args);
   }

   void NothingShouldHappen()
   {
   }

   void GivenThatASubscriptionWasAdded()
   {
      Event_Subscribe(&instance.interface, &subscription);
   }

   void GivenThatASubscriptionWasAddedAndRemoved()
   {
      Event_Subscribe(&instance.interface, &subscription);
      Event_Unsubscribe(&instance.interface, &subscription);
   }

   void AfterUnsubscribing()
   {
      Event_Unsubscribe(&instance.interface, &subscription);
   }

   void WhenAnotherSubscriptionIsAdded()
   {
      Event_Subscribe(&instance.interface, &anotherSubscription);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, ticks);
   }
};

TEST(Event_OneMillisecondTimer, ShouldInitialize)
{
   GivenThatTheEventHasBeenInitialized();
}

TEST(Event_OneMillisecondTimer, ShouldPublishEventIfSubscribedAndAfterOneMillisecond)
{
   GivenThatTheEventHasBeenInitialized();
   GivenThatASubscriptionWasAdded();

   APublicationShouldBeReceivedWithArgs(NULL);
   After(1);
}

TEST(Event_OneMillisecondTimer, ShouldKeepPublishingEventsEveryOneMillisecond)
{
   GivenThatTheEventHasBeenInitialized();
   GivenThatASubscriptionWasAdded();

   for(uint8_t i = 0; i < 10; i++)
   {
      APublicationShouldBeReceivedWithArgs(NULL);
      After(1);
   }
}

TEST(Event_OneMillisecondTimer, ShouldStopPublishingAfterUnsubscribing)
{
   GivenThatTheEventHasBeenInitialized();
   GivenThatASubscriptionWasAdded();

   for(uint8_t i = 0; i < 10; i++)
   {
      APublicationShouldBeReceivedWithArgs(NULL);
      After(1);
   }

   AfterUnsubscribing();

   NothingShouldHappen();
   After(1);
}

TEST(Event_OneMillisecondTimer, ShouldNotAllowMoreThanOneSubscription)
{
   GivenThatTheEventHasBeenInitialized();
   GivenThatASubscriptionWasAdded();
   CHECK_ASSERTION_FAILED(WhenAnotherSubscriptionIsAdded());
}

TEST(Event_OneMillisecondTimer, ShouldAllowANewSubscriptionAfterOneIsRemoved)
{
   GivenThatTheEventHasBeenInitialized();
   GivenThatASubscriptionWasAdded();

   AfterUnsubscribing();

   NothingShouldHappen();
   WhenAnotherSubscriptionIsAdded();
}
