/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GEA2MESSAGEENDPOINT_NONMOCKTESTDOUBLE_H
#define GEA2MESSAGEENDPOINT_NONMOCKTESTDOUBLE_H

#include <stdbool.h>

extern "C"
{
#include "I_Gea2MessageEndpoint.h"
#include "Event_Synchronous.h"
}

typedef struct
{
   I_Gea2MessageEndpoint_t interface;
   Event_Synchronous_t onReceiveEvent;
   uint8_t address;
} Gea2MessageEndpoint_NonMockTestDouble_t;

void Gea2MessageEndpoint_NonMockTestDouble_Init(
   Gea2MessageEndpoint_NonMockTestDouble_t *instance,
   const uint8_t address);

void Gea2MessageEndpoint_NonMockTestDouble_TriggerReceive(
   Gea2MessageEndpoint_NonMockTestDouble_t *instance,
   const Gea2Message_t *message);

#endif
