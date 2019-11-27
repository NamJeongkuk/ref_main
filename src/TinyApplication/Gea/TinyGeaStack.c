/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "TinyGeaStack.h"
#include "Gea2Addresses.h"
#include "BootLoader.h"
#include "TinyBootLoaderCommand.h"
#include "Version.h"
#include "utils.h"

extern Version_t version;

enum
{
   Gea2CommonCommand_Version = 0x01,
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

static void HandleTinyVersionRequest(TinyGeaStack_t *instance, const Gea2Packet_t *request)
{
   TinyGea2Interface_Send(&instance->_private.gea2Interface.interface, 6, PopulateTinyVersionResponse, (void *)(request));
}

static void GeaMessageReceived(void *context, const void *_args)
{
   REINTERPRET(instance, context, TinyGeaStack_t *);
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

void TinyGeaStack_Init(
   TinyGeaStack_t *instance,
   I_TinyUart_t *uart,
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

   TinySingleErdHeartbeatStream_Init(
      &instance->_private.gea2Interface.interface,
      timerModule);

   TinyEventSubscription_Init(&instance->_private.geaMessageSubscription, instance, GeaMessageReceived);
   TinyEvent_Subscribe(
      TinyGea2Interface_GetOnReceiveEvent(&instance->_private.gea2Interface.interface),
      &instance->_private.geaMessageSubscription);
}

void TinyGeaStack_Run(TinyGeaStack_t *instance)
{
   TinyGea2Interface_FullDuplex_Run(&instance->_private.gea2Interface);
}
