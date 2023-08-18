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
#include "PersonalityParametricData.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

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
   .rampingPwmDutyCyclePercentageErds = pwmVotedDutyCycleErds,
   .numberOfDoorErds = NUM_ELEMENTS(doorIsOpenErds),
   .numberOfRampingPwmDutyCyclePercentageErds = NUM_ELEMENTS(pwmVotedDutyCycleErds),
};

TEST_GROUP(LightingDoorVoteResolver)
{
   LightingDoorVoteResolver_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const LightingData_t *lightingData;
   EventSubscription_t dataModelOnChangeSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      lightingData = PersonalityParametricData_Get(dataModel)->lightingData;
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
         &config,
         lightingData->freezerBackWallDoorLightingData);
   }

   static void RampingUpCountChanged(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_FreshFoodBackWallLight_DoorVote)
      {
         const RampingPwmDutyCyclePercentageVote_t *count = (const RampingPwmDutyCyclePercentageVote_t *)args->data;

         mock().actualCall("RampingUpCountChanged").withParameter("count", count->rampingPwmDutyCyclePercentage.rampingUpCountInMsec);
      }
   }

   void GivenRampingUpCountSubscriptionIsInstalled()
   {
      EventSubscription_Init(
         &dataModelOnChangeSubscription,
         NULL,
         RampingUpCountChanged);

      DataModel_SubscribeAll(
         dataModel,
         &dataModelOnChangeSubscription);
   }

   void TheRampingUpCountShouldChangeTo(u_int8_t rampingUpCountInMsec)
   {
      mock().expectOneCall("RampingUpCountChanged").withParameter("count", rampingUpCountInMsec);
   }

   static void RampingDownCountChanged(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_FreshFoodBackWallLight_DoorVote)
      {
         const RampingPwmDutyCyclePercentageVote_t *count = (const RampingPwmDutyCyclePercentageVote_t *)args->data;

         mock().actualCall("RampingDownCountChanged").withParameter("count", count->rampingPwmDutyCyclePercentage.rampingDownCountInMsec);
      }
   }

   void GivenRampingDownCountSubscriptionIsInstalled()
   {
      EventSubscription_Init(
         &dataModelOnChangeSubscription,
         NULL,
         RampingDownCountChanged);

      DataModel_SubscribeAll(
         dataModel,
         &dataModelOnChangeSubscription);
   }

   void TheRampingDownCountShouldChangeTo(u_int8_t rampingDownCountInMsec)
   {
      mock().expectOneCall("RampingDownCountChanged").withParameter("count", rampingDownCountInMsec);
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
      RampingPwmDutyCyclePercentageVote_t freshFoodBackWallLightActual;
      RampingPwmDutyCyclePercentageVote_t freshFoodTopLightActual;
      DataModel_Read(dataModel, Erd_FreshFoodBackWallLight_DoorVote, &freshFoodBackWallLightActual);
      DataModel_Read(dataModel, Erd_FreshFoodTopLight_DoorVote, &freshFoodTopLightActual);

      CHECK_EQUAL(expected, freshFoodBackWallLightActual.rampingPwmDutyCyclePercentage.pwmDutyCyclePercentage);
      CHECK_EQUAL(expected, freshFoodTopLightActual.rampingPwmDutyCyclePercentage.pwmDutyCyclePercentage);
      CHECK_TRUE(freshFoodBackWallLightActual.care);
      CHECK_TRUE(freshFoodTopLightActual.care);
   }

   void TheRampingUpCountInMsecShouldBe(uint8_t expected)
   {
      RampingPwmDutyCyclePercentageVote_t freshFoodBackWallLightActual;
      RampingPwmDutyCyclePercentageVote_t freshFoodTopLightActual;
      DataModel_Read(dataModel, Erd_FreshFoodBackWallLight_DoorVote, &freshFoodBackWallLightActual);
      DataModel_Read(dataModel, Erd_FreshFoodTopLight_DoorVote, &freshFoodTopLightActual);

      CHECK_EQUAL(expected, freshFoodBackWallLightActual.rampingPwmDutyCyclePercentage.rampingUpCountInMsec);
      CHECK_EQUAL(expected, freshFoodTopLightActual.rampingPwmDutyCyclePercentage.rampingUpCountInMsec);
      CHECK_TRUE(freshFoodBackWallLightActual.care);
      CHECK_TRUE(freshFoodTopLightActual.care);
   }

   void TheRampingDownCountInMsecShouldBe(uint8_t expected)
   {
      RampingPwmDutyCyclePercentageVote_t freshFoodBackWallLightActual;
      RampingPwmDutyCyclePercentageVote_t freshFoodTopLightActual;
      DataModel_Read(dataModel, Erd_FreshFoodBackWallLight_DoorVote, &freshFoodBackWallLightActual);
      DataModel_Read(dataModel, Erd_FreshFoodTopLight_DoorVote, &freshFoodTopLightActual);

      CHECK_EQUAL(expected, freshFoodBackWallLightActual.rampingPwmDutyCyclePercentage.rampingDownCountInMsec);
      CHECK_EQUAL(expected, freshFoodTopLightActual.rampingPwmDutyCyclePercentage.rampingDownCountInMsec);
      CHECK_TRUE(freshFoodBackWallLightActual.care);
      CHECK_TRUE(freshFoodTopLightActual.care);
   }

   void GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax()
   {
      GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
      GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
      GivenInitialization();

      ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Min);

      WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, true);
      ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Max);
   }

   void GivenOneDoorIsOpenAndRampingUpCountInMsecIsMaxRampingUpCountInMsec()
   {
      GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
      GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
      GivenInitialization();
      GivenRampingUpCountSubscriptionIsInstalled();

      TheRampingUpCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

      TheRampingUpCountShouldChangeTo(UINT8_MAX);
      TheRampingUpCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
      WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, true);
   }

   void GivenOneDoorIsOpenAndRampingDownCountInMsecIsMaxRampingDownCountInMsec()
   {
      GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
      GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
      GivenInitialization();
      GivenRampingDownCountSubscriptionIsInstalled();

      TheRampingDownCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

      TheRampingDownCountShouldChangeTo(UINT8_MAX);
      TheRampingDownCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
      WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, true);
   }
};

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMinIfAllDoorsAreClosedOnInitialization)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMaxIfOneDoorIsOpenOnInitialization)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, true);
   GivenInitialization();

   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Max);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToMaxPwmRampingUpCountInMsecIfAllDoorsAreClosedOnInitalization)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   TheRampingUpCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToMaxPwmRampingUpCountInMsecIfOneDoorIsOpenOnInitalization)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, true);
   GivenRampingUpCountSubscriptionIsInstalled();

   TheRampingUpCountShouldChangeTo(UINT8_MAX);
   TheRampingUpCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   GivenInitialization();
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToMaxPwmRampingDownCountInMsecIfAllDoorsAreClosedOnInitalization)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   TheRampingDownCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToMaxPwmRampingDownCountInMsecIfOneDoorIsOpenOnInitalization)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, true);
   GivenRampingDownCountSubscriptionIsInstalled();

   TheRampingDownCountShouldChangeTo(UINT8_MAX);
   TheRampingDownCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   GivenInitialization();
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMaxWhenOneDoorOpens)
{
   GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax();
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMaxWhenOneDoorOpensAndThenMinWhenAllDoorsAreClosed)
{
   GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax();

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Max);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, false);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Max);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToMaxPwmRampingUpCountInMsecWhenOneDoorOpens)
{
   GivenOneDoorIsOpenAndRampingUpCountInMsecIsMaxRampingUpCountInMsec();
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToMaxPwmRampingUpCountInMsecWhenOneDoorOpensAndSetTheSameValueWhenAllDoorsAreClosed)
{
   GivenOneDoorIsOpenAndRampingUpCountInMsecIsMaxRampingUpCountInMsec();

   TheRampingUpCountShouldChangeTo(UINT8_MAX);
   TheRampingUpCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, false);
   TheRampingDownCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

   TheRampingUpCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToMaxPwmRampingDownCountInMsecWhenOneDoorOpens)
{
   GivenOneDoorIsOpenAndRampingDownCountInMsecIsMaxRampingDownCountInMsec();
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToMaxPwmRampingDownCountInMsecWhenOneDoorOpensAndSetTheSameValueWhenAllDoorsAreClosed)
{
   GivenOneDoorIsOpenAndRampingDownCountInMsecIsMaxRampingDownCountInMsec();

   TheRampingDownCountShouldChangeTo(UINT8_MAX);
   TheRampingDownCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, false);
   TheRampingDownCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

   TheRampingDownCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMinWhenAnyDoorIsOpenForTheMaxDoorOpenTime)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Min);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, true);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Max);

   After(lightingData->maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Max);

   After(1);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToMaxPwmRampingUpCountInMsecWhenAnyDoorIsOpenForTheMaxDoorOpenTime)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();
   GivenRampingUpCountSubscriptionIsInstalled();

   TheRampingUpCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

   TheRampingUpCountShouldChangeTo(UINT8_MAX);
   TheRampingUpCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, true);

   After(lightingData->maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheRampingUpCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

   TheRampingUpCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   After(1);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToMaxPwmRampingDownCountInMsecWhenAnyDoorIsOpenForTheMaxDoorOpenTime)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();
   GivenRampingDownCountSubscriptionIsInstalled();

   TheRampingDownCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

   TheRampingDownCountShouldChangeTo(UINT8_MAX);
   TheRampingDownCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, true);

   After(lightingData->maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheRampingDownCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

   TheRampingDownCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   After(1);
}

