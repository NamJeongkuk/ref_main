/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CabinetTemperatureExceededMonitor.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

enum
{
   Signal_AllDoorsAreClosed = Fsm_UserSignalStart,
   Signal_MinimumTimeDoorsMustBeClosedComplete,
   Signal_ADoorIsOpened
};

static void State_ADoorIsOpen(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_AllDoorsClosed(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_AllDoorsClosedForMinimumTime(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

enum
{
   Signal_MinimumDefrostTimeComplete = Fsm_UserSignalStart,
   Signal_DefrostIsNotInIdle,
   Signal_DefrostIsInIdle
};

static void State_DefrostIsNotInIdle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_DefrostIsInIdle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_DefrostIsInIdleForMinimumTime(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static bool AllDoorsAreClosed(CabinetTemperatureExceededMonitor_t *instance);

static void SetCabinetTemperatureExceededFaultTo(CabinetTemperatureExceededMonitor_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->faultErd,
      &state);
}

static bool CabinetTemperatureExceedsSetpointByHysteresisValue(CabinetTemperatureExceededMonitor_t *instance)
{
   TemperatureDegFx100_t cabinetTemperatureInDegFx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cabinetFilteredTemperatureInDegFX100Erd,
      &cabinetTemperatureInDegFx100);

   Setpoint_t setpointTemperatureInDegF;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cabinetSetpointTemperatureInDegFErd,
      &setpointTemperatureInDegF);

   return (cabinetTemperatureInDegFx100 > ((setpointTemperatureInDegF * 100) + instance->_private.data->temperatureUpperHysteresisLimitAboveSetpointInDegFX100));
}

static bool CabinetTemperatureIsLessThanSumOfSetpointAndHysteresisValue(CabinetTemperatureExceededMonitor_t *instance)
{
   TemperatureDegFx100_t cabinetTemperatureInDegFx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cabinetFilteredTemperatureInDegFX100Erd,
      &cabinetTemperatureInDegFx100);

   Setpoint_t setpointTemperatureInDegF;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cabinetSetpointTemperatureInDegFErd,
      &setpointTemperatureInDegF);

   return (cabinetTemperatureInDegFx100 < ((setpointTemperatureInDegF * 100) + instance->_private.data->temperatureLowerHysteresisLimitAboveSetpointInDegFX100));
}

static bool CabinetThermistorIsValid(CabinetTemperatureExceededMonitor_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cabinetThermistorIsValidErd,
      &state);

   return state;
}

static void MinimumTimeDoorsMustBeClosedComplete(void *context)
{
   CabinetTemperatureExceededMonitor_t *instance = context;

   Fsm_SendSignal(&instance->_private.doorFsm, Signal_MinimumTimeDoorsMustBeClosedComplete, NULL);
}

static void MinimumDefrostTimeComplete(void *context)
{
   CabinetTemperatureExceededMonitor_t *instance = context;

   Fsm_SendSignal(&instance->_private.defrostFsm, Signal_MinimumDefrostTimeComplete, NULL);
}

static bool ConditionsForExceededTemperatureAreMet(CabinetTemperatureExceededMonitor_t *instance)
{
   return (CabinetTemperatureExceedsSetpointByHysteresisValue(instance) &&
      instance->_private.allDoorsClosedForMinimumTime &&
      instance->_private.defrostIsInIdleForMinimumTime &&
      CabinetThermistorIsValid(instance));
}

