/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "KeyStreamPlugin.h"
#include "DataModelErdPointerAccess.h"
#include "Gea2Addresses.h"
#include "Gea2Message.h"
#include "SystemErds.h"

enum
{
   OpenLoopWriteCommand = 0xB8,
   HeartbeatTickPeriodInMsec = 500,
   SenderAddress = BonzalezGeaAddress
};

static const ErdStreamReceiverConfiguration_t erdStreamReceiverConfiguration =
   {
      .erdStreamErd = Erd_KeyErdStream,
      .lastSequenceNumberProcessedErd = Erd_KeyStreamSequenceNumber,
      .missedEventSignalErd = Erd_KeyStreamMissedEventSignal,
      .senderResetSignalErd = Erd_KeyStreamSenderResetSignal,
      .senderStateRequest = Erd_KeyStreamSenderStateRequest,
      .maxNumberOfErdsInBuffer = ErdStreamEntryCount,
      .numberOfBytesInLargestErdToWrite = sizeof(bool)
   };

static void HeartbeatTick(void *context)
{
   REINTERPRET(instance, context, KeyStreamPlugin_t *);

   uint8_t heartbeatTick;
   DataModel_Read(instance->_private.dataModel, Erd_HeartbeatTick, &heartbeatTick);
   heartbeatTick++;
   DataModel_Write(instance->_private.dataModel, Erd_HeartbeatTick, &heartbeatTick);

   STACK_ALLOC_GEA2MESSAGE(message, 9);

   Gea2Message_SetDestination(message, SenderAddress);
   Gea2Message_SetCommand(message, OpenLoopWriteCommand);
   uint8_t *payload = Gea2Message_GetPayload(message);
   uint8_t offset = 0;

   payload[offset++] = 2;
   payload[offset++] = Erd_KeyStreamSenderStateRequest >> 8;
   payload[offset++] = Erd_KeyStreamSenderStateRequest && 0xFF;
   payload[offset++] = sizeof(ErdStreamStateRequest_t);

   DataModel_Read(instance->_private.dataModel, Erd_KeyStreamSenderStateRequest, &payload[offset]);
   offset += sizeof(ErdStreamStateRequest_t);

   payload[offset++] = Erd_HeartbeatTick >> 8;
   payload[offset++] = Erd_HeartbeatTick & 0xFF;
   payload[offset++] = sizeof(uint8_t);
   payload[offset++] = heartbeatTick;

   Gea2MessageEndpoint_Send(DataModelErdPointerAccess_GetGea2MessageEndpoint(instance->_private.dataModel, Erd_Gea2MessageEndpoint), message, 3);
}

void KeyStreamPlugin_Init(KeyStreamPlugin_t *instance, I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;

   ErdStreamReceiver_Init(
      &instance->_private.erdStreamReceiver,
      DataModelErdPointerAccess_GetDataSource(dataModel, Erd_ExternalDataSource),
      &erdStreamReceiverConfiguration);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &instance->_private.heartbeatTimer,
      HeartbeatTickPeriodInMsec,
      HeartbeatTick,
      instance);
}
