/*!
 * @file
 * @brief Foreground full duplex GEA communication stack
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TINYGEASTACK_H
#define TINYGEASTACK_H

#include "I_TinyUart.h"
#include "I_TinyDataSource.h"
#include "TinyGea2Interface_FullDuplex.h"
#include "TinyEventSubscription.h"
#include "TinyErdGea2OpenLoopWriteApiRevision2.h"
#include "TinyErdHeartbeat.h"

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
      TinyErdGea2OpenLoopWriteApiRevision2_t erdGea2OpenLoopWriteApi;
      TinyErdHeartbeat_t erdHeartbeat;
      TinyEventSubscription_t geaMessageSubscription;
      uint8_t sendBuffer[SendBufferSize];
      uint8_t receiveBuffer[ReceiveBufferSize];
   } _private;
} TinyGeaStack_t;

/*!
 * @param instance
 * @param uart
 * @param dataSource
 * @param timerModule
 * @param geaAddress
 */
void TinyGeaStack_Init(
   TinyGeaStack_t *instance,
   I_TinyUart_t *uart,
   I_TinyDataSource_t *dataSource,
   TinyTimerModule_t *timerModule,
   uint8_t geaAddress);

/*!
 * @param instance
 * @return I_Gea2Interface_t
 */
I_TinyGea2Interface_t *TinyGeaStack_GetGea2Interface(TinyGeaStack_t *instance);

/*!
 * @param instance
 */
void TinyGeaStack_Run(TinyGeaStack_t *instance);

#endif
