/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "EnhancedSabbathMode.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "PersonalityParametricData.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   SomeTemperatureInDegFx100 = 5000,
   EnhancedSabbathModeHsmState_Unknown = 0xFF
};

static const Erd_t lightVotes[] = {
   Erd_FreshFoodBackWallLight_EnhancedSabbathVote,
   Erd_FreshFoodTopLight_EnhancedSabbathVote,
   Erd_FreezerBackWallLight_EnhancedSabbathVote,
   Erd_FreezerTopLight_EnhancedSabbathVote,
};

static const ErdList_t lightVoteErdList = {
   .erds = lightVotes,
   .numberOfErds = NUM_ELEMENTS(lightVotes)
};

static const EnhancedSabbathModeConfig_t config = {
   .disableMinimumCompressorTimesVoteErd = Erd_DisableMinimumCompressorTimes_EnhancedSabbathVote,
   .compressorSpeedVoteErd = Erd_CompressorSpeed_EnhancedSabbathVote,
   .condenserFanVoteErd = Erd_CondenserFanSpeed_SabbathVote,
   .freezerEvapFanVoteErd = Erd_FreezerEvapFanSpeed_SabbathVote,
   .damperPositionVoteErd = Erd_FreshFoodDamperPosition_EnhancedSabbathVote,
   .enhancedSabbathModeStatusErd = Erd_EnhancedSabbathModeEnable,
   .freshFoodSetpointVoteErd = Erd_FreshFoodSetpoint_EnhancedSabbathVote,
   .freezerSetpointVoteErd = Erd_FreezerSetpoint_EnhancedSabbathVote,
   .freshFoodAverageCabinetTemperatureErd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freezerAverageCabinetTemperatureErd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .freshFoodCabinetSetpointErd = Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100,
   .freezerCabinetSetpointErd = Erd_Freezer_AdjustedSetpointWithoutShiftInDegFx100,
   .overrideIceMakerEnabledRequestErd = Erd_IceMakerEnabledEnhancedSabbathOverrideRequest,
   .overrideIceMakerEnabledValueErd = Erd_IceMakerEnabledEnhancedSabbathOverrideValue,
   .dispensingDisabledErd = Erd_DispensingDisabled,
   .waitingForDefrostErd = Erd_WaitingToDefrost,
   .gridOverrideEnabledErd = Erd_GridOverrideEnable,
   .coolingModeErd = Erd_CoolingMode,
   .hsmStateErd = Erd_EnhancedSabbathModeHsmState,
   .lightVoteErdList = lightVoteErdList
};

