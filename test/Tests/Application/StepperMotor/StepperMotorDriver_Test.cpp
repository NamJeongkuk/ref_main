/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "StepperMotorDriver.h"
#include "Constants_Binary.h"
#include "StepperPositionRequest.h"
#include "utils.h"
#include "EventSubscription.h"
#include "EventQueue_InterruptSafe.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "ReferDataModel_TestDouble.h"
#include "GpioGroup_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And
#define After
#define Then

enum
{
   A = 0,
   B,
   Abar,
   Bbar,

   ClockwiseSteps = 600,
   CounterClockwiseSteps = 701
};

static const StepperMotorSubStep_t subSteps[] = {
   { ON, ON, OFF, OFF },
   { OFF, ON, ON, OFF },
   { OFF, OFF, ON, ON },
   { ON, OFF, OFF, ON }
};

static const StepperMotorSubStepConfiguration_t subStepConfig = {
   .subSteps = subSteps,
   .numberOfSubSteps = NUM_ELEMENTS(subSteps)
};

static const StepperMotorPins_t motorPins = {
   .motorDriveA = A,
   .motorDriveB = B,
   .motorDriveABar = Abar,
   .motorDriveBBar = Bbar,
};

static const StepperMotorDriverConfiguration_t config = {
   .stepperMotorPositionRequestErd = Erd_FreshFoodDamperStepperMotorPositionRequest,
   .motorControlRequestErd = Erd_FreshFoodDamperStepperMotorControlRequest,
   .motorEnableErd = Erd_FreshFoodDamperStepperMotorDriveEnable,
   .pins = &motorPins,
   .subStepConfig = &subStepConfig,
};

static bool inputStates[4];
static bool outputStates[4];

static void DataModelChanged(void *context, const void *_args)
{
   IGNORE(context);
   REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);

   if(args->erd == config.stepperMotorPositionRequestErd)
   {
      REINTERPRET(positionRequest, args->data, const StepperPositionRequest_t *);

      mock()
         .actualCall("Stepper Motor Request")
         .onObject(context)
         .withParameter("Erd", args->erd)
         .withParameter("Direction", positionRequest->direction)
         .withParameter("Steps", positionRequest->stepsToMove)
         .withParameter("ResetSubstep", positionRequest->resetSubstep);
   }
   else if(args->erd == config.motorControlRequestErd)
   {
      mock()
         .actualCall("Motor Control Request")
         .onObject(context)
         .withParameter("Erd", args->erd)
         .withParameter("Data", *(const bool *)args->data);
   }
}

static void FailedToEnqueueEvent()
{
   FAIL("Failed to enqueue event");
}

