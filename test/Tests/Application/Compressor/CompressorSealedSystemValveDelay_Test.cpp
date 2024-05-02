/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CompressorSealedSystemValveDelay.h"
#include "utils.h"
#include "SystemErds.h"
#include "SealedSystemValvePosition.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
}

#include "ReferDataModel_TestDouble.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

static Erd_t fanVoteErds[] = {
   Erd_CondenserFanSpeed_CompressorSealedSystemValveDelayVote,
   Erd_FreezerEvapFanSpeed_CompressorSealedSystemValveDelayVote,
   Erd_FreshFoodEvapFanSpeed_CompressorSealedSystemValveDelayVote,
};

static const CompressorSealedSystemValveDelayConfiguration_t config = {
   .gridCompressorVoteErd = Erd_CompressorSpeed_GridVote,
   .winningVoteErd = Erd_CompressorSpeed_WinningVoteErd,
   .ambientTemperatureIsValidErd = Erd_AmbientTemperature_IsValidResolved,
   .ambientTemperatureErd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .sealedSystemValveVotedPositionErd = Erd_SealedSystemValvePosition_ResolvedVote,
   .compressorDelayVoteErd = Erd_CompressorSpeed_SealedSystemValveDelayVote,
   .fanDelayVotesErdList = {
      .erds = fanVoteErds,
      .numberOfErds = NUM_ELEMENTS(fanVoteErds) }
};

static const CompressorData_t data = {
   .lowAmbientTriggerTemperatureInDegFx100 = 5000,
   .compressorTimes = {
      .remainOffAfterValveMoveTimeInMinutes = 1,
      .remainOffAfterValveMoveLowAmbientTimeInMinutes = 2 }
};

TEST_GROUP(CompressorSealedSystemValveDelay)
{
   CompressorSealedSystemValveDelay_t instance;
   ReferDataModel_TestDouble_t referTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referTestDouble);
      dataModel = referTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referTestDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      CompressorSealedSystemValveDelay_Init(
         &instance,
         dataModel,
         &data,
         &config);
   }

   void GivenTheModuleIsInitializedAndCOmpressorHasBeenVotedOn()
   {
      GivenTheGridVoteIs(CompressorSpeed_Off);
      GivenTheModuleIsInitialized();
      GivenTheGridVoteIs(CompressorSpeed_High);
   }

   void GivenTheDelayVotesAre(
      Vote_t care,
      CompressorSpeed_t compressorSpeed = CompressorSpeed_Off,
      FanSpeed_t fanSpeed = FanSpeed_Off)
   {
      CompressorVotedSpeed_t compressorVote = { .speed = compressorSpeed, .care = care };
      DataModel_Write(
         dataModel,
         config.compressorDelayVoteErd,
         &compressorVote);

      FanVotedSpeed_t fanVote = { .speed = fanSpeed, .care = care };
      ErdList_WriteAll(
         dataModel,
         &config.fanDelayVotesErdList,
         &fanVote);
   }

   void GivenTheSealedSystemValvePositionIs(SealedSystemValvePosition_t position)
   {
      SealedSystemValveVotedPosition_t votedPosition = { .position = position, .care = Vote_Care };

      DataModel_Write(
         dataModel,
         config.sealedSystemValveVotedPositionErd,
         &votedPosition);
   }

   void WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_t position)
   {
      GivenTheSealedSystemValvePositionIs(position);
   }

   void WhenTheGridVoteBecomes(CompressorSpeed_t speed)
   {
      CompressorVotedSpeed_t votedSpeed = { .speed = speed, .care = Vote_Care };

      DataModel_Write(
         dataModel,
         config.gridCompressorVoteErd,
         &votedSpeed);
   }

   void GivenTheGridVoteIs(CompressorSpeed_t speed)
   {
      WhenTheGridVoteBecomes(speed);
   }

   void WhenTheWinningVoteBecomes(Erd_t erd)
   {
      DataModel_Write(
         dataModel,
         config.winningVoteErd,
         &erd);
   }

   void GivenTheWinningVoteIs(Erd_t erd)
   {
      WhenTheWinningVoteBecomes(erd);
   }

   void AllVotesShouldBeDontCare(void)
   {
      CompressorVotedSpeed_t actualCompressorVote;
      DataModel_Read(
         dataModel,
         config.compressorDelayVoteErd,
         &actualCompressorVote);
      CHECK_EQUAL(Vote_DontCare, actualCompressorVote.care);

      FanVotedSpeed_t acualFanVote;
      for(uint8_t i = 0; i < NUM_ELEMENTS(fanVoteErds); i++)
      {
         DataModel_Read(
            dataModel,
            fanVoteErds[i],
            &acualFanVote);
         CHECK_EQUAL(Vote_DontCare, acualFanVote.care);
      }
   }

   void AllVotesShouldBeOff(void)
   {
      CompressorVotedSpeed_t actualCompressorVote;
      DataModel_Read(
         dataModel,
         config.compressorDelayVoteErd,
         &actualCompressorVote);
      CHECK_EQUAL(CompressorSpeed_Off, actualCompressorVote.speed);
      CHECK_EQUAL(Vote_Care, actualCompressorVote.care);

      FanVotedSpeed_t actualFanVote;
      for(uint8_t i = 0; i < NUM_ELEMENTS(fanVoteErds); i++)
      {
         DataModel_Read(
            dataModel,
            fanVoteErds[i],
            &actualFanVote);
         CHECK_EQUAL(FanSpeed_Off, actualFanVote.speed);
         CHECK_EQUAL(Vote_Care, actualFanVote.care);
      }
   }

   void GivenAmbientTemperatureIsValidAnd(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(
         dataModel,
         config.ambientTemperatureIsValidErd,
         set);

      DataModel_Write(
         dataModel,
         config.ambientTemperatureErd,
         &temperature);
   }

   void GivenAmbientTemperatureIsInvalid(void)
   {
      DataModel_Write(
         dataModel,
         config.ambientTemperatureIsValidErd,
         clear);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }
};