TEST_GROUP(EnhancedSabbathMode)
{
   EnhancedSabbathMode_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const EnhancedSabbathData_t *enhancedSabbathData;
   EventSubscription_t dataModelOnChangeSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);

      enhancedSabbathData = PersonalityParametricData_Get(dataModel)->enhancedSabbathData;
   }

   void GivenTheFreshFoodSetpointVoteIs(TemperatureDegFx100_t setpoint, Vote_t care)
   {
      SetpointVotedTemperature_t vote = {
         .temperatureInDegFx100 = setpoint,
         .care = care
      };

      DataModel_Write(dataModel, Erd_FreshFoodSetpoint_EnhancedSabbathVote, &vote);
   }

   void GivenTheFreezerSetpointVoteIs(TemperatureDegFx100_t setpoint, Vote_t care)
   {
      SetpointVotedTemperature_t vote = {
         .temperatureInDegFx100 = setpoint,
         .care = care
      };

      DataModel_Write(dataModel, Erd_FreezerSetpoint_EnhancedSabbathVote, &vote);
   }

   void GivenTheCompressorSpeedVoteIs(CompressorSpeed_t speed, Vote_t care)
   {
      CompressorVotedSpeed_t vote = {
         .speed = speed,
         .care = care
      };

      DataModel_Write(dataModel, Erd_CompressorSpeed_EnhancedSabbathVote, &vote);
   }

   void WhenTheCompressorSpeedVoteChangesTo(CompressorSpeed_t speed, Vote_t care)
   {
      GivenTheCompressorSpeedVoteIs(speed, care);
   }

   void GivenTheCondenserFanSpeedVoteIs(FanSpeed_t speed, Vote_t care)
   {
      FanVotedSpeed_t vote = {
         .speed = speed,
         .care = care
      };

      DataModel_Write(dataModel, Erd_CondenserFanSpeed_SabbathVote, &vote);
   }

   void WhenTheCondenserFanSpeedVoteChangesTo(FanSpeed_t speed, Vote_t care)
   {
      GivenTheCondenserFanSpeedVoteIs(speed, care);
   }

   void GivenTheFreezerEvapFanSpeedVoteIs(FanSpeed_t speed, Vote_t care)
   {
      FanVotedSpeed_t vote = {
         .speed = speed,
         .care = care
      };

      DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_SabbathVote, &vote);
   }

   void WhenTheFreezerEvapFanSpeedVoteChangesTo(FanSpeed_t speed, Vote_t care)
   {
      GivenTheFreezerEvapFanSpeedVoteIs(speed, care);
   }

   void GivenTheDamperPositionVoteIs(DamperPosition_t position, Vote_t care)
   {
      DamperVotedPosition_t vote = {
         .position = position,
         .care = care
      };

      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_EnhancedSabbathVote, &vote);
   }

   void WhenTheDamperPositionVoteChangesTo(DamperPosition_t position, Vote_t care)
   {
      GivenTheDamperPositionVoteIs(position, care);
   }

   void GivenTheDisableCompressorMiniumTimesVoteIs(bool state, Vote_t care)
   {
      BooleanVotedState_t vote = {
         .state = state,
         .care = care
      };

      DataModel_Write(dataModel, Erd_DisableMinimumCompressorTimes_EnhancedSabbathVote, &vote);
   }

   void GivenTheEnhancedSabbathModeStatusIs(bool state)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeEnable, &state);
   }

   void TheEnhancedSabbathModeStatusShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_EnhancedSabbathModeEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheEnhancedSabbathModeStatusChangesTo(bool state)
   {
      GivenTheEnhancedSabbathModeStatusIs(state);
   }

   void GivenTheIceMakerEnableOverrideRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMakerEnabledEnhancedSabbathOverrideRequest, &state);
   }

   void GivenTheIceMakerEnableOverrideValueIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMakerEnabledEnhancedSabbathOverrideValue, &state);
   }

   void GivenDispensingDisabledIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DispensingDisabled, &state);
   }

   void GivenTheGridOverrideIs(bool state)
   {
      DataModel_Write(dataModel, Erd_GridOverrideEnable, &state);
   }

   void GivenTheHsmStateIs(EnhancedSabbathModeHsmState_t state)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeHsmState, &state);
   }

   void GivenTheFreshFoodAverageCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void GivenTheFreezerAverageCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void GivenTheFreshFoodCabinetSetpointIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100, &temperature);
   }

   void GivenTheFreezerCabinetSetpointIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_AdjustedSetpointWithoutShiftInDegFx100, &temperature);
   }

   void TheFreshFoodSetpointVoteShouldBe(TemperatureDegFx100_t expectedSetpoint, Vote_t expectedCare)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodSetpoint_EnhancedSabbathVote, &actual);

      CHECK_EQUAL(expectedSetpoint, actual.temperatureInDegFx100);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void TheFreezerSetpointVoteShouldBe(TemperatureDegFx100_t expectedSetpoint, Vote_t expectedCare)
   {
      SetpointVotedTemperature_t actual;
      DataModel_Read(dataModel, Erd_FreezerSetpoint_EnhancedSabbathVote, &actual);

      CHECK_EQUAL(expectedSetpoint, actual.temperatureInDegFx100);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void TheCompressorSpeedVoteShouldBe(CompressorSpeed_t expectedSpeed, Vote_t expectedCare)
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeed_EnhancedSabbathVote, &actual);

      CHECK_EQUAL(expectedSpeed, actual.speed);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void TheCondenserFanSpeedVoteShouldBe(FanSpeed_t expectedSpeed, Vote_t expectedCare)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_SabbathVote, &actual);

      CHECK_EQUAL(expectedSpeed, actual.speed);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_t expectedSpeed, Vote_t expectedCare)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_SabbathVote, &actual);

      CHECK_EQUAL(expectedSpeed, actual.speed);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void TheDamperPositionVoteShouldBe(DamperPosition_t expectedPosition, Vote_t expectedCare)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_EnhancedSabbathVote, &actual);

      CHECK_EQUAL(expectedPosition, actual.position);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void TheDisableCompressorMiniumTimesVoteShouldBe(bool expectedState, Vote_t expectedCare)
   {
      BooleanVotedState_t actual;
      DataModel_Read(dataModel, Erd_DisableMinimumCompressorTimes_EnhancedSabbathVote, &actual);

      CHECK_EQUAL(expectedState, actual.state);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void TheIceMakerEnableOverrideRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMakerEnabledEnhancedSabbathOverrideRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheIceMakerEnableOverrideValueShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMakerEnabledEnhancedSabbathOverrideValue, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheDispensingDisabledShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DispensingDisabled, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheGridOverrideShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_GridOverrideEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheHsmStateShouldBe(EnhancedSabbathModeHsmState_t expected)
   {
      EnhancedSabbathModeHsmState_t actual;
      DataModel_Read(dataModel, Erd_EnhancedSabbathModeHsmState, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CoolingModeShouldBe(CoolingMode_t expected)
   {
      CoolingMode_t actual;
      DataModel_Read(dataModel, Erd_CoolingMode, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenInitialization()
   {
      EnhancedSabbathMode_Init(
         &instance,
         dataModel,
         &timerModuleTestDouble->timerModule,
         enhancedSabbathData,
         &config);
   }

   void LightPwmVotedDutyCyclesShouldBeOffAndDontCare()
   {
      for(uint8_t i = 0; i < config.lightVoteErdList.numberOfErds; i++)
      {
         RampingPwmDutyCyclePercentageVote_t rampingPwmDutyCyclePercentageVote;
         DataModel_Read(dataModel, config.lightVoteErdList.erds[i], &rampingPwmDutyCyclePercentageVote);

         CHECK_EQUAL(PercentageDutyCycle_Min, rampingPwmDutyCyclePercentageVote.rampingPwmDutyCyclePercentage.pwmDutyCyclePercentage);
         CHECK_EQUAL(Vote_DontCare, rampingPwmDutyCyclePercentageVote.care);
      }
   }

   void LightPwmVotedDutyCyclesShouldBeParametricPwmDutyCyclePercenageAndCare()
   {
      for(uint8_t i = 0; i < config.lightVoteErdList.numberOfErds; i++)
      {
         RampingPwmDutyCyclePercentageVote_t rampingPwmDutyCyclePercentageVote;
         DataModel_Read(dataModel, config.lightVoteErdList.erds[i], &rampingPwmDutyCyclePercentageVote);

         CHECK_EQUAL(enhancedSabbathData->lightsPwmDutyCyclePercentage, rampingPwmDutyCyclePercentageVote.rampingPwmDutyCyclePercentage.pwmDutyCyclePercentage);
         CHECK_EQUAL(Vote_Care, rampingPwmDutyCyclePercentageVote.care);
      }
   }

   void LightVotedRampingUpCountsShouldBeMaxRampingUpCountAndCare()
   {
      for(uint8_t i = 0; i < config.lightVoteErdList.numberOfErds; i++)
      {
         RampingPwmDutyCyclePercentageVote_t rampingPwmDutyCyclePercentageVote;
         DataModel_Read(dataModel, config.lightVoteErdList.erds[i], &rampingPwmDutyCyclePercentageVote);

         CHECK_EQUAL(UINT8_MAX, rampingPwmDutyCyclePercentageVote.rampingPwmDutyCyclePercentage.rampingUpCountInMsec);
         CHECK_EQUAL(Vote_Care, rampingPwmDutyCyclePercentageVote.care);
      }
   }

   void LightVotedRampingDownCountsShouldBeMaxRampingDownCountAndCare()
   {
      for(uint8_t i = 0; i < config.lightVoteErdList.numberOfErds; i++)
      {
         RampingPwmDutyCyclePercentageVote_t rampingPwmDutyCyclePercentageVote;
         DataModel_Read(dataModel, config.lightVoteErdList.erds[i], &rampingPwmDutyCyclePercentageVote);

         CHECK_EQUAL(UINT8_MAX, rampingPwmDutyCyclePercentageVote.rampingPwmDutyCyclePercentage.rampingDownCountInMsec);
         CHECK_EQUAL(Vote_Care, rampingPwmDutyCyclePercentageVote.care);
      }
   }

   void LightVotedRampingUpCountsShouldbeMaxRampingUpCountAndDontCare()
   {
      for(uint8_t i = 0; i < config.lightVoteErdList.numberOfErds; i++)
      {
         RampingPwmDutyCyclePercentageVote_t rampingPwmDutyCyclePercentageVote;
         DataModel_Read(dataModel, config.lightVoteErdList.erds[i], &rampingPwmDutyCyclePercentageVote);

         CHECK_EQUAL(UINT8_MAX, rampingPwmDutyCyclePercentageVote.rampingPwmDutyCyclePercentage.rampingUpCountInMsec);
         CHECK_EQUAL(Vote_DontCare, rampingPwmDutyCyclePercentageVote.care);
      }
   }

   void LightVotedRampingDownCountsShouldbeMaxRampingDownCountAndDontCare()
   {
      for(uint8_t i = 0; i < config.lightVoteErdList.numberOfErds; i++)
      {
         RampingPwmDutyCyclePercentageVote_t rampingPwmDutyCyclePercentageVote;
         DataModel_Read(dataModel, config.lightVoteErdList.erds[i], &rampingPwmDutyCyclePercentageVote);

         CHECK_EQUAL(UINT8_MAX, rampingPwmDutyCyclePercentageVote.rampingPwmDutyCyclePercentage.rampingDownCountInMsec);
         CHECK_EQUAL(Vote_DontCare, rampingPwmDutyCyclePercentageVote.care);
      }
   }

   void GivenInitializationInDisabledState()
   {
      GivenTheEnhancedSabbathModeStatusIs(CLEAR);
      GivenTheFreshFoodSetpointVoteIs(SomeTemperatureInDegFx100, Vote_Care);
      GivenTheFreezerSetpointVoteIs(SomeTemperatureInDegFx100, Vote_Care);
      GivenTheCompressorSpeedVoteIs(CompressorSpeed_SuperHigh, Vote_Care);
      GivenTheCondenserFanSpeedVoteIs(FanSpeed_SuperHigh, Vote_Care);
      GivenTheFreezerEvapFanSpeedVoteIs(FanSpeed_SuperHigh, Vote_Care);
      GivenTheDamperPositionVoteIs(DamperPosition_Open, Vote_Care);
      GivenTheDisableCompressorMiniumTimesVoteIs(false, Vote_Care);
      GivenTheHsmStateIs(EnhancedSabbathModeHsmState_Unknown);
      GivenTheIceMakerEnableOverrideRequestIs(true);
      GivenTheIceMakerEnableOverrideValueIs(false);
      GivenTheGridOverrideIs(true);
      GivenDispensingDisabledIs(true);
      GivenInitialization();

      TheFreshFoodSetpointVoteShouldBe(SomeTemperatureInDegFx100, Vote_DontCare);
      TheFreezerSetpointVoteShouldBe(SomeTemperatureInDegFx100, Vote_DontCare);
      TheCompressorSpeedVoteShouldBe(CompressorSpeed_SuperHigh, Vote_DontCare);
      TheCondenserFanSpeedVoteShouldBe(FanSpeed_SuperHigh, Vote_DontCare);
      TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_SuperHigh, Vote_DontCare);
      TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_DontCare);
      TheDisableCompressorMiniumTimesVoteShouldBe(false, Vote_DontCare);
      TheIceMakerEnableOverrideRequestShouldBe(false);
      TheIceMakerEnableOverrideValueShouldBe(true);
      TheGridOverrideShouldBe(false);
      TheDispensingDisabledShouldBe(false);
      TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);

      LightPwmVotedDutyCyclesShouldBeOffAndDontCare();
      LightVotedRampingUpCountsShouldbeMaxRampingUpCountAndDontCare();
      LightVotedRampingDownCountsShouldbeMaxRampingDownCountAndDontCare();
   }

   void GivenInitializationInFreshFoodStageWithLoadsSetToLow()
   {
      GivenInitializationInDisabledState();
      GivenTheFreshFoodAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 + 1);
      GivenTheFreshFoodCabinetSetpointIs(SomeTemperatureInDegFx100);

      WhenTheEnhancedSabbathModeStatusChangesTo(SET);
      TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
      TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_Care);
      TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
      TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
      TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_Care);
   }

   void GivenInitializationInFreshFoodStageWithLoadsSetToOff()
   {
      GivenInitializationInDisabledState();
      GivenTheFreshFoodAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 - 1);
      GivenTheFreshFoodCabinetSetpointIs(SomeTemperatureInDegFx100);

      WhenTheEnhancedSabbathModeStatusChangesTo(SET);
      TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
      TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
      TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
      TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
      TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);
   }

   void GivenInitializationInTheFreezerStageWithLoadsSetToLow()
   {
      GivenTheFreezerAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 + 1);
      GivenTheFreezerCabinetSetpointIs(SomeTemperatureInDegFx100);
      GivenInitializationInFreshFoodStageWithLoadsSetToOff();

      After(enhancedSabbathData->freshFoodStageTimeInMinutes * MSEC_PER_MIN);
      TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Freezer);
      TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_Care);
      TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
      TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
      TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);
   }

   void GivenInitializationInTheOffStage()
   {
      GivenInitializationInTheFreezerStageWithLoadsSetToLow();

      After(enhancedSabbathData->freezerStageTimeInMinutes * MSEC_PER_MIN);
      TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Off);
      CoolingModeShouldBe(CoolingMode_Off);

      TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
      TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
      TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
      TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }
};

