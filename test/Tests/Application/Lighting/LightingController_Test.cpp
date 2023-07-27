/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "LightingController.h"
#include "ConstArrayMap_LinearSearch.h"
#include "VotedPwmDutyCyclePair.h"
#include "PersonalityParametricData.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   SomePercentageDutyCycle = 4,
   SomeRampingUpCountPerMsec = 15,
   SomeRampingDownCountPerMsec = 20
};

static const VotedPwmDutyCyclePair_t votedPwmPairs[] = {
   { Erd_FreshFoodBackWallLight_ResolvedVote, Erd_FreshFoodBackWallLight_RampingPwm },
   { Erd_FreshFoodTopLight_ResolvedVote, Erd_FreshFoodTopLight_RampingPwm },
   { Erd_FreezerBackWallLight_ResolvedVote, Erd_FreezerBackWallLight_RampingPwm },
   { Erd_FreezerTopLight_ResolvedVote, Erd_FreezerTopLight_RampingPwm }
};

static const ConstArrayMap_LinearSearchConfiguration_t mapConfiguration = {
   votedPwmPairs,
   NUM_ELEMENTS(votedPwmPairs),
   ELEMENT_SIZE(votedPwmPairs),
   MEMBER_SIZE(VotedPwmDutyCyclePair_t, pwmDutyCycleResolvedVoteErd),
   OFFSET_OF(VotedPwmDutyCyclePair_t, pwmDutyCycleResolvedVoteErd)
};

TEST_GROUP(LightingController)
{
   LightingController_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   ConstArrayMap_LinearSearch_t map;
   I_DataModel_t *dataModel;
   RampingPwmDutyCyclePercentage_t rampingPwmDutyCyclePercentage;
   const LightingData_t *lightingData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      lightingData = PersonalityParametricData_Get(dataModel)->lightingData;

      ConstArrayMap_LinearSearch_Init(
         &map,
         &mapConfiguration);
   }

   void GivenInitialization()
   {
      LightingController_Init(
         &instance,
         dataModel,
         &map.interface,
         lightingData);
   }

   void GivenTheVotedPwmDutyCyclePercentageErdIs(Erd_t erd, PercentageDutyCycle_t pwmDutyCycle)
   {
      rampingPwmDutyCyclePercentage.pwmDutyCyclePercentage = pwmDutyCycle;

      RampingPwmDutyCyclePercentageVote_t votedPwmDutyCycle = {
         .rampingPwmDutyCyclePercentage = rampingPwmDutyCyclePercentage,
         .care = Vote_Care
      };

      DataModel_Write(dataModel, erd, &votedPwmDutyCycle);
   }

   void WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_t erd, PercentageDutyCycle_t pwmDutyCycle)
   {
      GivenTheVotedPwmDutyCyclePercentageErdIs(erd, pwmDutyCycle);
   }

   void ThePwmDutyCycleErdShouldBe(Erd_t erd, PwmDutyCycle_t expected)
   {
      RampingPwmDutyCycle_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(expected, actual.pwmDutyCycle);
   }

   PwmDutyCycle_t TheExpectedPwmDutyCycleIs(PercentageDutyCycle_t pwmDutyCyclePercentage)
   {
      PwmDutyCycle_t expectedPwmDutyCycle = (pwmDutyCyclePercentage * UINT16_MAX * lightingData->maximumLightDutyCyclePercentage) / (100 * 100);
      return expectedPwmDutyCycle;
   }

   void GivenTheRampingUpCountPerMsecIs(Erd_t erd, uint16_t rampingUpCountPerMsec)
   {
      rampingPwmDutyCyclePercentage.rampingUpCountInMsec = rampingUpCountPerMsec;

      RampingPwmDutyCyclePercentageVote_t votedPwmDutyCycle = {
         .rampingPwmDutyCyclePercentage = rampingPwmDutyCyclePercentage,
         .care = Vote_Care
      };

      DataModel_Write(dataModel, erd, &votedPwmDutyCycle);
   }

   void GivenTheRampingDownCountPerMsecIs(Erd_t erd, uint16_t rampingDownCountPerMsec)
   {
      rampingPwmDutyCyclePercentage.rampingDownCountInMsec = rampingDownCountPerMsec;

      RampingPwmDutyCyclePercentageVote_t votedPwmDutyCycle = {
         .rampingPwmDutyCyclePercentage = rampingPwmDutyCyclePercentage,
         .care = Vote_Care
      };

      DataModel_Write(dataModel, erd, &votedPwmDutyCycle);
   }

   void WhenTheRampingUpCountPerMsecChangesTo(Erd_t erd, uint16_t rampingUpCounterPerMsec)
   {
      GivenTheRampingUpCountPerMsecIs(erd, rampingUpCounterPerMsec);
   }

   void WhenTheRampingDownCountPerMsecChangesTo(Erd_t erd, uint16_t rampingDownCounterPerMsec)
   {
      GivenTheRampingDownCountPerMsecIs(erd, rampingDownCounterPerMsec);
   }

   void TheRampingUpCountPerMsecShouldBe(Erd_t erd, u_int16_t expected)
   {
      RampingPwmDutyCycle_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(expected, actual.rampingUpCountPerMsec);
   }

   void TheRampingDownCountPerMsecShouldBe(Erd_t erd, u_int16_t expected)
   {
      RampingPwmDutyCycle_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(expected, actual.rampingDownCountPerMsec);
   }
};

