/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TimerModuleStack.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

#if defined(DEBUG)
enum
{
   MaxTicksToSkipAtOnce = 1000,
};

static const TimeSource_SystemTickWithAcceleration_Config_t acceleratedTimeSourceConfig = {
   .accelerationEnabledErd = Erd_TimeAcceleration_Enable,
   .ticksRemainingInAccelerationErd = Erd_TimeAcceleration_Ticks,
   .completeSignalErd = Erd_TimeAcceleration_CompleteSignal,
   .maxTicksPerRtc = MaxTicksToSkipAtOnce,
};
#endif

TimerModule_t *TimerModuleStack_Init(
   TimerModuleStack_t *instance, 
   I_Interrupt_t *systemTickInterrupt, 
   I_Interrupt_t *fastTickInterrupt)
{
   instance->_private.systemTickInterrupt = systemTickInterrupt;
   instance->_private.fastTickInterrupt = fastTickInterrupt;

   // Use system tick interrupt handle to set up a 1mS time source
   TimeSource_Interrupt_Init(&instance->_private.systemTickTimeSource, instance->_private.systemTickInterrupt);
   instance->_private.timeSource = &instance->_private.systemTickTimeSource.interface;

#if defined(DEBUG)
   TimeSource_SystemTickWithAcceleration_Init(
      &instance->_private.acceleratedTimeSource,
      &acceleratedTimeSourceConfig,
      systemTickInterrupt);
   instance->_private.timeSource = &instance->_private.acceleratedTimeSource.interface;
#endif

   // Plug the raw system tick time source into the timer module
   TimerModule_Init(&instance->_private.timerModule_1ms, instance->_private.timeSource);

   return &instance->_private.timerModule_1ms;
}

#if defined(DEBUG)
static void RunTimeAcceleration(void *context)
{
   REINTERPRET(instance, context, TimerModuleStack_t *);

   TimeSource_SystemTickWithAcceleration_Run(
      &instance->_private.acceleratedTimeSource);
}
#endif

void TimerModuleStack_WritePointersToDataModel(TimerModuleStack_t *instance, I_DataModel_t *dataModel)
{
   DataModelErdPointerAccess_Write(dataModel, Erd_SystemTickInterrupt, instance->_private.systemTickInterrupt);
   DataModelErdPointerAccess_Write(dataModel, Erd_FastTickInterrupt, instance->_private.fastTickInterrupt);
   DataModelErdPointerAccess_Write(dataModel, Erd_TimeSource, instance->_private.timeSource);
   DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &instance->_private.timerModule_1ms);
#if defined(DEBUG)
   TimeSource_SystemTickWithAcceleration_SetDataModel(
      &instance->_private.acceleratedTimeSource,
      dataModel);

   TimerModule_StartPeriodic(
      &instance->_private.timerModule_1ms,
      &instance->_private.runTimer,
      1,
      RunTimeAcceleration,
      instance);
#endif
}
