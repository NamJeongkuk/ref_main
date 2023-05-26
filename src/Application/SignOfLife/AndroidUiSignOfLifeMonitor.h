/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ANDROIDUISIGNOFLIFEMONITOR_H
#define ANDROIDUISIGNOFLIFEMONITOR_H

#include "I_DataModel.h"
#include "HeartbeatTick.h"
#include "Fsm.h"

typedef struct
{
   Erd_t heartbeatTickFromUiErd; // uint8_t
   Erd_t uiSignOfLifeFsmStateErd; // AndroidUiSignOfLifeFsmState_t
   Erd_t mainboardIsConnectedToUiNvErd; // bool
   Erd_t numberOfTimesMainboardHasLostCommunicationWithUi; // uint16_t
   Erd_t numberOfTimesMainboardHasAttemptedToResetUi; // uint16_t
   Erd_t uiResetSignalErd; // Signal_t
   uint16_t maxUiUpdateIntervalInMinutes; // uint16_t
   uint16_t resetTimeInMinutes; // uint16_t
   uint16_t faultTimeInMinutes; // uint16_t
} AndroidUiSignOfLifeMonitorConfiguration_t;

typedef struct
{
   struct
   {
      const AndroidUiSignOfLifeMonitorConfiguration_t *config;
      I_DataModel_t *dataModel;
      EventSubscription_t uiHeartbeatChangedSubscription;
      TimerModule_t *timerModule;
      Timer_t resetTimer;
      Timer_t signOfLifeTimer;
      Timer_t faultTimer;
      Fsm_t fsm;
      EventSubscription_t dataModelChangedSubscription;
   } _private;
} AndroidUiSignOfLifeMonitor_t;

/*!
 * @param instance
 * @param config
 * @param dataModel
 */
void AndroidUiSignOfLifeMonitor_Init(
   AndroidUiSignOfLifeMonitor_t *instance,
   const AndroidUiSignOfLifeMonitorConfiguration_t *config,
   I_DataModel_t *dataModel);
#endif