TEST(CompressorSealedSystemValveDelay, ShouldNotVoteOffUntilCompressorHasBeenVotedOnOnce)
{
   GivenTheDelayVotesAre(Vote_DontCare);
   GivenTheSealedSystemValvePositionIs(SealedSystemValvePosition_A);
   GivenTheGridVoteIs(CompressorSpeed_High);
   GivenTheWinningVoteIs(config.gridCompressorVoteErd);
   GivenTheModuleIsInitialized();

   WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_B);
   AllVotesShouldBeDontCare();
}

TEST(CompressorSealedSystemValveDelay, ShouldNotVoteIfCompressorIsNotOffAndUnderGridControl)
{
   GivenTheModuleIsInitializedAndCOmpressorHasBeenVotedOn();

   WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_B);
   AllVotesShouldBeDontCare();

   WhenTheGridVoteBecomes(CompressorSpeed_Off);
   WhenTheWinningVoteBecomes(config.compressorDelayVoteErd);
   WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_C);
   AllVotesShouldBeDontCare();

   WhenTheGridVoteBecomes(CompressorSpeed_High);
   WhenTheWinningVoteBecomes(config.gridCompressorVoteErd);
   WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_D);
   AllVotesShouldBeDontCare();
}

TEST(CompressorSealedSystemValveDelay, ShouldVoteForCompressorAndFansToBeOffIfValveMovesWhenCompressorIsOffAndUnderGridControl)
{
   GivenTheDelayVotesAre(Vote_DontCare);
   GivenTheSealedSystemValvePositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitializedAndCOmpressorHasBeenVotedOn();

   WhenTheGridVoteBecomes(CompressorSpeed_Off);
   WhenTheWinningVoteBecomes(config.gridCompressorVoteErd);
   AllVotesShouldBeDontCare();
   WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_B);
   AllVotesShouldBeOff();
}

TEST(CompressorSealedSystemValveDelay, ShouldVoteDontCareCompressorAndFansToBeOffIfValveMovesWhenCompressorIsOffAndUnderGridControlAfterAmbientIsValidAndAtThresholdTimeout)
{
   GivenAmbientTemperatureIsValidAnd(data.lowAmbientTriggerTemperatureInDegFx100);
   GivenTheDelayVotesAre(Vote_DontCare);
   GivenTheSealedSystemValvePositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitializedAndCOmpressorHasBeenVotedOn();

   WhenTheWinningVoteBecomes(config.gridCompressorVoteErd);
   WhenTheGridVoteBecomes(CompressorSpeed_Off);
   WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_B);
   AllVotesShouldBeOff();

   After(data.compressorTimes.remainOffAfterValveMoveLowAmbientTimeInMinutes * MSEC_PER_MIN - 1);
   AllVotesShouldBeOff();

   After(1);
   AllVotesShouldBeDontCare();
}

