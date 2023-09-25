/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BroadcastResetPlugin.h"
#include "SystemErds.h"

static const BroadcastResetConfig_t broadcastResetConfig = {
   .broadcastResetRequestErd = Erd_BroadcastResetRequestSignal,
   .gea2MessageEndpointErd = Erd_Gea2MessageEndpoint,
   .resetErd = Erd_Reset
};

void BroadcastResetPlugin_Init(
   BroadcastResetPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   BroadcastReset_Init(
      &instance->_private.broadcastReset,
      dataModel,
      &broadcastResetConfig);
}
