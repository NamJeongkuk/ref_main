/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "GeaStackWithSingleErdHeartbeat.h"
#include "Gea2Addresses.h"
#include "BootLoader.h"
#include "TinyBootLoaderCommand.h"
#include "Version.h"
#include "stm8_tsl_api.h"
#include "utils.h"

#define CAP_SENSE_PARAMETERS_RESPONSE_PAYLOAD_SIZE \
   sizeof(uint8_t) +                               \
      sizeof(TSL_GlobalParameters_t) +             \
      sizeof(uint8_t) +                            \
      (sizeof(TSL_KeyParameters_t) * NUMBER_OF_SINGLE_CHANNEL_KEYS)

enum
{
   SetCapSenseParametersRequest = 0xBB,
   GetCapSenseParametersRequest = 0xBC,
   GetCapSenseParametersResponse = 0xBD
};

extern Version_t version;

static void PopulateTinyVersionResponse(void *context, Gea2Packet_t *packet)
{
   REINTERPRET(sourcePacket, context, const Gea2Packet_t *);
   packet->payload[0] = TinyBootLoaderCommand_VersionResponse;
   packet->payload[1] = sourcePacket->payload[1];
   packet->payload[2] = version.criticalMajor;
   packet->payload[3] = version.criticalMinor;
   packet->payload[4] = version.major;
   packet->payload[5] = version.minor;
}

static void HandleTinyVersionRequest(GeaStackWithSingleErdHeartbeat_t *instance, const Gea2Packet_t *request)
{
   TinyGea2Interface_Send(&instance->_private.gea2Interface.interface, request->source, 6, PopulateTinyVersionResponse, (void *)(request));
}

static void PopulateCapSenseParametersResponse(void *context, Gea2Packet_t *packet)
{
   REINTERPRET(sourcePacket, context, const Gea2Packet_t *);
   packet->payload[0] = GetCapSenseParametersResponse;
   packet->payload[9] = NUMBER_OF_SINGLE_CHANNEL_KEYS;
   TSL_GetParameters((TSL_GlobalParameters_t *)&packet->payload[1], (TSL_KeyParameters_t *)&packet->payload[10]);
}

static void HandleGetCapSenseParametersRequest(GeaStackWithSingleErdHeartbeat_t *instance, const Gea2Packet_t *request)
{
   TinyGea2Interface_Send(
      &instance->_private.gea2Interface.interface,
      request->source,
      CAP_SENSE_PARAMETERS_RESPONSE_PAYLOAD_SIZE,
      PopulateCapSenseParametersResponse,
      (void *)request);
}

static void HandleSetCapSenseParametersRequest(GeaStackWithSingleErdHeartbeat_t *instance, const Gea2Packet_t *request)
{
   TSL_SetParameters(
      (const TSL_GlobalParameters_t *)&request->payload[1],
      request->payload[9],
      (const TSL_KeyParameters_t *)&request->payload[10]);

   HandleGetCapSenseParametersRequest(instance, request);
}

static void GeaMessageReceived(void *context, const void *_args)
{
   REINTERPRET(instance, context, GeaStackWithSingleErdHeartbeat_t *);
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

      case GetCapSenseParametersRequest:
         HandleGetCapSenseParametersRequest(instance, packet);
         break;

      case SetCapSenseParametersRequest:
         HandleSetCapSenseParametersRequest(instance, packet);
         break;
   }
}

void GeaStackWithSingleErdHeartbeat_Init(
   GeaStackWithSingleErdHeartbeat_t *instance,
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
      sizeof(instance->_private.receiveBuffer),
      false);

   TinySingleErdHeartbeatStream_Init(
      &instance->_private.gea2Interface.interface,
      timerModule);

   TinyEventSubscription_Init(&instance->_private.geaMessageSubscription, instance, GeaMessageReceived);
   TinyEvent_Subscribe(
      TinyGea2Interface_GetOnReceiveEvent(&instance->_private.gea2Interface.interface),
      &instance->_private.geaMessageSubscription);
}

void GeaStackWithSingleErdHeartbeat_Run(GeaStackWithSingleErdHeartbeat_t *instance)
{
   TinyGea2Interface_FullDuplex_Run(&instance->_private.gea2Interface);
}
