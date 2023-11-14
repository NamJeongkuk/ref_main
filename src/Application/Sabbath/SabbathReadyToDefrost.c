/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SabbathReadyToDefrost.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "utils.h"

enum
{
   UpdatePeriodInMs = MSEC_PER_MIN
};

enum
{
   Signal_OneMinuteTimerExpired = Fsm_UserSignalStart,
   Signal_WaitingToDefrostChanged
};

static void State_WaitingToDefrost(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Defrosting(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static void IncrementSabbathWaitingForDefrostTimeInMinutes(SabbathReadyToDefrost_t *instance)
{
   uint16_t minutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathWaitingForDefrostTimeInMinutesErd,
      &minutes);

   minutes++;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sabbathWaitingForDefrostTimeInMinutesErd,
      &minutes);
}

static uint16_t SabbathWaitingForDefrostTimeInMinutes(SabbathReadyToDefrost_t *instance)
{
   uint16_t minutes;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathWaitingForDefrostTimeInMinutesErd,
      &minutes);

   return minutes;
}

static uint16_t SabbathTimeBetweenDefrostsInMinutes(SabbathReadyToDefrost_t *instance)
{
   return instance->_private.sabbathData->timeBetweenDefrostsInMinutes;
}

static void OneMinuteTimerExpired(void *context)
{
   SabbathReadyToDefrost_t *instance = context;

   Fsm_SendSignal(
      &instance->_private.fsm,
      Signal_OneMinuteTimerExpired,
      NULL);
}

static void SetSabbathReadyToDefrostTo(SabbathReadyToDefrost_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sabbathIsReadyToDefrostErd,
      &state);
}

static void ResetSabbathWaitingForDefrostTime(SabbathReadyToDefrost_t *instance)
{
   uint16_t zeroMinutes = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sabbathWaitingForDefrostTimeInMinutesErd,
      &zeroMinutes);
}

static void State_WaitingToDefrost(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   SabbathReadyToDefrost_t *instance = CONTAINER_OF(SabbathReadyToDefrost_t, _private.fsm, fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         if(SabbathWaitingForDefrostTimeInMinutes(instance) >= SabbathTimeBetweenDefrostsInMinutes(instance))
         {
            SetSabbathReadyToDefrostTo(instance, true);
         }
         else
         {
            SetSabbathReadyToDefrostTo(instance, false);
         }

         TimerModule_StartPeriodic(
            DataModelErdPointerAccess_GetTimerModule(
               instance->_private.dataModel,
               instance->_private.config->timerModuleErd),
            &instance->_private.timer,
            UpdatePeriodInMs,
            OneMinuteTimerExpired,
            instance);
         break;

      case Signal_OneMinuteTimerExpired:
         IncrementSabbathWaitingForDefrostTimeInMinutes(instance);
         if(SabbathWaitingForDefrostTimeInMinutes(instance) >= SabbathTimeBetweenDefrostsInMinutes(instance))
         {
            SetSabbathReadyToDefrostTo(instance, true);
         }
         break;

      case Signal_WaitingToDefrostChanged:
         Fsm_Transition(fsm, State_Defrosting);
         break;

      case Fsm_Exit:
         TimerModule_Stop(
            DataModelErdPointerAccess_GetTimerModule(
               instance->_private.dataModel,
               instance->_private.config->timerModuleErd),
            &instance->_private.timer);
         break;
   }
}

static void State_Defrosting(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   SabbathReadyToDefrost_t *instance = CONTAINER_OF(SabbathReadyToDefrost_t, _private.fsm, fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         break;

      case Signal_WaitingToDefrostChanged:
         Fsm_Transition(fsm, State_WaitingToDefrost);
         break;

      case Fsm_Exit:
         ResetSabbathWaitingForDefrostTime(instance);
         break;
   }
}

static bool WaitingToDefrost(SabbathReadyToDefrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waitingToDefrostErd,
      &state);

   return state;
}

static void DataModelChanged(void *context, const void *_args)
{
   SabbathReadyToDefrost_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->waitingToDefrostErd)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_WaitingToDefrostChanged, NULL);
   }
}

void SabbathReadyToDefrost_Init(
   SabbathReadyToDefrost_t *instance,
   I_DataModel_t *dataModel,
   const SabbathReadyToDefrostConfig_t *config,
   const SabbathData_t *sabbathData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.sabbathData = sabbathData;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->sabbathTimeBetweenDefrostsInMinutesErd,
      &instance->_private.sabbathData->timeBetweenDefrostsInMinutes);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);

   Fsm_Init(
      &instance->_private.fsm,
      WaitingToDefrost(instance) ? State_WaitingToDefrost : State_Defrosting);
}