TEST(LightingController, AllPwmsShouldBeZeroOnInit)
{
   GivenTheVotedPwmDutyCyclePercentageErdIs(Erd_FreezerTopLight_ResolvedVote, PercentageDutyCycle_Max);
   GivenTheVotedPwmDutyCyclePercentageErdIs(Erd_FreshFoodBackWallLight_ResolvedVote, PercentageDutyCycle_Max);
   GivenTheVotedPwmDutyCyclePercentageErdIs(Erd_FreshFoodTopLight_ResolvedVote, PercentageDutyCycle_Max);
   GivenTheVotedPwmDutyCyclePercentageErdIs(Erd_FreezerBackWallLight_ResolvedVote, PercentageDutyCycle_Max);
   GivenInitialization();

   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_RampingPwm, 0);
}

TEST(LightingController, ShouldOnlyUpdateFreshFoodBackWallLightPwmWhenVotedFor)
{
   GivenInitialization();

   WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_FreshFoodBackWallLight_ResolvedVote, PercentageDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, PwmDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_RampingPwm, 0);

   WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_FreshFoodBackWallLight_ResolvedVote, SomePercentageDutyCycle);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, TheExpectedPwmDutyCycleIs(SomePercentageDutyCycle));
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_RampingPwm, 0);
}

TEST(LightingController, ShouldOnlyUpdateFreshFoodTopLightPwmWhenVotedFor)
{
   GivenInitialization();

   WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_FreshFoodTopLight_ResolvedVote, PercentageDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_RampingPwm, PwmDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_RampingPwm, 0);

   WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_FreshFoodTopLight_ResolvedVote, SomePercentageDutyCycle);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_RampingPwm, TheExpectedPwmDutyCycleIs(SomePercentageDutyCycle));
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_RampingPwm, 0);
}

TEST(LightingController, ShouldOnlyUpdateFreezerBackWallLightPwmWhenVotedFor)
{
   GivenInitialization();

   WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_FreezerBackWallLight_ResolvedVote, PercentageDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_RampingPwm, PwmDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_RampingPwm, 0);

   WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_FreezerBackWallLight_ResolvedVote, SomePercentageDutyCycle);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_RampingPwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_RampingPwm, TheExpectedPwmDutyCycleIs(SomePercentageDutyCycle));
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_RampingPwm, 0);
}

TEST(LightingController, ShouldUpdateAllPwmsIfEverythingIsVotedFor)
{
   GivenInitialization();

   WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_FreshFoodBackWallLight_ResolvedVote, SomePercentageDutyCycle);
   WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_FreshFoodTopLight_ResolvedVote, SomePercentageDutyCycle + 1);
   WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_FreezerBackWallLight_ResolvedVote, SomePercentageDutyCycle + 2);
   WhenVotedPwmDutyCyclePercentageErdChangesTo(Erd_FreezerTopLight_ResolvedVote, SomePercentageDutyCycle + 3);

   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, TheExpectedPwmDutyCycleIs(SomePercentageDutyCycle));
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_RampingPwm, TheExpectedPwmDutyCycleIs(SomePercentageDutyCycle + 1));
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_RampingPwm, TheExpectedPwmDutyCycleIs(SomePercentageDutyCycle + 2));
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_RampingPwm, TheExpectedPwmDutyCycleIs(SomePercentageDutyCycle + 3));
}

