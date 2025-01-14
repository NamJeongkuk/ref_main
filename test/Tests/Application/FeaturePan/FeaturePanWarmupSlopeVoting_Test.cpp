/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanWarmupSlopeVoting.h"
#include "Constants_Binary.h"
#include "PersonalityParametricData.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   SlopeCalculationTimeInMsec = 1 * MSEC_PER_MIN,
   SomeTemperature = 1000,
};

static const FeaturePanData_t featurePanData = {
   .featurePanWarmupSlopeLowerLimitInDegFx100PerMinute = 100,
   .featurePanWarmupSlopeUpperLimitInDegFx100PerMinute = 200,
   .featurePanHeaterHighAmbientTemperatureLimitInDegFx100 = 8000,
   .freshFoodFanDeliCoolingSpeed = FanSpeed_Low,
};

static const FeaturePanWarmupSlopeVotingConfig_t config = {
   .featurePanCoolingModeErd = Erd_FeaturePanCoolingMode,
   .heaterVotedErd = Erd_ConvertibleCompartmentHeater_WarmupSlopeVote,
   .featurePanTemperatureDegFx100Erd = Erd_FeaturePan_FilteredTemperatureResolvedInDegFx100,
};

TEST_GROUP(FeaturePanWarmupSlopeVoting)
{
   FeaturePanWarmupSlopeVoting_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      FeaturePanWarmupSlopeVoting_Init(&instance, dataModel, &timerModuleTestDouble->timerModule, &config, &featurePanData);
   }

   void WhenTheModuleIsInitialized()
   {
      GivenTheModuleIsInitialized();
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenHeaterVoteIs(PercentageDutyCycle_t dutyCycle)
   {
      PercentageDutyCycleVote_t heaterVote = { .percentageDutyCycle = dutyCycle, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         config.heaterVotedErd,
         &heaterVote);
   }

   void GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_t mode)
   {
      DataModel_Write(
         dataModel,
         config.featurePanCoolingModeErd,
         &mode);
   }

   void WhenFeaturePanCoolingModeChangesTo(FeaturePanCoolingMode_t mode)
   {
      GivenFeaturePanCoolingModeIs(mode);
   }

   void GivenTheFeaturePanTemperatureIs(TemperatureDegFx100_t temp)
   {
      DataModel_Write(
         dataModel,
         config.featurePanTemperatureDegFx100Erd,
         &temp);
   }

   void HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_t expected)
   {
      PercentageDutyCycleVote_t heaterVote;
      DataModel_Read(dataModel, config.heaterVotedErd, &heaterVote);

      CHECK_EQUAL(expected, heaterVote.percentageDutyCycle);
      CHECK_EQUAL(Vote_Care, heaterVote.care);
   }

   void HeaterVoteShouldNotCare()
   {
      PercentageDutyCycleVote_t heaterVote;
      DataModel_Read(dataModel, config.heaterVotedErd, &heaterVote);

      CHECK_EQUAL(PercentageDutyCycle_Min, heaterVote.percentageDutyCycle);
      CHECK_EQUAL(Vote_DontCare, heaterVote.care);
   }
};

TEST(FeaturePanWarmupSlopeVoting, ShouldVoteHeaterOffOnInitWhenFeaturePanCoolingModeIsActiveHeating)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveHeating);
   GivenHeaterVoteIs(PercentageDutyCycle_Max);
   WhenTheModuleIsInitialized();

   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Min);
}

TEST(FeaturePanWarmupSlopeVoting, ShouldVoteHeaterDontCareOnInitWhenFeaturePanCoolingModeIsNotActiveHeating)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   GivenHeaterVoteIs(PercentageDutyCycle_Max);
   WhenTheModuleIsInitialized();

   HeaterVoteShouldNotCare();
}

TEST(FeaturePanWarmupSlopeVoting, ShouldVoteHeaterOnWhenFeaturePanTemperatureSlopeBecomesLessThanParametricConfigSlope)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveHeating);
   GivenTheFeaturePanTemperatureIs(SomeTemperature);
   GivenTheModuleIsInitialized();
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Min);

   GivenTheFeaturePanTemperatureIs(SomeTemperature + featurePanData.featurePanWarmupSlopeLowerLimitInDegFx100PerMinute - 1);
   After(SlopeCalculationTimeInMsec - 1);
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Min);
   After(1);
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Max);
}