TEST(LightingDoorVoteResolver, ShouldResetMaxDoorOpenTimeWhenAnotherDoorOpensWhileADifferentDoorIsOpen)
{
   GivenTheDoorIs(Erd_LeftSideFreshFoodDoorIsOpenResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Min);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreshFoodDoorIsOpenResolved, true);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Max);

   After(lightingData->maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Max);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);
   After(1);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Max);

   After(lightingData->maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdsToMinWhenOneDoorOpenOnInitializationAfterMaxDoorOpenTime)
{
   GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax();

   After(lightingData->maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Max);

   After(1);
   ThePwmVotedDutyCycleErdsShouldBe(PercentageDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToMaxPwmRampingUpCountInMsecWhenOneDoorOpenOnInitalizationAfterMaxDoorOpenTime)
{
   GivenOneDoorIsOpenAndRampingUpCountInMsecIsMaxRampingUpCountInMsec();

   After(lightingData->maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheRampingUpCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

   TheRampingUpCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   After(1);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToMaxPwmRampingDownCountInMsecWhenOneDoorOpenOnInitalizationAfterMaxDoorOpenTime)
{
   GivenOneDoorIsOpenAndRampingDownCountInMsecIsMaxRampingDownCountInMsec();

   After(lightingData->maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheRampingDownCountInMsecShouldBe(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

   TheRampingDownCountShouldChangeTo(lightingData->freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   After(1);
}