static void State_ADoorIsOpen(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   CabinetTemperatureExceededMonitor_t *instance = CONTAINER_OF(CabinetTemperatureExceededMonitor_t, _private.doorFsm, fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         break;

      case Signal_AllDoorsAreClosed:
         Fsm_Transition(&instance->_private.doorFsm, State_AllDoorsClosed);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_AllDoorsClosed(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   CabinetTemperatureExceededMonitor_t *instance = CONTAINER_OF(CabinetTemperatureExceededMonitor_t, _private.doorFsm, fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         TimerModule_StartOneShot(
            DataModelErdPointerAccess_GetTimerModule(
               instance->_private.dataModel,
               Erd_TimerModule),
            &instance->_private.powerUpAndDoorTimer,
            instance->_private.data->minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN,
            MinimumTimeDoorsMustBeClosedComplete,
            instance);
         break;

      case Signal_MinimumTimeDoorsMustBeClosedComplete:
         Fsm_Transition(&instance->_private.doorFsm, State_AllDoorsClosedForMinimumTime);
         break;

      case Signal_ADoorIsOpened:
         TimerModule_Stop(
            DataModelErdPointerAccess_GetTimerModule(
               instance->_private.dataModel,
               Erd_TimerModule),
            &instance->_private.powerUpAndDoorTimer);

         Fsm_Transition(&instance->_private.doorFsm, State_ADoorIsOpen);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_AllDoorsClosedForMinimumTime(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   CabinetTemperatureExceededMonitor_t *instance = CONTAINER_OF(CabinetTemperatureExceededMonitor_t, _private.doorFsm, fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         instance->_private.allDoorsClosedForMinimumTime = true;

         if(ConditionsForExceededTemperatureAreMet(instance))
         {
            SetCabinetTemperatureExceededFaultTo(instance, true);
         }
         break;

      case Signal_ADoorIsOpened:
         Fsm_Transition(&instance->_private.doorFsm, State_ADoorIsOpen);
         break;

      case Fsm_Exit:
         instance->_private.allDoorsClosedForMinimumTime = false;
         break;
   }
}

static void State_DefrostIsNotInIdle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   CabinetTemperatureExceededMonitor_t *instance = CONTAINER_OF(CabinetTemperatureExceededMonitor_t, _private.defrostFsm, fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         break;

      case Signal_DefrostIsInIdle:
         Fsm_Transition(&instance->_private.defrostFsm, State_DefrostIsInIdle);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_DefrostIsInIdle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   CabinetTemperatureExceededMonitor_t *instance = CONTAINER_OF(CabinetTemperatureExceededMonitor_t, _private.defrostFsm, fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         TimerModule_StartOneShot(
            DataModelErdPointerAccess_GetTimerModule(
               instance->_private.dataModel,
               Erd_TimerModule),
            &instance->_private.defrostTimer,
            instance->_private.data->minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN,
            MinimumDefrostTimeComplete,
            instance);
         break;

      case Signal_MinimumDefrostTimeComplete:
         Fsm_Transition(&instance->_private.defrostFsm, State_DefrostIsInIdleForMinimumTime);
         break;

      case Signal_DefrostIsNotInIdle:
         TimerModule_Stop(
            DataModelErdPointerAccess_GetTimerModule(
               instance->_private.dataModel,
               Erd_TimerModule),
            &instance->_private.defrostTimer);

         Fsm_Transition(&instance->_private.defrostFsm, State_DefrostIsNotInIdle);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_DefrostIsInIdleForMinimumTime(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   CabinetTemperatureExceededMonitor_t *instance = CONTAINER_OF(CabinetTemperatureExceededMonitor_t, _private.defrostFsm, fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         instance->_private.defrostIsInIdleForMinimumTime = true;

         if(ConditionsForExceededTemperatureAreMet(instance))
         {
            SetCabinetTemperatureExceededFaultTo(instance, true);
         }
         break;

      case Signal_DefrostIsNotInIdle:
         Fsm_Transition(&instance->_private.defrostFsm, State_DefrostIsNotInIdle);
         break;

      case Fsm_Exit:
         instance->_private.defrostIsInIdleForMinimumTime = false;
         break;
   }
}

static void DataModelChanged(void *context, const void *args)
{
   CabinetTemperatureExceededMonitor_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   for(uint8_t i = 0; i < instance->_private.config->doorErdList.numberOfErds; i++)
   {
      if(erd == instance->_private.config->doorErdList.erds[i])
      {
         if(AllDoorsAreClosed(instance))
         {
            Fsm_SendSignal(&instance->_private.doorFsm, Signal_AllDoorsAreClosed, NULL);
         }
         else
         {
            Fsm_SendSignal(&instance->_private.doorFsm, Signal_ADoorIsOpened, NULL);
         }

         return;
      }
   }

   if(erd == instance->_private.config->defrostHsmStateErd)
   {
      const DefrostHsmState_t *state = onChangeData->data;

      if(*state == DefrostHsmState_Idle)
      {
         Fsm_SendSignal(&instance->_private.defrostFsm, Signal_DefrostIsInIdle, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.defrostFsm, Signal_DefrostIsNotInIdle, NULL);
      }
   }
   else if(erd == instance->_private.config->cabinetFilteredTemperatureInDegFX100Erd ||
      erd == instance->_private.config->cabinetSetpointTemperatureInDegFErd ||
      erd == instance->_private.config->cabinetThermistorIsValidErd)
   {
      if(ConditionsForExceededTemperatureAreMet(instance))
      {
         SetCabinetTemperatureExceededFaultTo(instance, true);
      }
      else if(CabinetTemperatureIsLessThanSumOfSetpointAndHysteresisValue(instance) &&
         CabinetThermistorIsValid(instance))
      {
         SetCabinetTemperatureExceededFaultTo(instance, false);
      }
   }
}

static bool DoorIsOpen(CabinetTemperatureExceededMonitor_t *instance, uint8_t doorIndex)
{
   bool doorIsOpen;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->doorErdList.erds[doorIndex],
      &doorIsOpen);

   return doorIsOpen;
}

static bool AtLeastOneDoorIsOpen(CabinetTemperatureExceededMonitor_t *instance)
{
   for(uint8_t i = 0; i < instance->_private.config->doorErdList.numberOfErds; i++)
   {
      if(DoorIsOpen(instance, i))
      {
         return true;
      }
   }
   return false;
}

static bool AllDoorsAreClosed(CabinetTemperatureExceededMonitor_t *instance)
{
   return !AtLeastOneDoorIsOpen(instance);
}

static bool DefrostIsInIdle(CabinetTemperatureExceededMonitor_t *instance)
{
   DefrostHsmState_t state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostHsmStateErd,
      &state);

   return (state == DefrostHsmState_Idle);
}

static FsmState_t DoorFsmInitialState(CabinetTemperatureExceededMonitor_t *instance)
{
   if(AtLeastOneDoorIsOpen(instance))
   {
      return State_ADoorIsOpen;
   }
   else
   {
      return State_AllDoorsClosed;
   }
}

static FsmState_t DefrostFsmInitialState(CabinetTemperatureExceededMonitor_t *instance)
{
   if(DefrostIsInIdle(instance))
   {
      return State_DefrostIsInIdle;
   }
   else
   {
      return State_DefrostIsNotInIdle;
   }
}

static void MinimumTimeSincePowerOnComplete(void *context)
{
   CabinetTemperatureExceededMonitor_t *instance = context;

   Fsm_Init(
      &instance->_private.doorFsm,
      DoorFsmInitialState(instance));

   Fsm_Init(
      &instance->_private.defrostFsm,
      DefrostFsmInitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);
   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelSubscription);
}

void CabinetTemperatureExceededMonitor_Init(
   CabinetTemperatureExceededMonitor_t *instance,
   I_DataModel_t *dataModel,
   const CabinetTemperatureExceededMonitorConfiguration_t *config,
   const CabinetTemperatureExceededData_t *data)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.data = data;
   instance->_private.allDoorsClosedForMinimumTime = false;
   instance->_private.defrostIsInIdleForMinimumTime = false;

   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.powerUpAndDoorTimer,
      instance->_private.data->minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN,
      MinimumTimeSincePowerOnComplete,
      instance);
}