TEST(EnhancedSabbathMode, ShouldVoteEnhancedSabbathPwmPercentageAndRampingUpAndRampingDownCountsPerMsecToCareWhenEnhancedSabbathModeStatusIsSetOnInit)
{
   GivenTheEnhancedSabbathModeStatusIs(SET);
   GivenInitialization();

   LightPwmVotedDutyCyclesShouldBeParametricPwmDutyCyclePercenageAndCare();
   LightVotedRampingUpCountsShouldBeMaxRampingUpCountAndCare();
   LightVotedRampingDownCountsShouldBeMaxRampingDownCountAndCare();
}

TEST(EnhancedSabbathMode, ShouldVoteEnhancedSabbathPwmPercentageAndRampingUpAndRampingDownCountsPerMsecToDontCareThenToCareWhenEnhancedSabbathModeStatusIsClearOnInitAndChangesToSet)
{
   GivenTheEnhancedSabbathModeStatusIs(CLEAR);
   GivenInitialization();
   LightPwmVotedDutyCyclesShouldBeOffAndDontCare();
   LightVotedRampingUpCountsShouldbeMaxRampingUpCountAndDontCare();
   LightVotedRampingDownCountsShouldbeMaxRampingDownCountAndDontCare();

   WhenTheEnhancedSabbathModeStatusChangesTo(SET);
   LightPwmVotedDutyCyclesShouldBeParametricPwmDutyCyclePercenageAndCare();
   LightVotedRampingUpCountsShouldBeMaxRampingUpCountAndCare();
   LightVotedRampingDownCountsShouldBeMaxRampingDownCountAndCare();
}

