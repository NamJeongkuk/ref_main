/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "TinyGeaStack.h"
#include "Gea2Addresses.h"
#include "TinySystemErds.h"
#include "utils.h"

enum
{
   Gea2CommonCommand_Version = 0x01,
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

static void PopulateVersionResponse(void *context, Gea2Packet_t *packet)
{
   REINTERPRET(sourcePacket, context, const Gea2Packet_t *);
   packet->destination = sourcePacket->source;
   packet->payload[0] = Gea2CommonCommand_Version;
   packet->payload[1] = 0x01;
   packet->payload[2] = 0x02;
   packet->payload[3] = 0x03;
   packet->payload[4] = 0x04;
}

static void HandleVersionRequest(TinyGeaStack_t *instance, const Gea2Packet_t *request)
{
   TinyGea2Interface_Send(&instance->_private.gea2Interface.interface, 5, PopulateVersionResponse, request);
}

static void GeaMessageReceived(void *context, const void *_args)
{
   REINTERPRET(instance, context, TinyGeaStack_t *);
   REINTERPRET(args, _args, const TinyGea2InterfaceOnReceiveArgs_t *);
   const Gea2Packet_t *packet = args->packet;
   uint16_t command = packet->payload[0];

   switch(command)
   {
      case Gea2CommonCommand_Version:
         HandleVersionRequest(instance, packet);
         break;
   }
}

void TinyGeaStack_Init(
   TinyGeaStack_t *instance,
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
      sizeof(instance->_private.receiveBuffer));

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

   TinyEventSubscription_Init(&instance->_private.geaMessageSubscription, instance, GeaMessageReceived);
   TinyEvent_Subscribe(
      TinyGea2Interface_GetOnReceiveEvent(&instance->_private.gea2Interface.interface),
      &instance->_private.geaMessageSubscription);
}

I_TinyGea2Interface_t *TinyGeaStack_GetGea2Interface(TinyGeaStack_t *instance)
{
   return &instance->_private.gea2Interface.interface;
}

void TinyGeaStack_Run(TinyGeaStack_t *instance)
{
   TinyGea2Interface_FullDuplex_Run(&instance->_private.gea2Interface);
}
