/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostDoorHoldoffTimer.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "DefrostDoorHoldoffTimerFsmState.h"

enum
{
   Signal_RequestedOn = Fsm_UserSignalStart,
   Signal_RequestedOff,

   Signal_DoorOpened,
   Signal_DoorClosed,

   Signal_HoldoffTimerComplete,
   Signal_MaxTimerComplete,
};

static void State_Disabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Enabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static TimerTicks_t GetDoorHoldoffTime(DefrostDoorHoldoffTimer_t *instance)
{
   bool freshFoodIsDefrostOnly;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodOnlyDefrost,
      &freshFoodIsDefrostOnly);

   if(freshFoodIsDefrostOnly)
   {
      return instance->_private.defrostParametricData->defrostDoorHoldoffTimeForFreshFoodOnlyInMinutes * MSEC_PER_MIN;
   }
   return instance->_private.defrostParametricData->defrostDoorHoldoffTimeForFreshFoodAndFreezerInMinutes * MSEC_PER_MIN;
}

static TimerTicks_t GetMaxHoldoffTime(DefrostDoorHoldoffTimer_t *instance)
{
   return instance->_private.defrostParametricData->defrostMaxHoldoffTimeInMinutes * MSEC_PER_MIN;
}

static void SetHoldoffSatisfiedTo(void *context, bool actual)
{
   REINTERPRET(instance, context, DefrostDoorHoldoffTimer_t *);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->doorHoldoffSatisfied,
      &actual);
}

static void HoldoffTimerComplete(void *context)
{
   REINTERPRET(instance, context, DefrostDoorHoldoffTimer_t *);
   Fsm_SendSignal(
      &instance->_private.fsm,
      Signal_HoldoffTimerComplete,
      NULL);
}

static void MaxTimerComplete(void *context)
{
   REINTERPRET(instance, context, DefrostDoorHoldoffTimer_t *);
   Fsm_SendSignal(
      &instance->_private.fsm,
      Signal_MaxTimerComplete,
      NULL);
}

static DefrostDoorHoldoffTimer_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(DefrostDoorHoldoffTimer_t, _private.fsm, fsm);
}

static uint8_t GetNumberOfEvaporators(DefrostDoorHoldoffTimer_t *instance)
{
   return instance->_private.evaporatorParametricData->numberOfEvaporators;
}

static bool FreezerDoorIsClosed(DefrostDoorHoldoffTimer_t *instance)
{
   bool actual;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDoorOpenState,
      &actual);

   return !actual;
}

static bool ConvertibleCompartmentDoorIsClosed(DefrostDoorHoldoffTimer_t *instance)
{
   bool actual;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDoorOpenState,
      &actual);

   return !actual;
}

static bool AllFreshFoodDoorsAreClosed(DefrostDoorHoldoffTimer_t *instance)
{
   bool allFreshFoodDoorsClosed;
   DataModel_Read(instance->_private.dataModel,
      instance->_private.config->allFreshFoodDoorsAreClosedState,
      &allFreshFoodDoorsClosed);

   return allFreshFoodDoorsClosed;
}

static bool HoldoffTimerShouldStart(DefrostDoorHoldoffTimer_t *instance)
{
   return ((GetNumberOfEvaporators(instance) == 1) && (AllFreshFoodDoorsAreClosed(instance))) ||
      ((GetNumberOfEvaporators(instance) == 2) && (FreezerDoorIsClosed(instance) && ConvertibleCompartmentDoorIsClosed(instance)));
}

static void StartHoldoffTimer(DefrostDoorHoldoffTimer_t *instance)
{
   TimerModule_StartOneShot(instance->_private.timerModule,
      &instance->_private.holdoffTimer,
      GetDoorHoldoffTime(instance),
      HoldoffTimerComplete,
      instance);
}

static void StopHoldoffTimer(DefrostDoorHoldoffTimer_t *instance)
{
   if(TimerModule_IsRunning(instance->_private.timerModule,
         &instance->_private.holdoffTimer))
   {
      TimerModule_Stop(instance->_private.timerModule,
         &instance->_private.holdoffTimer);
   }
}

static void SetDoorHoldoffFsmStateErdTo(DefrostDoorHoldoffTimer_t *instance, DefrostDoorHoldoffTimerFsmState_t actual)
{
   DataModel_Write(instance->_private.dataModel,
      instance->_private.config->doorHoldoffTimerFsmState,
      &actual);
}

static void DataModelUpdated(void *context, const void *_args)
{
   REINTERPRET(instance, context, DefrostDoorHoldoffTimer_t *);
   REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);
   REINTERPRET(state, args->data, const bool *);

   if(args->erd == instance->_private.config->doorHoldoffRequest)
   {
      if(*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_RequestedOn, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_RequestedOff, NULL);
      }
   }

   else if(args->erd == instance->_private.config->allFreshFoodDoorsAreClosedState)
   {
      if(*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_DoorClosed, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_DoorOpened, NULL);
      }
   }

   else if(args->erd == instance->_private.config->freezerDoorOpenState)
   {
      if(*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_DoorOpened, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_DoorClosed, NULL);
      }
   }
   else if(args->erd == instance->_private.config->convertibleCompartmentDoorOpenState)
   {
      if(*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_DoorOpened, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_DoorClosed, NULL);
      }
   }
}

static void State_Disabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DefrostDoorHoldoffTimer_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         StopHoldoffTimer(instance);
         SetDoorHoldoffFsmStateErdTo(instance, DefrostDoorHoldoffTimerFsmState_Disabled);
         break;

      case Signal_RequestedOn:
         Fsm_Transition(&instance->_private.fsm, State_Enabled);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Enabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   DefrostDoorHoldoffTimer_t *instance = InstanceFromFsm(fsm);
   IGNORE_ARG(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetDoorHoldoffFsmStateErdTo(instance, DefrostDoorHoldoffTimerFsmState_Enabled);
         if(HoldoffTimerShouldStart(instance))
         {
            StartHoldoffTimer(instance);
         }

         TimerModule_StartOneShot(instance->_private.timerModule,
            &instance->_private.maxTimer,
            GetMaxHoldoffTime(instance),
            MaxTimerComplete,
            instance);
         break;

      case Signal_RequestedOff:
         SetHoldoffSatisfiedTo(instance, DISABLED);
         Fsm_Transition(&instance->_private.fsm, State_Disabled);
         break;

      case Signal_DoorOpened:
         SetHoldoffSatisfiedTo(instance, DISABLED);
         StopHoldoffTimer(instance);
         break;

      case Signal_DoorClosed:
         if(HoldoffTimerShouldStart(instance))
         {
            SetHoldoffSatisfiedTo(instance, DISABLED);
            StartHoldoffTimer(instance);
         }
         break;

      case Signal_HoldoffTimerComplete:
      case Signal_MaxTimerComplete:
         SetHoldoffSatisfiedTo(instance, ENABLED);
         break;

      case Fsm_Exit:
         break;
   }
}

void DefrostDoorHoldoffTimer_Init(DefrostDoorHoldoffTimer_t *instance,
   const DefrostDoorHoldoffTimerConfiguration_t *config,
   I_DataModel_t *dataModel)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.timerModule = DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule);
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;
   instance->_private.evaporatorParametricData = PersonalityParametricData_Get(dataModel)->evaporatorData;

   Fsm_Init(&instance->_private.fsm, State_Disabled);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelUpdated);

   Event_Subscribe(dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
