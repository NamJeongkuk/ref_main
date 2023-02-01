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
#include "PersonalityParametricData.h"
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
   AllOff = 0,
   Position1,
   Position2,
   Position3,
   Position4,

   A = 0,
   B,
   Abar,
   Bbar,
   NumberOfPins,

   StepsToOpen = 600,
   StepsToClose = 700
};

static const bool pinState[5][4] = {
   { OFF, OFF, OFF, OFF },
   { ON, ON, OFF, OFF },
   { OFF, ON, ON, OFF },
   { OFF, OFF, ON, ON },
   { ON, OFF, OFF, ON },
};

static const StepperMotorPins_t damperPins = {
   .motorDriveA = A,
   .motorDriveB = B,
   .motorDriveABar = Abar,
   .motorDriveBBar = Bbar
};

static const StepperMotorDriverConfiguration_t config = {
   .stepperMotorPositionRequestErd = Erd_FreshFoodDamperStepperMotorPositionRequest,
   .motorControlRequestErd = Erd_FreshFoodDamperStepperMotorControlRequest,
   .motorEnableErd = Erd_FreshFoodDamperStepperMotorDriveEnable,
   .pins = &damperPins
};

static bool inputStates[NumberOfPins];
static bool outputStates[NumberOfPins];

TEST_GROUP(StepperMotorDriver)
{
   StepperMotorDriver_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   Event_Synchronous_t stepEvent;
   GpioGroup_TestDouble_t gpioGroupDouble;
   const SingleDamperData_t *damperData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      Event_Synchronous_Init(&stepEvent);
      GpioGroup_TestDouble_Init(&gpioGroupDouble, inputStates, outputStates);

      damperData = PersonalityParametricData_Get(dataModelDouble.dataModel)->freshFoodDamperData;
   }

   void TheModuleIsInitialized()
   {
      StepperMotorDriver_Init(
         &instance,
         dataModelDouble.dataModel,
         &config,
         &gpioGroupDouble.gpioGroup,
         &stepEvent.interface);
   }

   void PinShouldBe(GpioChannel_t pinChannel, bool expectedState)
   {
      bool actual = GpioGroup_TestDouble_ReadOutputState(&gpioGroupDouble, pinChannel);
      CHECK_EQUAL_TEXT(expectedState, actual, "Unexpected gpio state");
   }

   void ThePinsShouldBe(const bool pinState[])
   {
      for(uint8_t i = 0; i < NumberOfPins; i++)
      {
         PinShouldBe(A + i, pinState[i]);
      }
   }

   void ThePinsAre(const bool pinState[])
   {
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, A, &pinState[0]);
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, B, &pinState[1]);
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, Abar, &pinState[2]);
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, Bbar, &pinState[3]);
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
         for(uint8_t j = 0; j < damperData->delayBetweenStepEventsInMs; j++)
         {
            WhenEventIsRaised(&stepEvent);
         }
         WhenEventIsRaised(&stepEvent);
      }
   }

   void ShouldStopOnNextTimeout()
   {
      WhenEventIsRaised(&stepEvent);
      ThePinsShouldBe(pinState[AllOff]);
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

   void StepperMotorDriveEnableIs(bool state)
   {
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorDriveEnable,
         &state);
   }
};

TEST(StepperMotorDriver, ShouldInitializeToAllPinsOff)
{
   Given ThePinsAre(pinState[Position1]);
   And WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);

   When TheModuleIsInitialized();
   ThePinsShouldBe(pinState[AllOff]);
}

TEST(StepperMotorDriver, ShouldSetMotorControlRequestWhenStepperMotorDirectionRequestChanges)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   StepperMotorControlRequestShouldBe(SET);
}

TEST(StepperMotorDriver, ShouldClearMotorControlRequestAfterMotorControlHasCompletedAllSteps)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(pinState[Position1]);

   After RunForNSteps(StepsToOpen);
   WhenEventIsRaised(&stepEvent);
   ThePinsShouldBe(pinState[AllOff]);
   StepperMotorControlRequestShouldBe(CLEAR);
}

