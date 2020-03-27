/*!
 * @file
 * @brief Foreground full duplex GEA communication stack
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GEASTACKWITHSINGLEERDHEARTBEAT_H
#define GEASTACKWITHSINGLEERDHEARTBEAT_H

#include "I_TinyUart.h"
#include "I_TinyDataSource.h"
#include "TinyGea2Interface_FullDuplex.h"
#include "TinyEventSubscription.h"
#include "TinySingleErdHeartbeatStream.h"

enum
{
   SendBufferSize = 80,
   ReceiveBufferSize = 80
};

typedef struct
{
   struct
   {
      TinyGea2Interface_FullDuplex_t gea2Interface;
      TinyEventSubscription_t geaMessageSubscription;
      uint8_t sendBuffer[SendBufferSize];
      uint8_t receiveBuffer[ReceiveBufferSize];
   } _private;
} GeaStackWithSingleErdHeartbeat_t;

/*!
 * @param instance
 * @param uart
 * @param dataSource
 * @param timerModule
 * @param geaAddress
 */
void GeaStackWithSingleErdHeartbeat_Init(
   GeaStackWithSingleErdHeartbeat_t *instance,
   I_TinyUart_t *uart,
   TinyTimerModule_t *timerModule,
   uint8_t geaAddress);

/*!
 * @param instance
 */
void GeaStackWithSingleErdHeartbeat_Run(GeaStackWithSingleErdHeartbeat_t *instance);

#endif
