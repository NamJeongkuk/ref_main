/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DamperFreezePrevention.h"
#include "DamperFreezePreventionFsmState.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "PercentageDutyCycleVote.h"

enum
{
   Signal_TargetThermistorIsInvalid = Fsm_UserSignalStart,
   Signal_TargetThermistorIsValid,
   Signal_SourceThermistorIsValid,
   Signal_SourceThermistorIsInvalid,
   Signal_DamperHeaterOnTimerExpired,
   Signal_CurrentDamperPositionChanged,
   Signal_TargetTimerExpired,
   Signal_SourceTemperatureTooHighToRunTemperatureMonitoring,
   Signal_SourceTemperatureLowEnoughToRunTemperatureMonitoring
};

static void State_Idle(Fsm_t *, const FsmSignal_t, const void *);
static void State_MonitoringTemperatureChange(Fsm_t *, const FsmSignal_t, const void *);
static void State_DamperHeaterOn(Fsm_t *, const FsmSignal_t, const void *);
static void State_MoveDamper(Fsm_t *, const FsmSignal_t, const void *);

static DamperFreezePrevention_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(DamperFreezePrevention_t, _private.fsm, fsm);
}

static void SetFsmStateTo(DamperFreezePrevention_t *instance, DamperFreezePreventionFsmState_t fsmState)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperFreezePreventionFsmStateErd,
      &fsmState);
}

static TemperatureDegFx100_t TargetTemperature(DamperFreezePrevention_t *instance)
{
   TemperatureDegFx100_t targetTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->targetFilteredTemperatureErd,
      &targetTemperature);

   return targetTemperature;
}

static TemperatureDegFx100_t SourceTemperature(DamperFreezePrevention_t *instance)
{
   TemperatureDegFx100_t sourceTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->sourceFilteredTemperatureErd,
      &sourceTemperature);

   return sourceTemperature;
}

static bool TargetThermistorIsValid(DamperFreezePrevention_t *instance)
{
   bool valid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->targetThermistorIsValidResolvedErd,
      &valid);

   return valid;
}

static bool SourceThermistorIsValid(DamperFreezePrevention_t *instance)
{
   bool valid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->sourceThermistorIsValidResolvedErd,
      &valid);

   return valid;
}

static void DamperHeaterTimerExpired(void *context)
{
   DamperFreezePrevention_t *instance = context;
   Fsm_SendSignal(
      &instance->_private.fsm,
      Signal_DamperHeaterOnTimerExpired,
      NULL);
}

static void MinTemperatureChangeTimerExpired(void *context)
{
   DamperFreezePrevention_t *instance = context;
   Fsm_SendSignal(
      &instance->_private.fsm,
      Signal_TargetTimerExpired,
      NULL);
}

static void StartTimerFor(DamperFreezePrevention_t *instance, TimerTicks_t ticks, void *context)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.timer,
      ticks,
      context,
      instance);
}

static DamperPosition_t CurrentDamperPosition(DamperFreezePrevention_t *instance)
{
   DamperPosition_t position;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->damperCurrentPositionErd,
      &position);

   return position;
}

static void VoteDamperHeaterOn(DamperFreezePrevention_t *instance)
{
   PercentageDutyCycleVote_t vote;
   vote.percentageDutyCycle = PercentageDutyCycle_Max;
   vote.care = Vote_Care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperHeaterVoteErd,
      &vote);
}

static void VoteDamperHeaterOff(DamperFreezePrevention_t *instance)
{
   PercentageDutyCycleVote_t vote;
   vote.percentageDutyCycle = PercentageDutyCycle_Min;
   vote.care = Vote_Care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperHeaterVoteErd,
      &vote);
}

static void VoteDamperOpen(DamperFreezePrevention_t *instance)
{
   DamperVotedPosition_t vote;
   vote.position = DamperPosition_Open;
   vote.care = Vote_Care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperPositionVoteErd,
      &vote);
}

static void VoteDamperClosed(DamperFreezePrevention_t *instance)
{
   DamperVotedPosition_t vote;
   vote.position = DamperPosition_Closed;
   vote.care = Vote_Care;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperPositionVoteErd,
      &vote);
}

static void VoteDamperDontCare(DamperFreezePrevention_t *instance)
{
   DamperVotedPosition_t vote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->damperPositionVoteErd,
      &vote);

   vote.care = Vote_DontCare;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->damperPositionVoteErd,
      &vote);
}

static bool ReadyToMonitorTemperature(DamperFreezePrevention_t *instance)
{
   return (TargetThermistorIsValid(instance) &&
      SourceThermistorIsValid(instance) &&
      SourceTemperature(instance) <
         instance->_private.singleDamperData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
}

void State_MoveDamper(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IGNORE(data);
   DamperFreezePrevention_t *instance = InstanceFromFsm(fsm);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DamperFreezePreventionFsmState_MoveDamper);
         if(CurrentDamperPosition(instance) == DamperPosition_Closed)
         {
            VoteDamperOpen(instance);
         }
         else
         {
            VoteDamperClosed(instance);
         }
         break;

      case Signal_CurrentDamperPositionChanged:
         VoteDamperDontCare(instance);
         if(ReadyToMonitorTemperature(instance))
         {
            Fsm_Transition(fsm, State_MonitoringTemperatureChange);
         }
         else
         {
            Fsm_Transition(fsm, State_Idle);
         }
         break;

      case Fsm_Exit:
         break;
   }
}

