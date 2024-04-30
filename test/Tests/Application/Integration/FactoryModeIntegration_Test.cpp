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
#include "EventQueueInterruptSafePlugin.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "Interrupt_TestDouble.h"
#include <stdio.h>

enum
{
   OneMinute = 1,
   TwoMinutes = 2
};

static const Erd_t bspValveErdList[] = {
   Erd_IceMaker0_WaterValveRelay,
   Erd_IceMaker1_WaterValveRelay,
   Erd_DispenserValveRelay,
   Erd_IsolationValveRelay,
};

static const Erd_t factoryModeValveErdList[] = {
   Erd_IceMaker0_WaterValve_FactoryVote,
   Erd_IceMaker1_WaterValve_FactoryVote,
   Erd_DispenserWaterValve_FactoryVote,
   Erd_IsolationWaterValve_FactoryVote,
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
   Erd_DamperHeater_Pwm
};

static const Erd_t uint8BspErdList[] = {
   Erd_IceMaker0_FillTubeHeater,
   Erd_IceMaker0_WaterValveRelay,
   Erd_IceMaker1_WaterValveRelay,
   Erd_DispenserValveRelay,
   Erd_IsolationValveRelay,
   Erd_IceMaker1_RakeMotorRelay,
   Erd_FreezerDefrostHeaterRelay,
   Erd_IceMaker1_HeaterRelay,
   Erd_TwistIceMakerMotorDriveEnable
};

