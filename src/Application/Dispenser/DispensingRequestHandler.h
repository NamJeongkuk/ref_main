/*!
 * @file
 * @brief Handles the dispensing request and ERDs and translates request to the state machine and updates status
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSINGREQUESTHANDLER_H
#define DISPENSINGREQUESTHANDLER_H

#include "DispensingRequest.h"
#include "EventSubscription.h"
#include "I_DataModel.h"
#include "Fsm.h"
#include "Timer.h"

enum
{
   DispensingRequestContinueWaitMaxTimeInMsec = 3000
};

typedef struct
{
   Erd_t dispensingRequestErd; // DispensingRequest_t
   Erd_t dispensingRequestStatusErd; // DispensingRequestStatus_t
   Erd_t privateDispensingRequestErd; // DispensingRequest_t
   Erd_t privateDispensingResultStatusErd; // DispenseStatus_t
} DispensingRequestHandlerConfig_t;

typedef struct
{
   struct
   {
      Fsm_t fsm;
      I_DataModel_t *dataModel;
      Timer_t dispensingRequestContinueWaitTimer;
      const DispensingRequestHandlerConfig_t *config;
      EventSubscription_t dataModelSubscription;
      DispensingRequest_t currentPrivateDispensingRequest;
   } _private;
} DispensingRequestHandler_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void DispensingRequestHandler_Init(
   DispensingRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const DispensingRequestHandlerConfig_t *config);

#endif
