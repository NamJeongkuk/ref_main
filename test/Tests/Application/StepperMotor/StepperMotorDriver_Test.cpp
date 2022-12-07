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
   EnablePin,
   NumberOfPins,

   StepsToOpen = 600,
   StepsToClose = 700
};

static const bool pinState[5][5] = {
   { OFF, OFF, OFF, OFF, OFF },
   { ON, ON, OFF, OFF, ON },
   { OFF, ON, ON, OFF, ON },
   { OFF, OFF, ON, ON, ON },
   { ON, OFF, OFF, ON, ON },
};

static const StepperMotorPins_t damperPins = {
   .motorDriveA = A,
   .motorDriveB = B,
   .motorDriveABar = Abar,
   .motorDriveBBar = Bbar,
   .motorDriveEnable = EnablePin
};

static const StepperMotorDriverConfiguration_t config = {
   .stepperMotorPositionRequestErd = Erd_FreshFoodDamperStepperMotorPositionRequest,
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
      GpioGroup_Write(&gpioGroupDouble.gpioGroup, EnablePin, &pinState[4]);
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

TEST(StepperMotorDriver, ShouldOnlyGetNewPositionAfterParametricAmountOfSteps)
{
   Given TheModuleIsInitialized();
   And WhenTheDirectionIsSetTo(TurningDirection_Clockwise);

   And WhenTheRequestedStepsAreSetTo(StepsToClose);

   for(uint8_t i = 0; i < damperData->delayBetweenStepEventsInMs; i++)
   {
      WhenEventIsRaised(&stepEvent);
   }
   ThePinsShouldBe(pinState[Position1]);

   WhenEventIsRaised(&stepEvent);
   ThePinsShouldBe(pinState[Position2]);
}

TEST(StepperMotorDriver, ShouldCorrectlyStartClosingWhenRequestedStepsChange)
{
   Given TheModuleIsInitialized();
   And WhenTheDirectionIsSetTo(TurningDirection_Clockwise);

   WhenTheRequestedStepsAreSetTo(StepsToClose);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position2]);
}

TEST(StepperMotorDriver, ShouldCorrectlyStartOpeningWhenRequestedStepsChange)
{
   Given TheModuleIsInitialized();
   And WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);

   WhenTheRequestedStepsAreSetTo(StepsToOpen);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position4]);
}

TEST(StepperMotorDriver, ShouldCorrectlyCycleThroughPositionsInClockwiseDirection)
{
   Given TheModuleIsInitialized();
   And WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   And WhenTheRequestedStepsAreSetTo(StepsToClose);
   ThePinsShouldBe(pinState[Position1]);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position2]);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position3]);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position4]);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position1]);
}

TEST(StepperMotorDriver, ShouldCorrectlyCycleThroughPositionsInCounterClockwiseDirection)
{
   Given TheModuleIsInitialized();
   And WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   And WhenTheRequestedStepsAreSetTo(StepsToOpen);
   ThePinsShouldBe(pinState[Position1]);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position4]);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position3]);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position2]);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position1]);

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position4]);
}

TEST(StepperMotorDriver, ShouldSetPinsToAllOffAfterCompletingAnOpenRequest)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);

   After RunForNSteps(StepsToClose);
   WhenEventIsRaised(&stepEvent);
   ThePinsShouldBe(pinState[AllOff]);
}

TEST(StepperMotorDriver, ShouldSetPinsToAllOffAfterCompletingACloseRequest)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToOpen);

   After RunForNSteps(StepsToOpen);
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldStopRunningAfterCompletingARequest)
{
   Given TheModuleIsInitialized();
   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);

   After RunForNSteps(StepsToClose);
   ShouldStopOnNextTimeout();
   ShouldStopOnNextTimeout();
}

TEST(StepperMotorDriver, ShouldHandleConsecutiveRequests)
{
   Given TheModuleIsInitialized();

   WhenTheDirectionIsSetTo(TurningDirection_Clockwise);
   WhenTheRequestedStepsAreSetTo(StepsToClose);
   RunForNSteps(StepsToClose);
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

   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position2]);

   WhenTheDirectionIsSetTo(TurningDirection_CounterClockwise);
   RunForNSteps(1);
   ThePinsShouldBe(pinState[Position3]);
}
