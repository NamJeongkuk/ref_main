/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "GeaStack.h"
#include "Gea2Addresses.h"
#include "NanoSystemErds.h"
#include "BootLoader.h"
#include "TinyBootLoaderCommand.h"
#include "utils.h"

extern Version_t version;

enum
{
   HeartbeatPeriodMsec = 100,
   RemoteErdStreamErd = 0xF123
};

static const TinyErdHeartbeatErdPair_t erdHeartbeatPairs[] =
   {
      { Erd_ErdStream, RemoteErdStreamErd }
   };

static const TinyErdHeartbeatConfiguration_t erdHeartbeatConfig =
   {
      .destination = TrukGeaAddress,
      .period = HeartbeatPeriodMsec,
      .pairs = erdHeartbeatPairs,
      .pairCount = NUM_ELEMENTS(erdHeartbeatPairs)
   };

#define EXPAND_AS_LOCAL_TO_REMOTE_ERD_MAP(Name, Number, DataType, Stream, RemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)                                                   \
   ({ Number COMMA RemoteErd COMMA CONCAT(INCLUDE_STREAM_EVENT_, Stream)(ErdStreamDataType_Event) CONCAT(INCLUDE_STREAM_LEVEL_, Stream)(ErdStreamDataType_Level) } COMMA)

static const ErdStreamLocalToRemoteErdMap_t streamLocalToRemoteErdMap[] =
   {
      ERD_TABLE(EXPAND_AS_LOCAL_TO_REMOTE_ERD_MAP)
   };

static const TinyErdStreamSenderConfiguration_t erdStreamSenderConfiguration =
   {
      .erdStreamErd = Erd_ErdStream,
      .requestedStateErdFromReceiver = Erd_ErdStreamRequestedState,
      .streamEntryCount = NumberOfStreamedErds,
      .sizeOfLargestStreamedErd = sizeof(StreamedErd_t),
      .mappings = streamLocalToRemoteErdMap,
      .mappingCount = NUM_ELEMENTS(streamLocalToRemoteErdMap)
   };

static void PopulateTinyVersionResponse(void *context, Gea2Packet_t *packet)
{
   REINTERPRET(sourcePacket, context, const Gea2Packet_t *);
   packet->destination = sourcePacket->source;
   packet->payload[0] = TinyBootLoaderCommand_VersionResponse;
   packet->payload[1] = sourcePacket->payload[1];
   packet->payload[2] = version.criticalMajor;
   packet->payload[3] = version.criticalMinor;
   packet->payload[4] = version.major;
   packet->payload[5] = version.minor;
}

static void HandleTinyVersionRequest(GeaStack_t *instance, const Gea2Packet_t *request)
{
   TinyGea2Interface_Send(&instance->_private.gea2Interface.interface, 6, PopulateTinyVersionResponse, (void *)(request));
}

static void GeaMessageReceived(void *context, const void *_args)
{
   REINTERPRET(instance, context, GeaStack_t *);
   REINTERPRET(args, _args, const TinyGea2InterfaceOnReceiveArgs_t *);
   const Gea2Packet_t *packet = args->packet;
   uint16_t command = packet->payload[0];

   switch(command)
   {
      case TinyBootLoaderCommand_VersionRequest:
         HandleTinyVersionRequest(instance, packet);
         break;

      case TinyBootLoaderCommand_JumpToBootLoader:
         BootLoader_JumpToBootLoader();
         break;
   }
}

void GeaStack_Init(
   GeaStack_t *instance,
   I_TinyUart_t *uart,
   I_TinyDataSource_t *dataSource,
   TinyTimerModule_t *timerModule,
   uint8_t geaAddress)
{
   TinyGea2Interface_FullDuplex_Init(
      &instance->_private.gea2Interface,
      uart,
      geaAddress,
      instance->_private.sendBuffer,
      sizeof(instance->_private.sendBuffer),
      instance->_private.receiveBuffer,
      sizeof(instance->_private.receiveBuffer),
      false);

   TinyErdGea2OpenLoopWriteApiRevision2_Init(
      &instance->_private.erdGea2OpenLoopWriteApi,
      dataSource,
      &instance->_private.gea2Interface.interface);

   TinyErdHeartbeat_Init(
      &instance->_private.erdHeartbeat,
      &instance->_private.gea2Interface.interface,
      dataSource,
      timerModule,
      &erdHeartbeatConfig);

   TinyErdStreamSender_Init(
      &instance->_private.erdStreamSender,
      dataSource,
      &erdStreamSenderConfiguration);

   TinyEventSubscription_Init(&instance->_private.geaMessageSubscription, instance, GeaMessageReceived);
   TinyEvent_Subscribe(
      TinyGea2Interface_GetOnReceiveEvent(&instance->_private.gea2Interface.interface),
      &instance->_private.geaMessageSubscription);
}

I_TinyGea2Interface_t *GeaStack_GetGea2Interface(GeaStack_t *instance)
{
   return &instance->_private.gea2Interface.interface;
}

void GeaStack_Run(GeaStack_t *instance)
{
   TinyGea2Interface_FullDuplex_Run(&instance->_private.gea2Interface);
}