TEST(CompressorSealedSystemValveDelay, ShouldVoteDontCareCompressorAndFansToBeOffIfValveMovesWhenCompressorIsOffAndUnderGridControlAfterAmbientIsValidAndBelowThresholdTimeout)
{
   GivenAmbientTemperatureIsValidAnd(data.lowAmbientTriggerTemperatureInDegFx100 - 1);
   GivenTheDelayVotesAre(Vote_DontCare);
   GivenTheSealedSystemValvePositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitializedAndCOmpressorHasBeenVotedOn();

   WhenTheWinningVoteBecomes(config.gridCompressorVoteErd);
   WhenTheGridVoteBecomes(CompressorSpeed_Off);
   WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_B);
   AllVotesShouldBeOff();

   After(data.compressorTimes.remainOffAfterValveMoveLowAmbientTimeInMinutes * MSEC_PER_MIN - 1);
   AllVotesShouldBeOff();

   After(1);
   AllVotesShouldBeDontCare();
}

TEST(CompressorSealedSystemValveDelay, ShouldVoteDontCareCompressorAndFansToBeOffIfValveMovesWhenCompressorIsOffAndUnderGridControlAfterAmbientIsValidAndAboveThresholdTimeout)
{
   GivenAmbientTemperatureIsValidAnd(data.lowAmbientTriggerTemperatureInDegFx100 + 1);
   GivenTheDelayVotesAre(Vote_DontCare);
   GivenTheSealedSystemValvePositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitializedAndCOmpressorHasBeenVotedOn();

   WhenTheWinningVoteBecomes(config.gridCompressorVoteErd);
   WhenTheGridVoteBecomes(CompressorSpeed_Off);
   WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_B);
   AllVotesShouldBeOff();

   After(data.compressorTimes.remainOffAfterValveMoveTimeInMinutes * MSEC_PER_MIN - 1);
   AllVotesShouldBeOff();

   After(1);
   AllVotesShouldBeDontCare();
}

TEST(CompressorSealedSystemValveDelay, ShouldVoteDontCareForCompressorAndFansIfValveMovesWhenCompressorIsOffAndUnderGridControlAfterAmbientIsInvalid)
{
   GivenAmbientTemperatureIsInvalid();
   GivenTheDelayVotesAre(Vote_DontCare);
   GivenTheSealedSystemValvePositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitializedAndCOmpressorHasBeenVotedOn();

   WhenTheWinningVoteBecomes(config.gridCompressorVoteErd);
   WhenTheGridVoteBecomes(CompressorSpeed_Off);
   WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_B);
   AllVotesShouldBeOff();

   After(data.compressorTimes.remainOffAfterValveMoveTimeInMinutes * MSEC_PER_MIN - 1);
   AllVotesShouldBeOff();

   After(1);
   AllVotesShouldBeDontCare();
}

TEST(CompressorSealedSystemValveDelay, ShouldContinueToVoteOffIfTimerHasAlreadyStartedEvenIfCompressorLosesGridControlOrGridVotesCompressorOn)
{
   GivenAmbientTemperatureIsValidAnd(data.lowAmbientTriggerTemperatureInDegFx100);
   GivenTheDelayVotesAre(Vote_DontCare);
   GivenTheSealedSystemValvePositionIs(SealedSystemValvePosition_A);
   GivenTheModuleIsInitializedAndCOmpressorHasBeenVotedOn();

   WhenTheWinningVoteBecomes(config.gridCompressorVoteErd);
   WhenTheGridVoteBecomes(CompressorSpeed_Off);
   WhenTheSealedSystemValveBecomes(SealedSystemValvePosition_B);
   AllVotesShouldBeOff();

   After(data.compressorTimes.remainOffAfterValveMoveLowAmbientTimeInMinutes * MSEC_PER_MIN - 1);
   WhenTheWinningVoteBecomes(config.compressorDelayVoteErd);
   WhenTheGridVoteBecomes(CompressorSpeed_High);
   AllVotesShouldBeOff();
}