TEST_GROUP(StepperMotorDriver)
{
   StepperMotorDriver_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   Event_Synchronous_t stepEvent;
   GpioGroup_TestDouble_t gpioGroupDouble;
   EventSubscription_t dataModelOnChangeSubscription;
   EventQueue_InterruptSafe_t eventQueue;
   uint8_t eventBuffer[50];
   uint8_t numberOfEventsBetweenSteps;
   uint16_t numberOfEventsForExcitationDelay;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      Event_Synchronous_Init(&stepEvent);
      GpioGroup_TestDouble_Init(&gpioGroupDouble, inputStates, outputStates);
      EventQueue_InterruptSafe_Init(&eventQueue, &eventBuffer, sizeof(eventBuffer), FailedToEnqueueEvent);

      EventSubscription_Init(&dataModelOnChangeSubscription, NULL, DataModelChanged);
      Event_Subscribe(dataModelDouble.dataModel->OnDataChange, &dataModelOnChangeSubscription);

      numberOfEventsBetweenSteps = 10;
      numberOfEventsForExcitationDelay = 500;
   }

   void TheModuleIsInitialized()
   {
      mock().disable();

      StepperMotorDriver_Init(
         &instance,
         dataModelDouble.dataModel,
         &config,
         &gpioGroupDouble.gpioGroup,
         &stepEvent.interface,
         &eventQueue.interface,
         &numberOfEventsBetweenSteps,
         &numberOfEventsForExcitationDelay);
   }

   void PinShouldBe(GpioChannel_t pinChannel, bool expectedState)
   {
      bool actual = GpioGroup_TestDouble_ReadOutputState(&gpioGroupDouble, pinChannel);
      CHECK_EQUAL_TEXT(expectedState, actual, "Unexpected gpio state");
   }

   void ThePinsShouldBe(const StepperMotorSubStep_t subStep)
   {
      PinShouldBe(A, subStep.a);
      PinShouldBe(B, subStep.b);
      PinShouldBe(Abar, subStep.aBar);
      PinShouldBe(Bbar, subStep.bBar);
   }

   void AllPinsShouldBeOff(void)
   {
      PinShouldBe(A, OFF);
      PinShouldBe(B, OFF);
      PinShouldBe(Abar, OFF);
      PinShouldBe(Bbar, OFF);
   }

   void ThePinsAre(const StepperMotorSubStep_t subStep)
   {
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, A, subStep.a);
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, B, subStep.b);
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, Abar, subStep.aBar);
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, Bbar, subStep.bBar);
   }

   void TheDirectionShouldBe(bool clockwiseDirection)
   {
      StepperPositionRequest_t actual;
      DataModel_Read(dataModelDouble.dataModel, config.stepperMotorPositionRequestErd, &actual);
      CHECK_EQUAL_TEXT(clockwiseDirection, actual.direction, "Unexpected turning direction");
   }

   void WhenTheDirectionIsSetTo(bool clockwiseDirection)
   {
      StepperPositionRequest_t request;
      DataModel_Read(dataModelDouble.dataModel, config.stepperMotorPositionRequestErd, &request);
      request.direction = clockwiseDirection;
      DataModel_Write(dataModelDouble.dataModel, config.stepperMotorPositionRequestErd, &request);
   }

   void WhenTheRequestedStepsAreSetTo(uint16_t stepsRequest)
   {
      StepperPositionRequest_t request;
      DataModel_Read(dataModelDouble.dataModel, config.stepperMotorPositionRequestErd, &request);
      request.stepsToMove = stepsRequest;
      DataModel_Write(dataModelDouble.dataModel, config.stepperMotorPositionRequestErd, &request);
   }

   void WhenTheSubstepResetIsSetTo(bool resetSubstep)
   {
      StepperPositionRequest_t request;
      DataModel_Read(dataModelDouble.dataModel, config.stepperMotorPositionRequestErd, &request);
      request.resetSubstep = resetSubstep;
      DataModel_Write(dataModelDouble.dataModel, config.stepperMotorPositionRequestErd, &request);
   }

   void WhenEventIsRaised(Event_Synchronous_t * event)
   {
      Event_Synchronous_Publish(event, NULL);
   }

   void AfterNSteps(uint16_t stepRequest)
   {
      for(uint16_t i = 0; i < stepRequest; i++)
      {
         for(uint8_t j = 0; j < numberOfEventsBetweenSteps; j++)
         {
            WhenEventIsRaised(&stepEvent);
         }
         WhenEventIsRaised(&stepEvent);
      }
   }

   void AfterNEvents(uint16_t numberOfEvents)
   {
      for(uint16_t i = 0; i < numberOfEvents; i++)
      {
         WhenEventIsRaised(&stepEvent);
      }
   }

   void ShouldStopOnNextTimeout()
   {
      WhenEventIsRaised(&stepEvent);

      AfterNEvents(numberOfEventsForExcitationDelay);
      AllPinsShouldBeOff();

      AfterTheEventQueueIsRun();
   }

   void TheGpioChannelDirectionIs(GpioChannel_t channel, GpioDirection_t direction)
   {
      GpioGroup_TestDouble_SetDirection(&gpioGroupDouble, channel, direction);
   }

   void TheGpioChannelDirectionShouldBe(GpioChannel_t channel, GpioDirection_t expected)
   {
      CHECK_EQUAL_TEXT(expected, GpioGroup_TestDouble_ReadDirection(&gpioGroupDouble, channel), "Unexpected GpioGroup Channel");
   }

   void StepperMotorControlRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorControlRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void StepperMotorStepRequestShouldBeCleared()
   {
      StepperPositionRequest_t actual;
      DataModel_Read(
         dataModelDouble.dataModel,
         config.stepperMotorPositionRequestErd,
         &actual);

      CHECK_EQUAL(0, actual.stepsToMove);
   }

   void StepperMotorDriveEnableIs(bool state)
   {
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorDriveEnable,
         &state);
   }

   void WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(uint16_t stepRequest)
   {
      When StepperMotorDriveEnableIs(CLEAR);
      WhenTheRequestedStepsAreSetTo(stepRequest);
      When StepperMotorDriveEnableIs(SET);
   }

   void TheMotorShouldBeRequestedTo(bool expectedRequest)
   {
      mock()
         .expectOneCall("Motor Control Request")
         .withParameter("Erd", config.motorControlRequestErd)
         .withParameter("Data", expectedRequest);
   }

   void TheStepRequestShouldBeSetTo(uint16_t expectedSteps)
   {
      mock()
         .expectOneCall("Stepper Motor Request")
         .withParameter("Erd", config.stepperMotorPositionRequestErd)
         .withParameter("Steps", expectedSteps)
         .ignoreOtherParameters();
   }

   void AfterTheEventQueueIsRun()
   {
      EventQueue_InterruptSafe_Run(&eventQueue);
   }
};

