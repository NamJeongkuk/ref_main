/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DamperHeaterDefrostControl.h"
#include "PersonalityParametricData.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "TddPersonality.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"

#define Given
#define And
#define When

static const DamperHeaterDefrostControlConfig_t config = {
   .defrostHeaterVoteErd = Erd_FreezerDefrostHeater_DefrostVote,
   .damperHeaterDefrostHeaterSyncVoteErd = Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,
   .defrostHeaterStateErd = Erd_FreezerDefrostHeaterRelay
};

TEST_GROUP(DamperHeaterDefrostControl)
{
   DamperHeaterDefrostControl_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleDouble;

   const DamperHeaterData_t *damperHeaterData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      damperHeaterData = PersonalityParametricData_Get(dataModelDouble.dataModel)->damperHeaterData;
   }

   void TheModuleIsInitialized()
   {
      DamperHeaterDefrostControl_Init(
         &instance,
         dataModelDouble.dataModel,
         &config,
         &timerModuleDouble->timerModule,
         damperHeaterData);
   }

   void TheDamperHeaterDefrostHeaterSyncVoteStateShouldBe(HeaterState_t expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,
         &actual);
      CHECK_EQUAL(expected, actual.state);
   }

   void TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(bool expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,
         &actual);
      CHECK_EQUAL(expected, actual.care);
   }

   void TheDamperHeaterDefrostHeaterSyncVoteIs(HeaterState_t state, bool care)
   {
      HeaterVotedState_t votedState = {
         .state = state,
         .care = care
      };
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,
         &votedState);
   }

   void DefrostHeaterDefrostVoteIsSetTo(HeaterState_t state, bool care)
   {
      HeaterVotedState_t votedState = {
         .state = state,
         .care = care
      };
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreezerDefrostHeater_DefrostVote,
         &votedState);
   }

   void TheDefrostHeaterStateIs(HeaterState_t state, bool care)
   {
      HeaterVotedState_t votedState = {
         .state = state,
         .care = care
      };
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreezerDefrostHeaterRelay,
         &votedState);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1);
   }
};

TEST(DamperHeaterDefrostControl, ShouldInitializeTheModule)
{
   TheModuleIsInitialized();
}

TEST(DamperHeaterDefrostControl, ShouldVoteCareWhenFollowingDefrostHeaterAndDefrostVoteCaresAndCurrentDefrostStateIsOn)
{
   Given TheDamperHeaterDefrostHeaterSyncVoteIs(PercentageDutyCycle_Min, Vote_DontCare);
   And TheDefrostHeaterStateIs(PercentageDutyCycle_Max, Vote_DontCare);
   And TheModuleIsInitialized();

   When DefrostHeaterDefrostVoteIsSetTo(PercentageDutyCycle_Max, Vote_Care);
   TheDamperHeaterDefrostHeaterSyncVoteStateShouldBe(PercentageDutyCycle_Max);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_Care);
}

TEST(DamperHeaterDefrostControl, ShouldVoteDontCareWhenFollowingDefrostHeaterAndDefrostVoteCaresAndCurrentDefrostStateIsOff)
{
   Given TheDamperHeaterDefrostHeaterSyncVoteIs(PercentageDutyCycle_Min, Vote_Care);
   And TheDefrostHeaterStateIs(PercentageDutyCycle_Min, Vote_DontCare);
   And TheModuleIsInitialized();

   When DefrostHeaterDefrostVoteIsSetTo(PercentageDutyCycle_Max, Vote_Care);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_DontCare);
}

TEST(DamperHeaterDefrostControl, ShouldMatchDefrostHeaterStateChangesWhenFollowingDefrostHeaterIsTrue)
{
   Given TheDamperHeaterDefrostHeaterSyncVoteIs(PercentageDutyCycle_Min, Vote_Care);
   And TheDefrostHeaterStateIs(PercentageDutyCycle_Max, Vote_DontCare);
   And TheModuleIsInitialized();

   When DefrostHeaterDefrostVoteIsSetTo(PercentageDutyCycle_Max, Vote_Care);
   TheDamperHeaterDefrostHeaterSyncVoteStateShouldBe(PercentageDutyCycle_Max);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_Care);

   When TheDefrostHeaterStateIs(PercentageDutyCycle_Min, Vote_Care);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_DontCare);
   TheDamperHeaterDefrostHeaterSyncVoteStateShouldBe(PercentageDutyCycle_Min);
}

TEST(DamperHeaterDefrostControl, ShouldVoteDontCareWhenFollowingDefrostHeaterButDefrostVoteIsDontCare)
{
   Given TheDamperHeaterDefrostHeaterSyncVoteIs(PercentageDutyCycle_Min, Vote_Care);
   And DefrostHeaterDefrostVoteIsSetTo(PercentageDutyCycle_Max, Vote_Care);
   And TheModuleIsInitialized();

   When DefrostHeaterDefrostVoteIsSetTo(PercentageDutyCycle_Min, Vote_DontCare);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_DontCare);
   TheDamperHeaterDefrostHeaterSyncVoteStateShouldBe(PercentageDutyCycle_Min);
}

