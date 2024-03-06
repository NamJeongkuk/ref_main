/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SealedSystemValveRequestManager.h"
#include "utils.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

#define InstanceFromFsm(fsm) CONTAINER_OF(SealedSystemValveRequestManager_t, _private.fsm, fsm)

enum
{
   Signal_PositionRequested = Fsm_UserSignalStart,
   Signal_StepRequestCompleted,
   Signal_HomeRequested
};

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Homing(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Moving(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static SealedSystemValvePosition_t RequestedPosition(SealedSystemValveRequestManager_t *instance)
{
   SealedSystemValveVotedPosition_t votedPosition;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->positionRequestResolvedVoteErd,
      &votedPosition);

   return votedPosition.position;
}

static SealedSystemValvePosition_t CurrentPosition(SealedSystemValveRequestManager_t *instance)
{
   SealedSystemValvePosition_t currentPosition;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->currentPositionErd,
      &currentPosition);

   return currentPosition;
}

static void SetDamperStepperMotorPositionRequest(SealedSystemValveRequestManager_t *instance, StepperPositionRequest_t stepsToMove)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->stepperMotorPositionRequestErd,
      &stepsToMove);
}

static void SetStepperPositionRequestForHoming(SealedSystemValveRequestManager_t *instance)
{
   StepperPositionRequest_t stepRequest;
   stepRequest.direction = TurningDirection_CounterClockwise;
   stepRequest.stepsToMove = instance->_private.data->numberOfStepsToHome;
   stepRequest.resetSubstep = true;

   SetDamperStepperMotorPositionRequest(instance, stepRequest);
}

static void SetCurrentPositionTo(SealedSystemValveRequestManager_t *instance, DamperPosition_t currentPosition)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->currentPositionErd,
      &currentPosition);
}

static void SetPreviousPositionTo(SealedSystemValveRequestManager_t *instance, DamperPosition_t previousPosition)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->previousPositionErd,
      &previousPosition);
}

static void SetPreviousPositionToCurrentPosition(SealedSystemValveRequestManager_t *instance)
{
   SealedSystemValvePosition_t currentPosition;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->currentPositionErd,
      &currentPosition);

   SetPreviousPositionTo(instance, currentPosition);
}

static uint8_t StepLocationForPosition(SealedSystemValveRequestManager_t *instance, SealedSystemValvePosition_t position)
{
   switch(position)
   {
      case SealedSystemValvePosition_A:
         return instance->_private.data->positionAStep;
      case SealedSystemValvePosition_B:
         return instance->_private.data->positionBStep;
      case SealedSystemValvePosition_C:
         return instance->_private.data->positionCStep;
      case SealedSystemValvePosition_D:
         return instance->_private.data->positionDStep;
      default:
         return 0;
   }
}

static TurningDirection_t SteppingDirectionToNewPosition(SealedSystemValveRequestManager_t *instance, SealedSystemValvePosition_t newPosition)
{
   uint8_t currentStep = StepLocationForPosition(instance, CurrentPosition(instance));
   uint8_t destinationStep = StepLocationForPosition(instance, newPosition);
   return ((currentStep < destinationStep) ? TurningDirection_Clockwise : TurningDirection_CounterClockwise);
}

static uint16_t StepsCountToNewPosition(SealedSystemValveRequestManager_t *instance, SealedSystemValvePosition_t newPosition)
{
   uint8_t currentStep = StepLocationForPosition(instance, CurrentPosition(instance));
   uint8_t destinationStep = StepLocationForPosition(instance, newPosition);
   return ABSOLUTE_DIFFERENCE(currentStep, destinationStep);
}

static void SetCurrentPositionRequestTo(SealedSystemValveRequestManager_t *instance, SealedSystemValvePosition_t requestedPosition)
{
   StepperPositionRequest_t stepRequest = {
      .direction = TurningDirection_Clockwise,
      .stepsToMove = 0,
      .resetSubstep = false
   };

   stepRequest.direction = SteppingDirectionToNewPosition(instance, requestedPosition);
   stepRequest.stepsToMove = StepsCountToNewPosition(instance, requestedPosition);

   SetDamperStepperMotorPositionRequest(instance, stepRequest);
}

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   SealedSystemValveRequestManager_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         if(CurrentPosition(instance) != RequestedPosition(instance))
         {
            if(RequestedPosition(instance) == SealedSystemValvePosition_Home)
            {
               Fsm_Transition(fsm, State_Homing);
            }
            else
            {
               Fsm_Transition(fsm, State_Moving);
            }
         }
         break;

      case Signal_PositionRequested:
         Fsm_Transition(fsm, State_Moving);
         break;

      case Signal_HomeRequested:
         Fsm_Transition(fsm, State_Homing);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Homing(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   SealedSystemValveRequestManager_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         instance->_private.activePositionRequest = SealedSystemValvePosition_Home;
         SetStepperPositionRequestForHoming(instance);
         break;

      case Signal_StepRequestCompleted:
         SetPreviousPositionToCurrentPosition(instance);
         SetCurrentPositionTo(instance, instance->_private.activePositionRequest);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Fsm_Exit:
         instance->_private.homingRequired = false;
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->homingRequestErd,
            &instance->_private.homingRequired);
         break;
   }
}

static void State_Moving(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   SealedSystemValveRequestManager_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         instance->_private.activePositionRequest = RequestedPosition(instance);
         SetCurrentPositionRequestTo(instance, instance->_private.activePositionRequest);
         break;

      case Signal_StepRequestCompleted:
         SetPreviousPositionToCurrentPosition(instance);
         SetCurrentPositionTo(instance, instance->_private.activePositionRequest);

         if(instance->_private.homingRequired)
         {
            Fsm_Transition(fsm, State_Homing);
         }
         else
         {
            Fsm_Transition(fsm, State_Idle);
         }
         break;

      case Signal_HomeRequested:
         instance->_private.homingRequired = true;
         break;

      case Fsm_Exit:
         break;
   }
}

static void OnDataModelChange(void *context, const void *_args)
{
   SealedSystemValveRequestManager_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->positionRequestResolvedVoteErd)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_PositionRequested, NULL);
   }
   else if(args->erd == instance->_private.config->stepperMotorPositionRequestErd)
   {
      const StepperPositionRequest_t *motorPosition = args->data;
      if(motorPosition->stepsToMove == 0)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_StepRequestCompleted, NULL);
      }
   }
   else if(args->erd == instance->_private.config->homingRequestErd)
   {
      const bool *homingRequest = args->data;
      if(*homingRequest)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_HomeRequested, NULL);
      }
   }
}

void SealedSystemValveRequestManager_Init(
   SealedSystemValveRequestManager_t *instance,
   I_DataModel_t *dataModel,
   const SealedSystemValveRequestManagerConfiguration_t *config,
   const SealedSystemValveData_t *data)
{
   instance->_private.config = config;
   instance->_private.data = data;
   instance->_private.dataModel = dataModel;

   SetCurrentPositionTo(instance, SealedSystemValvePosition_Home);
   SetPreviousPositionTo(instance, SealedSystemValvePosition_Home);
   Fsm_Init(&instance->_private.fsm, State_Homing);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      OnDataModelChange);
   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
