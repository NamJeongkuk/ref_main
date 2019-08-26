/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TinyGeaStack.h"
#include "uassert.h"
#include "Gea2Addresses.h"
#include "utils.h"

enum
{
   Gea2CommonCommand_Version = 0x0001
};

static void HandleVersionRequest(TinyGeaStack_t *instance, const Gea2Packet_t *request)
{
   uint8_t responseBuffer[10];
   Gea2Packet_t *response = (Gea2Packet_t *)responseBuffer;
   uint8_t *responsePayload = response->payload;

   response->payloadLength = 5;
   response->destination = request->source;

   *responsePayload++ = Gea2CommonCommand_Version;
   *responsePayload++ = 0x01;
   *responsePayload++ = 0x02;
   *responsePayload++ = 0x03;
   *responsePayload++ = 0x04;

   TinyGea2Interface_Send(&instance->_private.gea2Interface.interface, response);
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

void TinyGeaStack_Init(TinyGeaStack_t *instance, I_TinyUart_t *uart, uint8_t geaAddress)
{
   instance->_private.geaAddress = geaAddress;

   TinyGea2Interface_FullDuplex_Init(
      &instance->_private.gea2Interface,
      uart,
      instance->_private.geaAddress,
      instance->_private.sendBuffer,
      sizeof(instance->_private.sendBuffer),
      instance->_private.receiveBuffer,
      sizeof(instance->_private.receiveBuffer));

   TinyEventSubscription_Init(&instance->_private.geaMessageSubscription, instance, GeaMessageReceived);
   TinyEvent_Subscribe(
      TinyGea2Interface_GetOnReceiveEvent(&instance->_private.gea2Interface.interface),
      &instance->_private.geaMessageSubscription);
}

I_TinyGea2Interface_t * TinyGeaStack_GetGea2Interface(TinyGeaStack_t *instance)
{
   return &instance->_private.gea2Interface.interface;
}

void TinyGeaStack_Run(TinyGeaStack_t *instance)
{
   TinyGea2Interface_FullDuplex_Run(&instance->_private.gea2Interface);
}
