/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TimerModuleStack.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "Cmt0.h"

TimerModule_t * TimerModuleStack_Init(TimerModuleStack_t *instance)
{
   instance->_private.systemTickInterrupt = Cmt0_Init();

   // Use system tick interrupt handle to set up a 1mS time source
   TimeSource_Interrupt_Init(&instance->_private.systemTickTimeSource, instance->_private.systemTickInterrupt);

   // Plug the raw system tick time source into the timer module
   TimerModule_Init(&instance->_private.timerModule_1ms, &instance->_private.systemTickTimeSource.interface);

   return &instance->_private.timerModule_1ms;
}

void TimerModuleStack_WritePointersToDataModel(TimerModuleStack_t *instance, I_DataModel_t *dataModel)
{
   DataModelErdPointerAccess_Write(dataModel, Erd_SystemTickInterrupt, instance->_private.systemTickInterrupt);
   DataModelErdPointerAccess_Write(dataModel, Erd_TimeSource, &instance->_private.systemTickTimeSource.interface);
   DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &instance->_private.timerModule_1ms);
}
