/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AugerMotorController.h"
#include "AugerMotorControllerFsmState.h"
#include "utils.h"
#include "Constants_Binary.h"
#include "AugerMotorVotedIceType.h"
#include "DataModelErdPointerAccess.h"

enum
{
   Signal_StopDispensing = Fsm_UserSignalStart,
   Signal_Crushed,
   Signal_Cubed,
   Signal_RelayOperationDelayTimerExpired,
   Signal_PressureReliefTimerExpired,
};

static void State_PreIdleDelay(Fsm_t *, const FsmSignal_t, const void *);
static void State_Idle(Fsm_t *, const FsmSignal_t, const void *);
static void State_Cubed(Fsm_t *, const FsmSignal_t, const void *);
static void State_Crushed(Fsm_t *, const FsmSignal_t, const void *);
static void State_DispensingComplete(Fsm_t *, const FsmSignal_t, const void *);
static void State_MoveInReverseDirection(Fsm_t *, const FsmSignal_t, const void *);
static void State_EnsureRelaysAreOff(Fsm_t *, const FsmSignal_t, const void *);

static AugerMotorController_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(AugerMotorController_t, _private.fsm, fsm);
}

static void SetFsmStateTo(AugerMotorController_t *instance, AugerMotorControllerFsmState_t fsmState)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->augerMotorControllerFsmStateErd,
      &fsmState);
}

static void SetAugerMotorPowerTo(AugerMotorController_t *instance, bool power)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->augerMotorPowerErd,
      &power);
}

static void SetAugerMotorDirectionRelayTo(AugerMotorController_t *instance, bool direction)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->augerMotorDirectionErd,
      &direction);
}

static void ReverseDirectionDelayTimerExpired(void *context)
{
   AugerMotorController_t *instance = context;
   Fsm_SendSignal(&instance->_private.fsm, Signal_RelayOperationDelayTimerExpired, NULL);
}

static void PressureReliefTimerExpired(void *context)
{
   AugerMotorController_t *instance = context;
   Fsm_SendSignal(&instance->_private.fsm, Signal_PressureReliefTimerExpired, NULL);
}

static void StartDelayBetweenRelayOperations(AugerMotorController_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timer,
      instance->_private.augerMotorData->augerMotorReverseDirectionDelayInMsec,
      ReverseDirectionDelayTimerExpired,
      instance);
}

static void StartPressureReliefTimer(AugerMotorController_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timer,
      instance->_private.augerMotorData->augerMotorDispenseCompleteInMsec,
      PressureReliefTimerExpired,
      instance);
}

static AugerMotorIceType_t AugerMotorIceType(AugerMotorController_t *instance)
{
   AugerMotorVotedIceType_t votedIceType;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->augerMotorIceTypeVoteErd,
      &votedIceType);

   return votedIceType.iceType;
}

static void ReverseAugerDirectionRelay(AugerMotorController_t *instance)
{
   if(instance->_private.storedAugerMotorIceType == AugerMotorIceType_Cubed)
   {
      SetAugerMotorDirectionRelayTo(instance, ON);
   }
   else if(instance->_private.storedAugerMotorIceType == AugerMotorIceType_Crushed)
   {
      SetAugerMotorDirectionRelayTo(instance, OFF);
   }
}

// This delay is here to ensure that the relay operation delay holds between dispense cycles.
static void State_PreIdleDelay(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   AugerMotorController_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, AugerMotorControllerFsmState_PreIdleDelay);
         StartDelayBetweenRelayOperations(instance);
         break;

      case Signal_RelayOperationDelayTimerExpired:
         if(AugerMotorIceType(instance) == AugerMotorIceType_Off)
         {
            Fsm_Transition(&instance->_private.fsm, State_Idle);
         }
         else if(AugerMotorIceType(instance) == AugerMotorIceType_Crushed)
         {
            Fsm_Transition(&instance->_private.fsm, State_Crushed);
         }
         else if(AugerMotorIceType(instance) == AugerMotorIceType_Cubed)
         {
            Fsm_Transition(&instance->_private.fsm, State_Cubed);
         }
         break;
   }
}