TEST(StepperMotorDriver, ShouldInitializeToAllPinsOff)
{
   Given ThePinsAre(subSteps[0]);
   And WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);

   When TheModuleIsInitialized();
   AllPinsShouldBeOff();
}

TEST(StepperMotorDriver, ShouldSetMotorControlRequestWhenStepperMotorDirectionRequestChanges)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);

   WhenTheRequestedStepsAreSetTo(1);
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   StepperMotorControlRequestShouldBe(SET);
}

TEST(StepperMotorDriver, ShouldClearMotorControlRequestAfterMotorControlHasCompletedAllSteps)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(CounterClockwiseSteps);

   ThePinsShouldBe(subSteps[3]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[2]);

   AfterNSteps(CounterClockwiseSteps - 2);
   WhenEventIsRaised(&stepEvent);
   AfterNEvents(numberOfEventsForExcitationDelay);

   AfterTheEventQueueIsRun();
   StepperMotorControlRequestShouldBe(CLEAR);
}

TEST(StepperMotorDriver, ShouldNotDriveMotorWhenStepperMotorPositionRequestChangesButStepperMotorDriveEnableIsFalse)
{
   Given TheModuleIsInitialized();
   Given StepperMotorDriveEnableIs(false);
   AllPinsShouldBeOff();

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(ClockwiseSteps);
   AllPinsShouldBeOff();
}

TEST(StepperMotorDriver, ShouldNotDriveMotorWhenMotorDriveEnableChangesToFalseAndStepIsGreaterThanZero)
{
   Given StepperMotorDriveEnableIs(true);
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(ClockwiseSteps);
   AllPinsShouldBeOff();

   When StepperMotorDriveEnableIs(false);
   AllPinsShouldBeOff();
}

TEST(StepperMotorDriver, ShouldClearMotorRequestWhenMotorDriveEnableChangesToTrueAndStepIsZero)
{
   Given TheModuleIsInitialized();
   Given StepperMotorDriveEnableIs(false);
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(0);

   StepperMotorControlRequestShouldBe(CLEAR);
}

TEST(StepperMotorDriver, ShouldOnlyGetNewPositionWhenStepperMotorDriveEnableIsTrueAndAfterParametricAmountOfSteps)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(ClockwiseSteps);

   ThePinsShouldBe(subSteps[1]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[2]);

   for(uint8_t i = 0; i < numberOfEventsBetweenSteps; i++)
   {
      WhenEventIsRaised(&stepEvent);
   }
   ThePinsShouldBe(subSteps[2]);

   WhenEventIsRaised(&stepEvent);
   ThePinsShouldBe(subSteps[3]);
}

TEST(StepperMotorDriver, ShouldHoldPinSignalForExcitationPeriodWhenStartingMovement)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(ClockwiseSteps);

   ThePinsShouldBe(subSteps[1]);
   AfterNEvents(numberOfEventsForExcitationDelay - 1);
   ThePinsShouldBe(subSteps[1]);

   AfterNEvents(1);
   ThePinsShouldBe(subSteps[2]);
}

