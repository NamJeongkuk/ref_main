/*!
 * @file
 * @brief Module that determines which boards have ever been a part of the system via GEA Address
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BOARDSINSYSTEM_H
#define BOARDSINSYSTEM_H

#include "I_DataModel.h"
#include "Timer.h"
#include "EventSubscription.h"
#include "I_Gea2MessageEndpoint.h"

typedef struct
{
   uint8_t geaAddress;
   Erd_t nvErd; // bool
   Erd_t faultErd; // bool
} BoardInSystemData_t;

typedef struct
{
   const BoardInSystemData_t *boardsInSystemData;
   uint8_t allBoardsInSystemNumberEntries;
   uint8_t initialDiscoveryDelayTimeInSec;
   uint8_t retryPeriodForDiscoveryInSec;
   uint8_t numberOfDiscoveryRetryRequests;
   uint8_t retryPeriodForMonitoringInMinutes;
   uint8_t numberOfMonitoringRetryRequestsBeforeSettingFault;
   Erd_t geaMessageEndpointErd; // I_Gea2MessageEndpoint_t *
   Erd_t timerModuleErd; // TimerModule_t *
} BoardsInSystemConfig_t;

typedef struct
{
   struct
   {
      Timer_t retryTimer;
      EventSubscription_t onReceiveEventSubscription;
      const BoardsInSystemConfig_t *config;
      uint8_t *boardMissedResponseCountBuffer;
      I_DataModel_t *dataModel;
      uint8_t numberOfRetries;
   } _private;
} BoardsInSystem_t;

/*!
 *
 * @pre sizeOfBuffer >= config.allBoardsInSystemNumberEntries
 * @param instance
 * @param dataModel
 * @param config
 * @param boardMissedResponseCountBuffer
 * @param sizeOfBuffer
 */
void BoardsInSystem_Init(
   BoardsInSystem_t *instance,
   I_DataModel_t *dataModel,
   const BoardsInSystemConfig_t *config,
   uint8_t *boardMissedResponseCountBuffer,
   uint8_t sizeOfBuffer);

#endif
