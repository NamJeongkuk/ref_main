/*!
 * @file
 * @brief Initialize all components needed to set up a timer module
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TIMERMODULESTACK_H
#define TIMERMODULESTACK_H

#include "Timer.h"
#include "TimeSource_Interrupt.h"
#include "I_Interrupt.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      TimerModule_t timerModule_1ms;
      TimeSource_Interrupt_t systemTickTimeSource;
      I_Interrupt_t *systemTickInterrupt;
   } _private;
} TimerModuleStack_t;

/*!
 * Initialize everything needed for a timer module
 * @param instance
 * @param systemTickInterrupt
 * @return The initialized timer module interface
 */
TimerModule_t *TimerModuleStack_Init(TimerModuleStack_t *instance, I_Interrupt_t *systemTickInterrupt);

/*!
 * Writes system tick interrupt, time source, and timer module pointers to the data model
 * @param instance
 * @param dataModel
 */
void TimerModuleStack_WritePointersToDataModel(TimerModuleStack_t *instance, I_DataModel_t *dataModel);

#endif
