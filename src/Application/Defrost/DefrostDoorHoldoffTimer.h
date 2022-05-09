/*!
 * @file defrost door holdoff timer module
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTDOORHOLDOFFTIMER_H
#define DEFROSTDOORHOLDOFFTIMER_H

#include "DataModel.h"
#include "Timer.h"
#include "Fsm.h"

typedef struct
{
   Erd_t allFreshFoodDoorsAreClosedState; // bool
   Erd_t freezerDoorOpenState; // bool
   Erd_t convertibleCompartmentDoorOpenState; // bool

   Erd_t doorHoldoffRequest; // bool
   Erd_t doorHoldoffSatisfied; // bool

   Erd_t freshFoodOnlyDefrost; // bool

   Erd_t doorHoldoffTimerFsmState; // DefrostDoorHoldoffTimerFsmState_t
} DefrostDoorHoldoffTimerConfiguration_t;

typedef struct
{
   struct
   {
      const DefrostDoorHoldoffTimerConfiguration_t *config;
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      Timer_t holdoffTimer;
      Timer_t maxTimer;
      TimerModule_t *timerModule;
      Fsm_t fsm;
      const DefrostData_t *defrostParametricData;
      const EvaporatorData_t *evaporatorParametricData;
   } _private;
} DefrostDoorHoldoffTimer_t;

void DefrostDoorHoldoffTimer_Init(DefrostDoorHoldoffTimer_t *instance,
   const DefrostDoorHoldoffTimerConfiguration_t *configuration,
   I_DataModel_t *dataModel);

#endif
