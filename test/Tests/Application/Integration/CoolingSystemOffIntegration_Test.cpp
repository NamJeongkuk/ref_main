/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "StackConfigurator.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "Vote.h"
#include "Rx2xxResetSource.h"
#include "PersonalityParametricData.h"
#include "TddPersonality.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

enum
{
   Invalid = false,
   Valid = true,
   InitialTemperatureInDegFx100 = 3200,
   ShortDelayInMs = 5,
};

TEST_GROUP(CoolingSystemOffIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void GivenTheCoolingModeOffRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CoolingOffRequest, &state);
   }

   void WhenTheCoolingModeOffRequestChangesTo(bool state)
   {
      GivenTheCoolingModeOffRequestIs(state);
   }

   void GivenTheCoolingModeOffStatusIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CoolingOffStatus, &state);
   }

   void TheCoolingModeOffStatusShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CoolingOffStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DisableDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DisableDefrost, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheWinningVoteShouldBe(Erd_t winningVoteErd, WinningVoteErd_t expected)
   {
      WinningVoteErd_t actual;
      DataModel_Read(dataModel, winningVoteErd, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheFanVoteShouldBe(Erd_t fanVoteErd, FanSpeed_t expectedFanSpeed, Vote_t expectedCare)
   {
      FanVotedSpeed_t actualVotedSpeed;
      DataModel_Read(dataModel, fanVoteErd, &actualVotedSpeed);

      CHECK_EQUAL(expectedFanSpeed, actualVotedSpeed.speed);
      CHECK_EQUAL(expectedCare, actualVotedSpeed.care);
   }

   void TheHeaterStateVoteShouldBe(Erd_t heaterStateVoteErd, HeaterState_t expectedHeaterState, Vote_t expectedCare)
   {
      HeaterVotedState_t actualVotedHeaterState;
      DataModel_Read(dataModel, heaterStateVoteErd, &actualVotedHeaterState);

      CHECK_EQUAL(expectedHeaterState, actualVotedHeaterState.state);
      CHECK_EQUAL(expectedCare, actualVotedHeaterState.care);
   }

   void ThePercentageDutyCycleVoteShouldBe(Erd_t percentageDutyCycleVoteErd, PercentageDutyCycle_t expectedPercentageDutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t actualPercentageDutyCycleVote;
      DataModel_Read(dataModel, percentageDutyCycleVoteErd, &actualPercentageDutyCycleVote);

      CHECK_EQUAL(expectedPercentageDutyCycle, actualPercentageDutyCycleVote.percentageDutyCycle);
      CHECK_EQUAL(expectedCare, actualPercentageDutyCycleVote.care);
   }

   void TheCompressorSpeedVoteShouldBe(Erd_t compressorSpeedVoteErd, CompressorSpeed_t expectedCompressorSpeed, Vote_t expectedCare)
   {
      CompressorVotedSpeed_t actualCompressorSpeedVote;
      DataModel_Read(dataModel, compressorSpeedVoteErd, &actualCompressorSpeedVote);

      CHECK_EQUAL(expectedCompressorSpeed, actualCompressorSpeedVote.speed);
      CHECK_EQUAL(expectedCare, actualCompressorSpeedVote.care);
   }

   void TheDamperPositionVoteShouldBe(Erd_t damperPositionVoteErd, DamperPosition_t expectedDamperPosition, Vote_t expectedCare)
   {
      DamperVotedPosition_t actualDamperPositionVote;
      DataModel_Read(dataModel, damperPositionVoteErd, &actualDamperPositionVote);

      CHECK_EQUAL(expectedDamperPosition, actualDamperPositionVote.position);
      CHECK_EQUAL(expectedCare, actualDamperPositionVote.care);
   }

   void TheVotesShouldBeTheCoolingSystemOffVotes()
   {
      TheFanVoteShouldBe(Erd_CondenserFanSpeed_CoolingSystemOffVote, FanSpeed_Off, Vote_Care);
      TheFanVoteShouldBe(Erd_FreezerEvapFanSpeed_CoolingSystemOffVote, FanSpeed_Off, Vote_Care);
      TheHeaterStateVoteShouldBe(Erd_FreezerDefrostHeater_CoolingSystemOffVote, HeaterState_Off, Vote_Care);
      ThePercentageDutyCycleVoteShouldBe(Erd_FreshFoodDamperHeater_CoolingSystemOffVote, PercentageDutyCycle_Min, Vote_Care);
      ThePercentageDutyCycleVoteShouldBe(Erd_RecessHeater_CoolingSystemOffVote, PercentageDutyCycle_Min, Vote_Care);
      TheDamperPositionVoteShouldBe(Erd_FreshFoodDamperPosition_CoolingSystemOffVote, DamperPosition_Open, Vote_Care);
      TheCompressorSpeedVoteShouldBe(Erd_CompressorSpeed_CoolingSystemOffVote, CompressorSpeed_Off, Vote_Care);

      TheFanVoteShouldBe(Erd_CondenserFanSpeed_ResolvedVote, FanSpeed_Off, Vote_Care);
      TheFanVoteShouldBe(Erd_FreezerEvapFanSpeed_ResolvedVote, FanSpeed_Off, Vote_Care);
      TheHeaterStateVoteShouldBe(Erd_FreezerDefrostHeater_ResolvedVote, HeaterState_Off, Vote_Care);
      ThePercentageDutyCycleVoteShouldBe(Erd_FreshFoodDamperHeater_ResolvedVote, PercentageDutyCycle_Min, Vote_Care);
      ThePercentageDutyCycleVoteShouldBe(Erd_RecessHeater_ResolvedVote, PercentageDutyCycle_Min, Vote_Care);
      TheDamperPositionVoteShouldBe(Erd_FreshFoodDamperPosition_ResolvedVote, DamperPosition_Open, Vote_Care);
      TheCompressorSpeedVoteShouldBe(Erd_CompressorSpeed_ResolvedVote, CompressorSpeed_Off, Vote_Care);

      TheWinningVoteShouldBe(Erd_CondenserFanSpeed_WinningVoteErd, WinningVoteErd_Erd_CondenserFanSpeed_CoolingSystemOffVote);
      TheWinningVoteShouldBe(Erd_FreezerEvapFanSpeed_WinningVoteErd, WinningVoteErd_Erd_FreezerEvapFanSpeed_CoolingSystemOffVote);
      TheWinningVoteShouldBe(Erd_FreezerDefrostHeater_WinningVoteErd, WinningVoteErd_Erd_FreezerDefrostHeater_CoolingSystemOffVote);
      TheWinningVoteShouldBe(Erd_FreshFoodDamperHeater_WinningVoteErd, WinningVoteErd_Erd_FreshFoodDamperHeater_CoolingSystemOffVote);
      TheWinningVoteShouldBe(Erd_FreshFoodDamperPosition_WinningVoteErd, WinningVoteErd_Erd_FreshFoodDamperPosition_CoolingSystemOffVote);
      TheWinningVoteShouldBe(Erd_CompressorSpeed_WinningVoteErd, WinningVoteErd_Erd_CompressorSpeed_CoolingSystemOffVote);
      TheWinningVoteShouldBe(Erd_RecessHeater_WinningVoteErd, WinningVoteErd_Erd_RecessHeater_CoolingSystemOffVote);
   }

   void TheCoolingSystemOffVotesShouldBeDontCare()
   {
      TheFanVoteShouldBe(Erd_CondenserFanSpeed_CoolingSystemOffVote, FanSpeed_Off, Vote_DontCare);
      TheFanVoteShouldBe(Erd_FreezerEvapFanSpeed_CoolingSystemOffVote, FanSpeed_Off, Vote_DontCare);
      TheHeaterStateVoteShouldBe(Erd_FreezerDefrostHeater_CoolingSystemOffVote, HeaterState_Off, Vote_DontCare);
      ThePercentageDutyCycleVoteShouldBe(Erd_FreshFoodDamperHeater_CoolingSystemOffVote, PercentageDutyCycle_Min, Vote_DontCare);
      ThePercentageDutyCycleVoteShouldBe(Erd_RecessHeater_CoolingSystemOffVote, PercentageDutyCycle_Min, Vote_DontCare);
      TheDamperPositionVoteShouldBe(Erd_FreshFoodDamperPosition_CoolingSystemOffVote, DamperPosition_Open, Vote_DontCare);
      TheCompressorSpeedVoteShouldBe(Erd_CompressorSpeed_CoolingSystemOffVote, CompressorSpeed_Off, Vote_DontCare);
   }
};

TEST(CoolingSystemOffIntegration, ShouldVoteForAllGivenLoadsAndDisableDefrostWhenCoolingSystemIsTurnedOff)
{
   GivenTheCoolingModeOffStatusIs(false);
   GivenApplicationHasBeenInitialized();

   WhenTheCoolingModeOffRequestChangesTo(true);
   TheCoolingModeOffStatusShouldBe(true);
   TheVotesShouldBeTheCoolingSystemOffVotes();
   DisableDefrostShouldBe(ON);
}

TEST(CoolingSystemOffIntegration, ShouldVoteForAllGivenLoadsAndDisableDefrostWhenCoolingSystemIsTurnedOffOnInitialization)
{
   GivenTheCoolingModeOffStatusIs(true);
   GivenApplicationHasBeenInitialized();

   TheVotesShouldBeTheCoolingSystemOffVotes();
   DisableDefrostShouldBe(ON);
}

TEST(CoolingSystemOffIntegration, ShouldVoteForAllGivenLoadsToNotCareAndKeepDefrostEnabledWhenTheCoolingModeIsEnabledOnInitialization)
{
   GivenTheCoolingModeOffStatusIs(false);
   GivenApplicationHasBeenInitialized();

   TheCoolingSystemOffVotesShouldBeDontCare();
   DisableDefrostShouldBe(OFF);
}

TEST(CoolingSystemOffIntegration, ShouldVoteForAllGivenLoadsToNotCareAndEnableDefrostWhenCoolingSystemIsTurnedOn)
{
   GivenTheCoolingModeOffStatusIs(true);
   GivenTheCoolingModeOffRequestIs(true);
   GivenApplicationHasBeenInitialized();

   TheVotesShouldBeTheCoolingSystemOffVotes();
   DisableDefrostShouldBe(ON);

   WhenTheCoolingModeOffRequestChangesTo(false);
   TheCoolingSystemOffVotesShouldBeDontCare();
   DisableDefrostShouldBe(OFF);
}
