/*!
 * @file
 * @brief Monitors the defrost timer counts and when it reaches a certain threshold if certain conditions are true
 * then it sets the defrost timer is satisfied ERD
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTIMERISSATISFIEDMONITOR_H
#define DEFROSTTIMERISSATISFIEDMONITOR_H

#include "I_DataModel.h"
#include "DefrostData.h"
#include "SabbathData.h"
#include "Timer.h"
#include "Fsm.h"

typedef struct
{
   Erd_t defrostTimerIsSatisfiedMonitorFsmStateErd; // DefrostTimerIsSatisfiedMonitorFsmState_t
   Erd_t ramDefrostTimerCountInSecondsErd; // uint32_t
   Erd_t defrostTimerIsSatisfiedMonitorRequestErd; // DefrostTimerIsSatisfiedMonitorRequest_t
   Erd_t defrostTimerIsSatisfiedErd; // bool
   Erd_t sabbathModeErd; // bool
   Erd_t freezerDefrostWasAbnormalErd; // bool
   Erd_t freshFoodDefrostWasAbnormalErd; // bool
   Erd_t demandResponseLevelErd; // EnergyDemandLevel_t
   Erd_t maxTimeBetweenDefrostsInMinutesErd; // uint16_t
} DefrostTimerIsSatisfiedMonitorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Fsm_t fsm;
      Timer_t timer;
      const DefrostTimerIsSatisfiedMonitorConfig_t *config;
      const DefrostData_t *defrostParametricData;
      const SabbathData_t *sabbathParametricData;
      EventSubscription_t dataModelChangedSubscription;
      FsmSignal_t lastPublishedSignal;
   } _private;
} DefrostTimerIsSatisfiedMonitor_t;

void DefrostTimerIsSatisfiedMonitor_Init(
   DefrostTimerIsSatisfiedMonitor_t *instance,
   I_DataModel_t *dataModel,
   const DefrostTimerIsSatisfiedMonitorConfig_t *config);

#endif