static const ErdOffValuePair_t factoryVotePairs[] = {
   { Erd_CompressorSpeed_FactoryVote, CompressorSpeed_Off },
   { Erd_CondenserFanSpeed_FactoryServiceVote, FanSpeed_Off },
   { Erd_FreezerEvapFanSpeed_FactoryServiceVote, FanSpeed_Off },
   { Erd_FreshFoodDefrostHeater_FactoryVote, HeaterState_Off },
   { Erd_FreshFoodDamperPosition_FactoryVote, DamperPosition_Closed },
   { Erd_DamperHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_IceMaker0_FillTubeHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_IceMaker1_FillTubeHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_IceMaker2_FillTubeHeater_FactoryVote, PercentageDutyCycle_Min },
   { Erd_FreezerDefrostHeater_FactoryVote, HeaterState_Off },
   { Erd_IceMaker0_WaterValve_FactoryVote, WaterValveState_Off },
   { Erd_IceMaker1_WaterValve_FactoryVote, WaterValveState_Off },
   { Erd_IceMaker2_WaterValve_FactoryVote, WaterValveState_Off },
   { Erd_IceMaker0_HeaterRelay_FactoryVote, HeaterState_Off },
   { Erd_IceMaker1_HeaterRelay_FactoryVote, HeaterState_Off },
   { Erd_IceMaker2_HeaterRelay_FactoryVote, HeaterState_Off },
   { Erd_IceMaker0_RakeMotor_FactoryVote, IceMakerMotorState_Off },
   { Erd_IceMaker1_RakeMotor_FactoryVote, IceMakerMotorState_Off },
   { Erd_IceMaker2_RakeMotor_FactoryVote, IceMakerMotorState_Off },
   { Erd_IceMaker0_TwistMotor_FactoryVote, IceMakerMotorState_Homing },
   { Erd_IceMaker1_TwistMotor_FactoryVote, IceMakerMotorState_Homing },
   { Erd_IceMaker2_TwistMotor_FactoryVote, IceMakerMotorState_Homing },
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
   Interrupt_TestDouble_t *interruptTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleDouble->timerModule);
      interruptTestDouble = (Interrupt_TestDouble_t *)DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_FastTickInterrupt);
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
            factoryVotePairs[i].erd);

         switch(erdSize)
         {
            case sizeof(U8Vote_t): {
               U8Vote_t u8VoteErd;
               DataModel_Read(
                  dataModel,
                  factoryVotePairs[i].erd,
                  &u8VoteErd);

               CHECK_EQUAL(Vote_Care, u8VoteErd.vote);
               CHECK_EQUAL(factoryVotePairs[i].offValue, u8VoteErd.value);
            }
            break;

            case sizeof(U16Vote_t): {
               U16Vote_t u16VoteErd;
               DataModel_Read(
                  dataModel,
                  factoryVotePairs[i].erd,
                  &u16VoteErd);

               CHECK_EQUAL(Vote_Care, u16VoteErd.vote);
               CHECK_EQUAL(factoryVotePairs[i].offValue, u16VoteErd.value);
            }
            break;

            case sizeof(U32Vote_t): {
               U32Vote_t u32VoteErd;
               DataModel_Read(
                  dataModel,
                  factoryVotePairs[i].erd,
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

   void GivenTheIceMaker1HeaterRelayIs(HeaterState_t heaterState)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_HeaterRelay,
         &heaterState);
   }

   void WhenTheIceMaker1HeaterRelayIs(HeaterState_t heaterState)
   {
      GivenTheIceMaker1HeaterRelayIs(heaterState);
   }

   void WhenTheFactoryModeIceMaker1HeaterRelayBecomes(HeaterState_t heaterState)
   {
      HeaterVotedState_t heaterStateVote = { .state = heaterState, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_HeaterRelay_FactoryVote,
         &heaterStateVote);
   }

   void TheIceMaker1HeaterRelayShouldBe(HeaterState_t expectedHeaterState)
   {
      HeaterState_t actualHeaterState;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_HeaterRelay,
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

   void GivenTheIceMaker1RakeMotorRelayIs(IceMakerMotorState_t motorState)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_RakeMotorRelay,
         &motorState);
   }

   void WhenTheIceMaker1RakeMotorRelayIs(IceMakerMotorState_t motorState)
   {
      GivenTheIceMaker1RakeMotorRelayIs(motorState);
   }

   void WhenTheFactoryModeIceMaker1RakeMotorIs(IceMakerMotorState_t motorState)
   {
      IceMakerMotorVotedState_t motorStateVote = { .state = motorState, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_RakeMotor_FactoryVote,
         &motorStateVote);
   }

   void TheIceMaker1RakeMotorRelayShouldBe(IceMakerMotorState_t expectedMotorState)
   {
      IceMakerMotorState_t actualMotorState;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_RakeMotorRelay,
         &actualMotorState);

      CHECK_EQUAL(expectedMotorState, actualMotorState);
   }

   void GivenTheDamperHeaterPwmDutyCycleIs(PwmDutyCycle_t pwmDutyCycle)
   {
      DataModel_Write(
         dataModel,
         Erd_DamperHeater_Pwm,
         &pwmDutyCycle);
   }

   void WhenTheDamperHeaterPwmDutyCycleIs(PwmDutyCycle_t pwmDutyCycle)
   {
      GivenTheDamperHeaterPwmDutyCycleIs(pwmDutyCycle);
   }

   void WhenTheFactoryModeDamperHeaterPwmDutyCycleBecomes(PercentageDutyCycle_t percentDutyCycle)
   {
      PercentageDutyCycleVote_t votedDutyCycle = { .percentageDutyCycle = percentDutyCycle, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_DamperHeater_FactoryVote,
         &votedDutyCycle);
   }

   void TheDamperHeaterPwmDutyCycleShouldBe(PwmDutyCycle_t expectedPwmDutyCycle)
   {
      PwmDutyCycle_t actualDutyCycle;
      DataModel_Read(
         dataModel,
         Erd_DamperHeater_Pwm,
         &actualDutyCycle);

      CHECK_EQUAL(expectedPwmDutyCycle, actualDutyCycle);
   }

   void WhenTheIceMaker0FillTubeHeaterIs(bool status)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_FillTubeHeater,
         &status);
   }

   void GivenTheIceMaker0FillTubeHeaterIs(bool status)
   {
      WhenTheIceMaker0FillTubeHeaterIs(status);
   }

   void WhenTheIceMaker1FillTubeHeaterIs(bool status)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_FillTubeHeater,
         &status);
   }

   void GivenTheIceMaker1FillTubeHeaterIs(bool status)
   {
      WhenTheIceMaker1FillTubeHeaterIs(status);
   }

   void WhenTheFactoryModeIceMaker0FillTubeHeaterBecomes(PercentageDutyCycle_t percentDutyCycle)
   {
      PercentageDutyCycleVote_t votedDutyCycle = { .percentageDutyCycle = percentDutyCycle, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_FillTubeHeater_FactoryVote,
         &votedDutyCycle);
   }

   void WhenTheFactoryModeIceMaker1FillTubeHeaterBecomes(PercentageDutyCycle_t percentDutyCycle)
   {
      PercentageDutyCycleVote_t votedDutyCycle = { .percentageDutyCycle = percentDutyCycle, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_FillTubeHeater_FactoryVote,
         &votedDutyCycle);
   }

   void TheIceMaker0FillTubeHeaterShouldBe(bool expectedStatus)
   {
      bool actualStatus;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_FillTubeHeater,
         &actualStatus);

      CHECK_EQUAL(expectedStatus, actualStatus);
   }

   void TheIceMaker1FillTubeHeaterShouldBe(bool expectedStatus)
   {
      bool actualStatus;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_FillTubeHeater,
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

   void TheTwistIceMakerMotorDriveEnableShouldBe(bool expectedStatus)
   {
      bool actualStatus;
      DataModel_Read(dataModel, Erd_TwistIceMakerMotorDriveEnable, &actualStatus);

      CHECK_EQUAL(expectedStatus, actualStatus);
   }

   void WhenTheFactoryModeIceMaker0TwistMotorIs(IceMakerMotorState_t iceMakerMotorState)
   {
      IceMakerMotorVotedState_t votedAction = { .state = iceMakerMotorState, .care = Vote_Care };
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_TwistMotor_FactoryVote,
         &votedAction);
   }

   void GivenTheDamperStepperMotorDriveEnableIs(bool status)
   {
      DataModel_Write(dataModel, Erd_DamperStepperMotorDriveEnable, &status);
   }

   void GivenTheTwistIceMakerMotorControlRequestIs(bool request)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_TwistMotorControlRequest, &request);
   }

   void AfterNInterrupts(int numberOfInterrupts)
   {
      for(int i = 0; i < numberOfInterrupts; i++)
      {
         Interrupt_TestDouble_TriggerInterrupt(interruptTestDouble);
      }
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

TEST(FactoryModeIntegration, ShouldSetLightBspErdsToPwmDutyCycleMaxWhenFactoryModeLightErdsEnabled)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();
   GivenTheLightBspErdsAre({ PwmDutyCycle_Min, UINT8_MAX, UINT8_MAX });

   WhenTheFactoryModeLightErdsBecome({ PercentageDutyCycle_Max, UINT8_MAX, UINT8_MAX });
   TheLightBspErdsShouldBe(PwmDutyCycle_Max);

   After(OneMinute * MSEC_PER_MIN);
   TheLightBspErdsShouldBe(PwmDutyCycle_Max);
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
   GivenTheApplicationIsInFactoryMode();
   GivenTheValveBspErdsAre(WaterValveState_Off);

   WhenTheFactoryModeValveErdsBecome(WaterValveState_On);
   TheValveBspErdsShouldBe(WaterValveState_On);

   After(OneMinute * MSEC_PER_MIN);
   TheValveBspErdsShouldBe(WaterValveState_On);
}

