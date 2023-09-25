/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BroadcastReset.h"
#include "DataModelErdPointerAccess.h"
#include "Gea2Addresses.h"

enum
{
   ResetBoardCommand = 0xDD,
   ResetBoardData = 0xF401,
   NumberOfRetries = 3,
   SelfResetDelayInSeconds = 5
};

static void ResetMainBoardAfterDelay(BroadcastReset_t *instance, uint8_t delayInSeconds)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->resetErd,
      &delayInSeconds);
}

static void BroadcastResetRequestValueChanged(void *context, const void *_args)
{
   BroadcastReset_t *instance = context;
   IGNORE(_args);

   STACK_ALLOC_GEA2MESSAGE(message, 2);
   uint8_t *payload = Gea2Message_GetPayload(message);

   Gea2Message_SetCommand(message, ResetBoardCommand);

   payload[0] = ResetBoardData >> 8;
   payload[1] = ResetBoardData & 0xFF;

   Gea2Message_SetDestination(message, Gea2Address_AllBoards);
   Gea2Message_SetSource(message, Gea2Address_Mainboard);

   Gea2MessageEndpoint_Send(
      DataModelErdPointerAccess_GetGea2MessageEndpoint(
         instance->_private.dataModel,
         instance->_private.config->gea2MessageEndpointErd),
      message,
      NumberOfRetries);

   ResetMainBoardAfterDelay(instance, SelfResetDelayInSeconds);
}

void BroadcastReset_Init(
   BroadcastReset_t *instance,
   I_DataModel_t *dataModel,
   const BroadcastResetConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   EventSubscription_Init(
      &instance->_private.broadcastResetRequestSubscription,
      instance,
      BroadcastResetRequestValueChanged);

   DataModel_Subscribe(
      dataModel,
      instance->_private.config->broadcastResetRequestErd,
      &instance->_private.broadcastResetRequestSubscription);
}
