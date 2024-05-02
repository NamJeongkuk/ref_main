/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanDeliDeliFanDependencyVoting.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   NonZero = 1000
};

FeaturePanDeliDeliFanDependencyVotingConfig_t config = {
   .deliFanActualRpmErd = Erd_DeliFan_ActualRpm,
   .deliHeaterVotedErd = Erd_DeliPanHeater_DeliFanDependencyVote
};

TEST_GROUP(FeaturePanDeliDeliFanDependencyVoting)
{
   FeaturePanDeliDeliFanDependencyVoting_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
   }

   void GivenModuleIsInitialized()
   {
      FeaturePanDeliDeliFanDependencyVoting_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenDeliPanFanRpmIs(FanRpm_t fanRpm)
   {
      DataModel_Write(dataModel, Erd_DeliFan_ActualRpm, &fanRpm);
   }

   void WhenDeliPanFanRpmIs(FanRpm_t fanRpm)
   {
      GivenDeliPanFanRpmIs(fanRpm);
   }

   void DeliPanHeaterShouldBe(PercentageDutyCycleVote_t expected)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(dataModel, Erd_DeliPanHeater_DeliFanDependencyVote, &actual);

      CHECK_EQUAL(expected.percentageDutyCycle, actual.percentageDutyCycle);
      CHECK_EQUAL(expected.care, actual.care);
   }
};

TEST(FeaturePanDeliDeliFanDependencyVoting, ShouldVoteDeliHeaterOffWhenDeliFanRpmIsZeroOnInit)
{
   GivenDeliPanFanRpmIs(0);
   GivenModuleIsInitialized();

   DeliPanHeaterShouldBe({ PercentageDutyCycle_Min, Vote_Care });
}

TEST(FeaturePanDeliDeliFanDependencyVoting, ShouldVoteDontCareWhenDeliFanRpmIsNonZeroOnInit)
{
   GivenDeliPanFanRpmIs(NonZero);
   GivenModuleIsInitialized();

   DeliPanHeaterShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });
}

TEST(FeaturePanDeliDeliFanDependencyVoting, ShouldVoteDontCareWhenDeliFanRpmChangesToNonZero)
{
   GivenDeliPanFanRpmIs(0);
   GivenModuleIsInitialized();
   DeliPanHeaterShouldBe({ PercentageDutyCycle_Min, Vote_Care });

   WhenDeliPanFanRpmIs(NonZero);
   DeliPanHeaterShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });
}

TEST(FeaturePanDeliDeliFanDependencyVoting, ShouldVoteDeliHeaterOffWhenDeliFanRpmChangesToZero)
{
   GivenDeliPanFanRpmIs(NonZero);
   GivenModuleIsInitialized();
   DeliPanHeaterShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });

   WhenDeliPanFanRpmIs(0);
   DeliPanHeaterShouldBe({ PercentageDutyCycle_Min, Vote_Care });
}