TEST(FactoryModeIntegration, ShouldSetBspIceMaker1HeaterRelayWhenIceMaker1FactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();
   GivenTheIceMaker1HeaterRelayIs(HeaterState_Off);

   WhenTheFactoryModeIceMaker1HeaterRelayBecomes(HeaterState_On);
   TheIceMaker1HeaterRelayShouldBe(HeaterState_On);

   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker1HeaterRelayShouldBe(HeaterState_On);
}

TEST(FactoryModeIntegration, ShouldSetBspFreezerDefrostHeaterRelayWhenFactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();
   GivenTheFreezerDefrostHeaterRelayIs(HeaterState_Off);

   WhenTheFactoryModeFreezerDefrostHeaterRelayBecomes(HeaterState_On);
   TheFreezerDefrostHeaterRelayShouldBe(HeaterState_On);

   After(OneMinute * MSEC_PER_MIN);
   TheFreezerDefrostHeaterRelayShouldBe(HeaterState_On);
}

TEST(FactoryModeIntegration, ShouldSetBspIceMaker1RakeMotorRelayWhenIceMaker1FactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();
   GivenTheIceMaker1RakeMotorRelayIs(IceMakerMotorState_Off);

   WhenTheFactoryModeIceMaker1RakeMotorIs(IceMakerMotorState_Run);
   TheIceMaker1RakeMotorRelayShouldBe(IceMakerMotorState_Run);

   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker1RakeMotorRelayShouldBe(IceMakerMotorState_Run);
}

