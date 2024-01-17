/*!
 * @file
 * @brief Integration tests for Factory Mode
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Rx2xxResetSource.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "VoteType.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   OneMinute = 1,
   TwoMinutes = 2
};

static const Erd_t bspValveErdList[] = {
   Erd_AluminumMoldIceMakerWaterValveRelay,
   Erd_DispenserValveRelay,
   Erd_IsolationValveRelay,
   Erd_TwistTrayIceMakerWaterValveRelay
};

static const Erd_t factoryModeValveErdList[] = {
   Erd_AluminumMoldIceMakerWaterValve_FactoryVote,
   Erd_DispenserWaterValve_FactoryVote,
   Erd_IsolationWaterValve_FactoryVote,
   Erd_TwistTrayIceMakerWaterValve_FactoryVote
};

static const Erd_t bspLightsErdList[] = {
   Erd_FreshFoodBackWallLight_RampingPwm,
   Erd_FreshFoodTopLight_RampingPwm,
   Erd_FreezerBackWallLight_RampingPwm,
   Erd_FreezerTopLight_RampingPwm
};

static const Erd_t factoryModeLightErdList[] = {
   Erd_FreshFoodBackWallLight_FactoryVote,
   Erd_FreshFoodTopLight_FactoryVote,
   Erd_FreezerBackWallLight_FactoryVote,
   Erd_FreezerTopLight_FactoryVote
};

static const Erd_t uint16BspErdList[] = {
   Erd_FreshFoodDamperHeaterPwmDutyCycle
};

static const Erd_t uint8BspErdList[] = {
   Erd_FillTubeHeater,
   Erd_AluminumMoldIceMakerWaterValveRelay,
   Erd_DispenserValveRelay,
   Erd_IsolationValveRelay,
   Erd_TwistTrayIceMakerWaterValveRelay,
   Erd_IceMaker0_RakeMotorRelay,
   Erd_FreezerDefrostHeaterRelay,
   Erd_IceMaker0_HeaterRelay
};

static const FactoryVotePair_t factoryVotePairs[] = {
   { Erd_CompressorSpeed_FactoryVote, CompressorSpeed_Off },
   { Erd_CondenserFanSpeed_FactoryServiceVote, FanSpeed_Off },
   { Erd_FreezerEvapFanSpeed_FactoryServiceVote, FanSpeed_Off },
   { Erd_FreshFoodDefrostHeater_FactoryVote, HeaterState_Off },
   { Erd_FreshFoodDamperPosition_FactoryVote, DamperPosition_Closed },
   { Erd_FreshFoodDamperHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_FillTubeHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_FreezerDefrostHeater_FactoryVote, HeaterState_Off },
   { Erd_AluminumMoldIceMakerWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_IceMaker0_HeaterRelay_FactoryVote, HeaterState_Off },
   { Erd_AluminumMoldIceMakerRakeMotor_FactoryVote, MotorState_Off },
   { Erd_TwistTrayIceMakerWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_TwistTrayIceMakerMotor_FactoryVote, IceMakerMotorAction_RunHomingRoutine },
   { Erd_IsolationWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_AugerMotor_FactoryVote, AugerMotorIceType_Off },
   { Erd_DispenserWaterValve_FactoryVote, WaterValveState_Off },
   { Erd_DisableMinimumCompressorTimes_FactoryVote, true },
};

TEST_GROUP(FactoryModeIntegration)
{
   Application_t instance;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleDouble->timerModule);
   }

   void GivenTheApplicationIsInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void GivenTheFactoryModeEnableRequestIs(uint8_t requestInMinutes)
   {
      DataModel_Write(
         dataModel,
         Erd_FactoryModeEnableRequestInMinutes,
         &requestInMinutes);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks);
   }

   void FactoryModeShouldBeEntered()
   {
      uint8_t factoryModeEnableTimeInMinutes;
      DataModel_Read(
         dataModel,
         Erd_FactoryModeEnableRequestInMinutes,
         &factoryModeEnableTimeInMinutes);

      CHECK_TRUE(factoryModeEnableTimeInMinutes > 0);
   }

   void AllFactoryVoteErdsShouldBeVotedFor()
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(factoryVotePairs); i++)
      {
         uint8_t erdSize = DataModel_SizeOf(
            dataModel,
            factoryVotePairs[i].factoryVoteErd);

         switch(erdSize)
         {
            case sizeof(U8Vote_t): {
               U8Vote_t u8VoteErd;
               DataModel_Read(
                  dataModel,
                  factoryVotePairs[i].factoryVoteErd,
                  &u8VoteErd);

               CHECK_EQUAL(Vote_Care, u8VoteErd.vote);
               CHECK_EQUAL(factoryVotePairs[i].offValue, u8VoteErd.value);
            }
            break;

            case sizeof(U16Vote_t): {
               U16Vote_t u16VoteErd;
               DataModel_Read(
                  dataModel,
                  factoryVotePairs[i].factoryVoteErd,
                  &u16VoteErd);

               CHECK_EQUAL(Vote_Care, u16VoteErd.vote);
               CHECK_EQUAL(factoryVotePairs[i].offValue, u16VoteErd.value);
            }
            break;

            case sizeof(U32Vote_t): {
               U32Vote_t u32VoteErd;
               DataModel_Read(
                  dataModel,
                  factoryVotePairs[i].factoryVoteErd,
                  &u32VoteErd);

               CHECK_EQUAL(Vote_Care, u32VoteErd.vote);
               CHECK_EQUAL(factoryVotePairs[i].offValue, u32VoteErd.value);
            }
            break;
         }
      }
   }

   void AllBspErdsShouldBe(bool expectedStatus)
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(uint8BspErdList); i++)
      {
         uint8_t actualStatus;
         DataModel_Read(
            dataModel,
            uint8BspErdList[i],
            &actualStatus);

         CHECK_EQUAL(expectedStatus, actualStatus);
      }

      for(uint8_t i = 0; i < NUM_ELEMENTS(uint16BspErdList); i++)
      {
         uint16_t actualStatus;
         DataModel_Read(
            dataModel,
            uint16BspErdList[i],
            &actualStatus);

         CHECK_EQUAL(expectedStatus, actualStatus);
      }
   }

   void GivenTheLightBspErdsAre(RampingPwmDutyCycle_t rampingPwmDutyCycle)
   {
      ErdList_t bspListErds = { .erds = bspLightsErdList, .numberOfErds = NUM_ELEMENTS(bspLightsErdList) };
      ErdList_WriteAll(
         dataModel,
         &bspListErds,
         &rampingPwmDutyCycle);
   }

   void WhenTheLightBspErdsAre(RampingPwmDutyCycle_t rampingPwmDutyCycle)
   {
      GivenTheLightBspErdsAre(rampingPwmDutyCycle);
   }

   void TheLightBspErdsShouldBe(PwmDutyCycle_t expectedPwmDutyCycle)
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(bspLightsErdList); i++)
      {
         RampingPwmDutyCycle_t actualPwmDutyCycle;
         DataModel_Read(
            dataModel,
            bspLightsErdList[i],
            &actualPwmDutyCycle);

         CHECK_EQUAL(expectedPwmDutyCycle, actualPwmDutyCycle.pwmDutyCycle);
      }
   }

   void WhenTheFactoryModeLightErdsBecome(RampingPwmDutyCyclePercentage_t pwmDutyCyclePercentage)
   {
      RampingPwmDutyCyclePercentageVote_t vote = { .rampingPwmDutyCyclePercentage = pwmDutyCyclePercentage, .care = Vote_Care };

      ErdList_t factoryModeLightsList = { .erds = factoryModeLightErdList, .numberOfErds = NUM_ELEMENTS(factoryModeLightErdList) };
      ErdList_WriteAll(
         dataModel,
         &factoryModeLightsList,
         &vote);
   }

   void GivenTheValveBspErdsAre(bool status)
   {
      ErdList_t waterValveErdList = { .erds = bspValveErdList, .numberOfErds = NUM_ELEMENTS(bspValveErdList) };
      ErdList_WriteAll(
         dataModel,
         &waterValveErdList,
         &status);
   }

   void WhenTheValveBspErdsAre(bool status)
   {
      GivenTheValveBspErdsAre(status);
   }

   void TheValveBspErdsShouldBe(WaterValveState_t expectedWaterValveState)
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(bspValveErdList); i++)
      {
         WaterValveState_t actualWaterValveState;
         DataModel_Read(
            dataModel,
            bspValveErdList[i],
            &actualWaterValveState);

         CHECK_EQUAL(expectedWaterValveState, actualWaterValveState);
      }
   }

   void WhenTheFactoryModeValveErdsBecome(WaterValveState_t waterValveState)
   {
      WaterValveVotedState_t waterValveVotedState = { .state = waterValveState, .care = Vote_Care };

      ErdList_t factoryModeValveList = { .erds = factoryModeValveErdList, .numberOfErds = NUM_ELEMENTS(factoryModeValveErdList) };
      ErdList_WriteAll(
         dataModel,
         &factoryModeValveList,
         &waterValveVotedState);
   }

   void GivenTheIceMaker0HeaterRelayIs(HeaterState_t heaterState)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_HeaterRelay,
         &heaterState);
   }

   void WhenTheIceMaker0HeaterRelayIs(HeaterState_t heaterState)
   {
      GivenTheIceMaker0HeaterRelayIs(heaterState);
   }

   void WhenTheFactoryModeAluminumMoldIceMakerHeaterRelayBecomes(HeaterState_t heaterState)
   {
      HeaterVotedState_t heaterStateVote = { .state = heaterState, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_HeaterRelay_FactoryVote,
         &heaterStateVote);
   }

   void TheIceMaker0HeaterRelayShouldBe(HeaterState_t expectedHeaterState)
   {
      HeaterState_t actualHeaterState;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_HeaterRelay,
         &actualHeaterState);

      CHECK_EQUAL(expectedHeaterState, actualHeaterState);
   }

   void GivenTheFreezerDefrostHeaterRelayIs(HeaterState_t heaterState)
   {
      DataModel_Write(
         dataModel,
         Erd_FreezerDefrostHeaterRelay,
         &heaterState);
   }

   void WhenTheFreezerDefrostHeaterRelayIs(HeaterState_t heaterState)
   {
      GivenTheFreezerDefrostHeaterRelayIs(heaterState);
   }

   void WhenTheFactoryModeFreezerDefrostHeaterRelayBecomes(HeaterState_t heaterState)
   {
      HeaterVotedState_t heaterVotedState = { .state = heaterState, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_FreezerDefrostHeater_FactoryVote,
         &heaterVotedState);
   }

   void TheFreezerDefrostHeaterRelayShouldBe(HeaterState_t expectedHeaterState)
   {
      HeaterState_t actualHeaterState;
      DataModel_Read(
         dataModel,
         Erd_FreezerDefrostHeaterRelay,
         &actualHeaterState);

      CHECK_EQUAL(expectedHeaterState, actualHeaterState);
   }

   void GivenTheIceMaker0RakeMotorRelayIs(MotorState_t motorState)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_RakeMotorRelay,
         &motorState);
   }

   void WhenTheIceMaker0RakeMotorRelayIs(MotorState_t motorState)
   {
      GivenTheIceMaker0RakeMotorRelayIs(motorState);
   }

   void WhenTheFactoryModeIceMakerRakeMotorIs(MotorState_t motorState)
   {
      AluminumMoldIceMakerMotorVotedState_t motorStateVote = { .state = motorState, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerRakeMotor_FactoryVote,
         &motorStateVote);
   }

   void TheIceMaker0RakeMotorRelayShouldBe(MotorState_t expectedMotorState)
   {
      MotorState_t actualMotorState;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_RakeMotorRelay,
         &actualMotorState);

      CHECK_EQUAL(expectedMotorState, actualMotorState);
   }

   void GivenTheFreshFoodDamperHeaterPwmDutyCycleIs(PwmDutyCycle_t pwmDutyCycle)
   {
      DataModel_Write(
         dataModel,
         Erd_FreshFoodDamperHeaterPwmDutyCycle,
         &pwmDutyCycle);
   }

   void WhenTheFreshFoodDamperHeaterPwmDutyCycleIs(PwmDutyCycle_t pwmDutyCycle)
   {
      GivenTheFreshFoodDamperHeaterPwmDutyCycleIs(pwmDutyCycle);
   }

   void WhenTheFactoryModeFreshFoodDamperHeaterPwmDutyCycleBecomes(PercentageDutyCycle_t percentDutyCycle)
   {
      PercentageDutyCycleVote_t votedDutyCycle = { .percentageDutyCycle = percentDutyCycle, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_FreshFoodDamperHeater_FactoryVote,
         &votedDutyCycle);
   }

   void TheFreshFoodDamperHeaterPwmDutyCycleShouldBe(PwmDutyCycle_t expectedPwmDutyCycle)
   {
      PwmDutyCycle_t actualDutyCycle;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodDamperHeaterPwmDutyCycle,
         &actualDutyCycle);

      CHECK_EQUAL(expectedPwmDutyCycle, actualDutyCycle);
   }

   void WhenTheFillTubeHeaterIs(bool status)
   {
      DataModel_Write(
         dataModel,
         Erd_FillTubeHeater,
         &status);
   }

   void GivenTheFillTubeHeaterIs(bool status)
   {
      WhenTheFillTubeHeaterIs(status);
   }

   void WhenTheFactoryModeFillTubeHeaterBecomes(PercentageDutyCycle_t percentDutyCycle)
   {
      PercentageDutyCycleVote_t votedDutyCycle = { .percentageDutyCycle = percentDutyCycle, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_FillTubeHeater_FactoryVote,
         &votedDutyCycle);
   }

   void TheFillTubeHeaterShouldBe(bool expectedStatus)
   {
      bool actualStatus;
      DataModel_Read(
         dataModel,
         Erd_FillTubeHeater,
         &actualStatus);

      CHECK_EQUAL(expectedStatus, actualStatus);
   }

   void GivenTheApplicationIsInFactoryMode()
   {
      GivenTheFactoryModeEnableRequestIs(TwoMinutes);
      FactoryModeShouldBeEntered();
   }

   void WhenTheApplicationIsInFactoryMode()
   {
      GivenTheApplicationIsInFactoryMode();
   }
};

TEST(FactoryModeIntegration, ShouldTurnAllLoadsOffWhenEnteringFactoryMode)
{
   GivenTheApplicationIsInitialized();
   GivenTheFactoryModeEnableRequestIs(TwoMinutes);
   FactoryModeShouldBeEntered();
   AllFactoryVoteErdsShouldBeVotedFor();
   AllBspErdsShouldBe(OFF);

   After(OneMinute * MSEC_PER_MIN);
   AllFactoryVoteErdsShouldBeVotedFor();
   AllBspErdsShouldBe(OFF);
}

TEST(FactoryModeIntegration, ShouldSetLightBspErdsToPwmDutyCycleMinWhenFactoryModeLightErdsEnabled)
{
   GivenTheApplicationIsInitialized();
   GivenTheLightBspErdsAre({ PwmDutyCycle_Max, UINT8_MAX, UINT8_MAX });

   WhenTheFactoryModeLightErdsBecome({ PwmDutyCycle_Min, UINT8_MAX, UINT8_MAX });
   TheLightBspErdsShouldBe(PwmDutyCycle_Min);

   After(OneMinute * MSEC_PER_MIN);
   TheLightBspErdsShouldBe(PwmDutyCycle_Min);
}

TEST(FactoryModeIntegration, ShouldSetLightBspErdsToPwmDutyCycleMinWhenFactoryModeIsEnabled)
{
   GivenTheApplicationIsInitialized();
   GivenTheLightBspErdsAre({ PwmDutyCycle_Max, UINT8_MAX, UINT8_MAX });

   WhenTheApplicationIsInFactoryMode();
   TheLightBspErdsShouldBe(PwmDutyCycle_Min);
}

TEST(FactoryModeIntegration, ShouldSetWaterValveErdsToOffWhenFactoryModeWaterValveErdsEnabled)
{
   GivenTheApplicationIsInitialized();
   GivenTheValveBspErdsAre(WaterValveState_On);

   WhenTheFactoryModeValveErdsBecome(WaterValveState_Off);
   TheValveBspErdsShouldBe(WaterValveState_Off);

   After(OneMinute * MSEC_PER_MIN);
   TheValveBspErdsShouldBe(WaterValveState_Off);
}

TEST(FactoryModeIntegration, ShouldSetBspIceMaker0HeaterRelayWhenFactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheIceMaker0HeaterRelayIs(HeaterState_On);

   WhenTheFactoryModeAluminumMoldIceMakerHeaterRelayBecomes(HeaterState_Off);
   TheIceMaker0HeaterRelayShouldBe(HeaterState_Off);

   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker0HeaterRelayShouldBe(HeaterState_Off);
}

TEST(FactoryModeIntegration, ShouldSetBspFreezerDefrostHeaterRelayWhenFactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheFreezerDefrostHeaterRelayIs(HeaterState_On);

   WhenTheFactoryModeFreezerDefrostHeaterRelayBecomes(HeaterState_Off);
   TheFreezerDefrostHeaterRelayShouldBe(HeaterState_Off);

   After(OneMinute * MSEC_PER_MIN);
   TheFreezerDefrostHeaterRelayShouldBe(HeaterState_Off);
}

TEST(FactoryModeIntegration, ShouldSetBspIceMaker0RakeMotorRelayWhenFactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheIceMaker0RakeMotorRelayIs(MotorState_On);

   WhenTheFactoryModeIceMakerRakeMotorIs(MotorState_Off);
   TheIceMaker0RakeMotorRelayShouldBe(MotorState_Off);

   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker0RakeMotorRelayShouldBe(MotorState_Off);
}

TEST(FactoryModeIntegration, ShouldSetBspFreshFoodDamperHeaterPwmDutyCycleWhenFactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheFreshFoodDamperHeaterPwmDutyCycleIs(PwmDutyCycle_Max);

   WhenTheFactoryModeFreshFoodDamperHeaterPwmDutyCycleBecomes(PercentageDutyCycle_Min);
   TheFreshFoodDamperHeaterPwmDutyCycleShouldBe(PwmDutyCycle_Min);

   After(OneMinute * MSEC_PER_MIN);
   TheFreshFoodDamperHeaterPwmDutyCycleShouldBe(PwmDutyCycle_Min);
}

TEST(FactoryModeIntegration, ShouldSetBspFillTubeHeaterWhenFactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();
   TheFillTubeHeaterShouldBe(DISABLED);

   WhenTheFactoryModeFillTubeHeaterBecomes(PercentageDutyCycle_Max);
   After(OneMinute * MSEC_PER_MIN);
   TheFillTubeHeaterShouldBe(ENABLED);

   WhenTheFactoryModeFillTubeHeaterBecomes(PercentageDutyCycle_Min);
   TheFillTubeHeaterShouldBe(DISABLED);
}

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspWaterValveErdsWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheValveBspErdsAre(WaterValveState_On);
   TheValveBspErdsShouldBe(WaterValveState_On);

   After(OneMinute * MSEC_PER_MIN);
   TheValveBspErdsShouldBe(WaterValveState_On);
}

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspLightErdsWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheLightBspErdsAre({ PwmDutyCycle_Max, UINT8_MAX, UINT8_MAX });
   TheLightBspErdsShouldBe(PwmDutyCycle_Max);

   After(OneMinute * MSEC_PER_MIN);
   TheLightBspErdsShouldBe(PwmDutyCycle_Max);
}

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspIceMaker0HeaterRelayWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheIceMaker0HeaterRelayIs(HeaterState_On);
   TheIceMaker0HeaterRelayShouldBe(HeaterState_On);

   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker0HeaterRelayShouldBe(HeaterState_On);
}

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspFreezerDefrostHeaterRelayWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheFreezerDefrostHeaterRelayIs(HeaterState_On);
   TheFreezerDefrostHeaterRelayShouldBe(HeaterState_On);

   After(OneMinute * MSEC_PER_MIN);
   TheFreezerDefrostHeaterRelayShouldBe(HeaterState_On);
}

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspIceMaker0RakeMotorRelayWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheIceMaker0RakeMotorRelayIs(MotorState_On);
   TheIceMaker0RakeMotorRelayShouldBe(MotorState_On);

   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker0RakeMotorRelayShouldBe(MotorState_On);
}

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspFreshFoodDamperHeaterPwmDutyCycleWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheFreshFoodDamperHeaterPwmDutyCycleIs(PwmDutyCycle_Max);
   TheFreshFoodDamperHeaterPwmDutyCycleShouldBe(PwmDutyCycle_Max);

   After(OneMinute * MSEC_PER_MIN);
   TheFreshFoodDamperHeaterPwmDutyCycleShouldBe(PwmDutyCycle_Max);
}

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspFillTubeHeaterWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheFillTubeHeaterIs(ENABLED);
   TheFillTubeHeaterShouldBe(ENABLED);

   After(OneMinute * MSEC_PER_MIN);
   TheFillTubeHeaterShouldBe(ENABLED);
}
