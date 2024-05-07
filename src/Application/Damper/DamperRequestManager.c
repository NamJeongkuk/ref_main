/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DamperRequestManager.h"
#include "utils.h"
#include "SystemErds.h"
#include "Constants_Binary.h"

enum
{
   DontResetSubstep = false,
   ResetSubstep = true
};

enum
{
   Signal_PositionRequested = Fsm_UserSignalStart,
   Signal_StepRequestCompleted,
   Signal_HomeRequested,
   Signal_FullCycleRequested
};

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Homing(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Moving(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_FullCycle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static DamperRequestManager_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(DamperRequestManager_t, _private.fsm, fsm);
}

static DamperPosition_t DamperRequestedPosition(DamperRequestManager_t *instance)
{
   DamperVotedPosition_t damperVotedPosition;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->damperPositionRequestResolvedVoteErd,
      &damperVotedPosition);

   return damperVotedPosition.position;
}

static DamperPosition_t CurrentDamperPosition(DamperRequestManager_t *instance)
{
   DamperPosition_t damperCurrentPosition;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->damperCurrentPositionErd,
      &damperCurrentPosition);

   return damperCurrentPosition;
}

static void InitializeDamperPositionAtStartup(DamperRequestManager_t *instance)
{
   DamperPosition_t damperCurrentPosition = DamperRequestedPosition(instance);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperCurrentPositionErd,
      &damperCurrentPosition);
}

static void SetDamperStepperMotorPositionRequest(DamperRequestManager_t *instance, StepperPositionRequest_t stepsToMove)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperStepperMotorPositionRequestErd,
      &stepsToMove);
}

static void SetCurrentDamperPositionTo(DamperRequestManager_t *instance, DamperPosition_t currentPosition)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperCurrentPositionErd,
      &currentPosition);
}

static void OpenDamper(DamperRequestManager_t *instance, uint16_t stepCount, bool resetSubstep)
{
   StepperPositionRequest_t stepRequest = {
      .direction = TurningDirection_CounterClockwise,
      .stepsToMove = stepCount,
      .resetSubstep = resetSubstep
   };
   SetDamperStepperMotorPositionRequest(instance, stepRequest);
}

static void CloseDamper(DamperRequestManager_t *instance, uint16_t stepCount, bool resetSubstep)
{
   StepperPositionRequest_t stepRequest = {
      .direction = TurningDirection_Clockwise,
      .stepsToMove = stepCount,
      .resetSubstep = resetSubstep
   };
   SetDamperStepperMotorPositionRequest(instance, stepRequest);
}

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DamperRequestManager_t *instance = InstanceFromFsm(fsm);
   (void)data;

   switch(signal)
   {
      case Fsm_Entry:
         if(instance->_private.fullCycleRequired)
         {
            Fsm_Transition(fsm, State_FullCycle);
         }
         else if(instance->_private.homingRequired)
         {
            Fsm_Transition(fsm, State_Homing);
         }
         else if(CurrentDamperPosition(instance) != DamperRequestedPosition(instance))
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

      case Signal_FullCycleRequested:
         Fsm_Transition(fsm, State_FullCycle);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Homing(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DamperRequestManager_t *instance = InstanceFromFsm(fsm);
   (void)data;

   switch(signal)
   {
      case Fsm_Entry:
         CloseDamper(
            instance,
            instance->_private.damperData->stepsToHome,
            ResetSubstep);
         break;

      case Signal_StepRequestCompleted:
         SetCurrentDamperPositionTo(instance, DamperPosition_Closed);
         Fsm_Transition(fsm, State_Idle);
         break;

      case Signal_FullCycleRequested:
         instance->_private.fullCycleRequired = true;
         break;

      case Fsm_Exit:
         instance->_private.homingRequired = false;
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.configuration->damperHomingRequestErd,
            clear);
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.configuration->damperFullCycleRequestErd,
            clear);
         break;
   }
}

static void State_FullCycle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DamperRequestManager_t *instance = InstanceFromFsm(fsm);
   (void)data;

   switch(signal)
   {
      case Fsm_Entry:
         if(CurrentDamperPosition(instance) == DamperPosition_Open)
         {
            CloseDamper(
               instance,
               instance->_private.currentDamperStepData->stepsToClose,
               DontResetSubstep);
            instance->_private.closeBeforeCycling = true;
         }
         else
         {
            OpenDamper(
               instance,
               instance->_private.damperData->stepsToHome,
               DontResetSubstep);
         }
         break;

      case Signal_StepRequestCompleted:
         if(instance->_private.closeBeforeCycling)
         {
            instance->_private.closeBeforeCycling = false;
            SetCurrentDamperPositionTo(instance, DamperPosition_Closed);
            OpenDamper(
               instance,
               instance->_private.damperData->stepsToHome,
               DontResetSubstep);
         }
         else
         {
            SetCurrentDamperPositionTo(instance, DamperPosition_Open);
            Fsm_Transition(fsm, State_Homing);
         }
         break;

      case Fsm_Exit:
         instance->_private.fullCycleRequired = false;
         break;
   }
}

