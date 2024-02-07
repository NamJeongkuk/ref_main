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
      ReferDataModel_TestDouble_Init(&dataModelDouble);
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
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorDriveEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheDamperMotorDriveControlRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorControlRequest, &actual);

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

   void GivenTheIceMakerThermistorAdcCountIs(AdcCounts_t count)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_MoldThermistor_AdcCount, &count);
   }

   void GivenTheIceMakerIsEnabled()
   {
      DataModel_Write(dataModel, Erd_IceMaker0_EnableStatus, set);
      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, set);
   }

   void WhenTheIceMakerThermistorAdcCountIs(AdcCounts_t count)
   {
      GivenTheIceMakerThermistorAdcCountIs(count);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   static TimerTicks_t DamperHomingTime(I_DataModel_t * dataModel)
   {
      return (PersonalityParametricData_Get(dataModel)->freshFoodDamperData->stepsToHome * (PersonalityParametricData_Get(dataModel)->freshFoodDamperData->delayBetweenStepEventsInHundredsOfMicroseconds + 1)) + 1;
   }

   void AfterNInterrupts(int numberOfInterrupts)
   {
      for(int i = 0; i < numberOfInterrupts; i++)
      {
         Interrupt_TestDouble_TriggerInterrupt(interruptTestDouble);
      }
   }

   void WhenTheMotorSwitchIsDebouncedHigh(void)
   {
      DataModel_Write(dataModel, Erd_Gpio_GPIO_IN_02, clear);
   }

   void WhenTheMotorSwitchIsDebouncedLow(void)
   {
      DataModel_Write(dataModel, Erd_Gpio_GPIO_IN_02, set);
   }

   void WhenTheDamperIsRequestToPosition(DamperPosition_t position)
   {
      DamperVotedPosition_t votedPosition = {
         .position = position,
         .care = Vote_Care
      };

      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_FactoryVote, &votedPosition);
   }

   void TheMotorActionResultShouldBe(IceMakerMotorActionResult_t expected)
   {
      IceMakerMotorActionResult_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MotorActionResult, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheTwistTrayStateMachineStateShouldBe(IceMakerStateMachineState_t expected)
   {
      IceMakerStateMachineState_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_StateMachineState, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenTwistTrayMotorIsRequestedToHarvest()
   {
      IceMakerTestRequest_t testRequestValue = IceMakerTestRequest_Harvest;
      DataModel_Write(dataModel, Erd_IceMaker0_TestRequest, &testRequestValue);
   }

   uint16_t DamperStepsRemaining(void)
   {
      StepperPositionRequest_t request;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorPositionRequest, &request);
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

   void WhenTheTwistTrayMotorHomes()
   {
      AfterNInterrupts(twistTrayIceMakerData->harvestData.initialHomingTwistPeriodSecX10 * 100);
      WhenTheMotorSwitchIsDebouncedLow();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      WhenTheMotorSwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      AfterNInterrupts(TwistTrayMotorBrakingDurationInMsec);

      After(TwistTrayMotorControllerPollingTimeInMsec);
      TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homed);
   }

   void WhenTheTwistTrayMotorIsDoneHarvesting()
   {
      AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
      WhenTheMotorSwitchIsDebouncedLow();
      WhenTheMotorSwitchIsDebouncedHigh();
      WhenTheMotorSwitchIsDebouncedLow();
      AfterNInterrupts(CoastingTimeInMs);
      WhenTheMotorSwitchIsDebouncedHigh();
      WhenTheMotorSwitchIsDebouncedLow();

      WhenTheMotorSwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);

      AfterNInterrupts(CoastingTimeInMs);

      After(TwistTrayMotorControllerPollingTimeInMsec);
      TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvested);
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
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TheDamperMotorDriveEnableShouldBe(DISABLED);

   WhenTheTwistTrayMotorHomes();
   TheTwistTrayMotorEnableShouldBe(DISABLED);
   TheDamperMotorDriveEnableShouldBe(DISABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldNotBeEnabledAfterTheDamperIsDoneHomingIfTheThermistorIsInvalid)
{
   GivenTheIceMakerThermistorAdcCountIs(InvalidAdcCount);
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(DISABLED);
   TwistTrayIceMakerThermistorShouldBe(Invalid);
}

TEST(MotorDriverArbitrationIntegration, DamperShouldHaveMotorAfterItHomesAndTheTwistTrayHomesIfItRequestsTheMotorFirst)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMakerIsEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TwistTrayIceMakerThermistorShouldBe(Valid);

   WhenTheTwistTrayMotorHomes();
   TheTwistTrayMotorEnableShouldBe(DISABLED);

   WhenTheDamperIsRequestToPosition(DamperPosition_Open);
   WhenTwistTrayMotorIsRequestedToHarvest();

   TheDamperMotorDriveEnableShouldBe(ENABLED);
   TheDamperMotorDriveControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorEnableShouldBe(DISABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldHaveMotorAfterItHomesAndTheDamperHomesIfItRequestsTheMotorFirst)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMakerIsEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TwistTrayIceMakerThermistorShouldBe(Valid);

   WhenTheTwistTrayMotorHomes();
   TheTwistTrayMotorEnableShouldBe(DISABLED);

   WhenTwistTrayMotorIsRequestedToHarvest();
   WhenTheDamperIsRequestToPosition(DamperPosition_Open);

   TheTwistTrayMotorControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TheDamperMotorDriveEnableShouldBe(DISABLED);
   TheDamperMotorDriveControlRequestShouldBe(ENABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldGiveUpControlAfterAHarvestCompletes)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMakerIsEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   WhenTheTwistTrayMotorHomes();
   WhenTwistTrayMotorIsRequestedToHarvest();
   WhenTheTwistTrayMotorIsDoneHarvesting();

   // This ensures we've exited the Twist Tray Ice Maker Harvest State
   After(twistTrayIceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   TheTwistTrayStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   TheTwistTrayMotorControlRequestShouldBe(DISABLED);
   TheTwistTrayMotorEnableShouldBe(DISABLED);
}