TEST(EnhancedSabbathMode, ShouldVoteToChangeCountUpAndCountDownCountsPerMsecAndPwmPercentageWhenEnhancedSabbathModeStatusChangesFromSetToClear)
{
   GivenTheEnhancedSabbathModeStatusIs(CLEAR);
   GivenInitialization();
   LightPwmVotedDutyCyclesShouldBeOffAndDontCare();
   LightVotedRampingUpCountsShouldbeMaxRampingUpCountAndDontCare();
   LightVotedRampingDownCountsShouldbeMaxRampingDownCountAndDontCare();

   WhenTheEnhancedSabbathModeStatusChangesTo(SET);
   LightPwmVotedDutyCyclesShouldBeParametricPwmDutyCyclePercenageAndCare();
   LightVotedRampingUpCountsShouldBeMaxRampingUpCountAndCare();
   LightVotedRampingDownCountsShouldBeMaxRampingDownCountAndCare();

   WhenTheEnhancedSabbathModeStatusChangesTo(CLEAR);
   LightPwmVotedDutyCyclesShouldBeOffAndDontCare();
   LightVotedRampingUpCountsShouldbeMaxRampingUpCountAndDontCare();
   LightVotedRampingDownCountsShouldbeMaxRampingDownCountAndDontCare();
}

