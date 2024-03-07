/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TimeThatPrechillConditionsAreMet.h"
#include "ConvertibleCompartmentStateType.h"
#include "DataModelErdPointerAccess.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "CoolingMode.h"
#include "utils.h"

enum
{
   Signal_CompressorIsOff = Fsm_UserSignalStart,
   Signal_CompressorIsOn,
   Signal_CoolingModeChanged,
   Signal_OneMinutePassed,
   Signal_ConvertibleCompartmentStateChanged
};

static void State_Count(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Stop(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static TimeThatPrechillConditionsAreMet_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(TimeThatPrechillConditionsAreMet_t, _private.fsm, fsm);
}

static void DataModelChanged(void *context, const void *args)
{
   TimeThatPrechillConditionsAreMet_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->compressorIsOnErd)
   {
      const bool *state = onChangeData->data;

      if(*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_CompressorIsOn, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_CompressorIsOff, NULL);
      }
   }
   else if(erd == instance->_private.config->coolingModeErd)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_CoolingModeChanged, NULL);
   }
   else if(erd == instance->_private.config->convertibleCompartmentStateErd)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_ConvertibleCompartmentStateChanged, NULL);
   }
}

static void ResetTimeToZero(TimeThatPrechillConditionsAreMet_t *instance)
{
   uint16_t time = 0;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->timeThatPrechillConditionsAreMetInMinutesErd,
      &time);
}

static bool CompressorIsOn(TimeThatPrechillConditionsAreMet_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorIsOnErd,
      &state);

   return state;
}

static CoolingMode_t CoolingMode(TimeThatPrechillConditionsAreMet_t *instance)
{
   CoolingMode_t coolingMode;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->coolingModeErd,
      &coolingMode);

   return coolingMode;
}

static uint8_t NumberOfEvaporators(TimeThatPrechillConditionsAreMet_t *instance)
{
   return instance->_private.numberOfEvaporators;
}

static void PeriodicOneMinuteTimeout(void *context)
{
   TimeThatPrechillConditionsAreMet_t *instance = context;

   Fsm_SendSignal(&instance->_private.fsm, Signal_OneMinutePassed, NULL);
}

static void IncrementTime(TimeThatPrechillConditionsAreMet_t *instance)
{
   uint16_t time;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->timeThatPrechillConditionsAreMetInMinutesErd,
      &time);

   time = TRUNCATE_UNSIGNED_ADDITION(time, 1, UINT16_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->timeThatPrechillConditionsAreMetInMinutesErd,
      &time);
}

static ConvertibleCompartmentStateType_t ConvertibleCompartment(TimeThatPrechillConditionsAreMet_t *instance)
{
   ConvertibleCompartmentStateType_t type;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentStateErd,
      &type);

   return type;
}

static bool PrechillConditionsAreMet(TimeThatPrechillConditionsAreMet_t *instance)
{
   bool state = false;
   uint8_t numberOfEvaporators = NumberOfEvaporators(instance);

   if(CompressorIsOn(instance))
   {
      switch(numberOfEvaporators)
      {
         case 1:
         case 2:
            if(CoolingMode(instance) == CoolingMode_Freezer)
            {
               state = true;
            }
            break;

         case 3:
            if(ConvertibleCompartment(instance) == ConvertibleCompartmentStateType_FreshFood &&
               CoolingMode(instance) == CoolingMode_Freezer)
            {
               state = true;
            }

            else if(ConvertibleCompartment(instance) == ConvertibleCompartmentStateType_Freezer &&
               CoolingMode(instance) == CoolingMode_ConvertibleCompartment)
            {
               state = true;
            }
            break;
      }
   }

   return state;
}

static void StartOneMinutePeriodicTimer(TimeThatPrechillConditionsAreMet_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.periodicTimer,
      1 * MSEC_PER_MIN,
      PeriodicOneMinuteTimeout,
      instance);
}

static void StopPeriodicTimer(TimeThatPrechillConditionsAreMet_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.periodicTimer);
}

static void State_Count(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   TimeThatPrechillConditionsAreMet_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         StartOneMinutePeriodicTimer(instance);
         break;

      case Signal_OneMinutePassed:
         IncrementTime(instance);
         break;

      case Signal_CompressorIsOff:
         Fsm_Transition(&instance->_private.fsm, State_Stop);
         break;

      case Signal_CoolingModeChanged:
      case Signal_ConvertibleCompartmentStateChanged:
         if(!PrechillConditionsAreMet(instance))
         {
            Fsm_Transition(&instance->_private.fsm, State_Stop);
         }
         break;

      case Fsm_Exit:
         StopPeriodicTimer(instance);
         break;
   }
}

static void State_Stop(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   TimeThatPrechillConditionsAreMet_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         ResetTimeToZero(instance);
         break;

      case Signal_CompressorIsOn:
      case Signal_CoolingModeChanged:
      case Signal_ConvertibleCompartmentStateChanged:
         if(PrechillConditionsAreMet(instance))
         {
            Fsm_Transition(&instance->_private.fsm, State_Count);
         }
         break;

      case Fsm_Exit:
         break;
   }
}

static FsmState_t InitialState(TimeThatPrechillConditionsAreMet_t *instance)
{
   FsmState_t initialState;

   if(PrechillConditionsAreMet(instance))
   {
      initialState = State_Count;
   }
   else
   {
      initialState = State_Stop;
   }

   return initialState;
}

void TimeThatPrechillConditionsAreMet_Init(
   TimeThatPrechillConditionsAreMet_t *instance,
   I_DataModel_t *dataModel,
   const TimeThatPrechillConditionsAreMetConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.numberOfEvaporators = PersonalityParametricData_Get(dataModel)->platformData->numberOfEvaporators;

   Fsm_Init(&instance->_private.fsm, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
