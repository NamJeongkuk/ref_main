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
   Erd_LeftSideFreezerDoorStatusResolved,
   Erd_RightSideFreshFoodDoorStatusResolved
};

static LightingDoorVoteResolverConfig_t config = {
   .timerModuleErd = Erd_TimerModule,
   .rampingPwmDutyCyclePercentageErd = Erd_FreshFoodBackWallLight_DoorVote,
   .doorIsOpenErds = doorIsOpenErds,
   .numberOfDoorErds = NUM_ELEMENTS(doorIsOpenErds),
};

static RampingPwmDutyCyclePercentageBundleData_t normalBundle = {
   .maxPwmDutyCyclePercentage = 100,
   .minPwmDutyCyclePercentage = 20,
   .maxPwmRampingUpCountInMsec = 15,
   .maxPwmRampingDownCountInMsec = 15
};

static const DoorLightingData_t doorLightingData = {
   .normalOperationRampingPwmDutyCycle = &normalBundle
};

static const LightingData_t lightingData = {
   .maximumLightDutyCyclePercentage = 100,
   .maximumCompartmentLightOnTimeInMinutes = 15,
   .freshFoodBackWallDoorLightingData = &doorLightingData,
   .freshFoodTopAndSideDoorLightingData = &doorLightingData,
   .freezerBackWallDoorLightingData = &doorLightingData,
   .freezerTopAndSideDoorLightingData = &doorLightingData
};

