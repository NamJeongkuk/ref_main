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

static void SetPinsToStates(StepperMotorDriver_t *instance, bool pin1, bool pin2, bool pin3, bool pin4, bool pin5)
{
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pinArray->pins[0], pin1);
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pinArray->pins[1], pin2);
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pinArray->pins[2], pin3);
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pinArray->pins[3], pin4);
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pinArray->pins[4], pin5);
}

static void SetPinsToPosition(StepperMotorDriver_t *instance, uint8_t position)
{
   switch(position)
   {
      case AllOff:
         SetPinsToStates(instance, OFF, OFF, OFF, OFF, OFF);
         break;

      case Position1:
         SetPinsToStates(instance, ON, ON, OFF, OFF, ON);
         break;

      case Position2:
         SetPinsToStates(instance, OFF, ON, ON, OFF, ON);
         break;

      case Position3:
         SetPinsToStates(instance, OFF, OFF, ON, ON, ON);
         break;

      case Position4:
         SetPinsToStates(instance, ON, OFF, OFF, ON, ON);
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
   const StepperPositionRequest_t *newRequest = args;

   if(newRequest->stepsToMove > 0)
   {
      DataModel_Unsubscribe(
         instance->_private.dataModel,
         instance->_private.config->stepperMotorPositionRequestErd,
         &instance->_private.erdChangeSubscription);

      instance->_private.stepsToRun = newRequest->stepsToMove;
      instance->_private.directionToTurn = newRequest->direction;
      SetPinsToPosition(instance, instance->_private.currentStepPosition);

      Event_Subscribe(
         instance->_private.stepEvent,
         &instance->_private.stepEventSubscription);
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
      &instance->_private.stepEventSubscription,
      instance,
      StepTimeChange);
}
