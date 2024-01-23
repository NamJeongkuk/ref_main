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

static void SetPinsToStates(StepperMotorDriver_t *instance, bool a, bool b, bool aBar, bool bBar)
{
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pins->motorDriveA, a);
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pins->motorDriveB, b);
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pins->motorDriveABar, aBar);
   GpioGroup_Write(instance->_private.gpioGroup, instance->_private.config->pins->motorDriveBBar, bBar);
}

static void SetPinsToPosition(StepperMotorDriver_t *instance, uint8_t subStepIndex)
{
   SetPinsToStates(
      instance,
      instance->_private.config->subStepConfig->subSteps[subStepIndex].a,
      instance->_private.config->subStepConfig->subSteps[subStepIndex].b,
      instance->_private.config->subStepConfig->subSteps[subStepIndex].aBar,
      instance->_private.config->subStepConfig->subSteps[subStepIndex].bBar);
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
      if(instance->_private.currentSubStep < instance->_private.config->subStepConfig->numberOfSubSteps - 1)
      {
         instance->_private.currentSubStep++;
      }
      else
      {
         instance->_private.currentSubStep = 0;
      }
   }
   else
   {
      if(instance->_private.currentSubStep > 0)
      {
         instance->_private.currentSubStep--;
      }
      else
      {
         instance->_private.currentSubStep = instance->_private.config->subStepConfig->numberOfSubSteps - 1;
      }
   }
}

static void MoveToNextStep(void *context)
{
   StepperMotorDriver_t *instance = context;

   GetNextStep(instance);
   SetPinsToPosition(instance, instance->_private.currentSubStep);
   instance->_private.stepsToRun--;
}

static void StepTimeChange(void *context, const void *args)
{
   StepperMotorDriver_t *instance = context;
   IGNORE(args);

   if(instance->_private.stepsToRun == 0)
   {
      Event_Unsubscribe(
         instance->_private.timeSourceEvent,
         &instance->_private.timeSourceEventSubscription);

      DataModel_Subscribe(
         instance->_private.dataModel,
         instance->_private.config->stepperMotorPositionRequestErd,
         &instance->_private.erdChangeSubscription);

      SetPinsToStates(instance, OFF, OFF, OFF, OFF);

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->motorControlRequestErd,
         clear);

      ClearStepCountRequest(instance);
   }
   else if(instance->_private.countBetweenSteps < *instance->_private.numberOfEventsBetweenSteps)
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
         SetPinsToPosition(instance, instance->_private.currentSubStep);

         Event_Subscribe(
            instance->_private.timeSourceEvent,
            &instance->_private.timeSourceEventSubscription);
      }
   }
}

void StepperMotorDriver_Init(
   StepperMotorDriver_t *instance,
   I_DataModel_t *dataModel,
   const StepperMotorDriverConfiguration_t *config,
   I_GpioGroup_t *gpioGroup,
   I_Event_t *timeSourceEvent,
   const uint8_t *numberOfEventsBetweenSteps)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.gpioGroup = gpioGroup;
   instance->_private.timeSourceEvent = timeSourceEvent;
   instance->_private.currentSubStep = 0;
   instance->_private.stepsToRun = 0;
   instance->_private.countBetweenSteps = 0;
   instance->_private.numberOfEventsBetweenSteps = numberOfEventsBetweenSteps;

   SetPinsToStates(instance, OFF, OFF, OFF, OFF);

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
      &instance->_private.timeSourceEventSubscription,
      instance,
      StepTimeChange);
}