void State_DamperHeaterOn(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IGNORE(data);
   DamperFreezePrevention_t *instance = InstanceFromFsm(fsm);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DamperFreezePreventionFsmState_DamperHeaterOn);
         VoteDamperHeaterOn(instance);
         StartTimerFor(
            instance,
            (instance->_private.singleDamperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN),
            DamperHeaterTimerExpired);
         break;

      case Signal_DamperHeaterOnTimerExpired:
         VoteDamperHeaterOff(instance);
         Fsm_Transition(fsm, State_MoveDamper);
         break;

      case Fsm_Exit:

         break;
   }
}

void State_MonitoringTemperatureChange(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IGNORE(data);
   DamperFreezePrevention_t *instance = InstanceFromFsm(fsm);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DamperFreezePreventionFsmState_MonitoringTemperatureChange);
         break;

      case Signal_CurrentDamperPositionChanged:
         instance->_private.startingTemperature = TargetTemperature(instance);

         StartTimerFor(
            instance,
            (instance->_private.singleDamperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN),
            MinTemperatureChangeTimerExpired);
         break;

      case Signal_TargetTimerExpired:
         if(CurrentDamperPosition(instance) == DamperPosition_Open)
         {
            if((instance->_private.startingTemperature - TargetTemperature(instance)) <
               instance->_private.singleDamperData->targetCompartmentMinimumTemperatureChangeInDegFx100)
            {
               Fsm_Transition(fsm, State_DamperHeaterOn);
            }
         }
         else
         {
            if((TargetTemperature(instance) - instance->_private.startingTemperature) <
               instance->_private.singleDamperData->targetCompartmentMinimumTemperatureChangeInDegFx100)
            {
               Fsm_Transition(fsm, State_DamperHeaterOn);
            }
         }
         break;

      case Signal_SourceTemperatureTooHighToRunTemperatureMonitoring:
      case Signal_TargetThermistorIsInvalid:
      case Signal_SourceThermistorIsInvalid:
         Fsm_Transition(fsm, State_Idle);
         break;

      case Fsm_Exit:
         break;
   }
}

void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IGNORE(data);
   DamperFreezePrevention_t *instance = InstanceFromFsm(fsm);

   switch(signal)
   {
      case Fsm_Entry:
         SetFsmStateTo(instance, DamperFreezePreventionFsmState_Idle);
         break;

      case Signal_TargetThermistorIsValid:
      case Signal_SourceThermistorIsValid:
      case Signal_SourceTemperatureLowEnoughToRunTemperatureMonitoring:
         if(ReadyToMonitorTemperature(instance))
         {
            Fsm_Transition(fsm, State_MonitoringTemperatureChange);
         }
         break;

      case Fsm_Exit:
         break;
   }
}

static void DataModelChanged(void *context, const void *args)
{
   DamperFreezePrevention_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.configuration->targetThermistorIsValidResolvedErd)
   {
      const bool *targetThermistorIsValid = onChangeData->data;

      if(*targetThermistorIsValid)
      {
         Fsm_SendSignal(
            &instance->_private.fsm,
            Signal_TargetThermistorIsValid,
            NULL);
      }
      else
      {
         Fsm_SendSignal(
            &instance->_private.fsm,
            Signal_TargetThermistorIsInvalid,
            NULL);
      }
   }
   else if(erd == instance->_private.configuration->sourceThermistorIsValidResolvedErd)
   {
      const bool *sourceThermistorIsValid = onChangeData->data;

      if(*sourceThermistorIsValid)
      {
         Fsm_SendSignal(
            &instance->_private.fsm,
            Signal_SourceThermistorIsValid,
            NULL);
      }
      else
      {
         Fsm_SendSignal(
            &instance->_private.fsm,
            Signal_SourceThermistorIsInvalid,
            NULL);
      }
   }
   else if(erd == instance->_private.configuration->damperCurrentPositionErd)
   {
      Fsm_SendSignal(
         &instance->_private.fsm,
         Signal_CurrentDamperPositionChanged,
         NULL);
   }
   else if(erd == instance->_private.configuration->sourceFilteredTemperatureErd)
   {
      const TemperatureDegFx100_t *sourceTemperature = onChangeData->data;

      if(*sourceTemperature >= instance->_private.singleDamperData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100)
      {
         Fsm_SendSignal(
            &instance->_private.fsm,
            Signal_SourceTemperatureTooHighToRunTemperatureMonitoring,
            NULL);
      }
      else
      {
         Fsm_SendSignal(
            &instance->_private.fsm,
            Signal_SourceTemperatureLowEnoughToRunTemperatureMonitoring,
            NULL);
      }
   }
}

static FsmState_t InitialState(DamperFreezePrevention_t *instance)
{
   if(ReadyToMonitorTemperature(instance))
   {
      return State_MonitoringTemperatureChange;
   }
   return State_Idle;
}

void DamperFreezePrevention_Init(
   DamperFreezePrevention_t *instance,
   I_DataModel_t *dataModel,
   const DamperFreezePreventionConfiguration_t *configuration,
   const SingleDamperData_t *singleDamperData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.configuration = configuration;
   instance->_private.singleDamperData = singleDamperData;

   Fsm_Init(&instance->_private.fsm, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
