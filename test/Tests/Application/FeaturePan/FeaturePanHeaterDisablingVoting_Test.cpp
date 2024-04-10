/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanHeaterDisablingVoting.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   FeaturePanHeaterHighAmbientTemperatureParametricLimit = 100
};

const FeaturePanHeaterDisablingVotingConfig_t config = {
   .featurePanModeErd = Erd_FeaturePanMode,
   .heaterVotedErd = Erd_ConvertibleCompartmentHeater_DisableForAmbientTemperatureVote,
   .ambientTemperatureDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100
};

const FeaturePanData_t featurePanData = {
   .featurePanHeaterHighAmbientTemperatureLimitInDegFx100 = FeaturePanHeaterHighAmbientTemperatureParametricLimit
};

TEST_GROUP(FeaturePanHeaterDisablingVoting)
{
   FeaturePanHeaterDisablingVoting_t instance;
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
      FeaturePanHeaterDisablingVoting_Init(
         &instance,
         dataModel,
         &config,
         &featurePanData);
   }

   void GivenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Ambient_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenAmbientTemperatureIs(temperature);
   }

   void GivenFeaturePanModeIs(FeaturePanMode_t featurePanMode)
   {
      DataModel_Write(dataModel, Erd_FeaturePanMode, &featurePanMode);
   }

   void WhenFeaturePanModeIs(FeaturePanMode_t featurePanMode)
   {
      GivenFeaturePanModeIs(featurePanMode);
   }

   void HeaterVoteShouldBe(PercentageDutyCycleVote_t expected)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentHeater_DisableForAmbientTemperatureVote, &actual);
      CHECK_EQUAL(expected.percentageDutyCycle, actual.percentageDutyCycle);
      CHECK_EQUAL(expected.care, actual.care);
   }
};

TEST(FeaturePanHeaterDisablingVoting, ShouldVoteHeaterDutyCycleMinWhenAmbientTemperatureIsGreaterThanParametricValueAndFeaturePanStateIsActiveHeatingOnInit)
{
   GivenFeaturePanModeIs(FeaturePanMode_ActiveHeating);
   GivenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit + 1);
   GivenModuleIsInitialized();

   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_Care });
}

TEST(FeaturePanHeaterDisablingVoting, ShouldVoteDontCareWhenAmbientTemperatureIsLessThanParametricValueAndFeaturePanStateIsActiveHeatingOnInit)
{
   GivenFeaturePanModeIs(FeaturePanMode_ActiveHeating);
   GivenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit - 1);
   GivenModuleIsInitialized();

   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });
}

TEST(FeaturePanHeaterDisablingVoting, ShouldVoteHeaterDutyCycleMinWhenAmbientTemperatureIsEqualToParametricValueOnInit)
{
   GivenFeaturePanModeIs(FeaturePanMode_ActiveHeating);
   GivenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit);
   GivenModuleIsInitialized();

   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_Care });
}

TEST(FeaturePanHeaterDisablingVoting, ShouldVoteDontCareWhenAmbientTemperatureIsGreaterThanParametricValueAndFeaturePanStateIsNotActiveHeatingOnInit)
{
   GivenFeaturePanModeIs(FeaturePanMode_Neutral);
   GivenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit + 1);
   GivenModuleIsInitialized();

   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });
}

TEST(FeaturePanHeaterDisablingVoting, ShouldVoteHeaterDutyCycleMinWhenAmbientTemperatureChangesToBeGreaterThanOrEqualToParametricValueAndFeaturePanStateIsActiveHeating)
{
   GivenFeaturePanModeIs(FeaturePanMode_ActiveHeating);
   GivenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit - 1);
   GivenModuleIsInitialized();
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });

   WhenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit);
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_Care });

   WhenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit + 1);
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_Care });
}

TEST(FeaturePanHeaterDisablingVoting, ShouldVoteHeaterDutyCycleMinWhenFeaturePanModeChangesToActiveHeatingAndAmbientTemperatureIsGreaterThanParametricValue)
{
   GivenFeaturePanModeIs(FeaturePanMode_Neutral);
   GivenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit + 1);
   GivenModuleIsInitialized();
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });

   WhenFeaturePanModeIs(FeaturePanMode_ActiveHeating);
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_Care });
}

TEST(FeaturePanHeaterDisablingVoting, ShouldVoteDontCareWhenAmbientTemperatureChangesToBeLessThanParametricValueAndFeaturePanStateIsActiveHeating)
{
   GivenFeaturePanModeIs(FeaturePanMode_ActiveHeating);
   GivenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit + 1);
   GivenModuleIsInitialized();
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_Care });

   WhenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit - 1);
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });
}

TEST(FeaturePanHeaterDisablingVoting, ShouldVoteDontCareWhenFeaturePanModeChangesToNotActiveHeatingAndAmbientTemperatureIsGreaterThanParametricValue)
{
   GivenFeaturePanModeIs(FeaturePanMode_ActiveHeating);
   GivenAmbientTemperatureIs(FeaturePanHeaterHighAmbientTemperatureParametricLimit + 1);
   GivenModuleIsInitialized();
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_Care });

   WhenFeaturePanModeIs(FeaturePanMode_Neutral);
   HeaterVoteShouldBe({ PercentageDutyCycle_Min, Vote_DontCare });
}
