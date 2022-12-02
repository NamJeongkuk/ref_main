/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Constants_Binary.h"
#include "Signal.h"
#include "TimeSource_SysTickWithAcceleration.h"
#include "utils.h"

static void Tick(void *context, const void *_args)
{
   REINTERPRET(instance, context, TimeSource_SysTickWithAcceleration_t *);
   IGNORE(_args);

   instance->_private.ticks++;
}

static TimeSourceTickCount_t GetTicks(I_TimeSource_t *_instance)
{
   REINTERPRET(instance, _instance, TimeSource_SysTickWithAcceleration_t *);
   TimeSourceTickCount_t last;
   TimeSourceTickCount_t current;

   current = instance->_private.ticks;

   do
   {
      last = current;
      current = instance->_private.ticks;
   } while(last != current);

   return current;
}

static const I_TimeSource_Api_t api = { GetTicks };

void TimeSource_SysTickWithAcceleration_Init(
   TimeSource_SysTickWithAcceleration_t *instance,
   const TimeSource_SysTickWithAcceleration_Config_t *config,
   I_Interrupt_t *sysTickInterrupt)
{
   instance->_private.config = config;
   instance->_private.sysTickInterrupt = sysTickInterrupt;
   instance->interface.api = &api;

   EventSubscription_Init(
      &instance->_private.sysTickSubscription,
      instance,
      Tick);
   Event_Subscribe(
      instance->_private.sysTickInterrupt->OnInterrupt,
      &instance->_private.sysTickSubscription);
}

void TimeSource_SysTickWithAcceleration_SetDataModel(
   TimeSource_SysTickWithAcceleration_t *instance,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
}

void TimeSource_SysTickWithAcceleration_Run(
   TimeSource_SysTickWithAcceleration_t *instance)
{
   bool accelerationEnabled;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->accelerationEnabledErd,
      &accelerationEnabled);

   if(accelerationEnabled)
   {
      const uint32_t maxTicksPerRtc = instance->_private.config->maxTicksPerRtc;

      uint32_t ticksRemaining;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->ticksRemainingInAccelerationErd,
         &ticksRemaining);

      if(ticksRemaining <= maxTicksPerRtc)
      {
         instance->_private.ticks += ticksRemaining;
         ticksRemaining = 0;
      }
      else
      {
         instance->_private.ticks += maxTicksPerRtc;
         ticksRemaining -= maxTicksPerRtc;
      }

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->ticksRemainingInAccelerationErd,
         &ticksRemaining);

      if(ticksRemaining == 0)
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->accelerationEnabledErd,
            disabled);
         Signal_SendViaErd(
            DataModel_AsDataSource(instance->_private.dataModel),
            instance->_private.config->completeSignalErd);
      }
   }
}
