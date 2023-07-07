/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "LightingDoorVoteResolver.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   MaxDoorOpenTimeInMsec = 15 * MSEC_PER_MIN
};

static const Erd_t doorIsOpenErds[] = {
   Erd_LeftSideFreshFoodDoorIsOpenResolved,
   Erd_RightSideFreshFoodDoorStatusResolved
};

static const Erd_t pwmVotedDutyCycleErds[] = {
   Erd_FreshFoodBackWallLight_DoorVote,
   Erd_FreshFoodTopLight_DoorVote
};

static LightingDoorVoteResolverConfig_t config = {
   .timerModuleErd = Erd_TimerModule,
   .doorIsOpenErds = doorIsOpenErds,
   .pwmVotedDutyCycleErds = pwmVotedDutyCycleErds,
   .numberOfDoorErds = NUM_ELEMENTS(doorIsOpenErds),
   .numberOfPwmVotedDutyCycleErds = NUM_ELEMENTS(pwmVotedDutyCycleErds),
};

TEST_GROUP(LightingDoorVoteResolver)
{
   LightingDoorVoteResolver_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, 1000);
   }

   void GivenInitialization()
   {
      LightingDoorVoteResolver_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenTheDoorIs(Erd_t doorErd, bool state)
   {
      DataModel_Write(dataModel, doorErd, &state);
   }

   void WhenTheDoorStateChangesTo(Erd_t doorErd, bool state)
   {
      GivenTheDoorIs(doorErd, state);
   }

   void ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_t expected)
   {
      PwmVotedDutyCycle_t freshFoodBackWallLightActual;
      PwmVotedDutyCycle_t freshFoodTopLightActual;
      DataModel_Read(dataModel, Erd_FreshFoodBackWallLight_DoorVote, &freshFoodBackWallLightActual);
      DataModel_Read(dataModel, Erd_FreshFoodTopLight_DoorVote, &freshFoodTopLightActual);

      CHECK_EQUAL(expected, freshFoodBackWallLightActual.pwmDutyCycle);
      CHECK_EQUAL(expected, freshFoodTopLightActual.pwmDutyCycle);
      CHECK_TRUE(freshFoodBackWallLightActual.care);
      CHECK_TRUE(freshFoodTopLightActual.care);
   }

   void GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax()
   {
      GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
      GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
      GivenInitialization();

      ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Min);

      WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, true);
      ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Max);
   }
};

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMinIfAllDoorsAreClosedOnInitialization)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMaxIfOneDoorIsOpenOnInitialization)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, true);
   GivenInitialization();

   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Max);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMaxWhenOneDoorOpens)
{
   GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax();
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMaxWhenOneDoorOpensAndThenMinWhenAllDoorsAreClosed)
{
   GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax();

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Max);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, false);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Max);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMinWhenAnyDoorIsOpenForTheMaxDoorOpenTime)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Min);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, true);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Max);

   After(MaxDoorOpenTimeInMsec - 1);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Max);

   After(1);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldResetMaxDoorOpenTimeWhenAnotherDoorOpensWhileADifferentDoorIsOpen)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Min);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, true);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Max);

   After(MaxDoorOpenTimeInMsec - 1);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Max);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);
   After(1);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Max);

   After(MaxDoorOpenTimeInMsec - 1);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMinWhenOneDoorOpenOnInitializationAfterMaxDoorOpenTime)
{
   GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax();

   After(MaxDoorOpenTimeInMsec - 1);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Max);

   After(1);
   ThePwmVotedDutyCycleErdsShouldBe(PwmDutyCycle_Min);
}