TEST(DamperHeaterDefrostControl, ShouldStopMatchingDefrostHeaterStateChangesWhenDefrostHeaterVoteIsDontCare)
{
   Given TheDamperHeaterDefrostHeaterSyncVoteIs(PercentageDutyCycle_Min, Vote_DontCare);
   And TheDefrostHeaterStateIs(PercentageDutyCycle_Max, Vote_DontCare);
   And TheModuleIsInitialized();

   When DefrostHeaterDefrostVoteIsSetTo(PercentageDutyCycle_Max, Vote_Care);
   TheDamperHeaterDefrostHeaterSyncVoteStateShouldBe(PercentageDutyCycle_Max);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_Care);

   When DefrostHeaterDefrostVoteIsSetTo(PercentageDutyCycle_Min, Vote_DontCare);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_DontCare);
   TheDamperHeaterDefrostHeaterSyncVoteStateShouldBe(PercentageDutyCycle_Min);

   When TheDefrostHeaterStateIs(PercentageDutyCycle_Max, Vote_Care);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_DontCare);
   TheDamperHeaterDefrostHeaterSyncVoteStateShouldBe(PercentageDutyCycle_Min);
}

TEST_GROUP(DamperHeaterDefrostControl_DontFollow)
{
   DamperHeaterDefrostControl_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t timerModuleDouble;

   const DamperHeaterData_t *damperHeaterData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDamperHeaterIsOnWhenDefrostHeaterIsOnIsFalse);
      TimerModule_TestDouble_Init(&timerModuleDouble);

      damperHeaterData = PersonalityParametricData_Get(dataModelDouble.dataModel)->damperHeaterData;
   }

   void TheModuleIsInitialized()
   {
      DamperHeaterDefrostControl_Init(
         &instance,
         dataModelDouble.dataModel,
         &config,
         &timerModuleDouble.timerModule,
         damperHeaterData);
   }

   void TheDamperHeaterDefrostHeaterSyncVoteIs(HeaterState_t state, bool care)
   {
      HeaterVotedState_t votedState = {
         .state = state,
         .care = care
      };
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,
         &votedState);
   }

   void DefrostHeaterDefrostVoteIsSetTo(HeaterState_t state, bool care)
   {
      HeaterVotedState_t votedState = {
         .state = state,
         .care = care
      };
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreezerDefrostHeater_DefrostVote,
         &votedState);
   }

   void TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(bool expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,
         &actual);
      CHECK_EQUAL(expected, actual.care);
   }

   void TheDamperHeaterDefrostHeaterSyncVoteStateShouldBe(HeaterState_t expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,
         &actual);
      CHECK_EQUAL(expected, actual.state);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleDouble, ticks, 1);
   }
};

TEST(DamperHeaterDefrostControl_DontFollow, ShouldVoteCareForTimerAndThenVoteNotCareAfterTimerWhenDefrostVoteCaresAndThereIsOnTime)
{
   Given TheDamperHeaterDefrostHeaterSyncVoteIs(PercentageDutyCycle_Min, Vote_DontCare);

   When TheModuleIsInitialized();
   And DefrostHeaterDefrostVoteIsSetTo(PercentageDutyCycle_Max, Vote_Care);
   TheDamperHeaterDefrostHeaterSyncVoteStateShouldBe(PercentageDutyCycle_Max);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_Care);

   After(damperHeaterData->damperHeaterDefrostOnTimeInMinutes * MSEC_PER_MIN - 1);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_Care);
   After(1);
   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_DontCare);
}

TEST_GROUP(DamperHeaterDefrostControl_ZeroOnTime)
{
   DamperHeaterDefrostControl_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t timerModuleDouble;

   const DamperHeaterData_t *damperHeaterData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDamperHeaterZeroOnTimeMinutes);
      TimerModule_TestDouble_Init(&timerModuleDouble);

      damperHeaterData = PersonalityParametricData_Get(dataModelDouble.dataModel)->damperHeaterData;
   }

   void TheModuleIsInitialized()
   {
      DamperHeaterDefrostControl_Init(
         &instance,
         dataModelDouble.dataModel,
         &config,
         &timerModuleDouble.timerModule,
         damperHeaterData);
   }

   void TheDamperHeaterDefrostHeaterSyncVoteIs(HeaterState_t state, bool care)
   {
      HeaterVotedState_t votedState = {
         .state = state,
         .care = care
      };
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,
         &votedState);
   }

   void DefrostHeaterDefrostVoteIsSetTo(HeaterState_t state, bool care)
   {
      HeaterVotedState_t votedState = {
         .state = state,
         .care = care
      };
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreezerDefrostHeater_DefrostVote,
         &votedState);
   }

   void TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(bool expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote,
         &actual);
      CHECK_EQUAL(expected, actual.care);
   }
};

TEST(DamperHeaterDefrostControl_ZeroOnTime, ShouldVoteNotCareImmediatelyWhenDefrostVoteCaresAndOnTimeInMinutesIsZero)
{
   Given TheDamperHeaterDefrostHeaterSyncVoteIs(PercentageDutyCycle_Min, Vote_Care);

   When TheModuleIsInitialized();
   And DefrostHeaterDefrostVoteIsSetTo(PercentageDutyCycle_Max, Vote_Care);

   TheDamperHeaterDefrostHeaterSyncVoteCareShouldBe(Vote_DontCare);
}
