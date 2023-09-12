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
#include "ReferDataModel_TestDouble.h"

enum
{
   Closed = false,
   Opened = true
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

static bool testPassed;

TEST_GROUP(SabbathInhibitDoors)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   SabbathInhibitDoors_t instance;
   EventSubscription_t subscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      testPassed = false;
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
      for(uint8_t index; index < NUM_ELEMENTS(doorOverridePairs); index++)
      {
         DataModel_Write(
            dataModel,
            doorOverridePairs[index].doorStatusOverrideRequestErd,
            &state);
      }
   }

   void GivenAllOverrideValuesAre(bool state)
   {
      for(uint8_t index; index < NUM_ELEMENTS(doorOverridePairs); index++)
      {
         DataModel_Write(
            dataModel,
            doorOverridePairs[index].doorStatusOverrideValueErd,
            &state);
      }
   }

   void AllOverrideRequestsShouldBe(bool expectedState)
   {
      bool actualState;
      for(uint8_t index; index < NUM_ELEMENTS(doorOverridePairs); index++)
      {
         DataModel_Read(
            dataModel,
            doorOverridePairs[index].doorStatusOverrideRequestErd,
            &actualState);

         CHECK_EQUAL(expectedState, actualState);
      }
   }

   void AllOverrideValuesShouldBe(bool expectedState)
   {
      bool actualState;
      for(uint8_t index; index < NUM_ELEMENTS(doorOverridePairs); index++)
      {
         DataModel_Read(
            dataModel,
            doorOverridePairs[index].doorStatusOverrideValueErd,
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

   static void DataModelChangedSabbathGpioShouldBeSetWhenDoorIsResolvedToClosed(void *context, const void *_args)
   {
      REINTERPRET(dataModel, context, I_DataModel_t *);
      REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);

      if(args->erd == doorOverridePairs[0].doorStatusOverrideRequestErd)
      {
         bool gpioState;
         DataModel_Read(
            dataModel,
            config.sabbathGpioErd,
            &gpioState);

         testPassed = gpioState ? true : false;
      }
   }

   void SabbathGpioShouldBeSetWhenResolvedDoorsAreClosedUponEnteringSabbath(void)
   {
      EventSubscription_Init(
         &subscription,
         dataModel,
         DataModelChangedSabbathGpioShouldBeSetWhenDoorIsResolvedToClosed);
      Event_Subscribe(dataModel->OnDataChange, &subscription);

      WhenSabbathModeBecomes(ENABLED);
      CHECK_TRUE(testPassed);
   }

   static void DataModelChangedOverrideErdShouldBeSetWhenSabbathGpioIsSet(void *context, const void *_args)
   {
      REINTERPRET(dataModel, context, I_DataModel_t *);
      REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);

      if(args->erd == config.sabbathGpioErd)
      {
         bool request;
         testPassed = true;

         for(uint8_t index = 0; index < config.numberOfPairs; index++)
         {
            DataModel_Read(
               dataModel,
               doorOverridePairs[index].doorStatusOverrideRequestErd,
               &request);

            if(request == CLEAR)
            {
               testPassed = false;
               return;
            }
         }
      }
   }

   void ResolvedDoorsShouldRemainClosedWhenSabbathGpioIsSetUponDisablingSabbath(void)
   {
      EventSubscription_Init(
         &subscription,
         dataModel,
         DataModelChangedOverrideErdShouldBeSetWhenSabbathGpioIsSet);
      Event_Subscribe(dataModel->OnDataChange, &subscription);

      WhenSabbathModeBecomes(DISABLED);
      WhenEnhancedSabbathModeBecomes(DISABLED);
      CHECK_TRUE(testPassed);
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

TEST(SabbathInhibitDoors, ShouldVoteTheDoorStatesClosedBeforeClearingGpio)
{
   GivenSabbathGpioIs(SET);
   GivenAllOverrideRequestsAre(CLEAR);
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreDisabled();

   SabbathGpioShouldBeSetWhenResolvedDoorsAreClosedUponEnteringSabbath();
}

TEST(SabbathInhibitDoors, ShouldSetGpioBeforeVotingForTheDoorStates)
{
   GivenAllOverrideRequestsAre(CLEAR);
   GivenTheModuleIsInitalizedAndNormalSabbathAndEnhancedSabbathAreEnabled();

   ResolvedDoorsShouldRemainClosedWhenSabbathGpioIsSetUponDisablingSabbath();
}
