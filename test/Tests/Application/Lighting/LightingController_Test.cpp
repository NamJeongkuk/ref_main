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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   SomePwmDutyCycle = 4
};

static const VotedPwmDutyCyclePair_t votedPwmPairs[] = {
   { Erd_FreshFoodBackWallLight_ResolvedVote, Erd_FreshFoodBackWallLight_Pwm },
   { Erd_FreshFoodTopLight_ResolvedVote, Erd_FreshFoodTopLight_Pwm },
   { Erd_FreezerBackWallLight_ResolvedVote, Erd_FreezerBackWallLight_Pwm },
   { Erd_FreezerTopLight_ResolvedVote, Erd_FreezerTopLight_Pwm }
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

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      ConstArrayMap_LinearSearch_Init(
         &map,
         &mapConfiguration);
   }

   void GivenInitialization()
   {
      LightingController_Init(
         &instance,
         dataModel,
         &map.interface);
   }

   void GivenTheVotedPwmDutyCycleErdIs(Erd_t erd, PwmDutyCycle_t pwmDutyCycle)
   {
      PwmVotedDutyCycle_t votedPwmDutyCycle = {
         .pwmDutyCycle = pwmDutyCycle,
         .care = Vote_Care
      };

      DataModel_Write(dataModel, erd, &votedPwmDutyCycle);
   }

   void WhenVotedPwmDutyCycleErdChangesTo(Erd_t erd, PwmDutyCycle_t pwmDutyCycle)
   {
      GivenTheVotedPwmDutyCycleErdIs(erd, pwmDutyCycle);
   }

   void ThePwmDutyCycleErdShouldBe(Erd_t erd, PwmDutyCycle_t expected)
   {
      PwmDutyCycle_t actual;
      DataModel_Read(dataModel, erd, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(LightingController, AllPwmsShouldBeZeroOnInit)
{
   GivenTheVotedPwmDutyCycleErdIs(Erd_FreezerTopLight_ResolvedVote, PwmDutyCycle_Max);
   GivenTheVotedPwmDutyCycleErdIs(Erd_FreshFoodBackWallLight_ResolvedVote, PwmDutyCycle_Max);
   GivenTheVotedPwmDutyCycleErdIs(Erd_FreshFoodTopLight_ResolvedVote, PwmDutyCycle_Max);
   GivenTheVotedPwmDutyCycleErdIs(Erd_FreezerBackWallLight_ResolvedVote, PwmDutyCycle_Max);
   GivenInitialization();

   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_Pwm, 0);
}

TEST(LightingController, ShouldOnlyUpdateFreshFoodBackWallLightPwmWhenVotedFor)
{
   GivenInitialization();

   WhenVotedPwmDutyCycleErdChangesTo(Erd_FreshFoodBackWallLight_ResolvedVote, PwmDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_Pwm, PwmDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_Pwm, 0);

   WhenVotedPwmDutyCycleErdChangesTo(Erd_FreshFoodBackWallLight_ResolvedVote, SomePwmDutyCycle);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_Pwm, SomePwmDutyCycle);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_Pwm, 0);
}

TEST(LightingController, ShouldOnlyUpdateFreshFoodTopLightPwmWhenVotedFor)
{
   GivenInitialization();

   WhenVotedPwmDutyCycleErdChangesTo(Erd_FreshFoodTopLight_ResolvedVote, PwmDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_Pwm, PwmDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_Pwm, 0);

   WhenVotedPwmDutyCycleErdChangesTo(Erd_FreshFoodTopLight_ResolvedVote, SomePwmDutyCycle);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_Pwm, SomePwmDutyCycle);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_Pwm, 0);
}

TEST(LightingController, ShouldOnlyUpdateFreezerBackWallLightPwmWhenVotedFor)
{
   GivenInitialization();

   WhenVotedPwmDutyCycleErdChangesTo(Erd_FreezerBackWallLight_ResolvedVote, PwmDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_Pwm, PwmDutyCycle_Max);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_Pwm, 0);

   WhenVotedPwmDutyCycleErdChangesTo(Erd_FreezerBackWallLight_ResolvedVote, SomePwmDutyCycle);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_Pwm, 0);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_Pwm, SomePwmDutyCycle);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_Pwm, 0);
}

TEST(LightingController, ShouldUpdateAllPwmsIfEverythingIsVotedFor)
{
   GivenInitialization();

   WhenVotedPwmDutyCycleErdChangesTo(Erd_FreshFoodBackWallLight_ResolvedVote, SomePwmDutyCycle);
   WhenVotedPwmDutyCycleErdChangesTo(Erd_FreshFoodTopLight_ResolvedVote, SomePwmDutyCycle + 1);
   WhenVotedPwmDutyCycleErdChangesTo(Erd_FreezerBackWallLight_ResolvedVote, SomePwmDutyCycle + 2);
   WhenVotedPwmDutyCycleErdChangesTo(Erd_FreezerTopLight_ResolvedVote, SomePwmDutyCycle + 3);

   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodBackWallLight_Pwm, SomePwmDutyCycle);
   ThePwmDutyCycleErdShouldBe(Erd_FreshFoodTopLight_Pwm, SomePwmDutyCycle + 1);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerBackWallLight_Pwm, SomePwmDutyCycle + 2);
   ThePwmDutyCycleErdShouldBe(Erd_FreezerTopLight_Pwm, SomePwmDutyCycle + 3);
}