TEST(FactoryModeIntegration, ShouldSetBspDamperHeaterPwmDutyCycleWhenFactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();
   GivenTheDamperHeaterPwmDutyCycleIs(PwmDutyCycle_Min);

   WhenTheFactoryModeDamperHeaterPwmDutyCycleBecomes(PercentageDutyCycle_Max);
   TheDamperHeaterPwmDutyCycleShouldBe(PwmDutyCycle_Max);

   After(OneMinute * MSEC_PER_MIN);
   TheDamperHeaterPwmDutyCycleShouldBe(PwmDutyCycle_Max);
}

TEST(FactoryModeIntegration, ShouldSetBspFillTubeHeaterWhenIceMaker0FactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();
   TheIceMaker0FillTubeHeaterShouldBe(DISABLED);

   WhenTheFactoryModeIceMaker0FillTubeHeaterBecomes(PercentageDutyCycle_Max);
   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker0FillTubeHeaterShouldBe(ENABLED);

   WhenTheFactoryModeIceMaker0FillTubeHeaterBecomes(PercentageDutyCycle_Min);
   TheIceMaker0FillTubeHeaterShouldBe(DISABLED);
}

TEST(FactoryModeIntegration, ShouldSetBspFillTubeHeaterWhenIceMaker1FactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();
   TheIceMaker1FillTubeHeaterShouldBe(DISABLED);

   WhenTheFactoryModeIceMaker1FillTubeHeaterBecomes(PercentageDutyCycle_Max);
   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker1FillTubeHeaterShouldBe(ENABLED);

   WhenTheFactoryModeIceMaker1FillTubeHeaterBecomes(PercentageDutyCycle_Min);
   TheIceMaker1FillTubeHeaterShouldBe(DISABLED);
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

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspIceMaker1HeaterRelayWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheIceMaker1HeaterRelayIs(HeaterState_On);
   TheIceMaker1HeaterRelayShouldBe(HeaterState_On);

   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker1HeaterRelayShouldBe(HeaterState_On);
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

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspIceMaker1RakeMotorRelayWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheIceMaker1RakeMotorRelayIs(IceMakerMotorState_Run);
   TheIceMaker1RakeMotorRelayShouldBe(IceMakerMotorState_Run);

   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker1RakeMotorRelayShouldBe(IceMakerMotorState_Run);
}

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspDamperHeaterPwmDutyCycleWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheDamperHeaterPwmDutyCycleIs(PwmDutyCycle_Max);
   TheDamperHeaterPwmDutyCycleShouldBe(PwmDutyCycle_Max);

   After(OneMinute * MSEC_PER_MIN);
   TheDamperHeaterPwmDutyCycleShouldBe(PwmDutyCycle_Max);
}

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspIceMaker0FillTubeHeaterWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheIceMaker0FillTubeHeaterIs(ENABLED);
   TheIceMaker0FillTubeHeaterShouldBe(ENABLED);

   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker0FillTubeHeaterShouldBe(ENABLED);
}

TEST(FactoryModeIntegration, ShouldDirectlyModifyBspIceMaker1FillTubeHeaterWithoutOtherLogicOverride)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();

   WhenTheIceMaker1FillTubeHeaterIs(ENABLED);
   TheIceMaker1FillTubeHeaterShouldBe(ENABLED);

   After(OneMinute * MSEC_PER_MIN);
   TheIceMaker1FillTubeHeaterShouldBe(ENABLED);
}

