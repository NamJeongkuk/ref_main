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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   SomeTemperatureInDegFx100 = 5000,
   EnhancedSabbathFreshFoodTemperatureInDegFx100 = 3700,
   EnhancedSabbathFreezerTemperatureInDegFx100 = 0,
   EnhancedSabbathModeHsmState_Unknown = 0xFF,
   FreshFoodStageTimeInMin = 5,
   FreezerStageTimeInMin = 3,
   OffStageTimeInMin = 2,
   FreshFoodStageTimeInMsec = FreshFoodStageTimeInMin * MSEC_PER_MIN,
   FreezerStageTimeInMsec = FreezerStageTimeInMin * MSEC_PER_MIN,
   OffStageTimeInMsec = OffStageTimeInMin * MSEC_PER_MIN,
   EnhancedSabbathMaxTimeInMin = 4 * MINUTES_PER_DAY,
   EnhancedSabbathMaxTimeInMsec = MSEC_PER_MIN * EnhancedSabbathMaxTimeInMin,
};

static const EnhancedSabbathModeConfig_t config = {
   .disableMinimumCompressorTimesVoteErd = Erd_DisableMinimumCompressorTimes_EnhancedSabbathVote,
   .compressorSpeedVoteErd = Erd_CompressorSpeed_EnhancedSabbathVote,
   .condenserFanVoteErd = Erd_CondenserFanSpeed_SabbathVote,
   .freezerEvapFanVoteErd = Erd_FreezerEvapFanSpeed_SabbathVote,
   .damperPositionVoteErd = Erd_FreshFoodDamperPosition_EnhancedSabbathVote,
   .enhancedSabbathModeStatusErd = Erd_EnhancedSabbathModeStatus,
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
   .sabbathGpioErd = Erd_Gpio_SABBATH,
   .gridOverrideEnabledErd = Erd_GridOverrideEnable,
   .coolingModeErd = Erd_CoolingMode,
   .hsmStateErd = Erd_EnhancedSabbathModeHsmState
};

static const EnhancedSabbathData_t enhancedSabbathData = {
   .maxTimeInEnhancedSabbathModeInMinutes = EnhancedSabbathMaxTimeInMin,
   .freshFoodTemperatureInDegFx100 = EnhancedSabbathFreshFoodTemperatureInDegFx100,
   .freezerTemperatureInDegFx100 = EnhancedSabbathFreezerTemperatureInDegFx100,
   .numberOfFreshFoodDefrostsBeforeFreezerDefrost = 0,
   .minTimeBetweenTemperatureAveragingInMinutes = 0,
   .freshFoodStageTimeInMinutes = FreshFoodStageTimeInMin,
   .freezerStageTimeInMinutes = FreezerStageTimeInMin,
   .offStageTimeInMinutes = OffStageTimeInMin,
};

TEST_GROUP(EnhancedSabbathMode)
{
   EnhancedSabbathMode_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
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
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeStatus, &state);
   }

   void TheEnhancedSabbathModeStatusShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_EnhancedSabbathModeStatus, &actual);

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

   void GivenSabbathGpioIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Gpio_SABBATH, &state);
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

   void SabbathGpioShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_Gpio_SABBATH, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenInitialization()
   {
      EnhancedSabbathMode_Init(
         &instance,
         dataModel,
         &timerModuleTestDouble->timerModule,
         &enhancedSabbathData,
         &config);
   }

   void GivenInitializationInDisabledState()
   {
      GivenTheEnhancedSabbathModeStatusIs(CLEAR);
      GivenSabbathGpioIs(false);
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
      SabbathGpioShouldBe(true);
      TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
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

      After(FreshFoodStageTimeInMsec);
      TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Freezer);
      TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_Care);
      TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
      TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
      TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);
   }

   void GivenInitializationInTheOffStage()
   {
      GivenInitializationInTheFreezerStageWithLoadsSetToLow();

      After(FreezerStageTimeInMsec);
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

TEST(EnhancedSabbathMode, ShouldEnableIceMakersAndEnableDispensingAndSetSabbathGpioToTrueAndEnableTheGrid)
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
   SabbathGpioShouldBe(true);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
}

