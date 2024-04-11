/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanPulldownVoting.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   FeaturePanPulldownTemepratureLimitParametric = 1000
};

const FeaturePanPulldownVotingConfig_t config = {
   .featurePanTemperatureErd = Erd_DeliPan_FilteredTemperatureInDegFx100,
   .featurePanDamperPositionVoteErd = Erd_DeliDamperPosition_FeaturePanVote,
   .featurePanFanVoteErd = Erd_DeliFanSpeed_FeaturePanVote,
   .featurePanHeaterVoteErd = Erd_ConvertibleCompartmentHeater_PulldownVote
};

const FeaturePanData_t featurePanData = {
   .featurePanPulldownTemperatureLimitInDegFx100 = FeaturePanPulldownTemepratureLimitParametric
};

TEST_GROUP(FeaturePanPulldownVoting)
{
   FeaturePanPulldownVoting_t instance;
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
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenModuleIsInitialized()
   {
      FeaturePanPulldownVoting_Init(
         &instance,
         dataModel,
         &config,
         &featurePanData);
   }

   void GivenFeaturePanTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_DeliPan_FilteredTemperatureInDegFx100, &temperature);
   }

   void WhenFeaturePanTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenFeaturePanTemperatureIs(temperature);
   }

   void DamperPositionVoteShouldBe(DamperVotedPosition_t expected)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_DeliDamperPosition_FeaturePanVote, &actual);

      CHECK_EQUAL(expected.position, actual.position);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void EvapFanSpeedVoteShouldBe(FanVotedSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_DeliFanSpeed_FeaturePanVote, &actual);

      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void HeaterVoteShouldBe(PercentageDutyCycleVote_t expected)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentHeater_PulldownVote, &actual);

      CHECK_EQUAL(expected.percentageDutyCycle, actual.percentageDutyCycle);
      CHECK_EQUAL(expected.care, actual.care);
   }
};

TEST(FeaturePanPulldownVoting, ShouldVoteDamperClosedWhenFeaturePanTemperatureIsGreaterThanParametricValueOnInit)
{
   GivenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric + 1);
   GivenModuleIsInitialized();

   DamperPositionVoteShouldBe({ DamperPosition_Closed, Vote_Care });
}

TEST(FeaturePanPulldownVoting, ShouldVoteEvapFanSpeedOffWhenFeaturePanTemperatureIsGreaterThanParametricValueOnInit)
{
   GivenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric + 1);
   GivenModuleIsInitialized();

   EvapFanSpeedVoteShouldBe({ FanSpeed_Off, Vote_Care });
}

TEST(FeaturePanPulldownVoting, ShouldVoteHeaterDutyCycleMinWhenFeaturePanTemperatureIsGreaterThanParametricValueOnInit)
{
   GivenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric + 1);
   GivenModuleIsInitialized();

   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_Care });
}

TEST(FeaturePanPulldownVoting, ShouldVoteDontCareForAllErdsWhenFeaturePanTemperatureIsLessThanParametricValueOnInit)
{
   GivenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric - 1);
   GivenModuleIsInitialized();

   DamperPositionVoteShouldBe({ DamperPosition_Closed, Vote_DontCare });
   EvapFanSpeedVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });
}

TEST(FeaturePanPulldownVoting, ShouldVoteDontCareForAllErdsWhenFeaturePanTemperatureIsEqualToParametricValueOnInit)
{
   GivenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric);
   GivenModuleIsInitialized();

   DamperPositionVoteShouldBe({ DamperPosition_Closed, Vote_DontCare });
   EvapFanSpeedVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });
}

TEST(FeaturePanPulldownVoting, ShouldVoteDamperClosedWhenFeaturePanTemperatureChangesToBeGreaterThanParametricValue)
{
   GivenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric);
   GivenModuleIsInitialized();
   DamperPositionVoteShouldBe({ DamperPosition_Closed, Vote_DontCare });

   WhenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric + 1);
   DamperPositionVoteShouldBe({ DamperPosition_Closed, Vote_Care });
}

TEST(FeaturePanPulldownVoting, ShouldVoteEvapFanSpeedOffWhenFeaturePanTemperatureChangesToBeGreaterThanParametricValue)
{
   GivenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric);
   GivenModuleIsInitialized();
   EvapFanSpeedVoteShouldBe({ FanSpeed_Off, Vote_DontCare });

   WhenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric + 1);
   EvapFanSpeedVoteShouldBe({ FanSpeed_Off, Vote_Care });
}

TEST(FeaturePanPulldownVoting, ShouldVoteHeaterDutyCycleMinWhenFeaturePanTemperatureChangesToBeGreaterThanParametricValue)
{
   GivenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric);
   GivenModuleIsInitialized();
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });

   WhenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric + 1);
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_Care });
}

TEST(FeaturePanPulldownVoting, ShouldVoteDontCareForAllErdsWhenFeaturePanTemperatureChangesToBeEqualToOrLessThanParametricValue)
{
   GivenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric + 1);
   GivenModuleIsInitialized();
   DamperPositionVoteShouldBe({ DamperPosition_Closed, Vote_Care });
   EvapFanSpeedVoteShouldBe({ FanSpeed_Off, Vote_Care });
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_Care });

   WhenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric);
   DamperPositionVoteShouldBe({ DamperPosition_Closed, Vote_DontCare });
   EvapFanSpeedVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });

   WhenFeaturePanTemperatureIs(FeaturePanPulldownTemepratureLimitParametric - 1);
   DamperPositionVoteShouldBe({ DamperPosition_Closed, Vote_DontCare });
   EvapFanSpeedVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });
}
