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
   NumberOfPins,

   ClockwiseSteps = 600,
   CounterClockwiseSteps = 700
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

static const StepperMotorPins_t damperPins = { 
   .motorDriveA = A, 
   .motorDriveB = B, 
   .motorDriveABar = Abar, 
   .motorDriveBBar = Bbar,
};

static const StepperMotorDriverConfiguration_t config = {
   .stepperMotorPositionRequestErd = Erd_FreshFoodDamperStepperMotorPositionRequest,
   .motorControlRequestErd = Erd_FreshFoodDamperStepperMotorControlRequest,
   .motorEnableErd = Erd_FreshFoodDamperStepperMotorDriveEnable,
   .pins = &damperPins,
   .subStepConfig = &subStepConfig
};

static bool inputStates[NumberOfPins];
static bool outputStates[NumberOfPins];

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
         .withParameter("Steps", positionRequest->stepsToMove);
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

TEST_GROUP(StepperMotorDriver)
{
   StepperMotorDriver_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   Event_Synchronous_t stepEvent;
   GpioGroup_TestDouble_t gpioGroupDouble;
   EventSubscription_t dataModelOnChangeSubscription;
   uint8_t numberOfEventsBetweenSteps;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      Event_Synchronous_Init(&stepEvent);
      GpioGroup_TestDouble_Init(&gpioGroupDouble, inputStates, outputStates);

      EventSubscription_Init(&dataModelOnChangeSubscription, NULL, DataModelChanged);
      Event_Subscribe(dataModelDouble.dataModel->OnDataChange, &dataModelOnChangeSubscription);

      numberOfEventsBetweenSteps = 10;
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
         &numberOfEventsBetweenSteps);
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

   void WhenEventIsRaised(Event_Synchronous_t * event)
   {
      Event_Synchronous_Publish(event, NULL);
   }

   void RunForNSteps(uint16_t stepRequest)
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

   void ShouldStopOnNextTimeout()
   {
      WhenEventIsRaised(&stepEvent);
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
   WhenTheRequestedStepsAreSetTo(ClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(subSteps[0]);

   After RunForNSteps(ClockwiseSteps);
   WhenEventIsRaised(&stepEvent);
   AllPinsShouldBeOff();
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

TEST(StepperMotorDriver, ShouldDriveMotorWhenMotorDriveEnableChangesToTrueAndStepIsGreaterThanZero)
{
   Given TheModuleIsInitialized();
   Given StepperMotorDriveEnableIs(false);
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(ClockwiseSteps);

   When StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(subSteps[0]);
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
   WhenTheRequestedStepsAreSetTo(0);

   When StepperMotorDriveEnableIs(true);
   StepperMotorControlRequestShouldBe(CLEAR);
}

TEST(StepperMotorDriver, ShouldOnlyGetNewPositionWhenStepperMotorDriveEnableIsTrueAndAfterParametricAmountOfSteps)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);

   When StepperMotorDriveEnableIs(true);

   for(uint8_t i = 0; i < numberOfEventsBetweenSteps; i++)
   {
      WhenEventIsRaised(&stepEvent);
   }

   ThePinsShouldBe(subSteps[0]);

   WhenEventIsRaised(&stepEvent);
   ThePinsShouldBe(subSteps[1]);
}

TEST(StepperMotorDriver, ShouldCorrectlyStartClosingWhenStepperMotorDriveEnableIsTrueAndRequestedStepsChange)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);

   When StepperMotorDriveEnableIs(true);
   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[1]);
}

TEST(StepperMotorDriver, ShouldCorrectlyStartOpeningWhenStepperMotorDriveEnableIsTrueAndRequestedStepsChange)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(ClockwiseSteps);

   When StepperMotorDriveEnableIs(true);
   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[3]);
}

TEST(StepperMotorDriver, ShouldCorrectlyCycleThroughPositionsInClockwiseDirectionGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(subSteps[0]);

   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[1]);

   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[2]);

   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[3]);

   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[0]);
}

TEST(StepperMotorDriver, ShouldCorrectlyCycleThroughPositionsInCounterClockwiseDirectionGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(ClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(subSteps[0]);

   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[3]);

   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[2]);

   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[1]);

   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[0]);

   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[3]);
}

TEST(StepperMotorDriver, ShouldSetPinsToAllOffAfterCompletingAnOpenRequestGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);

   ThePinsShouldBe(subSteps[0]);

   After RunForNSteps(CounterClockwiseSteps);
   WhenEventIsRaised(&stepEvent);
   AllPinsShouldBeOff();
}

TEST(StepperMotorDriver, ShouldSetPinsToAllOffAfterCompletingACloseRequestGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(ClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(subSteps[0]);

   After RunForNSteps(ClockwiseSteps);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldStopRunningAfterCompletingARequestGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(subSteps[0]);

   After RunForNSteps(CounterClockwiseSteps);
   ShouldStopOnNextTimeout();
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldHandleConsecutiveRequestsGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(subSteps[0]);

   After RunForNSteps(CounterClockwiseSteps);
   ShouldStopOnNextTimeout();

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(ClockwiseSteps);
   After RunForNSteps(ClockwiseSteps);
   ShouldStopOnNextTimeout();

   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   After RunForNSteps(CounterClockwiseSteps);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldNotListenToChangesInStepRequestWhileRunningGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(ClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(subSteps[0]);

   After RunForNSteps(ClockwiseSteps - 10);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   After RunForNSteps(10);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldNotListenToChangesInDirectionRequestWhileRunningGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   Given StepperMotorDriveEnableIs(true);

   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[1]);

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   After RunForNSteps(1);
   ThePinsShouldBe(subSteps[2]);
}

TEST(StepperMotorDriver, ShouldClearMotorControlRequestThenClearStepRequestWhenCompleted)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(CounterClockwiseSteps);
   Given StepperMotorDriveEnableIs(enabled);

   After RunForNSteps(CounterClockwiseSteps);
   mock().enable();
   mock().strictOrder();

   TheMotorShouldBeRequestedTo(0);
   TheStepRequestShouldBeSetTo(0);
   After RunForNSteps(1);
}

TEST(StepperMotorDriver, ShouldNotReactToRequestsForZeroSteps)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(0);

   StepperMotorStepRequestShouldBeCleared();
}