TEST(StepperMotorDriver, ShouldHoldPinSignalForExcitationPeriodWhenEndingMovement)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(ClockwiseSteps);

   ThePinsShouldBe(subSteps[1]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[2]);

   AfterNSteps(ClockwiseSteps - 2);
   WhenEventIsRaised(&stepEvent);

   AfterNEvents(numberOfEventsForExcitationDelay - 1);
   ThePinsShouldBe(subSteps[0]);

   AfterNEvents(1);
   AllPinsShouldBeOff();
}

TEST(StepperMotorDriver, ShouldCorrectlyCycleThroughPositionsInClockwiseDirectionGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(ClockwiseSteps);

   ThePinsShouldBe(subSteps[1]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[2]);

   AfterNSteps(1);
   ThePinsShouldBe(subSteps[3]);

   AfterNSteps(1);
   ThePinsShouldBe(subSteps[0]);

   AfterNSteps(1);
   ThePinsShouldBe(subSteps[1]);

   AfterNSteps(1);
   ThePinsShouldBe(subSteps[2]);
}

TEST(StepperMotorDriver, ShouldCorrectlyCycleThroughPositionsInCounterClockwiseDirectionGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(CounterClockwiseSteps);

   ThePinsShouldBe(subSteps[3]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[2]);

   AfterNSteps(1);
   ThePinsShouldBe(subSteps[1]);

   AfterNSteps(1);
   ThePinsShouldBe(subSteps[0]);

   AfterNSteps(1);
   ThePinsShouldBe(subSteps[3]);

   AfterNSteps(1);
   ThePinsShouldBe(subSteps[2]);

   AfterNSteps(1);
   ThePinsShouldBe(subSteps[1]);
}

TEST(StepperMotorDriver, ShouldStopRunningAfterCompletingARequestGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(ClockwiseSteps);

   ThePinsShouldBe(subSteps[1]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[2]);

   AfterNSteps(ClockwiseSteps - 2);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldHandleConsecutiveRequestsGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(ClockwiseSteps);

   ThePinsShouldBe(subSteps[1]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[2]);

   AfterNSteps(ClockwiseSteps - 2);
   ShouldStopOnNextTimeout();

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(CounterClockwiseSteps);

   ThePinsShouldBe(subSteps[3]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[2]);

   AfterNSteps(CounterClockwiseSteps - 2);
   ShouldStopOnNextTimeout();

   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(ClockwiseSteps);

   ThePinsShouldBe(subSteps[0]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[1]);

   AfterNSteps(ClockwiseSteps - 2);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldNotListenToChangesInStepRequestWhileRunningGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(ClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);

   ThePinsShouldBe(subSteps[3]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[2]);

   AfterNSteps(ClockwiseSteps - 10);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   AfterNSteps(10);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldNotListenToChangesInDirectionRequestWhileRunningGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);

   ThePinsShouldBe(subSteps[1]);
   AfterNEvents(numberOfEventsForExcitationDelay);
   ThePinsShouldBe(subSteps[2]);

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   AfterNSteps(1);
   ThePinsShouldBe(subSteps[3]);
}

TEST(StepperMotorDriver, ShouldClearMotorControlRequestThenClearStepRequestWhenCompleted)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   Given StepperMotorDriveEnableIs(enabled);

   AfterNEvents(numberOfEventsForExcitationDelay);
   AfterNSteps(CounterClockwiseSteps);
   mock().enable();
   mock().strictOrder();

   AfterNEvents(numberOfEventsForExcitationDelay);
   TheMotorShouldBeRequestedTo(0);
   TheStepRequestShouldBeSetTo(0);
   AfterTheEventQueueIsRun();
}

TEST(StepperMotorDriver, ShouldNotReactToRequestsForZeroSteps)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(0);

   StepperMotorStepRequestShouldBeCleared();
}

TEST(StepperMotorDriver, ShouldStartNextMovementFromFirstSubStepIfSubStepResetRequestIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheSubstepResetIsSetTo(true);
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(CounterClockwiseSteps);
   AfterNEvents(numberOfEventsForExcitationDelay);
   AfterNSteps(CounterClockwiseSteps);
   AfterNEvents(numberOfEventsForExcitationDelay);
   AfterTheEventQueueIsRun();

   WhenTheSubstepResetIsSetTo(false);
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetToAndMotorDriveEnabled(ClockwiseSteps);
   ThePinsShouldBe(subSteps[0]);
}