TEST(EnhancedSabbathMode, ShouldVoteForLoadsToDontCareWhenWhenEnteringDisabledOnInit)
{
   GivenTheFreshFoodSetpointVoteIs(SomeTemperatureInDegFx100, Vote_Care);
   GivenTheFreezerSetpointVoteIs(SomeTemperatureInDegFx100, Vote_Care);
   GivenTheCompressorSpeedVoteIs(CompressorSpeed_SuperHigh, Vote_Care);
   GivenTheCondenserFanSpeedVoteIs(FanSpeed_SuperHigh, Vote_Care);
   GivenTheFreezerEvapFanSpeedVoteIs(FanSpeed_SuperHigh, Vote_Care);
   GivenTheDamperPositionVoteIs(DamperPosition_Open, Vote_Care);
   GivenTheDisableCompressorMiniumTimesVoteIs(false, Vote_Care);
   GivenTheEnhancedSabbathModeStatusIs(CLEAR);
   GivenTheHsmStateIs(EnhancedSabbathModeHsmState_Unknown);
   GivenInitialization();

   TheFreshFoodSetpointVoteShouldBe(SomeTemperatureInDegFx100, Vote_DontCare);
   TheFreezerSetpointVoteShouldBe(SomeTemperatureInDegFx100, Vote_DontCare);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_SuperHigh, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_SuperHigh, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_SuperHigh, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_DontCare);
   TheDisableCompressorMiniumTimesVoteShouldBe(false, Vote_DontCare);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
}

TEST(EnhancedSabbathMode, ShouldEnableIceMakersAndEnableDispensingAndEnableTheGrid)
{
   GivenTheEnhancedSabbathModeStatusIs(CLEAR);
   GivenTheIceMakerEnableOverrideRequestIs(true);
   GivenTheIceMakerEnableOverrideValueIs(false);
   GivenTheGridOverrideIs(true);
   GivenDispensingDisabledIs(true);
   GivenInitialization();

   TheIceMakerEnableOverrideRequestShouldBe(false);
   TheIceMakerEnableOverrideValueShouldBe(true);
   TheGridOverrideShouldBe(false);
   TheDispensingDisabledShouldBe(false);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
}