TEST_GROUP(LightingDoorVoteResolver)
{
   LightingDoorVoteResolver_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   EventSubscription_t dataModelOnChangeSubscription;

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
         &config,
         lightingData.freshFoodBackWallDoorLightingData);
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

   void TheRampingUpCountShouldChangeTo(uint8_t rampingUpCountInMsec)
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

   void TheRampingDownCountShouldChangeTo(uint8_t rampingDownCountInMsec)
   {
      mock().expectOneCall("RampingDownCountChanged").withParameter("count", rampingDownCountInMsec);
   }

   static void PwmDutyCycleChanged(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_FreshFoodBackWallLight_DoorVote)
      {
         const RampingPwmDutyCyclePercentageVote_t *vote = (const RampingPwmDutyCyclePercentageVote_t *)args->data;

         mock().actualCall("PwmDutyCycleChanged").withParameter("vote", vote->rampingPwmDutyCyclePercentage.pwmDutyCyclePercentage);
      }
   }

   void GivenPwmDutyCycleSubscriptionIsInstalled()
   {
      EventSubscription_Init(
         &dataModelOnChangeSubscription,
         NULL,
         PwmDutyCycleChanged);

      DataModel_SubscribeAll(
         dataModel,
         &dataModelOnChangeSubscription);
   }

   void ThePwmDutyCycleShouldChangeTo(PercentageDutyCycle_t percentageDutyCycle)
   {
      mock().expectOneCall("PwmDutyCycleChanged").withParameter("vote", percentageDutyCycle);
   }

   void GivenTheDoorIs(Erd_t doorErd, bool state)
   {
      DataModel_Write(dataModel, doorErd, &state);
   }

   void WhenTheDoorStateChangesTo(Erd_t doorErd, bool state)
   {
      GivenTheDoorIs(doorErd, state);
   }

   void ThePwmVotedDutyCycleErdShouldBe(PwmDutyCycle_t expected)
   {
      RampingPwmDutyCyclePercentageVote_t freshFoodBackWallLightActual;
      DataModel_Read(dataModel, Erd_FreshFoodBackWallLight_DoorVote, &freshFoodBackWallLightActual);

      CHECK_EQUAL(expected, freshFoodBackWallLightActual.rampingPwmDutyCyclePercentage.pwmDutyCyclePercentage);
      CHECK_TRUE(freshFoodBackWallLightActual.care);
   }

   void TheRampingUpCountInMsecShouldBe(uint8_t expected)
   {
      RampingPwmDutyCyclePercentageVote_t freshFoodBackWallLightActual;
      DataModel_Read(dataModel, Erd_FreshFoodBackWallLight_DoorVote, &freshFoodBackWallLightActual);

      CHECK_EQUAL(expected, freshFoodBackWallLightActual.rampingPwmDutyCyclePercentage.rampingUpCountInMsec);
      CHECK_TRUE(freshFoodBackWallLightActual.care);
   }

   void TheRampingDownCountInMsecShouldBe(uint8_t expected)
   {
      RampingPwmDutyCyclePercentageVote_t freshFoodBackWallLightActual;
      DataModel_Read(dataModel, Erd_FreshFoodBackWallLight_DoorVote, &freshFoodBackWallLightActual);

      CHECK_EQUAL(expected, freshFoodBackWallLightActual.rampingPwmDutyCyclePercentage.rampingDownCountInMsec);
      CHECK_TRUE(freshFoodBackWallLightActual.care);
   }

   void GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax()
   {
      GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
      GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
      GivenInitialization();

      ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);

      WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);
      ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);
   }

   void GivenOneDoorIsOpenAndRampingUpCountInMsecIsMaxRampingUpCountInMsec()
   {
      GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
      GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
      GivenInitialization();
      GivenRampingUpCountSubscriptionIsInstalled();

      TheRampingUpCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

      TheRampingUpCountShouldChangeTo(UINT8_MAX);
      TheRampingUpCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
      WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);
   }

   void GivenOneDoorIsOpenAndRampingDownCountInMsecIsMaxRampingDownCountInMsec()
   {
      GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
      GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
      GivenInitialization();
      GivenRampingDownCountSubscriptionIsInstalled();

      TheRampingDownCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

      TheRampingDownCountShouldChangeTo(UINT8_MAX);
      TheRampingDownCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
      WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);
   }

   void NothingShouldHappen()
   {
   }
};

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdToMinIfAllDoorsAreClosedOnInitialization)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdToParametricNormalOperationMaxPwmDutyCyclePercentageIfOneDoorIsOpenOnInitialization)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, true);
   GivenInitialization();

   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToParametricMaxPwmRampingUpCountInMsecIfAllDoorsAreClosedOnInitialization)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   TheRampingUpCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToUint8MaxThenParametricMaxPwmRampingUpCountInMsecIfOneDoorIsOpenOnInitialization)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, true);
   GivenRampingUpCountSubscriptionIsInstalled();

   TheRampingUpCountShouldChangeTo(UINT8_MAX);
   TheRampingUpCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   GivenInitialization();
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToParametricMaxPwmRampingDownCountInMsecIfAllDoorsAreClosedOnInitialization)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   TheRampingDownCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToUint8MaxThenParametricMaxPwmRampingDownCountInMsecIfOneDoorIsOpenOnInitialization)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, true);
   GivenRampingDownCountSubscriptionIsInstalled();

   TheRampingDownCountShouldChangeTo(UINT8_MAX);
   TheRampingDownCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   GivenInitialization();
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdToParametricMaxPwmDutyCycleWhenOneDoorOpens)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdToParametricMaxPwmDutyCycleWhenOneDoorOpensAndThenMinWhenAllDoorsAreClosed)
{
   GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax();

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, false);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, false);
   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToUint8MaxThenParametricMaxPwmRampingUpCountInMsecWhenOneDoorOpens)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();
   GivenRampingUpCountSubscriptionIsInstalled();

   TheRampingUpCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

   TheRampingUpCountShouldChangeTo(UINT8_MAX);
   TheRampingUpCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToUint8MaxThenParametricMaxPwmRampingUpCountInMsecWhenOneDoorOpensAndSetTheSameValueWhenAllDoorsAreClosed)
{
   GivenOneDoorIsOpenAndRampingUpCountInMsecIsMaxRampingUpCountInMsec();

   NothingShouldHappen();
   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, false);
   TheRampingDownCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

   TheRampingUpCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, false);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToUint8MaxThenParametricMaxPwmRampingDownCountInMsecWhenOneDoorOpens)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();
   GivenRampingDownCountSubscriptionIsInstalled();

   TheRampingDownCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

   TheRampingDownCountShouldChangeTo(UINT8_MAX);
   TheRampingDownCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToUint8MaxThenParametricMaxPwmRampingDownCountInMsecWhenOneDoorOpensAndSetTheSameValueWhenAllDoorsAreClosed)
{
   GivenOneDoorIsOpenAndRampingDownCountInMsecIsMaxRampingDownCountInMsec();

   NothingShouldHappen();
   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, false);
   TheRampingDownCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

   TheRampingDownCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, false);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdToMinWhenAnyDoorIsOpenForTheMaxDoorOpenTime)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   After(lightingData.maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   After(1);
   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToParametricMaxPwmRampingUpCountInMsecWhenAnyDoorIsOpenForTheMaxDoorOpenTime)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();
   GivenRampingUpCountSubscriptionIsInstalled();

   TheRampingUpCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

   TheRampingUpCountShouldChangeTo(UINT8_MAX);
   TheRampingUpCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);

   After(lightingData.maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheRampingUpCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

   TheRampingUpCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   After(1);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToParametricMaxPwmRampingDownCountInMsecWhenAnyDoorIsOpenForTheMaxDoorOpenTime)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();
   GivenRampingDownCountSubscriptionIsInstalled();

   TheRampingDownCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

   TheRampingDownCountShouldChangeTo(UINT8_MAX);
   TheRampingDownCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);

   After(lightingData.maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheRampingDownCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

   TheRampingDownCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   After(1);
}

