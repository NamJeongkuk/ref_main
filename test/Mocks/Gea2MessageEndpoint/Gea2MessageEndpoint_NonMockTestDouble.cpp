/*!
 * @file
 * @brief Implementation of GEA2 message endpoint non mock test double.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "Gea2MessageEndpoint_NonMockTestDouble.h"
#include "CppUTestExt/MockSupport.h"
#include "utils.h"

static void Send(I_Gea2MessageEndpoint_t *_instance, Gea2Message_t *message, const uint8_t retries)
{
   IGNORE(_instance);
   IGNORE(message);
   IGNORE(retries);
}

static I_Event_t *GetOnReceiveEvent(I_Gea2MessageEndpoint_t *_instance)
{
   REINTERPRET(instance, _instance, Gea2MessageEndpoint_NonMockTestDouble_t *);
   return &instance->onReceiveEvent.interface;
}

static const I_Gea2MessageEndpoint_Api_t api = { Send, GetOnReceiveEvent };

void Gea2MessageEndpoint_NonMockTestDouble_Init(Gea2MessageEndpoint_NonMockTestDouble_t *instance, const uint8_t address)
{
   instance->interface.api = &api;

   instance->address = address;

   Event_Synchronous_Init(&instance->onReceiveEvent);
}

void Gea2MessageEndpoint_NonMockTestDouble_TriggerReceive(Gea2MessageEndpoint_NonMockTestDouble_t *instance,
   const Gea2Message_t *message)
{
   const Gea2MessageEndpointOnReceiveArgs_t args = { message };

   Event_Synchronous_Publish(&instance->onReceiveEvent, &args);
}