TEST(EnhancedSabbathMode, ShouldEnterStageFreshFoodAndSetCoolingModeToFreshFoodAndVoteForFreshFoodAndFreezerSetpointsThenDisableDispensingAndIceMakersAndDisableMiniumCompressorTimesThenOverrideGridWhenEnhancedSabbathModeIsEnabled)
{
   GivenInitializationInDisabledState();

   WhenTheEnhancedSabbathModeStatusChangesTo(SET);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   CoolingModeShouldBe(CoolingMode_FreshFood);
   TheFreshFoodSetpointVoteShouldBe(enhancedSabbathData->freshFoodSetpointTemperatureInDegFx100, Vote_Care);
   TheFreezerSetpointVoteShouldBe(enhancedSabbathData->freezerSetpointTemperatureInDegFx100, Vote_Care);
   TheIceMakerEnableOverrideRequestShouldBe(true);
   TheIceMakerEnableOverrideValueShouldBe(false);
   TheGridOverrideShouldBe(true);
   TheDispensingDisabledShouldBe(true);
}

TEST(EnhancedSabbathMode, ShouldEnterStageFreshFoodAndVoteFanAndCompressorOffThenDamperToBeClosedWhenEnhancedSabbathModeIsEnabledAndFreshFoodAverageCabinetTemperatureIsLessThanFreshFoodCabinetSetpoint)
{
   GivenInitializationInDisabledState();
   GivenTheFreshFoodAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 - 1);
   GivenTheFreshFoodCabinetSetpointIs(SomeTemperatureInDegFx100);

   WhenTheEnhancedSabbathModeStatusChangesTo(SET);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);
}

TEST(EnhancedSabbathMode, ShouldEnterStageFreshFoodAndVoteFanAndCompressorLowThenDamperToBeOnWhenEnhancedSabbathModeIsEnabledAndFreshFoodAverageCabinetTemperatureIsGreaterThanFreshFoodCabinetSetpoint)
{
   GivenInitializationInDisabledState();
   GivenTheFreshFoodAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 + 1);
   GivenTheFreshFoodCabinetSetpointIs(SomeTemperatureInDegFx100);

   WhenTheEnhancedSabbathModeStatusChangesTo(SET);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_Care);
}

TEST(EnhancedSabbathMode, ShouldEnterFreezerStageWhenFreshFoodStageTimerEndsAndSetCoolingModeToFreezer)
{
   GivenInitializationInFreshFoodStageWithLoadsSetToLow();

   After(enhancedSabbathData->freshFoodStageTimeInMinutes * MSEC_PER_MIN - 1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   CoolingModeShouldBe(CoolingMode_FreshFood);

   After(1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Freezer);
   CoolingModeShouldBe(CoolingMode_Freezer);
}

TEST(EnhancedSabbathMode, ShouldEnterFreezerStageWhenFreshFoodStageTimerEndsAndShouldVoteLoadsToOffIfFreezerAverageCabinetTemperatureIsLessThanFreezerCabinetSetpoint)
{
   GivenTheFreezerAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 - 1);
   GivenTheFreezerCabinetSetpointIs(SomeTemperatureInDegFx100);
   GivenInitializationInFreshFoodStageWithLoadsSetToLow();

   After(enhancedSabbathData->freshFoodStageTimeInMinutes * MSEC_PER_MIN - 1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_Care);

   After(1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Freezer);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);
}

TEST(EnhancedSabbathMode, ShouldEnterFreezerStageWhenFreshFoodStageTimerEndsAndShouldVoteLoadsToLowIfFreezerAverageCabinetTemperatureIsLessThanFreezerCabinetSetpoint)
{
   GivenTheFreezerAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 + 1);
   GivenTheFreezerCabinetSetpointIs(SomeTemperatureInDegFx100);
   GivenInitializationInFreshFoodStageWithLoadsSetToOff();

   After(enhancedSabbathData->freshFoodStageTimeInMinutes * MSEC_PER_MIN - 1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);

   WhenTheDamperPositionVoteChangesTo(DamperPosition_Open, Vote_Care);

   After(1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Freezer);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);
}

TEST(EnhancedSabbathMode, ShouldEnterOffStageWhenFreezerStageTimerEndsAndSetCoolingModeToOffAndSetLoadsToOff)
{
   GivenInitializationInTheFreezerStageWithLoadsSetToLow();

   After(enhancedSabbathData->freezerStageTimeInMinutes * MSEC_PER_MIN - 1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Freezer);
   CoolingModeShouldBe(CoolingMode_Freezer);

   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);

   WhenTheDamperPositionVoteChangesTo(DamperPosition_Open, Vote_Care);

   After(1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Off);
   CoolingModeShouldBe(CoolingMode_Off);

   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);
}

