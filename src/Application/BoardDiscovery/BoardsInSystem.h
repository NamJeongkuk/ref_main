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
} GeaAddressToNvInSystemMap_t;

typedef struct
{
   const GeaAddressToNvInSystemMap_t *addressToErdMapArray;
   uint8_t addressToErdMapArrayNumberEntries;
   uint8_t retryPeriodInSec;
   uint8_t initialDelayTimeInSec;
   uint8_t numberOfRetryRequests;
   Erd_t geaMessageEndpointErd; // I_Gea2MessageEndpoint_t *
   Erd_t TimerModuleErd; // TimerModule_t *
} BoardsInSystemConfig_t;

typedef struct
{
   struct
   {
      Timer_t retryTimer;
      EventSubscription_t onReceiveEventSubscription;
      const BoardsInSystemConfig_t *config;
      I_DataModel_t *dataModel;
      uint8_t numberOfRetries;
   } _private;
} BoardsInSystem_t;

/*!
 *
 * @param instance
 * @param dataModel
 * @param config
 */
void BoardsInSystem_Init(
   BoardsInSystem_t *instance,
   I_DataModel_t *dataModel,
   const BoardsInSystemConfig_t *config);

#endif