TEST(EnhancedSabbathMode, ShouldEnterStageFreshFoodAndSetCoolingModeToFreshFoodAndVoteForFreshFoodAndFreezerSetpointsThenDisableDispensingAndIceMakersAndDisableMiniumCompressorTimesThenOverrideGridThenSetSabbathGpioToFalseWhenEnhancedSabbathModeIsEnabled)
{
   GivenInitializationInDisabledState();

   WhenTheEnhancedSabbathModeStatusChangesTo(SET);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   CoolingModeShouldBe(CoolingMode_FreshFood);
   TheFreshFoodSetpointVoteShouldBe(EnhancedSabbathFreshFoodTemperatureInDegFx100, Vote_Care);
   TheFreezerSetpointVoteShouldBe(EnhancedSabbathFreezerTemperatureInDegFx100, Vote_Care);
   TheIceMakerEnableOverrideRequestShouldBe(true);
   TheIceMakerEnableOverrideValueShouldBe(false);
   TheGridOverrideShouldBe(true);
   TheDispensingDisabledShouldBe(true);
   SabbathGpioShouldBe(false);
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

   After(FreshFoodStageTimeInMsec - 1);
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

   After(FreshFoodStageTimeInMsec - 1);
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

   After(FreshFoodStageTimeInMsec - 1);
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

   After(FreezerStageTimeInMsec - 1);
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

   After(OffStageTimeInMsec - 1);
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

   After(OffStageTimeInMsec - 1);
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

   After(OffStageTimeInMsec - 1);
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
   GivenSabbathGpioIs(false);
   GivenTheHsmStateIs(EnhancedSabbathModeHsmState_Unknown);
   GivenTheIceMakerEnableOverrideRequestIs(true);
   GivenTheIceMakerEnableOverrideValueIs(false);
   GivenTheGridOverrideIs(true);
   GivenDispensingDisabledIs(true);
   GivenInitialization();

   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   CoolingModeShouldBe(CoolingMode_FreshFood);
   TheFreshFoodSetpointVoteShouldBe(EnhancedSabbathFreshFoodTemperatureInDegFx100, Vote_Care);
   TheFreezerSetpointVoteShouldBe(EnhancedSabbathFreezerTemperatureInDegFx100, Vote_Care);
   TheIceMakerEnableOverrideRequestShouldBe(true);
   TheIceMakerEnableOverrideValueShouldBe(false);
   TheGridOverrideShouldBe(true);
   TheDispensingDisabledShouldBe(true);
   SabbathGpioShouldBe(false);
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
   GivenSabbathGpioIs(false);
   GivenTheIceMakerEnableOverrideRequestIs(true);
   GivenTheIceMakerEnableOverrideValueIs(false);
   GivenTheGridOverrideIs(true);
   GivenDispensingDisabledIs(true);
   GivenInitialization();

   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_FreshFood);
   CoolingModeShouldBe(CoolingMode_FreshFood);
   TheFreshFoodSetpointVoteShouldBe(EnhancedSabbathFreshFoodTemperatureInDegFx100, Vote_Care);
   TheFreezerSetpointVoteShouldBe(EnhancedSabbathFreezerTemperatureInDegFx100, Vote_Care);
   TheIceMakerEnableOverrideRequestShouldBe(true);
   TheIceMakerEnableOverrideValueShouldBe(false);
   TheGridOverrideShouldBe(true);
   TheDispensingDisabledShouldBe(true);
   SabbathGpioShouldBe(false);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_Care);
}