TEST(EnhancedSabbathMode, ShouldEnterFreshFoodStageWhenOffStageTimerEndsAndSetCoolingModeToFreshFood)
{
   GivenInitializationInTheOffStage();

   After(enhancedSabbathData->offStageTimeInMinutes * MSEC_PER_MIN - 1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Off);
   CoolingModeShouldBe(CoolingMode_Off);

   After(1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   CoolingModeShouldBe(CoolingMode_FreshFood);
}

TEST(EnhancedSabbathMode, ShouldEnterStageFreshFoodFromOffStageAndVoteFanAndCompressorOffThenDamperToBeClosedWhenStageTimerExpiresAndFreshFoodAverageCabinetTemperatureIsLessThanFreshFoodCabinetSetpoint)
{
   GivenInitializationInTheOffStage();
   GivenTheFreshFoodAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 - 1);
   GivenTheFreshFoodCabinetSetpointIs(SomeTemperatureInDegFx100);

   After(enhancedSabbathData->offStageTimeInMinutes * MSEC_PER_MIN - 1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Off);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);

   WhenTheCompressorSpeedVoteChangesTo(CompressorSpeed_SuperHigh, Vote_Care);
   WhenTheCondenserFanSpeedVoteChangesTo(FanSpeed_SuperHigh, Vote_Care);
   WhenTheFreezerEvapFanSpeedVoteChangesTo(FanSpeed_SuperHigh, Vote_Care);
   WhenTheDamperPositionVoteChangesTo(DamperPosition_Open, Vote_Care);

   After(1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);
}

TEST(EnhancedSabbathMode, ShouldEnterStageFreshFoodAndVoteFanAndCompressorLowThenDamperToBeOnWhenStageTimerExpiresAndFreshFoodAverageCabinetTemperatureIsGreaterThanFreshFoodCabinetSetpoint)
{
   GivenInitializationInTheOffStage();
   GivenTheFreshFoodAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 + 1);
   GivenTheFreshFoodCabinetSetpointIs(SomeTemperatureInDegFx100);

   After(enhancedSabbathData->offStageTimeInMinutes * MSEC_PER_MIN - 1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Off);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);

   After(1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_Care);
}

TEST(EnhancedSabbathMode, ShouldBeInFreshFoodStageWithAllLoadsSetToOffWhenInitializedWithEnhancedSabbathModeStatusSet)
{
   GivenTheEnhancedSabbathModeStatusIs(SET);
   GivenTheFreshFoodAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 - 1);
   GivenTheFreshFoodCabinetSetpointIs(SomeTemperatureInDegFx100);
   GivenTheHsmStateIs(EnhancedSabbathModeHsmState_Unknown);
   GivenTheIceMakerEnableOverrideRequestIs(true);
   GivenTheIceMakerEnableOverrideValueIs(false);
   GivenTheGridOverrideIs(true);
   GivenDispensingDisabledIs(true);
   GivenInitialization();

   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   CoolingModeShouldBe(CoolingMode_FreshFood);
   TheFreshFoodSetpointVoteShouldBe(enhancedSabbathData->freshFoodSetpointTemperatureInDegFx100, Vote_Care);
   TheFreezerSetpointVoteShouldBe(enhancedSabbathData->freezerSetpointTemperatureInDegFx100, Vote_Care);
   TheIceMakerEnableOverrideRequestShouldBe(true);
   TheIceMakerEnableOverrideValueShouldBe(false);
   TheGridOverrideShouldBe(true);
   TheDispensingDisabledShouldBe(true);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);
}

TEST(EnhancedSabbathMode, ShouldBeInFreshFoodStageWithAllLoadsSetToLowWhenInitializedWithEnhancedSabbathModeStatusSet)
{
   GivenTheEnhancedSabbathModeStatusIs(SET);
   GivenTheFreshFoodAverageCabinetTemperatureIs(SomeTemperatureInDegFx100 + 1);
   GivenTheFreshFoodCabinetSetpointIs(SomeTemperatureInDegFx100);
   GivenTheIceMakerEnableOverrideRequestIs(true);
   GivenTheIceMakerEnableOverrideValueIs(false);
   GivenTheGridOverrideIs(true);
   GivenDispensingDisabledIs(true);
   GivenInitialization();

   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   CoolingModeShouldBe(CoolingMode_FreshFood);
   TheFreshFoodSetpointVoteShouldBe(enhancedSabbathData->freshFoodSetpointTemperatureInDegFx100, Vote_Care);
   TheFreezerSetpointVoteShouldBe(enhancedSabbathData->freezerSetpointTemperatureInDegFx100, Vote_Care);
   TheIceMakerEnableOverrideRequestShouldBe(true);
   TheIceMakerEnableOverrideValueShouldBe(false);
   TheGridOverrideShouldBe(true);
   TheDispensingDisabledShouldBe(true);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_Care);
}