void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   AugerMotorController_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, AugerMotorControllerFsmState_Idle);
         break;

      case Signal_Crushed:
         Fsm_Transition(&instance->_private.fsm, State_Crushed);
         break;

      case Signal_Cubed:
         Fsm_Transition(&instance->_private.fsm, State_Cubed);
         break;
   }
}

void State_Cubed(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   AugerMotorController_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, AugerMotorControllerFsmState_Cubed);
         instance->_private.storedAugerMotorIceType = AugerMotorIceType_Cubed;

         SetAugerMotorPowerTo(instance, ON);
         break;

      case Signal_StopDispensing:
      case Signal_Crushed:
         Fsm_Transition(&instance->_private.fsm, State_DispensingComplete);
         break;
   }
}

void State_Crushed(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   AugerMotorController_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, AugerMotorControllerFsmState_Crushed);
         instance->_private.storedAugerMotorIceType = AugerMotorIceType_Crushed;

         SetAugerMotorDirectionRelayTo(instance, ON);
         StartDelayBetweenRelayOperations(instance);
         break;

      case Signal_RelayOperationDelayTimerExpired:
         SetAugerMotorPowerTo(instance, ON);
         break;

      case Signal_StopDispensing:
      case Signal_Cubed:
         Fsm_Transition(&instance->_private.fsm, State_DispensingComplete);
         break;
   }
}

void State_DispensingComplete(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   AugerMotorController_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, AugerMotorControllerFsmState_DispensingComplete);
         SetAugerMotorPowerTo(instance, OFF);

         StartDelayBetweenRelayOperations(instance);
         break;

      case Signal_RelayOperationDelayTimerExpired:
         Fsm_Transition(&instance->_private.fsm, State_MoveInReverseDirection);
         break;
   }
}

static void State_MoveInReverseDirection(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   AugerMotorController_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, AugerMotorControllerFsmState_MoveInReverseDirection);
         ReverseAugerDirectionRelay(instance);
         StartDelayBetweenRelayOperations(instance);
         break;

      case Signal_RelayOperationDelayTimerExpired:
         SetAugerMotorPowerTo(instance, ON);
         StartPressureReliefTimer(instance);
         break;

      case Signal_PressureReliefTimerExpired:
         Fsm_Transition(&instance->_private.fsm, State_EnsureRelaysAreOff);
         break;
   }
}

static void State_EnsureRelaysAreOff(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   AugerMotorController_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, AugerMotorControllerFsmState_EnsureRelaysAreOff);
         SetAugerMotorPowerTo(instance, OFF);
         StartDelayBetweenRelayOperations(instance);
         break;

      case Signal_RelayOperationDelayTimerExpired:
         SetAugerMotorDirectionRelayTo(instance, OFF);
         Fsm_Transition(&instance->_private.fsm, State_PreIdleDelay);
         break;
   }
}

static void OnAugerMotorIceTypeVoteChange(void *context, const void *args)
{
   AugerMotorController_t *instance = context;
   const AugerMotorVotedIceType_t *votedIceType = args;

   switch(votedIceType->iceType)
   {
      case AugerMotorIceType_Off:
         Fsm_SendSignal(&instance->_private.fsm, Signal_StopDispensing, NULL);
         break;

      case AugerMotorIceType_Crushed:
         Fsm_SendSignal(&instance->_private.fsm, Signal_Crushed, NULL);
         break;

      case AugerMotorIceType_Cubed:
         Fsm_SendSignal(&instance->_private.fsm, Signal_Cubed, NULL);
         break;
   }
}

void AugerMotorController_Init(
   AugerMotorController_t *instance,
   I_DataModel_t *dataModel,
   const AugerMotorControllerConfig_t *config,
   const AugerMotorData_t *augerMotorData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.augerMotorData = augerMotorData;
   instance->_private.storedAugerMotorIceType = AugerMotorIceType_Off;

   Fsm_Init(&instance->_private.fsm, State_Idle);

   EventSubscription_Init(
      &instance->_private.augerMotorIceTypeVoteChangeSubscription,
      instance,
      OnAugerMotorIceTypeVoteChange);
   DataModel_Subscribe(
      dataModel,
      config->augerMotorIceTypeVoteErd,
      &instance->_private.augerMotorIceTypeVoteChangeSubscription);
}