TEST(EnhancedSabbathMode, ShouldSetSabbathModeToDisabledWhenInFreshFoodStageAndStatusIsCleared)
{
   GivenInitializationInFreshFoodStageWithLoadsSetToLow();

   WhenTheEnhancedSabbathModeStatusChangesTo(CLEAR);
   TheFreshFoodSetpointVoteShouldBe(EnhancedSabbathFreshFoodTemperatureInDegFx100, Vote_DontCare);
   TheFreezerSetpointVoteShouldBe(EnhancedSabbathFreezerTemperatureInDegFx100, Vote_DontCare);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Open, Vote_DontCare);
   TheDisableCompressorMiniumTimesVoteShouldBe(true, Vote_DontCare);
   TheIceMakerEnableOverrideRequestShouldBe(false);
   TheIceMakerEnableOverrideValueShouldBe(true);
   TheGridOverrideShouldBe(false);
   TheDispensingDisabledShouldBe(false);
   SabbathGpioShouldBe(true);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);

   After(FreshFoodStageTimeInMsec - 1);
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
   TheFreshFoodSetpointVoteShouldBe(EnhancedSabbathFreshFoodTemperatureInDegFx100, Vote_DontCare);
   TheFreezerSetpointVoteShouldBe(EnhancedSabbathFreezerTemperatureInDegFx100, Vote_DontCare);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Low, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_DontCare);
   TheDisableCompressorMiniumTimesVoteShouldBe(true, Vote_DontCare);
   TheIceMakerEnableOverrideRequestShouldBe(false);
   TheIceMakerEnableOverrideValueShouldBe(true);
   TheGridOverrideShouldBe(false);
   TheDispensingDisabledShouldBe(false);
   SabbathGpioShouldBe(true);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);

   After(FreezerStageTimeInMsec - 1);
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
   TheFreshFoodSetpointVoteShouldBe(EnhancedSabbathFreshFoodTemperatureInDegFx100, Vote_DontCare);
   TheFreezerSetpointVoteShouldBe(EnhancedSabbathFreezerTemperatureInDegFx100, Vote_DontCare);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_DontCare);
   TheDisableCompressorMiniumTimesVoteShouldBe(true, Vote_DontCare);
   TheIceMakerEnableOverrideRequestShouldBe(false);
   TheIceMakerEnableOverrideValueShouldBe(true);
   TheGridOverrideShouldBe(false);
   TheDispensingDisabledShouldBe(false);
   SabbathGpioShouldBe(true);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);

   After(OffStageTimeInMsec - 1);
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

   After(EnhancedSabbathMaxTimeInMsec - 1);
   TheEnhancedSabbathModeStatusShouldBe(true);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Stage_Off);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_Care);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_Care);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_Care);

   TheFreshFoodSetpointVoteShouldBe(EnhancedSabbathFreshFoodTemperatureInDegFx100, Vote_Care);
   TheFreezerSetpointVoteShouldBe(EnhancedSabbathFreezerTemperatureInDegFx100, Vote_Care);
   TheDisableCompressorMiniumTimesVoteShouldBe(true, Vote_Care);
   TheIceMakerEnableOverrideRequestShouldBe(true);
   TheIceMakerEnableOverrideValueShouldBe(false);
   TheGridOverrideShouldBe(true);
   TheDispensingDisabledShouldBe(true);
   SabbathGpioShouldBe(false);

   After(1);
   TheEnhancedSabbathModeStatusShouldBe(false);
   TheHsmStateShouldBe(EnhancedSabbathModeHsmState_Disabled);
   TheCompressorSpeedVoteShouldBe(CompressorSpeed_Off, Vote_DontCare);
   TheCondenserFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheFreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off, Vote_DontCare);
   TheDamperPositionVoteShouldBe(DamperPosition_Closed, Vote_DontCare);

   TheFreshFoodSetpointVoteShouldBe(EnhancedSabbathFreshFoodTemperatureInDegFx100, Vote_DontCare);
   TheFreezerSetpointVoteShouldBe(EnhancedSabbathFreezerTemperatureInDegFx100, Vote_DontCare);
   TheDisableCompressorMiniumTimesVoteShouldBe(true, Vote_DontCare);
   TheIceMakerEnableOverrideRequestShouldBe(false);
   TheIceMakerEnableOverrideValueShouldBe(true);
   TheGridOverrideShouldBe(false);
   TheDispensingDisabledShouldBe(false);
   SabbathGpioShouldBe(true);
}