TEST(FeaturePanWarmupSlopeVoting, ShouldVoteHeaterOffWhenFeaturePanTemperatureSlopeBecomesGreaterThanParametricConfigSlope)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveHeating);
   GivenTheFeaturePanTemperatureIs(SomeTemperature);
   GivenTheModuleIsInitialized();
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Min);

   GivenTheFeaturePanTemperatureIs(SomeTemperature + featurePanData.featurePanWarmupSlopeLowerLimitInDegFx100PerMinute - 1);
   After(SlopeCalculationTimeInMsec);
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Max);

   GivenTheFeaturePanTemperatureIs(SomeTemperature + featurePanData.featurePanWarmupSlopeLowerLimitInDegFx100PerMinute - 1 + featurePanData.featurePanWarmupSlopeUpperLimitInDegFx100PerMinute + 1);
   After(SlopeCalculationTimeInMsec);
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Min);
}

TEST(FeaturePanWarmupSlopeVoting, ShouldVoteHeaterOnWhenFeaturePanTemperatureSlopeIsLessThanParametricConfigSlopeAndFeaturePanCoolingModeChangesToActiveHeating)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   GivenTheFeaturePanTemperatureIs(SomeTemperature);
   GivenTheModuleIsInitialized();
   HeaterVoteShouldNotCare();

   GivenTheFeaturePanTemperatureIs(SomeTemperature + featurePanData.featurePanWarmupSlopeLowerLimitInDegFx100PerMinute - 1);
   After(SlopeCalculationTimeInMsec);
   HeaterVoteShouldNotCare();

   WhenFeaturePanCoolingModeChangesTo(FeaturePanCoolingMode_ActiveHeating);
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Max);
}

TEST(FeaturePanWarmupSlopeVoting, ShouldVoteHeaterOffWhenFeaturePanTemperatureSlopeIsGreaterThanParametricConfigSlopeAndFeaturePanCoolingModeChangesToActiveHeating)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   GivenTheFeaturePanTemperatureIs(SomeTemperature);
   GivenTheModuleIsInitialized();
   HeaterVoteShouldNotCare();

   GivenTheFeaturePanTemperatureIs(SomeTemperature + featurePanData.featurePanWarmupSlopeUpperLimitInDegFx100PerMinute + 1);
   After(SlopeCalculationTimeInMsec);
   HeaterVoteShouldNotCare();

   WhenFeaturePanCoolingModeChangesTo(FeaturePanCoolingMode_ActiveHeating);
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Min);
}

TEST(FeaturePanWarmupSlopeVoting, ShouldVoteHeaterOffWhenFeaturePanCoolingModeChangesToNonActiveHeating)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveHeating);
   GivenTheFeaturePanTemperatureIs(SomeTemperature);
   GivenTheModuleIsInitialized();
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Min);

   GivenTheFeaturePanTemperatureIs(SomeTemperature + featurePanData.featurePanWarmupSlopeLowerLimitInDegFx100PerMinute - 1);
   After(SlopeCalculationTimeInMsec);
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Max);

   WhenFeaturePanCoolingModeChangesTo(FeaturePanCoolingMode_ActiveCooling);
   HeaterVoteShouldNotCare();
}

TEST(FeaturePanWarmupSlopeVoting, ShouldVoteHeaterOffWhenFeaturePanCoolingModeChangesToActiveHeatingAndSlopeHasNotYetBeenCalculated)
{
   GivenFeaturePanCoolingModeIs(FeaturePanCoolingMode_ActiveCooling);
   GivenTheFeaturePanTemperatureIs(SomeTemperature);
   GivenTheModuleIsInitialized();
   HeaterVoteShouldNotCare();

   WhenFeaturePanCoolingModeChangesTo(FeaturePanCoolingMode_ActiveHeating);
   HeaterVoteShouldNotCare();

   GivenTheFeaturePanTemperatureIs(SomeTemperature + featurePanData.featurePanWarmupSlopeLowerLimitInDegFx100PerMinute - 1);
   After(SlopeCalculationTimeInMsec);
   HeaterVoteShouldCareForDutyCycle(PercentageDutyCycle_Max);
}