TEST(StepperMotorDriver, ShouldNotDriveMotorWhenStepperMotorPositionRequestChangesButStepperMotorDriveEnableIsFalse)
{
   Given TheModuleIsInitialized();
   Given StepperMotorDriveEnableIs(false);
   ThePinsShouldBe(pinState[AllOff]);

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   ThePinsShouldBe(pinState[AllOff]);
}

TEST(StepperMotorDriver, ShouldDriveMotorWhenMotorDriveEnableChangesToTrueAndStepIsGreaterThanZero)
{
   Given TheModuleIsInitialized();
   Given StepperMotorDriveEnableIs(false);
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);

   When StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(pinState[Position1]);
}

TEST(StepperMotorDriver, ShouldNotDriveMotorWhenMotorDriveEnableChangesToFalseAndStepIsGreaterThanZero)
{
   Given StepperMotorDriveEnableIs(true);
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   ThePinsShouldBe(pinState[AllOff]);

   When StepperMotorDriveEnableIs(false);
   ThePinsShouldBe(pinState[AllOff]);
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
   WhenTheRequestedStepsAreSetTo(StepsToClose);

   When StepperMotorDriveEnableIs(true);

   for(uint8_t i = 0; i < damperData->delayBetweenStepEventsInMs; i++)
   {
      WhenEventIsRaised(&stepEvent);
   }

   ThePinsShouldBe(pinState[Position1]);

   WhenEventIsRaised(&stepEvent);
   ThePinsShouldBe(pinState[Position2]);
}

TEST(StepperMotorDriver, ShouldCorrectlyStartClosingWhenStepperMotorDriveEnableIsTrueAndRequestedStepsChange)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);

   When StepperMotorDriveEnableIs(true);
   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position2]);
}

TEST(StepperMotorDriver, ShouldCorrectlyStartOpeningWhenStepperMotorDriveEnableIsTrueAndRequestedStepsChange)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);

   When StepperMotorDriveEnableIs(true);
   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position4]);
}

TEST(StepperMotorDriver, ShouldCorrectlyCycleThroughPositionsInClockwiseDirectionGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(pinState[Position1]);

   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position2]);

   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position3]);

   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position4]);

   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position1]);
}

TEST(StepperMotorDriver, ShouldCorrectlyCycleThroughPositionsInCounterClockwiseDirectionGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(pinState[Position1]);

   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position4]);

   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position3]);

   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position2]);

   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position1]);

   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position4]);
}

TEST(StepperMotorDriver, ShouldSetPinsToAllOffAfterCompletingAnOpenRequestGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);
   Given StepperMotorDriveEnableIs(true);

   ThePinsShouldBe(pinState[Position1]);

   After RunForNSteps(StepsToClose);
   WhenEventIsRaised(&stepEvent);
   ThePinsShouldBe(pinState[AllOff]);
}

TEST(StepperMotorDriver, ShouldSetPinsToAllOffAfterCompletingACloseRequestGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(pinState[Position1]);

   After RunForNSteps(StepsToOpen);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldStopRunningAfterCompletingARequestGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(pinState[Position1]);

   After RunForNSteps(StepsToClose);
   ShouldStopOnNextTimeout();
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldHandleConsecutiveRequestsGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(pinState[Position1]);

   After RunForNSteps(StepsToClose);
   ShouldStopOnNextTimeout();

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   After RunForNSteps(StepsToOpen);
   ShouldStopOnNextTimeout();

   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);
   After RunForNSteps(StepsToClose);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldNotListenToChangesInStepRequestWhileRunningGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   Given StepperMotorDriveEnableIs(true);
   ThePinsShouldBe(pinState[Position1]);

   After RunForNSteps(StepsToOpen - 10);
   WhenTheRequestedStepsAreSetTo(StepsToClose);
   After RunForNSteps(10);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldNotListenToChangesInDirectionRequestWhileRunningGivenStepperMotorDriveEnableIsTrue)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);
   Given StepperMotorDriveEnableIs(true);

   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position2]);

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   After RunForNSteps(1);
   ThePinsShouldBe(pinState[Position3]);
}
