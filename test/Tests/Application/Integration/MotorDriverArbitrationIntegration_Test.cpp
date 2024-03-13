/*!
 * @file
 * @brief Integration test for the motor driver on the application level
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Rx2xxResetSource.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "EventQueueInterruptSafePlugin.h"
#include "TddPersonality.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "Interrupt_TestDouble.h"

enum
{
   ValidAdcCount = 30336,
   InvalidAdcCount = 0,
   Valid = true,
   Invalid = false,
   TwistTrayMotorControllerPollingTimeInMsec = 150,
   TwistTrayMotorBrakingDurationInMsec = 1 * MSEC_PER_SEC,
   CoastingTimeInMs = 1000
};

TEST_GROUP(MotorDriverArbitrationIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Interrupt_TestDouble_t *interruptTestDouble;
   Interrupt_TestDouble_t *fastInterruptTestDouble;
   const TwistTrayIceMakerData_t *twistTrayIceMakerData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualTwistTrayIceMaker);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      interruptTestDouble = (Interrupt_TestDouble_t *)DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_SystemTickInterrupt);
      fastInterruptTestDouble = (Interrupt_TestDouble_t *)DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_FastTickInterrupt);

      twistTrayIceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot0Data->twistTrayData;
   }

   void GivenTheApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void TheDamperMotorDriveEnableShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DamperStepperMotorDriveEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheDamperMotorDriveControlRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DamperStepperMotorControlRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheTwistTrayMotorControlRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistIceMakerMotorControlRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TwistTrayIceMakerThermistorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MoldThermistorIsValid, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheTwistTrayMotorEnableShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistIceMakerMotorDriveEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheIceMaker0ThermistorAdcCountIs(AdcCounts_t count)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_MoldThermistor_AdcCount, &count);
   }

   void GivenTheIceMaker1ThermistorAdcCountIs(AdcCounts_t count)
   {
      DataModel_Write(dataModel, Erd_IceMaker1_MoldThermistor_AdcCount, &count);
   }

   void GivenTheIceMakerIsEnabled()
   {
      DataModel_Write(dataModel, Erd_IceMaker0_EnableStatus, set);
      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, set);
   }

   void GivenTwoTwistTrayIceMakersAreEnabled()
   {
      DataModel_Write(dataModel, Erd_IceMaker0_EnableStatus, set);
      DataModel_Write(dataModel, Erd_IceMaker1_EnableStatus, set);
      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, set);
   }

   void WhenTheIceMaker0ThermistorAdcCountIs(AdcCounts_t count)
   {
      GivenTheIceMaker0ThermistorAdcCountIs(count);
   }

   void WhenTheIceMaker1ThermistorAdcCountIs(AdcCounts_t count)
   {
      GivenTheIceMaker1ThermistorAdcCountIs(count);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   static TimerTicks_t DamperHomingTime(I_DataModel_t * dataModel)
   {
      return (PersonalityParametricData_Get(dataModel)->damperData->stepsToHome * (PersonalityParametricData_Get(dataModel)->damperData->delayBetweenStepEventsInHundredsOfMicroseconds + 1)) + 1;
   }

   void AfterNInterrupts(int numberOfInterrupts)
   {
      for(int i = 0; i < numberOfInterrupts; i++)
      {
         Interrupt_TestDouble_TriggerInterrupt(interruptTestDouble);
      }
   }

   void WhenTheMotor0SwitchIsDebouncedHigh(void)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_TwistMotorSwitchState, clear);
   }

   void WhenTheMotor0SwitchIsDebouncedLow(void)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_TwistMotorSwitchState, set);
   }

   void WhenTheMotor1SwitchIsDebouncedHigh(void)
   {
      DataModel_Write(dataModel, Erd_IceMaker1_TwistMotorSwitchState, clear);
   }

   void WhenTheMotor1SwitchIsDebouncedLow(void)
   {
      DataModel_Write(dataModel, Erd_IceMaker1_TwistMotorSwitchState, set);
   }

   void WhenTheDamperIsRequestToPosition(DamperPosition_t position)
   {
      DamperVotedPosition_t votedPosition = {
         .position = position,
         .care = Vote_Care
      };

      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_FactoryVote, &votedPosition);
   }

   void TheMotorAction0ResultShouldBe(IceMakerMotorActionResult_t expected)
   {
      IceMakerMotorActionResult_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MotorActionResult, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheMotorAction1ResultShouldBe(IceMakerMotorActionResult_t expected)
   {
      IceMakerMotorActionResult_t actual;
      DataModel_Read(dataModel, Erd_IceMaker1_MotorActionResult, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheTwistTrayStateMachineStateShouldBe(IceMakerStateMachineState_t expected)
   {
      IceMakerStateMachineState_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_StateMachineState, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenTwistTrayMotor0IsRequestedToHarvest()
   {
      IceMakerTestRequest_t testRequestValue = IceMakerTestRequest_Harvest;
      DataModel_Write(dataModel, Erd_IceMaker0_TestRequest, &testRequestValue);
   }

   void WhenTwistTrayMotor1IsRequestedToHarvest()
   {
      IceMakerTestRequest_t testRequestValue = IceMakerTestRequest_Harvest;
      DataModel_Write(dataModel, Erd_IceMaker1_TestRequest, &testRequestValue);
   }

   uint16_t DamperStepsRemaining(void)
   {
      StepperPositionRequest_t request;
      DataModel_Read(dataModel, Erd_DamperStepperMotorPositionRequest, &request);
      return request.stepsToMove;
   }

   void AfterTheEventQueueIsRun()
   {
      EventQueue_InterruptSafe_Run(EventQueueInterruptSafePlugin_GetInterruptSafeEventQueue());
   }

   void WhenTheDamperHasStoppedMoving(void)
   {
      while(DamperStepsRemaining() > 0)
      {
         Interrupt_TestDouble_TriggerInterrupt(fastInterruptTestDouble);
         AfterTheEventQueueIsRun();
      }
   }

   void GivenTheDamperHasHomed()
   {
      TheDamperMotorDriveEnableShouldBe(ENABLED);
      TheDamperMotorDriveControlRequestShouldBe(ENABLED);
      TheTwistTrayMotorEnableShouldBe(DISABLED);

      WhenTheDamperHasStoppedMoving();

      TheDamperMotorDriveEnableShouldBe(DISABLED);
      TheDamperMotorDriveControlRequestShouldBe(DISABLED);
   }

   void WhenTheTwistTrayMotor0Homes()
   {
      AfterNInterrupts(twistTrayIceMakerData->harvestData.initialHomingTwistPeriodSecX10 * 100);
      WhenTheMotor0SwitchIsDebouncedLow();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      WhenTheMotor0SwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      AfterNInterrupts(TwistTrayMotorBrakingDurationInMsec);

      After(TwistTrayMotorControllerPollingTimeInMsec);
      TheMotorAction0ResultShouldBe(IceMakerMotorActionResult_Homed);
   }

   void WhenTheTwistTrayMotor1Homes()
   {
      AfterNInterrupts(twistTrayIceMakerData->harvestData.initialHomingTwistPeriodSecX10 * 100);
      WhenTheMotor1SwitchIsDebouncedLow();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      WhenTheMotor1SwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      AfterNInterrupts(TwistTrayMotorBrakingDurationInMsec);

      After(TwistTrayMotorControllerPollingTimeInMsec);
      TheMotorAction1ResultShouldBe(IceMakerMotorActionResult_Homed);
   }

   void WhenTheDualTwistTrayMotorsHome()
   {
      WhenTheTwistTrayMotor0Homes();
      WhenTheTwistTrayMotor1Homes();
   }

   void WhenTheTwistTrayMotor0IsDoneHarvesting()
   {
      AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
      WhenTheMotor0SwitchIsDebouncedLow();
      WhenTheMotor0SwitchIsDebouncedHigh();
      WhenTheMotor0SwitchIsDebouncedLow();
      AfterNInterrupts(CoastingTimeInMs);
      WhenTheMotor0SwitchIsDebouncedHigh();
      WhenTheMotor0SwitchIsDebouncedLow();

      WhenTheMotor0SwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);

      AfterNInterrupts(CoastingTimeInMs);

      After(TwistTrayMotorControllerPollingTimeInMsec);
      TheMotorAction0ResultShouldBe(IceMakerMotorActionResult_Harvested);
   }

   void WhenTheTwistTrayMotor1IsDoneHarvesting()
   {
      AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
      WhenTheMotor1SwitchIsDebouncedLow();
      WhenTheMotor1SwitchIsDebouncedHigh();
      WhenTheMotor1SwitchIsDebouncedLow();
      AfterNInterrupts(CoastingTimeInMs);
      WhenTheMotor1SwitchIsDebouncedHigh();
      WhenTheMotor1SwitchIsDebouncedLow();

      WhenTheMotor1SwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);

      AfterNInterrupts(CoastingTimeInMs);

      After(TwistTrayMotorControllerPollingTimeInMsec);
      TheMotorAction1ResultShouldBe(IceMakerMotorActionResult_Harvested);
   }
};

TEST(MotorDriverArbitrationIntegration, DamperShouldHaveTheMotorAndBeHomingOnInitialization)
{
   GivenTheApplicationHasBeenInitialized();

   TheDamperMotorDriveEnableShouldBe(ENABLED);
   TheDamperMotorDriveControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorEnableShouldBe(DISABLED);

   WhenTheDamperHasStoppedMoving();

   TheDamperMotorDriveEnableShouldBe(DISABLED);
   TheDamperMotorDriveControlRequestShouldBe(DISABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldBeEnabledAndHomeAfterTheDamperIsDoneHoming)
{
   GivenTheIceMaker0ThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TheDamperMotorDriveEnableShouldBe(DISABLED);

   WhenTheTwistTrayMotor0Homes();
   TheTwistTrayMotorEnableShouldBe(DISABLED);
   TheDamperMotorDriveEnableShouldBe(DISABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldNotBeEnabledAfterTheDamperIsDoneHomingIfTheThermistorIsInvalid)
{
   GivenTheIceMaker0ThermistorAdcCountIs(InvalidAdcCount);
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(DISABLED);
   TwistTrayIceMakerThermistorShouldBe(Invalid);
}

TEST(MotorDriverArbitrationIntegration, DamperShouldHaveMotorAfterItHomesAndTheTwistTrayHomesIfItRequestsTheMotorFirst)
{
   GivenTheIceMaker0ThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMakerIsEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TwistTrayIceMakerThermistorShouldBe(Valid);

   WhenTheTwistTrayMotor0Homes();
   TheTwistTrayMotorEnableShouldBe(DISABLED);

   WhenTheDamperIsRequestToPosition(DamperPosition_Open);
   WhenTwistTrayMotor0IsRequestedToHarvest();

   TheDamperMotorDriveEnableShouldBe(ENABLED);
   TheDamperMotorDriveControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorEnableShouldBe(DISABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldHaveMotorAfterItHomesAndTheDamperHomesIfItRequestsTheMotorFirst)
{
   GivenTheIceMaker0ThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMakerIsEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TwistTrayIceMakerThermistorShouldBe(Valid);

   WhenTheTwistTrayMotor0Homes();
   TheTwistTrayMotorEnableShouldBe(DISABLED);

   WhenTwistTrayMotor0IsRequestedToHarvest();
   WhenTheDamperIsRequestToPosition(DamperPosition_Open);

   TheTwistTrayMotorControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TheDamperMotorDriveEnableShouldBe(DISABLED);
   TheDamperMotorDriveControlRequestShouldBe(ENABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldGiveUpControlAfterAHarvestCompletes)
{
   GivenTheIceMaker0ThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMakerIsEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   WhenTheTwistTrayMotor0Homes();
   WhenTwistTrayMotor0IsRequestedToHarvest();
   WhenTheTwistTrayMotor0IsDoneHarvesting();

   // This ensures we've exited the Twist Tray Ice Maker Harvest State
   After(twistTrayIceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   TheTwistTrayStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   TheTwistTrayMotorControlRequestShouldBe(DISABLED);
   TheTwistTrayMotorEnableShouldBe(DISABLED);
}

TEST(MotorDriverArbitrationIntegration, ShouldHomeDualTwistTrayMotorsAtTheSameTime)
{
   GivenTheIceMaker0ThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMaker1ThermistorAdcCountIs(ValidAdcCount);
   GivenTwoTwistTrayIceMakersAreEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   WhenTheTwistTrayMotor0Homes();
   TheMotorAction0ResultShouldBe(IceMakerMotorActionResult_Homed);

   WhenTheTwistTrayMotor1Homes();
   TheMotorAction1ResultShouldBe(IceMakerMotorActionResult_Homed);
}

TEST(MotorDriverArbitrationIntegration, ShouldMaintainRequestSignalIfOneMotorStopsMovingButNotTheOther)
{
   GivenTheIceMaker0ThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMaker1ThermistorAdcCountIs(ValidAdcCount);
   GivenTwoTwistTrayIceMakersAreEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   WhenTheDualTwistTrayMotorsHome();
   TheTwistTrayMotorControlRequestShouldBe(DISABLED);

   WhenTwistTrayMotor0IsRequestedToHarvest();
   TheTwistTrayMotorControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorEnableShouldBe(ENABLED);

   WhenTwistTrayMotor1IsRequestedToHarvest();
   WhenTheTwistTrayMotor0IsDoneHarvesting();
   TheTwistTrayMotorControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorEnableShouldBe(ENABLED);

   WhenTheTwistTrayMotor1IsDoneHarvesting();
   TheTwistTrayMotorControlRequestShouldBe(DISABLED);
   TheTwistTrayMotorEnableShouldBe(DISABLED);
}
