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

enum
{
   AllOff = 0,
   Position1,
   Position2,
   Position3,
   Position4,

   Pin1 = 0,
   Pin2,
   Pin3,
   Pin4,
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

static const Erd_t pins[] = {
   Pin1,
   Pin2,
   Pin3,
   Pin4
};

static const StepperMotorPinArray_t pinArray = {
   pins,
   NUM_ELEMENTS(pins)
};

static StepperMotorDriverConfiguration_t config = {
   .stepperMotorPositionRequestErd = Erd_DamperMotorPositionRequest,
   .pinArray = &pinArray
};

static bool inputStates[NumberOfPins];
static bool outputStates[NumberOfPins];

TEST_GROUP(StepperMotorDriver)
{
   StepperMotorDriver_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   Event_Synchronous_t stepEvent;
   GpioGroup_TestDouble_t gpioGroupDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      Event_Synchronous_Init(&stepEvent);
      GpioGroup_TestDouble_Init(&gpioGroupDouble, inputStates, outputStates);
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
      for(uint8_t i = 1; i < NumberOfPins; i++)
      {
         PinShouldBe(Pin1 + i, pinState[i]);
      }
   }

   void ThePinsAre(const bool pinState[])
   {
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, Pin1, &pinState[0]);
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, Pin2, &pinState[1]);
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, Pin3, &pinState[2]);
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, Pin4, &pinState[3]);
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

   void WhenStepEventIsRaisedTwice()
   {
      Event_Synchronous_Publish(&stepEvent, NULL);
      Event_Synchronous_Publish(&stepEvent, NULL);
   }

   void RunForNSteps(uint16_t stepRequest)
   {
      for(uint16_t i = 0; i < stepRequest; i++)
      {
         WhenEventIsRaised(&stepEvent);
         WhenEventIsRaised(&stepEvent);
      }
   }

   void ShouldStopOnNextTimeout()
   {
      WhenEventIsRaised(&stepEvent);
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
};

TEST(StepperMotorDriver, ShouldInitializeToAllPinsOff)
{
   Given ThePinsAre(pinState[Position1]);
   And WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);

   When TheModuleIsInitialized();
   ThePinsShouldBe(pinState[AllOff]);
}

TEST(StepperMotorDriver, ShouldGoToPositionOneWhenFirstGettingRequestChange)
{
   Given TheModuleIsInitialized();

   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   ThePinsShouldBe(pinState[Position1]);
}

TEST(StepperMotorDriver, ShouldOnlyGetNewPositionAfterEveryTwoEvents)
{
   Given TheModuleIsInitialized();
   And WhenTheDirectionIsSetTo(TurningDirection_Clockwise);

   And WhenTheRequestedStepsAreSetTo(StepsToOpen);

   WhenEventIsRaised(&stepEvent);
   ThePinsShouldBe(pinState[Position1]);

   WhenEventIsRaised(&stepEvent);
   ThePinsShouldBe(pinState[Position2]);
}

TEST(StepperMotorDriver, ShouldCorrectlyStartOpeningWhenRequestedStepsChange)
{
   Given TheModuleIsInitialized();
   And WhenTheDirectionIsSetTo(TurningDirection_Clockwise);

   WhenTheRequestedStepsAreSetTo(StepsToOpen);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position2]);
}

TEST(StepperMotorDriver, ShouldCorrectlyStartClosingWhenRequestedStepsChange)
{
   Given TheModuleIsInitialized();
   And WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);

   WhenTheRequestedStepsAreSetTo(StepsToClose);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position4]);
}

TEST(StepperMotorDriver, ShouldCorrectlyCycleThroughPositionsInClockwiseDirection)
{
   Given TheModuleIsInitialized();
   And WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   And WhenTheRequestedStepsAreSetTo(StepsToOpen);
   ThePinsShouldBe(pinState[Position1]);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position2]);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position3]);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position4]);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position1]);
}

TEST(StepperMotorDriver, ShouldCorrectlyCycleThroughPositionsInCounterClockwiseDirection)
{
   Given TheModuleIsInitialized();
   And WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   And WhenTheRequestedStepsAreSetTo(StepsToClose);
   ThePinsShouldBe(pinState[Position1]);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position4]);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position3]);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position2]);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position1]);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position4]);
}

TEST(StepperMotorDriver, ShouldSetPinsToAllOffAfterCompletingAnOpenRequest)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);

   After RunForNSteps(StepsToOpen);
   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[AllOff]);
}

TEST(StepperMotorDriver, ShouldSetPinsToAllOffAfterCompletingACloseRequest)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);

   After RunForNSteps(StepsToClose);
   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[AllOff]);
}

TEST(StepperMotorDriver, ShouldStopRunningAfterCompletingARequest)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);

   After RunForNSteps(StepsToClose);
   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[AllOff]);
   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[AllOff]);
}

TEST(StepperMotorDriver, ShouldHandleConsecutiveRequests)
{
   Given TheModuleIsInitialized();

   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   RunForNSteps(StepsToOpen);
   ShouldStopOnNextTimeout();

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);
   RunForNSteps(StepsToClose);
   ShouldStopOnNextTimeout();

   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   RunForNSteps(StepsToOpen);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldNotListenToChangesInStepRequestWhileRunning)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);
   RunForNSteps(StepsToOpen - 10);

   WhenTheRequestedStepsAreSetTo(StepsToClose);
   RunForNSteps(10);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldNotListenToChangesInDirectionRequestWhileRunning)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);

   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position2]);

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenStepEventIsRaisedTwice();
   ThePinsShouldBe(pinState[Position3]);
}
