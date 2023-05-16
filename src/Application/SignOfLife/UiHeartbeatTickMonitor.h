/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef UIHEARTBEATTICKMONITOR_H
#define UIHEARTBEATTICKMONITOR_H

#include "I_DataModel.h"
#include "HeartbeatTick.h"

typedef struct
{
   Erd_t heartbeatTickFromUiErd; // uint8_t
   Erd_t activeCommunicationWithUiErd; // bool
   TimerTicks_t maxUiUpdateIntervalInMsec; // TimerTicks_t
} UiHeartbeatTickMonitorConfiguration_t;

typedef struct
{
   struct
   {
      const UiHeartbeatTickMonitorConfiguration_t *config;
      I_DataModel_t *dataModel;
      EventSubscription_t uiHeartbeatChangedSubscription;
      Timer_t communicationTimeoutTimer;
   } _private;
} UiHeartbeatTickMonitor_t;

/*!
 * @param instance
 * @param config
 * @param dataModel
 */
void UiHeartbeatTickMonitor_Init(
   UiHeartbeatTickMonitor_t *instance,
   const UiHeartbeatTickMonitorConfiguration_t *config,
   I_DataModel_t *dataModel);
#endif