TEST(LightingController, AllRampingUpShouldBeZeroOnInit)
{
   GivenTheRampingUpCountPerMsecIs(Erd_FreezerTopLight_ResolvedVote, SomeRampingUpCountPerMsec);
   GivenTheRampingUpCountPerMsecIs(Erd_FreshFoodBackWallLight_ResolvedVote, SomeRampingUpCountPerMsec);
   GivenTheRampingUpCountPerMsecIs(Erd_FreshFoodTopLight_ResolvedVote, SomeRampingUpCountPerMsec);
   GivenTheRampingUpCountPerMsecIs(Erd_FreezerBackWallLight_ResolvedVote, SomeRampingUpCountPerMsec);
   GivenInitialization();

   TheRampingUpCountPerMsecShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, 0);
   TheRampingUpCountPerMsecShouldBe(Erd_FreshFoodTopLight_RampingPwm, 0);
   TheRampingUpCountPerMsecShouldBe(Erd_FreezerBackWallLight_RampingPwm, 0);
   TheRampingUpCountPerMsecShouldBe(Erd_FreezerTopLight_RampingPwm, 0);
}

TEST(LightingController, AllRampingDownShouldBeZeroOnInit)
{
   GivenTheRampingDownCountPerMsecIs(Erd_FreezerTopLight_ResolvedVote, SomeRampingDownCountPerMsec);
   GivenTheRampingDownCountPerMsecIs(Erd_FreshFoodBackWallLight_ResolvedVote, SomeRampingDownCountPerMsec);
   GivenTheRampingDownCountPerMsecIs(Erd_FreshFoodTopLight_ResolvedVote, SomeRampingDownCountPerMsec);
   GivenTheRampingDownCountPerMsecIs(Erd_FreezerBackWallLight_ResolvedVote, SomeRampingDownCountPerMsec);
   GivenInitialization();

   TheRampingDownCountPerMsecShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, 0);
   TheRampingDownCountPerMsecShouldBe(Erd_FreshFoodTopLight_RampingPwm, 0);
   TheRampingDownCountPerMsecShouldBe(Erd_FreezerBackWallLight_RampingPwm, 0);
   TheRampingDownCountPerMsecShouldBe(Erd_FreezerTopLight_RampingPwm, 0);
}

TEST(LightingController, ShouldUpdateAllRampingUpCountsPerMsecIfEverythingIsVotedFor)
{
   GivenInitialization();

   WhenTheRampingUpCountPerMsecChangesTo(Erd_FreshFoodBackWallLight_ResolvedVote, SomeRampingUpCountPerMsec);
   WhenTheRampingUpCountPerMsecChangesTo(Erd_FreshFoodTopLight_ResolvedVote, SomeRampingUpCountPerMsec + 1);
   WhenTheRampingUpCountPerMsecChangesTo(Erd_FreezerBackWallLight_ResolvedVote, SomeRampingUpCountPerMsec + 2);
   WhenTheRampingUpCountPerMsecChangesTo(Erd_FreezerTopLight_ResolvedVote, SomeRampingUpCountPerMsec + 3);

   TheRampingUpCountPerMsecShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, SomeRampingUpCountPerMsec);
   TheRampingUpCountPerMsecShouldBe(Erd_FreshFoodTopLight_RampingPwm, SomeRampingUpCountPerMsec + 1);
   TheRampingUpCountPerMsecShouldBe(Erd_FreezerBackWallLight_RampingPwm, SomeRampingUpCountPerMsec + 2);
   TheRampingUpCountPerMsecShouldBe(Erd_FreezerTopLight_RampingPwm, SomeRampingUpCountPerMsec + 3);
}

TEST(LightingController, ShouldUpdateAllRampingDownCountsPerMsecIfEverythingIsVotedFor)
{
   GivenInitialization();

   WhenTheRampingDownCountPerMsecChangesTo(Erd_FreshFoodBackWallLight_ResolvedVote, SomeRampingDownCountPerMsec);
   WhenTheRampingDownCountPerMsecChangesTo(Erd_FreshFoodTopLight_ResolvedVote, SomeRampingDownCountPerMsec + 1);
   WhenTheRampingDownCountPerMsecChangesTo(Erd_FreezerBackWallLight_ResolvedVote, SomeRampingDownCountPerMsec + 2);
   WhenTheRampingDownCountPerMsecChangesTo(Erd_FreezerTopLight_ResolvedVote, SomeRampingDownCountPerMsec + 3);

   TheRampingDownCountPerMsecShouldBe(Erd_FreshFoodBackWallLight_RampingPwm, SomeRampingDownCountPerMsec);
   TheRampingDownCountPerMsecShouldBe(Erd_FreshFoodTopLight_RampingPwm, SomeRampingDownCountPerMsec + 1);
   TheRampingDownCountPerMsecShouldBe(Erd_FreezerBackWallLight_RampingPwm, SomeRampingDownCountPerMsec + 2);
   TheRampingDownCountPerMsecShouldBe(Erd_FreezerTopLight_RampingPwm, SomeRampingDownCountPerMsec + 3);
}
