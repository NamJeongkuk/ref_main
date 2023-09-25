/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BROADCASTRESET_H
#define BROADCASTRESET_H

#include "I_DataModel.h"
#include "EventSubscription.h"
#include "I_Gea2MessageEndpoint.h"

typedef struct
{
   Erd_t broadcastResetRequestErd; // Signal_t
   Erd_t resetErd; // uint8_t
   Erd_t gea2MessageEndpointErd; // I_Gea2MessageEndpoint_t *
} BroadcastResetConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t broadcastResetRequestSubscription;
      const BroadcastResetConfig_t *config;
   } _private;
} BroadcastReset_t;

void BroadcastReset_Init(
   BroadcastReset_t *instance,
   I_DataModel_t *dataModel,
   const BroadcastResetConfig_t *config);

#endif