static void State_Moving(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DamperRequestManager_t *instance = InstanceFromFsm(fsm);
   (void)data;

   switch(signal)
   {
      case Fsm_Entry:
         if(DamperRequestedPosition(instance) == DamperPosition_Open)
         {
            OpenDamper(
               instance,
               instance->_private.currentDamperStepData->stepsToOpen,
               DontResetSubstep);
         }
         else if(DamperRequestedPosition(instance) == DamperPosition_Closed)
         {
            CloseDamper(
               instance,
               instance->_private.currentDamperStepData->stepsToClose,
               DontResetSubstep);
         }
         break;

      case Signal_StepRequestCompleted:
         SetCurrentDamperPositionTo(instance, (CurrentDamperPosition(instance) == DamperPosition_Open) ? DamperPosition_Closed : DamperPosition_Open);
         Fsm_Transition(&instance->_private.fsm, State_Idle);
         break;

      case Signal_HomeRequested:
         instance->_private.homingRequired = true;
         break;

      case Signal_FullCycleRequested:
         instance->_private.fullCycleRequired = true;
         break;

      case Fsm_Exit:
         break;
   }
}

static void SetCurrentStepsUsingConvertibleCompartmentState(DamperRequestManager_t *instance, ConvertibleCompartmentStateType_t currentState)
{
   if(currentState == ConvertibleCompartmentStateType_FreshFood)
   {
      instance->_private.currentDamperStepData = &instance->_private.damperData->convertibleDamperStepData->damperStepsAsFreshFood;
   }
   else if(currentState == ConvertibleCompartmentStateType_Freezer)
   {
      instance->_private.currentDamperStepData = &instance->_private.damperData->convertibleDamperStepData->damperStepsAsFreezer;
   }
}

static void OnDataModelChange(void *context, const void *_args)
{
   DamperRequestManager_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.configuration->damperPositionRequestResolvedVoteErd)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_PositionRequested, NULL);
   }
   else if(args->erd == instance->_private.configuration->damperStepperMotorPositionRequestErd)
   {
      const StepperPositionRequest_t *motorPosition = args->data;
      if(motorPosition->stepsToMove == 0)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_StepRequestCompleted, NULL);
      }
   }
   else if(args->erd == instance->_private.configuration->damperHomingRequestErd)
   {
      const bool *homingRequest = args->data;
      if(*homingRequest)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_HomeRequested, NULL);
      }
   }
   else if(args->erd == instance->_private.configuration->convertibleCompartmentStateErd)
   {
      if(instance->_private.damperData->damperId == DamperId_ConvertibleCompartmentDamper)
      {
         const ConvertibleCompartmentStateType_t *convertibleCompartmentState = args->data;
         SetCurrentStepsUsingConvertibleCompartmentState(instance, *convertibleCompartmentState);

         if(CurrentDamperPosition(instance) == DamperPosition_Open)
         {
            Fsm_SendSignal(&instance->_private.fsm, Signal_HomeRequested, NULL);
         }
      }
   }
   else if(args->erd == instance->_private.configuration->damperFullCycleRequestErd)
   {
      const bool *fullCycleState = args->data;
      if(*fullCycleState)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_FullCycleRequested, NULL);
      }
   }
}

void DamperRequestManager_Init(
   DamperRequestManager_t *instance,
   I_DataModel_t *dataModel,
   const DamperRequestManagerConfiguration_t *config,
   const DamperData_t *damperData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.configuration = config;
   instance->_private.damperData = damperData;

   instance->_private.homingRequired = false;
   instance->_private.fullCycleRequired = false;
   instance->_private.closeBeforeCycling = false;

   if(damperData->damperId == DamperId_NormalDamper)
   {
      instance->_private.currentDamperStepData = damperData->damperStepData;
   }
   else if(damperData->damperId == DamperId_ConvertibleCompartmentDamper)
   {
      ConvertibleCompartmentStateType_t convertibleCompartmentState;
      DataModel_Read(
         dataModel,
         instance->_private.configuration->convertibleCompartmentStateErd,
         &convertibleCompartmentState);
      SetCurrentStepsUsingConvertibleCompartmentState(instance, convertibleCompartmentState);
   }

   Fsm_Init(&instance->_private.fsm, State_Homing);

   InitializeDamperPositionAtStartup(instance);

   EventSubscription_Init(&instance->_private.dataModelSubscription, instance, OnDataModelChange);
   Event_Subscribe(dataModel->OnDataChange, &instance->_private.dataModelSubscription);
}
