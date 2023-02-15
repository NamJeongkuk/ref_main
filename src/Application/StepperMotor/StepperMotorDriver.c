/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "StepperMotorDriver.h"
#include "Constants_Binary.h"
#include "PersonalityParametricData.h"
#include "StepperPositionRequest.h"
#include "utils.h"

enum
{
   AllOff,
   Position1,
   Position2,
   Position3,
   Position4
};

static void SetPinsToStates(StepperMotorDriver_t *instance, bool a, bool b, bool aBar, bool bBar)
{
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pins->motorDriveA, a);
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pins->motorDriveB, b);
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pins->motorDriveABar, aBar);
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pins->motorDriveBBar, bBar);
}

static void SetPinsToPosition(StepperMotorDriver_t *instance, uint8_t position)
{
   switch(position)
   {
      case AllOff:
         SetPinsToStates(instance, OFF, OFF, OFF, OFF);
         break;

      case Position1:
         SetPinsToStates(instance, ON, ON, OFF, OFF);
         break;

      case Position2:
         SetPinsToStates(instance, OFF, ON, ON, OFF);
         break;

      case Position3:
         SetPinsToStates(instance, OFF, OFF, ON, ON);
         break;

      case Position4:
         SetPinsToStates(instance, ON, OFF, OFF, ON);
         break;

      default:
         break;
   }
}

static void ClearStepCountRequest(StepperMotorDriver_t *instance)
{
   StepperPositionRequest_t positionRequest;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->stepperMotorPositionRequestErd,
      &positionRequest);

   positionRequest.stepsToMove = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->stepperMotorPositionRequestErd,
      &positionRequest);
}

static void OnDamperPositionRequestChange(void *context, const void *args)
{
   StepperMotorDriver_t *instance = context;
   const StepperPositionRequest_t *positionRequest = args;

   if(positionRequest->stepsToMove > 0)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->motorControlRequestErd,
         set);
   }
}

static void GetNextStep(StepperMotorDriver_t *instance)
{
   if(instance->_private.directionToTurn == TurningDirection_Clockwise)
   {
      instance->_private.currentStepPosition++;
   }
   else
   {
      instance->_private.currentStepPosition--;
   }

   if(instance->_private.currentStepPosition < Position1)
   {
      instance->_private.currentStepPosition = Position4;
   }
   else if(instance->_private.currentStepPosition > Position4)
   {
      instance->_private.currentStepPosition = Position1;
   }
}

static void MoveToNextStep(void *context)
{
   StepperMotorDriver_t *instance = context;

   GetNextStep(instance);
   SetPinsToPosition(instance, instance->_private.currentStepPosition);
   instance->_private.stepsToRun--;
}

static void StepTimeChange(void *context, const void *args)
{
   StepperMotorDriver_t *instance = context;
   IGNORE(args);

   if(instance->_private.stepsToRun == 0)
   {
      Event_Unsubscribe(
         instance->_private.stepEvent,
         &instance->_private.stepEventSubscription);

      DataModel_Subscribe(
         instance->_private.dataModel,
         instance->_private.config->stepperMotorPositionRequestErd,
         &instance->_private.erdChangeSubscription);

      SetPinsToPosition(instance, AllOff);

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->motorControlRequestErd,
         clear);

      ClearStepCountRequest(instance);
   }
   else if(instance->_private.countBetweenSteps < instance->_private.freshFoodDamperParametricData->delayBetweenStepEventsInMs)
   {
      instance->_private.countBetweenSteps++;
   }
   else
   {
      MoveToNextStep(instance);
      instance->_private.countBetweenSteps = 0;
   }
}

static void MotorControlEnableUpdated(void *context, const void *args)
{
   StepperMotorDriver_t *instance = context;
   const bool *motorControlEnabled = args;

   if(*motorControlEnabled)
   {
      StepperPositionRequest_t stepperMotorPositionRequest;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->stepperMotorPositionRequestErd,
         &stepperMotorPositionRequest);

      if(stepperMotorPositionRequest.stepsToMove > 0)
      {
         DataModel_Unsubscribe(
            instance->_private.dataModel,
            instance->_private.config->stepperMotorPositionRequestErd,
            &instance->_private.erdChangeSubscription);

         instance->_private.stepsToRun = stepperMotorPositionRequest.stepsToMove;
         instance->_private.directionToTurn = stepperMotorPositionRequest.direction;
         SetPinsToPosition(instance, instance->_private.currentStepPosition);

         Event_Subscribe(
            instance->_private.stepEvent,
            &instance->_private.stepEventSubscription);
      }
   }
}

void StepperMotorDriver_Init(
   StepperMotorDriver_t *instance,
   I_DataModel_t *dataModel,
   const StepperMotorDriverConfiguration_t *config,
   I_GpioGroup_t *gpioGroup,
   I_Event_t *stepEvent)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.gpioGroup = gpioGroup;
   instance->_private.stepEvent = stepEvent;
   instance->_private.currentStepPosition = Position1;
   instance->_private.stepsToRun = 0;
   instance->_private.countBetweenSteps = 0;
   instance->_private.freshFoodDamperParametricData = PersonalityParametricData_Get(dataModel)->freshFoodDamperData;

   SetPinsToPosition(instance, AllOff);

   EventSubscription_Init(
      &instance->_private.erdChangeSubscription,
      instance,
      OnDamperPositionRequestChange);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->stepperMotorPositionRequestErd,
      &instance->_private.erdChangeSubscription);

   EventSubscription_Init(
      &instance->_private.motorEnableSubscription,
      instance,
      MotorControlEnableUpdated);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->motorEnableErd,
      &instance->_private.motorEnableSubscription);

   EventSubscription_Init(
      &instance->_private.stepEventSubscription,
      instance,
      StepTimeChange);
}