TEST(LightingDoorVoteResolver, ShouldResetMaxDoorOpenTimeWhenAnotherDoorOpensWhileADifferentAssociatedDoorIsOpen)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   After(lightingData.maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);
   After(1);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   After(lightingData.maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetPwmVotedDutyCycleErdToMinWhenOneDoorOpenOnInitializationAfterMaxDoorOpenTime)
{
   GivenOneDoorIsOpenAndPwmVotedDutyCycleIsMax();

   After(lightingData.maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   After(1);
   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingUpCountInMsecToParametricMaxPwmRampingUpCountInMsecWhenOneDoorOpenOnInitializationAfterMaxDoorOpenTime)
{
   GivenOneDoorIsOpenAndRampingUpCountInMsecIsMaxRampingUpCountInMsec();

   After(lightingData.maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheRampingUpCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);

   TheRampingUpCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   After(1);
}

TEST(LightingDoorVoteResolver, ShouldSetRampingDownCountInMsecToParametricMaxPwmRampingDownCountInMsecWhenOneDoorOpenOnInitializationAfterMaxDoorOpenTime)
{
   GivenOneDoorIsOpenAndRampingDownCountInMsecIsMaxRampingDownCountInMsec();

   After(lightingData.maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheRampingDownCountInMsecShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);

   TheRampingDownCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   After(1);
}

TEST(LightingDoorVoteResolver, ShouldNotRedoRampingLightOnWhenADoorOpensAndAnotherAssociatedDoorIsAlreadyOpenForRampingDownCount)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();
   GivenRampingDownCountSubscriptionIsInstalled();

   TheRampingDownCountShouldChangeTo(UINT8_MAX);
   TheRampingDownCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingDownCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);

   NothingShouldHappen();
   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);
}

TEST(LightingDoorVoteResolver, ShouldNotRedoRampingLightOnWhenADoorOpensAndAnotherAssociatedDoorIsAlreadyOpenForRampingUpCount)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();
   GivenRampingUpCountSubscriptionIsInstalled();

   TheRampingUpCountShouldChangeTo(UINT8_MAX);
   TheRampingUpCountShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmRampingUpCountInMsec);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);

   NothingShouldHappen();
   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);
}

TEST(LightingDoorVoteResolver, ShouldNotRedoRampingLightOnWhenADoorOpensAndAnotherAssociatedDoorIsAlreadyOpen)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();
   GivenPwmDutyCycleSubscriptionIsInstalled();

   ThePwmDutyCycleShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->minPwmDutyCyclePercentage);
   ThePwmDutyCycleShouldChangeTo(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);
   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);

   NothingShouldHappen();
   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);
}

TEST(LightingDoorVoteResolver, ShouldNotTurnLightsOnAfterTheMaxOpenDoorTimeUntilAllDoorsAreClosedAndThenADoorOpens)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, false);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, false);
   GivenInitialization();

   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   After(lightingData.maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   After(1);
   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);

   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, true);
   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, false);
   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, false);

   WhenTheDoorStateChangesTo(Erd_LeftSideFreezerDoorStatusResolved, true);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);
}

TEST(LightingDoorVoteResolver, ShouldTurnOffLightsAfterMaxOpenDoorTimeAsLongAsOneDoorIsStillOpen)
{
   GivenTheDoorIs(Erd_LeftSideFreezerDoorStatusResolved, true);
   GivenTheDoorIs(Erd_RightSideFreshFoodDoorStatusResolved, true);
   GivenInitialization();

   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   After(lightingData.maximumCompartmentLightOnTimeInMinutes * MSEC_PER_MIN - 1);
   WhenTheDoorStateChangesTo(Erd_RightSideFreshFoodDoorStatusResolved, false);
   ThePwmVotedDutyCycleErdShouldBe(lightingData.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle->maxPwmDutyCyclePercentage);

   After(1);
   ThePwmVotedDutyCycleErdShouldBe(PercentageDutyCycle_Min);
}