TEST(FactoryModeIntegration, ShouldSetBspTwistMotorWhenIceMaker0FactoryVoteTriggered)
{
   GivenTheApplicationIsInitialized();
   GivenTheApplicationIsInFactoryMode();
   GivenTheTwistIceMakerMotorControlRequestIs(CLEAR);
   GivenTheDamperStepperMotorDriveEnableIs(DISABLED);
   TheTwistIceMakerMotorDriveEnableShouldBe(DISABLED);

   WhenTheFactoryModeIceMaker0TwistMotorIs(IceMakerMotorState_Run);
   TheTwistIceMakerMotorDriveEnableShouldBe(ENABLED);

   After(OneMinute * MSEC_PER_MIN);
   TheTwistIceMakerMotorDriveEnableShouldBe(ENABLED);
}

TEST_GROUP(FactoryModeIntegration_FourDoor)
{
   Application_t instance;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleDouble;
   Interrupt_TestDouble_t *interruptTestDouble;
   const SealedSystemValveData_t *valveData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvapFourDoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleDouble->timerModule);
      interruptTestDouble = (Interrupt_TestDouble_t *)DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_SystemTickInterrupt);
      valveData = PersonalityParametricData_Get(dataModel)->sealedSystemValveData;
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

   void AfterNInterrupts(int numberOfInterrupts)
   {
      for(int i = 0; i < numberOfInterrupts; i++)
      {
         Interrupt_TestDouble_TriggerInterrupt(interruptTestDouble);
      }
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks);
      AfterNInterrupts(ticks);
      EventQueue_InterruptSafe_Run(EventQueueInterruptSafePlugin_GetInterruptSafeEventQueue());
   }

   void SealedSystemValveCurrentPositionShouldBe(SealedSystemValvePosition_t expected)
   {
      SealedSystemValvePosition_t actual;
      DataModel_Read(dataModel, Erd_SealedSystemValveCurrentPosition, &actual);
      CHECK_EQUAL(expected, actual);
   };

   void GivenResolvedPositionIs(SealedSystemValvePosition_t position)
   {
      SealedSystemValveVotedPosition_t votePosition = {
         .position = position,
         .care = Vote_Care
      };
      DataModel_Write(dataModel, Erd_SealedSystemValvePosition_ResolvedVote, &votePosition);
   }

   void GivenTheApplicationIsInitializedAndSealedSystemIsInStateC()
   {
      GivenTheApplicationIsInitialized();
      After((valveData->refrigerantValvePowerUpTimeInSeconds + 1) * MSEC_PER_SEC);
      SealedSystemValveCurrentPositionShouldBe(SealedSystemValvePosition_Home);

      After((2 * valveData->excitationDelayInMilliseconds) + (valveData->positionBStep * (valveData->delayBetweenStepEventsInMilliseconds + 1)));
      SealedSystemValveCurrentPositionShouldBe(SealedSystemValvePosition_B);

      GivenResolvedPositionIs(SealedSystemValvePosition_C);
      After((2 * valveData->excitationDelayInMilliseconds) + ((valveData->positionCStep - valveData->positionDStep) * (valveData->delayBetweenStepEventsInMilliseconds + 1)));
      SealedSystemValveCurrentPositionShouldBe(SealedSystemValvePosition_C);
   }
};

TEST(FactoryModeIntegration_FourDoor, ShouldSetSealedSystemToHomeBeforeSettingToPositionDWhenFactoryModeEnabled)
{
   GivenTheApplicationIsInitializedAndSealedSystemIsInStateC();
   GivenTheFactoryModeEnableRequestIs(1);
   After((2 * valveData->excitationDelayInMilliseconds) + ((valveData->numberOfStepsToHome) * (valveData->delayBetweenStepEventsInMilliseconds + 1)));
   SealedSystemValveCurrentPositionShouldBe(SealedSystemValvePosition_Home);

   After((2 * valveData->excitationDelayInMilliseconds) + ((valveData->positionDStep) * (valveData->delayBetweenStepEventsInMilliseconds + 1)));
   SealedSystemValveCurrentPositionShouldBe(SealedSystemValvePosition_D);
}
