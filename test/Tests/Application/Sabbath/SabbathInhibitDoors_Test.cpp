/*!
 * @file
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SabbathInhibitDoors.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   Closed = false,
   Opened = true
};

enum
{
   SabbathGpioDelayInMilliseconds = 100
};

static const SabbathDoorOverridePair_t doorOverridePairs[] = {
   {
      .doorStatusOverrideRequestErd = Erd_RightSideFreshFoodDoorStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_RightSideFreshFoodDoorStatus_SabbathOverrideValue,
   },
   {
      .doorStatusOverrideRequestErd = Erd_LeftSideFreezerDoorStatus_SabbathOverrideRequest,
      .doorStatusOverrideValueErd = Erd_LeftSideFreezerDoorStatus_SabbathOverrideValue,
   },
};

static const SabbathInhibitDoorsConfiguration_t config = {
   .doorOverrideErdPairList = doorOverridePairs,
   .numberOfPairs = NUM_ELEMENTS(doorOverridePairs),
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .sabbathGpioErd = Erd_Gpio_SABBATH
};

TEST_GROUP(SabbathInhibitDoors)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   SabbathInhibitDoors_t instance;
   EventSubscription_t subscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);

      mock().strictOrder();
   }

   static void DataModelChanged(void *context, const void *args)
   {
      (void)context;
      const DataModelOnDataChangeArgs_t *onChangeData = (const DataModelOnDataChangeArgs_t *)args;
      Erd_t erd = onChangeData->erd;

      if(erd == Erd_Gpio_SABBATH)
      {
         const bool *state = (const bool *)onChangeData->data;

         mock().actualCall("SabbathGpioErdChanged").withParameter("state", *state);
      }
      else if(erd == Erd_LeftSideFreezerDoorStatus_SabbathOverrideRequest)
      {
         const bool *state = (const bool *)onChangeData->data;

         mock().actualCall("LeftSideFreezerDoorStatusOverrideRequest").withParameter("state", *state);
      }
      else if(erd == Erd_RightSideFreshFoodDoorStatus_SabbathOverrideRequest)
      {
         const bool *state = (const bool *)onChangeData->data;

         mock().actualCall("RightSideFreshFoodDoorStatusOverrideRequest").withParameter("state", *state);
      }
   }

   void GivenDataModelSubscriptionHasBeenInitializedAndSubscribedTo()
   {
      EventSubscription_Init(
         &subscription,
         NULL,
         DataModelChanged);

      DataModel_SubscribeAll(
         dataModel,
         &subscription);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenTheSabbathInhibitDoorsIsInitialized()
   {
      SabbathInhibitDoors_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_SabbathModeEnable,
         &state);
   }

   void GivenEnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_EnhancedSabbathModeEnable,
         &state);
   }

   void WhenEnhancedSabbathModeBecomes(bool state)
   {
      GivenEnhancedSabbathModeIs(state);
   }

   void WhenSabbathModeBecomes(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void GivenAllOverrideRequestsAre(bool state)
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(doorOverridePairs); i++)
      {
         DataModel_Write(
            dataModel,
            doorOverridePairs[i].doorStatusOverrideRequestErd,
            &state);
      }
   }

   void GivenAllOverrideValuesAre(bool state)
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(doorOverridePairs); i++)
      {
         DataModel_Write(
            dataModel,
            doorOverridePairs[i].doorStatusOverrideValueErd,
            &state);
      }
   }

   void AllOverrideRequestsShouldBe(bool expectedState)
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(doorOverridePairs); i++)
      {
         bool actualState;
         DataModel_Read(
            dataModel,
            doorOverridePairs[i].doorStatusOverrideRequestErd,
            &actualState);

         CHECK_EQUAL(expectedState, actualState);
      }
   }

   void AllOverrideValuesShouldBe(bool expectedState)
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(doorOverridePairs); i++)
      {
         bool actualState;
         DataModel_Read(
            dataModel,
            doorOverridePairs[i].doorStatusOverrideValueErd,
            &actualState);

         CHECK_EQUAL(expectedState, actualState);
      }
   }

   void GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled()
   {
      GivenEnhancedSabbathModeIs(DISABLED);
      GivenSabbathModeIs(DISABLED);
      GivenTheSabbathInhibitDoorsIsInitialized();
   }

   void GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreEnabled()
   {
      GivenEnhancedSabbathModeIs(ENABLED);
      GivenSabbathModeIs(ENABLED);
      GivenTheSabbathInhibitDoorsIsInitialized();
   }

   void GivenTheModuleIsInitalizedAndNormalSabbathIsEnabledAndEnhancedSabbathIsDisabled()
   {
      GivenEnhancedSabbathModeIs(DISABLED);
      GivenSabbathModeIs(ENABLED);
      GivenTheSabbathInhibitDoorsIsInitialized();
   }

   void GivenSabbathGpioIs(bool state)
   {
      DataModel_Write(dataModel, config.sabbathGpioErd, &state);
   }

   void SabbathGpioShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, config.sabbathGpioErd, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void SabbathGpioShouldBeSet()
   {
      mock().expectOneCall("SabbathGpioErdChanged").withParameter("state", true);
   }

   void SabbathGpioShouldBeCleared()
   {
      mock().expectOneCall("SabbathGpioErdChanged").withParameter("state", false);
   }

   void OverrideRequestsShouldBeSet()
   {
      mock().expectOneCall("RightSideFreshFoodDoorStatusOverrideRequest").withParameter("state", true);
      mock().expectOneCall("LeftSideFreezerDoorStatusOverrideRequest").withParameter("state", true);
   }

   void OverrideRequestsShouldBeCleared()
   {
      mock().expectOneCall("RightSideFreshFoodDoorStatusOverrideRequest").withParameter("state", false);
      mock().expectOneCall("LeftSideFreezerDoorStatusOverrideRequest").withParameter("state", false);
   }
};

TEST(SabbathInhibitDoors, ShouldSetOverrideRequestsIfInSabbathModeOnInitialization)
{
   GivenSabbathModeIs(ENABLED);
   GivenAllOverrideRequestsAre(CLEAR);
   GivenTheSabbathInhibitDoorsIsInitialized();

   AllOverrideRequestsShouldBe(SET);
}

TEST(SabbathInhibitDoors, ShouldSetOverrideValuesToClosedIfInSabbathModeOnInitialization)
{
   GivenEnhancedSabbathModeIs(ENABLED);
   GivenAllOverrideValuesAre(Opened);
   GivenTheSabbathInhibitDoorsIsInitialized();

   AllOverrideValuesShouldBe(Closed);
}

TEST(SabbathInhibitDoors, ShouldNotSetOverrideRequestsOrValuesWhenInitializedNotInSabbath)
{
   GivenSabbathModeIs(DISABLED);
   GivenAllOverrideRequestsAre(CLEAR);
   GivenAllOverrideValuesAre(Opened);
   GivenTheSabbathInhibitDoorsIsInitialized();

   AllOverrideRequestsShouldBe(CLEAR);
   AllOverrideValuesShouldBe(Opened);
}

TEST(SabbathInhibitDoors, ShouldSetOverrideErdsWhenRegularSabbathIsEntered)
{
   GivenTheSabbathInhibitDoorsIsInitialized();
   GivenAllOverrideValuesAre(Opened);
   GivenAllOverrideRequestsAre(CLEAR);

   WhenSabbathModeBecomes(SET);
   AllOverrideValuesShouldBe(Closed);
   AllOverrideRequestsShouldBe(SET);
}

TEST(SabbathInhibitDoors, ShouldSetOverrideErdsWhenEnhancedSabbathIsEntered)
{
   GivenTheSabbathInhibitDoorsIsInitialized();
   GivenAllOverrideValuesAre(Opened);
   GivenAllOverrideRequestsAre(CLEAR);

   WhenEnhancedSabbathModeBecomes(SET);
   AllOverrideValuesShouldBe(Closed);
   AllOverrideRequestsShouldBe(SET);
}

TEST(SabbathInhibitDoors, ShouldClearSabbathGpioWhenEitherSabbathOrEnhancedSabbathAreEntered)
{
   GivenSabbathGpioIs(CLEAR);
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled();
   SabbathGpioShouldBe(SET);

   WhenSabbathModeBecomes(ENABLED);
   SabbathGpioShouldBe(CLEAR);

   WhenSabbathModeBecomes(DISABLED);
   SabbathGpioShouldBe(SET);

   WhenEnhancedSabbathModeBecomes(ENABLED);
   SabbathGpioShouldBe(CLEAR);
}

TEST(SabbathInhibitDoors, ShouldSetSabbathGpioWhenBothSabbathAndEnhancedSabbathAreCleared)
{
   GivenSabbathGpioIs(SET);
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreEnabled();
   SabbathGpioShouldBe(CLEAR);

   WhenSabbathModeBecomes(DISABLED);
   SabbathGpioShouldBe(CLEAR);

   WhenEnhancedSabbathModeBecomes(DISABLED);
   SabbathGpioShouldBe(SET);
}

TEST(SabbathInhibitDoors, ShouldVoteTheDoorStatesClosedBeforeClearingGpioWhenSabbathIsEnabled)
{
   GivenSabbathGpioIs(SET);
   GivenAllOverrideRequestsAre(CLEAR);
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled();
   GivenDataModelSubscriptionHasBeenInitializedAndSubscribedTo();

   OverrideRequestsShouldBeSet();
   SabbathGpioShouldBeCleared();
   WhenSabbathModeBecomes(ENABLED);
}

TEST(SabbathInhibitDoors, ShouldSetGpioWhenSabbathIsDisabledAndClearOverrideRequestsAfterSabbathGpioDelay)
{
   GivenSabbathGpioIs(CLEAR);
   GivenAllOverrideRequestsAre(CLEAR);
   GivenTheModuleIsInitalizedAndNormalSabbathIsEnabledAndEnhancedSabbathIsDisabled();
   GivenDataModelSubscriptionHasBeenInitializedAndSubscribedTo();

   SabbathGpioShouldBeSet();
   WhenSabbathModeBecomes(DISABLED);

   OverrideRequestsShouldBeCleared();
   After(SabbathGpioDelayInMilliseconds);
}
