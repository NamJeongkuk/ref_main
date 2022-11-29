/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "DefrostHeaterOnTimeCounter.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"

static DefrostState_t DefrostState(DefrostHeaterOnTimeCounter_t *instance)
{
   DefrostState_t defrostState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostStateErd,
      &defrostState);
   return defrostState;
}

static HeaterState_t DefrostHeaterState(DefrostHeaterOnTimeCounter_t *instance)
{
   HeaterVotedState_t heaterVotedState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostHeaterStateErd,
      &heaterVotedState);
   return heaterVotedState.state;
}

static void ResetHeaterOnTimeCounter(DefrostHeaterOnTimeCounter_t *instance)
{
   uint8_t zero = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostHeaterOnTimeErd,
      &zero);
}

static void IncrementHeaterOnTimeCounter(void *context)
{
   DefrostHeaterOnTimeCounter_t *instance = context;

   uint8_t count;
   DataModel_Read(instance->_private.dataModel, instance->_private.config->defrostHeaterOnTimeErd, &count);
   count++;
   DataModel_Write(instance->_private.dataModel, instance->_private.config->defrostHeaterOnTimeErd, &count);
}

static void DefrostHeaterStateChanged(void *context, const void *args)
{
   DefrostHeaterOnTimeCounter_t *instance = context;
   const HeaterVotedState_t *heaterVotedState = args;

   if((heaterVotedState->state == HeaterState_On) && (DefrostState(instance) == DefrostState_HeaterOn))
   {
      TimerModule_StartPeriodic(
         DataModelErdPointerAccess_GetTimerModule(
            instance->_private.dataModel,
            Erd_TimerModule),
         &instance->_private.periodicTimer,
         MSEC_PER_MIN,
         IncrementHeaterOnTimeCounter,
         instance);
   }
   else
   {
      TimerModule_Stop(
         DataModelErdPointerAccess_GetTimerModule(
            instance->_private.dataModel,
            Erd_TimerModule),
         &instance->_private.periodicTimer);
      ResetHeaterOnTimeCounter(instance);
   }
}

void DefrostHeaterOnTimeCounter_Init(
   DefrostHeaterOnTimeCounter_t *instance,
   I_DataModel_t *dataModel,
   const DefrostHeaterOnTimeCounterConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   if(DefrostState(instance) == DefrostState_HeaterOn)
   {
      if(DefrostHeaterState(instance) == HeaterState_On)
      {
         TimerModule_StartPeriodic(
            DataModelErdPointerAccess_GetTimerModule(
               instance->_private.dataModel,
               Erd_TimerModule),
            &instance->_private.periodicTimer,
            MSEC_PER_MIN,
            IncrementHeaterOnTimeCounter,
            instance);
      }
   }
   else
   {
      ResetHeaterOnTimeCounter(instance);
   }

   EventSubscription_Init(
      &instance->_private.subscription,
      instance,
      DefrostHeaterStateChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->defrostHeaterStateErd,
      &instance->_private.subscription);
}