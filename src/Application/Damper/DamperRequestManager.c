/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DamperRequestManager.h"
#include "utils.h"
#include "SystemErds.h"

enum
{
   FutureParametricsStepsToOpen = 650,
   FutureParametricsStepsToClose = 700,
   FutureParametricsStepsToHome = 1850
};

enum
{
   Signal_PositionRequested = Fsm_UserSignalStart,
   Signal_StepRequestCompleted,
   Signal_HomeRequested
};

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Homing(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Moving(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static DamperRequestManager_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(DamperRequestManager_t, _private.fsm, fsm);
}

static DamperVotedPosition_t ReadDamperRequestedPosition(DamperRequestManager_t *instance)
{
   DamperVotedPosition_t damperVotedPosition;

   DataModel_Read(
      instance->_private.dataModel,
      Erd_FreshFoodDamperPosition_ResolvedVote,
      &damperVotedPosition);

   return damperVotedPosition;
}

static DamperPosition_t ReadCurrentDamperPosition(DamperRequestManager_t *instance)
{
   DamperPosition_t damperCurrentPosition;

   DataModel_Read(
      instance->_private.dataModel,
      Erd_FreshFoodDamperCurrentPosition,
      &damperCurrentPosition);

   return damperCurrentPosition;
}

static void InitializeDamperPositionAtStartup(DamperRequestManager_t *instance)
{
   DamperPosition_t damperCurrentPosition = ReadDamperRequestedPosition(instance).position;

   DataModel_Write(
      instance->_private.dataModel,
      Erd_FreshFoodDamperCurrentPosition,
      &damperCurrentPosition);
}

static void WriteDamperStepRequest(DamperRequestManager_t *instance, StepperPositionRequest_t stepsToMove)
{
   DataModel_Write(
      instance->_private.dataModel,
      Erd_FreshFoodDamperStepperMotorPositionRequest,
      &stepsToMove);
}

static void WriteHomingStateDamperMotorSettings(DamperRequestManager_t *instance)
{
   StepperPositionRequest_t stepRequest;

   stepRequest.direction = CounterClockwise;
   stepRequest.stepsToMove = FutureParametricsStepsToHome;

   WriteDamperStepRequest(instance, stepRequest);
}

static void WriteCurrentDamperPositionTo(DamperRequestManager_t *instance, DamperPosition_t currentPosition)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperCurrentPositionErd,
      &currentPosition);
}

static void SetCurrentPositionRequestTo(DamperRequestManager_t *instance, DamperVotedPosition_t requestedPosition)
{
   StepperPositionRequest_t stepRequest;
   DamperPosition_t damperCurrentPosition;

   if(requestedPosition.position == DamperPosition_Open)
   {
      stepRequest.direction = Clockwise;
      stepRequest.stepsToMove = FutureParametricsStepsToOpen;
      damperCurrentPosition = DamperPosition_Open;
   }
   else if(requestedPosition.position == DamperPosition_Closed)
   {
      stepRequest.direction = CounterClockwise;
      stepRequest.stepsToMove = FutureParametricsStepsToClose;
      damperCurrentPosition = DamperPosition_Closed;
   }

   WriteCurrentDamperPositionTo(instance, damperCurrentPosition);
   WriteDamperStepRequest(instance, stepRequest);
}

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DamperRequestManager_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         if(ReadCurrentDamperPosition(instance) != ReadDamperRequestedPosition(instance).position)
         {
            Fsm_Transition(fsm, State_Moving);
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
   DamperRequestManager_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         WriteHomingStateDamperMotorSettings(instance);
         WriteCurrentDamperPositionTo(instance, DamperPosition_Closed);
         break;

      case Signal_StepRequestCompleted:
         Fsm_Transition(fsm, State_Idle);
         break;

      case Fsm_Exit:
         instance->_private.homingRequired = false;
         break;
   }
}

static void State_Moving(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DamperRequestManager_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetCurrentPositionRequestTo(instance, ReadDamperRequestedPosition(instance));
         break;

      case Signal_StepRequestCompleted:
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
   DamperRequestManager_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == Erd_FreshFoodDamperPosition_ResolvedVote)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_PositionRequested, NULL);
   }
   else if(args->erd == Erd_FreshFoodDamperStepperMotorPositionRequest)
   {
      const StepperPositionRequest_t *motorPosition = args->data;
      if(motorPosition->stepsToMove == 0)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_StepRequestCompleted, NULL);
      }
   }
   else if(args->erd == Erd_FreshFoodDamperHomingRequest)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_HomeRequested, NULL);
   }
}

void DamperRequestManager_Init(
   DamperRequestManager_t *instance,
   I_DataModel_t *dataModel,
   const DamperRequestManagerConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.configuration = config;

   Fsm_Init(&instance->_private.fsm, State_Homing);

   InitializeDamperPositionAtStartup(instance);

   EventSubscription_Init(&instance->_private.dataModelSubsciption, instance, OnDataModelChange);
   Event_Subscribe(dataModel->OnDataChange, &instance->_private.dataModelSubsciption);
}