TEST(EnhancedSabbathMode, ShouldSetSabbathModeToDisabledWhenInFreshFoodStageAndStatusIsCleared)
{
   GivenInitializationInFreshFoodStageWithLoadsSetToLow();

   WhenTheEnhancedSabbathModeStatusChangesTo(CLEAR);
   TheFreshFoodSetpointVoteShouldBe(enhancedSabbathData->freshFoodSetpointTemperatureInDegFx100, Vote_DontCare);
   TheFreezerSetpointVoteShouldBe(enhancedSabbathData->freezerSetpointTemperatureInDegFx100, Vote_DontCare);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_DontCare);
   TheDisableCompressorMiniumTimesVoteShouldBe(true, Vote_DontCare);
   TheIceMakerEnableOverrideRequestShouldBe(false);
   TheIceMakerEnableOverrideValueShouldBe(true);
   TheGridOverrideShouldBe(false);
   TheDispensingDisabledShouldBe(false);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);

   After(enhancedSabbathData->freshFoodStageTimeInMinutes * MSEC_PER_MIN - 1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_DontCare);

   After(1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_DontCare);
}

TEST(EnhancedSabbathMode, ShouldSetSabbathModeToDisabledWhenInFreezerStageAndStatusIsCleared)
{
   GivenInitializationInTheFreezerStageWithLoadsSetToLow();

   WhenTheEnhancedSabbathModeStatusChangesTo(CLEAR);
   TheFreshFoodSetpointVoteShouldBe(enhancedSabbathData->freshFoodSetpointTemperatureInDegFx100, Vote_DontCare);
   TheFreezerSetpointVoteShouldBe(enhancedSabbathData->freezerSetpointTemperatureInDegFx100, Vote_DontCare);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_DontCare);
   TheDisableCompressorMiniumTimesVoteShouldBe(true, Vote_DontCare);
   TheIceMakerEnableOverrideRequestShouldBe(false);
   TheIceMakerEnableOverrideValueShouldBe(true);
   TheGridOverrideShouldBe(false);
   TheDispensingDisabledShouldBe(false);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);

   After(enhancedSabbathData->freezerStageTimeInMinutes * MSEC_PER_MIN - 1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_DontCare);

   After(1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_DontCare);
}

TEST(EnhancedSabbathMode, ShouldSetSabbathModeToDisabledWhenInOffStageAndStatusIsCleared)
{
   GivenInitializationInTheOffStage();

   WhenTheEnhancedSabbathModeStatusChangesTo(CLEAR);
   TheFreshFoodSetpointVoteShouldBe(enhancedSabbathData->freshFoodSetpointTemperatureInDegFx100, Vote_DontCare);
   TheFreezerSetpointVoteShouldBe(enhancedSabbathData->freezerSetpointTemperatureInDegFx100, Vote_DontCare);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_DontCare);
   TheDisableCompressorMiniumTimesVoteShouldBe(true, Vote_DontCare);
   TheIceMakerEnableOverrideRequestShouldBe(false);
   TheIceMakerEnableOverrideValueShouldBe(true);
   TheGridOverrideShouldBe(false);
   TheDispensingDisabledShouldBe(false);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);

   After(enhancedSabbathData->offStageTimeInMinutes * MSEC_PER_MIN - 1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_DontCare);

   After(1);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_DontCare);
}

TEST(EnhancedSabbathMode, ShouldSetEnhancedSabbathModeToDisabledIfMaxTimeHasBeenReached)
{
   GivenInitializationInFreshFoodStageWithLoadsSetToLow();
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);

   After(enhancedSabbathData->maxTimeInEnhancedSabbathModeInMinutes * MSEC_PER_MIN - 1);
   TheEnhancedSabbathModeStatusShouldBe(true);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Off);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);

   TheFreshFoodSetpointVoteShouldBe(enhancedSabbathData->freshFoodSetpointTemperatureInDegFx100, Vote_Care);
   TheFreezerSetpointVoteShouldBe(enhancedSabbathData->freezerSetpointTemperatureInDegFx100, Vote_Care);
   TheDisableCompressorMiniumTimesVoteShouldBe(true, Vote_Care);
   TheIceMakerEnableOverrideRequestShouldBe(true);
   TheIceMakerEnableOverrideValueShouldBe(false);
   TheGridOverrideShouldBe(true);
   TheDispensingDisabledShouldBe(true);

   After(1);
   TheEnhancedSabbathModeStatusShouldBe(false);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_DontCare);

   TheFreshFoodSetpointVoteShouldBe(enhancedSabbathData->freshFoodSetpointTemperatureInDegFx100, Vote_DontCare);
   TheFreezerSetpointVoteShouldBe(enhancedSabbathData->freezerSetpointTemperatureInDegFx100, Vote_DontCare);
   TheDisableCompressorMiniumTimesVoteShouldBe(true, Vote_DontCare);
   TheIceMakerEnableOverrideRequestShouldBe(false);
   TheIceMakerEnableOverrideValueShouldBe(true);
   TheGridOverrideShouldBe(false);
   TheDispensingDisabledShouldBe(false);
}
