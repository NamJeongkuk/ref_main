/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AndroidUiSignOfLifeMonitor.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "AndroidUiSignOfLifeFsmState.h"
#include "Constants_Time.h"

enum
{
   Signal_SignOfLifeChanged = Fsm_UserSignalStart,
   Signal_ResetTimerExpired,
   Signal_FaultTimerExpired,
   Signal_SignOfLifeTimerExpired,
   Signal_AndroidUiConnectedToMainboard
};

static void State_LostCommunicationWithAndroidUi(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_Fault(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_HasCommunicationWithAndroidUi(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_Idle(Fsm_t *fsm, FsmSignal_t signal, const void *data);

static AndroidUiSignOfLifeMonitor_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(AndroidUiSignOfLifeMonitor_t, _private.fsm, fsm);
}

static void UpdateFsmState(AndroidUiSignOfLifeMonitor_t *instance, AndroidUiSignOfLifeFsmState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->uiSignOfLifeFsmStateErd,
      &state);
}

static void SignOfLifeTimerExpired(void *context)
{
   AndroidUiSignOfLifeMonitor_t *instance = context;

   Fsm_SendSignal(
      &instance->_private.fsm,
      Signal_SignOfLifeTimerExpired,
      NULL);
}

static void StartSignOfLifeTimer(AndroidUiSignOfLifeMonitor_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.signOfLifeTimer,
      instance->_private.config->maxUiUpdateIntervalInMinutes * MSEC_PER_MIN,
      SignOfLifeTimerExpired,
      instance);
}

static void ResetTimerExpired(void *context)
{
   AndroidUiSignOfLifeMonitor_t *instance = context;

   Fsm_SendSignal(
      &instance->_private.fsm,
      Signal_ResetTimerExpired,
      NULL);
}

static void StartResetTimer(AndroidUiSignOfLifeMonitor_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.resetTimer,
      (instance->_private.config->resetTimeInMinutes - instance->_private.config->maxUiUpdateIntervalInMinutes) * MSEC_PER_MIN,
      ResetTimerExpired,
      instance);
}

static void FaultTimerExpired(void *context)
{
   AndroidUiSignOfLifeMonitor_t *instance = context;

   Fsm_SendSignal(
      &instance->_private.fsm,
      Signal_FaultTimerExpired,
      NULL);
}

static void StartFaultTimer(AndroidUiSignOfLifeMonitor_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.faultTimer,
      (instance->_private.config->faultTimeInMinutes - instance->_private.config->maxUiUpdateIntervalInMinutes) * MSEC_PER_MIN,
      FaultTimerExpired,
      instance);
}

static void IncrementCommunicationLostErd(AndroidUiSignOfLifeMonitor_t *instance)
{
   Signal_SendViaErd(
      DataModel_AsDataSource(instance->_private.dataModel),
      instance->_private.config->numberOfTimesMainboardHasLostCommunicationWithUi);
}

static void IncrementNumberOfUiResetsErd(AndroidUiSignOfLifeMonitor_t *instance)
{
   Signal_SendViaErd(
      DataModel_AsDataSource(instance->_private.dataModel),
      instance->_private.config->numberOfTimesMainboardHasAttemptedToResetUi);
}

static void SendUiResetSignal(AndroidUiSignOfLifeMonitor_t *instance)
{
   Signal_SendViaErd(
      DataModel_AsDataSource(instance->_private.dataModel),
      instance->_private.config->uiResetSignalErd);
}

static bool AndroidUiIsConnected(AndroidUiSignOfLifeMonitor_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->mainboardIsConnectedToUiNvErd,
      &state);
   return state;
}

static void State_HasCommunicationWithAndroidUi(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   AndroidUiSignOfLifeMonitor_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateFsmState(instance, AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);
         StartSignOfLifeTimer(instance);
         break;

      case Signal_SignOfLifeTimerExpired:
         Fsm_Transition(fsm, State_LostCommunicationWithAndroidUi);
         break;

      case Signal_SignOfLifeChanged:
         StartSignOfLifeTimer(instance);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_LostCommunicationWithAndroidUi(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   AndroidUiSignOfLifeMonitor_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateFsmState(instance, AndroidUiSignOfLifeFsmState_LostCommunicationWithUi);
         StartFaultTimer(instance);
         StartResetTimer(instance);
         IncrementCommunicationLostErd(instance);
         break;

      case Signal_SignOfLifeChanged:
         Fsm_Transition(fsm, State_HasCommunicationWithAndroidUi);
         break;

      case Signal_ResetTimerExpired:
         IncrementNumberOfUiResetsErd(instance);
         SendUiResetSignal(instance);
         break;

      case Signal_FaultTimerExpired:
         Fsm_Transition(fsm, State_Fault);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Fault(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   AndroidUiSignOfLifeMonitor_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateFsmState(instance, AndroidUiSignOfLifeFsmState_Fault);
         break;

      case Signal_SignOfLifeChanged:
         Fsm_Transition(fsm, State_HasCommunicationWithAndroidUi);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_Idle(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   AndroidUiSignOfLifeMonitor_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateFsmState(instance, AndroidUiSignOfLifeFsmState_Idle);
         if(AndroidUiIsConnected(instance))
         {
            StartSignOfLifeTimer(instance);
         }
         break;

      case Signal_AndroidUiConnectedToMainboard:
         StartSignOfLifeTimer(instance);
         break;

      case Signal_SignOfLifeChanged:
         Fsm_Transition(fsm, State_HasCommunicationWithAndroidUi);
         break;

      case Signal_SignOfLifeTimerExpired:
         Fsm_Transition(fsm, State_LostCommunicationWithAndroidUi);
         break;

      case Fsm_Exit:
         break;
   }
}

static void DataModelChanged(void *context, const void *args)
{
   AndroidUiSignOfLifeMonitor_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeArgs = args;

   if(onChangeArgs->erd == instance->_private.config->heartbeatTickFromUiErd)
   {
      Fsm_SendSignal(
         &instance->_private.fsm,
         Signal_SignOfLifeChanged,
         NULL);
   }
   else if(onChangeArgs->erd == instance->_private.config->mainboardIsConnectedToUiNvErd)
   {
      const bool *uiIsConnected = onChangeArgs->data;
      if(*uiIsConnected)
      {
         Fsm_SendSignal(
            &instance->_private.fsm,
            Signal_AndroidUiConnectedToMainboard,
            NULL);
      }
   }
}

void AndroidUiSignOfLifeMonitor_Init(
   AndroidUiSignOfLifeMonitor_t *instance,
   const AndroidUiSignOfLifeMonitorConfiguration_t *config,
   I_DataModel_t *dataModel)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.timerModule = DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule);

   EventSubscription_Init(
      &instance->_private.dataModelChangedSubscription,
      instance,
      DataModelChanged);

   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelChangedSubscription);

   Fsm_Init(
      &instance->_private.fsm,
      State_Idle);
}
