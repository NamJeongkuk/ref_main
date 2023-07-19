/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Input_EnhancedSabbathRandomAdditionalMinute.h"
#include "Constants_Time.h"
#include "Timer.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

enum
{
   OneMinuteInMsec = 1 * MSEC_PER_MIN,
   NumberOfChecks = 150
};

TEST_GROUP(Input_EnhancedSabbathRandomAdditionalMinute)
{
   Input_EnhancedSabbathRandomAdditionalMinute_t instance;

   void GivenInitialization()
   {
      Input_EnhancedSabbathRandomAdditionalMinute_Init(&instance);
   }
};

TEST(Input_EnhancedSabbathRandomAdditionalMinute, GeneratedTimeShouldNotExceedOneMinuteInMsec)
{
   GivenInitialization();

   TimerTicks_t currentTime;

   for(uint8_t i = 0; i < NumberOfChecks; i++)
   {
      Input_Read(&instance.interface, &currentTime);

      CHECK(currentTime <= OneMinuteInMsec);
   }
}
