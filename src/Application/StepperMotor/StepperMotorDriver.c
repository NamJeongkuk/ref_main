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
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"

typedef struct
{
   StepperMotorDriver_t *instance;
} InterruptEventContext_t;

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

static void OnStepChangeRequest(void *context, const void *args)
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

static void MoveToNextStep(StepperMotorDriver_t *instance)
{
   GetNextStep(instance);
   SetPinsToPosition(instance, instance->_private.currentSubStep);
   instance->_private.stepsToRun--;
}

static void MoveToFirstStep(StepperMotorDriver_t *instance)
{
   instance->_private.currentSubStep = 0;
   SetPinsToPosition(instance, instance->_private.currentSubStep);
   instance->_private.stepsToRun--;
}

static void ReadyForNextRequest(const void *data)
{
   const InterruptEventContext_t *context = data;

   DataModel_Subscribe(
      context->instance->_private.dataModel,
      context->instance->_private.config->stepperMotorPositionRequestErd,
      &context->instance->_private.erdChangeSubscription);

   DataModel_Write(
      context->instance->_private.dataModel,
      context->instance->_private.config->motorControlRequestErd,
      clear);

   ClearStepCountRequest(context->instance);
}

static void EndExcitationDelayExpired(StepperMotorDriver_t *instance)
{
   Event_Unsubscribe(
      instance->_private.timeSourceEvent,
      &instance->_private.timeSourceEventSubscription);

   SetPinsToStates(instance, OFF, OFF, OFF, OFF);
   InterruptEventContext_t eventContext = { instance };

   // Enqueue ready request to decouple ERD interactions from interrupt context
   EventQueue_EnqueueWithData(
      instance->_private.eventQueue,
      ReadyForNextRequest,
      &eventContext,
      sizeof(eventContext));
}

static void StartExcitationDelayExpired(StepperMotorDriver_t *instance)
{
   MoveToNextStep(instance);
}

static void StepTimeChange(void *context, const void *args)
{
   StepperMotorDriver_t *instance = context;
   IGNORE(args);

   if(instance->_private.excitationDelayActive)
   {
      instance->_private.excitationDelayStepCount++;

      if(instance->_private.excitationDelayStepCount >= *instance->_private.numberOfEventsForExcitationDelay)
      {
         instance->_private.excitationDelayActive = false;
         instance->_private.excitationDelayStepCount = 0;

         if(instance->_private.stepsToRun > 0)
         {
            StartExcitationDelayExpired(instance);
         }
         else
         {
            EndExcitationDelayExpired(instance);
         }
      }
   }
   else
   {
      if(instance->_private.stepsToRun == 0)
      {
         instance->_private.excitationDelayActive = true;
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
         // Don't take new requests while moving
         DataModel_Unsubscribe(
            instance->_private.dataModel,
            instance->_private.config->stepperMotorPositionRequestErd,
            &instance->_private.erdChangeSubscription);

         instance->_private.stepsToRun = stepperMotorPositionRequest.stepsToMove;
         instance->_private.directionToTurn = stepperMotorPositionRequest.direction;

         if(instance->_private.subStepResetRequested)
         {
            MoveToFirstStep(instance);
         }
         else
         {
            MoveToNextStep(instance);
         }

         instance->_private.subStepResetRequested = stepperMotorPositionRequest.resetSubstep;

         // Setup to introduce start excitation delay
         instance->_private.excitationDelayActive = true;
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
   I_EventQueue_t *eventQueue,
   const uint8_t *numberOfEventsBetweenSteps,
   const uint16_t *numberOfEventsForExcitationDelay)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.gpioGroup = gpioGroup;
   instance->_private.timeSourceEvent = timeSourceEvent;
   instance->_private.currentSubStep = 0;
   instance->_private.stepsToRun = 0;
   instance->_private.countBetweenSteps = 0;
   instance->_private.numberOfEventsBetweenSteps = numberOfEventsBetweenSteps;
   instance->_private.numberOfEventsForExcitationDelay = numberOfEventsForExcitationDelay;
   instance->_private.excitationDelayActive = false;
   instance->_private.excitationDelayStepCount = 0;
   instance->_private.eventQueue = eventQueue;

   SetPinsToStates(instance, OFF, OFF, OFF, OFF);

   EventSubscription_Init(
      &instance->_private.erdChangeSubscription,
      instance,
      OnStepChangeRequest);
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
